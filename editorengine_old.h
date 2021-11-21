#ifndef __EDITOR_ENGINE__
#define __EDITOR_ENGINE__


#include <string>
#include <vector>
#include <iterator>
#include <list>
#include <boost/regex.hpp>

//連結リスト終端キャラクタ
//本来通信制御のテキスト終端で使うのだが、このプログラムではリスト端に使っている
#define ETX 3

/******************************************************************************
テキストエディタのエンジン。
グラフィック関係とキーボード、マウスなど入出力関係は抽象化されており、
このクラス単体でエディタの機能が完結するようになっている。
標準C++(STL)とboostのみ使用で、環境に依存しない。
C++11は使用していない。

内部データは1本の双方向連結リストで表現される。
-2     -1          0             1      2            cnt-1  cnt         cnt+1
NULL - root[ETX] - (first)node - node - node - ... - node - last[ETX] - NULL
rootノードは表示されない。firstノードからが実際のエディタ内で表示される文字に対応する。
何も文字がない場合(初期状態)は以下のような状態になっている。
-2     -1          0(cnt)      1(cnt+1)
NULL - root[ETX] - last[ETX] - NULL
rootと終端ノードにはETXが入る。
EOF(0x1A)ではないのは何か理由があってそうしたのだが忘れてしまった。
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
	char w;	//横幅ピクセルサイズ(nodeのと違って直接参照可能)
    int r,g,b;	//文字色
    int br,bg,bb;	//背景色
	bool bold, italic, underline;
    bool selection;
    bool current;
};

enum{
    ATTR_NULL, ATTR_RANGE, ATTR_WORD, ATTR_REGEXP
};

enum{
    ATTR_RANGEPART_NULL, ATTR_RANGEPART_START, ATTR_RANGEPART_END, ATTR_RANGEPART_MIDDLE
};

//予約属性ID
//UIに使う属性はATTR_USERUI以上ATTR_USER未満
//エディタ内で使う属性はATTR_USER以上を指定すること
enum{
    ATTR_NULLID, ATTR_TEXT, ATTR_SELECTION, ATTR_FIND, ATTR_CURSOR, ATTR_FINDAREA,
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
    bool samesymbol;    //開始と終了シンボルが同一のときはtrue

public:
    //文字列
    EditorAttribute(int id, const std::wstring& name,
        int r, int g, int b,
        int br, int bg, int bb,
        bool bold, bool italic, bool underline,
        const std::vector<std::wstring>& words=g_emptywstrs)
    {
        this->id = id;
        this->name = name;
        this->r = r;
        this->g = g;
        this->b = b;
        this->br = br;
        this->bg = bg;
        this->bb = bb;
        this->bold = bold;
        this->italic = italic;
        this->underline = underline;
        this->stsym = L"";
        this->edsym = L"";
        this->words = words;
        this->samesymbol = false;
        type = ATTR_WORD;
    }

    //範囲
    EditorAttribute(int id, const std::wstring& name,
        int r, int g, int b,
        int br, int bg, int bb,
        bool bold, bool italic, bool underline,
        const std::wstring& stsym, const std::wstring& edsym)
    {
        this->id = id;
        this->name = name;
        this->r = r;
        this->g = g;
        this->b = b;
        this->br = br;
        this->bg = bg;
        this->bb = bb;
        this->bold = bold;
        this->italic = italic;
        this->underline = underline;
        this->stsym = stsym;
        this->edsym = edsym;
        samesymbol = stsym==edsym;
        type = ATTR_RANGE;
    }

    //正規表現
    EditorAttribute(int id, const std::wstring& name,
        int r, int g, int b,
        int br, int bg, int bb,
        bool bold, bool italic, bool underline,
        const std::wstring& re)
    {
        this->id = id;
        this->name = name;
        this->r = r;
        this->g = g;
        this->b = b;
        this->br = br;
        this->bg = bg;
        this->bb = bb;
        this->bold = bold;
        this->italic = italic;
        this->underline = underline;
        this->regexp = re;
        samesymbol = false;
        type = ATTR_REGEXP;
    }

    //デフォルトコンストラクタ
    EditorAttribute()
    {
        EditorAttribute(0, L"", 0,0,0, 255,255,255, false,false,false);
        type = ATTR_NULL;
    }
};


static EditorAttribute g_nullattr;

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
    struct AttributeGroup
	{
        int id;
        int refcount;   //自動でdeleteするための参照カウント
                        //C++11であればshared_ptr使った方が良いかもしれない
        int wordindex;  //単語属性のときに使う単語のインデックス
        EditorAttribute *attrib;
	};
    typedef std::list<AttributeGroup*> AttributeGroupList;

    struct BelongingGroup
    {
        int rangepart;
        AttributeGroup *group;
    };
    typedef std::list<BelongingGroup*> BelongingGroupList;

	struct Node
	{
		wchar_t ch;
        char w;	//横幅ピクセルサイズ
		Node *prev, *next;
        BelongingGroupList blattrgrps;
	};

	struct Iterator
		:public std::iterator<std::bidirectional_iterator_tag, wchar_t>
	{
		Iterator(const Node *n = NULL)
		{
			nd = const_cast<Node*>(n);
		}
		bool operator==(const Iterator& it) const
		{
			return it.GetNode() == nd ? true: false;
		}
		bool operator!=(const Iterator& it) const
		{
			return it.GetNode() == nd ? false : true;
		}
		Iterator &operator++(){
			nd = nd->next;
			return *this;
		}
        Iterator operator++(int){
			nd = nd->next;
			return *this;
		}
		Iterator &operator--(){
			nd = nd->prev;
			return *this;
		}
        Iterator operator--(int){
			nd = nd->prev;
			return *this;
		}
        Iterator &operator+=(int n){
            for(int i=0; i<n; i++)
                nd = nd->next;
            return *this;
        }
		wchar_t &operator*(){ return nd->ch; }
		Node* GetNode() const{ return nd; }

	private:
		Node *nd;
	};




    //連続した文字列の記録
    //置換以外ではUndoUnitがこれの要素を1つだけ持つ
    //置換では要素数が常に偶数で、削除、挿入、削除、挿入…と並ぶ
    struct UndoRecord
    {
        int type;
        int index;
        std::list<wchar_t> str; //前方挿入も行うので内部が配列のstd::stringは使わない
    };

    //Undoを記録する一つの単位
	struct UndoUnit
    {
        std::list<UndoRecord> records;
	};




//protected:
public:	//デバッグ用にpublicにしてある
    static EditorAttributeSet defattrs;

	//ノード
	Node* crt;		//現在のカーソル
	Node* root;		//ルートノード
    Node* last;     //終端ノード
    Node* org;		//表示原点
    EditorAttributeSet attribset;	//属性
    AttributeGroupList attrgrplist;
    AttributeGroup* atgrpfind;   //検索結果のハイライトに使う
    AttributeGroup* atgrpfindarea;
    EditorAttribute *attrtext, *attrsel; //特殊属性
	bool *orgattrs;
    bool modified;

	//ウィンドウ情報
	int wndx, wndy, tabsize;	//窓情報（ｘとタブはピクセル単位、ｙは行単位）
	int curx, cury, mx;			//カーソル位置（ｘはピクセル単位、ｙは行単位）
    int curindex;               //カーソルインデックス(先頭からの文字数)
	int totalline, scroll;		//改行と折り返しを考慮した総行数とスクロール位置
	int totalpara, scrollpara;	//改行のみをカウントしたもの（パラグラフ）
	int rettable[200];			//改行のある行かない行か
    int chcount;                //双方向リストのサイズ(リスト両端は含まない)
	
	//表示属性
    int fontheight;				//文字の大きさ（高さ）（ピクセル単位）
    int scrollsize;				//ホイール・ページup/downで一度にスクロールする行数
	bool autoindent;			//オートインデント
	bool showlineno;			//行番号表示
    bool drawable;				//再描画命令を出すか
	bool shiftkey;				//KeyEvent()から各キー関数に渡す時に使う

    //範囲選択に使う
    int ancindex;
    int ancx, ancy;
    Node *anchor;
    Node *findstart, *findend;
    int findstartindex, findendindex;

    //検索条件
    std::wstring findcond_word;
    bool findcond_inselarea, findcond_casesens, findcond_regexp;
	
	//Undo関連
    UndoUnit *undounit;
	int crtuu, enduu, startuu;
	int maxundo;
    int prerec_index;
    int prerec_type;

	//BeginScan(),ProceedScan()で使う
	int scanx, scany;
	Node dummyorgnode;
	Node *scannode, *endnode;
	Character* scanret;
	int scanmode;		//root, screen, selected

protected:
    //ノード操作
    void Insert(wchar_t chara);
	void Delete();
    void DeleteArea(bool recording);	//選択範囲を消す
	void BackSpace();

	//ノード操作補助
    Node* GetLineHead(Node *basenode, int basey, int *heady=NULL);
	Node* GetLineTail(Node *basenode, int basex, int basey, int *tailx=NULL, int *taily=NULL);

	//属性関係
    void ClearAttributes(Node* n);
	void RemoveAttribute(Node* start, Node* end, int attribgrpid);
    bool RemoveAttribute(Node* n, int grpid);
    bool RemoveAttribute(Node* n, EditorAttribute* attr);
    bool HasAttribute(Node *n, int grpid);
    int GetRangePart(Node *n, int grpid);
    int GetRangePart(Node *n, EditorAttribute* attr);
    void SetRangePart(Node *n, int grpid, int rangepart);
    void SetAttribute(Node *n, AttributeGroup* attrgrp, int rangepart);
    void RebuildAttribute(Node *stnode, bool *needrepaint);


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
    void GetSelectionPos(Node** stn, int* stx, int* sty, int* stindex,
                         Node** edn, int* edx, int* edy, int* edindex);

	//Undo関係
    void RecordUndo(int inputtype, std::list<wchar_t> &chlist);
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
    static void ConnectCharWidthCallback(void* callback, void* dat);
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

    void SetScrollSize(int size){scrollsize=size;}
	void SetAutoIndent(bool autoidt){ autoindent=autoidt;}
    void AllowDrawing(bool allow){drawable=allow;}

    bool IsSelected();
	void IsUndoable(bool *undoable, bool *redoable);

	//入力
    void KeyEvent(int keycode, wchar_t chara, unsigned long state);	//キーボード入力
    void MouseEvent(int button, int nclicks, int x, int y, unsigned long state); //マウス入力（座標はピクセル単位）
	void ScrollEvent(int value);		//スクロールバー入力

    //検索・置換
    void HighlightFindWord(const std::wstring& word,
                           bool inselarea, bool casesens, bool regexp);
    bool Find(const std::wstring& word, bool fromtop, bool to_bottom, bool inselarea,
              bool casesens, bool regexp);
    bool Replace(const wchar_t* find, const wchar_t* rep, bool skip);
    void ReleaseFindingArea();
    int ReplaceAll(const std::wstring& find, const std::wstring& rep, bool alltext);


	void Undo();
	void Redo();

	//ノード取得
	Node* GetRoot(){return root->next;}
	Character* BeginScan(int mode);		//表示原点を返し、描画開始準備をする
	Character* ProceedScan();

	//その他変数取得
    int GetCharacterCount(){return chcount;}
	void GetCursor(int* x, int* y);
	int GetTotalLine(){return totalline;}
	int GetScrollPos(){return scroll;}
	int* GetLineNumber(bool LFonly);
    int GetLineCountOfScreen(){return wndy;}

	//便利な関数
protected:
    bool nodecmp(Node* n, const std::wstring& str, bool casesens);

	static unsigned long GenerateUniqueID();
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


std::list<wchar_t> wstolist(const std::wstring& str);
std::wstring listtows(const std::list<wchar_t>& list);
std::wstring join(const std::vector<std::wstring>& strs, const wchar_t& separator);
std::vector<std::wstring> split(const std::wstring& str, const wchar_t &separator);
//std::wstring &trim(std::wstring &s);
//void trim(std::vector<std::wstring> &s);
std::wstring escape(const std::wstring &s);
std::wstring unescape(const std::wstring &s);

#endif

