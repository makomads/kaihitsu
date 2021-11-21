#include <QHBoxLayout>
#include "comboboxdialog.h"
#include "ui_comboboxdialog.h"

ComboboxDialog::ComboboxDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComboboxDialog)
{
    ui->setupUi(this);
}

ComboboxDialog::~ComboboxDialog()
{
    delete ui;
}

void ComboboxDialog::createComboBoxes(int count)
{
    int i;

    comboboxes.resize(count);
    labels.resize(count);

    for(i=0; i<count; i++){

        labels[i] = new QLabel();
        ui->lyoGrid->addWidget(labels[i],i,0);

        comboboxes[i] = new QComboBox();
        comboboxes[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        ui->lyoGrid->addWidget(comboboxes[i],i,1);
    }
}

void ComboboxDialog::setCurrentByValue(int index, QVariant value)
{
    QComboBox *combo = comboboxes[index];
    for(int i=0; i<combo->count(); i++){
        if(combo->itemData(i) == value){
            combo->setCurrentIndex(i);
            break;
        }
    }
}

void ComboboxDialog::setListData(int index, const ComboboxDialogData &listdata)
{
    ComboboxDialogData::const_iterator it;
    clearListData(index);
    for(it=listdata.begin(); it!=listdata.end(); it++){
        addListData(index, (*it).first, (*it).second);
    }
}

void ComboboxDialog::addListData(int index, const QString &name, const QVariant &value)
{
    comboboxes.at(index)->addItem(name, value);
}

void ComboboxDialog::setListLabel(int index, const QString &label)
{
    labels.at(index)->setText(label);
}

void ComboboxDialog::clearListData(int index)
{
    comboboxes.at(index)->clear();
}

QString ComboboxDialog::selectedDataName(int index)
{
    return comboboxes[index]->itemText(comboboxes[index]->currentIndex());
}

QVariant ComboboxDialog::selectedDataValue(int index)
{
    return comboboxes[index]->itemData(comboboxes[index]->currentIndex());
}



