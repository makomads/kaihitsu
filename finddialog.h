#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QButtonGroup>
#include <QDialog>

namespace Ui {
class FindDialog;
}



class FindDialog : public QDialog
{
    Q_OBJECT

protected:
    QRect savedgeom_find, savedgeom_replace;
    bool tofind;
    QButtonGroup* grpfindarea;

public:
    explicit FindDialog(QWidget *parent = 0);
    ~FindDialog();
    void show(bool tofind);

private:
    void closeEvent(QCloseEvent *e);

private slots:
    void on_btnclosefind_clicked();

private:
    Ui::FindDialog *ui;

    friend class MainWindow;
};



#endif // FINDDIALOG_H
