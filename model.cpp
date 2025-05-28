#include <stdio.h>
#include <QByteArray>
//#include <QTextCodec>
#include <QFile>
#include <QFileInfo>
#include "model.h"
#include "ictextcodec.h"





Model::Model()
    :EditorEngine()
{
    filecodecname = "UTF-8";
    filelineend = "\n";
    filetypeconf = NULL;
}

int Model::load(QString codecname, QString lineend)
{
    QFile file(filepath);
    QByteArray data;
    IcTextCodec* codec;
    QString textdata;

    Clear();

    if(!file.open(QIODevice::ReadOnly))
        return -1;
    data = file.readAll();
    file.close();

    //コーデック
    if(codecname==AUTOSIGNATURE)
        codecname = IcTextCodec::detectEncoding(data, 1000);
    codec = IcTextCodec::codecForName(codecname.toLatin1());
    filecodecname = codec->name();
    textdata = codec->toUnicode(data);

    //改行コード
    textdata = textdata.replace("\r\n", "\x1A");    //\r\nを制御文字SUBで置き換える
    if(lineend == AUTOSIGNATURE){
        filelineend = "\n";
        if(textdata.count("\x1A") > textdata.count("\n"))
            filelineend = "\r\n";
        else if(textdata.count("\r") > textdata.count("\n"))
            filelineend = "\r";
    }
    else{
        filelineend = lineend;
    }
    //内部の処理では\nで統一する
    textdata = textdata.replace("\x1A","\n");
    textdata = textdata.replace("\r","\n");

    //連結リストへ挿入
    InsertFast(textdata.toStdWString());

    SetModified(false);
    return textdata.size();
}


int Model::save(QString codecname, QString lineend)
{
    int readlen;
    int bufsize = 1000;
    wchar_t buf[1000];
    QString text;
    IcTextCodec* codec;
    QByteArray data;
    QFile file(filepath);

    if(!file.open(QIODevice::WriteOnly))
        return -1;

    //改行コード準備
    if(lineend != AUTOSIGNATURE)
        filelineend = lineend;
    //文字コード準備
    if(codecname != AUTOSIGNATURE)
        filecodecname= codecname;
    codec = IcTextCodec::codecForName(filecodecname.toLatin1());

    //書き込み
    ResetOutputStream();
    while(true){
        readlen = ReadStream(buf, bufsize);
        if(readlen < 0)
            break;
        text = QString::fromWCharArray(buf, readlen);

        //改行コード変換
        if(filelineend!="\n")
            text = text.replace("\n", filelineend);

        //文字コード変換
        data = codec->fromUnicode(text);
        file.write(data);
    }

    SetModified(false);
    return text.size();
}

//コンフィグを適用する
//(ビューパラメータの反映、およびモデルからビューへの設定はアタッチの際適用される)
void Model::applyConfig(ConfigData *configdata)
{
    QString text;
    Paragraph *prg;
    QFileInfo fileinfo(filepath);

    //拡張子で適用するコンフィグを選ぶ
    filetypeconf = &configdata->filetypes[0];   //まず要素0番のデフォルトコンフィグを入れてnullを防ぐ
    for(int i=1; i<configdata->filetypes.size(); i++){
        for(QStringList::iterator it=configdata->filetypes[i].exts.begin();
            it!=configdata->filetypes[i].exts.end(); it++){
            if(*it == fileinfo.completeSuffix()){
                filetypeconf = &configdata->filetypes[i];
                break;
            }
        }
    }

    AllowDrawing(false);

    //属性再構築のため文字だけ抜き出してクリアする
    text.reserve(GetCharacterCount());
    for(prg=rootprg->next; prg!=terminalprg; prg=prg->next){
        QString prgtext;
        if(prg->next == terminalprg) //EOFを除外
            prgtext = QString::fromStdWString(prg->text.substr(0,prg->text.length()-1));
        else
            prgtext = QString::fromStdWString(prg->text);
        text.append(prgtext);
    }
    Clear();

    //属性適用
    SetScrollSize(configdata->scrollsize);
    SetAttributeSet(filetypeconf->attrset);
    InsertFast(text.toStdWString());
    AllowDrawing(true);
}


QString Model::escape(const QString &str) //static
{
    QString ret;
    QChar ch;
    for(int i=0; i<str.length(); i++){
        ch = str.at(i);
        if(ch=='\\')
            ret.append("\\");
        else if(ch=='\n')
            ret.append("\\n");
        else if(ch=='\t')
            ret.append("\\t");
        else
            ret.append(ch);
    }
    return ret;
}

QString Model::unescape(const QString &str) //static
{
    QString ret;
    bool escaped = false;
    QChar ch;
    for(int i=0; i<str.length(); i++){
        ch = str.at(i);
        if(ch=='\\'){
            escaped = true;
            continue;
        }

        if(escaped){
            if(ch=='n')
                ret.append('\n');
            else if(ch=='t')
                ret.append('\t');
            else if(ch=='\\')
                ret.append('\\');
            else
                ret.append(ch);
            escaped = false;
        }
        else
            ret.append(ch);
    }
    return ret;
}

