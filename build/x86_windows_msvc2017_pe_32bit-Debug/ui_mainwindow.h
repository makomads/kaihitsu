/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.12
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "editorview.h"
#include "mainwindow.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionNew_File;
    QAction *actionOpen;
    QAction *actionSave;
    QAction *actionSaveas;
    QAction *actionConfig;
    QAction *actionExit;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionCut;
    QAction *actionCopy;
    QAction *actionPaste;
    QAction *actionDelete;
    QAction *actionWebpage;
    QAction *actionVersion;
    QAction *actionFind;
    QAction *actionFindnext;
    QAction *actionReplace;
    QAction *actionSaveall;
    QAction *actionClose;
    QAction *actionCloseall;
    QAction *actionSelectall;
    QAction *actionFindprev;
    QAction *actionTextencoding_input;
    QAction *actionTextencoding_output;
    QAction *actionShowLinenumber;
    QAction *actionShowRuler;
    QAction *actionShowToolbar;
    QAction *actionShowStatusbar;
    QAction *actionDummy;
    QAction *actionTextencoding_reset;
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    TabBar *tabbar;
    QPushButton *btntabclose;
    QGridLayout *gridLayout_2;
    QScrollBar *scrEditorViewV;
    EditorView *editorview;
    QScrollBar *scrEditorViewH;
    QTextEdit *debugedit;
    QMenuBar *menuBar;
    QMenu *menu;
    QMenu *menu_2;
    QMenu *menuRecent;
    QMenu *menuEdit;
    QMenu *menuHelp;
    QMenu *menu_3;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(408, 376);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        actionNew_File = new QAction(MainWindow);
        actionNew_File->setObjectName(QString::fromUtf8("actionNew_File"));
        actionNew_File->setIconText(QString::fromUtf8("filenew"));
        actionOpen = new QAction(MainWindow);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionOpen->setIconText(QString::fromUtf8("open"));
        actionSave = new QAction(MainWindow);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        actionSaveas = new QAction(MainWindow);
        actionSaveas->setObjectName(QString::fromUtf8("actionSaveas"));
        actionConfig = new QAction(MainWindow);
        actionConfig->setObjectName(QString::fromUtf8("actionConfig"));
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionUndo = new QAction(MainWindow);
        actionUndo->setObjectName(QString::fromUtf8("actionUndo"));
        actionRedo = new QAction(MainWindow);
        actionRedo->setObjectName(QString::fromUtf8("actionRedo"));
        actionCut = new QAction(MainWindow);
        actionCut->setObjectName(QString::fromUtf8("actionCut"));
        actionCopy = new QAction(MainWindow);
        actionCopy->setObjectName(QString::fromUtf8("actionCopy"));
        actionPaste = new QAction(MainWindow);
        actionPaste->setObjectName(QString::fromUtf8("actionPaste"));
        actionDelete = new QAction(MainWindow);
        actionDelete->setObjectName(QString::fromUtf8("actionDelete"));
        actionWebpage = new QAction(MainWindow);
        actionWebpage->setObjectName(QString::fromUtf8("actionWebpage"));
        actionVersion = new QAction(MainWindow);
        actionVersion->setObjectName(QString::fromUtf8("actionVersion"));
        actionFind = new QAction(MainWindow);
        actionFind->setObjectName(QString::fromUtf8("actionFind"));
        actionFindnext = new QAction(MainWindow);
        actionFindnext->setObjectName(QString::fromUtf8("actionFindnext"));
        actionReplace = new QAction(MainWindow);
        actionReplace->setObjectName(QString::fromUtf8("actionReplace"));
        actionSaveall = new QAction(MainWindow);
        actionSaveall->setObjectName(QString::fromUtf8("actionSaveall"));
        actionClose = new QAction(MainWindow);
        actionClose->setObjectName(QString::fromUtf8("actionClose"));
        actionCloseall = new QAction(MainWindow);
        actionCloseall->setObjectName(QString::fromUtf8("actionCloseall"));
        actionSelectall = new QAction(MainWindow);
        actionSelectall->setObjectName(QString::fromUtf8("actionSelectall"));
        actionFindprev = new QAction(MainWindow);
        actionFindprev->setObjectName(QString::fromUtf8("actionFindprev"));
        actionTextencoding_input = new QAction(MainWindow);
        actionTextencoding_input->setObjectName(QString::fromUtf8("actionTextencoding_input"));
        actionTextencoding_output = new QAction(MainWindow);
        actionTextencoding_output->setObjectName(QString::fromUtf8("actionTextencoding_output"));
        actionShowLinenumber = new QAction(MainWindow);
        actionShowLinenumber->setObjectName(QString::fromUtf8("actionShowLinenumber"));
        actionShowLinenumber->setCheckable(true);
        actionShowRuler = new QAction(MainWindow);
        actionShowRuler->setObjectName(QString::fromUtf8("actionShowRuler"));
        actionShowRuler->setCheckable(true);
        actionShowToolbar = new QAction(MainWindow);
        actionShowToolbar->setObjectName(QString::fromUtf8("actionShowToolbar"));
        actionShowToolbar->setCheckable(true);
        actionShowStatusbar = new QAction(MainWindow);
        actionShowStatusbar->setObjectName(QString::fromUtf8("actionShowStatusbar"));
        actionShowStatusbar->setCheckable(true);
        actionDummy = new QAction(MainWindow);
        actionDummy->setObjectName(QString::fromUtf8("actionDummy"));
        actionTextencoding_reset = new QAction(MainWindow);
        actionTextencoding_reset->setObjectName(QString::fromUtf8("actionTextencoding_reset"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy1);
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 1, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        tabbar = new TabBar(centralWidget);
        tabbar->setObjectName(QString::fromUtf8("tabbar"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(tabbar->sizePolicy().hasHeightForWidth());
        tabbar->setSizePolicy(sizePolicy2);
        tabbar->setMaximumSize(QSize(16777215, 40));

        horizontalLayout->addWidget(tabbar);

        btntabclose = new QPushButton(centralWidget);
        btntabclose->setObjectName(QString::fromUtf8("btntabclose"));
        QSizePolicy sizePolicy3(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(btntabclose->sizePolicy().hasHeightForWidth());
        btntabclose->setSizePolicy(sizePolicy3);
        btntabclose->setMaximumSize(QSize(32, 32));

        horizontalLayout->addWidget(btntabclose);


        verticalLayout->addLayout(horizontalLayout);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(0);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        scrEditorViewV = new QScrollBar(centralWidget);
        scrEditorViewV->setObjectName(QString::fromUtf8("scrEditorViewV"));
        scrEditorViewV->setOrientation(Qt::Vertical);

        gridLayout_2->addWidget(scrEditorViewV, 0, 1, 1, 1);

        editorview = new EditorView(centralWidget);
        editorview->setObjectName(QString::fromUtf8("editorview"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(editorview->sizePolicy().hasHeightForWidth());
        editorview->setSizePolicy(sizePolicy4);

        gridLayout_2->addWidget(editorview, 0, 0, 1, 1);

        scrEditorViewH = new QScrollBar(centralWidget);
        scrEditorViewH->setObjectName(QString::fromUtf8("scrEditorViewH"));
        scrEditorViewH->setOrientation(Qt::Horizontal);

        gridLayout_2->addWidget(scrEditorViewH, 1, 0, 1, 1);


        verticalLayout->addLayout(gridLayout_2);

        debugedit = new QTextEdit(centralWidget);
        debugedit->setObjectName(QString::fromUtf8("debugedit"));
        debugedit->setMaximumSize(QSize(16777215, 70));

        verticalLayout->addWidget(debugedit);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 408, 21));
        menu = new QMenu(menuBar);
        menu->setObjectName(QString::fromUtf8("menu"));
        menu_2 = new QMenu(menu);
        menu_2->setObjectName(QString::fromUtf8("menu_2"));
        menuRecent = new QMenu(menu);
        menuRecent->setObjectName(QString::fromUtf8("menuRecent"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        menu_3 = new QMenu(menuBar);
        menu_3->setObjectName(QString::fromUtf8("menu_3"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menu_3->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menu->addAction(actionNew_File);
        menu->addAction(actionOpen);
        menu->addAction(actionSave);
        menu->addAction(actionSaveas);
        menu->addAction(actionSaveall);
        menu->addSeparator();
        menu->addAction(actionClose);
        menu->addAction(actionCloseall);
        menu->addSeparator();
        menu->addAction(menuRecent->menuAction());
        menu->addAction(actionConfig);
        menu->addAction(menu_2->menuAction());
        menu->addAction(actionExit);
        menu_2->addAction(actionTextencoding_reset);
        menu_2->addAction(actionTextencoding_input);
        menu_2->addAction(actionTextencoding_output);
        menuRecent->addAction(actionDummy);
        menuEdit->addAction(actionUndo);
        menuEdit->addAction(actionRedo);
        menuEdit->addSeparator();
        menuEdit->addAction(actionCut);
        menuEdit->addAction(actionCopy);
        menuEdit->addAction(actionPaste);
        menuEdit->addSeparator();
        menuEdit->addAction(actionFind);
        menuEdit->addAction(actionFindprev);
        menuEdit->addAction(actionFindnext);
        menuEdit->addAction(actionReplace);
        menuEdit->addSeparator();
        menuEdit->addAction(actionSelectall);
        menuHelp->addAction(actionWebpage);
        menuHelp->addAction(actionVersion);
        menu_3->addAction(actionShowLinenumber);
        menu_3->addAction(actionShowRuler);
        menu_3->addAction(actionShowToolbar);
        menu_3->addAction(actionShowStatusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", nullptr));
        actionNew_File->setText(QApplication::translate("MainWindow", "\346\226\260\350\246\217\344\275\234\346\210\220(&N)", nullptr));
#ifndef QT_NO_SHORTCUT
        actionNew_File->setShortcut(QApplication::translate("MainWindow", "Ctrl+N", nullptr));
#endif // QT_NO_SHORTCUT
        actionOpen->setText(QApplication::translate("MainWindow", "\351\226\213\343\201\217(&O)", nullptr));
#ifndef QT_NO_SHORTCUT
        actionOpen->setShortcut(QApplication::translate("MainWindow", "Ctrl+O", nullptr));
#endif // QT_NO_SHORTCUT
        actionSave->setText(QApplication::translate("MainWindow", "\344\270\212\346\233\270\343\201\215\344\277\235\345\255\230(&S)", nullptr));
#ifndef QT_NO_SHORTCUT
        actionSave->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", nullptr));
#endif // QT_NO_SHORTCUT
        actionSaveas->setText(QApplication::translate("MainWindow", "\345\220\215\345\211\215\343\202\222\344\273\230\343\201\221\343\201\246\344\277\235\345\255\230(&A)", nullptr));
        actionConfig->setText(QApplication::translate("MainWindow", "\350\250\255\345\256\232(&C)", nullptr));
        actionExit->setText(QApplication::translate("MainWindow", "\347\265\202\344\272\206(&X)", nullptr));
        actionUndo->setText(QApplication::translate("MainWindow", "\345\205\203\343\201\253\346\210\273\343\201\231(&U)", nullptr));
#ifndef QT_NO_STATUSTIP
        actionUndo->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_SHORTCUT
        actionUndo->setShortcut(QApplication::translate("MainWindow", "Ctrl+Z", nullptr));
#endif // QT_NO_SHORTCUT
        actionRedo->setText(QApplication::translate("MainWindow", "\343\202\204\343\202\212\347\233\264\343\201\231(&R)", nullptr));
#ifndef QT_NO_SHORTCUT
        actionRedo->setShortcut(QApplication::translate("MainWindow", "Ctrl+Y", nullptr));
#endif // QT_NO_SHORTCUT
        actionCut->setText(QApplication::translate("MainWindow", "\345\210\207\343\202\212\345\217\226\343\202\212(&T)", nullptr));
#ifndef QT_NO_STATUSTIP
        actionCut->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_SHORTCUT
        actionCut->setShortcut(QApplication::translate("MainWindow", "Ctrl+X", nullptr));
#endif // QT_NO_SHORTCUT
        actionCopy->setText(QApplication::translate("MainWindow", "\343\202\263\343\203\224\343\203\274(&C)", nullptr));
#ifndef QT_NO_SHORTCUT
        actionCopy->setShortcut(QApplication::translate("MainWindow", "Ctrl+C", nullptr));
#endif // QT_NO_SHORTCUT
        actionPaste->setText(QApplication::translate("MainWindow", "\350\262\274\343\202\212\344\273\230\343\201\221(&P)", nullptr));
#ifndef QT_NO_SHORTCUT
        actionPaste->setShortcut(QApplication::translate("MainWindow", "Ctrl+V", nullptr));
#endif // QT_NO_SHORTCUT
        actionDelete->setText(QApplication::translate("MainWindow", "\345\211\212\351\231\244(&L)", nullptr));
        actionWebpage->setText(QApplication::translate("MainWindow", "\343\202\246\343\202\247\343\203\226\343\203\232\343\203\274\343\202\270(&W)", nullptr));
        actionVersion->setText(QApplication::translate("MainWindow", "\343\203\220\343\203\274\343\202\270\343\203\247\343\203\263\346\203\205\345\240\261(&A)", nullptr));
        actionFind->setText(QApplication::translate("MainWindow", "\346\244\234\347\264\242(&F)", nullptr));
#ifndef QT_NO_SHORTCUT
        actionFind->setShortcut(QApplication::translate("MainWindow", "Ctrl+F", nullptr));
#endif // QT_NO_SHORTCUT
        actionFindnext->setText(QApplication::translate("MainWindow", "\346\254\241\343\202\222\346\244\234\347\264\242(&N)", nullptr));
#ifndef QT_NO_SHORTCUT
        actionFindnext->setShortcut(QApplication::translate("MainWindow", "F3", nullptr));
#endif // QT_NO_SHORTCUT
        actionReplace->setText(QApplication::translate("MainWindow", "\347\275\256\346\217\233(&R)", nullptr));
#ifndef QT_NO_SHORTCUT
        actionReplace->setShortcut(QApplication::translate("MainWindow", "Ctrl+R", nullptr));
#endif // QT_NO_SHORTCUT
        actionSaveall->setText(QApplication::translate("MainWindow", "\343\201\231\343\201\271\343\201\246\344\277\235\345\255\230", nullptr));
        actionClose->setText(QApplication::translate("MainWindow", "\351\226\211\343\201\230\343\202\213", nullptr));
        actionCloseall->setText(QApplication::translate("MainWindow", "\343\201\231\343\201\271\343\201\246\351\226\211\343\201\230\343\202\213", nullptr));
        actionSelectall->setText(QApplication::translate("MainWindow", "\343\201\231\343\201\271\343\201\246\351\201\270\346\212\236", nullptr));
#ifndef QT_NO_SHORTCUT
        actionSelectall->setShortcut(QApplication::translate("MainWindow", "Ctrl+A", nullptr));
#endif // QT_NO_SHORTCUT
        actionFindprev->setText(QApplication::translate("MainWindow", "\345\211\215\343\202\222\346\244\234\347\264\242", nullptr));
#ifndef QT_NO_SHORTCUT
        actionFindprev->setShortcut(QApplication::translate("MainWindow", "Shift+F3", nullptr));
#endif // QT_NO_SHORTCUT
        actionTextencoding_input->setText(QApplication::translate("MainWindow", "\345\205\245\345\212\233", nullptr));
        actionTextencoding_output->setText(QApplication::translate("MainWindow", "\345\207\272\345\212\233", nullptr));
        actionShowLinenumber->setText(QApplication::translate("MainWindow", "\350\241\214\347\225\252\345\217\267", nullptr));
        actionShowRuler->setText(QApplication::translate("MainWindow", "\343\203\253\343\203\274\343\203\251\343\203\274", nullptr));
        actionShowToolbar->setText(QApplication::translate("MainWindow", "\343\203\204\343\203\274\343\203\253\343\203\220\343\203\274", nullptr));
        actionShowStatusbar->setText(QApplication::translate("MainWindow", "\343\202\271\343\203\206\343\203\274\343\202\277\343\202\271\343\203\220\343\203\274", nullptr));
        actionDummy->setText(QApplication::translate("MainWindow", "dummy", nullptr));
        actionTextencoding_reset->setText(QApplication::translate("MainWindow", "\343\203\252\343\202\273\343\203\203\343\203\210(\343\201\231\343\201\271\343\201\246\350\207\252\345\213\225)", nullptr));
        btntabclose->setText(QApplication::translate("MainWindow", "x", nullptr));
        menu->setTitle(QApplication::translate("MainWindow", "\343\203\225\343\202\241\343\202\244\343\203\253(&F)", nullptr));
        menu_2->setTitle(QApplication::translate("MainWindow", "\346\226\207\345\255\227\343\202\263\343\203\274\343\203\211\343\201\250\346\224\271\350\241\214", nullptr));
        menuRecent->setTitle(QApplication::translate("MainWindow", "\346\234\200\350\277\221\343\201\256\343\203\225\343\202\241\343\202\244\343\203\253", nullptr));
        menuEdit->setTitle(QApplication::translate("MainWindow", "\347\267\250\351\233\206(&E)", nullptr));
        menuHelp->setTitle(QApplication::translate("MainWindow", "\343\203\230\343\203\253\343\203\227(&H)", nullptr));
        menu_3->setTitle(QApplication::translate("MainWindow", "\350\241\250\347\244\272(&V)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
