#include "finddialog.h"
#include "ui_finddialog.h"

FindDialog::FindDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FindDialog)
{
    ui->setupUi(this);
    savedgeom_find=QRect(0,0,0,0);
    savedgeom_replace=QRect(0,0,0,0);
    //サイズをlayoutSizeConstraintをNoConstraintにすること

    connect(ui->txtfind,SIGNAL(returnPressed()), ui->btnfindnext, SLOT(click()));

    grpfindarea = new QButtonGroup(this);
    grpfindarea->addButton(ui->rdoalltext);
    grpfindarea->addButton(ui->rdoselection);
}

FindDialog::~FindDialog()
{
    delete ui;
    delete grpfindarea;
}

void FindDialog::show(bool tofind)
{
    QVector<QWidget*> hiddenparts, shownparts;
    int i;

    this->tofind = tofind;
    //検索用パーツ
    if(tofind){
        //置換用パーツを隠す
        hiddenparts << ui->grparea << ui->txtreplace << ui->lblreplace
                    <<ui->btnreplace << ui->btnreplaceall;
        //検索用パーツを表示
        shownparts  << ui->txtfind << ui->chkfromtop << ui->chkregularexp
                   <<ui->btnfindnext << ui->btnfindprevious << ui->btnclosefind;

        //位置は開くたびに初期化されるので記憶したもので書き戻す
        if(savedgeom_find.width()==0){
            savedgeom_find = geometry();
        }
        else{
            setGeometry(savedgeom_find);
        }
    }
    //置換用パーツ
    else{
        hiddenparts << ui->btnfindprevious;
        shownparts << ui->txtfind << ui->chkfromtop << ui->chkregularexp
                   <<ui->btnfindnext  << ui->btnclosefind
                  << ui->grparea << ui->txtreplace << ui->lblreplace
                                  <<ui->btnreplace << ui->btnreplaceall;

        if(savedgeom_replace.width()==0){
            savedgeom_replace = geometry();
        }
        else{
            setGeometry(savedgeom_replace);
        }
    }

    for(i=0; i<shownparts.size(); i++){
        shownparts[i]->show();
    }
    for(i=0; i<hiddenparts.size(); i++){
        hiddenparts[i]->hide();
    }

    ui->txtfind->setFocus();
    ui->txtfind->selectAll();
    QDialog::show();
/*
#ifdef QT_DEBUG
    ui->rdoselection->setChecked(true);
    ui->txtfind->setText("abc");
#endif*/
}


void FindDialog::closeEvent(QCloseEvent *e)
{
    if(tofind)
        savedgeom_find = geometry();
    else
        savedgeom_replace = geometry();
}




void FindDialog::on_btnclosefind_clicked()
{
    close();
}
