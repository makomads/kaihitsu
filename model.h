#ifndef __EDITOR_MODEL__
#define __EDITOR_MODEL__

#include "configdialog.h"
#include "editorengine.h"

#include <QVariant>
#include <QObject>
#include <QString>
#include "ictextcodec.h"    //<QTextCodec>

#define AUTOSIGNATURE "#AUTO"



/*
	エディタのデータを扱うクラス。
	内部でのデータ処理はEditorEngineに任せ、
	このクラス自身は入出力を行うのみ。
*/
class Model: public EditorEngine
{
protected:
    QString filepath;
    QString filecodecname;
    QString filelineend;
    FileTypeConfig *filetypeconf;

public:
    Model();
    int load(QString codecname=AUTOSIGNATURE, QString lineend=AUTOSIGNATURE);
    int save(QString codecname=AUTOSIGNATURE, QString lineend=AUTOSIGNATURE);
    void applyConfig(ConfigData *configdata);
    FileTypeConfig* getFileTypeConfig(){return filetypeconf;}

    QString filePath(){return filepath;}
    void setFilePath(QString path){filepath = path;}
    QString lineEnd(){return filelineend;}
    void setLineEnd(QString le){filelineend=le;}
    QString textCodecName(){return filecodecname;}
    void setTextCodecName(QString name){filecodecname=name;}

    static QString escape(const QString& str);
    static QString unescape(const QString& str);
};


//QVariantにポインタを登録するために必要
Q_DECLARE_METATYPE(Model*)


#endif
