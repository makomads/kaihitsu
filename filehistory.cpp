#include "filehistory.h"


FileHistory::FileHistory()
{

}

void FileHistory::setMaximum(int max)
{
    maxsize = max;
    while(histinfos.size() > maxsize){
        histinfos.remove(histinfos.size()-1);
    }
}

void FileHistory::update(FileHistoryInfo &histinfo)
{
    int index = findIndex(histinfo.path);
    if(index>=0){
        histinfos.remove(index);
    }
    histinfos.insert(0, histinfo);

    if(histinfos.size() > maxsize){
        histinfos.remove(maxsize);
    }
}

void FileHistory::update(const QString &path, int scrollvalue)
{
    FileHistoryInfo histinfo;
    histinfo.path = path;
    int index = findIndex(path);
    if(index>=0){
        histinfo = histinfos[index];
    }
    if(scrollvalue>0)
        histinfo.scrollvalue = scrollvalue;
    update(histinfo);
}


FileHistoryInfo FileHistory::find(const QString &path)
{
    int index = findIndex(path);
    if(index>=0)
        return histinfos[index];
    return FileHistoryInfo();

}

int FileHistory::findIndex(const QString &path)
{
    for(int i=0; i<histinfos.size(); i++){
        if(histinfos[i].path == path)
            return i;
    }
    return -1;
}


