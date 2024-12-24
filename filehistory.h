#ifndef FILEHISTORY_H
#define FILEHISTORY_H

#include <QVector>

struct FileHistoryInfo
{
    QString path;
    int scrollvalue;
    FileHistoryInfo(){
        scrollvalue = 0;
    }
};

class FileHistory
{
    QVector<FileHistoryInfo> histinfos;
    int maxsize;

public:
    FileHistory();
    void setMaximum(int max);
    void update(FileHistoryInfo& histinfo);
    void update(const QString &path, int scrollvalue);
    FileHistoryInfo find(const QString &path);
    int findIndex(const QString &path);

    friend class MainWindow;
};

#endif // FILEHISTORY_H
