#ifndef MainWindow_H
#define MainWindow_H

#include <QMainWindow>
#include <QToolBar>
#include <QTabBar>
#include "configdialog.h"
#include "editorview.h"
#include "finddialog.h"
#include "model.h"
#include "filehistory.h"

#ifdef WIN32
#include <QSettings>
#include <windows.h>
#endif

namespace Ui {
class MainWindow;
}

#define NEWFILENAME "NewFile"


class TabBar: public QTabBar
{
    Q_OBJECT

public:
    TabBar(QWidget *parent);
    QRect tabRect ( int index ) const;

protected:

    QSize tabSizeHint(int index) const; //override
};



class MainWindow: public QMainWindow
{
    Q_OBJECT
protected:
    QString settingspath;
    QString cwtablepath;
    EditorView *editorview;
    QVector<Model*> models;
    QString lastdir;
    ConfigData conf, defconf;
    FindDialog *dlgfind;
    QRect dlgconfigrect;
    QString dlgconfig_attrcolwidths;
    QMenu *popupmenu;
    FileHistory filehistory;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    ConfigData createDefaultConfig();
    void onModelModified(EditorEngine *sender);
    void readSettings();
    void writeSettings();
    void applyConfig();
    void closeEvent(QCloseEvent *e);
    void resizeEvent(QResizeEvent *e);
    void showEvent(QShowEvent *e);
    void firstShowEvent();
    Model* activeModel();
    FileTypeConfig findFileTypeByExtention(const QString& ext);
    void setTextEncodingMenu(QAction *action, QString menutext, QString codecname, QString lineend);
    void updateMenuRecent();

//メニュースロット
//このクラスが扱うのはファイル関係のみで、
//あとは各モジュールに任せる
public slots:
    void onTriggerMenu(QAction *act);
    void onCurrentChanged(int index);
    void onEditorViewEntered(QEvent *e);
    void onEditorViewDropped(QDropEvent *e);

    void fileNew();
    void openFile();
    void openFile(QString fpath);
    void save();
    void saveAs();
    void saveAll();
    void close();


    void showFindDialog();
    void findNext();
    void findPrevious();
    void showReplaceDialog();
    void replace();
    void replaceAll();

    void on_btntabclose_clicked();
    void on_editorview_mouseRightButtonPressed(QMouseEvent* e);

private:
    Ui::MainWindow *ui;
    friend class Application;
};




#endif // MainWindow_H
