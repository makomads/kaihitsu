#ifndef ICTEXTCODEC_H
#define ICTEXTCODEC_H
#include <QString>
#include <QByteArray>
#include "iconv.h"

/*
 * Qt5までのQTextCodecをIconvに置き換えたもの
 * Qt6のQStringConverterはUnicode系しかサポートしないため
 */

class IcTextCodec
{
    QString codecname;
    iconv_t encoder, decoder;
protected:
    int convert(iconv_t codec, QByteArray& outbuf, const QByteArray &inbuf);

public:
    IcTextCodec();
    IcTextCodec(const char *name);
    ~IcTextCodec();
    QString toUnicode(const QByteArray &a);
    QByteArray fromUnicode(const QString &str);
    QString name();

    static IcTextCodec* codecForName(const char *name);
    static IcTextCodec* codecForLocale();
    static QList<QByteArray> availableCodecs();

    static QString detectEncoding(QByteArray &text, int hdsize); //非互換
};

#endif // ICTEXTCODEC_H
