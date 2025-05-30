﻿#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <vector>
#include <QApplication>
#include <QFile>
#include <QKeyEvent>
#include <QPainter>
#include <QInputMethodEvent>
#include <QList>
#include <QTextFormat>
#include <QWheelEvent>
#include <QClipboard>
#include <QTextEdit>
#include <QDebug>
#include <QMimeData>
#include <QMessageBox>

#include "editorview.h"

extern QTextEdit *debugout;

void RepaintCB(int start, int end, void* dat);
void SlideCB(int dist, void* dat);


PixmapItem::PixmapItem()
    :QLayoutItem()
{
    pm = new QPixmap(10,10);
}


void PixmapItem::setGeometry(const QRect &rc)
{
    rect = rc;
    auto pixelratio = qApp->devicePixelRatio();
    //if(rect.width()*pixelratio>pm->width() || rect.height()*pixelratio>pm->height()){
        delete pm;
        pm = new QPixmap(rect.width()*pixelratio+0.999, rect.height()*pixelratio+0.999);
    //}
}

EditorView::EditorView(QWidget *parent)
    :QWidget(parent)
{
    model = NULL;
    setAcceptDrops(true);
    pixelratio = qApp->devicePixelRatio();

    //レイアウト
    lo = new QGridLayout(this);
    setLayout(lo);

    lo->addItem(rulerloi = new PixmapItem(), 0,0,1,3);
    lo->addItem(hseploi = new PixmapItem(), 1,0,1,3);
    lo->addItem(linenoloi = new PixmapItem(), 2,0);
    lo->addItem(vseploi = new PixmapItem(), 2,1);
    lo->addItem(textloi = new PixmapItem(), 2,2);

    lo->setSpacing(0);
    lo->setContentsMargins(0,0,0,0);
    lo->setColumnStretch(2,1);
    lo->setRowStretch(2,1);


    //ウィジェット属性
	setFocusPolicy(Qt::WheelFocus);
    setAutoFillBackground(false);  //Qtのscrollによるブランク領域再描画を無効
    setAttribute(Qt::WA_OpaquePaintEvent , true);
    //setAttribute(Qt::WA_PaintOnScreen, true);		//Qtによるダブルバッファリング無効
    setAttribute(Qt::WA_InputMethodEnabled);
    //setAttribute( Qt::WA_KeyCompression );

    //その他の変数
    compo = new Composition(this);
    showlineno=true;
    showruler=true;
    fontsize = 14;
    lineheight = 14;
    createBackBuffer();
    fontmet = NULL;     //フォントがセットされたら割り当てる

    //エディタエンジンコールバック
    EditorEngine::ConnectSlideCallback((void*)SlideCB, this);
    EditorEngine::ConnectRepaintCallback((void*)RepaintCB, this);
}

void EditorView::setScrollBars(QScrollBar *v, QScrollBar *h)
{
    vscrollbar=v;
    hscrollbar=h;
    QObject::connect(vscrollbar, SIGNAL(valueChanged(int)), this, SLOT(scrollVertically(int)) );
    QObject::connect(hscrollbar, SIGNAL(valueChanged(int)), this, SLOT(scrollHorizontally(int)) );
    vscrollbar->setStyleSheet("QScrollBar::handle:vertical {min-height: 32px;}" );
}



void EditorView::setFont(int fontsize, int lineheight, QString fontfamily, QString fontstyle, QString charwidthpath)
{
    this->fontsize = fontsize;
    this->lineheight = lineheight;

    textfont = QFont(fontfamily);
    textfont.setPixelSize(fontsize);
    if(fontstyle != "")
        textfont.setStyleName(fontstyle);

    linenofont = QFont(fontfamily);
    linenofont.setPixelSize(fontsize);

    //文字幅のキャッシュを作る
    if(fontmet)
        delete fontmet;
    fontmet = new QFontMetrics(textfont);
    QFile cwfile(charwidthpath);
    char cwtable[65536];
    if(cwfile.exists()){
        cwfile.open(QIODevice::ReadOnly);
        cwfile.read(cwtable, sizeof(cwtable));
    }
    else{
        memset(cwtable, 0, sizeof(cwtable));
        for(int i=0; i<65536; i++){
            //if(fontmet->inFont((QChar)i))
            cwtable[i] = fontmet->horizontalAdvance((QChar)i);
        }
        if(cwfile.open(QIODevice::WriteOnly)){
            cwfile.write(cwtable, sizeof(cwtable));
        }
    }

    EditorEngine::SetWidthsTable(cwtable);

    compo->setFont(fontfamily, fontsize/pixelratio);
}



void EditorView::detatch()
{
    model = NULL;
}

void EditorView::showLineNumber(bool show)
{
    if(showlineno != show){
        showlineno = show;
        createBackBuffer();
    }
}

void EditorView::showRuler(bool show)
{
    if(showruler != show){
        showruler = show;
        createBackBuffer();
    }
}

void EditorView::createBackBuffer()
{
    int rulerheight = 18;
    int margin = 2;

    //行番号領域、テキスト領域を再計算
    if(showruler){
        rulerloi->resize(width(), rulerheight);
    }
    else{
        rulerloi->resize(width(), 0);
    }
    hseploi->resize(width(), margin);
    if(showlineno){
        //fontsizeの単位はピクセルなのでdipに直す
        linenoloi->resize((fontsize/2*5)/pixelratio, linenoloi->height());
    }
    else{
        linenoloi->resize(0,linenoloi->height());
    }
    vseploi->resize(margin, vseploi->height());

    lo->invalidate();
    lo->activate();

    //モデル
    if(model){
        //固定描画部
        EditorAttribute *attr;
        QPainter p;
        QRect rc;

        attr = model->FindAttribute(ATTR_RULER);
        p.begin(hseploi->pixmap());
        rc = QRect(0,0,hseploi->pixmap()->width(),hseploi->pixmap()->height());
        p.fillRect(rc, QColor(attr->br,attr->bg,attr->bb));
        p.end();

        attr = model->FindAttribute(ATTR_LINENO);
        p.begin(vseploi->pixmap());
        rc = QRect(0,0,vseploi->pixmap()->width(),vseploi->pixmap()->height());
        p.fillRect(rc, QColor(attr->br,attr->bg,attr->bb));
        p.end();

        p.begin(textloi->pixmap());
        attr = model->FindAttribute(ATTR_BACKGROUND);
        rc = QRect(0,0,textloi->pixmap()->width(),textloi->pixmap()->height());
        p.fillRect(rc, QColor(attr->r,attr->g,attr->b));
        p.end();


        paintRuler(hscrollbar->value());

        //ビューパラメータ
        int wrapwidth;
        FileTypeConfig *ftconf = model->getFileTypeConfig();
        wrapwidth = ftconf->autowrap?
                    textloi->pixmap()->width():
                    ftconf->wrapsize * fontsize;
        model->SetViewParams((int)((float)fontsize/2*ftconf->tabsize),
                             wrapwidth, textloi->pixmap()->height(), lineheight);
        repaintBackBuffer();
    }


}

void EditorView::paintRuler(int value)
{
    if(!showruler)
        return;

    EditorAttribute *pattr;
    int orgpoint=(textloi->x() - value*(float)fontsize/2) * pixelratio; //dip to pixel
    int col, x;
    QPixmap *pm = rulerloi->pixmap();
    QPainter p(pm);
    QFont rulerfont(linenofont.family());
    pattr = model->FindAttribute(ATTR_RULER);
    p.fillRect(pm->rect(), QColor(pattr->br,pattr->bg,pattr->bb));

    rulerfont.setPixelSize(linenofont.pixelSize());

    p.setPen(QColor(pattr->r,pattr->g,pattr->b));
    p.drawLine(orgpoint, pm->height()-3, pm->width(), pm->height()-3);
    for(col=0,x=0; x<pm->width(); col++){
        x = orgpoint + int(col * (float)fontsize/2);
        if(col%10==0){
            p.drawLine(x, 3, x, pm->height()-3);

            QRect textrc(x+1, 0, fontsize, pm->height());
            p.drawText(textrc, Qt::AlignVCenter | Qt::AlignLeft,
                       QString("%1").arg((int)(col/10)));
        }
        else{
            p.drawLine(x, pm->height()-6, x, pm->height()-3);
        }
    }

    repaint(rulerloi->geometry());
}

void EditorView::enterEvent(QEvent *event)
{
    emit entered(event);
}

void EditorView::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void EditorView::dropEvent(QDropEvent *event)
{
    emit dropped(event);
}


//モデルを入れ替え、モデルのビューに対する設定を適用する
void EditorView::attach(Model *mdl)
{
    FileTypeConfig *ftconf;

    if(!mdl){
        detatch();
        return;
    }

    model=mdl;
    model->AllowDrawing(false);

    ftconf = model->getFileTypeConfig();
    hscrollbar->setVisible(!ftconf->autowrap);
    createBackBuffer();

    EditorAttribute *attr = model->FindAttribute(ATTR_BACKGROUND);
    auto bgcolor = QColor(attr->br, attr->bg, attr->bb);
    compo->setBackground(bgcolor);
/*
    //スクロールバー
    if(model->GetScrollPos() > vscrollbar->maximum()){
        vscrollbar->setMaximum(model->GetTotalLine()-1);
        vscrollbar->setValue(model->GetScrollPos());
    }
    else{
        vscrollbar->setValue(model->GetScrollPos());
        vscrollbar->setMaximum(model->GetTotalLine()-1);
    }
	vscrollbar->setSingleStep(1);
    vscrollbar->setPageStep((int)(textloi->height()/fontsize));
*/
    model->AllowDrawing(true);
}





void EditorView::resizeEvent( QResizeEvent *e )
{
    createBackBuffer();

    debugout->append(QString("RESIZE view(%1,%2), pm(%3,%4)").arg(textloi->width()).arg(textloi->height()).arg(textloi->pixmap()->width()).arg(textloi->pixmap()->height()));
}



bool EditorView::event(QEvent *e)
{
	//Tabが押されたときはここでフィルタリングする
    if(e->type() == QEvent::KeyPress){
		QKeyEvent *ke = (QKeyEvent*)e;
        if(ke->key()==Qt::Key_Tab || ke->key()==Qt::Key_Backtab){
			keyPressEvent(ke);
            return true; //以降のQtフレームワーク内部での処理をストップさせる
		}
	}
    return QWidget::event(e);
}


void EditorView::keyPressEvent(QKeyEvent *e)
{
    unsigned long state=0;
	wchar_t unichar;
    int key=KEY_NULL;
	uint i;

	//修飾キー
    if(e->modifiers()&Qt::ShiftModifier) state |= MODKEY_SHIFT;
    if(e->modifiers()&Qt::ControlModifier) state |= MODKEY_CONTROL;

	//押されたキー
	switch(e->key()){
    case Qt::Key_Pause:
#ifdef QT_DEBUG
        //このキーはデバッグに使う
        /*
        model->HighlightFindWord(L"a.b", true, false, true);
        repaintBackBuffer();*/
        //model->Insert(L"a\"\"aa\nbbb\n\"\"c*/cc");
        //model->Find(L"a..", false, true, true, true); //fromtop,reverse, case, regexp
        //model->Find(L"abc", false, false, false, false); //fromtop,reverse, case, regexp
        //model->Replace(L"a..",L"def", false, true, true);
        //model->HighlightFindWord(L"ccc", false, false);
        //model->ReplaceAll(L"aaa\n",L"hoge",true,true,false);
        //model->ReplaceAll(L"#",L"#\n",true,true,false);
        model->ReplaceAll(L"\n",L"",true,true,false);
#endif
        return;
	case Qt::Key_Up:		key=KEY_UP; break;
	case Qt::Key_Down:		key=KEY_DOWN; break;
	case Qt::Key_Left:		key=KEY_LEFT; break;
	case Qt::Key_Right:		key=KEY_RIGHT; break;
	case Qt::Key_PageUp:	key=KEY_PAGEUP; break;
	case Qt::Key_PageDown:	key=KEY_PAGEDOWN; break;
	case Qt::Key_Home:		key=KEY_HOME; break;
	case Qt::Key_End:		key=KEY_END; break;

	case Qt::Key_Enter: 
	case Qt::Key_Return:	key=KEY_ENTER; break;
	case Qt::Key_Space:		key=KEY_SPACE; break;
    case Qt::Key_Tab:
        key=KEY_TAB;
        break;
    case Qt::Key_Backtab:
        state |= MODKEY_SHIFT;
        key=KEY_TAB;
        break;
	case Qt::Key_Delete:	key=KEY_DELETE; break;
	case Qt::Key_Backspace:	key=KEY_BACKSPACE; break;

	case Qt::Key_Alt:		key=KEY_ALT;	break;

    case Qt::Key_Escape:
#ifdef QT_DEBUG
        qApp->exit(0); return; //[test]
#endif
	case Qt::Key_unknown:
		//これはX11ではIMのインプット中にも入ってくる
		return;

	default:
        if(e->text().length()==0) return;
        for(i=0; i<(unsigned)e->text().length(); i++){
            unichar = e->text().at(i).unicode();
            model->KeyEvent(KEY_CHAR, unichar, state);
        }
		return;
	}

	//文字以外のとき
    if(key!=KEY_NULL){
        model->KeyEvent(key, 0, state);
        return; //Qtフレームワークにキー処理を渡さない
    }
    else
        return QWidget::keyPressEvent(e);
}




QVariant EditorView::inputMethodQuery ( Qt::InputMethodQuery query ) const
{
	switch(query){
    //case Qt::ImMicroFocus:
    case Qt::ImCursorRectangle:
        return QRect(compo->candidatePosition(), QSize(fontsize,lineheight));
    case Qt::ImFont:
        return textfont;
    case Qt::ImEnabled:
        return QWidget::inputMethodQuery(query);
/*
 * これらは再変換に使うのみなので再変換をサポートするまでは使わない
    case Qt::ImCursorPosition:
        return 30;
    case Qt::ImSurroundingText:
        return tr("あいうえお");
    case Qt::ImCurrent:
        return tr("いろは");
*/
    default:
        return QWidget::inputMethodQuery(query);
    }

    return QWidget::inputMethodQuery(query);
}




void EditorView::inputMethodEvent(QInputMethodEvent *e)
{
    //確定文字を入力
    if(e->commitString().length()>=1){
        model->Insert(e->commitString().toStdWString());
    }

    //未確定(変換中)文字
	compo->inputMethodEvent(e);
    repaint(compo->boundingRect());

//	QWidget::inputMethodEvent(e);
}




QPoint EditorView::toEditorPos(const QPoint &pt)
{
    //ウィジェットのDIP座標からテキスト領域のピクセル座標に変換する
    int x, y;
    if(!hscrollbar->isHidden())
        x = pt.x() - textloi->x() + hscrollbar->value()*fontsize/2;
    else
        x = pt.x() - textloi->x();
    y = pt.y() - textloi->y();

    return QPoint(x * pixelratio, y * pixelratio);
}

unsigned long EditorView::mouseEventToState(QMouseEvent *e)
{
    //Qtのマウスイベントからエディタエンジンのイベントに変換
    unsigned long state=0;
    if(e->buttons()&Qt::LeftButton)  state |= BUTTON_LEFT;
    if(e->modifiers()&Qt::ShiftModifier) state |= MODKEY_SHIFT;
    return state;
}


void EditorView::mouseMoveEvent( QMouseEvent *e )
{
    QPoint pt = toEditorPos(e->pos());
    model->MouseEvent(MOUSE_MOVE, 0, pt.x(), pt.y(), mouseEventToState(e));
}

void EditorView::mousePressEvent( QMouseEvent *e )
{
    QPoint pt = toEditorPos(e->pos());
	switch(e->button()){
    case Qt::LeftButton:
        model->MouseEvent(BUTTON_LEFT, 1, pt.x(), pt.y(), mouseEventToState(e));
		break;
    case Qt::RightButton:
        emit mouseRightButtonPressed(e);
        break;
    default:
        break;
	}
}

void EditorView::mouseDoubleClickEvent( QMouseEvent *e )
{
    QPoint pt = toEditorPos(e->pos());
	switch(e->button()){
    case Qt::LeftButton:
        model->MouseEvent(BUTTON_LEFT, 2, pt.x(), pt.y(), mouseEventToState(e));
		break;
    default:
        break;
	}
}

void EditorView::mouseReleaseEvent( QMouseEvent *e )
{
    QPoint pt = toEditorPos(e->pos());
	switch(e->button()){
    case Qt::LeftButton:
        model->MouseEvent(BUTTON_LEFT, 0, pt.x(), pt.y(), mouseEventToState(e));
		break;
    default:
        break;
	}
}


void EditorView::wheelEvent(QWheelEvent *e)
{
    int px = (e->position().x() - textloi->x()) * pixelratio;
    int py = (e->position().y() - textloi->y()) * pixelratio;
    unsigned long state=0;
    if(e->buttons()&Qt::LeftButton)  state |= BUTTON_LEFT;
    if(e->modifiers()&Qt::ShiftModifier) state |= MODKEY_SHIFT;

    if(e->angleDelta().y()>0)
        model->MouseEvent(BUTTON_WHEELUP, 1, px, py, state);
    else
        model->MouseEvent(BUTTON_WHEELDOWN, 1, px, py, state);
}




void EditorView::scrollVertically(int value)
{
	model->ScrollEvent(value);
}

void EditorView::scrollHorizontally(int value)
{
    repaintBackBuffer();
    paintRuler(value);
    repaint(rulerloi->geometry());
}



void SlideCB(int dist, void* dat)
{((EditorView*)dat)->slide(dist);}
void EditorView::slide(int dist)
{
    int dy = dist*lineheight;
    textloi->pixmap()->scroll(0, dy, QRect(0,0,textloi->pixmap()->width(),textloi->pixmap()->height()));
    linenoloi->pixmap()->scroll(0, dy, QRect(0,0,linenoloi->pixmap()->width(),linenoloi->pixmap()->height()));
    //repaint(textloi->geometry());
    //repaint(linenoloi->geometry());
    repaint();
}

void EditorView::repaintBackBuffer()
{
    if(model)
        repaintCB(0, model->GetLineCountOfScreen());
}


void RepaintCB(int start, int end, void* dat)
{((EditorView*)dat)->repaintCB(start,end);}
void EditorView::repaintCB(int start, int end)
{
    if(!model)
        return;

    QPainter p;
    Character* chara;
    int px,py;
    EditorAttribute *pattr;
    int fontbaseheight = (int)(lineheight * (float)fontmet->ascent() / (fontmet->ascent() + fontmet->descent()) + 0.5);

    /******** テキスト領域 ********/
    p.begin(textloi->pixmap());
    //背景
    pattr = model->FindAttribute(ATTR_BACKGROUND);
    p.fillRect(
            0,
            start*lineheight,
            textloi->pixmap()->width(),
            (end-start+1)*lineheight,
            QColor(pattr->r,pattr->g,pattr->b) );

    //初期化
    //textfont.setBold(false);
    p.setFont(textfont);
    p.setPen(QColor(0,0,0));

    //開始位置まで進める
    chara = model->BeginScreenScan();
    while(chara->line!=start){
        if(chara->ch==ETX) break;
        chara = model->ProceedScan();
    }

    //描画
    while(chara->line<=end){
        //textfont.setBold(chara->bold);
        p.setFont(textfont);
        px = chara->x - hscrollbar->value()*(float)fontsize/2;
        py = chara->line * lineheight;
        if(chara->ch==ETX){
            p.fillRect(px, py, fontmet->horizontalAdvance('E'), lineheight, QColor(Qt::lightGray) );
            p.setPen(QColor(Qt::white));
            p.drawText(px, py+fontbaseheight, QString(QChar('E')));
            break;
        }
        else if(chara->ch=='\n'){
            //p.setPen(QColor(chara->r,chara->g,chara->b));
            //p.drawText(px, py-2, QString(QChar('N')));
        }
        else{
            //背景が透明色以外のときは背景を塗る
            if(chara->br!=TRANSPARENT_COLOR){
                p.fillRect(px, py, chara->w, lineheight, QColor(chara->br,chara->bg,chara->bb) );
            }
            //文字描画、アンダーラインを考慮して少し上にあげる
            p.setPen(QColor(chara->r,chara->g,chara->b));
            p.drawText(px, py+fontbaseheight, QString(QChar(chara->ch)));
        }
        chara = model->ProceedScan();
    }
    p.end();

    /******** 行番号 ********/
    if(showlineno){
        p.begin(linenoloi->pixmap());
        int i, col;
        int *lineno;
        char strlineno[100];
        pattr = model->FindAttribute(ATTR_LINENO);
        p.setPen(QColor(pattr->r,pattr->g,pattr->b));
        p.setFont(linenofont);
        lineno=model->GetLineNumberTable();

        p.fillRect(
            0,
            start*lineheight,
            linenoloi->pixmap()->width(),
            (end-start+1)*lineheight,
            QColor(pattr->br,pattr->bg,pattr->bb) );

        for(i=start; i<=end; i++){
            if(model->GetScrollPos() + i >= model->GetTotalLine()) break;
            if(lineno[i]==-1) continue;
            sprintf(strlineno,"%d", lineno[i]+1);	//0行ではなく1行から始める
            col=4-log10((double)(lineno[i]+2));	//右詰めにする
            p.drawText(col*fontsize/2, i*lineheight+fontbaseheight, strlineno);
        }
        p.end();
    }

    //実画面へ転送
    //repaint(0, textloi->y()+start*lineheight, width(), (end-start+1)*lineheight);
    repaint();

    //スクロールバーはこのタイミングで更新する
    int nscreenlines = qRound((double)textloi->height()/lineheight);
    if(model->GetScrollPos() > vscrollbar->maximum()){
        vscrollbar->setRange(0, model->GetTotalLine()-1);
        vscrollbar->setValue(model->GetScrollPos());
    }
    else{
        vscrollbar->setValue(model->GetScrollPos());
        vscrollbar->setRange(0, model->GetTotalLine()-1);
    }
	vscrollbar->setSingleStep(1);
    vscrollbar->setPageStep(nscreenlines);

    //IM表示位置
    int curx, cury;
    model->GetCursor(&curx, &cury);
    curx = curx/pixelratio + textloi->x();
    cury = (cury*lineheight)/pixelratio + textloi->y();
    compo->setMicroFocusHint(textloi->geometry(), QPoint(curx, cury), lineheight/pixelratio);

    ///////////////////////////////////////////////
    //デバッグ用
#ifdef QT_DEBUG
    QString debugstr;

    debugstr += QString::fromStdWString(model->debugstr);
    debugstr += tr("\n");

    //行番号
    /*
    for(int i=0; i<model->GetLineCountOfScreen(); i++){
        debugstr += tr("%1,").arg(model->GetLineNumberTable()[i]);
    }
    debugstr += "\n";
*/
    //ノード
    debugstr += tr("%13 %11 crt:(prg%12,hy%1,ry%2,x%3) org:(hy%8,ry%9,x%10) mx=%4,idx=%6,nattr=%7 Scroll:%5\n")
            .arg(model->crt.hy)
            .arg(model->crt.ry)
            .arg(model->crt.x)
            .arg(model->mx)
            .arg(model->scroll)
            .arg(model->crt.index)
            .arg(model->crt.paragraph->attrgrps.size())
            .arg(model->org.hy)
            .arg(model->org.ry)
            .arg(model->org.x)
            .arg(model->org.prgidx)
            .arg(model->crt.prgidx)
            .arg(model->ntotallines)
            ;

    //属性
    for(int i=0; i<model->crt.paragraph->attrgrps.size(); i++){
        EditorEngine::AttributeGroup *attrgrp = model->crt.paragraph->attrgrps[i];
        debugstr += tr("%5 stsym:%1:%3,edsym%2:%4\n")
                .arg(QString::fromStdWString(attrgrp->attr->stsym))
                .arg(QString::fromStdWString(attrgrp->attr->edsym))
                .arg(attrgrp->start.index)
                .arg(attrgrp->end.index)
                .arg(attrgrp->id)
                ;
    }


    //Undoレコード
    int i,j;
    for(i=0; i<model->maxundo; i++){
        EditorEngine::UndoUnit *uu;
        QString undostr;
        std::list<EditorEngine::UndoRecord>::iterator it;
        uu = &model->undounit[i];
        for(it=uu->records.begin(); it!=uu->records.end(); it++){
            undostr = QString::fromStdWString(it->str);
            undostr = undostr.replace(tr("\n"), tr("\\n"));
            undostr = undostr.replace(tr("\t"), tr("\\t"));
            debugstr.append(tr("(type%1,prgidx%2,txtidx%3,%4)").arg(
                (*it).type).arg((*it).prgindex).arg((*it).textindex).arg(undostr) );
        }
        debugstr.append("\n");
    }

    debugstr.append(QString("fontmet asc%1 dsc%2").arg(fontbaseheight).arg(fontmet->descent()));

    debugout->clear();
    debugout->insertPlainText(debugstr);
#endif

}




void EditorView::paintEvent( QPaintEvent *e )
{
    //qDebug()<<"paintEvent:"<<e->rect() << "text" <<textloi->geometry();

    if(model==NULL) return;


    QPainter p(this);

    //バックバッファ
    PixmapItem *pis[] = {rulerloi, linenoloi, textloi, hseploi, vseploi, NULL};
    for(int i=0; pis[i]!=NULL; i++){
        PixmapItem *pi = pis[i];

        p.drawPixmap(pi->geometry(), *pi->pixmap(), pi->pixmap()->rect());

        /*
        if(!e->rect().intersects(pi->geometry())) continue;

        QRect isec = e->rect().intersected(pi->geometry());
        p.drawPixmap(isec.topLeft(), *pi->pixmap(),
                     QRect(isec.x()-pi->x(), isec.y()-pi->y(),
                           isec.width(), isec.height()) );
*/
    }

    //カーソル
	int curx,cury;
	model->GetCursor(&curx, &cury);
    curx /= pixelratio;
    curx += textloi->x() - hscrollbar->value()*fontsize/2;
    cury = (int)((cury*lineheight)/pixelratio + textloi->y() + 0.999);
    if(curx>=textloi->x() && cury >=textloi->y())
       p.drawLine(curx, cury, curx, cury+lineheight/pixelratio);

    //IM
    compo->draw(&p);

}

void EditorView::focusInEvent(QFocusEvent* e)
{
}





void EditorView::cut()
{
	copy();
    model->DeleteSelectedArea();
}


void EditorView::copy()
{
    QString selstr = QString::fromStdWString(model->GetSelectedText());
    if(selstr.length()!=0)
        QApplication::clipboard()->setText(selstr);
}


void EditorView::paste()
{
	QString qstr;
	std::wstring wstr;
	int i;

    wstr.reserve(qstr.length());
	qstr = QApplication::clipboard()->text();
	for(i=0; i<qstr.length(); i++){
		if(qstr.at(i)=='\r') continue;
		wstr += (wchar_t)qstr.at(i).unicode();
	}
	model->Insert(wstr);
}


void EditorView::undo()
{
    model->Undo();
}

void EditorView::redo()
{
    model->Redo();
}


void EditorView::selectAll()
{
    model->SelectAll();
}









