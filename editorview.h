#ifndef __EDITOR__
#define __EDITOR__



#include <QFrame>
#include <QPixmap>
#include <QScrollBar>
#include <QEvent>
#include <vector>
#include <QGridLayout>
#include "model.h"
#include "composition.h"
#include "configdialog.h"

/*
ATTR_NULLID, ATTR_TEXT, ATTR_SELECTION, ATTR_FIND,
ATTR_USERUI=100,
ATTR_USER=1000,
ATTR_COMMON=10000,
*/
//追加の属性ID
enum{
    ATTR_BACKGROUND = ATTR_USERUI,
    ATTR_IMPREEDIT,
    ATTR_IMCOMMIT,
    ATTR_LINENO,
    ATTR_RULER,
};


//テキストや行番号の表示領域を並べるためのレイアウト
class PixmapItem: public QLayoutItem
{
    QPixmap *pm;
    QRect rect;

public:
    PixmapItem();
    virtual QSize sizeHint() const {return rect.size();}
    virtual QSize minimumSize() const {return QSize(0,0);}
    virtual QSize maximumSize() const {return QSize(65535,65535);}
    virtual Qt::Orientations expandingDirections() const { return Qt::Vertical | Qt::Horizontal;}
    virtual void setGeometry(const QRect&);
    virtual QRect geometry() const{return rect;}
    virtual bool isEmpty() const {return false;}

    QPixmap *pixmap(){return pm;}
    int width(){return rect.width();}
    int height(){return rect.height();}
    int x(){return rect.x();}
    int y(){return rect.y();}
    void resize(int w,int h){setGeometry(QRect(rect.x(),rect.y(),w,h));}
};

class EditorView: public QWidget
{
	Q_OBJECT
protected:
    Model *model;
    Composition *compo;

    QGridLayout *lo;
    PixmapItem *rulerloi;
    PixmapItem *hseploi;
    PixmapItem *linenoloi;
    PixmapItem *vseploi;
    PixmapItem *textloi;

	QScrollBar *vscrollbar, *hscrollbar;
    QFont textfont, linenofont;
    QFontMetrics* fontmet;
    int fontsize, lineheight;
    bool showlineno, showruler;

public:
    EditorView(QWidget *parent);
    void setScrollBars(QScrollBar *v, QScrollBar *h);
    void setFont(int fontsize, int lineheight, QString fontfamily, QString charwidthpath);
    void attach(Model *model);
    void detatch();
    void showLineNumber(bool show);
    void showRuler(bool show);

    Model *attachedModel(){return model;}


protected:
    void createBackBuffer();
    void paintRuler(int value);

	//Qtイベント
    void    enterEvent ( QEvent * event );
    void dragEnterEvent(QDragEnterEvent *event) override;
    void    dropEvent( QDropEvent * event ) override;
	void	mousePressEvent( QMouseEvent * );
	void	mouseMoveEvent( QMouseEvent * );
	void	mouseReleaseEvent( QMouseEvent * );
	void	mouseDoubleClickEvent( QMouseEvent * );
	void 	wheelEvent( QWheelEvent * );

	void	keyPressEvent( QKeyEvent * );
	void	resizeEvent( QResizeEvent * ); 
	void	paintEvent( QPaintEvent * );

	bool	event ( QEvent * e );	//Tabを入力するためフィルタリングに使う

	unsigned long mouseEventToState(QMouseEvent *e);

	void inputMethodEvent(QInputMethodEvent *e); 
	QVariant inputMethodQuery ( Qt::InputMethodQuery query ) const;

    QPoint toEditorPos(const QPoint& pt);

public:
    void    repaintBackBuffer();
    void	repaintCB(int start, int end);
    void	slide(int dist);
	int		charWidth(wchar_t ch);

public slots:
	void	scrollHorizontally(int value);
	void	scrollVertically(int value);

	//メニュースロット
	void	copy();
	void	paste();
	void	cut();
    void    undo();
    void    redo();
	void	selectAll();

signals:
    void    mouseRightButtonPressed(QMouseEvent* e);
    void    entered(QEvent *e);
    void    dropped(QDropEvent * event);
};





#endif



