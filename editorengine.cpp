//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <ctype.h>
#include <iostream>
#include <cwctype>
#include <list>
#include <vector>
#include <regex>

#include "editorengine.h"


char EditorEngine::charwidths[65536];   //static

//接続されたコールバック関数とおまけポインタ
static void* cb_slide=NULL;		//void cbfunc(int dist, void* dat)
static void* cb_slidedat=NULL;
static void* cb_repaint=NULL;		//void cbfunc(int start, int end, void* dat)
static void* cb_repaintdat=NULL;
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
    return NULL;
}

void EditorEngine::SetWidthsTable(char *table)  //static
{
    memcpy(charwidths, table, 64*1024);
    charwidths['\n']=0;
}

bool EditorEngine::IsValidRegexp(const std::wstring re)    //static
{
    std::wregex pattern;
    try{
        pattern.assign(re);
    }catch(std::regex_error& e){
        return false;
    }
    return true;
}


EditorEngine::EditorEngine()
{
    //段落とカーソル
    rootprg = new Paragraph;
    crt.paragraph = new Paragraph;
    terminalprg = new Paragraph;
    rootprg->next = crt.paragraph;
    crt.paragraph->prev=rootprg;
    crt.paragraph->text.push_back(ETX);
    crt.paragraph->nlines=1;
    crt.paragraph->next = terminalprg;
    terminalprg->prev = crt.paragraph;
    anchor.paragraph = crt.paragraph;
    org.paragraph = crt.paragraph;

    //ウィンドウ情報
    wndx = 640;
    wndy = 25;
    tabsize = 16*8;
    fontheight = 16;
    mx = 0;
    nparagraphs = 2;
    ntotallines = 0;
    scroll = 0;
    memset(linenos, 0, sizeof(linenos));
    modified = false;
    scrollsize=1;
    autoindent=false;
    showlineno=true;
    drawablelevel=0;
    shiftkey=false;

    //属性
    attrtext = NULL;
    attrsel = NULL;
    attrfind = NULL;

    //Undo関連
    maxundo=4;
    undounit=new UndoUnit[maxundo];
    crtuu=enduu=startuu=maxundo-1;
    prerec_type=UU_NULL;
}

EditorEngine::~EditorEngine()
{
    Clear();
    delete terminalprg;
    delete rootprg;
}


void EditorEngine::SetAttributeSet(const EditorAttributeSet &as)
{
    EditorAttributeSet::iterator asit;
    std::vector<std::wstring> dummy;
    attribset = as;

    //不正な正規表現を除外
    asit = attribset.begin();
    while(asit!=attribset.end()){
        if(asit->type==ATTR_REGEXP && !IsValidRegexp(asit->regexp)){
            asit = attribset.erase(asit);
        }
        else{
            asit++;
        }
    }


    //デフォルト属性を追加
    for(int i=0; i<defattrs.size(); i++){
        if(FindAttribute(defattrs[i].id)==NULL)
            attribset.push_back(defattrs[i]);
    }

    //必須属性が存在しなければここでハードコーディングで入れる
    if(!FindAttribute(ATTR_TEXT))
        attribset.push_back(EditorAttribute(
                ATTR_TEXT, L"text", ATTR_NULL,
                0,0,0, -1,-1,-1, 0,0,0, dummy, 0) );
    if(!FindAttribute(ATTR_FIND))
        attribset.push_back(EditorAttribute(
                ATTR_FIND, L"find", ATTR_NULL,
                0,0,0,  255,192,128,  0,0,0, dummy, 999) );
    if(!FindAttribute(ATTR_SELECTION))
        attribset.push_back(EditorAttribute(
                ATTR_SELECTION, L"selection", ATTR_NULL,
                -1,-1,-1,  128,192,255,  0,0,0, dummy, 999) );

    //再度特殊属性のポインタ取得
    attrtext = FindAttribute(ATTR_TEXT);
    attrsel = FindAttribute(ATTR_SELECTION);
    attrfind = FindAttribute(ATTR_FIND);
}




void EditorEngine::SetViewParams(int tabsize, int wndxpx, int wndypx, int fontheight)
{
    int nlines;

    //画面内の表示行数を計算
    nlines = (int)(wndypx/fontheight);
    if(wndypx%fontheight!=0) nlines++;

    //ビューのパラメータに変更がなければ位置の再計算をせず返る
    if(this->tabsize==tabsize && this->wndx==wndxpx
            && this->wndy==nlines && this->fontheight == fontheight)
        return;

    //ビューパラメータ更新
    this->tabsize=tabsize;
    this->wndx=wndxpx;
    this->wndy=nlines;
    this->fontheight=fontheight;

    //現在の状態を作り直す
    ntotallines = 0;
    for(Paragraph *prg=rootprg->next; prg!=terminalprg; prg=prg->next){
        prg->nlines = 0;
        CalcParagraphLines(prg);
    }
    //表示原点
    org.hy = 0;
    for(Paragraph *prg=rootprg->next; prg!=org.paragraph; prg=prg->next){
        org.hy += prg->nlines;
    }
    scroll = org.hy;
    org.ry = 0;
    org.index = 0;
    org.x = 0;
    crt = org;
    DropAnchor();
    Repaint();
}




//再描画
void EditorEngine::Repaint(){Repaint(0,wndy-1);}
void EditorEngine::Repaint(int start, int end)
{
    if(drawablelevel<0 || cb_repaint==NULL) return;
    if(start<0) start=0;
    if(end>=wndy) end=wndy-1;
    ((void(*)(int,int,void*))cb_repaint)(start, end, cb_repaintdat);

    //[debug]
    //((void(*)(int,int,bool,void*))cb_repaint)(0, wndy-1, true, cb_repaintdat);
}


//描画域のスクロール
//イベントのスクロールと混同しないようslideという名前にした
void EditorEngine::Slide(int dist)
{
    if(drawablelevel<0 || cb_slide==NULL) return;
    ((void(*)(int,void*))cb_slide)(dist, cb_slidedat);
}



void EditorEngine::AllowDrawing(bool allow)
{
    if(allow)
        drawablelevel++;
    else
        drawablelevel--;
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

//文字列の挿入連結のみ行う
//引数posの位置は挿入後の位置に更新される
EditorEngine::Position EditorEngine::InsertPrimitive(const Position& pos, const std::wstring &str)
{
    Position workpos = pos;
    std::wstring reststr;
    int startidx=0, endidx=0, sublen=0;

    //現在段落の現在位置以降の部分を別に保持する
    sublen = workpos.paragraph->text.length() - workpos.index;
    reststr = workpos.paragraph->text.substr(workpos.index, sublen);
    workpos.paragraph->text.erase(workpos.index, sublen);

    //改行ごとに切り出して段落を作り連結
    while(true){
        endidx = str.find(L'\n', startidx);
        //改行を検出
        if(endidx!=std::string::npos){
            sublen = endidx-startidx+1;
            workpos.paragraph->text.append(str.substr(startidx, sublen));
            Paragraph *newprg = new Paragraph;
            ConcatParagraphsAfter(workpos.paragraph, newprg, newprg);
            CalcParagraphLines(workpos.paragraph);
            workpos.paragraph = newprg;
            workpos.prgidx++;
            workpos.index = 0;
            startidx = endidx+1;
        }
        //文字列終端を検出
        else{
            endidx = str.length();
            sublen = endidx-startidx;
            workpos.paragraph->text.append(str.substr(startidx, sublen));
            workpos.index += sublen;
            //最後の段落に保持していた残り部分を追加
            workpos.paragraph->text.append(reststr);
            CalcParagraphLines(workpos.paragraph);
            break;
        }
    }
    return workpos;
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
    bool repaintall=false, needrepaint=false;
    Position workpos;
    Cursor oldcrt;
    std::wstring reststr;
    int startidx=0, endidx=0, sublen=0;
    int oldntotallines = ntotallines;

    if(str.length()==0)
        return;

    AllowDrawing(false);

    //範囲選択時は解除するため無条件で全描画
    repaintall |= IsSelected();

    SetModified(true);
    ShowCursor();
    DeleteArea(recording);
    oldcrt = crt;

    //Undo情報を記録
    if(recording){
        RecordUndo(UU_INS,crt,str);
    }

    //テキスト挿入
    workpos = (Position)crt;
    workpos = InsertPrimitive(workpos, str);

    //カーソル位置合わせ
    int w = 0;
    crt.ry = 0;
    crt.x = 0;
    crt.index = 0;
    while(crt.paragraph != workpos.paragraph){
        crt.hy += crt.paragraph->nlines;
        crt.paragraph = crt.paragraph->next;
        crt.prgidx++;
    }
    while(crt.index != workpos.index){
        crt.x += CharWidth(crt);
        crt.index++;
        w = CharWidth(crt);
        if(crt.x+w >= wndx){
            crt.x = 0;
            crt.ry++;
        }
    }
    mx = crt.x;
    DropAnchor();

    //属性更新
    RebuildAttribute(&oldcrt, &crt, &needrepaint);
    repaintall |= needrepaint;

    //描画
    if(oldntotallines != ntotallines)
        repaintall = true;
    AllowDrawing(true);
    if(repaintall)
        Repaint();
    else
        Repaint(oldcrt.hy-scroll, crt.hy+crt.paragraph->nlines-1-scroll);

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
    if(crt.paragraph->prev==rootprg && crt.index==0)
        return;
    DropAnchor();
    shiftkey=true;
    Back();
    shiftkey=false;
    AllowDrawing(true);

    //Undo記録、削除
    RecordUndo(UU_BS, crt, crt.paragraph->text.substr(crt.index,1));

    DeleteArea(false);
}

void EditorEngine::ConcatParagraphsBefore(Paragraph *base, Paragraph *inshead, Paragraph *instail)
{
    inshead->prev = base->prev;
    if(base->prev)
        base->prev->next = inshead;
    instail->next = base;
    base->prev = instail;

}

void EditorEngine::ConcatParagraphsAfter(Paragraph *base, Paragraph *inshead, Paragraph *instail)
{
    if(base->next)
        base->next->prev = instail;
    instail->next = base->next;

    inshead->prev = base;
    base->next = inshead;
}


//段落内行数を調べ、段落インスタンスが保持している行数を更新する
//さらに文書内総行数も更新する
void EditorEngine::CalcParagraphLines(EditorEngine::Paragraph *prg)
{
    int w;
    int textlen = prg->text.length();
    Cursor cur;
    cur.paragraph = prg;

    if(textlen==0){
        prg->nlines=1;
        return;
    }

    ntotallines -= prg->nlines;

    while(true){
        cur.x += CharWidth(cur);
        cur.index++;
        if(cur.index == textlen)
            break;
        w = CharWidth(cur);
        if(cur.x+w >= wndx){
            cur.x = 0;
            cur.ry++;
        }
    }
    prg->nlines = cur.ry+1;

    ntotallines += prg->nlines;
}


//PositionをCusorに変換する
//posはbase以降に存在すること
EditorEngine::Cursor EditorEngine::PosToCur(const Position &pos, const Cursor &base)
{
    Cursor cur = base;
    cur.ry = 0;
    cur.x = 0;
    cur.index = 0;
    while(cur.paragraph != pos.paragraph){
        cur.hy += cur.paragraph->nlines;
        cur.paragraph = cur.paragraph->next;
        cur.prgidx++;
    }
    while(cur.index != pos.index){
        cur.x += CharWidth(cur);
        cur.index++;
        if(cur.x + CharWidth(cur) >= wndx){
            cur.x = 0;
            cur.ry++;
        }
    }
    return cur;
}



int EditorEngine::CharWidth(const Cursor& cur)
{
    if(cur.index >= cur.paragraph->text.length())
        return 0;

    wchar_t ch = cur.paragraph->text[cur.index];
    if(ch=='\t'){
        if(cur.x==0)
            return tabsize;
        else
            return tabsize - cur.x%tabsize;
    }
    else
        return EditorEngine::charwidths[ch];
}



//区切られた単語か調べる
bool EditorEngine::IsWholeWord(const Position &pos, int len)
{
    wchar_t ch;
    if(pos.index>=1){
        ch = pos.paragraph->text[pos.index-1];
        if(std::iswalnum(ch) || ch=='_')
            return false;
    }
    if(pos.index+len <= pos.paragraph->text.length()-1){
        ch = pos.paragraph->text[pos.index+len];
        if(std::iswalnum(ch) || ch=='_')
            return false;
    }
    return true;
}


//テキスト消去、消去したテキストを返す
std::wstring EditorEngine::DeletePrimitive(
        EditorEngine::Position &stpos, EditorEngine::Position &edpos)
{
    std::wstring reststr;
    Paragraph *workprg;
    std::wstring deletedstr;

    //開始段落と終了段落の削除されないテキスト連結
    reststr = edpos.paragraph->text.substr(
                edpos.index, edpos.paragraph->text.length()-edpos.index);
    if(stpos.paragraph == edpos.paragraph)
        deletedstr.append( stpos.paragraph->text.substr(
            stpos.index, edpos.index-stpos.index) );
    else
        deletedstr.append( stpos.paragraph->text.substr(
            stpos.index, stpos.paragraph->text.length()-stpos.index) );
    stpos.paragraph->text.erase(
        stpos.index, stpos.paragraph->text.length()-stpos.index);
    stpos.paragraph->text.append(reststr);
    //2段目以降を削除
    workprg = stpos.paragraph->next;
    Paragraph *edposnext = edpos.paragraph->next;
    while(workprg!=edposnext){
        Paragraph *nextprg = workprg->next;
        ntotallines -= workprg->nlines;
        if(workprg!=edpos.paragraph)
            deletedstr.append(workprg->text);
        else
            deletedstr.append(workprg->text.substr(0, edpos.index));
        delete workprg;
        workprg = nextprg;
    }
    //stとed->nextの段落連結
    stpos.paragraph->next = edposnext;
    edposnext->prev = stpos.paragraph;

    CalcParagraphLines(stpos.paragraph);
    return deletedstr;
}


//現在のカーソルのある文字を１字消す
void EditorEngine::Delete()
{
    if(crt.paragraph->text[crt.index]==ETX) return;

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
    Cursor selst, seled;
    Paragraph *workprg;
    std::wstring reststr;
    std::wstring delstr;
    int noldtotallines = ntotallines;

    if(!IsSelected())
        return;
    AllowDrawing(false);
    GetSelectionArea(&selst, &seled);
    crt = selst;
    anchor = seled;

    //表示原点ノードを削除されないよう表示原点を開始位置より前にもってくる
    ShowCursor();

    //削除
    delstr = DeletePrimitive(selst, seled);

    //現在地の段落を選択開始地点の段落に
    crt.hy = selst.hy;
    crt.prgidx = selst.prgidx;
    crt.paragraph = selst.paragraph;

    //カーソル位置の再計算
    crt.ry = 0;
    crt.x = 0;
    crt.index = 0;
    while(crt.index != selst.index){
        crt.x += CharWidth(crt);
        crt.index++;
        if(crt.x+CharWidth(crt) >= wndx){
            crt.x = 0;
            crt.ry++;
        }
    }
    mx=crt.x;
    DropAnchor();

    //Undo記録
    if(recording)
        RecordUndo(UU_DEL, crt, delstr);

    //属性更新
    bool needrepaint;
    RebuildAttribute(&crt, &crt, &needrepaint);

    //描画
    AllowDrawing(true);

    if(needrepaint)
        Repaint();
    else{
        int cury = crt.hy + crt.ry - scroll;
        if(noldtotallines == ntotallines){
            Repaint(cury-1, crt.hy-scroll + crt.paragraph->nlines-1);
        }
        else
            Repaint();
    }

}





//キー入力
void EditorEngine::KeyEvent(int keycode, wchar_t chara, unsigned long state)
{
    int i;

    shiftkey = state&MODKEY_SHIFT? true: false;

    switch(keycode){
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
        while(crt.index!=0) Back();
        Repaint(crt.hy+crt.ry-scroll, crt.hy+crt.ry-scroll);
        break;
    case KEY_END:
        while(crt.index!=crt.paragraph->text.length()-1) Forward();
        Repaint(crt.hy+crt.ry-scroll, crt.hy+crt.ry-scroll);
        break;

    //入力系
    case KEY_ENTER:
        autoindent=true;
        if(!autoindent)
            Insert(L'\n');
        else{
            int count=0;
            wchar_t indentchar=0;
            if(crt.paragraph->text.at(0)=='\t')
                indentchar = '\t';
            else if(crt.paragraph->text.at(0)==' ')
                indentchar = ' ';
            while(crt.paragraph->text.at(count)==indentchar)
                count++;
            Insert(L'\n');
            if(indentchar!=0){
                for(int i=0; i<count; i++)
                    Insert(indentchar);
            }
        }
        break;
    case KEY_SPACE:	Insert(L' '); break;
    case KEY_TAB:
        if(IsSelected()){
            bool backtab = shiftkey;
            Cursor selst, seled;
            GetSelectionArea(&selst, &seled);
            AllowDrawing(false);
            crt = selst;
            DropAnchor();   //選択を解除しないとInsertで選択範囲が削除される
            while(crt.index!=0)
                Back();
            while(crt.prgidx != seled.prgidx+1){
                if(!backtab)
                    Insert(L'\t');
                else if(crt.paragraph->text.at(crt.index) == '\t')
                    Delete();
                do{
                    Forward();
                }while(crt.index!=0);
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
    if(crt.paragraph->text[crt.index]==ETX) return;
    ShowCursor();

    //進める
    crt.x += CharWidth(crt);
    crt.index++;
    if(crt.index == crt.paragraph->text.length()){
        crt.hy += crt.paragraph->nlines;
        crt.paragraph = crt.paragraph->next;
        crt.prgidx++;
        crt.index = 0;
        crt.ry = 0;
        crt.x = 0;
    }
    else if(crt.x+CharWidth(crt) >= wndx){
        crt.x = 0;
        crt.ry++;
    }
    if(!shiftkey) DropAnchor();


    //再描画
    int cury = crt.hy + crt.ry - scroll;
    if(cury>=wndy-2){
        ScrollDown();
        Slide(-1);
        cury--;
        Repaint(cury-1,cury+2);
    }
    else if(crt.x==0) Repaint(cury-1,cury);
    else Repaint(cury,cury);

    mx=crt.x;
}


//１文字戻る
void EditorEngine::Back()
{
    int w;
    int destidx;
    int oldx = crt.x;

    if(crt.paragraph->prev==rootprg && crt.index==0) return;
    ShowCursor();

    //戻る
    destidx = crt.index-1;
    if(destidx<0){
        crt.paragraph = crt.paragraph->prev;
        crt.prgidx--;
        crt.hy -= crt.paragraph->nlines;
        destidx = crt.paragraph->text.length()-1;
    }

    //座標を調べる
    crt.ry = 0;
    crt.x = 0;
    crt.index = 0;
    while(crt.index != destidx){
        crt.x += CharWidth(crt);
        crt.index++;
        w = CharWidth(crt);
        if(crt.x+w >= wndx){
            crt.x = 0;
            crt.ry++;
        }
    }

    if(!shiftkey) DropAnchor();

    //スクロールと再描画
    int cury = crt.hy + crt.ry - scroll;
    if(cury<0){
        ScrollUp();
        Slide(1);
        Repaint(0,1);
    }
    else if(oldx==0 || crt.paragraph->text[crt.index]=='\n') Repaint(cury,cury+1);
    else Repaint(cury,cury);

    mx=crt.x;
}


//１行進む
void EditorEngine::LineDown()
{
    wchar_t ch;
    int w;
    ShowCursor();

    //次の行の先頭まで進める
    if(crt.ry == crt.paragraph->nlines-1){
        if(crt.paragraph->next == terminalprg)
            return;
        crt.hy += crt.paragraph->nlines;
        crt.paragraph = crt.paragraph->next;
        crt.prgidx++;
        crt.index = 0;
        crt.x = 0;
        crt.ry = 0;
    }
    else{
        while(true){
            crt.x += CharWidth(crt);
            if(crt.x >= wndx){
                crt.ry++;
                crt.x=0;
                break;
            }
            crt.index++;
        }
    }

    //mxの位置へ進める
    while(true){
        ch = crt.paragraph->text[crt.index];
        w = CharWidth(crt);
        if(crt.x+w>mx || ch=='\n' || ch==ETX) break;
        crt.x += w;
        crt.index++;
    }
    if(!shiftkey) DropAnchor();

    //再描画
    int cury = crt.hy + crt.ry - scroll;
    if(cury>=wndy-2){
        ScrollDown();
        Slide(-1);
        cury--;
        Repaint(cury-1,cury+2);
    }
    else{
        Repaint(cury-1,cury);
    }
}

//１行戻る
void EditorEngine::LineUp()
{
    wchar_t ch=0;
    int w;
    int dstry;

    if(crt.paragraph->prev == rootprg && crt.ry==0)
        return;

    ShowCursor();

    //もし一番上の表示行なら、１行スクロールアップする
    if(crt.hy+crt.ry-scroll==0){
        ScrollUp();
        Slide(1);
    }

    //現在の段落の先頭行なら後方の段落の先頭へ戻る
    //そうでなければ現在の段落の先頭へ戻る
    if(crt.ry==0){
        crt.paragraph = crt.paragraph->prev;
        crt.prgidx--;
        crt.hy -= crt.paragraph->nlines;
        dstry = crt.paragraph->nlines-1;
    }
    else{
        dstry = crt.ry-1;
    }
    crt.ry = 0;
    crt.index = 0;
    crt.x = 0;

    //xを調べる
    while(true){
        w = CharWidth(crt);
        ch = crt.paragraph->text[crt.index];

        //発見か
        if((crt.ry==dstry && crt.x+w > mx) || ch=='\n'){
            break;
        }

        //次へ
        crt.x += w;
        crt.index++;
        w = CharWidth(crt);
        if(crt.x+w >= wndx){
            crt.x = 0;
            crt.ry++;
        }
    }

    if(!shiftkey) DropAnchor();
    int cury = crt.hy + crt.ry -scroll;
    Repaint(cury,cury+1);
}



//マウス
//buttonは主イベントを発生させたボタン
//nclicksは、0だとリリース、1でシングル、2でダブル、3でトリプル
//stateはマウスボタンの状態、および修飾キーの状態
void EditorEngine::MouseEvent(int button, int nclicks, int x, int y, unsigned long state)
{
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
            int oldcury = crt.hy + crt.ry - scroll;
            int cury;
            //普通のエディタは枠外にドラッグするとスクロールするのだが、
            //それは上位層で実装する
            SetCursor(x,y);
            cury = crt.hy + crt.ry - scroll;

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
    int oldcury = crt.hy + crt.ry - scroll;//cury;
    int cury;

    SetCursor(x,y);
    if(!shiftkey) DropAnchor();
    cury = crt.hy + crt.ry - scroll;

    //前回範囲選択がされていたなら、解除しないとなので全描画する
    //さもなくば差分のみ
    if(selected){
        Repaint();
    }
    else{
        if(oldcury!=cury && oldcury>=0 && oldcury <wndy)
            Repaint(oldcury, oldcury);
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
    wchar_t ch;
    int chartype = 0;

    ch = crt.paragraph->text.at(crt.index);
    if(isalnum(ch) || ch=='_')
        chartype = 'A'; //数字、アルファベットとアンダーバー
    else if(ispunct(ch) && ch!='_')
        chartype = 'P'; //アンダーバーを除く記号文字
    else if(ch >= 0x0100)
        chartype = 'Z'; //全角文字

    //選択範囲先頭を探す
    while(true){
        if(crt.index==0)
            break;
        ch = crt.paragraph->text.at(crt.index-1);
        if(chartype == 'A' && !(isalnum(ch) || ch=='_'))
            break;
        else if(chartype == 'P' && !(ispunct(ch) && ch!='_'))
            break;
        else if(chartype == 'Z' && !(ch >= 0x0100))
            break;
        Back();
    }
    //選択範囲終端を探す
    shiftkey=true;
    while(true){
        if(crt.index==crt.paragraph->text.length()-1)
            break;
        ch = crt.paragraph->text.at(crt.index);
        if(chartype == 'A' && !(isalnum(ch) || ch=='_'))
            break;
        else if(chartype == 'P' && !(ispunct(ch) && ch!='_'))
            break;
        else if(chartype == 'Z' && !(ch >= 0x0100))
            break;
        Forward();
    }
    shiftkey=false;

    AllowDrawing(true);
    debugstr += chartype;
    Repaint();
}

void EditorEngine::LeftButtonDownTriple(int x, int y, unsigned long state)
{
    AllowDrawing(false);
    while(crt.index!=0)
        Back();
    shiftkey=true;
    while(crt.index!=crt.paragraph->text.length()-1)
        Forward();
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
    Cursor newcrt;
    int absy;

    //範囲外なら範囲内に直す
    if(y<0){ y=0; }
    if(y>=wndy*fontheight){ y=wndy*fontheight-1; }
    if(x<0) x=0;
    if(x>=wndx) x=wndx-1;

    //yをピクセルから行数に直す
    y=(int)(y/fontheight);

    //y絶対座標
    absy = y + scroll;

    //まずｙを合わせる
    newcrt = org;
    while(newcrt.hy + newcrt.paragraph->nlines <= absy && newcrt.paragraph->next!=terminalprg){
        newcrt.hy += newcrt.paragraph->nlines;
        newcrt.paragraph = newcrt.paragraph->next;
        newcrt.prgidx++;
        newcrt.ry = 0;
        newcrt.x = 0;
        newcrt.index = 0;
    }
    while(newcrt.hy + newcrt.ry != absy){
        wchar_t ch = newcrt.paragraph->text[newcrt.index];
        if(ch==ETX || ch=='\n')
            break;
        newcrt.x += CharWidth(newcrt);
        newcrt.index++;
        if(newcrt.x + CharWidth(newcrt) >= wndx){
            newcrt.ry++;
            newcrt.x=0;
        }
    }
    //次にｘ
    while(true){
        wchar_t ch = newcrt.paragraph->text[newcrt.index];
        if(ch==ETX || ch=='\n')
            break;
        int w = CharWidth(newcrt);
        if(newcrt.x + w/2 >= x)   //半分を超えたら次のカーソル位置を含める
            break;
        newcrt.x += w;
        newcrt.index++;
    }

    //更新
    crt = newcrt;
    mx = newcrt.x;
}




bool EditorEngine::ScrollDown()
{
    //次の原点を探す
    if(org.ry==org.paragraph->nlines-1){
        if(org.paragraph->next == terminalprg) //最後の行なら何もしない
            return false;
        org.hy += org.paragraph->nlines;
        org.paragraph = org.paragraph->next;
        org.ry = 0;
        org.index = 0;
        org.prgidx++;
    }
    else{
        while(true){
            org.x += CharWidth(org);
            org.index++;
            if(org.x + CharWidth(org) >= wndx){
                org.ry++;
                org.x=0;
                break;
            }
        }
    }
    scroll++;
    return true;
}



bool EditorEngine::ScrollUp()
{
    if(scroll==0) return false;

    int newry;
    if(org.ry==0){
        org.paragraph = org.paragraph->prev;
        org.hy -= org.paragraph->nlines;
        newry = org.paragraph->nlines-1;
        org.prgidx--;
    }
    else{
        newry = org.ry-1;
    }
    org.ry = 0;
    org.index = 0;
    while(org.ry!=newry){
        org.x += CharWidth(org);
        org.index++;
        if(org.x + CharWidth(org) >= wndx){
            org.ry++;
            org.x=0;
        }
    }
    scroll--;
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
    int cury = crt.hy + crt.ry - scroll;

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
    org.paragraph = rootprg->next;
    org.prgidx = 0;
    org.index = 0;
    org.hy = 0;
    org.ry = 0;
    org.x = 0;
    scroll = 0;
    crt = org;
    DropAnchor();
}


void EditorEngine::Clear()
{
    Paragraph *prg;
    //削除(rootとterminalを除く)
    for(prg=rootprg->next->next; prg!=NULL; prg=prg->next){
        delete prg->prev;
    }
    //再構築
    scroll = 0;
    ntotallines = 0;
    nparagraphs = 0;
    rootprg->next = new Paragraph();
    crt = Cursor();
    crt.paragraph = rootprg->next;
    crt.paragraph->prev = rootprg;
    crt.paragraph->text.push_back(ETX);
    crt.paragraph->nlines=1;
    crt.paragraph->next = terminalprg;
    terminalprg->prev = crt.paragraph;
    org = Cursor();
    org.paragraph = crt.paragraph;

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
}

bool EditorEngine::IsSelected()
{
    if( crt.paragraph==anchor.paragraph && crt.index==anchor.index ) return false;
    else return true;
}

void EditorEngine::SelectAll()
{
    //先頭にアンカー
    anchor.paragraph = rootprg->next;
    anchor.index = 0;
    anchor.hy = 0;
    anchor.ry = 0;
    anchor.x = 0;
    //カレント位置を終端に
    crt = anchor;
    while(crt.paragraph->next!=terminalprg){
        crt.hy += crt.paragraph->nlines;
        crt.paragraph = crt.paragraph->next;
        crt.prgidx++;
    }
    while(crt.paragraph->text[crt.index] != ETX){
        crt.x += CharWidth(crt);
        crt.index++;
        if(crt.x + CharWidth(crt) >= wndx){
            crt.x=0;
            crt.ry++;
        }
    }
    Repaint();
}


//選択範囲の開始ノードおよび座標、終了ノードおよび座標を得る
void EditorEngine::GetSelectionArea(Cursor* stpos, Cursor* edpos)
{
    int ancabsy = anchor.hy + anchor.ry;
    int crtabsy = crt.hy + crt.ry;

    //アンカー始点、現在地終点の場合
    if((ancabsy<crtabsy) || (ancabsy==crtabsy && anchor.x<crt.x)){
        if(stpos) *stpos = anchor;
        if(edpos) *edpos = crt;
    }
    //現在地始点、アンカー終点の場合
    else{
        if(stpos) *stpos = crt;
        if(edpos) *edpos = anchor;
    }
}

std::wstring EditorEngine::GetSelectedText()
{
    Cursor st, ed;
    Paragraph *prg;
    int idx1, idx2;
    std::wstring ret;

    GetSelectionArea(&st,&ed);
    prg=st.paragraph;
    while(prg!=ed.paragraph->next){
        if(prg==st.paragraph)
            idx1 = st.index;
        else
            idx1 = 0;
        if(prg==ed.paragraph)
            idx2 = ed.index;
        else
            idx2 = prg->text.length();
        ret.append(prg->text.substr(idx1, idx2-idx1));
        prg = prg->next;
    }

    return ret;
}


////////////////////////////////////////////////////////////////////////////////
//
//				検索と置換
//




void EditorEngine::HighlightFindWord(const std::wstring &word, bool casesens, bool regexp)
{
    Paragraph *prg = rootprg->next;

    //検索
    //正規表現で検索
    if(regexp){
        std::wregex pattern(word);
        std::match_results<std::wstring::iterator> results;
        attrfind->type = ATTR_REGEXP;
        attrfind->regexp = word;
        for(prg=rootprg->next; prg!=terminalprg; prg=prg->next){
            std::wstring::iterator it = prg->text.begin();
            int stidx = 0;
            while(std::regex_search(it, prg->text.end(), results, pattern)){
                AttributeGroup* attrgrp = new AttributeGroup();
                attrgrp->id = GenerateUniqueID();
                attrgrp->attr = attrfind;
                attrgrp->start.paragraph = prg;
                attrgrp->start.index = results.position() + stidx;
                attrgrp->end.paragraph = prg;
                attrgrp->end.index = results.position() + stidx + results.length();
                attrgrp->wordindex = -1;
                SetAttribute(prg, attrgrp);

                it += results.position()+1;
                stidx += results.position()+1;
            }
        }
    }
    //単語検索
    else{
        std::wstring insenstext, insenswd;
        attrfind->type = ATTR_WORD;
        attrfind->words.resize(1);
        attrfind->words[0] = word;
        attrfind->casesensitive = casesens;
        if(!casesens){
            insenswd.resize(word.length());
            std::transform(word.begin(),word.end(),insenswd.begin(),::toupper);
        }
        for(prg=rootprg->next; prg!=terminalprg; prg=prg->next){
            int stidx = 0, foundidx;
            while(true){
                if(!casesens){
                    if(stidx==0){
                        insenstext.resize(prg->text.length());
                        std::transform(prg->text.begin(),prg->text.end(),
                                       insenstext.begin(),::toupper);
                    }
                    foundidx = insenstext.find(insenswd, stidx);
                }
                else
                    foundidx = prg->text.find(word, stidx);
                if(foundidx == std::string::npos)
                    break;
                AttributeGroup* attrgrp = new AttributeGroup();
                attrgrp->id = GenerateUniqueID();
                attrgrp->attr = attrfind;
                attrgrp->start.paragraph = prg;
                attrgrp->start.index = foundidx;
                attrgrp->end.paragraph = prg;
                attrgrp->end.index = foundidx + word.length();
                attrgrp->wordindex = 0;
                SetAttribute(prg, attrgrp);

                stidx = foundidx + 1;
            }
        }
    }

    Repaint();
}


//検索
bool EditorEngine::Find(const std::wstring& word, bool fromtop, bool reverse,
          bool casesens, bool regexp)
{
    Position stpos;
    Paragraph *prg;
    int foundidx = std::string::npos;
    int wordlen;

    //不正な正規表現を除外
    if(regexp && !IsValidRegexp(word)){
        return false;
    }

    //前方検索
    if(!reverse){
        //検索開始位置
        if(fromtop){
            stpos.paragraph = rootprg->next;
            stpos.index = 0;
        }
        else{
            //選択されていれば選択終了位置を検索開始位置とする
            //範囲検索は現在サポートしない(検索結果を範囲選択とするため)
            if(IsSelected()){
                Cursor cursor;
                GetSelectionArea(NULL, &cursor);
                stpos = (Position)cursor;
            }
            else
                stpos = (Position)crt;
        }

        //単語検索
        if(!regexp){
            std::wstring tfword;
            wordlen = word.length();
            if(!casesens){
                tfword.resize(word.length());
                std::transform(word.begin(), word.end(), tfword.begin(), ::toupper);
            }
            else
                tfword = word;
            //段落ごと検索
            for(prg=stpos.paragraph; prg!=terminalprg; prg=prg->next){
                if(prg==stpos.paragraph){
                    foundidx = wstrfind(tfword, prg->text, stpos.index, -1, casesens);
                }
                else{
                    foundidx = wstrfind(tfword, prg->text, 0, -1, casesens);
                }
                if(foundidx >= 0)
                    break;
            }
        }
        //正規表現
        else{
            std::wregex pattern(word);
            std::match_results<std::wstring::iterator> results;
            for(prg=stpos.paragraph; prg!=terminalprg; prg=prg->next){
                std::wstring::iterator it = prg->text.begin();
                foundidx = std::string::npos;
                if(prg==stpos.paragraph)
                    it += stpos.index;
                if(std::regex_search(it, prg->text.end(), results, pattern)){
                    foundidx = results.position();
                    if(prg==stpos.paragraph)
                        foundidx += stpos.index;
                    wordlen = results.length();
                    //選択範囲と検索結果が一致のときは除外して再建策
                    if(IsSelected() && prg==stpos.paragraph && results.position()==0){
                        it++;
                        foundidx = std::string::npos;
                        if(std::regex_search(it, prg->text.end(), results, pattern)){
                            foundidx = results.position() + stpos.index + 1;
                            wordlen = results.length();
                        }
                    }
                }
                if(foundidx != std::string::npos)
                    break;
            }
        }
    }
    //後方検索
    else{
        int candidx, findst;
        if(fromtop){
            stpos.paragraph = terminalprg->prev;
            stpos.index = stpos.paragraph->text.length();
        }
        else{
            if(IsSelected()){
                Cursor cursor;
                GetSelectionArea(&cursor, NULL);
                stpos = (Position)cursor;
            }
            else
                stpos = (Position)crt;
        }

        if(!regexp){
            std::wstring tfword;
            wordlen = word.length();
            if(!casesens){
                tfword.resize(word.length());
                std::transform(word.begin(), word.end(), tfword.begin(), ::toupper);
            }
            else
                tfword = word;
            for(prg=stpos.paragraph; prg!=rootprg; prg=prg->prev){
                foundidx = std::string::npos;
                candidx = 0;
                findst = 0;
                do{
                    if(prg==stpos.paragraph)
                        candidx = wstrfind(tfword, prg->text, findst, stpos.index, casesens);
                    else
                        candidx = wstrfind(tfword, prg->text, findst, -1, casesens);
                    if(candidx != std::string::npos){
                        foundidx = candidx;
                        findst = candidx+1;
                    }
                }while(candidx != std::string::npos);
                if(foundidx != std::string::npos)
                    break;
            }
        }
        else{
            std::wregex pattern(word);
            std::match_results<std::wstring::iterator> results;
            for(prg=stpos.paragraph; prg!=rootprg; prg=prg->prev){
                std::wstring::iterator it = prg->text.begin();
                foundidx = std::string::npos;
                candidx = 0;
                findst = 0;
                do{
                    candidx = std::string::npos;
                    if(prg==stpos.paragraph){
                        if(std::regex_search(
                                    prg->text.begin()+findst, prg->text.begin()+stpos.index,
                                    results, pattern)){
                            candidx = results.position() + findst;
                            wordlen = results.length();
                        }
                    }
                    else{
                        if(std::regex_search(prg->text.begin()+findst, prg->text.end(),
                                               results, pattern)){
                            candidx = results.position() + findst;
                            wordlen = results.length();
                        }
                    }
                    if(candidx != std::string::npos){
                        foundidx = candidx;
                        findst = candidx+1;
                    }
                }while(candidx != std::string::npos);
                if(foundidx != std::string::npos)
                    break;
            }
        }

    }

    //検索成功なら一致した部分を選択する
    if(foundidx>=0){
        AllowDrawing(false);

        //段落を合わせる
        if(!reverse){
            if(fromtop){
                crt.paragraph = rootprg->next;
                crt.hy = 0;
                crt.prgidx = 0;
            }
            while(crt.paragraph != prg){
                crt.hy += crt.paragraph->nlines;
                crt.paragraph = crt.paragraph->next;
                crt.prgidx++;
            }
        }
        else{
            if(fromtop){
                while(crt.paragraph->next != terminalprg){
                    crt.hy += crt.paragraph->nlines;
                    crt.paragraph = crt.paragraph->next;
                    crt.prgidx++;
                }
            }
            while(crt.paragraph != prg){
                crt.paragraph = crt.paragraph->prev;
                crt.prgidx--;
                crt.hy -= crt.paragraph->nlines;
            }
        }
        //インデックスを合わせる
        crt.index = 0;
        crt.x = 0;
        crt.ry = 0;
        while(crt.index != foundidx){
            crt.x += CharWidth(crt);
            crt.index++;
            if(crt.x + CharWidth(crt) >= wndx){
                crt.ry++;
                crt.x = 0;
            }
        }
        //選択する
        DropAnchor();
        shiftkey=true;
        for(int i=0; i<wordlen; i++)
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




//選択範囲を置換して次を検索
//ただし選択範囲が検索と一致しない場合は置換しない
bool EditorEngine::Replace(const std::wstring& findword, const std::wstring& repword,
             bool fromtop, bool casesens, bool regexp)
{
    if(regexp && !IsValidRegexp(findword)){
        return false;
    }

    if(!IsSelected()){
        Find(findword, fromtop, false, casesens, regexp);
        return false;
    }

    //選択範囲がマッチしているか
    std::wstring seltext = GetSelectedText();
    int pos = std::string::npos;
    if(!regexp){
        if(seltext.length() != findword.length())
            pos = std::string::npos;
        else
            pos = seltext.find(findword);
    }
    else{
        std::wregex pattern(findword);
        std::match_results<std::wstring::const_iterator> results;
        if(std::regex_match(seltext, results, pattern)){
            pos = results.position();
        }
    }
    //選択範囲がマッチしていたら置換して次へ
    if(pos!=std::string::npos){
        DeleteArea(true);
        Insert(repword,true);
        Find(findword, fromtop, false, casesens, regexp);
        return true;
    }
    //選択範囲がマッチしないときは検索のみ
    else{
        Find(findword, fromtop, false, casesens, regexp);
        return false;
    }
}


int EditorEngine::ReplaceAll(const std::wstring &findword, const std::wstring &repword,
                             bool alltext, bool casesens, bool regexp)
{
    Position stpos, edpos, foundpos, foundendpos;
    Paragraph *prg;
    int prgidx;
    int stidx, foundidx;
    int foundlen;
    std::wregex pattern;
    std::match_results<std::wstring::iterator> results;
    std::wstring tffindword;
    std::wstring recstr;
    int prescroll = scroll;
    int count=0;

    if(regexp && !IsValidRegexp(findword)){
        return false;
    }

    AllowDrawing(false);

    //開始地点
    if(alltext){
        ResetCursorPos();
        stpos.paragraph = rootprg->next;
        stpos.prgidx = 0;
        stpos.index = 0;
        edpos.paragraph = terminalprg->prev;
        edpos.index = edpos.paragraph->text.length();
    }
    else{
        Cursor stcur, edcur;
        GetSelectionArea(&stcur, &edcur);
        stpos = (Position)stcur;
        edpos = (Position)edcur;
        crt = stcur;
        DropAnchor();
    }

    //下準備
    //正規表現ならパターン作成、大文字小文字区別なしなら大文字に変換
    if(regexp){
        pattern.assign(findword);
    }
    else{
        foundlen = findword.length();
        if(!casesens){
            tffindword.resize(findword.length());
            std::transform(findword.begin(),findword.end(),tffindword.begin(), ::toupper);
        }
        else
            tffindword = findword;
    }

    //段落ごとに走査
    //ループ終了条件は改行置換でterminalprg->prevが消えることも想定
    for(prg=stpos.paragraph,prgidx=stpos.prgidx;
            prg!=edpos.paragraph->next && prg!=terminalprg; prg=prg->next,prgidx++){
        stidx = 0;
        foundidx = std::string::npos;
        if(prg==stpos.paragraph){
            stidx = stpos.index;
        }
        do{
            //検索
            if(!regexp){
                foundidx = wstrfind(tffindword, prg->text, stidx, -1, casesens);
            }
            else{
                std::wstring::iterator it = prg->text.begin();
                it += stidx;
                if(std::regex_search(it, prg->text.end(), results, pattern)){
                    foundidx = results.position() + stidx;
                    foundlen = results.length();
                }
                else{
                    foundidx = std::string::npos;
                    foundlen = 0;
                }
            }

            if(prg == edpos.paragraph && (foundidx+foundlen) > edpos.index)
                foundidx = std::string::npos;
            if(foundlen==0)
                foundidx = std::string::npos;

            //置換
            if(foundidx>=0){
                count++;
                foundpos = Position(prg, prgidx, foundidx);
                foundendpos = Position(prg, prgidx, foundidx+foundlen);
                if(foundendpos.index >= prg->text.length()){
                    foundendpos.paragraph = foundendpos.paragraph->next;
                    foundendpos.prgidx++;
                    foundendpos.index = foundendpos.index - prg->text.length();
                }

                //削除処理
                //範囲終了段落なら終了位置を移動する
                if(prg == edpos.paragraph){
                    edpos.index -= foundlen;
                }
                //改行が置換され、かつ次の段落が範囲終了段落の場合は、終了段落を移動する
                if(foundendpos.index == foundpos.paragraph->text.length() &&
                        foundendpos.paragraph->next == edpos.paragraph){
                    edpos.paragraph = foundendpos.paragraph->prev;
                    edpos.prgidx--;
                    edpos.index = edpos.paragraph->text.size() - foundlen + edpos.index;
                }
                recstr = DeletePrimitive(foundpos, foundendpos);
                RecordUndo(UU_REPLACE, foundpos, recstr);

                //挿入処理
                RecordUndo(UU_REPLACE, foundpos, repword);
                if(foundpos.paragraph == edpos.paragraph){
                    int oldendprglen = edpos.paragraph->text.length();
                    foundpos = InsertPrimitive(foundpos, repword);
                    edpos.index += edpos.paragraph->text.length() - oldendprglen;
                }
                else{
                    foundpos = InsertPrimitive(foundpos, repword);
                }
                stidx = foundpos.index;
                prgidx = foundpos.prgidx;
                prg = foundpos.paragraph;
            }

        }while(foundidx != std::string::npos);

    }
    recstr.clear();
    RecordUndo(UU_REPLACEEND, foundpos, recstr);

    //表示原点再取得(原点が削除されているおそれがあるため)
    scroll = 0;
    org = Cursor();
    org.paragraph = rootprg->next;
    while(org.hy + org.paragraph->nlines <= prescroll
          && org.paragraph->next != terminalprg){
        org.hy += org.paragraph->nlines;
        org.paragraph = org.paragraph->next;
        org.prgidx++;
        scroll+=org.paragraph->nlines;
    }
    if(org.paragraph->next != terminalprg){
        while(org.hy + org.ry < prescroll){
            org.x += CharWidth(org);
            if(org.x+CharWidth(org) >= wndx){
                org.x = 0;
                org.ry++;
                scroll++;
            }
            org.index++;
        }
    }
    //カーソルは原点と同じにする
    crt = org;

    //再描画
    edpos = Position(prg,prgidx,0);
    RebuildAttribute(&stpos, &edpos, NULL);
    AllowDrawing(true);
    DropAnchor();
    ShowCursor();
    Repaint();
    return count;
}



////////////////////////////////////////////////////////////////////////////////
//
//				表示情報取得
//

//表示原点の取得
Character* EditorEngine::BeginScreenScan()
{
    scanpos=org;
    for(int i=0; i<wndy; i++)
        linenos[i] = -1;
    plineno = linenos;
    prgno = org.prgidx;
    if(org.index==0)
        *plineno = prgno;
    else
        *plineno = -1;
    return ProceedScan();
}


//ノードを進める（コメントや強調単語などもここで判別）
Character* EditorEngine::ProceedScan()
{
    wchar_t ch;
    int w;
    EditorAttribute *attr;
    Cursor selst, seled;

    //文字取得
    ch = scanpos.paragraph->text[scanpos.index];
    scanret.ch = ch;

    //最上位の属性を採用する
    attr=NULL;
    for(int i=0; i<scanpos.paragraph->attrgrps.size(); i++){
        AttributeGroup* attrgrp = scanpos.paragraph->attrgrps[i];
        //属性の範囲内か
        if(scanpos.paragraph == attrgrp->start.paragraph &&
                scanpos.index < attrgrp->start.index)
            continue;
        if(scanpos.paragraph == attrgrp->end.paragraph &&
                scanpos.index >= attrgrp->end.index+attrgrp->attr->edsym.length())
            continue;

        //優先度上位の属性なら更新
        if(attr==NULL || (attr && attrgrp->attr->layerlevel > attr->layerlevel))
            attr = attrgrp->attr;
    }
    //無属性のときは通常テキスト
    if(attr==NULL){
        attr = attrtext;
    }
    scanret.r=attr->r;
    scanret.g=attr->g;
    scanret.b=attr->b;
    scanret.br=attr->br;
    scanret.bg=attr->bg;
    scanret.bb=attr->bb;
    scanret.bold = attr->bold;
    scanret.italic = attr->italic;
    scanret.underline = attr->underline;


    //現在地か
    if(scanpos.paragraph==crt.paragraph && scanpos.index==crt.index){
        attr = FindAttribute(ATTR_CURSOR);
        if(attr->id!=ATTR_NULLID){
            scanret.r = attr->r;
            scanret.g = attr->g;
            scanret.b = attr->b;
            if(attr->br!=TRANSPARENT_COLOR){
                scanret.br = attr->br;
                scanret.bg = attr->bg;
                scanret.bb = attr->bb;
            }
        }
    }

    //選択範囲であれば背景色を上書き、文字色は透過でなければ上書き
    GetSelectionArea(&selst, &seled);
    int absscany = scanpos.hy + scanpos.ry;
    int absselsty = selst.hy + selst.ry;
    int absseledy = seled.hy + seled.ry;
    if( (absscany>absselsty || (absscany==absselsty && scanpos.x>=selst.x)) &&
        (absscany<absseledy || (absscany==absseledy && scanpos.x<seled.x)) ){
        attr = FindAttribute(ATTR_SELECTION);
        scanret.br = attr->br;
        scanret.bg = attr->bg;
        scanret.bb = attr->bb;
        if(attrsel->r!=TRANSPARENT_COLOR){
            scanret.r = attr->r;
            scanret.g = attr->g;
            scanret.b = attr->b;
        }
    }

    //進める前に現在地を渡す
    scanret.x = scanpos.x;
    scanret.line = scanpos.ry + scanpos.hy - scroll;



    //進める
    w = CharWidth(scanpos);
    scanret.w = w;
    scanpos.x += w;
    scanpos.index++;
    if(scanpos.x+CharWidth(scanpos) >= wndx){
        scanpos.x = 0;
        scanpos.ry++;
    }
    if(scanpos.index == scanpos.paragraph->text.length()){
        scanpos.hy += scanpos.paragraph->nlines;
        scanpos.paragraph = scanpos.paragraph->next;
        scanpos.prgidx++;
        scanpos.index = 0;
        scanpos.x = 0;
        scanpos.ry = 0;
    }


    //行番号
    if(scanpos.x == 0){
        plineno++;
        if(scanpos.ry == 0){
            prgno++;
            *plineno = prgno;
        }
        else
            *plineno = -1;
    }

    return &scanret;
}

void EditorEngine::ResetOutputStream()
{
    streampos.paragraph = rootprg->next;
    streampos.index = 0;
}

int EditorEngine::ReadStream(wchar_t *buf, int bufsize)
{
    int readlen=0;
    int prglen = streampos.paragraph->text.length();
    if(streampos.paragraph == terminalprg)
        return -1;
    if(streampos.paragraph->next == terminalprg)    //ETXを除外
        prglen--;

    const wchar_t* ptext = streampos.paragraph->text.c_str();
    if(prglen - streampos.index >= bufsize){
        readlen = bufsize;
        memcpy(buf, &ptext[streampos.index], sizeof(wchar_t)*readlen);
        streampos.index += bufsize;
    }
    else{
        readlen = prglen - streampos.index;
        memcpy(buf, &ptext[streampos.index], sizeof(wchar_t)*readlen);
        streampos.paragraph = streampos.paragraph->next;
        streampos.index = 0;
    }
    return readlen;
}


int EditorEngine::GetCharacterCount()
{
    Paragraph *prg;
    int count=0;
    for(prg=rootprg->next; prg!=terminalprg; prg=prg->next){
        count += prg->text.length();
    }
    return count-1; //EOFを除く
}




void EditorEngine::GetCursor(int* x, int* y)
{
    *x = crt.x;
    *y = crt.hy + crt.ry -scroll;
}




////////////////////////////////////////////////////////////////////////////////
//
//				Undo
//

//削除、または挿入の記録
void EditorEngine::RecordUndo(int inputtype, const Position& pos, const std::wstring& str)
{
    UndoRecordList::iterator lastrecit;

    //前回の記録とタイプが違う場合、現在地が変更された場合、段落が変わった場合に、
    //Undo単位を確定させ新しくUndo単位を作る
    if( (prerec_type!=inputtype && inputtype!=UU_REPLACEEND)
            || (inputtype==UU_INS &&
                (pos.paragraph != prerec_pos.paragraph || pos.index != prerec_pos.index+1))
            || (inputtype==UU_DEL &&
                (pos.paragraph != prerec_pos.paragraph || pos.index != prerec_pos.index))
            || (inputtype==UU_BS  &&
                (pos.paragraph != prerec_pos.paragraph || pos.index != prerec_pos.index-1))
            ){
        //単位確定、次のUndo単位へ進める
        crtuu++;
        enduu=crtuu;
        if(crtuu==maxundo){ crtuu=enduu=0; }
        if(crtuu==startuu) startuu++;
        if(startuu==maxundo) startuu=0;

        //Undo単位はサイクリックな使い回しなので最も古いものを消して初期化する
        undounit[crtuu].records.clear();
        if(inputtype!=UU_REPLACE){
            UndoRecord newrec;
            newrec.type = inputtype;
            undounit[crtuu].records.push_back(newrec);

            //BS以外の時は最初の位置が元に戻す位置になるのでここで位置確定
            //BSの時は入力の都度更新
            if(inputtype==UU_DEL || inputtype==UU_INS){
                lastrecit = undounit[crtuu].records.end();
                lastrecit--;
                lastrecit->prgindex = pos.prgidx;
                lastrecit->textindex = pos.index;
            }
        }
    }

    //インプットの種類によって変更情報を保存
    switch(inputtype)
    {
    case UU_BS:
        lastrecit = undounit[crtuu].records.end();
        lastrecit--;
        //BSの時は常に元に戻す位置を更新
        lastrecit->prgindex = pos.prgidx;
        lastrecit->textindex = pos.index;
        //連結（一番最初に挿入）
        lastrecit->str.insert(0, str);
        break;
    case UU_DEL:
    case UU_INS:
        lastrecit = undounit[crtuu].records.end();
        lastrecit--;
        //連結（一番最後に追加）
        lastrecit->str.append(str);
        break;
    case UU_REPLACE:
        undounit[crtuu].records.push_back(UndoRecord());
        lastrecit = undounit[crtuu].records.end();
        lastrecit--;
        if((undounit[crtuu].records.size()-1)%2==0)
            lastrecit->type=UU_DEL;
        else
            lastrecit->type=UU_INS;
        lastrecit->prgindex = pos.prgidx;
        lastrecit->textindex = pos.index;
        lastrecit->str = str;
        break;
    case UU_NULL:
    default:
        break;
    }

    //現在の状態を保存して次回単位確定の判定に使う
    prerec_pos=(Position)crt;
    prerec_type=inputtype;
}


void EditorEngine::Replay(const EditorEngine::UndoUnit &uu, bool undo)
{
    int optype;
    UndoRecordList::const_iterator it, itlast;
    Position workpos;
    Position rebstpos, rebedpos;
    Paragraph *rebedprg;

    //std::listにlastがある環境ではそれを使う、ない場合の代用
    itlast = uu.records.end();
    itlast--;

    //一括置換時、UNDOの時はリストの最後から、REDOの時は最初から辿る
    if(undo)
        it=itlast;
    else
        it=uu.records.begin();


    //一斉置換以外では表示原点を消さないためにカーソルを表示しておく
    //一斉置換のときは最後にルート段落から表示原点を再取得する
    if(uu.records.size()==1)
        ShowCursor();

    //再操作
    workpos = crt;
    while(true){
        //操作を行う位置まで進める
        if(it->prgindex > workpos.prgidx){
            while(workpos.prgidx != it->prgindex){
                workpos.paragraph = workpos.paragraph->next;
                workpos.prgidx++;
            }
        }
        else if(it->prgindex < workpos.prgidx){
            while(workpos.prgidx != it->prgindex){
                workpos.paragraph = workpos.paragraph->prev;
                workpos.prgidx--;
            }
        }
        workpos.index = it->textindex;

        //最初の要素なら属性再構築のための開始点を得る
        if(it==uu.records.begin()){
            rebstpos = workpos;
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

        //削除の取り消し（挿入）
        if(optype==UU_DEL){
            workpos = InsertPrimitive(workpos, it->str);
        }
        //挿入の取り消し（削除）
        if(optype==UU_INS){
            Position deledpos = workpos;
            deledpos.index += it->str.size();
            while(deledpos.index >= deledpos.paragraph->text.size()){
                deledpos.index -= deledpos.paragraph->text.size();
                deledpos.paragraph = deledpos.paragraph->next;
                deledpos.prgidx++;
            }
            DeletePrimitive(workpos, deledpos);
        }

        //最後の要素なら属性再構築のための終了点を得る
        if(it==itlast){
            rebedprg = workpos.paragraph->next;
        }

        //終端判定とイテレータ
        if(!undo){
            it++;
            if(it==uu.records.end())
                break;
        }
        else{
            if(it==uu.records.begin())
                break;
            it--;
        }
    }

    //一斉置換のときは表示原点再取得(原点段落が削除されているおそれがあるため)
    if(uu.records.size()!=1){
        org = Cursor();
        org.paragraph = rootprg->next;
        while(org.hy + org.paragraph->nlines <= scroll){
            org.hy += org.paragraph->nlines;
            org.paragraph = org.paragraph->next;
            org.prgidx++;
        }
        while(org.hy + org.ry < scroll){
            org.x += CharWidth(org);
            if(org.x+CharWidth(org) >= wndx){
                org.x = 0;
                org.ry++;
            }
            org.index++;
        }
    }

    //カーソル合わせ
    crt = Cursor();
    //段落合わせ
    crt.paragraph = org.paragraph;
    crt.prgidx = org.prgidx;
    crt.hy = org.hy;
    if(workpos.prgidx > crt.prgidx){
        while(crt.paragraph != workpos.paragraph){
            crt.hy += crt.paragraph->nlines;
            crt.paragraph = crt.paragraph->next;
            crt.prgidx++;
        }
    }
    else if(workpos.prgidx < crt.prgidx){
        while(crt.paragraph != workpos.paragraph){
            crt.paragraph = crt.paragraph->prev;
            crt.prgidx--;
            crt.hy -= crt.paragraph->nlines;
        }
    }
    //段落内位置合わせ
    while(crt.index != workpos.index){
        crt.x += CharWidth(crt);
        crt.index++;
        if(crt.x+CharWidth(crt) >= wndx){
            crt.x = 0;
            crt.ry++;
        }
    }
    mx = crt.x;
    DropAnchor();

    //属性再構築
    rebedpos = rebstpos;
    rebedpos.index = 0;
    while(rebedpos.paragraph != rebedprg){
        rebedpos.paragraph = rebedpos.paragraph->next;
        rebedpos.prgidx++;
    }
    RebuildAttribute(&rebstpos, &rebedpos, NULL);

    //再描画
    ShowCursor();
    DropAnchor();



}



void EditorEngine::Undo()
{
    if(crtuu==startuu) return;

    //取り消し処理
    AllowDrawing(false);
    Replay(undounit[crtuu],true);
    AllowDrawing(true);
    Repaint();

    //Undo単位を一つ戻す
    crtuu--;
    if(crtuu==-1) crtuu=maxundo-1;
    prerec_type = UU_NULL;
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
    prerec_type = UU_NULL;
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


void EditorEngine::SetAttribute(Paragraph *prg, AttributeGroup *grp)
{
    prg->attrgrps.push_back(grp);
    grp->refcount++;
}

bool EditorEngine::RemoveAttribute(Paragraph *prg, int grpid)
{
    AttributeGroup *grp;
    if(!prg)
        return false;
    for(int i=0; i<prg->attrgrps.size(); i++){
        grp = prg->attrgrps[i];
        if(grp->id == grpid){
            prg->attrgrps.erase(prg->attrgrps.begin()+i);
            grp->refcount--;
            if(grp->refcount==0)
                delete grp;
            return true;
        }
    }
    return false;
}

bool EditorEngine::HasAttribute(EditorEngine::Paragraph *prg, int grpid)
{
    for(int i=0; i<prg->attrgrps.size(); i++){
        if(prg->attrgrps[i]->id == grpid){
            return true;
        }
    }
    return false;
}



void EditorEngine::RebuildAttribute(Position *stpos, Position *edpos, bool *needrepaint)
{
    Paragraph *prg;
    int prgidx;

    if(needrepaint)
        *needrepaint = false;

    //複数行属性再構築
    Position tmpstpos;
    EditorAttribute *workattr=NULL;
    AttributeGroup *bkcontgrp=NULL, *fwcontgrp=NULL;
    //再構築開始段落の一つ後方から属性が継続しているか
    prg = stpos->paragraph->prev;
    for(int i=0; i<prg->attrgrps.size(); i++){
        AttributeGroup* grp = prg->attrgrps[i];
        if(grp->attr->type == ATTR_MULTILINE && grp->end.paragraph != prg){
            bkcontgrp = grp;
            tmpstpos = grp->start;
            workattr = grp->attr;
            break;
        }
    }
    //段落ごと再構築
    for(prg=stpos->paragraph, prgidx=stpos->prgidx; prg!=terminalprg; prg=prg->next, prgidx++){
        int idx=0;
        int i;

        //文末まで調べる必要があるか
        if(prg==edpos->paragraph->next){
            AttributeGroup *exbkcontgrp=NULL;
            for(i=0; i<prg->attrgrps.size()&&!exbkcontgrp; i++){
                AttributeGroup* grp = prg->attrgrps[i];
                if(grp->attr->type == ATTR_MULTILINE){
                    //後方継続属性グループのポインタを取得
                    if(grp->start.paragraph!=prg){
                        exbkcontgrp = grp;
                        break;
                    }
                }
            }
            if(exbkcontgrp == bkcontgrp)
                break;
            else if(needrepaint)
                *needrepaint=true;
        }

        //この段落の現在ある属性についての前処理
        for(i=0; i<prg->attrgrps.size();){
            AttributeGroup* grp = prg->attrgrps[i];
            if(grp->attr->type == ATTR_MULTILINE){
                //前方へ継続している属性グループのポインタを保持
                if(grp->end.paragraph!=prg){
                    fwcontgrp = grp;
                    i++;
                }
                //前方継続属性以外は(いったん)この段落をグループから削除する
                else{
                    //id検索ループの手間を省くためここは関数から修正の必要あり
                    RemoveAttribute(prg, grp->id);
                }
            }
            else
                i++;
        }

        //再構築する
        while(idx<prg->text.length()){
            //属性範囲内でなければ開始シンボルを探す
            if(!workattr){
                while(workattr==NULL && idx<prg->text.length()){
                    for(int i=0; i<attribset.size(); i++){
                        EditorAttribute* attr = &attribset[i];
                        if(attr->type!=ATTR_MULTILINE)
                            continue;
                        //開始シンボル発見
                        if(prg->text.compare(idx, attr->stsym.length(), attr->stsym)==0){
                            workattr = attr;
                            tmpstpos = Position(prg, prgidx, idx);
                            idx += attr->stsym.length()-1;    //-1してるのはforループで+1されるため
                            break;
                        }
                    }
                    idx++;
                }
                if(idx==prg->text.length()){
                    //テキスト端まで行ったということは、前方継続属性なしで段落終了
                    if(fwcontgrp){
                        RemoveAttribute(prg, fwcontgrp->id);
                        fwcontgrp = NULL;
                    }
                    break;
                }
            }
            //属性範囲内ならば終了シンボルを探す
            else{
                idx = prg->text.find(workattr->edsym, idx);

                //終了シンボル発見
                if(idx != std::wstring::npos){
                    AttributeGroup* workgrp;

                    //後方継続属性でなければ新規グループ作成
                    if(tmpstpos.paragraph==prg){
                        workgrp = new AttributeGroup;
                        workgrp->id = GenerateUniqueID();
                        workgrp->attr = workattr;
                        workgrp->start = tmpstpos;
                        workgrp->wordindex = -1;
                    }
                    //後方継続属性ならそのグループを使う
                    else{
                        workgrp = bkcontgrp;
                    }

                    //終了位置
                    workgrp->end = Position(prg, prgidx, idx);
                    idx += workattr->edsym.length();

                    //この段落をグループに追加
                    if(!(fwcontgrp && fwcontgrp==workgrp)){
                        prg->attrgrps.push_back(workgrp);
                        workgrp->refcount++;
                    }
                    else{
                        fwcontgrp = NULL;
                        //workgrpとアドレスは同一なのでRemoveAttributeはしない
                    }

                    workattr = NULL;
                    tmpstpos = Position(NULL, 0, 0);
                }
                //終了シンボルがこの段落にない場合は前方継続属性になる
                else{
                    //既存の前方継続属性がある場合
                    if(fwcontgrp){
                        //再構築前と異なる属性が前方に継続する場合はグループから外す
                        if(fwcontgrp->attr != workattr){
                            RemoveAttribute(prg, fwcontgrp->id);
                            fwcontgrp = NULL;
                        }
                        //既存の前方継続属性と同じ属性の場合は開始位置を更新
                        else if(tmpstpos.paragraph==prg){
                            fwcontgrp->start = tmpstpos;
                        }
                    }

                    //前方継続属性が再構築前からないか、異なる属性が継続になった場合
                    if(!fwcontgrp){
                        AttributeGroup* workgrp;
                        if(tmpstpos.paragraph == prg){
                            //この段落が開始点なら新規グループ作成
                            workgrp = new AttributeGroup;
                            workgrp->id = GenerateUniqueID();
                            workgrp->attr = workattr;
                            workgrp->start = tmpstpos;
                            workgrp->wordindex = -1;
                        }
                        else{
                            //後方から継続する属性がこの段落を通り前方まで継続する場合
                            workgrp = bkcontgrp;
                        }
                        //グループに追加
                        prg->attrgrps.push_back(workgrp);
                        workgrp->refcount++;
                        fwcontgrp = workgrp;
                    }

                    idx = prg->text.length();
                    break;
                }
            }
        }
        //次の段落へ
        if(fwcontgrp)
            workattr = fwcontgrp->attr;
        else
            workattr = NULL;
        bkcontgrp = fwcontgrp;
        fwcontgrp = NULL;
    }




    //単一行属性
    //再構築範囲内の属性を削除する
    //移動を追うより再構築した方が簡単だからである
    for(prg=stpos->paragraph; prg!=edpos->paragraph->next; prg=prg->next){
        int attridx = 0;
        while(prg->attrgrps.size()!=attridx){
            if(prg->attrgrps[attridx]->attr->type==ATTR_MULTILINE)
                attridx++;
            else
                RemoveAttribute(prg, prg->attrgrps[attridx]->id);
        }
    }
    //単一行属性再構築
    for(int i=0; i<attribset.size(); i++){
        EditorAttribute* attr = &attribset[i];
        if(attr->type==ATTR_MULTILINE)
            continue;

        if(attr->type==ATTR_WORD){
            std::wstring insenstext, insenswd;
            int idx=0, len=0;
            for(int j=0; j<attr->words.size(); j++){
                len = attr->words[j].length();
                if(!attr->casesensitive){
                    insenswd.resize(len);
                    std::transform(attr->words[j].begin(),attr->words[j].end(),
                                   insenswd.begin(),::toupper);
                }
                for(prg=stpos->paragraph,prgidx=stpos->prgidx;
                        prg!=edpos->paragraph->next; prg=prg->next,prgidx++){
                    idx = 0;
                    while(true){
                        if(!attr->casesensitive){
                            if(idx==0){
                                insenstext.resize(prg->text.length());
                                std::transform(prg->text.begin(),prg->text.end(),
                                               insenstext.begin(),::toupper);
                            }
                            idx = insenstext.find(insenswd, idx);
                        }
                        else
                            idx = prg->text.find(attr->words[j], idx);
                        if(idx == std::wstring::npos)
                            break;
                        if(IsWholeWord(Position(prg,prgidx,idx),len)){
                            AttributeGroup* grp = new AttributeGroup();
                            grp->id = GenerateUniqueID();
                            grp->attr = attr;
                            grp->start.paragraph = prg;
                            grp->start.index = idx;
                            grp->end.paragraph = prg;
                            grp->end.index = idx+len;
                            grp->wordindex = j;
                            SetAttribute(prg,grp);
                        }
                        idx++;
                    }
                }
            }
        }
        else if(attr->type==ATTR_REGEXP){
            std::wregex pattern;
            std::match_results<std::wstring::iterator> results;
            int itpos;
            pattern.assign(attr->regexp);
            for(prg=stpos->paragraph; prg!=edpos->paragraph->next; prg=prg->next){
                std::wstring::iterator it = prg->text.begin();
                itpos = 0;
                while(std::regex_search(it, prg->text.end(), results, pattern)){
                    AttributeGroup *grp = new AttributeGroup();
                    grp->id = GenerateUniqueID();
                    grp->attr = attr;
                    grp->start.paragraph = prg;
                    grp->start.index = itpos + results.position();
                    grp->end.paragraph = prg;
                    grp->end.index = itpos + results.position() + results.length();
                    SetAttribute(prg,grp);
                    itpos += results.position() + 1;
                    it = prg->text.begin() + itpos;
                }
            }
        }
        else if(attr->type==ATTR_RANGE){
            int stidx, edidx, workidx;
            for(prg=stpos->paragraph,prgidx=stpos->prgidx;
                    prg!=edpos->paragraph->next; prg=prg->next,prgidx++){
                workidx = 0;
                stidx = 0;
                edidx = 0;
                while(true){
                    stidx = prg->text.find(attr->stsym, workidx);
                    if(stidx == std::wstring::npos)
                        break;
                    workidx = stidx+attr->stsym.length();
                    edidx = prg->text.find(attr->edsym, workidx);
                    if(edidx != std::wstring::npos){
                        AttributeGroup* grp = new AttributeGroup();
                        grp->id = GenerateUniqueID();
                        grp->attr = attr;
                        grp->start.paragraph = prg;
                        grp->start.index = stidx;
                        grp->end.paragraph = prg;
                        grp->end.index = edidx;
                        SetAttribute(prg,grp);
                        workidx = edidx+attr->edsym.length();
                    }
                    else
                        break;
                }
            }
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


//大文字小文字の区別をする/しない付き検索
//区別しないときは検索ワードは呼び出し側で大文字にすること
int wstrfind(const std::wstring& word, const std::wstring& str, int startpos, int endpos, bool casesense)
{
    int i,j;
    wchar_t ch;
    int wordlen = word.length();
    if(endpos<0)
        endpos = str.length();
    int strend = endpos-word.length()+1;
    if(endpos - startpos < wordlen)
        return -1;
    for(i=startpos; i<strend; i++){
        for(j=0; j<wordlen; j++){
            if(!casesense)
                ch = ::toupper(str[i+j]);
            else
                ch = str[i+j];
            if(ch!=word[j])
                break;
        }
        if(j==wordlen)
            break;
    }
    if(i==strend)
        return -1;
    return i;
}

