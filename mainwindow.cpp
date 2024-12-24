#include <QApplication>
#include <QTextEdit>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDir>
#include <QFileDialog>
#include <QTextCodec>
#include <QMessageBox>
#include <QWheelEvent>
#include <QStringList>
#include <QSettings>
#include <QMimeData>

#include "configdialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_finddialog.h"
#include "ui_configdialog.h"
#include "model.h"
#include "comboboxdialog.h"
#include "versiondialog.h"

QTextEdit *debugout;

void ModifiedCB(EditorEngine *sender, void* dat);



TabBar::TabBar(QWidget *parent)
    :QTabBar(parent)
{

}

QRect TabBar::tabRect(int index) const
{
    return QTabBar::tabRect(index);
}

QSize TabBar::tabSizeHint(int index) const
{
    QSize size = QTabBar::tabSizeHint(index);
    if(size.width()<80)
        size.setWidth(80);
    return size;
}



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabbar->setExpanding(false);    //これをやらないとtabSizeHintがきかない
    //ui->tabbar->setStyleSheet("QTabBar::tab {min-width: 80px;}");


    //ツールバーとアイコン
    //OSがアイコンテーマを提供しているときはそれを使う
    //さもなくばリソースアイコンを使う
    ui->mainToolBar->setIconSize(QSize(16,16));
    ui->mainToolBar->setMovable(false);
    QString iconnames[]={"document-new","document-open","document-save","|",
                        "edit-cut","edit-copy","edit-paste","|",
                         "edit-undo","edit-redo",""};
    QAction *actions[]={ui->actionNew_File, ui->actionOpen, ui->actionSave, NULL,
                        ui->actionCut, ui->actionCopy, ui->actionPaste, NULL,
                        ui->actionUndo, ui->actionRedo, NULL};
    for(int i=0; iconnames[i]!=""; i++){
        if(iconnames[i]=="|"){
            ui->mainToolBar->addSeparator();
        }
        else{
            QIcon icon;
            if(QIcon::hasThemeIcon(iconnames[i]))
                icon = QIcon::fromTheme(iconnames[i]);
            else
                icon = QIcon(QString(":/resources/icons/%1").arg(iconnames[i]));
            actions[i]->setIcon(icon);
            ui->mainToolBar->addAction(actions[i]);
        }
    }



    //エディタ
    editorview = ui->editorview;
    editorview->setScrollBars(ui->scrEditorViewV, ui->scrEditorViewH);
    debugout = ui->debugedit;
    editorview->setFocus();

    //その他インスタンス
    dlgfind = new FindDialog(this);
    popupmenu = new QMenu(editorview);

    //タイトル
    QString windowtitle = tr("tr_title");
#ifdef QT_DEBUG
    windowtitle += "debug";
#endif
    setWindowTitle(windowtitle);

    //シグナル/スロット、コールバック接続
    //これ以外に関数名からmocにより自動で接続されるスロットもあることに注意
    connect(ui->menuBar, SIGNAL(triggered(QAction*)), SLOT(onTriggerMenu(QAction*)));
    connect(ui->tabbar, SIGNAL(currentChanged(int)), SLOT(onCurrentChanged(int)));
    connect(dlgfind->ui->btnfindnext, SIGNAL(clicked()), SLOT(findNext()));
    connect(dlgfind->ui->btnfindprevious, SIGNAL(clicked()), SLOT(findPrevious()));
    connect(dlgfind->ui->btnreplace, SIGNAL(clicked()), SLOT(replace()));
    connect(dlgfind->ui->btnreplaceall, SIGNAL(clicked()), SLOT(replaceAll()));
    EditorEngine::ConnectModifiedCallback((void*)ModifiedCB, (void*)this);
    connect(editorview, SIGNAL(entered(QEvent*)), SLOT(onEditorViewEntered(QEvent*)));
    connect(editorview, SIGNAL(dropped(QDropEvent*)), SLOT(onEditorViewDropped(QDropEvent*)));

    //設定ファイル
    int size;
#ifdef _WIN32
    QString settingsdir = QApplication::applicationDirPath();
#else
    QString settingsdir = QDir(getenv("HOME")).filePath(".config");
#endif
    settingspath = QDir(settingsdir).filePath("kaihitsu.ini");
    cwtablepath = QDir(settingsdir).filePath("kaihitsucw.bin");

    //設定読み込み
    QSettings settings(settingspath, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("UTF-8"));

    settings.beginGroup("window");
    setGeometry( settings.value("geometry", QRect(100,100,400,400)).toRect() );
    dlgfind->savedgeom_find = settings.value("dlgfind.geometry.find", QRect(0,0,0,0)).toRect();
    dlgfind->savedgeom_replace = settings.value("dlgfind.geometry.replace", QRect(0,0,0,0)).toRect();
    dlgconfigrect =  settings.value("dlgconfig.geometry", QRect(0,0,0,0)).toRect();
    dlgconfig_attrcolwidths =
            settings.value("dlgconfig.attrcolwidths", "70,50,60,70,70,25,25,25").toString();
    settings.endGroup();

    settings.beginGroup("menu");
    setTextEncodingMenu(ui->actionTextencoding_input, tr("tr_input"),
                        settings.value("textencoding.input.codec",AUTOSIGNATURE).toString(),
                        settings.value("textencoding.input.lineend",AUTOSIGNATURE).toString() );
    setTextEncodingMenu(ui->actionTextencoding_output, tr("tr_output"),
                        settings.value("textencoding.output.codec",AUTOSIGNATURE).toString(),
                        settings.value("textencoding.output.lineend",AUTOSIGNATURE).toString() );
    ui->actionShowLinenumber->setChecked(settings.value("linenumber.show",true).toBool());
    ui->actionShowRuler->setChecked(settings.value("ruler.show",true).toBool());
    ui->actionShowToolbar->setChecked(settings.value("toolbar.show",true).toBool());
    ui->actionShowStatusbar->setChecked(settings.value("statusbar.show",true).toBool());
    settings.endGroup();

    ui->menuRecent->clear(); //ダミーメニューを消す
    size = settings.beginReadArray("filehistory");
    for(int i=0; i<size; i++){
        FileHistoryInfo histinfo;
        settings.setArrayIndex(i);
        histinfo.path = settings.value("path","").toString();
        histinfo.scrollvalue = settings.value("scrollvalue",0).toInt();
        filehistory.histinfos.append(histinfo);
    }
    updateMenuRecent();
    settings.endArray();


    defconf = createDefaultConfig();

    //共通設定
    settings.beginGroup("common");
    conf.fontsize   = settings.value("fontsize",defconf.fontsize).toInt();
    conf.lineheight = settings.value("lineheight",defconf.lineheight).toInt();
    conf.fontfamily = settings.value("fontfamily",defconf.fontfamily).toString();
    conf.maxfilesizemb = settings.value("maxfilesize_mb",defconf.maxfilesizemb).toInt();
    conf.warningfilesizemb = settings.value("warningfilesize_mb",defconf.warningfilesizemb).toInt();
    conf.scrollsize = settings.value("scrollsize",defconf.scrollsize).toInt();
    conf.maxrecent = settings.value("maxrecent",defconf.maxrecent).toInt();

    lastdir = settings.value("lastdir","").toString();
    settings.endGroup();

    //ファイルタイプ別設定
    size = settings.beginReadArray("filetypes");
    for(int i=0; i<size; i++){
        FileTypeConfig filetype;
        settings.setArrayIndex(i);
        filetype.name = settings.value("name",tr("item%1").arg(i)).toString();
        filetype.exts = settings.value("extensions","").toStringList();
        filetype.tabsize = settings.value("tabsize",8).toInt();
        filetype.autowrap = settings.value("autowrap",false).toBool();
        filetype.wrapsize = settings.value("wrapsize",80).toInt();
        filetype.autoindent = settings.value("autoindent",false).toBool();
        filetype.focusoutsave = settings.value("focusoutsave",false).toBool();

        int attrsize = settings.beginReadArray("attributes");
        for(int j=0; j<attrsize; j++){
            EditorAttribute attr;
            QColor color;
            QString str;
            settings.setArrayIndex(j);
            attr.id = settings.value("id",ATTR_NULLID).toInt();
            attr.name = settings.value("name",tr("item%1").arg(j)).toString().toStdWString();
            attr.type = settings.value("type",ATTR_NULL).toInt();
            attr.words =
                split(settings.value("words","").toStringList().join(",").toStdWString(),L',');
            if(attr.words.size()==1 && attr.words[0].length()==0)
                attr.words.clear();
            attr.regexp = settings.value("regexp","(?!)").toString().toStdWString();
            attr.stsym = settings.value("startsymbol","").toString().toStdWString();
            attr.edsym = settings.value("endsymbol","").toString().toStdWString();
            str = settings.value("forecolor","black").toString();
            if(str==tr("%1").arg(TRANSPARENT_COLOR)){
                attr.r = attr.g = attr.b = TRANSPARENT_COLOR;
            }
            else{
                color = QColor(str);
                attr.r = color.red();
                attr.g = color.green();
                attr.b = color.blue();
            }
            str = settings.value("backcolor","white").toString();
            if(str==tr("%1").arg(TRANSPARENT_COLOR)){
                attr.br = attr.bg = attr.bb = TRANSPARENT_COLOR;
            }
            else{
                color = QColor(str);
                attr.br = color.red();
                attr.bg = color.green();
                attr.bb = color.blue();
            }
            attr.bold = settings.value("bold").toBool();
            attr.italic = settings.value("italic").toBool();
            attr.underline = settings.value("underline").toBool();
            attr.layerlevel = settings.value("layerlevel").toInt();
            filetype.attrset.push_back(attr);
        }
        settings.endArray();
        conf.filetypes.append(filetype);
    }
    settings.endArray();

    if(conf.filetypes.size()==0){
        conf.filetypes = defconf.filetypes;
    }

    //設定適用
    applyConfig();

    QFont debugfont("MS Gothic");
    debugfont.setFixedPitch(true);
    debugout->setFont(debugfont);
    debugout->hide();


    fileNew();
    //activeModel()->InsertFast(L"He/**/llo my e\nditor!aa/*aaaaaaaaaaa\nbb*/bbb\nc*/cabcccccc\nccc");
    //  activeModel()->InsertFast(L"e/*ddd*/b\naa/*aaaaaaaaaaab*/ccc");
    //activeModel()->InsertFast(L"a/*ddd*/b\nc/*ccc*/c");
    //activeModel()->InsertFast(L"a\naabcc\nc");
    //activeModel()->InsertFast(L"abc\ndef");
    //activeModel()->InsertFast(L"aaa\nbbbbbb");
    //activeModel()->InsertFast(L"cccaabccc\ncabcccABCabce");
    //activeModel()->InsertFast(L"ab\naabccccaBc\ncccABCabce");
    //activeModel()->InsertFast(L"aahttpaa\"\"bb\nb\"\"cc");
    //activeModel()->InsertFast(L"aaaa\n\"\"bb\"d\n\"\"cc");
    //activeModel()->InsertFast(L"xabcy\nxxabcyyabczz");
    //activeModel()->InsertFast(L"xabcy\n01234567890123456789abc\n０１２３４５６７８９０１２３４abcxxx");
    //debugout->show();

}

void MainWindow::firstShowEvent()
{
#ifdef QT_DEBUG
    debugout->show();
    openFile(":/resources/debug.txt");
#endif

    //コマンドライン引数
    //最初の要素は実行ファイルのパス、次からが引数
    QStringList args = qApp->arguments();
    for(int i=1; i<args.length(); i++){
        openFile(args[i]);
    }

    if(ui->tabbar->count()==0){
        fileNew();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

ConfigData MainWindow::createDefaultConfig()
{
    /*この関数はコンフィグをファイル化し、リソースに置き換える予定*/

    ConfigData confdata;

    confdata.fontsize   = 16;
    confdata.lineheight = 18;
    confdata.fontfamily = qApp->font().family();
    confdata.maxfilesizemb = 100;
    confdata.warningfilesizemb = 30;
    confdata.scrollsize = 5;
    confdata.maxrecent = 20;

    //属性
    EditorAttributeSet *attribset;
    std::vector<std::wstring> dummy;

    //設定不履行時の属性
    //この属性は消すことができず、個別のファイルタイプはこの属性にオーバーライトで設定される
    FileTypeConfig defft;
    defft.name="Default";
    defft.tabsize = 8;
    defft.autowrap=true;
    defft.wrapsize=0;
    confdata.filetypes.append(defft);
    attribset = &confdata.filetypes[0].attrset;
    attribset->push_back(EditorAttribute(
            ATTR_TEXT,      tr("tr_attr_text").toStdWString(),  ATTR_NULL,
            0,0,0,  -1,-1,-1,   0,0,0, dummy, 0) );
    attribset->push_back(EditorAttribute(
            ATTR_CURSOR,    tr("tr_attr_cursor").toStdWString(), ATTR_NULL,
            192,0,0,  -1,-1,-1,  0,0,0, dummy, 0) );
    attribset->push_back(EditorAttribute(
            ATTR_SELECTION, tr("tr_attr_selection").toStdWString(), ATTR_NULL,
            -1,-1,-1,  128,192,255,  0,0,0, dummy, 999) );
    attribset->push_back(EditorAttribute(
            ATTR_FIND,      tr("tr_attr_find").toStdWString(),ATTR_NULL,
            0,0,0,  255,192,128,  0,0,0, dummy, 999) );
    attribset->push_back(EditorAttribute(
            ATTR_BACKGROUND,tr("tr_attr_background").toStdWString(),ATTR_NULL,
            251,253,255, -1,-1,-1,  0,0,0, dummy, 0) );
    attribset->push_back(EditorAttribute(
            ATTR_IMPREEDIT, tr("tr_attr_impreedit").toStdWString(), ATTR_NULL,
            0,0,0,  255,255,255,   0,0,0, dummy, 0) );
    attribset->push_back(EditorAttribute(
            ATTR_IMCOMMIT,  tr("tr_attr_imcommit").toStdWString(),ATTR_NULL,
            255,255,255,  0,0,0, 0,0,0, dummy, 0) );
    attribset->push_back(EditorAttribute(
            ATTR_LINENO,    tr("tr_attr_lineno").toStdWString(),ATTR_NULL,
            128,64,192,  255,246,251,  0,0,0, dummy, 0) );
    attribset->push_back(EditorAttribute(
            ATTR_RULER,     tr("tr_attr_ruler").toStdWString(), ATTR_NULL,
            0,0,0,  255,240,216,    0,0,0, dummy, 0) );

    //テスト用属性
    FileTypeConfig ft;
    ft.name="Test";
    ft.exts << "txt" << "csv";
    ft.tabsize = 8;
    ft.autowrap=true;
    ft.wrapsize=0;
    confdata.filetypes.append(ft);

    int attrid = ATTR_USER;
    attribset = &confdata.filetypes[1].attrset;
    std::vector<std::wstring> words;

    words.clear();
    words.push_back(L"abc");
    attribset->push_back(EditorAttribute(
            attrid++, L"abc",ATTR_WORD,0,128,255,1,1,1,0,0,0,words,10));
    words.clear();
    words.push_back(L"/*");
    words.push_back(L"*/");
    attribset->push_back(EditorAttribute(
            attrid++, L"ccom",ATTR_MULTILINE, 255,0,0,    TRANSPARENT_COLOR,-1,-1,0,0,0,words,11));
    words.clear();
    words.push_back(L"\"\"");
    words.push_back(L"\"\"");
    attribset->push_back(EditorAttribute(
            attrid++, L"string",ATTR_RANGE, 255,0,0,    TRANSPARENT_COLOR,-1,-1,0,0,0,words,13));
    words.clear();
    words.push_back(L"[a-z]+://[\\w/:%#\\$&\\?\\(\\)~\\.=\\+\\-]+");
    //words.push_back(L"http..");
    attribset->push_back(EditorAttribute(
            attrid++, L"url",ATTR_REGEXP,0,128,255, TRANSPARENT_COLOR,-1,-1,0,0,0,words,12));

    //Python
    ft = FileTypeConfig();
    ft.name="Python";
    ft.exts << "py";
    ft.tabsize = 4;
    ft.autowrap=true;
    ft.wrapsize=0;
    confdata.filetypes.append(ft);

    attrid = ATTR_USER;
    attribset = &confdata.filetypes[2].attrset;

    words.clear();
    words.push_back(L"#");
    words.push_back(L"\\n");
    attribset->push_back(EditorAttribute(
            attrid++, L"comment1",ATTR_RANGE,0,128,0,1,1,1,-1,-1,-1,words,11));
    words.clear();
    words.push_back(L"\"\"\"");
    words.push_back(L"\"\"\"");
    attribset->push_back(EditorAttribute(
            attrid++, L"comment2",ATTR_MULTILINE,0,128,0,1,1,1,-1,-1,-1,words,11));
    words.clear();
    words.push_back(L"\"");
    words.push_back(L"\"");
    attribset->push_back(EditorAttribute(
            attrid++, L"quote1",ATTR_RANGE,192,96,0,1,1,1,-1,-1,-1,words,12));
    words.clear();
    words.push_back(L"'");
    words.push_back(L"'");
    attribset->push_back(EditorAttribute(
            attrid++, L"quote2",ATTR_RANGE,192,96,0,1,1,1,-1,-1,-1,words,13));
    words.clear();
    QStringList qwords = QString(
            "and,as,assert,break,class,continue,def,del,elif,else,except,exec,"
            "finally,for,from,global,if,import,in,is,lambda,not,or,pass,print,raise,"
            "return,try,while,with,yield").split(",");
    for(QStringList::iterator it=qwords.begin(); it!=qwords.end(); it++)
        words.push_back(it->toStdWString());
    attribset->push_back(EditorAttribute(
            attrid++, L"yoyaku",ATTR_WORD,0,128,255,1,1,1,-1,-1,-1,words,14));

    return confdata;
}


void ModifiedCB(EditorEngine *sender, void* dat)
{
    ((MainWindow*)dat)->onModelModified(sender);
}
void MainWindow::onModelModified(EditorEngine *sender)
{
    for(int i=0; i<ui->tabbar->count(); i++){
        EditorEngine *e = ui->tabbar->tabData(i).value<Model*>();
        if(e==sender){
            QString tabtext = ui->tabbar->tabText(i);
            if(sender->IsModified()){
                tabtext = tr("*")+tabtext;
            }
            else{
                if(tabtext.at(0)=='*'){
                    tabtext = tabtext.remove(0,1);
                }
            }
            ui->tabbar->setTabText(i, tabtext);
            break;
        }
    }
}


void MainWindow::readSettings()
{


}

void MainWindow::writeSettings()
{
    QSettings settings(settingspath, QSettings::IniFormat);
    QStringList strlist;

    settings.beginGroup("window");
    settings.setValue("geometry",geometry());
    settings.setValue("dlgfind.geometry.find",dlgfind->savedgeom_find);
    settings.setValue("dlgfind.geometry.replace",dlgfind->savedgeom_replace);
    settings.setValue("dlgconfig.geometry", dlgconfigrect);
    settings.setValue("dlgconfig.attrcolwidths", dlgconfig_attrcolwidths);
    settings.endGroup();

    settings.beginGroup("menu");
    strlist = ui->actionTextencoding_input->data().toStringList();
    settings.setValue("textencoding.input.codec",strlist.at(0));
    settings.setValue("textencoding.input.lineend",strlist.at(1));
    strlist = ui->actionTextencoding_output->data().toStringList();
    settings.setValue("textencoding.output.codec",strlist.at(0));
    settings.setValue("textencoding.output.lineend",strlist.at(1));
    settings.endGroup();

    settings.beginWriteArray("filehistory");
    for(int i=0; i<filehistory.histinfos.size(); i++){
        FileHistoryInfo histinfo = filehistory.histinfos[i];
        settings.setArrayIndex(i);
        settings.setValue("path", histinfo.path);
        settings.setValue("scrollvalue", histinfo.scrollvalue);
    }
    settings.endArray();

    settings.beginGroup("common");
    //settings.setValue("",conf);
    settings.setValue("fontsize",conf.fontsize);
    settings.setValue("lineheight",conf.lineheight);
    settings.setValue("fontfamily",conf.fontfamily);
    settings.setValue("warningfilesize_mb",conf.warningfilesizemb);
    settings.setValue("maxfilesize_mb",conf.maxfilesizemb);
    settings.setValue("scrollsize",conf.scrollsize);
    settings.setValue("lastdir",lastdir);
    settings.endGroup();

    settings.beginWriteArray("filetypes");
    for (int i = 0; i < conf.filetypes.size(); ++i) {
        settings.setArrayIndex(i);
        FileTypeConfig filetype = conf.filetypes[i];
        settings.setValue("name", filetype.name);
        settings.setValue("extensions", filetype.exts);
        settings.setValue("tabsize", filetype.tabsize);
        settings.setValue("autowrap", filetype.autowrap);
        settings.setValue("wrapsize", filetype.wrapsize);
        settings.setValue("autoindent", filetype.autoindent);
        settings.setValue("focusoutsave", filetype.focusoutsave);
        settings.beginWriteArray("attributes");
        for(int j=0; j<filetype.attrset.size(); j++){
            settings.setArrayIndex(j);
            EditorAttribute *attr = &filetype.attrset.at(j);
            settings.setValue("id", attr->id);
            settings.setValue("name", QString::fromStdWString(attr->name));
            settings.setValue("type", attr->type);
            settings.setValue("startsymbol", QString::fromStdWString(attr->stsym));
            settings.setValue("endsymbol", QString::fromStdWString(attr->edsym));
            if(attr->r==TRANSPARENT_COLOR)
                settings.setValue("forecolor", TRANSPARENT_COLOR);
            else
                settings.setValue("forecolor", QColor(attr->r,attr->g,attr->b).name());
            if(attr->br==TRANSPARENT_COLOR)
                settings.setValue("backcolor", TRANSPARENT_COLOR);
            else
                settings.setValue("backcolor", QColor(attr->br,attr->bg,attr->bb).name());
            settings.setValue("bold", attr->bold);
            settings.setValue("italic", attr->italic);
            settings.setValue("underline", attr->underline);
            settings.setValue("words",
                    QString::fromStdWString(join(attr->words,L',')).split(","));
            settings.setValue("regexp",  QString::fromStdWString(attr->regexp));
            settings.setValue("layerlevel", attr->layerlevel);
        }
        settings.endArray();

    }
    settings.endArray();



    settings.setValue("a/b/c",10000000);

}

//コンフィグダイアログのデータ(またはsettingsから読み込んだデータ)を反映させる
void MainWindow::applyConfig()
{
    EditorEngine::SetDefaultAttributeSet(conf.filetypes[0].attrset);
    editorview->setFont(conf.fontsize, conf.lineheight, conf.fontfamily, cwtablepath);
    filehistory.setMaximum(conf.maxrecent);

    //モデルにパラメータを反映させる
    for(int i=0; i<models.size(); i++){
        models[i]->applyConfig(&conf);
        if(models[i]==activeModel())
            editorview->attach(models[i]);
    }
}


Model *MainWindow::activeModel()
{
    return  ui->tabbar->tabData(ui->tabbar->currentIndex()).value<Model*>();
}


void MainWindow::setTextEncodingMenu(QAction *action, QString menutext, QString codecname, QString lineend)
{
    QStringList actiondata;
    actiondata.append(codecname);
    actiondata.append(lineend);
    action->setData(actiondata);

    if(codecname == AUTOSIGNATURE)
        codecname = tr("tr_auto");
    if(lineend == AUTOSIGNATURE)
        lineend = tr("tr_auto");
    else{
        lineend = lineend.replace("\r","\\r");
        lineend = lineend.replace("\n","\\n");
    }
    action->setText(tr("%1(%2,%3)").arg(menutext).arg(codecname).arg(lineend));
}

void MainWindow::onTriggerMenu(QAction *action)
{
    Model *m = activeModel();

    if(action == ui->actionNew_File){
        fileNew();
    }
    else if(action == ui->actionOpen){
        openFile();
    }
    else if(action == ui->actionSave){
        save();
    }
    else if(action == ui->actionSaveas){
        saveAs();
    }
    else if(action == ui->actionSaveall){

    }
    else if(action == ui->actionClose){
        on_btntabclose_clicked();
    }
    else if(action == ui->actionCloseall){

    }
    else if(action == ui->actionConfig){
        ConfigDialog dlg(this);
        QStringList widths;

        //ジオメトリ調整
        if(dlgconfigrect.width()!=0)
            dlg.setGeometry(dlgconfigrect);
        else
            dlg.resize(610,500);
        widths = dlgconfig_attrcolwidths.split(",");
        for(int i=0; i<dlg.ui->lstattributes->columnCount(); i++){
            dlg.ui->lstattributes->setColumnWidth(i, widths.at(i).toInt());
        }
        //ダイアログ起動とOKが押されたとき
        if(dlg.exec(conf) == QDialog::Accepted){
            ConfigData oldconf = conf;
            conf = dlg.configData();

            //フォントが変更されていたら幅キャッシュを更新する
            if(conf.fontfamily != oldconf.fontfamily){
                QFile file(cwtablepath);
                if(file.exists())
                    file.remove();
            }

            //適用
            applyConfig();
            writeSettings();
        }
        //ダイアログのサイズを保存
        dlgconfigrect = dlg.geometry();
        widths.clear();
        for(int i=0; i<dlg.ui->lstattributes->columnCount(); i++){
            widths << tr("%1").arg(dlg.ui->lstattributes->columnWidth(i));
        }
        dlgconfig_attrcolwidths = widths.join(",");
    }
    else if(action == ui->actionTextencoding_input ||
            action == ui->actionTextencoding_output){
        ComboboxDialog dlg(this);
        dlg.createComboBoxes(2);
        QList<QByteArray> codecs = QTextCodec::availableCodecs();
        QList<QByteArray>::Iterator it;
        QStringList actiondata = action->data().toStringList();
        dlg.setWindowTitle(tr("文字コード"));
        dlg.setListLabel(0, tr("文字コード:"));
        dlg.setListLabel(1, tr("改行:"));
        //文字コードのコンボボックス
        qSort(codecs);
        dlg.addListData(0,"tr_auto", AUTOSIGNATURE);
        for(it=codecs.begin(); it!=codecs.end(); it++){
            dlg.addListData(0,*it,*it);
        }
        //改行コードのコンボボックス
        dlg.addListData(1, "tr_auto", AUTOSIGNATURE);
        dlg.addListData(1,"LF(\\n)","\n");
        dlg.addListData(1,"CR(\\r)","\r");
        dlg.addListData(1,"CRLF(\\r\\n)","\r\n");
        //初期値
        dlg.setCurrentByValue(0, actiondata.at(0));
        dlg.setCurrentByValue(1, actiondata.at(1));
        //ダイアログ表示
        if(dlg.exec() == QDialog::Accepted){
            if(action == ui->actionTextencoding_input)
                setTextEncodingMenu(action, tr("tr_input"),
                                    dlg.selectedDataValue(0).toString(),
                                    dlg.selectedDataValue(1).toString());
            else
                setTextEncodingMenu(action, tr("tr_output"),
                                    dlg.selectedDataValue(0).toString(),
                                    dlg.selectedDataValue(1).toString());
        }
    }
    else if(action == ui->actionTextencoding_reset){
        setTextEncodingMenu(ui->actionTextencoding_input, tr("tr_input"),
                            AUTOSIGNATURE, AUTOSIGNATURE);
        setTextEncodingMenu(ui->actionTextencoding_output, tr("tr_output"),
                            AUTOSIGNATURE, AUTOSIGNATURE);
    }
    else if(action->parent() == ui->menuRecent){
        openFile(action->text());
    }
    else if(action == ui->actionExit){
        close();
    }


    //編集
    if(action == ui->actionUndo){
        editorview->undo();
    }
    else if(action == ui->actionRedo){
        editorview->redo();
    }
    else if(action == ui->actionCut){
        editorview->cut();
    }
    else if(action == ui->actionCopy){
        editorview->copy();
    }
    else if(action == ui->actionPaste){
        editorview->paste();
    }
    else if(action == ui->actionFind){
        showFindDialog();
    }
    else if(action == ui->actionFindnext){
        findNext();
    }
    else if(action == ui->actionFindprev){
        findPrevious();
    }
    else if(action == ui->actionReplace){
        showReplaceDialog();
    }
    else if(action == ui->actionSelectall){
        editorview->selectAll();
    }

    if(action==ui->actionShowLinenumber){
        editorview->showLineNumber(action->isChecked());
    }
    else if(action==ui->actionShowRuler){
        editorview->showRuler(action->isChecked());
    }
    else if(action==ui->actionShowToolbar){
        ui->mainToolBar->setHidden(!ui->actionShowToolbar->isChecked());
    }
    else if(action==ui->actionShowStatusbar){
        ui->statusBar->setHidden(!ui->actionShowStatusbar->isChecked());
    }

    if(action==ui->actionWebpage){

    }
    else if(action==ui->actionVersion){
        VersionDialog dlg(this);
        dlg.setWindowTitle(tr("バージョン"));
        dlg.exec();
    }
}

void MainWindow::updateMenuRecent()
{
    ui->menuRecent->clear();
    for(int i=0; i<filehistory.histinfos.size(); i++){
        FileHistoryInfo histinfo = filehistory.histinfos[i];
        ui->menuRecent->addAction(histinfo.path);
    }
}

void MainWindow::onCurrentChanged(int index)
{
    Model *model = activeModel();

    //この関数はタブ切り替え時のみならずタブ新規作成時にも呼ばれる
    //そのときはmodelがまだNULLなので除外する
    if(model!=NULL){
        //エディタビュー
        if(editorview->attachedModel() != model){
            editorview->attach(model);
        }
        editorview->repaint();

        //ステータスバー
        QString lineend = model->lineEnd();
        lineend = lineend.replace("\n", "\\n");
        lineend = lineend.replace("\r", "\\r");
        statusBar()->showMessage(tr("%1 %2").arg(model->textCodecName()).arg(lineend));
    }

    //タブを全部閉じたとき
    if(index<0){
        fileNew();
        //この後この関数が再度呼ばれるが、modelがNULLでないので再帰はしない
    }


}

void MainWindow::onEditorViewEntered(QEvent *e)
{
    Model *model = activeModel();
    if(!model)
        return;
    QString lineend = model->lineEnd();
    lineend = lineend.replace("\n", "\\n");
    lineend = lineend.replace("\r", "\\r");
    statusBar()->showMessage(tr("%1 %2").arg(model->textCodecName()).arg(lineend));

}

void MainWindow::onEditorViewDropped(QDropEvent *e)
{
    const QMimeData *mimeData = e->mimeData();

    if(mimeData->hasUrls()){
        QList<QUrl> urlList = mimeData->urls();
        for (int i = 0; i < urlList.size() ; i++){
            if(urlList.at(i).isLocalFile())
                openFile(urlList.at(i).toLocalFile());
        }
    }


}


void MainWindow::closeEvent(QCloseEvent *e)
{
    int i, nmodified=0;
    bool accept=false;

    //スクロール位置を保存
    for(i=0; i<models.size(); i++){
        if(!models[i]->filePath().isEmpty()){
            filehistory.update(models[i]->filePath(), models[i]->GetScrollPos());
        }
    }

    writeSettings();

    //変更されたファイルがあるときは終了してよいか確認
    for(i=0; i<models.size(); i++){
        if(models[i]->IsModified())
            nmodified++;
    }
    if(nmodified==0){
        accept=true;
    }
    else{
        QMessageBox msgBox;
        msgBox.setText(tr("tr_message_confirmclose_text"));
        msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::No);
        int ret = msgBox.exec();
        if(ret == QMessageBox::Yes){
            accept = true;
        }
        else
            accept = false;
    }
    accept?e->accept():e->ignore();
}


void MainWindow::resizeEvent(QResizeEvent *e)
{
}

void MainWindow::showEvent(QShowEvent *e)
{
    //初回表示時のタイミングで初期処理をさせる
    QVariant ffirst = property("firstshowevent");
    if(!ffirst.isValid()){
        setProperty("firstshowevent", true);
        firstShowEvent();
    }

}

void MainWindow::focusOutEvent(QFocusEvent *e)
{


}

void MainWindow::changeEvent(QEvent *e)
{
    if(e->type()==QEvent::ActivationChange){
        //非アクティブ時に自動保存
        FileTypeConfig *filetypeconf = activeModel()->getFileTypeConfig();
        if(!isActiveWindow() && filetypeconf->focusoutsave){
            saveAll();
        }
    }

}






////////////////////////////////////////////////////////////////////////////////
//
//				メニュー関数
//

void MainWindow::fileNew()
{
    //空のモデルを入れる
    Model *model =new Model();
    model->SetModified(false);
    model->applyConfig(&conf);
    editorview->attach(model);
    models.push_back(model);

    int tabindex = ui->tabbar->addTab(NEWFILENAME);
    ui->tabbar->setTabData(tabindex, QVariant::fromValue(model));

    //インデックスが現在と同一だとcurrentChangedシグナルが発せられないので
    //明示的にスロットを呼ぶ
    if(ui->tabbar->currentIndex()==tabindex)
        onCurrentChanged(tabindex);
    else
        ui->tabbar->setCurrentIndex(tabindex);

}



void MainWindow::openFile()
{
    QString fpath;
    fpath = QFileDialog::getOpenFileName(this, tr("tr_filedialog_title"), lastdir);
    if(!fpath.isNull()){
        QFileInfo fileinfo(fpath);
        lastdir = fileinfo.absoluteDir().absolutePath();
        openFile(fpath);
    }
}
void MainWindow::openFile(QString fpath)
{
    Model *model=NULL;
    QFile file(fpath);
    QString codecname = ui->actionTextencoding_input->data().toStringList().at(0);
    QString lineend = ui->actionTextencoding_input->data().toStringList().at(1);

    if(!file.exists()){
        QMessageBox::warning(this, tr("tr_warning_nofile_title"),
                             tr("tr_warning_nofile_text")
                             + QString("\n%1").arg(fpath));
        return;
    }
    if(file.size() >= conf.warningfilesizemb*1024*1024){
        QMessageBox msgBox;
        msgBox.setText(tr("tr_warning_bigsizefile_text"));
        msgBox.setInformativeText(tr("tr_warning_bigsizefile_informative"));
        msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        msgBox.setDefaultButton(QMessageBox::Yes);
        if(msgBox.exec() != QMessageBox::Yes)
            return;
    }
    if(file.size() >= conf.maxfilesizemb*1024*1024){
        QMessageBox msgBox;
        msgBox.setText(tr("tr_error_bigsizefile_text"));
        msgBox.setInformativeText(tr("tr_error_bigsizefile_informative"));
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    }


    //モデル作成と読み込み
    model = new Model();
    model->setFilePath(fpath);
    model->applyConfig(&conf);
    editorview->attach(model);
    if(model->load(codecname, lineend)<0){
        QMessageBox::warning(this, tr("tr_warning_fileloadfailed_title"),
                             tr("tr_warning_fileloadfailed_text"));
        delete model;
        return;
    }
    models.push_back(model);

    //タブ追加
    int tabindex = ui->tabbar->addTab(file.fileName());
    ui->tabbar->setTabData(tabindex, QVariant::fromValue(model));
    ui->tabbar->setCurrentIndex(tabindex);
    editorview->setFocus();

    //もし初期状態のままなら初期タブを消す
    //タブを全部消すことはできないQtの仕様なので、タブ追加後に初期タブを消す
    if(ui->tabbar->count()==2){
        Model *firstmodel = ui->tabbar->tabData(0).value<Model*>();
        if(!firstmodel->IsModified() && firstmodel->GetCharacterCount()==0){
            models.remove(models.indexOf(firstmodel));
            ui->tabbar->removeTab(0);
            delete firstmodel;
        }
    }

    //最近のファイルにスクロールデータがあれば適用
    FileHistoryInfo histinfo = filehistory.find(model->filePath());
    if(!histinfo.path.isEmpty()){
        model->ScrollEvent(histinfo.scrollvalue);
        model->MouseEvent(BUTTON_LEFT,1,0,0,0);
    }

    //最近のファイルを更新
    filehistory.update(model->filePath(), -1);
    updateMenuRecent();
}



void MainWindow::saveAs()
{
    QString fpath;
    QFileInfo finfo;
    Model *m = activeModel();
    QString codecname = ui->actionTextencoding_output->data().toStringList().at(0);
    QString lineend = ui->actionTextencoding_output->data().toStringList().at(1);

    fpath = QFileDialog::getSaveFileName(this, tr("tr_filedialog_title"), m->filePath());
    if(!fpath.isNull()){
        finfo.setFile(fpath);
        ui->tabbar->setTabText(ui->tabbar->currentIndex(), finfo.fileName());
        m->setFilePath(fpath);
        m->save(codecname, lineend);
        lastdir = finfo.absoluteDir().absolutePath();
        filehistory.update(m->filePath(), m->GetScrollPos());
        updateMenuRecent();
    }
}


void MainWindow::save()
{
    Model *m = activeModel();
    QString codecname = ui->actionTextencoding_output->data().toStringList().at(0);
    QString lineend = ui->actionTextencoding_output->data().toStringList().at(1);

    if(m->filePath().isEmpty())
        saveAs();
    else{
        m->save(codecname, lineend);
        filehistory.update(m->filePath(), m->GetScrollPos());
        updateMenuRecent();
    }


}


void MainWindow::saveAll()
{
    int i;
    Model *m = activeModel();
    QString codecname = ui->actionTextencoding_output->data().toStringList().at(0);
    QString lineend = ui->actionTextencoding_output->data().toStringList().at(1);

    for(i=0; i<models.size(); i++){
        if(m->filePath().isEmpty())
            continue;
        m->save(codecname, lineend);
        filehistory.update(m->filePath(), m->GetScrollPos());
    }
    updateMenuRecent();
}



void MainWindow::close()
{
    QMainWindow::close();
}


void MainWindow::showFindDialog()
{
    dlgfind->show(true);
}

void MainWindow::findNext()
{
    QString strfind = dlgfind->ui->txtfind->text();
    if(dlgfind->ui->chkescape->isChecked())
        strfind = Model::unescape(strfind);
    activeModel()->Find(strfind.toStdWString(),
                        dlgfind->ui->chkfromtop->isChecked(),
                        false,
                        dlgfind->ui->chkcasesensitive->isChecked(),
                        dlgfind->ui->chkregularexp->isChecked());
    dlgfind->ui->chkfromtop->setChecked(false);
}

void MainWindow::findPrevious()
{
    QString strfind = dlgfind->ui->txtfind->text();
    if(dlgfind->ui->chkescape->isChecked())
        strfind = Model::unescape(strfind);
    activeModel()->Find(strfind.toStdWString(),
                        dlgfind->ui->chkfromtop->isChecked(),
                        true,
                        dlgfind->ui->chkcasesensitive->isChecked(),
                        dlgfind->ui->chkregularexp->isChecked());
    dlgfind->ui->chkfromtop->setChecked(false);
}

void MainWindow::showReplaceDialog()
{
    //範囲選択されている場合はその範囲、さもなくば全体を検索範囲にする
    activeModel()->IsSelected()?
        dlgfind->ui->rdoselection->setChecked(true):
        dlgfind->ui->rdoalltext->setChecked(true);

    dlgfind->show(false);
}

void MainWindow::replace()
{
    if(dlgfind->ui->chkregularexp->isChecked()){
        activeModel()->Replace(dlgfind->ui->txtfind->text().toStdWString(),
                               dlgfind->ui->txtreplace->text().toStdWString(),
                               dlgfind->ui->chkfromtop->isChecked(),
                               dlgfind->ui->chkcasesensitive->isChecked(),
                               true);
    }
    else{
        QString strfind = dlgfind->ui->txtfind->text();
        if(dlgfind->ui->chkescape->isChecked())
            strfind = Model::unescape(strfind);
        QString strreplace = dlgfind->ui->txtreplace->text();
        if(dlgfind->ui->chkescape->isChecked())
            strreplace = Model::unescape(strreplace);

        activeModel()->Replace(strfind.toStdWString(),
                               strreplace.toStdWString(),
                               dlgfind->ui->chkfromtop->isChecked(),
                               dlgfind->ui->chkcasesensitive->isChecked(),
                               false);

    }
    dlgfind->ui->chkfromtop->setChecked(false);

}

void MainWindow::replaceAll()
{
    if(dlgfind->ui->chkregularexp->isChecked()){
        activeModel()->ReplaceAll(dlgfind->ui->txtfind->text().toStdWString(),
                                  dlgfind->ui->txtreplace->text().toStdWString(),
                                  dlgfind->ui->rdoalltext->isChecked(),
                                  dlgfind->ui->chkcasesensitive->isChecked(),
                                  true);
    }
    else{
        QString strfind = dlgfind->ui->txtfind->text();
        if(dlgfind->ui->chkescape->isChecked())
            strfind = Model::unescape(strfind);
        QString strreplace = dlgfind->ui->txtreplace->text();
        if(dlgfind->ui->chkescape->isChecked())
            strreplace = Model::unescape(strreplace);
        activeModel()->ReplaceAll(strfind.toStdWString(),
                                  strreplace.toStdWString(),
                                  dlgfind->ui->rdoalltext->isChecked(),
                                  dlgfind->ui->chkcasesensitive->isChecked(),
                                  false);

    }
}




void MainWindow::on_btntabclose_clicked()
{
    Model *m = activeModel();
    int ret = QMessageBox::No;

    //変更されていたらセーブするか確認ダイアログを出す
    if(m->IsModified()){
        QMessageBox msgBox;
        msgBox.setText(tr("tr_message_filemodified_text"));
        msgBox.setInformativeText(tr("tr_message_filemodified_informative"));
        msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Yes);
        ret = msgBox.exec();
        if(ret == QMessageBox::Yes)
            save();
    }

    if(ret!=QMessageBox::Cancel){
        FileHistoryInfo historyinfo = filehistory.find(m->filePath());
        if(!historyinfo.path.isEmpty()){
            filehistory.update(m->filePath(), m->GetScrollPos());
        }
        models.remove(models.indexOf(m));
        ui->tabbar->removeTab(ui->tabbar->currentIndex());
        delete m;
    }
}


void MainWindow::on_editorview_mouseRightButtonPressed(QMouseEvent *e)
{
    //popupはスクリーン座標を指定する必要がある
    QPoint pos = editorview->mapToGlobal(e->pos());
    popupmenu->addAction(ui->actionCut);
    popupmenu->addAction(ui->actionCopy);
    popupmenu->addAction(ui->actionPaste);
    popupmenu->popup(pos);

}


