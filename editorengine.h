#ifndef __EDITOR_ENGINE__
#define __EDITOR_ENGINE__


#include <string>
#include <vector>
#include <iterator>
#include <list>

//連結リスト終端キャラクタ
//本来通信制御のテキスト終端で使うのだが、このプログラムではリスト端に使っている
#define ETX ((wchar_t)3)

/******************************************************************************
テキストエディタのエンジン。
グラフィック関係とキーボード、マウスなど入出力関係は抽象化されており、
このクラス単体でエディタの機能が完結するようになっている。
依存はboostのregexのみ。
******************************************************************************/

enum{SCAN_ROOT, SCAN_SCREEN, SCAN_SELECTED};	//スキャンモード
enum{FINDAREA_ALL, FINDAREA_SELECTION};
enum{UU_NULL, UU_INS, UU_BS, UU_DEL, UU_REPLACE, UU_REPLACEEND };	//Undoの単位確定判定に使う

#define TRANSPARENT_COLOR -1

/*
    EditorModel::Nodeと本質的に同じだが、
    これは対外的に使用する
*/
struct Character
{
    wchar_t ch;
    int line, x;	//表示位置
    char w;	//横幅ピクセルサイズ
    int r,g,b;	//文字色
    int br,bg,bb;	//背景色
    bool bold, italic, underline;
};

enum{
    ATTR_NULL, ATTR_MULTILINE, ATTR_WORD, ATTR_REGEXP, ATTR_RANGE
};


//予約属性ID
//UIに使う属性はATTR_USERUI以上ATTR_USER未満
//エディタ内で使う属性はATTR_USER以上を指定すること
enum{
    ATTR_NULLID, ATTR_TEXT, ATTR_SELECTION, ATTR_FIND, ATTR_CURSOR,
    ATTR_USERUI=100,
    ATTR_USER=1000,
    ATTR_COMMON=10000,
};

static std::wstring g_emptywstr = L"";
static std::vector<std::wstring> g_emptywstrs;

/*
    属性
*/
struct EditorAttribute
{
public:
    int id;
    std::wstring name;
    int type;
    int r, g, b;	//文字色
    int br, bg, bb;	//背景色、透明色を含む、透明色のRGBは仕様書で
    bool bold, italic, underline;	//太字、斜体、下線
    std::wstring stsym, edsym;	//範囲強調の場合の開始と終了シンボル、文字列の場合stsymのみ使う
    std::vector<std::wstring> words;    //単語属性の場合に使う
    std::wstring regexp;
    int layerlevel; //大きいほど手前(優先度が高い)
public: //private:
    bool casesensitive;  //検索属性のみ内部で使用

    friend class EditorEngine;

public:
    EditorAttribute(int id, const std::wstring& name, int type,
        int r, int g, int b, int br, int bg, int bb,
        bool bold, bool italic, bool underline,
        const std::vector<std::wstring>& words, int layerlevel)
    {
        this->id = id;
        this->name = name;
        this->type = type;
        this->r = r;
        this->g = g;
        this->b = b;
        this->br = br;
        this->bg = bg;
        this->bb = bb;
        this->bold = bold;
        this->italic = italic;
        this->underline = underline;
        casesensitive = true;
        switch(type){
        case ATTR_WORD:
            this->words = words;
            break;
        case ATTR_MULTILINE:
        case ATTR_RANGE:
            stsym = words[0];
            edsym = words[1];
            break;
        case ATTR_REGEXP:
            regexp = words[0];
            break;
        default:
            type = ATTR_NULL;
            break;
        }
        this->layerlevel = layerlevel;
    }

    EditorAttribute()
    {
        std::vector<std::wstring> emptywstrs;
        EditorAttribute(0, L"", ATTR_NULL, 0,0,0, 255,255,255, false,false,false, emptywstrs, 0);
    }
};



/*
    属性をまとめたもの
*/
class EditorAttributeSet: public std::vector<EditorAttribute>
{
protected:
    std::string name;

public:
    EditorAttribute* FindAttribute(int id);

};





/*
    エディタエンジン。
*/
class EditorEngine
{
//protected:
public:
    struct Paragraph;
    struct Position
    {
        Paragraph *paragraph;
        int prgidx;
        int index;
        Position(){
            paragraph = NULL;
            prgidx = 0;
            index = 0;
        }
        Position(Paragraph *prg, int prgindex, int idx){
            paragraph = prg;
            prgidx = prgindex;
            index = idx;
        }
    };

    struct Cursor:public Position
    {
        int hy;     //現在の段落先頭のy位置、スクロールを考慮しない絶対位置
        int x;
        int ry;        //段落内の相対的なy位置
        Cursor():Position(){
            hy = 0;
            x = 0;
            ry = 0;
        }
    };

    struct AttributeGroup
    {
        int id;
        int refcount;   //自動でdeleteするための参照カウントshared_ptr
        Position start, end;
        int wordindex;  //単語属性のときに使う単語のインデックス
        EditorAttribute *attr;
        AttributeGroup(){
            id = 0;
            refcount = 0;
            wordindex = -1;
            attr = NULL;
        }
    };

    //段落、すなわち改行(または文末)までの連続した文字列
    struct Paragraph
    {
        std::wstring text;
        int nlines;
        std::vector<AttributeGroup*> attrgrps;
        Paragraph *prev, *next;
        Paragraph(){
            nlines = 0;
            prev = NULL;
            next = NULL;
        }
    };

    //連続した文字列の記録
    //置換以外ではUndoUnitがこれの要素を1つだけ持つ
    //置換では要素数が常に偶数で、削除、挿入、削除、挿入…と並ぶ
    struct UndoRecord
    {
        int type;
        int prgindex;       //段落インデックス
        int textindex;      //段落内の文字インデックス
        std::wstring str;
    };

    //Undoを記録する一つの単位
    typedef std::list<UndoRecord> UndoRecordList;
    struct UndoUnit
    {
        UndoRecordList records;
    };




protected:
    std::wstring debugstr;
    static char charwidths[65536];

    //段落とカーソル
    Paragraph *rootprg; //ルート段落
    Paragraph *terminalprg; //最終段落
    Cursor org;      //スクリーン表示原点
    Cursor anchor;   //選択範囲の端点
    Cursor crt;      //現在地

    //表示関連
    int wndx, wndy, tabsize;	//窓情報（ｘとタブはピクセル単位、ｙは行単位）#viewparams
    int fontheight;				//文字の大きさ（高さ）（ピクセル単位） #viewparams
    int mx;                     //カーソル記憶位置
    int nparagraphs;            //段落数
    int ntotallines;            //総行数
    int scroll;                 //スクロール位置
    bool modified;              //
    int scrollsize;				//ホイール・ページup/downで一度にスクロールする行数
    bool autoindent;			//オートインデント
    bool showlineno;			//行番号表示
    int drawablelevel;			//再描画命令を出すか、0なら描画可、マイナスなら不可
    bool shiftkey;				//KeyEvent()から各キー関数に渡す時に使う

    //属性
    static EditorAttributeSet defattrs;     //共通属性
    EditorAttributeSet attribset;           //属性
    EditorAttribute *attrtext, *attrsel, *attrfind;    //特殊属性

    //Undo関連
    UndoUnit *undounit;
    int crtuu, enduu, startuu;
    int maxundo;
    Position prerec_pos;
    int prerec_type;

    //BeginScan(),ProceedScan()で使う
    Cursor scanpos;
    Character scanret;
    int linenos[300];			//表示用行番号テーブル
    int* plineno;               //その走査用
    int prgno;

    //ファイル出力用
    Position streampos;

protected:
    //挿入削除
    void Insert(wchar_t chara);
    void Delete();
    void DeleteArea(bool recording);	//選択範囲を消す
    std::wstring DeletePrimitive(Position& stpos, Position& edpos);
    void BackSpace();

    //段落
    void ConcatParagraphsBefore(Paragraph *base, Paragraph *inshead, Paragraph *instail);
    void ConcatParagraphsAfter(Paragraph *base, Paragraph *inshead, Paragraph *instail);
    void CalcParagraphLines(Paragraph *prg);

    //ポジション、カーソル
    Cursor PosToCur(const Position& pos, const Cursor& base);
    int CharWidth(const Cursor& cur);
    bool IsWholeWord(const Position& pos, int len);

    //属性関係
    void SetAttribute(Paragraph *prg, AttributeGroup *grp);
    bool RemoveAttribute(Paragraph *prg, int grpid);
    bool HasAttribute(Paragraph *prg, int grpid);
    void RebuildAttribute(Position *stpos, Position *edpos, bool *needrepaint);



    //移動
      //キーボード系
    void Back();			//１文字戻る
    void Forward();			//１文字進む
    void LineUp();			//１行上へ
    void LineDown();		//１行下へ
      //マウス系
    void LeftButtonDownSingle(int x, int y, unsigned long state);
    void LeftButtonDownDouble(int x, int y, unsigned long state);
    void LeftButtonDownTriple(int x, int y, unsigned long state);
    void WheelUp(int x, int y, bool left_button);		//１頁上へ
    void WheelDown(int x, int y, bool left_button);		//１頁下へ
    void SetCursor(int x, int y);	//x,yともピクセル単位
      //スクロール
    bool ScrollUp();		//１行スクロールアップ（カーソルはそのまま）
    bool ScrollDown();		//１行スクロールダウン（〃）
    void ShowCursor();		//

    //範囲選択関係
    void DropAnchor();
    void SelectAll();
    void GetSelectionArea(Cursor* stpos, Cursor* edpos);
    std::wstring GetSelectedText();

    //Undo関係
    void RecordUndo(int inputtype, const Position& pos, const std::wstring& str);
    void Replay(const UndoUnit& uu, bool undo);

    //再描画
    void Repaint();
    void Repaint(int start, int end);
    void Slide(int dist);

public:
    EditorEngine();
    ~EditorEngine();

    //コールバック接続
    static void ConnectSlideCallback(void* callback, void* dat);
    static void ConnectRepaintCallback(void* callback, void* dat);
    static void ConnectModifiedCallback(void* callback, void* dat);

    //デフォルト属性
    static void SetDefaultAttributeSet(const EditorAttributeSet& attr);
    EditorAttribute* FindAttribute(int id);

    //初期化と設定
    void SetAttributeSet(const EditorAttributeSet &as);
    void SetViewParams(int tabsize,int wndxpx, int wndypx, int fontheight); //ピクセルサイズで入れること
    void ResetCursorPos();
    void Clear();
    void DeleteSelectedArea(){ DeleteArea(true); }
    void SetModified(bool mod);
    bool IsModified(){return modified;}

    void Insert(const std::wstring& str, bool recording=true);
    void InsertFast(const std::wstring& str);
    Position InsertPrimitive(const Position& pos, const std::wstring& str);

    void SetScrollSize(int size){scrollsize=size;}
    void SetAutoIndent(bool autoidt){ autoindent=autoidt;}
    void AllowDrawing(bool allow);

    bool IsSelected();
    void IsUndoable(bool *undoable, bool *redoable);

    //入力
    void KeyEvent(int keycode, wchar_t chara, unsigned long state);	//キーボード入力
    void MouseEvent(int button, int nclicks, int x, int y, unsigned long state); //マウス入力（座標はピクセル単位）
    void ScrollEvent(int value);		//スクロールバー入力

    //検索・置換
    void HighlightFindWord(const std::wstring& word, bool casesens, bool regexp);
    bool Find(const std::wstring& word, bool fromtop, bool reverse, bool casesens, bool regexp);
    bool Replace(const std::wstring& findword, const std::wstring& repword,
                 bool fromtop, bool casesens, bool regexp);
    int ReplaceAll(const std::wstring& find, const std::wstring& rep,
                   bool alltext, bool casesens, bool regexp);

    void Undo();
    void Redo();

    //文字取得
    Paragraph* GetRoot(){return rootprg->next;}
    Character* BeginScreenScan();		//表示原点を返し、描画開始準備をする
    Character* ProceedScan();
    void ResetOutputStream();
    int ReadStream(wchar_t *buf, int bufsize);

    //その他変数取得
    int GetCharacterCount();
    void GetCursor(int* x, int* y);
    int GetTotalLine(){return ntotallines;}
    int GetScrollPos(){return scroll;}
    int *GetLineNumberTable(){return linenos;}
    int GetLineCountOfScreen(){return wndy;}

    //static関数
    static unsigned long GenerateUniqueID();
    static void SetWidthsTable(char *table);
    static bool IsValidRegexp(const std::wstring pattern);


    friend class EditorView; //デバッグ時のみ
};



enum {
    KEY_NULL = 0,
    KEY_SPACE = ' ',
    KEY_TAB = '\t',
    KEY_CHAR = 'a',

    KEY_ESCAPE = 0x1000,            // MISC KEYS
    KEY_BACKTAB = 0x1002,
    KEY_BACKSPACE = 0x1003,
    KEY_RETURN = 0x1004,
    KEY_ENTER = 0x1005,
    KEY_INSERT = 0x1006,
    KEY_DELETE = 0x1007,
    KEY_PAUSE = 0x1008,
    KEY_PRINT = 0x1009,
    KEY_SYSREQ = 0x100A,
    KEY_HOME = 0x1010,              // CURSOR MOVEMENT
    KEY_END = 0x1011,
    KEY_LEFT = 0x1012,
    KEY_UP = 0x1013,
    KEY_RIGHT = 0x1014,
    KEY_DOWN = 0x1015,
    KEY_PRIOR = 0x1016, KEY_PAGEUP = KEY_PRIOR,
    KEY_NEXT = 0x1017, KEY_PAGEDOWN = KEY_NEXT,
    KEY_SHIFT = 0x1020,             // MODIFIERS
    KEY_CONTROL = 0x1021,
    KEY_META = 0x1022,
    KEY_ALT = 0x1023,
    KEY_CAPSLOCK = 0x1024,
    KEY_NUMLOCK = 0x1025,
    KEY_SCROLLLOCK = 0x1026,
    KEY_F1 = 0x1030,                // FUNCTION KEYS
    KEY_F2 = 0x1031,
    KEY_F3 = 0x1032,
    KEY_F4 = 0x1033,
    KEY_F5 = 0x1034,
    KEY_F6 = 0x1035,
    KEY_F7 = 0x1036,
    KEY_F8 = 0x1037,
    KEY_F9 = 0x1038,
    KEY_F10 = 0x1039,
    KEY_F11 = 0x103A,
    KEY_F12 = 0x103B,

    KEY_UNKNOWN = 0xFFFF
};


enum{
    MODKEY_SHIFT = 0x00200000,
    MODKEY_CONTROL = 0x00400000,
    MODKEY_ALT = 0x00800000
};

enum{
    BUTTON_LEFT = 1,
    BUTTON_MIDDLE = 2,
    BUTTON_RIGHT = 4,
    BUTTON_WHEELUP = 8,
    BUTTON_WHEELDOWN = 16,

    MOUSE_MOVE = 256
};


std::wstring join(const std::vector<std::wstring>& strs, const wchar_t& separator);
std::vector<std::wstring> split(const std::wstring& str, const wchar_t &separator);
int wstrfind(const std::wstring& word, const std::wstring& str, int startpos, int endpos, bool casesense);

#endif
