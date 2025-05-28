#include "versiondialog.h"
#include "ui_versiondialog.h"

VersionDialog::VersionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VersionDialog)
{
    ui->setupUi(this);

    ui->lblversion->setText(VERSION_STRING);
}

VersionDialog::~VersionDialog()
{
    delete ui;
}
