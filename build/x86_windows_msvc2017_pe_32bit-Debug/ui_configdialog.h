/********************************************************************************
** Form generated from reading UI file 'configdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONFIGDIALOG_H
#define UI_CONFIGDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConfigDialog
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_9;
    QLineEdit *edt_scrollsize;
    QSpacerItem *horizontalSpacer_10;
    QHBoxLayout *horizontalLayout_5;
    QLineEdit *edt_filesize_warning;
    QLabel *label_7;
    QSpacerItem *horizontalSpacer_4;
    QHBoxLayout *horizontalLayout_4;
    QLineEdit *edt_lineheight;
    QLabel *label_4;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_6;
    QLineEdit *edt_filesize_max;
    QLabel *label_8;
    QSpacerItem *horizontalSpacer_5;
    QLabel *label_5;
    QLabel *label_9;
    QLabel *label;
    QLabel *label_6;
    QSpacerItem *verticalSpacer;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout_10;
    QLineEdit *edt_maxrecent;
    QSpacerItem *horizontalSpacer_12;
    QLabel *label_10;
    QHBoxLayout *horizontalLayout_3;
    QComboBox *cmb_fontfamily;
    QLineEdit *edt_fontsize;
    QLabel *label_3;
    QSpacerItem *horizontalSpacer_2;
    QLabel *label_11;
    QCheckBox *chk_inactsave;
    QWidget *tab_2;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout_4;
    QListWidget *lsttypes;
    QHBoxLayout *horizontalLayout_8;
    QPushButton *btnmoveupfiletype;
    QPushButton *btnmovedownfiletype;
    QSpacerItem *horizontalSpacer_9;
    QGridLayout *gridLayout_2;
    QPushButton *btnaddfiletype;
    QSpacerItem *horizontalSpacer_11;
    QPushButton *btndelfiletype;
    QVBoxLayout *verticalLayout_2;
    QGridLayout *gridLayout_3;
    QLabel *label_14;
    QLabel *label_15;
    QLabel *label_16;
    QLabel *label_17;
    QLineEdit *txttypename;
    QLineEdit *txtextension;
    QHBoxLayout *horizontalLayout_13;
    QLineEdit *txttabsize;
    QSpacerItem *horizontalSpacer_7;
    QHBoxLayout *horizontalLayout_14;
    QLineEdit *txtwrapsize;
    QCheckBox *chkautowrap;
    QSpacerItem *horizontalSpacer_8;
    QCheckBox *chkautoindent;
    QLabel *label_13;
    QTableWidget *lstattributes;
    QHBoxLayout *horizontalLayout_7;
    QPushButton *btnmoveupattr;
    QPushButton *btnmovedownattr;
    QPushButton *btnaddattr;
    QPushButton *btndelattr;
    QSpacerItem *horizontalSpacer_6;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *btnok;
    QPushButton *btncancel;
    QPushButton *btnapply;

    void setupUi(QDialog *ConfigDialog)
    {
        if (ConfigDialog->objectName().isEmpty())
            ConfigDialog->setObjectName(QString::fromUtf8("ConfigDialog"));
        ConfigDialog->resize(527, 595);
        verticalLayout = new QVBoxLayout(ConfigDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        tabWidget = new QTabWidget(ConfigDialog);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        gridLayout = new QGridLayout(tab);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QString::fromUtf8("horizontalLayout_9"));
        edt_scrollsize = new QLineEdit(tab);
        edt_scrollsize->setObjectName(QString::fromUtf8("edt_scrollsize"));
        edt_scrollsize->setMaximumSize(QSize(80, 16777215));

        horizontalLayout_9->addWidget(edt_scrollsize);

        horizontalSpacer_10 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_9->addItem(horizontalSpacer_10);


        gridLayout->addLayout(horizontalLayout_9, 4, 1, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        edt_filesize_warning = new QLineEdit(tab);
        edt_filesize_warning->setObjectName(QString::fromUtf8("edt_filesize_warning"));
        edt_filesize_warning->setMaximumSize(QSize(80, 16777215));

        horizontalLayout_5->addWidget(edt_filesize_warning);

        label_7 = new QLabel(tab);
        label_7->setObjectName(QString::fromUtf8("label_7"));

        horizontalLayout_5->addWidget(label_7);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_4);


        gridLayout->addLayout(horizontalLayout_5, 2, 1, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        edt_lineheight = new QLineEdit(tab);
        edt_lineheight->setObjectName(QString::fromUtf8("edt_lineheight"));
        edt_lineheight->setMaximumSize(QSize(40, 16777215));
        edt_lineheight->setMaxLength(2);

        horizontalLayout_4->addWidget(edt_lineheight);

        label_4 = new QLabel(tab);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout_4->addWidget(label_4);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);


        gridLayout->addLayout(horizontalLayout_4, 1, 1, 1, 1);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        edt_filesize_max = new QLineEdit(tab);
        edt_filesize_max->setObjectName(QString::fromUtf8("edt_filesize_max"));
        edt_filesize_max->setMaximumSize(QSize(80, 16777215));

        horizontalLayout_6->addWidget(edt_filesize_max);

        label_8 = new QLabel(tab);
        label_8->setObjectName(QString::fromUtf8("label_8"));

        horizontalLayout_6->addWidget(label_8);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_5);


        gridLayout->addLayout(horizontalLayout_6, 3, 1, 1, 1);

        label_5 = new QLabel(tab);
        label_5->setObjectName(QString::fromUtf8("label_5"));

        gridLayout->addWidget(label_5, 2, 0, 1, 1);

        label_9 = new QLabel(tab);
        label_9->setObjectName(QString::fromUtf8("label_9"));

        gridLayout->addWidget(label_9, 4, 0, 1, 1);

        label = new QLabel(tab);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        label_6 = new QLabel(tab);
        label_6->setObjectName(QString::fromUtf8("label_6"));

        gridLayout->addWidget(label_6, 3, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout->addItem(verticalSpacer, 9, 0, 1, 1);

        label_2 = new QLabel(tab);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QString::fromUtf8("horizontalLayout_10"));
        edt_maxrecent = new QLineEdit(tab);
        edt_maxrecent->setObjectName(QString::fromUtf8("edt_maxrecent"));
        edt_maxrecent->setMaximumSize(QSize(80, 16777215));

        horizontalLayout_10->addWidget(edt_maxrecent);

        horizontalSpacer_12 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_10->addItem(horizontalSpacer_12);


        gridLayout->addLayout(horizontalLayout_10, 5, 1, 1, 1);

        label_10 = new QLabel(tab);
        label_10->setObjectName(QString::fromUtf8("label_10"));

        gridLayout->addWidget(label_10, 5, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        cmb_fontfamily = new QComboBox(tab);
        cmb_fontfamily->setObjectName(QString::fromUtf8("cmb_fontfamily"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(cmb_fontfamily->sizePolicy().hasHeightForWidth());
        cmb_fontfamily->setSizePolicy(sizePolicy);

        horizontalLayout_3->addWidget(cmb_fontfamily);

        edt_fontsize = new QLineEdit(tab);
        edt_fontsize->setObjectName(QString::fromUtf8("edt_fontsize"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(edt_fontsize->sizePolicy().hasHeightForWidth());
        edt_fontsize->setSizePolicy(sizePolicy1);
        edt_fontsize->setMaximumSize(QSize(40, 16777215));
        edt_fontsize->setBaseSize(QSize(0, 0));
        edt_fontsize->setMaxLength(2);

        horizontalLayout_3->addWidget(edt_fontsize);

        label_3 = new QLabel(tab);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_3->addWidget(label_3);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        gridLayout->addLayout(horizontalLayout_3, 0, 1, 1, 1);

        label_11 = new QLabel(tab);
        label_11->setObjectName(QString::fromUtf8("label_11"));

        gridLayout->addWidget(label_11, 6, 0, 1, 1);

        chk_inactsave = new QCheckBox(tab);
        chk_inactsave->setObjectName(QString::fromUtf8("chk_inactsave"));

        gridLayout->addWidget(chk_inactsave, 6, 1, 1, 1);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        horizontalLayout_2 = new QHBoxLayout(tab_2);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        lsttypes = new QListWidget(tab_2);
        lsttypes->setObjectName(QString::fromUtf8("lsttypes"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(lsttypes->sizePolicy().hasHeightForWidth());
        lsttypes->setSizePolicy(sizePolicy2);
        lsttypes->setDragDropMode(QAbstractItemView::DragDrop);

        verticalLayout_4->addWidget(lsttypes);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QString::fromUtf8("horizontalLayout_8"));
        btnmoveupfiletype = new QPushButton(tab_2);
        btnmoveupfiletype->setObjectName(QString::fromUtf8("btnmoveupfiletype"));
        btnmoveupfiletype->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_8->addWidget(btnmoveupfiletype);

        btnmovedownfiletype = new QPushButton(tab_2);
        btnmovedownfiletype->setObjectName(QString::fromUtf8("btnmovedownfiletype"));
        btnmovedownfiletype->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_8->addWidget(btnmovedownfiletype);

        horizontalSpacer_9 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_8->addItem(horizontalSpacer_9);


        verticalLayout_4->addLayout(horizontalLayout_8);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        btnaddfiletype = new QPushButton(tab_2);
        btnaddfiletype->setObjectName(QString::fromUtf8("btnaddfiletype"));

        gridLayout_2->addWidget(btnaddfiletype, 0, 0, 1, 1);

        horizontalSpacer_11 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_2->addItem(horizontalSpacer_11, 0, 1, 1, 1);

        btndelfiletype = new QPushButton(tab_2);
        btndelfiletype->setObjectName(QString::fromUtf8("btndelfiletype"));

        gridLayout_2->addWidget(btndelfiletype, 1, 0, 1, 1);


        verticalLayout_4->addLayout(gridLayout_2);


        horizontalLayout_2->addLayout(verticalLayout_4);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        label_14 = new QLabel(tab_2);
        label_14->setObjectName(QString::fromUtf8("label_14"));

        gridLayout_3->addWidget(label_14, 0, 0, 1, 1);

        label_15 = new QLabel(tab_2);
        label_15->setObjectName(QString::fromUtf8("label_15"));

        gridLayout_3->addWidget(label_15, 1, 0, 1, 1);

        label_16 = new QLabel(tab_2);
        label_16->setObjectName(QString::fromUtf8("label_16"));

        gridLayout_3->addWidget(label_16, 2, 0, 1, 1);

        label_17 = new QLabel(tab_2);
        label_17->setObjectName(QString::fromUtf8("label_17"));

        gridLayout_3->addWidget(label_17, 3, 0, 1, 1);

        txttypename = new QLineEdit(tab_2);
        txttypename->setObjectName(QString::fromUtf8("txttypename"));

        gridLayout_3->addWidget(txttypename, 0, 1, 1, 1);

        txtextension = new QLineEdit(tab_2);
        txtextension->setObjectName(QString::fromUtf8("txtextension"));

        gridLayout_3->addWidget(txtextension, 1, 1, 1, 1);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName(QString::fromUtf8("horizontalLayout_13"));
        txttabsize = new QLineEdit(tab_2);
        txttabsize->setObjectName(QString::fromUtf8("txttabsize"));
        txttabsize->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_13->addWidget(txttabsize);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_13->addItem(horizontalSpacer_7);


        gridLayout_3->addLayout(horizontalLayout_13, 2, 1, 1, 1);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName(QString::fromUtf8("horizontalLayout_14"));
        txtwrapsize = new QLineEdit(tab_2);
        txtwrapsize->setObjectName(QString::fromUtf8("txtwrapsize"));
        txtwrapsize->setMaximumSize(QSize(80, 16777215));

        horizontalLayout_14->addWidget(txtwrapsize);

        chkautowrap = new QCheckBox(tab_2);
        chkautowrap->setObjectName(QString::fromUtf8("chkautowrap"));

        horizontalLayout_14->addWidget(chkautowrap);

        horizontalSpacer_8 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_14->addItem(horizontalSpacer_8);


        gridLayout_3->addLayout(horizontalLayout_14, 3, 1, 1, 1);

        chkautoindent = new QCheckBox(tab_2);
        chkautoindent->setObjectName(QString::fromUtf8("chkautoindent"));

        gridLayout_3->addWidget(chkautoindent, 4, 0, 1, 2);


        verticalLayout_2->addLayout(gridLayout_3);

        label_13 = new QLabel(tab_2);
        label_13->setObjectName(QString::fromUtf8("label_13"));

        verticalLayout_2->addWidget(label_13);

        lstattributes = new QTableWidget(tab_2);
        if (lstattributes->columnCount() < 8)
            lstattributes->setColumnCount(8);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        lstattributes->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        lstattributes->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        lstattributes->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        QTableWidgetItem *__qtablewidgetitem3 = new QTableWidgetItem();
        lstattributes->setHorizontalHeaderItem(3, __qtablewidgetitem3);
        QTableWidgetItem *__qtablewidgetitem4 = new QTableWidgetItem();
        lstattributes->setHorizontalHeaderItem(4, __qtablewidgetitem4);
        QTableWidgetItem *__qtablewidgetitem5 = new QTableWidgetItem();
        lstattributes->setHorizontalHeaderItem(5, __qtablewidgetitem5);
        QTableWidgetItem *__qtablewidgetitem6 = new QTableWidgetItem();
        lstattributes->setHorizontalHeaderItem(6, __qtablewidgetitem6);
        QTableWidgetItem *__qtablewidgetitem7 = new QTableWidgetItem();
        lstattributes->setHorizontalHeaderItem(7, __qtablewidgetitem7);
        lstattributes->setObjectName(QString::fromUtf8("lstattributes"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(lstattributes->sizePolicy().hasHeightForWidth());
        lstattributes->setSizePolicy(sizePolicy3);

        verticalLayout_2->addWidget(lstattributes);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));
        btnmoveupattr = new QPushButton(tab_2);
        btnmoveupattr->setObjectName(QString::fromUtf8("btnmoveupattr"));
        btnmoveupattr->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_7->addWidget(btnmoveupattr);

        btnmovedownattr = new QPushButton(tab_2);
        btnmovedownattr->setObjectName(QString::fromUtf8("btnmovedownattr"));
        btnmovedownattr->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_7->addWidget(btnmovedownattr);

        btnaddattr = new QPushButton(tab_2);
        btnaddattr->setObjectName(QString::fromUtf8("btnaddattr"));

        horizontalLayout_7->addWidget(btnaddattr);

        btndelattr = new QPushButton(tab_2);
        btndelattr->setObjectName(QString::fromUtf8("btndelattr"));

        horizontalLayout_7->addWidget(btndelattr);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_7->addItem(horizontalSpacer_6);


        verticalLayout_2->addLayout(horizontalLayout_7);

        verticalLayout_2->setStretch(2, 1);

        horizontalLayout_2->addLayout(verticalLayout_2);

        horizontalLayout_2->setStretch(0, 1);
        horizontalLayout_2->setStretch(1, 3);
        tabWidget->addTab(tab_2, QString());

        verticalLayout->addWidget(tabWidget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        btnok = new QPushButton(ConfigDialog);
        btnok->setObjectName(QString::fromUtf8("btnok"));

        horizontalLayout->addWidget(btnok);

        btncancel = new QPushButton(ConfigDialog);
        btncancel->setObjectName(QString::fromUtf8("btncancel"));

        horizontalLayout->addWidget(btncancel);

        btnapply = new QPushButton(ConfigDialog);
        btnapply->setObjectName(QString::fromUtf8("btnapply"));

        horizontalLayout->addWidget(btnapply);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(ConfigDialog);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(ConfigDialog);
    } // setupUi

    void retranslateUi(QDialog *ConfigDialog)
    {
        ConfigDialog->setWindowTitle(QApplication::translate("ConfigDialog", "Dialog", nullptr));
        label_7->setText(QApplication::translate("ConfigDialog", "MB", nullptr));
        label_4->setText(QApplication::translate("ConfigDialog", "px", nullptr));
        label_8->setText(QApplication::translate("ConfigDialog", "MB", nullptr));
        label_5->setText(QApplication::translate("ConfigDialog", "\343\203\225\343\202\241\343\202\244\343\203\253\343\202\265\343\202\244\343\202\272\350\255\246\345\221\212:", nullptr));
        label_9->setText(QApplication::translate("ConfigDialog", "\343\202\271\343\202\257\343\203\255\343\203\274\343\203\253\351\207\217:", nullptr));
        label->setText(QApplication::translate("ConfigDialog", "\343\203\225\343\202\251\343\203\263\343\203\210:", nullptr));
        label_6->setText(QApplication::translate("ConfigDialog", "<html><head/><body><p>\343\203\225\343\202\241\343\202\244\343\203\253\343\202\265\343\202\244\343\202\272\344\270\212\351\231\220:</p></body></html>", nullptr));
        label_2->setText(QApplication::translate("ConfigDialog", "\350\241\214\343\201\256\351\253\230\343\201\225:", nullptr));
        label_10->setText(QApplication::translate("ConfigDialog", "\346\234\200\350\277\221\343\201\256\343\203\225\343\202\241\343\202\244\343\203\253\346\234\200\345\244\247\346\225\260", nullptr));
        label_3->setText(QApplication::translate("ConfigDialog", "px", nullptr));
        label_11->setText(QApplication::translate("ConfigDialog", "\351\235\236\343\202\242\343\202\257\343\203\206\343\202\243\343\203\226\346\231\202\343\201\253\344\277\235\345\255\230", nullptr));
        chk_inactsave->setText(QApplication::translate("ConfigDialog", "CheckBox", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("ConfigDialog", "\345\205\261\351\200\232\350\250\255\345\256\232", nullptr));
        btnmoveupfiletype->setText(QApplication::translate("ConfigDialog", "\342\206\221", nullptr));
        btnmovedownfiletype->setText(QApplication::translate("ConfigDialog", "\342\206\223", nullptr));
        btnaddfiletype->setText(QApplication::translate("ConfigDialog", "\350\277\275\345\212\240", nullptr));
        btndelfiletype->setText(QApplication::translate("ConfigDialog", "\345\211\212\351\231\244", nullptr));
        label_14->setText(QApplication::translate("ConfigDialog", "\350\250\255\345\256\232\345\220\215", nullptr));
        label_15->setText(QApplication::translate("ConfigDialog", "\346\213\241\345\274\265\345\255\220", nullptr));
        label_16->setText(QApplication::translate("ConfigDialog", "\343\202\277\343\203\226\345\271\205", nullptr));
        label_17->setText(QApplication::translate("ConfigDialog", "\350\241\214\343\201\256\346\226\207\345\255\227\346\225\260", nullptr));
        chkautowrap->setText(QApplication::translate("ConfigDialog", "\350\207\252\345\213\225\346\212\230\343\202\212\350\277\224\343\201\227", nullptr));
        chkautoindent->setText(QApplication::translate("ConfigDialog", "\343\202\252\343\203\274\343\203\210\343\202\244\343\203\263\343\203\207\343\203\263\343\203\210", nullptr));
        label_13->setText(QApplication::translate("ConfigDialog", "\350\211\262\343\201\250\345\274\267\350\252\277\350\241\250\347\244\272", nullptr));
        QTableWidgetItem *___qtablewidgetitem = lstattributes->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("ConfigDialog", "\345\220\215\345\211\215", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = lstattributes->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("ConfigDialog", "\347\250\256\345\210\245", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = lstattributes->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QApplication::translate("ConfigDialog", "\346\226\207\345\255\227\345\210\227", nullptr));
        QTableWidgetItem *___qtablewidgetitem3 = lstattributes->horizontalHeaderItem(3);
        ___qtablewidgetitem3->setText(QApplication::translate("ConfigDialog", "\346\226\207\345\255\227\350\211\262", nullptr));
        QTableWidgetItem *___qtablewidgetitem4 = lstattributes->horizontalHeaderItem(4);
        ___qtablewidgetitem4->setText(QApplication::translate("ConfigDialog", "\350\203\214\346\231\257\350\211\262", nullptr));
        QTableWidgetItem *___qtablewidgetitem5 = lstattributes->horizontalHeaderItem(5);
        ___qtablewidgetitem5->setText(QApplication::translate("ConfigDialog", "B", nullptr));
        QTableWidgetItem *___qtablewidgetitem6 = lstattributes->horizontalHeaderItem(6);
        ___qtablewidgetitem6->setText(QApplication::translate("ConfigDialog", "I", nullptr));
        QTableWidgetItem *___qtablewidgetitem7 = lstattributes->horizontalHeaderItem(7);
        ___qtablewidgetitem7->setText(QApplication::translate("ConfigDialog", "U", nullptr));
        btnmoveupattr->setText(QApplication::translate("ConfigDialog", "\342\206\221", nullptr));
        btnmovedownattr->setText(QApplication::translate("ConfigDialog", "\342\206\223", nullptr));
        btnaddattr->setText(QApplication::translate("ConfigDialog", "\350\277\275\345\212\240", nullptr));
        btndelattr->setText(QApplication::translate("ConfigDialog", "\345\211\212\351\231\244", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("ConfigDialog", "\343\202\277\343\202\244\343\203\227\345\210\245\350\250\255\345\256\232", nullptr));
        btnok->setText(QApplication::translate("ConfigDialog", "OK", nullptr));
        btncancel->setText(QApplication::translate("ConfigDialog", "\343\202\255\343\203\243\343\203\263\343\202\273\343\203\253", nullptr));
        btnapply->setText(QApplication::translate("ConfigDialog", "\351\201\251\347\224\250", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ConfigDialog: public Ui_ConfigDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONFIGDIALOG_H
