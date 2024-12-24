#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include <QTableWidgetItem>
#include "editorengine.h"


namespace Ui {
class ConfigDialog;
}

struct FileTypeConfig
{
    QString name;
    QStringList exts;
    int tabsize;
    int wrapsize;
    bool autowrap;
    bool autoindent;
    bool focusoutsave;
    EditorAttributeSet attrset;
};


//呼び出し元とデータのやりとりするのに使う構造体
//この構造体内のメンバにはポインタを含めないこと
struct ConfigData
{
    int fontsize;
    int lineheight;
    QString fontfamily;
    int warningfilesizemb;
    int maxfilesizemb;
    int scrollsize;
    QVector<FileTypeConfig> filetypes;
    int maxrecent;
};

class ConfigDialog : public QDialog
{
    Q_OBJECT
protected:
    ConfigData data, defdata;

public:
    explicit ConfigDialog(QWidget *parent = 0);
    ~ConfigDialog();
    int exec(const ConfigData& configdata);
    ConfigData configData(){return data;}

protected:
    void swapQTableWidgetRows(int row1, int row2);
    void fileTypePageToData(QListWidgetItem* listitem);
    void dataToFiletypePage(QListWidgetItem* listitem);


private:
    void closeEvent(QCloseEvent *e);

private slots:
    void on_btnok_clicked();
    void on_btncancel_clicked();
    void on_btnapply_clicked();

    void on_lsttypes_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_btnmoveupfiletype_clicked();
    void on_btnmovedownfiletype_clicked();
    void on_btnaddfiletype_clicked();
    void on_btndelfiletype_clicked();

    void on_lstattributes_itemChanged(QTableWidgetItem *item);
    void on_btnaddattr_clicked();
    void on_btndelattr_clicked();
    void on_btnmoveupattr_clicked();
    void on_btnmovedownattr_clicked();

private:
    Ui::ConfigDialog *ui;

    friend class MainWindow;
};


//QVariantに登録
Q_DECLARE_METATYPE(FileTypeConfig*)

#endif // CONFIGDIALOG_H
