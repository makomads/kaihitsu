#ifndef COMBOBOXDIALOG_H
#define COMBOBOXDIALOG_H

#include <QDialog>
#include <QVector>
#include <QList>
#include <QPair>
#include <QComboBox>
#include <QLabel>


namespace Ui {
class ComboboxDialog;
}

typedef QPair<QString,QVariant> ComboboxDialogDataPair;
typedef QList<ComboboxDialogDataPair> ComboboxDialogData;

class ComboboxDialog : public QDialog
{
    Q_OBJECT
    QVector<QComboBox*> comboboxes;
    QVector<QLabel*> labels;

public:
    explicit ComboboxDialog(QWidget *parent = 0);
    ~ComboboxDialog();
    void createComboBoxes(int count);

    void setCurrentByValue(int index, QVariant value);

    void setListData(int index, const ComboboxDialogData& listdata);
    void addListData(int index, const QString& name, const QVariant& value);
    void setListLabel(int index, const QString& label);
    void clearListData(int index);

    QString selectedDataName(int index);
    QVariant selectedDataValue(int index);

private:
    Ui::ComboboxDialog *ui;
};

#endif // COMBOBOXDIALOG_H
