/********************************************************************************
** Form generated from reading UI file 'versiondialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VERSIONDIALOG_H
#define UI_VERSIONDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_VersionDialog
{
public:
    QDialogButtonBox *buttonBox;
    QLabel *lblversion;

    void setupUi(QDialog *VersionDialog)
    {
        if (VersionDialog->objectName().isEmpty())
            VersionDialog->setObjectName(QString::fromUtf8("VersionDialog"));
        VersionDialog->resize(400, 300);
        buttonBox = new QDialogButtonBox(VersionDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        lblversion = new QLabel(VersionDialog);
        lblversion->setObjectName(QString::fromUtf8("lblversion"));
        lblversion->setGeometry(QRect(80, 90, 50, 12));

        retranslateUi(VersionDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), VersionDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), VersionDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(VersionDialog);
    } // setupUi

    void retranslateUi(QDialog *VersionDialog)
    {
        VersionDialog->setWindowTitle(QApplication::translate("VersionDialog", "Dialog", nullptr));
        lblversion->setText(QApplication::translate("VersionDialog", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class VersionDialog: public Ui_VersionDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VERSIONDIALOG_H
