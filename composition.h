#ifndef __COMPO___
#define __COMPO___

#include <QFontMetrics>
#include <QVector>
class QWidget;
class QTextFormat;
class QInputMethodEvent;
class QPainter;
class QRect;
class QPoint;


/*
	変換中の文字列を管理・表示する
*/


class Composition
{
	QWidget *owner;
	QRect rect;
	QPoint curpos;
	int lineheight;
	QVector<QTextFormat> formats;
    QVector<QString> lines; //変換中に折り返しの場合もあるので複数行保持する
	QString str;
    int compocurpos;
    QColor compocursorcolor;
    QFontMetrics* fontmet;
    QFont font;

public:
    Composition(QWidget *w);
    void setFont(QFont &f, QFontMetrics *fontmet);
	void setMicroFocusHint(QRect rc, QPoint pt, int lineheight);
    void setCursor(int pos, QColor color);
	void inputMethodEvent(QInputMethodEvent *e);
	void draw(QPainter *p);
	QRect boundingRect();
    QPoint candidatePosition();
};







#endif


