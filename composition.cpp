#include <QInputMethodEvent>
#include <QPainter>
#include <QRect>
#include <QPoint>
#include <QTextFormat>
#include <QFontMetrics>
#include <QWidget>
#include "composition.h"


Composition::Composition(QWidget* w)
{
    owner = w;
    fontmet = NULL;
}


void Composition::setFont(QFont &f, QFontMetrics *metrics)
{
    font = f;
    fontmet = metrics;
}

void Composition::setMicroFocusHint(QRect rc, QPoint pt, int lh)
{
	curpos = pt;
	rect = rc;
    lineheight = lh;
}

void Composition::setCursor(int pos, QColor color)
{
    compocurpos = pos;
    compocursorcolor = color;
}



void Composition::inputMethodEvent(QInputMethodEvent *e)
{
	QList<QInputMethodEvent::Attribute> attrs = e->attributes();
	int i, j, x, w;
	QString str, line;
    QChar c;

	formats.clear();
	lines.clear();

    str = e->preeditString();
	if(str=="") return;

    //IMイベントの属性別処理
	formats.resize(str.length());
	for(i=0; i<attrs.size(); i++){
		const QInputMethodEvent::Attribute &attr = attrs.at(i);

        //あとで描画しやすいように文字属性を文字に割り当てる
		if(attr.type==QInputMethodEvent::TextFormat){
			QTextFormat f = qvariant_cast<QTextFormat>(attr.value);
			if(attr.start + attr.length > formats.size()) continue;
			for(j=attr.start; j<attr.start+attr.length; j++){
				formats[j] = f;
			}
		}

        //IM内カーソル位置
        else if(attr.type == QInputMethodEvent::Cursor){
            if(attr.length == 0){
                setCursor(-1, QColor(0,0,0));
            }
            else{
                if(attr.value.type() == QVariant::Color){
                    setCursor(attr.start, QColor(attr.value.toString()));
                }
                else{
                    setCursor(attr.start, QColor(0,0,0));
                }
            }
        }
	}
	
	//文字列を行ごとに振り分ける
	x = curpos.x();
	for(i=0; i<str.length(); i++){
		c = str.at(i);
        w = fontmet->width(c);
		x += w;
		if(x > rect.right()){
			lines.append(line);
			line.clear();
			x = rect.x();
		}
		line.append(c);
	}
	if(line.length()!=0)
		lines.append(line);
}



QRect Composition::boundingRect()
{
    //行末折り返しを考慮してカーソル位置より1行上を矩形上端とする
    //また全確定時にlines.size()==0となった後このメソッドが呼ばれることを考慮して
    //カーソル位置より1行下を矩形下端とする
    return QRect(0, curpos.y()-lineheight, owner->width(), lineheight*(lines.size()+2));
}

QPoint Composition::candidatePosition()
{
    return QPoint(curpos.x(), curpos.y());
}




void Composition::draw(QPainter *p)
{
	if(lines.size()==0) return;

    int i, j, n, x, y;
    QChar c;
    p->setFont(font);

	for(i=0, n=0; i<lines.size(); i++){
        if(i==0) x = curpos.x();
		else x = rect.x();
		for(j=0; j<lines[i].length(); j++, n++){
			c = lines[i].at(j);

			//背景
			y = curpos.y() + i*lineheight;
            if(formats[n].background().isOpaque())
                p->fillRect(x, y, fontmet->width(c), lineheight, formats[n].background());
            else
                p->fillRect(x, y, fontmet->width(c), lineheight, QBrush(QColor("white")));

			//文字
			y = curpos.y() + (i+1)*lineheight-2;
			p->setPen(QPen(formats[n].foreground().color()));
            p->drawText(x, y, QString(c));

            //カーソル
            if(n==compocurpos){
                p->setPen(QPen(compocursorcolor));
                p->drawLine(x, y, x, y-lineheight);
            }
			
            x += fontmet->width(c);
		}
	}

}



