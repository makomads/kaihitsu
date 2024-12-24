#include <QApplication>
#include <QDesktopWidget>
#include <QFontDatabase>
#include <QMessageBox>
#include "configdialog.h"
#include "ui_configdialog.h"
#include "model.h"

ConfigDialog::ConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}

int ConfigDialog::exec(const ConfigData &configdata)
{
    data = configdata;

    ui->btnapply->hide();

    //共通タブ
    QFontDatabase fontdb;
    for(int i=0; i<fontdb.families().count(); i++){
        ui->cmb_fontfamily->insertItem(i, fontdb.families()[i] );
        if( fontdb.families()[i] == data.fontfamily )
            ui->cmb_fontfamily->setCurrentIndex(i);
    }
    ui->edt_fontsize->setText(tr("%1").arg(data.fontsize));
    ui->edt_lineheight->setText(tr("%1").arg(data.lineheight));
    ui->edt_filesize_warning->setText(tr("%1").arg(data.warningfilesizemb));
    ui->edt_filesize_max->setText(tr("%1").arg(data.maxfilesizemb));
    ui->edt_scrollsize->setText(tr("%1").arg(data.scrollsize));
    ui->edt_maxrecent->setText(tr("%1").arg(data.maxrecent));
    ui->chk_inactsave->setChecked(data.inactiveautosave);

    //タイプ別設定タブ
    for(int i=0; i<data.filetypes.size(); i++){
        QListWidgetItem *item = new QListWidgetItem(data.filetypes[i].name, ui->lsttypes);
        FileTypeConfig *filetype = new FileTypeConfig();
        *filetype = data.filetypes[i];
        item->setData(Qt::UserRole, QVariant::fromValue(filetype));
        ui->lsttypes->addItem(item);
    }

    //changedスロットはaddItemでは呼ばれないので明示的に初期値をセットする
    ui->lsttypes->setCurrentRow(0);

    return QDialog::exec();
}




void ConfigDialog::closeEvent(QCloseEvent *e)
{
    for(int i=0;i<ui->lsttypes->count(); i++){
        delete ui->lsttypes->item(i)->data(Qt::UserRole).value<FileTypeConfig*>();
    }
}


void ConfigDialog::on_btnok_clicked()
{
    on_btnapply_clicked();
    done(QDialog::Accepted);
}

void ConfigDialog::on_btncancel_clicked()
{
    done(QDialog::Rejected);
}

void ConfigDialog::on_btnapply_clicked()
{    
    bool ok;

    //共通タブ
    data.fontfamily = ui->cmb_fontfamily->currentText();
    data.fontsize = ui->edt_fontsize->text().toInt(&ok);
    if(!ok) data.fontsize = 14;
    data.lineheight = ui->edt_lineheight->text().toInt(&ok);
    if(!ok) data.lineheight = 14;
    data.warningfilesizemb = ui->edt_filesize_warning->text().toInt(&ok);
    if(!ok) data.warningfilesizemb = 30;
    data.maxfilesizemb = ui->edt_filesize_max->text().toInt(&ok);
    if(!ok) data.maxfilesizemb = 100;
    data.scrollsize = ui->edt_scrollsize->text().toInt(&ok);
    if(!ok) data.scrollsize = 5;
    data.maxrecent = ui->edt_maxrecent->text().toInt(&ok);
    if(!ok) data.maxrecent = 20;
    data.inactiveautosave = ui->chk_inactsave->isChecked();

    //ファイルタイプタブ
    fileTypePageToData(ui->lsttypes->currentItem());
    data.filetypes.clear();
    data.filetypes.resize(ui->lsttypes->count());
    for(int i=0;i<data.filetypes.size(); i++){
        data.filetypes[i] = *ui->lsttypes->item(i)->data(Qt::UserRole).value<FileTypeConfig*>();
    }

}


//ファイルタイプリスト選択時
//このスロットはaddItem()では呼ばれない
void ConfigDialog::on_lsttypes_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if(previous)
        fileTypePageToData(previous);
    dataToFiletypePage(current);
}




//ファイルタイプページのデータを書き戻しする
void ConfigDialog::fileTypePageToData(QListWidgetItem* listitem)
{
    bool ok;
    FileTypeConfig *filetype;
    filetype = listitem->data(Qt::UserRole).value<FileTypeConfig*>();

    filetype->name = ui->txttypename->text();
    listitem->setText(filetype->name);
    filetype->exts = ui->txtextension->text().split(",");
    filetype->tabsize = ui->txttabsize->text().toInt(&ok);
    if(!ok) filetype->tabsize = 8;
    filetype->wrapsize = ui->txtwrapsize->text().toInt(&ok);
    if(!ok) filetype->wrapsize = 80;
    filetype->autowrap = ui->chkautowrap->isChecked();
    filetype->attrset.clear();
    for(int i=0; i<ui->lstattributes->rowCount(); i++){
        EditorAttribute attr;
        QTableWidgetItem *item;
        int col=0;
        QComboBox *combo;
        QStringList strs;
        std::vector<std::wstring> words;
        //名前とID
        item = ui->lstattributes->item(i,col++);
        attr.id = item->data(Qt::UserRole).toInt();
        attr.name = item->text().toStdWString();
        attr.layerlevel = ui->lstattributes->rowCount() - i;
        //種別
        combo = (QComboBox*)ui->lstattributes->cellWidget(i,col++);
        attr.type = combo->itemData(combo->currentIndex()).toInt();
        //シンボル
        item = ui->lstattributes->item(i,col++);
        words = split(Model::unescape(item->text()).toStdWString(),L',');
        if(attr.type==ATTR_WORD){
            attr.words = words;
        }
        else if(attr.type==ATTR_RANGE || attr.type==ATTR_MULTILINE){
            attr.stsym = words[0];
            if(words.size()>=2)
                attr.edsym = words[1];
        }
        else if(attr.type==ATTR_REGEXP){
            attr.regexp = item->text().toStdWString();
        }
        //文字色
        strs = ui->lstattributes->item(i,col++)->text().split(",");
        attr.r = strs[0].toInt();
        attr.g = strs[1].toInt();
        attr.b = strs[2].toInt();
        //背景色
        strs = ui->lstattributes->item(i,col++)->text().split(",");
        attr.br = strs[0].toInt();
        attr.bg = strs[1].toInt();
        attr.bb = strs[2].toInt();
        //BIU
        attr.bold = ui->lstattributes->item(i,col++)->checkState()==Qt::Checked?true:false;
        attr.italic = ui->lstattributes->item(i,col++)->checkState()==Qt::Checked?true:false;
        attr.underline = ui->lstattributes->item(i,col++)->checkState()==Qt::Checked?true:false;
        filetype->attrset.push_back(attr);
    }

}


void ConfigDialog::dataToFiletypePage(QListWidgetItem *listitem)
{
    //各ウィジェットへセット
    FileTypeConfig *ftc = listitem->data(Qt::UserRole).value<FileTypeConfig*>();
    ui->txttypename->setText(ftc->name);
    ui->txtextension->setText(ftc->exts.join(","));
    ui->txttabsize->setText(tr("%1").arg(ftc->tabsize));
    ui->txtwrapsize->setText(tr("%1").arg(ftc->wrapsize));
    ui->chkautowrap->setChecked(ftc->autowrap);
    ui->lstattributes->clearContents();
    ui->lstattributes->setRowCount(0);
    for(int i=0; i<ftc->attrset.size(); i++){
        QTableWidgetItem *item;
        QString itemtext;
        EditorAttribute *attr = &ftc->attrset[i];
        int col=0;
        ui->lstattributes->insertRow(i);
        //名前と隠し項目のID
        item = new QTableWidgetItem(QString::fromStdWString(attr->name));
        item->setData(Qt::UserRole, attr->id);
        ui->lstattributes->setItem(i, col++, item);
        //種別、文字列
        QComboBox *combo = new QComboBox(ui->lstattributes);
        if(attr->id<ATTR_USER){
            combo->addItem(tr("tr_attype_special"), attr->type);
            ui->lstattributes->setCellWidget(i, col++, combo);
            ui->lstattributes->setItem(i, col++, new QTableWidgetItem(""));
        }
        else{
            combo->addItem(tr("tr_attype_range"), ATTR_RANGE);
            combo->addItem(tr("tr_attype_word"), ATTR_WORD);
            combo->addItem(tr("tr_attype_regexp"), ATTR_REGEXP);
            combo->addItem(tr("tr_attype_multilne"), ATTR_MULTILINE);
            combo->setCurrentIndex(combo->findData(attr->type));
            ui->lstattributes->setCellWidget(i, col++, combo);
            if(attr->type==ATTR_RANGE || attr->type==ATTR_MULTILINE){
                itemtext = Model::escape(
                    tr("%1,%2").arg(QString::fromStdWString(attr->stsym))
                            .arg(QString::fromStdWString(attr->edsym))
                );
            }
            else if(attr->type==ATTR_WORD){
                itemtext = Model::escape(QString::fromStdWString(join(attr->words,L',')));
            }
            else if(attr->type==ATTR_REGEXP){
                itemtext = Model::escape(QString::fromStdWString(attr->regexp));
            }
            item = new QTableWidgetItem(itemtext);
            ui->lstattributes->setItem(i, col++, item);
        }
        //文字色
        item = new QTableWidgetItem(tr("%1,%2,%3").arg(attr->r).arg(attr->g).arg(attr->b));
        ui->lstattributes->setItem(i, col++, item);
        //背景色
        item = new QTableWidgetItem(tr("%1,%2,%3").arg(attr->br).arg(attr->bg).arg(attr->bb));
        ui->lstattributes->setItem(i, col++, item);
        //BIU
        item = new QTableWidgetItem();
        item->setCheckState(attr->bold?Qt::Checked:Qt::Unchecked);
        ui->lstattributes->setItem(i, col++, item);
        item = new QTableWidgetItem();
        item->setCheckState(attr->italic?Qt::Checked:Qt::Unchecked);
        ui->lstattributes->setItem(i, col++, item);
        item = new QTableWidgetItem();
        item->setCheckState(attr->underline?Qt::Checked:Qt::Unchecked);
        ui->lstattributes->setItem(i, col++, item);
    }
}



void ConfigDialog::on_btnmoveupfiletype_clicked()
{
    int row;
    QListWidgetItem *item;

    row = ui->lsttypes->currentRow();
    if(row==0 || row==1)    //デフォルト属性は動かさない
        return;

    item = ui->lsttypes->takeItem(row);
    ui->lsttypes->insertItem(row-1,item);
    ui->lsttypes->setCurrentRow(row-1);
}

void ConfigDialog::on_btnmovedownfiletype_clicked()
{
    int row;
    QListWidgetItem *item;

    row = ui->lsttypes->currentRow();
    if(row==0 || row==ui->lsttypes->count()-1)
        return;

    item = ui->lsttypes->takeItem(row);
    ui->lsttypes->insertItem(row+1,item);
    ui->lsttypes->setCurrentRow(row+1);
}

void ConfigDialog::on_btnaddfiletype_clicked()
{
    QListWidgetItem *item;
    FileTypeConfig *filetype = new FileTypeConfig();
    filetype->name = tr("item%1").arg(ui->lsttypes->count());
    filetype->tabsize = 8;
    filetype->wrapsize = 80;
    filetype->autowrap = false;

    //リストへ追加
    item = new QListWidgetItem(filetype->name, ui->lsttypes);
    item->setData(Qt::UserRole, QVariant::fromValue(filetype));
    ui->lsttypes->addItem(item);
    ui->lsttypes->setCurrentRow(ui->lsttypes->count()-1);


}

void ConfigDialog::on_btndelfiletype_clicked()
{
    QListWidgetItem *item;
    int row = ui->lsttypes->currentRow();

    if(row==0){
        QMessageBox::warning(this, "Warning", "The default filetype can not be removed.");
        return;
    }
    item = ui->lsttypes->takeItem(row);
    delete item->data(Qt::UserRole).value<FileTypeConfig*>();
    delete item;
}





//属性リストのアイテム変更時
//このスロットはsetItem()で呼ばれる
void ConfigDialog::on_lstattributes_itemChanged(QTableWidgetItem *item)
{
    QStringList rgb;
    int r,g,b;
    bool ok;
    int substcolor;
    int row = item->row();

    //背景色変更
    if(item->column()==3 || item->column()==4){
        //エラー時の代用色
        if(item->column()==3)
            substcolor=0;
        if(item->column()==4)
            substcolor=TRANSPARENT_COLOR;

        //RGBに分割
        rgb = item->text().split(",");
        if(rgb.length()==1){
            r = rgb[0].toInt(&ok);
            r = ok?qBound(TRANSPARENT_COLOR, r, 255):substcolor;
            g = b = r;
        }
        else if(rgb.length()==3){
            r = rgb[0].toInt(&ok);
            r = ok?qBound(TRANSPARENT_COLOR, r, 255):substcolor;
            g = rgb[1].toInt(&ok);
            g = ok?qBound(TRANSPARENT_COLOR, g, 255):substcolor;
            b = rgb[2].toInt(&ok);
            b = ok?qBound(TRANSPARENT_COLOR, b, 255):substcolor;
        }
        else{
            r = g = b = substcolor;
        }
        if(r==TRANSPARENT_COLOR)
            g = b = TRANSPARENT_COLOR;

        //テーブルに書き戻し
        //setText()はsetItem()と違いシグナルを出さないのでループにはならない
        item->setText(tr("%1,%2,%3").arg(r).arg(g).arg(b));
        if(r==TRANSPARENT_COLOR){
            item->setBackgroundColor(QColor("white"));
            item->setTextColor(QColor("black"));
        }else{
            item->setBackgroundColor(QColor(r,g,b));
            if(r+g+b < (256*3)/2)
                item->setTextColor(QColor("white"));
        }
    }

}


//属性リストの項目up
void ConfigDialog::on_btnmoveupattr_clicked()
{
    int row = ui->lstattributes->currentRow();
    if(row==0)
        return;
    swapQTableWidgetRows(row, row-1);
    ui->lstattributes->setCurrentCell(row-1, ui->lstattributes->currentColumn());
}

//属性リストの項目down
void ConfigDialog::on_btnmovedownattr_clicked()
{
    int row = ui->lstattributes->currentRow();
    if(row==ui->lstattributes->rowCount()-1)
        return;
    swapQTableWidgetRows(row, row+1);
    ui->lstattributes->setCurrentCell(row+1, ui->lstattributes->currentColumn());
}

//属性追加
void ConfigDialog::on_btnaddattr_clicked()
{
    int newid;
    int lastrow = ui->lstattributes->rowCount();
    int col=0;
    QTableWidgetItem *item;

    //新規IDを作る
    if(ui->lsttypes->currentRow()==0)
        newid = ATTR_COMMON;
    else
        newid = ATTR_USER;
    while(true){
        int i;
        for(i=0; i<lastrow; i++){
            if(ui->lstattributes->item(i,0)->data(Qt::UserRole).toInt() == newid)
                break;
        }
        if(i==lastrow)
            break;
        newid++;
    }

    //行追加
    ui->lstattributes->insertRow(lastrow);

    //名前と隠し項目のID
    item = new QTableWidgetItem(tr("item%1").arg(lastrow+1));
    item->setData(Qt::UserRole, newid);
    ui->lstattributes->setItem(lastrow, col++, item);
    //種別
    QComboBox *combo = new QComboBox(ui->lstattributes);
    combo->addItem(tr("tr_attype_range"), ATTR_RANGE);
    combo->addItem(tr("tr_attype_word"), ATTR_WORD);
    combo->addItem(tr("tr_attype_regexp"), ATTR_REGEXP);
    combo->addItem(tr("tr_attype_multilne"), ATTR_MULTILINE);
    combo->setCurrentIndex(0);
    ui->lstattributes->setCellWidget(lastrow, col++, combo);
    //文字列
    ui->lstattributes->setItem(lastrow, col++, new QTableWidgetItem(","));
    //文字色
    ui->lstattributes->setItem(lastrow, col++, new QTableWidgetItem("0,0,0"));
    //背景色
    ui->lstattributes->setItem(lastrow, col++, new QTableWidgetItem("-1,-1,-1"));
    //BIU
    for(int i=0; i<3; i++){
        item = new QTableWidgetItem();
        item->setCheckState(Qt::Unchecked);
        ui->lstattributes->setItem(lastrow, col++, item);
    }

}

//属性削除
void ConfigDialog::on_btndelattr_clicked()
{
    ui->lstattributes->removeRow(ui->lstattributes->currentRow());
}



void ConfigDialog::swapQTableWidgetRows(int row1, int row2)
{
    QTableWidgetItem *item1, *item2;
    QComboBox *combo1, *combo2;
    for(int i=0; i<ui->lstattributes->columnCount(); i++){
        /*
         * QTableWidget::setCellWidgetは元々あったwidgetのオーナーシップを削除する
         * なのでsetCellWidget(cellWidget())ではオーナーシップが消えてしまう
         * そこでデータの単位でスワップする必要がある
        */
        combo1 = dynamic_cast<QComboBox*>(ui->lstattributes->cellWidget(row1,i));
        if(combo1){
            combo2 = dynamic_cast<QComboBox*>(ui->lstattributes->cellWidget(row2,i));
            QVector<QVariant> datum1, datum2;
            QVector<QString> text1, text2;
            int index1, index2;
            for(int j=0; j<combo1->count(); j++){
                datum1.push_back(combo1->itemData(j));
                text1.push_back(combo1->itemText(j));
            }
            for(int j=0; j<combo2->count(); j++){
                datum2.push_back(combo2->itemData(j));
                text2.push_back(combo2->itemText(j));
            }
            index1 = combo1->currentIndex();
            index2 = combo2->currentIndex();
            combo1->clear();
            combo2->clear();
            for(int j=0; j<datum1.size(); j++){
                combo2->addItem(text1[j], datum1[j]);
            }
            for(int j=0; j<datum2.size(); j++){
                combo1->addItem(text2[j], datum2[j]);
            }
            combo2->setCurrentIndex(index1);
            combo1->setCurrentIndex(index2);
        }
        else{
            item1 = ui->lstattributes->takeItem(row1, i);
            item2 = ui->lstattributes->takeItem(row2, i);
            ui->lstattributes->setItem(row1,i,item2);
            ui->lstattributes->setItem(row2,i,item1);
        }
    }
}



