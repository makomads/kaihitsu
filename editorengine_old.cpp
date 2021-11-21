#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <list>
#include <vector>
#include <map>
#include "editorengine.h"


//接続されたコールバック関数とおまけポインタ
static void* cb_slide=NULL;		//void cbfunc(int dist, void* dat)
static void* cb_slidedat=NULL;
static void* cb_repaint=NULL;		//void cbfunc(int start, int end, void* dat)
static void* cb_repaintdat=NULL;
static void* cb_charwidth=NULL;		//int cbfunc(unsigned short char, void* dat)
static void* cb_charwidthdat=NULL;
static void* cb_modified=NULL;
static void* cb_modifieddat=NULL;

void EditorEngine::ConnectRepaintCallback(void* callback, void* dat)    //static
{
    cb_repaint=callback;
    cb_repaintdat=dat;
}

void EditorEngine::ConnectSlideCallback(void* callback, void* dat)   //static
{
    cb_slide=callback;
    cb_slidedat=dat;
}

void EditorEngine::ConnectCharWidthCallback(void* callback, void* dat)   //static
{
    cb_charwidth=callback;
    cb_charwidthdat=dat;
}

void EditorEngine::ConnectModifiedCallback(void *callback, void *dat)   //static
{
    cb_modified=callback;
    cb_modifieddat=dat;
}


EditorAttributeSet EditorEngine::defattrs;     //static

void EditorEngine::SetDefaultAttributeSet(const EditorAttributeSet &attr)   //static
{
    defattrs = attr;
}

EditorAttribute *EditorEngine::FindAttribute(int id)
{
    for(int i=0; i<attribset.size(); i++)
        if(attribset[i].id==id)
            return &attribset[i];
    //NULLポインタは返さずヌルオブジェクトを返す
    return &g_nullattr;
}


EditorEngine::EditorEngine()
{
	root=new Node;
	root->prev=NULL;
	root->ch=ETX;
    root->w=0;
	root->next=new Node;
    org=root;
    last=root->next;
    last->prev=root;
    last->next=NULL;
    last->ch=ETX;
    last->w=0;
    crt = last;
	scanret=new Character;

	curx=0; cury=0;
    curindex=0;
	totalline=1; scroll=0;
	totalpara=scrollpara=0;
    chcount=0;
    ancx = ancy = 0;
    ancindex = 0;
    anchor = crt;
    findstart = NULL;
    findend = NULL;

    fontheight=12;
    tabsize=fontheight*8;
    scrollsize=1;
    drawable=true;
    showlineno=true;
    shiftkey=false;

    maxundo=4;
    undounit=new UndoUnit[maxundo];
	crtuu=enduu=startuu=maxundo-1;
    prerec_index=0;
    prerec_type=UU_NULL;

    //特殊属性グループ
    //通常の属性グループは強調文字一致で動的に作られるが、これらのグループは常に存在する
    atgrpfind = new AttributeGroup();
    atgrpfind->id = GenerateUniqueID();
    atgrpfind->attrib = NULL;
    atgrpfind->refcount = 1; //1以上にすると自動的に削除されない
    atgrpfind->wordindex = 0;
    attrgrplist.push_back(atgrpfind);

    atgrpfindarea = new AttributeGroup();
    atgrpfindarea->id = GenerateUniqueID();
    atgrpfindarea->attrib = NULL;
    atgrpfindarea->refcount = 1;
    atgrpfindarea->wordindex = 0;
    attrgrplist.push_back(atgrpfindarea);

    SetViewParams(8, 100, 100, 12);
	orgattrs=NULL;
}

EditorEngine::~EditorEngine()
{
	Clear();
    delete root->next;
    delete root;
}


void EditorEngine::SetAttributeSet(const EditorAttributeSet &as)
{
    attribset = as;

    //デフォルト属性を追加
    for(int i=0; i<defattrs.size(); i++){
        if(FindAttribute(defattrs[i].id)->id==ATTR_NULLID)
            attribset.push_back(defattrs[i]);
    }

    //必須属性が存在しなければここでハードコーディングで入れる
    if(FindAttribute(ATTR_TEXT)->id==ATTR_NULLID)
        attribset.push_back(EditorAttribute(ATTR_TEXT, L"text", 0,0,0, TRANSPARENT_COLOR,TRANSPARENT_COLOR,TRANSPARENT_COLOR,0,0,0));
    if(FindAttribute(ATTR_FIND)->id==ATTR_NULLID)
        attribset.insert(attribset.begin(),EditorAttribute(ATTR_FIND, L"find",192,192,32,0,0,0,0,0,0,L"",L""));
    if(FindAttribute(ATTR_FINDAREA)->id==ATTR_NULLID)
        attribset.insert(attribset.begin(),EditorAttribute(ATTR_FINDAREA, L"findarea",192,0,0,192,192,192,0,0,0,L"",L""));
    if(FindAttribute(ATTR_SELECTION)->id==ATTR_NULLID)
        attribset.push_back(EditorAttribute(ATTR_SELECTION, L"selection", 0,0,0, 64,192,255,0,0,0));

    //優先属性を先頭にもってくる
    for(int i=0; i<attribset.size(); i++){
        if(attribset[i].id==ATTR_FIND || attribset[i].id==ATTR_FINDAREA
                || attribset[i].id==ATTR_SELECTION){
            //(ここはC++11以降であればstd::moveで置き換えた方がよい)
            EditorAttribute tmpattr = attribset[i];
            attribset.erase(attribset.begin()+i);
            attribset.insert(attribset.begin(), tmpattr);
        }
    }

    //再度特殊属性のポインタ取得
    //std::vector内部のメモリ位置は挿入すると変わるのでこの関数の最後に取得する
    atgrpfind->attrib = FindAttribute(ATTR_FIND);
    atgrpfindarea->attrib = FindAttribute(ATTR_FINDAREA);
    attrtext = FindAttribute(ATTR_TEXT);
    attrsel = FindAttribute(ATTR_SELECTION);
}




void EditorEngine::SetViewParams(int tabsize,int wndxpx, int wndypx, int fontheight)
{
    int nlines;

    //画面内の表示行数を計算
    nlines = (int)(wndypx/fontheight);
    if(wndypx%fontheight!=0) nlines++;

    //ビューのパラメータに変更がなければ位置の再計算をしない
    if(this->tabsize==tabsize && this->wndx==wndxpx
            && this->wndy==nlines && this->fontheight == fontheight)
        return;

	this->tabsize=tabsize;
    this->wndx=wndxpx;
    this->wndy=nlines;
    this->fontheight=fontheight;

	bool findingorg=false;
	Node *n=root->next;
	int x=0;

	//現在の状態を作り直す
	totalline=1; scroll=0;	
	while(true){
        //元の表示原点を通過したら新しい表示原点を調べるモードに入る
		if(n==org) findingorg=true;
		if(n->ch==ETX) break;
		
		//カーソルの同期
        x+=n->w;
		n=n->next;
		
		//折り返し判定と原点判定も同時に
        if(x>=wndxpx || n->prev->ch=='\n'){
			if(findingorg){
                org=n->prev;
                findingorg=false;
				scroll=totalline;
			}
            totalline++;
            x=0;
		}
	}
	
    //現在地とアンカーは再計算が面倒なので左上とする
    crt=org->next;
    curx=cury=mx=0;
    curindex=0;
    for(Node *nd=root->next; nd->ch!=ETX&&nd!=crt; nd=nd->next) curindex++;
    DropAnchor();
	Repaint();
}




//再描画
void EditorEngine::Repaint(){Repaint(0,wndy-1);}
void EditorEngine::Repaint(int start, int end)
{
    if(!drawable || cb_repaint==NULL) return;
	if(start<0) start=0;
	if(end>=wndy) end=wndy-1;
    ((void(*)(int,int,void*))cb_repaint)(start, end, cb_repaintdat);

    //[debug]
    //((void(*)(int,int,bool,void*))cb_repaint)(0, wndy-1, true, cb_repaintdat);
}


//部分スクロール
void EditorEngine::Slide(int dist)
{
    if(!drawable || cb_slide==NULL) return;
    ((void(*)(int,void*))cb_slide)(dist, cb_slidedat);
}




//ファイル読み込み時を想定して高速な挿入を行う
//...が、未実装なので通常の挿入をする
void EditorEngine::InsertFast(const std::wstring& str)
{
    AllowDrawing(false);
    Insert(str, false);
    ResetCursorPos();
    AllowDrawing(true);
    Repaint();
}


void EditorEngine::Insert(wchar_t chara)
{
    std::wstring wstr;
    wstr += chara;
    Insert(wstr,true);
}


//挿入
//文字入力、ペーストなどから呼ばれる
void EditorEngine::Insert(const std::wstring& str, bool recording)
{
    Node *n, *insprev, *headprev, *tail;
    bool repaintall=false, needrepaint=false;
    int heady;
    int oldtaily, newtaily;
	std::wstring::const_iterator sit;
    int oldx, tabx;

    AllowDrawing(false);

    //範囲選択時は解除するため無条件で全描画
    if(findstart!=NULL)
        repaintall |= true;
    repaintall |= IsSelected();

    SetModified(true);
    ShowCursor();
    ReleaseFindingArea();
    DeleteArea(recording);

	//後で判定に使うため、挿入前の状態で必要なものを保存しておく
    //挿入位置は挿入後動いてしまうので、一つ後方ノードを保持する
    oldx = curx;
	insprev=crt->prev;
    tail = GetLineTail(crt, curx, cury, NULL, &oldtaily);
    headprev = GetLineHead(crt, cury, &heady)->prev;

    //Undo情報を記録
    if(recording){
        std::list<wchar_t> chlist;
        for(sit=str.begin(); sit!=str.end(); sit++){
            if(*sit==ETX) continue;
            chlist.push_back(*sit);
        }
        RecordUndo(UU_INS, chlist);
    }

    //ノード連結
	for(sit=str.begin(); sit!=str.end(); sit++){
        if(*sit==ETX) continue;
		n = new Node;
        n->ch = *sit;
        n->w = ((int(*)(wchar_t,void*))cb_charwidth)(n->ch, cb_charwidthdat);
		n->prev=crt->prev;
		n->prev->next=n;
		n->next=crt;
		crt->prev=n;
        chcount++;
        curindex++;
	}

    //タブ幅調整、挿入開始位置から改行まで
    //タブ幅は折り返しを考慮しない
    tabx = oldx;
    n=insprev->next;
    while(n!=tail){
        if(n->ch=='\t'){
            n->w = tabsize - tabx%tabsize;
        }
        tabx += n->w;
        if(n->ch=='\n')
            tabx=0;
        n = n->next;
    }

    //カーソル位置と文字幅
    //行末禁則処理のためカーソル位置は段落頭から再計算する
    //例えばカーソルxが0、後方行末残が4pixelで1pixelの文字を挿入すると
    //後方行末に挿入され、カーソル位置は変更されない
    n = headprev->next;
    curx = 0;
    cury = heady;
    while(n!=crt){
        curx+=n->w;
        n = n->next;
        if(curx+n->w>=wndx || n->prev->ch=='\n'){
            curx=0;
            cury++;
        }
    }
    mx = curx;
    DropAnchor();

    //属性更新
    RebuildAttribute(insprev->next, &needrepaint);
    repaintall |= needrepaint;

    //再び段落端を調べて変化があれば行数を増やす
    GetLineTail(crt, curx, cury, NULL, &newtaily);
    if(oldtaily != newtaily){
        totalline += newtaily-oldtaily;
        repaintall |= true;
    }

    //描画
    AllowDrawing(true);
    if(repaintall)
        Repaint();
    else
        Repaint(heady, newtaily);

    //カーソルが下部自動スクロール域に入っていたらスクロールする
    ShowCursor();
}



void EditorEngine::BackSpace()
{
    if(IsSelected()){
        DeleteArea(true);
        return;
    }

    AllowDrawing(false);
    //後方1文字選択
    if(crt->prev->ch==ETX)
        return;
    DropAnchor();
    shiftkey=true;
    Back();
    shiftkey=false;
    AllowDrawing(true);

    //Undo記録、削除
    std::list<wchar_t> chlist;
    chlist.push_back(crt->ch);
    RecordUndo(UU_BS, chlist);
    DeleteArea(false);
}

//現在のカーソルのある文字を１字消す
void EditorEngine::Delete()
{
    if(crt->next==NULL) return;

    //アンカーを下ろしてカーソルを1つ進め、1文字分範囲選択する
    AllowDrawing(false);
    DropAnchor();
    shiftkey=true;
    Forward();
    shiftkey=false;
    AllowDrawing(true);
    //消す
    DeleteArea(true);
}

void EditorEngine::DeleteArea(bool recording)
{
    Node *nd, *next, *prev;
    int selstx, selsty, selstidx, seledx, seledy;
    Node *ndselst, *ndseled, *headprev;
    bool repaintall=false, needrepaint=false;
    int oldtaily, newtaily, heady;
    int tabx;

    AllowDrawing(false);

    if(findstart!=NULL)
        repaintall |= true;

    if( !IsSelected() ) return;
    SetModified(true);
    ReleaseFindingArea();

    //選択範囲取得、かつカレントノードを選択範囲の開始位置へ持っていく
    //この関数内部で現在地情報を使っているので、直接引数に現在地を入れてはいけない
    GetSelectionPos(&ndselst, &selstx, &selsty, &selstidx, &ndseled, &seledx, &seledy, NULL);
    crt = ndselst;
    curx = selstx;
    cury = selsty;
    curindex = selstidx;

    //表示原点ノードを削除されないよう表示原点を開始位置より前にもってくる
    ShowCursor();

    //選択開始地点(現在地)が属する段落の先頭一つ前方と、選択終了地点が属する段落の末尾を保存
    headprev = GetLineHead(crt, cury, &heady)->prev;
    GetLineTail(ndseled, seledx, seledy, NULL, &oldtaily);

    //Undo情報記録
    if(recording){
        std::list<wchar_t> chlist;
        for(Node *nd=ndselst; nd!=ndseled; nd=nd->next)
            chlist.push_back(nd->ch);
        RecordUndo(UU_DEL, chlist);
    }

    //ノード削除
    prev = crt->prev;
    while(crt!=ndseled){
        next = crt->next;
        if(crt->ch=='\n'){  totalpara--; }
        ClearAttributes(crt);
		delete crt;
        chcount--;
        crt = next;
	}
	crt->prev = prev;
	prev->next = crt;

    //現在地ノードは動かないが、行末禁則処理によりカーソル位置の再計算は必要
    nd = headprev->next;
    curx = 0;
    cury = heady;
    while(nd!=crt){
        curx+=nd->w;
        nd = nd->next;
        if(curx+nd->w>=wndx || nd->prev->ch=='\n'){
            curx=0;
            cury++;
        }
    }
	mx=curx;
    DropAnchor();

    //現在地から行末までのタブ幅
    for(tabx=curx, nd=crt; nd->ch!='\n' && nd->ch!=ETX; nd=nd->next){
        if(nd->ch=='\t'){
            nd->w = tabsize - tabx%tabsize;
            break;
        }
        tabx += nd->w;
    }

    //属性更新
    RebuildAttribute(NULL, &needrepaint);
    repaintall |= needrepaint;

    //再び行端を調べる
    GetLineTail(crt, curx, cury, NULL, &newtaily);
    if(oldtaily != newtaily){
        totalline -= oldtaily-newtaily;
        repaintall |= true;
    }

    //描画
    AllowDrawing(true);
    if(repaintall)
        Repaint();
    else{
        if(curx==0)
            Repaint(cury-1, newtaily);
        else
            Repaint(cury, newtaily);
    }

}




//キー入力
void EditorEngine::KeyEvent(int keycode, wchar_t chara, unsigned long state)
{
    int i;
    ReleaseFindingArea();

    shiftkey = state&MODKEY_SHIFT? true: false;

    if(state&MODKEY_CONTROL && keycode==KEY_CHAR){
		char c = toupper(chara);
		switch(c){
		case 'Z':
			Undo();
			break;
		case 'Y':
			Redo();
			break;
		case 'A':
			SelectAll();
			break;
		}
	}
    else switch(keycode){
	//移動系
	case KEY_UP:	LineUp(); break;
	case KEY_DOWN:	LineDown(); break;
	case KEY_LEFT:	Back(); break;
	case KEY_RIGHT:	Forward(); break;
    case KEY_PAGEUP:
		for(i=0;i<scrollsize;i++) LineUp();
		break;
	case KEY_PAGEDOWN:
		for(i=0;i<scrollsize;i++) LineDown();
		break;
	case KEY_HOME:
		while(crt->prev->ch!='\n' && crt->prev->ch!=ETX) Back();
		Repaint(cury,cury);
		break;
	case KEY_END:
		while(crt->ch!='\n' && crt->ch!=ETX) Forward();
		Repaint(cury,cury);
		break;

	//入力系
    case KEY_ENTER:
        if(!autoindent)
            Insert(L'\n');
        else{
            Node *nd = GetLineHead(crt,0,NULL);
            int tablevel=0;
            while(nd->ch=='\t'){
                tablevel++;
                nd=nd->next;
            }
            Insert(L'\n');
            for(int i=0; i<tablevel; i++)
                Insert(L'\t');
        }
        break;
	case KEY_SPACE:	Insert(L' '); break;
    case KEY_TAB:
        if(IsSelected()){
            Node *head, *tail;
            bool backtab = shiftkey;
            GetSelectionPos(&head,NULL,NULL,NULL, &tail,NULL,NULL,NULL);
            head = GetLineHead(head,0,NULL);
            tail = GetLineTail(tail,0,0,NULL,NULL)->next;
            AllowDrawing(false);
            DropAnchor();
            while(crt!=head)
                Back();
            while(crt->next!=tail){
                if(crt->prev->ch=='\n' || crt->prev->ch==ETX){
                    if(!backtab){
                        Insert(L'\t');
                    }
                    else if(crt->ch=='\t'){
                        Delete();
                    }
                }
                Forward();
            }
            AllowDrawing(true);
            Repaint();
        }
        else
            Insert(L'\t');
        break;
	case KEY_DELETE:		
        if(IsSelected()) DeleteArea(true);
		else Delete();
		break;
	case KEY_BACKSPACE:		BackSpace(); break;
	case KEY_ALT:		break;
	case KEY_CHAR:
		Insert(chara); 
		break;
	default:
		break;
	}

	shiftkey=false;
}


//１文字進む
void EditorEngine::Forward()
{
	if(crt->next==NULL) return;
	ShowCursor();

    //進める
    curx+=crt->w;
	crt=crt->next;
    curindex++;
    if(curx+crt->w>=wndx || crt->prev->ch=='\n'){
        curx=0;
        cury++;
    }
    if(!shiftkey) DropAnchor();

	//再描画
	if(cury>=wndy-2){
		ScrollDown();
        Slide(-1);
		Repaint(cury-1,cury+2);
	}
	else if(curx==0) Repaint(cury-1,cury);
	else Repaint(cury,cury);
	mx=curx;
}


//１文字戻る
void EditorEngine::Back()
{
    Node *nd;
    int oldx=curx;

    if(crt->prev->ch==ETX) return;
	ShowCursor();

    //戻る
	crt=crt->prev;
    curindex--;
    if(curx==0){//行頭だったなら前の行へ
        cury--;
        //x座標を調べる
        nd=crt;
        do nd=nd->prev; while(nd->ch!='\n' && nd->ch!=ETX);	//まず行頭へ
        nd=nd->next;
        curx=0;
        while(nd!=crt){
            curx+=nd->w;
            nd=nd->next;
            if(curx+nd->w >= wndx) curx=0;
        }
    }
    else{
        curx -= crt->w;
    }
    if(!shiftkey) DropAnchor();

	//スクロールと再描画
	if(cury<0){
		ScrollUp();
        Slide(1);
		Repaint(0,1);
	}
    else if(oldx==0 || crt->ch=='\n') Repaint(cury,cury+1);
	else Repaint(cury,cury);

	mx=curx;
}


//１行進む
void EditorEngine::LineDown()
{
    Node* newcrt=crt;
    Node* nd;
	int x=curx;

	ShowCursor();

    //次の行の先頭まで進める
	while(true){
        if(newcrt->ch==ETX) return;
        if(newcrt->ch=='\n'){ newcrt=newcrt->next; break; }
        x+=newcrt->w;
		if(x>=wndx) break;
        newcrt=newcrt->next;
	}
	cury++; x=0;

    //mxの位置へ進める
	while(true){
		curx=x;
        x+=newcrt->w;
        if(x>mx || newcrt->ch=='\n' || newcrt->ch==ETX) break;
        newcrt=newcrt->next;
	}

    //現在地更新
    for(nd=crt; nd!=newcrt; nd=nd->next) curindex++;
    crt=newcrt;
    if(!shiftkey) DropAnchor();

	//再描画
	if(cury>=wndy-2){
		ScrollDown();
        Slide(-1);
        Repaint(cury-1,cury+2);
	}
    else Repaint(cury-1,cury);
}

//１行戻る
void EditorEngine::LineUp()
{
	int newx, x, prex;	//ｘ位置候補と走査用ｘと一つ前のｘ
    Node *nd, *newcrt;
	if(scroll==0 && cury==0) return;

	ShowCursor();

	//もし一番上の表示行なら、１行スクロールアップする
	if(cury==0){
		ScrollUp();
        Slide(1);
	}

	//xを調べる（xは表示原点から走査）
	x=newx=0;
    nd=newcrt=org->next;
    while(nd!=crt && nd->ch!=ETX){
		prex=x;

		//mx以下の位置で改行があったら候補とする
        if(nd->ch=='\n'){	newx=x;  newcrt=nd;  x=0; }

		//次へ進めて
        x+=nd->w;
        nd=nd->next;
		//もしmx以上なら一つ前のが候補ということになる
		if(x>mx){
			newx=prex;
            newcrt=nd->prev;
			//次の行頭まで進めてしまう
			while(true){
                if(nd->ch==ETX) break;
                if(nd->prev->ch=='\n') break;
                x+=nd->w;
				if(x>=wndx) break;
                nd=nd->next;
			}
			x=0;
		}
	}

    //現在地更新
    for(nd=crt; nd!=newcrt; nd=nd->prev) curindex--;
	crt=newcrt;
	curx=newx;
	cury--;
    if(!shiftkey) DropAnchor();

	Repaint(cury,cury+1);
}



//マウス
//buttonは主イベントを発生させたボタン
//nclicksは、0だとリリース、1でシングル、2でダブル、3でトリプル
//stateはマウスボタンの状態、および修飾キーの状態
void EditorEngine::MouseEvent(int button, int nclicks, int x, int y, unsigned long state)
{
    ReleaseFindingArea();
    shiftkey = state&MODKEY_SHIFT? true: false;

	switch(button){
    case BUTTON_LEFT:
        if(nclicks==1) LeftButtonDownSingle(x, y, state);
        else if(nclicks==2) LeftButtonDownDouble(x, y, state);
        else if(nclicks==3) LeftButtonDownTriple(x, y, state);
        else if(nclicks==0){
            //ボタンリリース
        }
		break;
	case BUTTON_RIGHT:
		break;
	case MOUSE_MOVE:
        //ドラッグ
		if(state&BUTTON_LEFT){
			int oldcury = cury;
			//普通のエディタは枠外にドラッグするとスクロールするのだが、
			//ここではとりあえずしない
			/*
			if(y<0){
				if(ScrollUp()){
                    Slide(1);
				}
			}
			if(y>=(wndy-1)*fontsize) ScrollDown();
			*/
			SetCursor(x,y);

			if(oldcury<cury) Repaint(oldcury, cury);
			else Repaint(cury, oldcury);
		}
		break;
	case BUTTON_WHEELUP:
		WheelUp(x,y, state&BUTTON_LEFT?true:false);
		break;
	case BUTTON_WHEELDOWN:
		WheelDown(x,y, state&BUTTON_LEFT?true:false);
		break;
	default:
		break;
	}

    shiftkey = false;
}


void EditorEngine::LeftButtonDownSingle(int x, int y, unsigned long state)
{
    bool selected = IsSelected(); //変更前の選択状態を保持
	int oldcury = cury;
	
    SetCursor(x,y);
    if(!shiftkey) DropAnchor();

	//前回範囲選択がされていたなら、解除しないとなので全描画する
	//さもなくば差分のみ
    if(selected){
		Repaint();
	}
    else{
        if(oldcury!=cury) Repaint(oldcury, oldcury);
        Repaint(cury,cury);
    }

	//スクロール
	if(cury==wndy-1){
		Repaint(oldcury, oldcury);
		ScrollDown(); 
		ScrollDown();
        Slide(-2);
		Repaint(wndy-2, wndy-1);
	}
	else if(cury==wndy-2){
		Repaint(oldcury, oldcury);
		ScrollDown();
        Slide(-1);
		Repaint(wndy-1, wndy-1);
	}
}


void EditorEngine::LeftButtonDownDouble(int x, int y, unsigned long state)
{
    AllowDrawing(false);
    Back();
    while((iswalpha(crt->ch)||crt->ch=='_') && crt->prev->ch!=ETX){
        Back();
    }
    if(crt->prev->ch!=ETX)
        Forward();
    shiftkey=true;
    while((iswalpha(crt->ch)||crt->ch=='_') && crt->ch!=ETX){
        Forward();
    }
    shiftkey=false;

    AllowDrawing(true);
    Repaint();

}

void EditorEngine::LeftButtonDownTriple(int x, int y, unsigned long state)
{
    AllowDrawing(false);
    Back();
    while(crt->ch!='\n' && crt->prev->ch!=ETX){
        Back();
    }
    if(crt->prev->ch!=ETX)
        Forward();
    shiftkey=true;
    while(crt->ch!='\n' && crt->ch!=ETX){
        Forward();
    }
    shiftkey=false;

    AllowDrawing(true);
    Repaint();
}


void EditorEngine::WheelUp(int x, int y, bool left_button)
{
    int scrolledsize;
	if(scroll==0) return;
    for(scrolledsize=0; scrolledsize<scrollsize ;scrolledsize++){
        if(!ScrollUp()) break;
    }
    if(left_button){
        //ボタンが押されている状態でホイール操作をしたらカーソル位置も変更する
    }
    Slide(scrolledsize);
    Repaint(0, scrolledsize-1);
}


void EditorEngine::WheelDown(int x, int y, bool left_button)
{
    int scrolledsize;
    for(scrolledsize=0;scrolledsize<scrollsize;scrolledsize++){
        if(!ScrollDown()) break;
    }
    if(left_button){
        //ボタンが押されている状態でホイール操作をしたらカーソル位置も変更する
    }
    Slide(-scrolledsize);
    Repaint(wndy-scrolledsize-1, wndy-1);
}




void EditorEngine::SetCursor(int x, int y)
{
    Node* newcrt=org->next;
    int newx=0, newy=0;
    Node *nd;
    int distance=0;
	
	//範囲外なら範囲内に直す
	if(y<0){ y=0; }
    if(y>=wndy*fontheight){ y=wndy*fontheight-1; }
	if(x<0) x=0;
	if(x>=wndx) x=wndx-1;

	//yをピクセルから行数に直す
    y=(int)(y/fontheight);

	//まずｙを合わせる
    while(newy!=y){
        if(newcrt->ch==ETX)
            break;
        if(newcrt->ch=='\n'){
            newy++;
            newx=0;
        }
        newx += newcrt->w;
        newcrt = newcrt->next;
        if(newx+newcrt->w >= wndx){
            newy++;
            newx=0;
		}
	}
	//次にｘ
	while(true){
        if(newcrt->ch==ETX || newcrt->ch=='\n')
            break;
        if(newx+newcrt->w/2 >= x)   //半分を超えたら次のカーソル位置を含める
            break;
        newx+=newcrt->w;
        newcrt=newcrt->next;
    }
    //インデックス
    if(newy<cury || (newy==cury)&&(newx<curx)){
        for(nd=newcrt; nd!=crt; nd=nd->next)
            distance--;
    }
    else{
        for(nd=crt; nd!=newcrt; nd=nd->next)
            distance++;
    }

    //更新
    crt = newcrt;
    curx = newx;
    cury = newy;
    curindex += distance;
	mx=curx;
}




bool EditorEngine::ScrollDown()
{
	Node *n;
	int x=0;

	//次の原点を探す
	n = org->next;
    while(x+n->w<wndx){
		if(n->ch==ETX) return false;	//最後の行なら何もしない
		if(n->ch=='\n'){n=n->next; scrollpara++; break;}
        x+=n->w;
		n=n->next;
	}

	org=n->prev;
	scroll++;
	cury--;
	return true;
}



bool EditorEngine::ScrollUp()
{
    Node* neworg;	//返す候補
	Node* n;	//走査用
	int x=0;

    if(scroll==0 || org->prev==NULL) return false;
    if(org->ch=='\n') scrollpara--;	//原点が段落端
	
	//まず一つ前の改行の次（つまり行頭）まで戻す（とりあえず折り返しは考えない）
	n=org;
	while(n->prev->ch!='\n' && n->prev->ch!=ETX)  n=n->prev;
	
	//次に走査して新しい表示原点を探す
	x=0;
	neworg=n->prev;	//とりあえずの候補
	while(true){
        //前行を全部走査したのなら、候補を返す
		if(n==org){ org=neworg; break; } 

		//文字走査
		if(n->ch=='\n'){ x=0; neworg=n; }
        x+=n->w;
        if(x>=wndx){ neworg=n->prev;  x=n->w; }	//折り返し検出
		n=n->next;
	}
	scroll--;
	cury++;
	return true;
}


//スクロールバー
//valueはスクロール量ではなく絶対行
void EditorEngine::ScrollEvent(int value)
{
    int i;
    int dist = value - scroll;

    //下方スクロール(上方へスライドして下端を更新)
    if(value>scroll){
        for(i=0; i<dist; i++)
            ScrollDown();
        Slide(-dist);
        Repaint(wndy-dist-1, wndy-1);
	}
    //上方スクロール(下方へスライドして上端を更新)
    else if(value<scroll){
        for(i=0; i<-dist; i++)
            ScrollUp();
        Slide(-dist);
        Repaint(0, -dist);
	}
    //value==scrollのときは何もしない
}





//カーソルが画面外にあるとき表示させる
void EditorEngine::ShowCursor()
{
	int i;
    int nscroll;
	
	//画面より上のとき
	if(cury<0){
		nscroll = -cury;
        for(i=0; i<nscroll; i++)
            ScrollUp();
        Repaint();
	}
	
    //下のときは画面外ではなく入力域外のとき表示させる
	if(cury>=wndy-2){
		nscroll = cury-(wndy-3);
		for(i=0; i<nscroll; i++)
            ScrollDown();
        Repaint();
	}
}

void EditorEngine::ResetCursorPos()
{
    crt=root->next;
    curx=cury=mx=0;
	org=root;
    curindex=0;
	scroll=0;
	scrollpara=0;
    DropAnchor();
}


void EditorEngine::Clear()
{
    Node *next;

    curx=cury=mx=0;
    org=root;
    scroll=0;
    scrollpara=0;
    totalpara=0;
    totalline=0;
    chcount = 0;

    crt=root->next;
    while(crt->ch!=ETX){
        next = crt->next;
        ClearAttributes(crt);
        delete crt;
        crt = next;
    }
    crt->prev = root;
    root->next = crt;
    curindex=0;
    DropAnchor();
}

void EditorEngine::SetModified(bool mod)
{
    bool oldstate = modified;
    modified = mod;

    //変更状態が変わったらコールバックする
    if(mod != oldstate && cb_modified!=NULL){
        ((void(*)(EditorEngine*, void*))cb_modified)(this, cb_modifieddat);
    }
}


////////////////////////////////////////////////////////////////////////////////
//
//				範囲選択
//

//選択範囲の現在地の反対端
void EditorEngine::DropAnchor()
{
    anchor = crt;
    ancindex = curindex;
    ancx = curx;
    ancy = cury+scroll; //スクロールを考慮してyは絶対座標で保持
}

bool EditorEngine::IsSelected()
{
    if( crt==anchor ) return false;
	else return true;
}

void EditorEngine::SelectAll()
{
    //先頭にアンカー
    anchor=root->next;
    ancx=0;
    ancy=0;
    ancindex=0;
    //終端にカレント
    while(crt->ch!=ETX){
        if(crt->ch=='\n'){
            cury++;
            curx=0;
        }
        curx += crt->w;
        crt = crt->next;
        curindex++;
        if(curx+crt->w >= wndx){
            cury++;
            curx=0;
        }
    }
    Repaint();
}


//選択範囲の開始ノードおよび座標、終了ノードおよび座標を得る
void EditorEngine::GetSelectionPos(EditorEngine::Node **stn, int *stx, int *sty, int *stindex,
                                   EditorEngine::Node **edn, int *edx, int *edy, int *edindex)
{
    //アンカー始点、現在地終点の場合
    if(ancindex < curindex){
        if(stn!=NULL)*stn=anchor;
        if(stx!=NULL)*stx=ancx;
        if(sty!=NULL)*sty=ancy-scroll;  //ancyは絶対座標なので相対に直す
        if(stindex!=NULL) *stindex=ancindex;
        if(edn!=NULL)*edn=crt;
        if(edx!=NULL)*edx=curx;
        if(edy!=NULL)*edy=cury;
        if(edindex!=NULL)*edindex=curindex;
    }
    //現在地始点、アンカー終点の場合
    else{
        if(stn!=NULL)*stn=crt;
        if(stx!=NULL)*stx=curx;
        if(sty!=NULL)*sty=cury;
        if(stindex!=NULL)*stindex=curindex;
        if(edn!=NULL)*edn=anchor;
        if(edx!=NULL)*edx=ancx;
        if(edy!=NULL)*edy=ancy-scroll;
        if(edindex!=NULL)*edindex=ancindex;
    }
}

////////////////////////////////////////////////////////////////////////////////
//
//				検索と置換
//

//ノードと文字列の比較
bool EditorEngine::nodecmp(Node* n, const std::wstring& str, bool casesens)
{
    std::wstring::const_iterator it;
    if(str.length()==0)
        return false;
	for(it=str.begin(); it!=str.end(); it++){
        if(casesens){
            if(n->ch != *it || n->ch==ETX) return false;
        }
        else{
            if(towupper(n->ch) != towupper(*it) || n->ch==ETX) return false;
        }
		n=n->next;
	}
	return true;
}



void EditorEngine::HighlightFindWord(const std::wstring &word,
                                     bool inselarea, bool casesens, bool regexp)
{
    Node *nd, *endnd, *findend;

    //現在の検索属性を解除
    nd = root->next;
    while(nd!=last){
        RemoveAttribute(nd, atgrpfind->attrib);
        nd = nd->next;
    }

    //属性グループ準備
    boost::wregex pattern;
    boost::match_results<Iterator> results;
    if(regexp){
        pattern.assign(word);
        atgrpfind->attrib->type = ATTR_REGEXP;
        atgrpfind->attrib->regexp = word;
    }
    else{
        atgrpfind->attrib->type = ATTR_WORD;
        atgrpfind->attrib->words.resize(1);
        atgrpfind->attrib->words[0] = word;
    }

    //開始点
    if(inselarea){
        GetSelectionPos(&nd, NULL,NULL,NULL, &findend, NULL,NULL,NULL);
    }
    else{
        nd = root->next;
        findend = last;
    }

    //検索
    //正規表現で検索
    if(regexp){
        //正規表現のときは段落単位で検索する
        do{
            //段落端を探す
            endnd = nd;
            while(endnd->ch!='\n' && endnd!=findend)
                endnd = endnd->next;

            //検索
            while(boost::regex_search(Iterator(nd), Iterator(endnd), results, pattern)){
                for(int i=0; i<results.position(); i++)
                    nd = nd->next;
                for(int i=0; i<results.length(); i++){
                    SetAttribute(nd, atgrpfind, ATTR_RANGEPART_START);
                    nd = nd->next;
                }
            }
            nd = endnd->next;
        }while(endnd!=findend);
    }
    //単語検索
    else{
        while(nd!=findend){
            if( nodecmp(nd, word, casesens)==true ){
                for(int i=0; i<word.length(); i++){
                    SetAttribute(nd, atgrpfind, ATTR_RANGEPART_START);
                    nd = nd->next;
                }
            }
            else
                nd = nd->next;
        }
    }

}


//検索
bool EditorEngine::Find(const std::wstring& word, bool fromtop, bool to_bottom, bool inselarea,
          bool casesens, bool regexp)
{
    Node *nd;
    int findstartx, findstarty, findendx, findendy;

    //検索範囲
    if(inselarea){
        //最初の検索なら新しい選択範囲を検索範囲とし、属性を与える
        if(findstart==NULL || fromtop){
            GetSelectionPos(&findstart, &findstartx, &findstarty, &findstartindex,
                            &findend,   &findendx,   &findendy,   &findendindex);

            for(nd=findstart; nd!=findend; nd=nd->next){
                SetAttribute(nd, atgrpfindarea, ATTR_RANGEPART_NULL);
            }
            fromtop=true;
            if(to_bottom){
                crt = findstart;
                curx = findstartx;
                cury = findstarty;
                curindex = findstartindex;
            }
            else{
                crt = findend;
                curx = findendx;
                cury = findendy;
                curindex = findendindex;
            }
        }
    }
    else{
        findstart = root->next;
        findend = last;
    }

    //初回検索
    if(word!=findcond_word || inselarea!=findcond_inselarea ||
            casesens!=findcond_casesens || regexp!=findcond_regexp)
        HighlightFindWord(word, inselarea, casesens, regexp);
    findcond_word=word;
    findcond_inselarea=inselarea;
    findcond_casesens=casesens;
    findcond_regexp=regexp;

    //前方移動
    if(to_bottom){
        //検索開始位置
        if(fromtop){
            nd = findstart;
        }
        else{
            //前回の検索結果が選択されているので、選択終端から始めないとループしてしまう
            GetSelectionPos(NULL, NULL, NULL, NULL, &nd, NULL, NULL, NULL);
        }

        while(nd!=findend){
            if(HasAttribute(nd,atgrpfind->id))
                break;
            nd = nd->next;
        }
    }
    //後方移動
    else{
        if(fromtop){
            nd = findend;
        }
        else{
            GetSelectionPos(&nd, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        }

        while(true){
            nd = nd->prev;
            if(nd==findstart->prev)
                break;
            if(HasAttribute(nd,atgrpfind->id))
                break;
        }
    }

    //検索成功なら一致した部分を選択する
    if((to_bottom&&nd!=findend) || (!to_bottom&&nd!=findstart->prev)){
        AllowDrawing(false);
        while(crt!=nd)
            to_bottom?Forward():Back();
        DropAnchor();
        shiftkey=true;
        while(HasAttribute(crt,atgrpfind->id))
            Forward();
        shiftkey=false;

        ShowCursor();
        AllowDrawing(true);
        Repaint();
        return true;
    }
    //検索失敗
    else{
        return false;
    }
}


void EditorEngine::ReleaseFindingArea()
{
    //前回の検索範囲が文書全体のときは開始ノードはNULLでないが属性があるので解除する
    if(findstart && HasAttribute(findstart, atgrpfindarea->id)){
        for(Node *nd=findstart; nd!=findend; nd=nd->next){
            RemoveAttribute(nd, atgrpfindarea->id);
        }
        Repaint();
    }
    findstart=NULL;
    findend=NULL;
}


//逐次置換
//初回時は必ずskip=trueで呼ぶかFind()を使い、置換文字列を選択しておくこと
bool EditorEngine::Replace(const wchar_t* find, const wchar_t* rep, bool skip)
{
    /*
	unsigned int i;

	if(!skip){
		//skipでない時は必ず置換元文字列が選択されているものとする
		DeleteArea();
		for(i=0; rep[i]!='\0'; i++) Insert(rep[i]);
	}
	//次の置換候補
	if( Find(find, false,true) == false ) return false;
	return true;
    */
    return true;
}

int EditorEngine::ReplaceAll(const std::wstring &find, const std::wstring &rep, bool alltext)
{
    Node *ndselst, *ndseled;
    int selstx, selsty, selstidx;
    std::list<wchar_t> recstr;
    Node *nd;

    unsigned int i, count=0;

    AllowDrawing(false);

    //開始地点へ持っていく
    if(alltext)
        ResetCursorPos();
    else{
        GetSelectionPos(&ndselst, &selstx, &selsty, &selstidx, &ndseled, NULL, NULL, NULL);
        crt = ndselst;
        curx = selstx;
        cury = selsty;
        curindex = selstidx;
        ShowCursor();
    }

    //置換する
    while(true){
        if( (!alltext && crt==ndseled) ||
            (alltext && crt->ch==ETX) ) break;

        if( nodecmp(crt, find, true)==true ){
            count++;
            //削除
            recstr.clear();
            nd = crt;
            for(i=0; i<find.size(); i++){   //後に正規表現に対応させるため実テキストを保存
                recstr.push_back(nd->ch);
                nd = nd->next;
            }
            RecordUndo(UU_REPLACE, recstr);
            DropAnchor();
            shiftkey=true;
            for(i=0; i<find.size(); i++){
                Forward();
            }
            DeleteArea(false);
            shiftkey=false;
            //挿入
            recstr = wstolist(rep);
            RecordUndo(UU_REPLACE, recstr);
            Insert(rep, false);
            for(i=0; i<rep.size(); i++){
                Forward();
            }
        }
        else Forward();
    }

    recstr.clear();
    RecordUndo(UU_REPLACEEND, recstr);
    AllowDrawing(true);
    ShowCursor();
    Repaint();
    return count;
}



////////////////////////////////////////////////////////////////////////////////
//
//				表示情報取得
//

//表示原点の取得
Character* EditorEngine::BeginScan(int mode)
{
	scanx=0;
	scanmode=mode;
	if(scanmode==SCAN_ROOT){
		scannode=root;
		scany=0;
	} 
	else if(scanmode==SCAN_SCREEN){
		scannode=org;
		scany=scroll;
	}
	else if(scanmode==SCAN_SELECTED){
        GetSelectionPos(&scannode,NULL,NULL,NULL, &endnode,NULL,NULL,NULL);
		//ProceedScanは次へ進めるので、ひとつ前を選択しておく必要がある
		dummyorgnode.next = scannode;
		scannode = &dummyorgnode;
	}
	return ProceedScan();	//org,rootは実際には表示原点の一つ前のノードなので進める
}


//ノードを進める（コメントや強調単語などもここで判別）
Character* EditorEngine::ProceedScan()
{
    int i,j;
    BelongingGroupList::iterator bgit;

	if(scannode->next==NULL){
		memset(scanret,0,sizeof(Character));
		return scanret;
	}
	//進める
	scannode=scannode->next;
	scanret->ch = scannode->ch;
	
	//出力用ノードを作る

    //全スキャン
    if(scanmode==SCAN_ROOT){
        ;
    }
    //選択範囲スキャン
	if(scanmode==SCAN_SELECTED){
		if(scannode==endnode){
			scanret->ch=ETX;
		}
	}
    //表示用スキャンのときは属性を付加する
    else if(scanmode==SCAN_SCREEN){
		scanret->ch = scannode->ch;
        scanret->w= scannode->w;

		//スクリーンスキャンのときは文字の位置情報を付与する
		if(scanmode==SCAN_SCREEN){
			if(scanx+scanret->w >= wndx){
				scanret->x = 0;
				scanret->line = scany+1-scroll;
			}
			else{
				scanret->x = scanx;
				scanret->line = scany-scroll;
			}
		}
		else{
			scanret->x = 0;
			scanret->line = 0;
        }
		
		//属性判定
        //通常テキスト(属性を使わない、またはない場合)
        if(attribset.size()==0 || scannode->blattrgrps.size()==0){
            scanret->r=attrtext->r; scanret->g=attrtext->g; scanret->b=attrtext->b;
            scanret->br=attrtext->br; scanret->bg=attrtext->bg; scanret->bb=attrtext->bb;
            scanret->bold = attrtext->bold;
            scanret->italic = attrtext->italic;
            scanret->underline = attrtext->underline;
		}
		else{
			EditorAttribute *topattr=NULL;
            unsigned int level=9999;
			//最上位の属性を採用する
            for(bgit=scannode->blattrgrps.begin(); bgit!=scannode->blattrgrps.end(); bgit++){
                BelongingGroup *blg = (*bgit);
                for(j=0; j<attribset.size(); j++){
                    if((*bgit)->group->attrib == &attribset.at(j)){
						if(j<level){
                            topattr = (*bgit)->group->attrib;
							level = j;
						}
						break;
					}
				}
            }
            scanret->r=topattr->r;  scanret->g=topattr->g;  scanret->b=topattr->b;
            scanret->br=topattr->br;  scanret->bg=topattr->bg;  scanret->bb=topattr->bb;
            scanret->bold = topattr->bold;
		}

        //現在地か
        scanret->current = false;
        if(scannode == crt){
            scanret->current = true;
            EditorAttribute *pattr = FindAttribute(ATTR_CURSOR);
            if(pattr->id!=ATTR_NULLID){
                scanret->r = pattr->r;
                scanret->g = pattr->g;
                scanret->b = pattr->b;
                if(pattr->br!=TRANSPARENT_COLOR){
                    scanret->br = pattr->br;
                    scanret->bg = pattr->bg;
                    scanret->bb = pattr->bb;
                }
            }
        }

        //選択範囲内か
		int sx,sy,ex,ey;
		bool insel=false;
        scanret->selection = false;
        GetSelectionPos(NULL,&sx, &sy,NULL, NULL, &ex, &ey,NULL);
		sy+=scroll; ey+=scroll;
		if(scany==sy){
			if(ey==sy && sx<=scanx && scanx<ex) insel=true;
			if(ey>sy && sx<=scanx) insel=true;
		}
		else if(scany>sy){
			if(scany<ey) insel=true;
			if(scany==ey && scanx<ex) insel=true; 
		}
        //選択範囲であれば背景色を上書き、文字色は透過でなければ上書き
        if(insel){
            scanret->selection = true;
            scanret->br = attrsel->br;
            scanret->bg = attrsel->bg;
            scanret->bb = attrsel->bb;
            if(attrsel->r!=TRANSPARENT_COLOR){
                scanret->r = attrsel->r;
                scanret->g = attrsel->g;
                scanret->b = attrsel->b;
            }
        }
	
		//スキャンカーソルを進める
		scanx+=scanret->w;
		if(scanx>=wndx || scanret->ch=='\n'){ 
			scany++; 
			scanx=scanret->w; 
		}
	}

	//そして渡す
	return scanret;
}

//行番号の表示テーブルを返す。
//暫定的にこの関数を使っているが、２度ノード走査をすることになるので、
//速度アップのためにはこの関数を使わず描画と同時に行番号を表示したほうがよい。
int* EditorEngine::GetLineNumber(bool LFonly)
{
	//折り返しも１行とする場合
	if(!LFonly){
		for(int i=0;i<wndy;i++) rettable[i]=scroll+i;
        rettable[wndy] = -2;
		return rettable;
	}
	
    //折り返しを１行としない場合
	Node* n=org->next;
	int cntline=1,x=0,y=0;
	
	//最初の行は常に表示
	if(org->ch!='\n' && org->ch!=ETX) rettable[y]=-1;
	else rettable[y]=scrollpara;
	y++;
	
	//２行目以降
	while(y<wndy)
	{
        x+=n->w;
        if(x>=wndx){
            rettable[y]=-1;
            y++;
            x=n->w;
        }
        if(n->ch=='\n'){
            rettable[y]=scrollpara+cntline;
            cntline++;
            y++;
            x=0;
        }
		if(n->ch==ETX){
            //残りを埋める
            for(;y<wndy;y++){
				rettable[y]=scrollpara+cntline;
				cntline++;
			}
			break;
		}
		n=n->next;
    }
	return rettable;
}



void EditorEngine::GetCursor(int* x, int* y)
{
	*x=curx; *y=cury;
}



////////////////////////////////////////////////////////////////////////////////
//
//				Undo
//

//削除、または挿入の記録
void EditorEngine::RecordUndo(int inputtype, std::list<wchar_t> & chlist)
{
    UndoRecord *lastrec=NULL;

    //前回の記録とタイプが違う場合、現在地が変更された場合に、
    //Undo単位を確定させ新しくUndo単位を作る
    if( (prerec_type!=inputtype && inputtype!=UU_REPLACEEND)
            || (inputtype==UU_INS && (curindex != prerec_index+1))
            || (inputtype==UU_DEL && (curindex != prerec_index))
            || (inputtype==UU_BS  && (curindex != prerec_index-1))
            ){
        //次のUndo単位へ進める
        crtuu++;
        enduu=crtuu;
        if(crtuu==maxundo){ crtuu=enduu=0; }
        if(crtuu==startuu) startuu++;
        if(startuu==maxundo) startuu=0;

        //Undo単位はサイクリックな使い回しなので最も古いものを消して初期化する
        undounit[crtuu].records.clear();
        if(inputtype!=UU_REPLACE){
            undounit[crtuu].records.push_front(UndoRecord());
            undounit[crtuu].records.front().type=inputtype;

            //BS以外の時は最初の位置が元に戻す位置になるのでここで位置確定
            //BSの時は入力の都度更新
            if(inputtype==UU_DEL || inputtype==UU_INS){
                undounit[crtuu].records.front().index = curindex;
            }
        }
    }

	//インプットの種類によって変更情報を保存
	switch(inputtype)
	{
	case UU_BS:
        lastrec = &undounit[crtuu].records.front();
		//BSの時は常に元に戻す位置を更新
        lastrec->index = curindex;
		//連結（一番最初に挿入）
        lastrec->str.splice(lastrec->str.begin(), chlist);
		break;
	case UU_DEL:
	case UU_INS:
        lastrec = &undounit[crtuu].records.front();
		//連結（一番最後に追加）
        lastrec->str.splice(lastrec->str.end(), chlist);
		break;
    case UU_REPLACE:
        undounit[crtuu].records.push_front(UndoRecord());
        lastrec = &undounit[crtuu].records.front();
        if((undounit[crtuu].records.size()-1)%2==0)
            lastrec->type=UU_DEL;
        else
            lastrec->type=UU_INS;
        lastrec->index = curindex;
        lastrec->str = chlist;
        break;
	case UU_NULL:
	default:
		break;
	}
	
    //現在の状態を保存して次回単位確定の判定に使う
    prerec_index=curindex;
    prerec_type=inputtype;
}


void EditorEngine::Replay(const EditorEngine::UndoUnit &uu, bool undo)
{
    int optype;
    std::list<UndoRecord>::const_iterator it;
    if(undo)
        it=uu.records.begin();
    else
        it=uu.records.end();

    while(true){
        //redoの時の終端判定とイテレータ
        if(!undo){
            if(it==uu.records.begin())
                break;
            it--;
        }
        else if(undo && it==uu.records.end()){
            break;
        }

        //UndoとRedoで操作を逆にする
        if(it->type==UU_BS || it->type==UU_DEL){
            if(undo) optype = UU_DEL;
            else optype = UU_INS;
        }
        else if(it->type==UU_INS){
            if(undo) optype = UU_INS;
            else optype = UU_DEL;
        }

        //操作を行う位置まで進める
        if(it->index < curindex){
            crt=root->next;
            cury=-scroll;
            curx=0;
            curindex=0;
        }
        while(curindex!=it->index){
            curx+=crt->w;
            if(crt->ch=='\n'){ curx=0; cury++; }
            crt=crt->next;
            if(curx+crt->w>=wndx){ curx=0; cury++; }
            curindex++;
        }

        //削除の取り消し（挿入）
        if(optype==UU_DEL){
            DropAnchor();
            Insert(listtows(it->str), false);
        }
        //挿入の取り消し（削除）
        if(optype==UU_INS){
            DropAnchor();
            shiftkey=true;
            for(int i=0;i<it->str.size();i++)
                Forward();
            shiftkey=false;
            DeleteArea(false);
        }

        //undoの時の終端判定とイテレータ
        if(undo)
            it++;
    }
}



void EditorEngine::Undo()
{
	if(crtuu==startuu) return;

    //取り消し処理
    AllowDrawing(false);
    Replay(undounit[crtuu],true);
    AllowDrawing(true);
    ShowCursor();
    Repaint();

	//Undo単位を一つ戻す
	crtuu--;
	if(crtuu==-1) crtuu=maxundo-1;
}

void EditorEngine::Redo()
{
	if(crtuu==enduu) return;

	//Undo単位を進める
	crtuu++;
	if(crtuu==maxundo) crtuu=0;

    //取り消し処理
    AllowDrawing(false);
    Replay(undounit[crtuu],false);
    AllowDrawing(true);
	Repaint();
}


void EditorEngine::IsUndoable(bool *undoable, bool *redoable)
{
	if(crtuu!=enduu) *redoable=true;
	else *redoable=false;

	if(crtuu!=startuu) *undoable=true;
	else *undoable=false;
}



////////////////////////////////////////////////////////////////////////////////
//
//				属性
//



void EditorEngine::ClearAttributes(EditorEngine::Node *n)
{
    BelongingGroupList::iterator bgit;
    AttributeGroupList::iterator agit;
    AttributeGroup *grp;

    for(bgit=n->blattrgrps.begin(); bgit!=n->blattrgrps.end();){
        //グループの参照カウントを減らし、0になったらグループを削除
        grp = (*bgit)->group;
        grp->refcount--;
        if(grp->refcount==0){
            for(agit=attrgrplist.begin(); agit!=attrgrplist.end(); agit++ ){
                if((*agit)->id == grp->id){
                    delete (*agit);
                    attrgrplist.erase(agit);
                    break;
                }
            }
        }
        //所属を削除
        delete (*bgit);
        bgit = n->blattrgrps.erase(bgit);
    }
}


//指定された範囲で指定されたグループの属性を解除する
void EditorEngine::RemoveAttribute(Node* start, Node* end, int id)
{
    Node *nd = start;
    do{
        RemoveAttribute(nd,id);
        nd=nd->next;
    }while(nd!=end && nd->ch!=ETX);
}

bool EditorEngine::RemoveAttribute(EditorEngine::Node *n, int grpid)
{
    BelongingGroupList::iterator bgit;
    AttributeGroupList::iterator agit;
    AttributeGroup *grp;

    for(bgit=n->blattrgrps.begin(); bgit!=n->blattrgrps.end(); bgit++){
        grp = (*bgit)->group;
        if(grp->id == grpid){
            //グループの参照カウントを減らし、0になったらグループを削除
            grp->refcount--;
            if(grp->refcount==0){
                for(agit=attrgrplist.begin(); agit!=attrgrplist.end(); agit++ ){
                    if((*agit)->id == grp->id){
                        delete (*agit);
                        attrgrplist.erase(agit);
                        break;
                    }
                }
            }

            //所属グループから削除
            delete (*bgit);
            n->blattrgrps.erase(bgit);
            return true;
        }
    }
    return false;
}

bool EditorEngine::RemoveAttribute(EditorEngine::Node *n, EditorAttribute *attr)
{
    BelongingGroupList::iterator bgit;
    AttributeGroupList::iterator agit;
    AttributeGroup *grp;

    for(bgit=n->blattrgrps.begin(); bgit!=n->blattrgrps.end(); bgit++){
        grp = (*bgit)->group;
        if(grp->attrib == attr){
            //グループの参照カウントを減らし、0になったらグループを削除
            grp->refcount--;
            if(grp->refcount==0){
                for(agit=attrgrplist.begin(); agit!=attrgrplist.end(); agit++ ){
                    if((*agit)->id == grp->id){
                        delete (*agit);
                        attrgrplist.erase(agit);
                        break;
                    }
                }
            }

            //所属グループから削除
            delete (*bgit);
            n->blattrgrps.erase(bgit);
            return true;
        }
    }
    return false;
}



bool EditorEngine::HasAttribute(Node *n, int grpid)
{
    BelongingGroupList::iterator bgit;
    for(bgit=n->blattrgrps.begin(); bgit!=n->blattrgrps.end(); bgit++){
        if((*bgit)->group->id == grpid) return true;
    }
    return false;
}

int EditorEngine::GetRangePart(EditorEngine::Node *n, int grpid)
{
    BelongingGroupList::iterator bgit;
    for(bgit=n->blattrgrps.begin(); bgit!=n->blattrgrps.end(); bgit++){
        if((*bgit)->group->id == grpid) return (*bgit)->rangepart;
    }
    return ATTR_RANGEPART_NULL;
}


int EditorEngine::GetRangePart(EditorEngine::Node *n, EditorAttribute* attr)
{
    BelongingGroupList::iterator bgit;
    for(bgit=n->blattrgrps.begin(); bgit!=n->blattrgrps.end(); bgit++){
        if((*bgit)->group->attrib == attr) return (*bgit)->rangepart;
    }
    return ATTR_RANGEPART_NULL;
}


void EditorEngine::SetRangePart(EditorEngine::Node *n, int grpid, int rangepart)
{
    BelongingGroupList::iterator bgit;
    for(bgit=n->blattrgrps.begin(); bgit!=n->blattrgrps.end(); bgit++){
        if((*bgit)->group->id == grpid){
            (*bgit)->rangepart = rangepart;
        }
    }
}


//ノードに指定IDの属性がない場合は追加、IDが重複する場合は変更する
void EditorEngine::SetAttribute(Node *nd, AttributeGroup* attrgrp, int rangepart)
{
    BelongingGroupList::iterator bgit;

    for(bgit=nd->blattrgrps.begin(); bgit!=nd->blattrgrps.end(); bgit++){
        if((*bgit)->group->id == attrgrp->id){
            (*bgit)->rangepart = rangepart;
            return;
        }
    }

    //重複IDが見つからなかったら追加
    BelongingGroup *blgrp = new BelongingGroup();
    blgrp->rangepart = rangepart;
    blgrp->group = attrgrp;
    blgrp->group->refcount++;
    nd->blattrgrps.push_back(blgrp);
}


//削除、挿入どちらにも使える属性再構築
//stnodeは挿入開始ノード、削除の時はNULL
//needrepaintは全体再描画フラグ
//シンボルの破壊か新規作成があればこれをtrueにする
//それ以外の場合は挿入ノードのみの更新なので属性の有無にかかわらず再描画される
//このフラグはfalseに初期化されないので、もし呼び出し前にtrueならtrueを維持する
void EditorEngine::RebuildAttribute(EditorEngine::Node *stnode, bool *needrepaint)
{
    BelongingGroupList::iterator bgit;
    Node *nd;
    Node *head, *tail;
    int i,j;
    std::map<int, AttributeGroup*> destroymap;
    std::map<int, AttributeGroup*>::iterator destroyit;
    std::map<std::wstring, EditorAttribute*> samesymbolattrmap;
    std::map<std::wstring, EditorAttribute*>::iterator samesymbolattrit;

    //削除時は挿入開始位置を現在地と同一とすることで同じアルゴリズムを使えるようになる
    if(stnode == NULL)
        stnode = crt;

    //行頭行末を得る
    head = stnode;
    while(head->prev->ch!='\n' && head->prev->ch!=ETX) head=head->prev;
    tail = crt;
    while(tail->ch!='\n' && tail->ch!=ETX) tail=tail->next;

    //スタートシンボル破壊判定
    /*　後方判定、前方判定、両方判定の3回の判定が必要。3回の判定はandを取り、一つでも破壊判定があれば破壊と判断。
     * $:挿入開始ノード、挿入文字が1文字の場合や削除時は、現在位置(^)と同じ
     * ^:現在地ノード
    1.後方判定:$の一つ後方がシンボルであれば、シンボルの文字数戻り、その位置のノードがシンボルであれば破壊していない
    2.前方判定:^自身がシンボルであれば、シンボルの文字数-1進み、その位置のノードがシンボルであれば破壊していない
    3.両方判定:^自身がシンボルでなく、かつ$の一つ後方がその属性を持たなければ破壊された
    */
    //後方
    destroymap.clear();
    for(bgit=stnode->prev->blattrgrps.begin(); bgit!=stnode->prev->blattrgrps.end(); bgit++){
        if(destroymap.count((*bgit)->group->id)!=0)
            continue;
        if((*bgit)->rangepart != ATTR_RANGEPART_START)
            continue;
        nd = stnode;
        std::wstring symbol;
        if((*bgit)->group->wordindex<0)
            symbol = (*bgit)->group->attrib->stsym;
        else
            symbol = (*bgit)->group->attrib->words[(*bgit)->group->wordindex];
        for(i=0; i<symbol.length() && nd->ch!=ETX; i++)
            nd = nd->prev;
        if(GetRangePart(nd, (*bgit)->group->id)!=ATTR_RANGEPART_START)
            destroymap[(*bgit)->group->id] = (*bgit)->group;
    }
    //前方および両方
    for(bgit=crt->blattrgrps.begin(); bgit!=crt->blattrgrps.end(); bgit++){
        if(destroymap.count((*bgit)->group->id)!=0)
            continue;
        //前方
        if((*bgit)->rangepart == ATTR_RANGEPART_START){
            nd = crt;
            std::wstring symbol;
            if((*bgit)->group->wordindex<0)
                symbol = (*bgit)->group->attrib->stsym;
            else
                symbol = (*bgit)->group->attrib->words[(*bgit)->group->wordindex];
            for(i=0; i<symbol.length()-1 && nd->ch!=ETX; i++)
                nd = nd->next;
            if(GetRangePart(nd, (*bgit)->group->id)!=ATTR_RANGEPART_START)
                destroymap[(*bgit)->group->id] = (*bgit)->group;
        }
        //両方
        else{
            if(!HasAttribute(stnode->prev, (*bgit)->group->id))
                destroymap[(*bgit)->group->id] = (*bgit)->group;
        }
    }
    //破壊処理
    if(destroymap.size()!=0)
        *needrepaint = true;
    for(destroyit=destroymap.begin(); destroyit!=destroymap.end(); destroyit++){
        int grpid = (*destroyit).first;
        AttributeGroup *grp = (*destroyit).second;

        //同一シンボル属性は後で処理
        if(grp->attrib->samesymbol){
            samesymbolattrmap[grp->attrib->stsym] = grp->attrib;
            continue;
        }

        //後方
        nd = stnode->prev;
        while(RemoveAttribute(nd, grpid))
            nd = nd->prev;
        //前方
        nd = crt;
        while(RemoveAttribute(nd, grpid))
            nd = nd->next;
    }

    //エンドシンボル破壊判定
    /*
     * $:挿入開始ノード、挿入文字が1文字の場合や削除時は、現在位置と同じ
     * ^:現在地ノード
    1.後方判定:$の一つ後方がシンボルであれば、シンボルの文字数戻り、その位置のノードがシンボルであれば破壊していない
    2.前方判定:^自身がシンボルであれば、シンボルの文字数-1進み、その位置のノードがシンボルであれば破壊していない
    3.両方判定:$の一つ後方がシンボルでなく、かつ^自身がその属性を持たなければ破壊された
    */
    //後方および両方
    destroymap.clear();
    for(bgit=stnode->prev->blattrgrps.begin(); bgit!=stnode->prev->blattrgrps.end(); bgit++){
        if(destroymap.count((*bgit)->group->id)!=0)
            continue;
        //後方
        if((*bgit)->rangepart == ATTR_RANGEPART_END){
            nd = stnode;
            for(i=0; i<(*bgit)->group->attrib->edsym.length() && nd->ch!=ETX; i++)
                nd = nd->prev;
            if(GetRangePart(nd, (*bgit)->group->id)!=ATTR_RANGEPART_END)
                destroymap[(*bgit)->group->id] = (*bgit)->group;
        }
        //両方
        else{
            if(!HasAttribute(crt, (*bgit)->group->id))
                destroymap[(*bgit)->group->id] = (*bgit)->group;
        }
    }
    //前方
    for(bgit=crt->blattrgrps.begin(); bgit!=crt->blattrgrps.end(); bgit++){
        if(destroymap.count((*bgit)->group->id)!=0)
            continue;
        if((*bgit)->rangepart != ATTR_RANGEPART_END)
            continue;
        nd = crt;
        for(i=0; i<(*bgit)->group->attrib->edsym.length()-1 && nd->ch!=ETX; i++)
            nd = nd->next;
        if(GetRangePart(nd, (*bgit)->group->id)!=ATTR_RANGEPART_END)
            destroymap[(*bgit)->group->id] = (*bgit)->group;
    }
    //破壊処理
    if(destroymap.size()!=0)
        *needrepaint = true;
    for(destroyit=destroymap.begin(); destroyit!=destroymap.end(); destroyit++){
        int grpid = (*destroyit).first;
        AttributeGroup *grp = (*destroyit).second;

        //同一シンボル属性は後で処理
        if(grp->attrib->samesymbol){
            samesymbolattrmap[grp->attrib->stsym] = grp->attrib;
            continue;
        }
        //破壊された後方エンドシンボルをミドルに変更
        nd = stnode->prev;
        while(GetRangePart(nd,grpid)==ATTR_RANGEPART_END){
            SetRangePart(nd,grpid,ATTR_RANGEPART_MIDDLE);
            nd = nd->prev;
        }
        //破壊された前方エンドシンボルの属性を一時的に削除
        //こうすることで挿入ノードと元々のエンドシンボルを同等に扱うことができる
        nd = crt;
        while(HasAttribute(nd,grpid)){
            RemoveAttribute(nd, grpid);
            nd = nd->next;
        }
        //挿入位置から後方へ属性付与しつつ新たなエンドシンボルを探す
        nd = stnode;
        std::wstring endsymbol = (*destroyit).second->attrib->edsym;
        for(i=0; i<endsymbol.length()-1; i++){
            nd = nd->prev;
            if(nd->ch==ETX){
                nd = nd->next;
                break;
            }
        }
        while(nd->ch!=ETX){
            if(!nodecmp(nd, endsymbol, true)){
                SetAttribute(nd, (*destroyit).second, ATTR_RANGEPART_MIDDLE);
                nd = nd->next;
                continue;
            }
            else{
                //終了シンボルを発見したので、エンドパートに指定
                for(i=0; i<endsymbol.length(); i++, nd = nd->next){
                    SetAttribute(nd, (*destroyit).second, ATTR_RANGEPART_END);
                }
                break;
            }
        }
    }

    //挿入されたミドルパートに属性付与
    /*
     * $:挿入開始ノード、挿入文字が1文字の場合や削除時は、現在位置と同じ
     * ^:現在地ノード(すでに属性が付与されている)
    ^とその一つ後方の所属属性グループのマッチングを行い、相違があれば挿入と判断、
        割り入れた文字列にはその属性が付与される
    */
    BelongingGroupList::iterator bgit1, bgit2;
    for(bgit1=stnode->prev->blattrgrps.begin(); bgit1!=stnode->prev->blattrgrps.end(); bgit1++){
        //範囲属性以外を除外
        if((*bgit1)->group->attrib->type!=ATTR_RANGE)
            continue;
        //一つ後方が終了シンボルは除外
        if((*bgit1)->rangepart == ATTR_RANGEPART_END)
            continue;

        //現在地と一つ後方のマッチング
        for(bgit2=stnode->blattrgrps.begin(); bgit2!=stnode->blattrgrps.end(); bgit2++){
            if((*bgit1)->group->id == (*bgit2)->group->id)
                break;
        }
        //相違があった場合
        if(bgit2 == stnode->blattrgrps.end()){
            for(nd=stnode; nd!=crt; nd = nd->next){
                SetAttribute(nd, (*bgit1)->group, ATTR_RANGEPART_MIDDLE);

                //改行が挿入された場合は同一シンボル属性か調べる
                if((*bgit1)->group->attrib->samesymbol && nd->ch=='\n'){
                    samesymbolattrmap[(*bgit1)->group->attrib->stsym]
                            = (*bgit1)->group->attrib;
                }
            }
        }
    }



    //新規終了シンボル
    /*
     * 挿入開始ノードの持つ属性について、終了シンボルを構成するか調べる
     * この時点で挿入ノードには仮としてミドルパートが前段で割り当てられている
    */
    //シンボルを探す
    Node *attrstart;
    for(bgit=stnode->blattrgrps.begin(); bgit!=stnode->blattrgrps.end(); bgit++){
        std::wstring endsymbol = (*bgit)->group->attrib->edsym;

        //範囲属性以外を除外
        if((*bgit)->group->attrib->type!=ATTR_RANGE)
            continue;

        //初期位置まで戻す
        nd = stnode;
        for(i=0; i<endsymbol.length()-1; i++){
            nd = nd->prev;
            if(nd->ch==ETX){
                nd = nd->next;
                break;
            }
        }
        //新規構築の終了シンボルを探す
        while(nd!=crt && nd->ch!=ETX){
            //シンボルでなければ次の文字へ
            if(!nodecmp(nd, endsymbol, true)){
                nd = nd->next;
                continue;
            }

            //以下シンボルと一致の場合

            //同一シンボル属性は後で処理
            if((*bgit)->group->attrib->samesymbol){
                samesymbolattrmap[endsymbol] = (*bgit)->group->attrib;
                break;
            }

            //呼び出し元の再描画が必要
            *needrepaint = true;
            //終了シンボルを発見したので、エンドパートに指定
            for(i=0; i<endsymbol.length(); i++, nd = nd->next){
                SetAttribute(nd, (*bgit)->group, ATTR_RANGEPART_END);
            }
            //これ以降の属性は削除
            while(RemoveAttribute(nd, (*bgit)->group->id)){
                nd = nd->next;
            }
            break;
        }
    }


    //新規開始シンボル判定、および新規属性グループ作成
    /*
     *  //(要書き換え)
        start, endは、insertのときは挿入範囲、deleteの時はcrtとcrt->next
        このとき新規スタートシンボルを作るか調べる範囲は、
        たとえばスタートシンボルが３文字だったとしたら、

                %%%%%%%
        xxxxxxxx#xxxxxx$xxxxxxx
              '''''''''''
        この'の範囲になる。(%は挿入範囲、#はstart、$はend)
    */
    int wordindex=0;
    for(j=0; j<attribset.size(); ){
        AttributeGroup* attrgrp;
        EditorAttribute *attr = &attribset.at(j);
        std::vector<std::wstring> words = attr->words;
        std::wstring startsymbol = attr->stsym;
        std::wstring endsymbol = attr->edsym;

        if(attr->type == ATTR_WORD && words.size()>0)
            startsymbol = words[wordindex];
        else if(attr->type == ATTR_RANGE)
            wordindex = -1;
        else if(attr->type == ATTR_REGEXP){ //正規表現は後で
            j++;
            continue;
        }


        //挿入開始位置からスタートシンボルの文字数-1戻った位置を走査開始点とする
        attrstart = stnode;
        for(i=0; i<startsymbol.length()-1; i++){
            attrstart = attrstart->prev;
            if(attrstart->ch==ETX){
                attrstart = attrstart->next;
                break;
            }
        }
        while(attrstart!=crt && attrstart->ch!=ETX){
            nd = attrstart;

            //開始シンボルを作らないときは次へ
            if(!nodecmp(nd, startsymbol, true)){
                attrstart = attrstart->next;
                continue;
            }

            //同一シンボル属性は後で処理
            if(attr->samesymbol){
                samesymbolattrmap[startsymbol] = attr;
                break;
            }

            //呼び出し元の再描画が必要
            *needrepaint = true;

            //開始シンボルまたは単語を発見したので新規に属性グループを作る
            attrgrp = new AttributeGroup();
            attrgrp->id = GenerateUniqueID();
            attrgrp->attrib = attr;
            attrgrp->refcount = 0;
            attrgrp->wordindex = wordindex;
            attrgrplist.push_back(attrgrp);

            //開始シンボルを付与
            for(i=0; i<startsymbol.length(); i++, nd = nd->next){
                SetAttribute(nd, attrgrp, ATTR_RANGEPART_START);
            }

            //範囲属性のときは終了シンボルまで属性を付与する
            if(attr->type==ATTR_RANGE){
                while(nd->ch!=ETX){
                    //終了位置が見つかった
                    if(nodecmp(nd, endsymbol, true)){
                        for(i=0; i<endsymbol.length(); i++, nd = nd->next){
                            SetAttribute(nd, attrgrp, ATTR_RANGEPART_END);
                        }
                        break;
                    }
                    //見つからないのでミドルパートの属性を付加して続ける
                    else{
                        SetAttribute(nd, attrgrp, ATTR_RANGEPART_MIDDLE);
                        nd = nd->next;
                    }
                }
            }

            //走査開始位置を1文字進める
            attrstart = attrstart->next;

            //ここでbreakはしない
            // Cタイプのコメントを例にすると"/*aaa/*bbb" というような挿入も考えられるため
        }

        //単語属性でないときは次の属性へ
        if(words.size()==0){
            j++;
            wordindex = 0;
        }
        //単語属性のときは次の単語へ、全部の単語を調べたら次の属性へ
        else{
            wordindex++;
            if(wordindex==words.size()){
                j++;
                wordindex=0;
            }
        }
    }

    //新規開始シンボル正規表現
    //正規表現は文字数が何文字か不定なので、段落単位で調べる必要がある
    for(j=0; j<attribset.size(); j++){
        AttributeGroup* attrgrp;
        EditorAttribute *attr = &attribset.at(j);
        if(attr->type != ATTR_REGEXP)
            continue;
        if(attr->regexp.empty())
            continue;

        boost::wregex pattern(attr->regexp);
        boost::match_results<Iterator> results;

        //段落単位で検索
        Node *endnd;
        nd = head;
        do{
            //段落端を探す
            endnd = nd;
            while(endnd->ch!='\n' && endnd!=tail)
                endnd = endnd->next;

            //検索
            while(boost::regex_search(Iterator(nd), Iterator(endnd), results, pattern)){
                for(int i=0; i<results.position(); i++)
                    nd = nd->next;
                attrgrp = new AttributeGroup();
                attrgrp->id = GenerateUniqueID();
                attrgrp->attrib = attr;
                attrgrp->refcount = 0;
                attrgrp->wordindex = -1;
                attrgrplist.push_back(attrgrp);
                for(int i=0; i<results.length(); i++){
                    SetAttribute(nd, atgrpfind, ATTR_RANGEPART_START);
                    nd = nd->next;
                }
            }
            nd = endnd->next;
        }while(endnd!=tail);
    }


    //同一シンボル属性の処理
    //同一シンボル属性は段落単位で処理する
    for(samesymbolattrit=samesymbolattrmap.begin(); samesymbolattrit!=samesymbolattrmap.end();
            samesymbolattrit++){
        AttributeGroup* attrgrp;
        EditorAttribute *attr = (*samesymbolattrit).second;
        std::wstring symbol = attr->stsym;

        //まず消す
        for(nd=head; nd!=tail; nd=nd->next){
            RemoveAttribute(nd, attr);
        }

        //新規作成
        for(nd=head; nd!=tail; nd=nd->next){
            if(!nodecmp(nd, symbol, true)){
                continue;
            }

            //開始シンボルを発見したので新規に属性グループを作る
            attrgrp = new AttributeGroup();
            attrgrp->id = GenerateUniqueID();
            attrgrp->attrib = attr;
            attrgrp->refcount = 0;
            attrgrp->wordindex = -1;
            attrgrplist.push_back(attrgrp);

            //開始シンボルを付与
            for(i=0; i<symbol.length(); i++, nd = nd->next){
                SetAttribute(nd, attrgrp, ATTR_RANGEPART_START);
            }
            //終了シンボルまで付与する
            //挿入文字列に\nが入る可能性があるためtailに加えて判定が必要
            while(nd!=tail && nd->ch!='\n'){
                //終了位置が見つかった
                if(nodecmp(nd, symbol, true)){
                    for(i=0; i<symbol.length(); i++, nd = nd->next){
                        SetAttribute(nd, attrgrp, ATTR_RANGEPART_END);
                    }
                    break;
                }
                //見つからないのでミドルパートの属性を付加して続ける
                else{
                    SetAttribute(nd, attrgrp, ATTR_RANGEPART_MIDDLE);
                    nd = nd->next;
                }
            }
            //ここで終了判定を入れないとループが戻った時nextされる
            if(nd==tail)
                break;
        }
    }


}







////////////////////////////////////////////////////////////////////////////////
//
//				ヘルパー関数
//



//属性グループを作るためユニークIDを生成する
unsigned long EditorEngine::GenerateUniqueID() //static
{
	//[prov]
	//きわめて簡単なことしかやってないので、ここは改良する必要がある
	static unsigned long id=0;
	id++;
	return id;
}



//行の最初のノード、および必要であればy位置を得る
EditorEngine::Node* EditorEngine::GetLineHead(Node *basenode, int basey, int *heady)
{
    int x, y;
    Node *nd, *head;

    //行頭ノードを探す
    nd = basenode;
    while(nd->prev->ch!='\n' && nd->prev->ch!=ETX) nd=nd->prev;
    head = nd;

    //必要であれば行の開始y座標を調べる
    if(heady!=NULL){
        x=y=0;
        while(nd!=basenode){
            x+=nd->w;
            nd = nd->next;
            if(x+nd->w>=wndx || nd->prev->ch=='\n'){
                x=0;
                y++;
            }
        }
        *heady = basey-y;
    }
    return head;
}


//行の最後のノード(\nかETX)、および必要であれば位置を得る
EditorEngine::Node* EditorEngine::GetLineTail(
        Node *basenode, int basex, int basey, int *tailx, int *taily)
{
    int x, y;
    Node *nd, *tail;

    //最初に末端ノードだけ探しておく
    nd = basenode;
    while(nd->ch!='\n' && nd->ch!=ETX) nd=nd->next;
    tail = nd;

    //必要であれば末端のx,y座標を調べる
    if(tailx!=NULL || taily!=NULL){
        nd = basenode;
        x=basex;
        y=basey;
        while(nd!=tail){
            x+=nd->w;
            nd = nd->next;
            if(x+nd->w>=wndx || nd->prev->ch=='\n'){
                x=0;
                y++;
            }
        }
        if(tailx) *tailx=x;
        if(taily) *taily=y;
    }
    return tail;
}



std::wstring join(const std::vector<std::wstring>& strs, const wchar_t& separator)
{
    std::wstring ret;
    for(int i=0; i<strs.size(); i++){
        ret += strs[i];
        if(i!=strs.size()-1)
            ret += separator;
    }
    return ret;
}





std::vector<std::wstring> split(const std::wstring &str, const wchar_t &separator)
{
    std::vector<std::wstring> ret;
    std::wstring::const_iterator it;

    std::wstring item;
    for(it=str.begin(); it!=str.end(); it++){
        if(*it == separator){
            ret.push_back(item);
            item.clear();
        }else{
            item += *it;
        }
    }
    ret.push_back(item);
    return ret;
}

/*
std::wstring &trim(std::wstring &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}



void trim(std::vector<std::wstring> &s)
{
    for(int i=0; i<s.size(); i++)
        trim(s[i]);
}*/


std::wstring escape(const std::wstring &s)
{
    std::wstring ret;
    std::wstring::const_iterator it;
    for(it=s.begin(); it!=s.end(); it++){
        if(*it==L' ')
            ret+=L"\\s";
        else if(*it==L'\t')
            ret+=L"\\t";
        else if(*it==L'\r')
            ret+=L"\\r";
        else if(*it==L'\n')
            ret+=L"\\n";
        else if(*it==L'\\')
            ret+=L"\\\\";
        else
            ret+=*it;
    }
    return ret;
}



std::wstring unescape(const std::wstring &s)
{

}






std::wstring listtows(const std::list<wchar_t> &list)
{
    std::wstring ret;
    std::list<wchar_t>::const_iterator it;
    for(it=list.begin(); it!=list.end(); it++){
        ret.push_back(*it);
    }
    return ret;
}


std::list<wchar_t> wstolist(const std::wstring &str)
{
    std::list<wchar_t> ret;
    std::wstring::const_iterator it;
    for(it=str.begin(); it!=str.end(); it++){
        ret.push_back(*it);
    }
    return ret;
}
