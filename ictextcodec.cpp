#include <QStringList>
#include <QList>
#include <QByteArray>
#include "ictextcodec.h"

IcTextCodec *g_codec = NULL;

IcTextCodec::IcTextCodec()
{
    decoder = NULL;
    encoder = NULL;
}

IcTextCodec::IcTextCodec(const char *name)
{
    codecname = QString(name);
    decoder = iconv_open("UTF-8", name);
    encoder = iconv_open(name, "UTF-8");
}

IcTextCodec::~IcTextCodec()
{
    iconv_close(decoder);
    decoder = NULL;
    iconv_close(encoder);
    encoder = NULL;
}


int IcTextCodec::convert(iconv_t codec, QByteArray &outbuf, const QByteArray &inbuf)
{
    size_t inbufsize = inbuf.size();
    size_t outbufsize = inbufsize*2+4;
    size_t outbufleftlen = outbufsize;
    outbuf.resize(outbufsize);
    char *inptr = (char*)inbuf.data();
    char *outptr = outbuf.data();

    iconv(codec, &inptr, &inbufsize, &outptr, &outbufleftlen);
    //outbufleftlenは関数実行前はoutbufの長さ、実行後は残りの長さになる

    outbuf.truncate(outbufsize-outbufleftlen);
    return (int)outbuf.size();
}




QString IcTextCodec::toUnicode(const QByteArray &a)
{
    QByteArray outbuf;
    int len;
    len = convert(decoder, outbuf, a);
    return QString::fromUtf8(outbuf, len);
}

QByteArray IcTextCodec::fromUnicode(const QString &str)
{
    QByteArray outbuf;
    convert(encoder, outbuf, str.toUtf8());
    return outbuf;
}

QString IcTextCodec::name()
{
    return codecname;
}

IcTextCodec* IcTextCodec::codecForName(const char *name) //static
{
    if(g_codec!=NULL)
        delete g_codec;
    g_codec = new IcTextCodec(name);
    return g_codec;
}

IcTextCodec* IcTextCodec::codecForLocale() //static
{
    return codecForName("UTF-8");
}

QList<QByteArray> IcTextCodec::availableCodecs()
{
    QList<QByteArray> availables = {"UTF-8","Shift_JIS","EUC-JP",
                                    "UTF-16","UTF-16BE","UTF-16LE","UTF-32","UTF-32BE","UTF-32LE","ISO-8859-1"};
    return availables;
}

QString IcTextCodec::detectEncoding(QByteArray &text, int hdsize) //static
{
    QList<QByteArray> codecnames = IcTextCodec::availableCodecs();
    QString bestcodec;
    size_t inbufsize = hdsize<text.size()?hdsize:text.size();
    size_t outbufsize = inbufsize*2+4;
    //char *inbuf = new char[inbufsize];
    char *outbuf = new char[outbufsize];
    size_t nmaxerrors = 4;  //これよりエラー数の小さいものを候補とする

    for(auto i=0; i<codecnames.size(); i++){
        char *outptr = outbuf;
        char *inptr = text.data();
        size_t inlen = inbufsize;
        size_t outlen = outbufsize;
        iconv_t codec = iconv_open("UTF-8",codecnames[i]);
        auto nerrors = iconv(codec, &inptr, &inlen, &outptr, &outlen);
        iconv_close(codec);

        //末端のエラーはマルチバイト文字の切れ端の可能性があるのでエラーとみなさない
        if(inlen<3)
            nerrors = 0;
        if(nerrors < nmaxerrors){
            bestcodec = codecnames[i];
            nmaxerrors = nerrors;
        }
        if(nerrors==0){
            break;
        }
    }
    //delete [] inbuf;
    delete [] outbuf;

    return bestcodec;

}
