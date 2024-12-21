/********************************************************************************
** Form generated from reading UI file 'finddialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FINDDIALOG_H
#define UI_FINDDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_FindDialog
{
public:
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *lblfind;
    QLineEdit *txtfind;
    QLabel *lblreplace;
    QLineEdit *txtreplace;
    QCheckBox *chkescape;
    QCheckBox *chkregularexp;
    QCheckBox *chkfromtop;
    QCheckBox *chkcasesensitive;
    QGroupBox *grparea;
    QVBoxLayout *verticalLayout_3;
    QRadioButton *rdoselection;
    QRadioButton *rdoalltext;
    QSpacerItem *verticalSpacer;
    QVBoxLayout *verticalLayout_2;
    QPushButton *btnreplaceall;
    QPushButton *btnreplace;
    QPushButton *btnfindprevious;
    QPushButton *btnfindnext;
    QPushButton *btnclosefind;
    QSpacerItem *verticalSpacer_2;

    void setupUi(QDialog *FindDialog)
    {
        if (FindDialog->objectName().isEmpty())
            FindDialog->setObjectName(QString::fromUtf8("FindDialog"));
        FindDialog->resize(489, 281);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(FindDialog->sizePolicy().hasHeightForWidth());
        FindDialog->setSizePolicy(sizePolicy);
        horizontalLayout = new QHBoxLayout(FindDialog);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetNoConstraint);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        lblfind = new QLabel(FindDialog);
        lblfind->setObjectName(QString::fromUtf8("lblfind"));

        gridLayout->addWidget(lblfind, 0, 0, 1, 1);

        txtfind = new QLineEdit(FindDialog);
        txtfind->setObjectName(QString::fromUtf8("txtfind"));

        gridLayout->addWidget(txtfind, 0, 1, 1, 1);

        lblreplace = new QLabel(FindDialog);
        lblreplace->setObjectName(QString::fromUtf8("lblreplace"));

        gridLayout->addWidget(lblreplace, 1, 0, 1, 1);

        txtreplace = new QLineEdit(FindDialog);
        txtreplace->setObjectName(QString::fromUtf8("txtreplace"));

        gridLayout->addWidget(txtreplace, 1, 1, 1, 1);


        verticalLayout->addLayout(gridLayout);

        chkescape = new QCheckBox(FindDialog);
        chkescape->setObjectName(QString::fromUtf8("chkescape"));

        verticalLayout->addWidget(chkescape);

        chkregularexp = new QCheckBox(FindDialog);
        chkregularexp->setObjectName(QString::fromUtf8("chkregularexp"));

        verticalLayout->addWidget(chkregularexp);

        chkfromtop = new QCheckBox(FindDialog);
        chkfromtop->setObjectName(QString::fromUtf8("chkfromtop"));

        verticalLayout->addWidget(chkfromtop);

        chkcasesensitive = new QCheckBox(FindDialog);
        chkcasesensitive->setObjectName(QString::fromUtf8("chkcasesensitive"));

        verticalLayout->addWidget(chkcasesensitive);

        grparea = new QGroupBox(FindDialog);
        grparea->setObjectName(QString::fromUtf8("grparea"));
        sizePolicy.setHeightForWidth(grparea->sizePolicy().hasHeightForWidth());
        grparea->setSizePolicy(sizePolicy);
        verticalLayout_3 = new QVBoxLayout(grparea);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        rdoselection = new QRadioButton(grparea);
        rdoselection->setObjectName(QString::fromUtf8("rdoselection"));

        verticalLayout_3->addWidget(rdoselection);

        rdoalltext = new QRadioButton(grparea);
        rdoalltext->setObjectName(QString::fromUtf8("rdoalltext"));

        verticalLayout_3->addWidget(rdoalltext);


        verticalLayout->addWidget(grparea);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        btnreplaceall = new QPushButton(FindDialog);
        btnreplaceall->setObjectName(QString::fromUtf8("btnreplaceall"));

        verticalLayout_2->addWidget(btnreplaceall);

        btnreplace = new QPushButton(FindDialog);
        btnreplace->setObjectName(QString::fromUtf8("btnreplace"));

        verticalLayout_2->addWidget(btnreplace);

        btnfindprevious = new QPushButton(FindDialog);
        btnfindprevious->setObjectName(QString::fromUtf8("btnfindprevious"));

        verticalLayout_2->addWidget(btnfindprevious);

        btnfindnext = new QPushButton(FindDialog);
        btnfindnext->setObjectName(QString::fromUtf8("btnfindnext"));

        verticalLayout_2->addWidget(btnfindnext);

        btnclosefind = new QPushButton(FindDialog);
        btnclosefind->setObjectName(QString::fromUtf8("btnclosefind"));

        verticalLayout_2->addWidget(btnclosefind);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);


        horizontalLayout->addLayout(verticalLayout_2);


        retranslateUi(FindDialog);

        QMetaObject::connectSlotsByName(FindDialog);
    } // setupUi

    void retranslateUi(QDialog *FindDialog)
    {
        FindDialog->setWindowTitle(QApplication::translate("FindDialog", "Dialog", nullptr));
        lblfind->setText(QApplication::translate("FindDialog", "\346\244\234\347\264\242\346\226\207\345\255\227", nullptr));
        lblreplace->setText(QApplication::translate("FindDialog", "\347\275\256\346\217\233\346\226\207\345\255\227", nullptr));
        chkescape->setText(QApplication::translate("FindDialog", "\343\202\250\343\202\271\343\202\261\343\203\274\343\203\227(\\n:\346\224\271\350\241\214,\\t:\343\202\277\343\203\226,\\\\:\\\343\201\253\347\275\256\343\201\215\346\217\233\343\201\210 \346\255\243\350\246\217\350\241\250\347\217\276\343\202\210\343\202\212\351\253\230\351\200\237)", nullptr));
        chkregularexp->setText(QApplication::translate("FindDialog", "\346\255\243\350\246\217\350\241\250\347\217\276", nullptr));
        chkfromtop->setText(QApplication::translate("FindDialog", "\345\205\210\351\240\255\343\201\213\343\202\211\346\244\234\347\264\242", nullptr));
        chkcasesensitive->setText(QApplication::translate("FindDialog", "\345\244\247\346\226\207\345\255\227\345\260\217\346\226\207\345\255\227\343\202\222\345\214\272\345\210\245\343\201\231\343\202\213", nullptr));
        grparea->setTitle(QApplication::translate("FindDialog", "\344\270\200\346\213\254\347\275\256\346\217\233\347\257\204\345\233\262", nullptr));
        rdoselection->setText(QApplication::translate("FindDialog", "\351\201\270\346\212\236\347\257\204\345\233\262", nullptr));
        rdoalltext->setText(QApplication::translate("FindDialog", "\346\226\207\346\233\270\345\205\250\344\275\223", nullptr));
        btnreplaceall->setText(QApplication::translate("FindDialog", "\344\270\200\346\213\254\347\275\256\346\217\233", nullptr));
        btnreplace->setText(QApplication::translate("FindDialog", "\347\275\256\346\217\233/\346\234\200\345\210\235\343\201\256\346\244\234\347\264\242", nullptr));
        btnfindprevious->setText(QApplication::translate("FindDialog", "\344\270\212\346\226\271\343\201\270\346\244\234\347\264\242(Shift+F3)", nullptr));
#ifndef QT_NO_SHORTCUT
        btnfindprevious->setShortcut(QApplication::translate("FindDialog", "Shift+F3", nullptr));
#endif // QT_NO_SHORTCUT
        btnfindnext->setText(QApplication::translate("FindDialog", "\344\270\213\346\226\271\343\201\270\346\244\234\347\264\242(F3)", nullptr));
#ifndef QT_NO_SHORTCUT
        btnfindnext->setShortcut(QApplication::translate("FindDialog", "F3", nullptr));
#endif // QT_NO_SHORTCUT
        btnclosefind->setText(QApplication::translate("FindDialog", "\351\226\211\343\201\230\343\202\213(Esc)", nullptr));
#ifndef QT_NO_SHORTCUT
        btnclosefind->setShortcut(QApplication::translate("FindDialog", "Esc", nullptr));
#endif // QT_NO_SHORTCUT
    } // retranslateUi

};

namespace Ui {
    class FindDialog: public Ui_FindDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FINDDIALOG_H
