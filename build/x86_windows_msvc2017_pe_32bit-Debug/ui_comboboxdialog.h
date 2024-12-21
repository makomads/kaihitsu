/********************************************************************************
** Form generated from reading UI file 'comboboxdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMBOBOXDIALOG_H
#define UI_COMBOBOXDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ComboboxDialog
{
public:
    QVBoxLayout *lyoVert;
    QGridLayout *lyoGrid;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ComboboxDialog)
    {
        if (ComboboxDialog->objectName().isEmpty())
            ComboboxDialog->setObjectName(QString::fromUtf8("ComboboxDialog"));
        ComboboxDialog->resize(400, 126);
        lyoVert = new QVBoxLayout(ComboboxDialog);
        lyoVert->setObjectName(QString::fromUtf8("lyoVert"));
        lyoGrid = new QGridLayout();
        lyoGrid->setObjectName(QString::fromUtf8("lyoGrid"));

        lyoVert->addLayout(lyoGrid);

        buttonBox = new QDialogButtonBox(ComboboxDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        lyoVert->addWidget(buttonBox);


        retranslateUi(ComboboxDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), ComboboxDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ComboboxDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ComboboxDialog);
    } // setupUi

    void retranslateUi(QDialog *ComboboxDialog)
    {
        ComboboxDialog->setWindowTitle(QApplication::translate("ComboboxDialog", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ComboboxDialog: public Ui_ComboboxDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMBOBOXDIALOG_H
