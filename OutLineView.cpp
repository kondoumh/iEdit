// OutLineView.cpp : OutlineView クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "iEdit.h"

#include "iEditDoc.h"
#include "OutLineView.h"
#include "LinkInfoDlg.h"
#include "LinkInfo2Dlg.h"
#include "OnProcDlg.h"
#include "SelImportDlg.h"
#include "SelExportDlg.h"
#include "ImportTextDlg.h"

#include "nodeSrchDlg.h"
#include "setFoldUpDlg.h"
#include "InpcnDlg.h"
#include "Token.h"
#include "SetHtmlExportDlg.h"
#include "Utilities.h"
#include <shlobj.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGS_FRAME _T("Frame Options")

// 関数オブジェクトお試し 関数ポインタ版
int labelMessage(CTreeCtrl& tree, HTREEITEM item) {
	CString s = tree.GetItemText(item);
	AfxMessageBox(s);
	return 0;
};

typedef pointer_to_binary_function<CTreeCtrl&, HTREEITEM, int> OnNode;
OnNode test(labelMessage);


// 関数オブジェクトお試し binary_function 版
struct treeTest : std::binary_function<CTreeCtrl&, HTREEITEM, int> {
	int operator() (const CTreeCtrl& tree, HTREEITEM item) const {
		CString s = tree.GetItemText(item);
		AfxMessageBox(s);
		return 0;
	}
};

// vector にラベルとキーを詰め込む関数オブジェクト
struct copyLabels : std::binary_function<CTreeCtrl&, HTREEITEM, int> {
	Labels& ls_;
	copyLabels(Labels& ls) : ls_(ls) {}
	int operator() (const CTreeCtrl& tree, HTREEITEM item) const {
		label l;
		l.key = tree.GetItemData(item);
		if (tree.GetParentItem(item) != NULL) {
			l.parent = tree.GetItemData(tree.GetParentItem(item));
		} else {
			l.parent = tree.GetItemData(tree.GetRootItem());
		}
		l.name = tree.GetItemText(item);
		l.state = tree.GetItemState(item, TVIS_EXPANDED | TVIS_SELECTED);
		ls_.push_back(l);
		return 0;
	}
};

// KeySetにキーを詰め込む関数オブジェクト
struct copyKeys : std::binary_function<CTreeCtrl&, HTREEITEM, int> {
	KeySet& ks_;
	copyKeys(KeySet& ks) : ks_(ks) {}
	int operator() (const CTreeCtrl& tree, HTREEITEM item) const {
		DWORD key;
		key = tree.GetItemData(item);
		ks_.insert(key);
		return 0;
	}
};

// vector にキーを詰め込む関数オブジェクト
struct copyKeyVec : std::binary_function<CTreeCtrl&, HTREEITEM, int> {
	serialVec& svec_;
	copyKeyVec(serialVec& svec) : svec_(svec) {}
	int operator() (const CTreeCtrl& tree, HTREEITEM item) const {
		DWORD key = tree.GetItemData(item);
		svec_.push_back(key);
		return 0;
	}
};


////////////////////////////////////////////////////////////
//     tree 巡回アルゴリズム treeview_for_each            //
////////////////////////////////////////////////////////////
template<class BinaryFunction>
BinaryFunction treeview_for_each(CTreeCtrl& tree, BinaryFunction function, HTREEITEM item=0)
{
	HTREEITEM hItem;
	if (item == 0) {
		item = tree.GetRootItem();
	}
	while (item != NULL) {
		hItem = item;
		function(tree, item);
		if (tree.ItemHasChildren(hItem)) {
			treeview_for_each(tree, function, tree.GetChildItem(item));
		}
		item = tree.GetNextSiblingItem(item);
	}
	return function;
}


template<class BinaryFunction>
BinaryFunction treeview_for_each2(CTreeCtrl& tree, BinaryFunction function, HTREEITEM item=0)
{
	HTREEITEM hItem;
	if (item == 0) {
		item = tree.GetRootItem();
	}
	while (item != NULL) {
		hItem = item;
		function(tree, item);
		item = tree.GetNextSiblingItem(item);
	}
	return function;
}


////////////////////////////////////////////////////////////
//        tree 検索アルゴリズム treeview_find             //
////////////////////////////////////////////////////////////
template<class Predicate>
HTREEITEM treeview_find(CTreeCtrl& tree, Predicate pred, HTREEITEM item=0)
{
	HTREEITEM hItem;
	if (item == 0) {
		item = tree.GetRootItem();
	}
	while (item != NULL) {
		hItem = item;
		if (pred()) {
			return hItem;
		}
		if (tree.ItemHasChildren(hItem)) {
			HTREEITEM cItem = treeview_find(tree, pred, tree.GetChildItem(item));
			if (cItem != NULL) {
				return cItem;
			}
		}
		item = tree.GetNextSiblingItem(item);
	}
	return NULL;
}

// フォルダ選択ダイアログ用コールバックプロシージャ
static int _stdcall FolderDlgCallBackProc( 
	HWND hWnd, UINT uiMsg, LPARAM lParam, LPARAM lpData)
{
	if(uiMsg == BFFM_INITIALIZED){
		if( lpData != NULL){
			SendMessage( hWnd, 
					BFFM_SETSELECTION, 
					TRUE, 
					lpData );
		}
	}
	return(0);
}

/////////////////////////////////////////////////////////////////////////////
// OutlineView

IMPLEMENT_DYNCREATE(OutlineView, CTreeView)

BEGIN_MESSAGE_MAP(OutlineView, CTreeView)
	//{{AFX_MSG_MAP(OutlineView)
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_ADD_CHILD, OnAddChild)
	ON_UPDATE_COMMAND_UI(ID_ADD_CHILD, OnUpdateAddChild)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)
	ON_COMMAND(ID_ADD_SIBLING, OnAddSibling)
	ON_UPDATE_COMMAND_UI(ID_ADD_SIBLING, OnUpdateAddSibling)
	ON_COMMAND(ID_EDIT_LABEL, OnEditLabel)
	ON_UPDATE_COMMAND_UI(ID_EDIT_LABEL, OnUpdateEditLabel)
	ON_COMMAND(ID_LEBEL_UP, OnLebelUp)
	ON_UPDATE_COMMAND_UI(ID_LEBEL_UP, OnUpdateLebelUp)
	ON_COMMAND(ID_LEBEL_DOWN, OnLebelDown)
	ON_UPDATE_COMMAND_UI(ID_LEBEL_DOWN, OnUpdateLebelDown)
	ON_COMMAND(ID_ORDER_UP, OnOrderUp)
	ON_UPDATE_COMMAND_UI(ID_ORDER_UP, OnUpdateOrderUp)
	ON_COMMAND(ID_ORDER_DOWN, OnOrderDown)
	ON_UPDATE_COMMAND_UI(ID_ORDER_DOWN, OnUpdateOrderDown)
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_UPDATE_COMMAND_UI(ID_DELETE, OnUpdateDelete)
	ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_ADD_LINK, OnAddLink)
	ON_UPDATE_COMMAND_UI(ID_ADD_LINK, OnUpdateAddLink)
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_ADD_URL, OnAddUrl)
	ON_UPDATE_COMMAND_UI(ID_ADD_URL, OnUpdateAddUrl)
	ON_COMMAND(ID_SELECT_CHILD, OnSelectChild)
	ON_UPDATE_COMMAND_UI(ID_SELECT_CHILD, OnUpdateSelectChild)
	ON_COMMAND(ID_SELECT_PARENT, OnSelectParent)
	ON_UPDATE_COMMAND_UI(ID_SELECT_PARENT, OnUpdateSelectParent)
	ON_COMMAND(ID_SORT_CHILDREN, OnSortChildren)
	ON_UPDATE_COMMAND_UI(ID_SORT_CHILDREN, OnUpdateSortChildren)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, OnBegindrag)
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYUP()
	ON_COMMAND(ID_IMPORT_DATA, OnImportData)
	ON_UPDATE_COMMAND_UI(ID_IMPORT_DATA, OnUpdateImportData)
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, OnUpdateEditFind)
	ON_WM_DESTROY()
	ON_COMMAND(ID_EDIT_REPLACE, OnEditReplace)
	ON_COMMAND(ID_SET_FOLDUP, OnSetFoldup)
	ON_UPDATE_COMMAND_UI(ID_SET_FOLDUP, OnUpdateSetFoldup)
	ON_COMMAND(ID_SHOW_SELECTED_BRANCH, OnShowSelectedBranch)
	ON_UPDATE_COMMAND_UI(ID_SHOW_SELECTED_BRANCH, OnUpdateShowSelectedBranch)
	ON_COMMAND(ID_SHOW_SELECTED_CHILDREN, OnShowSelectedChildren)
	ON_UPDATE_COMMAND_UI(ID_SHOW_SELECTED_CHILDREN, OnUpdateShowSelectedChildren)
	ON_COMMAND(ID_DROP_FIRST_ORDER, OnDropFirstOrder)
	ON_UPDATE_COMMAND_UI(ID_DROP_FIRST_ORDER, OnUpdateDropFirstOrder)
	ON_COMMAND(ID_DROP_LEVEL_UP, OnDropLevelUp)
	ON_UPDATE_COMMAND_UI(ID_DROP_LEVEL_UP, OnUpdateDropLevelUp)
	ON_COMMAND(ID_COPY_TREE_TO_CLIPBOARD, OnCopyTreeToClipboard)
	ON_UPDATE_COMMAND_UI(ID_COPY_TREE_TO_CLIPBOARD, OnUpdateCopyTreeToClipboard)
	//}}AFX_MSG_MAP
	// 標準印刷コマンド
//	ON_COMMAND(ID_FILE_PRINT, CTreeView::OnFilePrint)
//	ON_COMMAND(ID_FILE_PRINT_DIRECT, CTreeView::OnFilePrint)
//	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CTreeView::OnFilePrintPreview)
	ON_MESSAGE(WM_LISTUPNODES, OnListUpNodes)
	ON_MESSAGE(WM_SRCHNODE, OnSearchNode)
	ON_MESSAGE(WM_CLOSESRCHWINDOW, OnHideSrchDlg)
	ON_COMMAND(ID_ADD_CHILD2, OnAddChild2)
	ON_UPDATE_COMMAND_UI(ID_ADD_CHILD2, OnUpdateAddChild2)
	ON_COMMAND(ID_CREATE_CLONE, &OutlineView::OnCreateClone)
	ON_UPDATE_COMMAND_UI(ID_CREATE_CLONE, &OutlineView::OnUpdateCreateClone)
	ON_COMMAND(ID_RESET_SHOW_SUBBRANCH, &OutlineView::OnResetShowSubbranch)
	ON_UPDATE_COMMAND_UI(ID_RESET_SHOW_SUBBRANCH, &OutlineView::OnUpdateResetShowSubbranch)
	ON_COMMAND(ID_TREE_IMAGE_CHCEK, &OutlineView::OnTreeImageChcek)
	ON_UPDATE_COMMAND_UI(ID_TREE_IMAGE_CHCEK, &OutlineView::OnUpdateTreeImageChcek)
	ON_UPDATE_COMMAND_UI(ID_TREE_IMAGE_BLUE, &OutlineView::OnUpdateTreeImageBlue)
	ON_COMMAND(ID_TREE_IMAGE_BLUE, &OutlineView::OnTreeImageBlue)
	ON_COMMAND(ID_TREE_IMAGE_RED, &OutlineView::OnTreeImageRed)
	ON_UPDATE_COMMAND_UI(ID_TREE_IMAGE_RED, &OutlineView::OnUpdateTreeImageRed)
	ON_COMMAND(ID_TREE_IMAGE_YEALOW, &OutlineView::OnTreeImageYealow)
	ON_UPDATE_COMMAND_UI(ID_TREE_IMAGE_YEALOW, &OutlineView::OnUpdateTreeImageYealow)
	ON_COMMAND(ID_TREE_IMAGE_CANCEL, &OutlineView::OnTreeImageCancel)
	ON_UPDATE_COMMAND_UI(ID_TREE_IMAGE_CANCEL, &OutlineView::OnUpdateTreeImageCancel)
	ON_COMMAND(ID_TREE_IMAGE_QUESTION, &OutlineView::OnTreeImageQuestion)
	ON_UPDATE_COMMAND_UI(ID_TREE_IMAGE_QUESTION, &OutlineView::OnUpdateTreeImageQuestion)
	ON_COMMAND(ID_TREE_IMAGE_WARNING, &OutlineView::OnTreeImageWarning)
	ON_UPDATE_COMMAND_UI(ID_TREE_IMAGE_WARNING, &OutlineView::OnUpdateTreeImageWarning)
	ON_COMMAND(ID_TREE_IMAGE_FACE, &OutlineView::OnTreeImageFace)
	ON_UPDATE_COMMAND_UI(ID_TREE_IMAGE_FACE, &OutlineView::OnUpdateTreeImageFace)
	ON_COMMAND(ID_TREE_IMAGE_IDEA, &OutlineView::OnTreeImageIdea)
	ON_UPDATE_COMMAND_UI(ID_TREE_IMAGE_IDEA, &OutlineView::OnUpdateTreeImageIdea)
	ON_COMMAND(ID_PASTE_TREE_FROM_CLIPBOARD, &OutlineView::OnPasteTreeFromClipboard)
	ON_UPDATE_COMMAND_UI(ID_PASTE_TREE_FROM_CLIPBOARD, &OutlineView::OnUpdatePasteTreeFromClipboard)
	ON_COMMAND(ID_EXPORT_TO_HTML, &OutlineView::OnExportToHtml)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_TO_HTML, &OutlineView::OnUpdateExportToHtml)
	ON_COMMAND(ID_EXPORT_TO_TEXT, &OutlineView::OnExportToText)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_TO_TEXT, &OutlineView::OnUpdateExportToText)
	ON_COMMAND(ID_EXPORT_TO_XML, &OutlineView::OnExportToXml)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_TO_XML, &OutlineView::OnUpdateExportToXml)
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OutlineView クラスの構築/消滅

OutlineView::OutlineView()
{
	// TODO: この場所に構築用のコードを追加してください。
	m_bHitR = false;
	m_bAdding = false;
	m_bLabelEditting = false;
	m_bAddingLink = false;
	m_hLinkCsr = AfxGetApp()->LoadCursor(IDC_LINKTREE);
	m_bItemDragging = false;
	m_hitemDragPre = NULL;
	m_bAddingChild = false;
	m_opTreeOut = 0;
	m_pSrchDlg = NULL;
	m_hItemMoved = NULL;
	m_hParentPreMove = NULL;
	m_hSiblingPreMove = NULL;
	m_bNodeSel = false;
	m_exportOption.treeOption = 0;
	m_exportOption.htmlOutOption = 0;
	m_exportOption.textOption = 0;
	m_exportOption.imgOption = 0;
	m_exportOption.navOption = 0;
	m_exportOption.htmlOutDir = "";
}

OutlineView::~OutlineView()
{
}

BOOL OutlineView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。
	cs.style |= TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS/* | TVS_DISABLEDRAGDROP*/;
	return CTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// OutlineView クラスの描画

void OutlineView::OnDraw(CDC* pDC)
{
	iEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: この場所にネイティブ データ用の描画コードを追加します。
	
}

void OutlineView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();
	
	// TODO:  GetTreeCtrl() メンバ関数の呼び出しを通して直接そのリスト コントロールに
	//  アクセスすることによって TreeView をアイテムで固定できます。
	if (GetDocument()->isOldBinary() || GetDocument()->getSerialVersion() <= 1) {
		treeConstruct();
	} else {
		treeConstruct2();
	}
	
	doColorSetting(); // 背景色や文字色の設定
	
	// SubBranch表示状態のリストア
	if (GetDocument()->isShowSubBranch()) {
		int branchMode = GetDocument()->getInitialBranchMode();
		m_hItemShowRoot = findKeyItem(GetDocument()->getBranchRootKey(), tree().GetRootItem());
		if (m_hItemShowRoot == NULL) return;
		KeySet ks;
		ks.insert(tree().GetItemData(m_hItemShowRoot));
		if (branchMode == 1) {
			treeview_for_each2(tree(), copyKeys(ks), tree().GetChildItem(m_hItemShowRoot));
			GetDocument()->setVisibleNodes(ks);
			GetDocument()->setShowBranch(tree().GetItemData(m_hItemShowRoot));
			tree().SetItemImage(m_hItemShowRoot, 1, 1);
		} else if (branchMode == 2) {
			treeview_for_each(tree(), copyKeys(ks), tree().GetChildItem(m_hItemShowRoot));
			GetDocument()->setVisibleNodes(ks);
			GetDocument()->setShowBranch(tree().GetItemData(m_hItemShowRoot));
			tree().SetItemImage(m_hItemShowRoot, 2, 2);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// OutlineView クラスの印刷

BOOL OutlineView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// デフォルトの印刷準備
	return DoPreparePrinting(pInfo);
}

void OutlineView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷前の特別な初期化処理を追加してください。
}

void OutlineView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷後の後処理を追加してください。
}

/////////////////////////////////////////////////////////////////////////////
// OutlineView クラスの診断

#ifdef _DEBUG
void OutlineView::AssertValid() const
{
	CTreeView::AssertValid();
}

void OutlineView::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}

iEditDoc* OutlineView::GetDocument() // 非デバッグ バージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(iEditDoc)));
	return (iEditDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// OutlineView クラスのメッセージ ハンドラ

int OutlineView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: この位置に固有の作成用コードを追加してください
	m_imgList.Create(IDB_TREE, 16, 11, RGB(255, 0, 255));
	tree().SetImageList(&m_imgList, TVSIL_NORMAL);
	setViewFont();
	m_pSrchDlg = new nodeSrchDlg;
	m_pSrchDlg->Create("", "", SW_HIDE, CRect(0, 0, 0, 0), this, IDD_NODESRCH);
	m_hCsrCopy = AfxGetApp()->LoadCursor(IDC_POINTER_COPY);
	m_hCsrMove = AfxGetApp()->LoadCursor(IDC_POINTER_MOVE);
	return 0;
}

void OutlineView::treeConstruct()
{
	iEditDoc* pDoc = GetDocument();
	Labels ls;
	pDoc->copyNodeLabels(ls);
	
	HTREEITEM hRoot = tree().InsertItem(ls[0].name, 0, 0);
	tree().SetItemData(hRoot, ls[0].key);
	tree().SetItemState(hRoot, ls[0].state, TVIS_EXPANDED);
	DWORD preKey = 0;
	HTREEITEM hPreParent = tree().GetRootItem();
	HTREEITEM hsel = hPreParent;
	for (unsigned int i = 1; i < ls.size(); i++) {
		if (ls[i].parent != preKey) {
			HTREEITEM hParent = findKeyItem(ls[i].parent, tree().GetRootItem());
			if (hParent != NULL) {
				hPreParent = hParent;
				preKey = ls[i].parent;
			}
		}
		
		HTREEITEM hnew = tree().InsertItem(ls[i].name, 0, 0, hPreParent);
		tree().SetItemData(hnew, ls[i].key);
		tree().SetItemState(hnew, ls[i].state, TVIS_EXPANDED);
		if (ls[i].state & TVIS_SELECTED) hsel = hnew;
	}
	tree().SelectItem(hsel);
	m_hItemShowRoot = tree().GetRootItem();
}

void OutlineView::treeConstruct2()
{
	iEditDoc* pDoc = GetDocument();
	Labels ls;
	pDoc->copyNodeLabels(ls);

	HTREEITEM hRoot = tree().InsertItem(ls[0].name, 0, 0);
	tree().SetItemData(hRoot, ls[0].key);
	tree().SetItemState(hRoot, ls[0].state, TVIS_EXPANDED);
//	tree().SetItemImage(hRoot, ls[0].treeIconId, ls[0].treeIconId);
	/* Rootのイメージをシリアル化で再現するとネットワークビューに
	   何も表示されないので、今はコメントアウト
	   TODO:原因追及
	 */
	
	DWORD preKey = 0;
	HTREEITEM hParent = tree().GetRootItem();
	HTREEITEM hSel = hParent;
	HTREEITEM hPrevNew = tree().GetRootItem();
	int prevLevel = 0;
	
	HTREEITEM hNew = hRoot;
	for (unsigned int i = 1; i < ls.size(); i++) {
		if (prevLevel > ls[i].level) {
			unsigned int diff = prevLevel - ls[i].level;
			
			HTREEITEM hIt = hPrevNew;
			HTREEITEM hItParent = tree().GetParentItem(hIt);
			for (unsigned int u = 0; u <= diff; u++) {
				hItParent = tree().GetParentItem(hIt);
				hIt = hItParent;
			}
			if (ls[i].parent == tree().GetItemData(hItParent)) {
				hNew = tree().InsertItem(ls[i].name, 0, 0, hItParent);
				hParent = hItParent;
			} else {
				AfxMessageBox("キーが一致しませんa");
			}
			preKey = ls[i].parent;
		} else if (prevLevel < ls[i].level) {
			if (ls[i].parent == preKey) {
				hNew = tree().InsertItem(ls[i].name, 0, 0, hPrevNew);
				hParent = hPrevNew;
			} else {
				AfxMessageBox("キーが一致しませんb");
			}
		} else {
			hNew = tree().InsertItem(ls[i].name, 0, 0, hParent, hPrevNew);
		}
		tree().SetItemData(hNew, ls[i].key);
		tree().SetItemState(hNew, ls[i].state, TVIS_EXPANDED);
		tree().SetItemImage(hNew, ls[i].treeIconId, ls[i].treeIconId);
		if (ls[i].state & TVIS_SELECTED) hSel = hNew;
		
		preKey = ls[i].key;
		hPrevNew = hNew;
		prevLevel = ls[i].level;
	}
	tree().SelectItem(hSel);
	m_hItemShowRoot = tree().GetRootItem();
}


void OutlineView::treeAddBranch(const DWORD rootKey)
{
	iEditDoc* pDoc = GetDocument();
	Labels ls;
	pDoc->copyNodeLabels(ls);
	
	int loop = ls.size();
	ASSERT(loop >= 0);
	COnProcDlg prcdlg;
	prcdlg.Create(IDD_ONPROC);
	prcdlg.m_ProcName.SetWindowText("登録中");
	prcdlg.m_ProgProc.SetStep(1);              // プログレスバーの初期設定
	prcdlg.m_ProgProc.SetRange(0, loop);
	
	HTREEITEM hRoot = findKeyItem(rootKey, tree().GetRootItem());
	DWORD preKey = rootKey;
	HTREEITEM hPreParent = hRoot;
	HTREEITEM hsel = hPreParent;
	
	for (unsigned int i = 0; i < ls.size(); i++) {
		if (ls[i].parent != preKey) {
			HTREEITEM hParent = findKeyItem(ls[i].parent, tree().GetRootItem());
			if (hParent != NULL) {
				hPreParent = hParent;
				preKey = ls[i].parent;
			}
		}
		HTREEITEM hnew = tree().InsertItem(ls[i].name, 0, 0, hPreParent);
		tree().SetItemData(hnew, ls[i].key);
	//	tree().SetItemState(hnew, TVIS_EXPANDED, TVIS_EXPANDED);
		prcdlg.m_ProgProc.StepIt();  // プログレスバーを更新
	}
	tree().SelectItem(hsel);
	tree().Expand(hsel, TVIS_EXPANDED);
}

void OutlineView::treeAddBranch2(const DWORD rootKey, nVec &addNodes)
{
	iEditDoc* pDoc = GetDocument();
	
	HTREEITEM hRoot = findKeyItem(rootKey, tree().GetRootItem());
	HTREEITEM hNew;
	if (addNodes.size() > 0) {
		hNew = tree().InsertItem(addNodes[0].getName(), 0, 0, hRoot);
		tree().SetItemData(hNew, addNodes[0].getKey());
		tree().SetItemState(hNew, TVIS_EXPANDED, TVIS_EXPANDED);
		tree().SetItemImage(hNew, 0, 0);
		addNodes[0].setParent(tree().GetItemData(hRoot));
		GetDocument()->addNode2(addNodes[0]);
	}
	
	int loop = addNodes.size() - 1;
	if (loop <= 0) return;
	
	COnProcDlg prcdlg;
	prcdlg.Create(IDD_ONPROC);
	prcdlg.m_ProcName.SetWindowText("登録中");
	prcdlg.m_ProgProc.SetStep(1);              // プログレスバーの初期設定
	prcdlg.m_ProgProc.SetRange(0, loop);
	
	DWORD preKey = rootKey;
	HTREEITEM hParent = hRoot;
	HTREEITEM hSel = hParent;
	int prevLevel = addNodes[0].getLevel();
	HTREEITEM hPrevNew = hNew;
	for (unsigned int i = 1; i < addNodes.size(); i++) {
		preKey = addNodes[i-1].getKey();
		hPrevNew = hNew;
		prevLevel = addNodes[i-1].getLevel();
		if (prevLevel > addNodes[i].getLevel()) {
			unsigned int diff = prevLevel - addNodes[i].getLevel();
			
			HTREEITEM hIt = hPrevNew;
			HTREEITEM hItParent = tree().GetParentItem(hIt);
			for (unsigned int u = 0; u <= diff; u++) {
				hItParent = tree().GetParentItem(hIt);
				hIt = hItParent;
			}
			hNew = tree().InsertItem(addNodes[i].getName(), 0, 0, hItParent);
			hParent = hItParent;
		} else if (prevLevel < addNodes[i].getLevel()) {
			hNew = tree().InsertItem(addNodes[i].getName(), 0, 0, hPrevNew);
			hParent = hPrevNew;
		} else {
			hNew = tree().InsertItem(addNodes[i].getName(), 0, 0, hParent, hPrevNew);
		}
		addNodes[i].setParent(tree().GetItemData(hParent));
		GetDocument()->addNode2(addNodes[i]);
		tree().SetItemData(hNew, addNodes[i].getKey());
		tree().SetItemImage(hNew, 0, 0);
		
		prcdlg.m_ProgProc.StepIt();  // プログレスバーを更新
	}
	
	tree().SelectItem(hSel);
	iHint h; h.event = iHint::reflesh;
	GetDocument()->UpdateAllViews(NULL, (LPARAM)tree().GetItemData(curItem()), &h);
}


HTREEITEM OutlineView::findKeyItem(DWORD key, HTREEITEM item=NULL)
{
	HTREEITEM hItem;
	if (item == 0) {
		item = tree().GetRootItem();
	}
	while (item != NULL) {
		hItem = item;
		if (tree().GetItemData(hItem) == key) {
			return hItem;
		}
		if (tree().ItemHasChildren(hItem)) {
			HTREEITEM cItem = findKeyItem(key, tree().GetChildItem(item));
			if (cItem != NULL) {
				return cItem;
			}
		}
		item = tree().GetNextSiblingItem(item);
	}
	return NULL;
}

void OutlineView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加してください
	CMenu menu;
	menu.LoadMenu(IDR_CONTEXT);
	
	CMenu* pPopup;
	if (!m_bHitR && point.x > 0 && point.y > 0) {
		return;
	} else {
		pPopup = menu.GetSubMenu(0);
	}
	
	if (m_bAddingLink) {
		m_bAddingLink = false;
		return;
	}
	
	ASSERT(pPopup != NULL);
	if (point.x < 0 || point.y < 0) {
		CRect r; GetWindowRect(&r);
		point = r.TopLeft();
		tree().GetItemRect(curItem(), &r, TRUE);
		CPoint p; p.x = (r.left + r.right)/2; p.y = (r.top + r.bottom)/2;
		point += p;
	}
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
}

void OutlineView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	if (m_bAddingLink) {
		m_bAddingLink = false;
	}
	
	m_bHitR = false;
	
	TV_HITTESTINFO hitTestInfo;
	hitTestInfo.pt = point;
	tree().HitTest(&hitTestInfo);
	
	if (hitTestInfo.flags & TVHT_ONITEMRIGHT || hitTestInfo.flags & TVHT_ONITEMINDENT ||
		hitTestInfo.flags & TVHT_ONITEMBUTTON || hitTestInfo.flags & TVHT_NOWHERE) {
		CTreeView::OnRButtonDown(nFlags, point);
		return;
	}
	
	if (hitTestInfo.hItem != 0) m_bHitR = true;
	tree().SelectItem(hitTestInfo.hItem);
}

///////////////////////////
// 子ノード追加
void OutlineView::OnAddChild() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (m_bAdding) return;
	CEdit* pEdit = tree().GetEditControl();
	if (pEdit != NULL) return;
	
	m_bAdding = true;
	m_bAddingChild = true;
	GetDocument()->disableUndo();
	clearUndo();
	HTREEITEM newItem = tree().InsertItem("新しいノード", 0, 0, curItem());
	m_HNew = newItem;
	tree().Expand(curItem(), TVE_EXPAND);
	tree().SelectItem(newItem);
	tree().EditLabel(newItem);
}

void OutlineView::OnUpdateAddChild(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bAdding);
}

void OutlineView::OnAddSibling() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (m_bAdding) return;
	
	CEdit* pEdit = tree().GetEditControl();
	if (pEdit != NULL) return;
	
	m_bAdding = true;
	m_bAddingChild = false;
	GetDocument()->disableUndo();
	clearUndo();
	CString cur = tree().GetItemText(curItem());
	int cr;
	if ((cr = cur.Find('\r')) != -1) {
		cur = cur.Left(cr);
	}
	HTREEITEM newItem = tree().InsertItem(cur, 0, 0, tree().GetParentItem(curItem()), curItem());
	m_HNew = newItem;
	tree().SelectItem(newItem);
	tree().EditLabel(newItem);
}

void OutlineView::OnUpdateAddSibling(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(curItem() != tree().GetRootItem());
}

void OutlineView::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_bLabelEditting = true;
	*pResult = 0;
}

void OutlineView::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_bLabelEditting = false;
	CString editString = pTVDispInfo->item.pszText;
	
	iEditDoc* pDoc = GetDocument();
	if (!m_bAdding) {
		// ラベルが編集された場合
		if (editString != "") {
			pTVDispInfo->item.mask = TVIF_TEXT;
			tree().SetItem(&pTVDispInfo->item);
			pDoc->setKeyNodeName(tree().GetItemData(curItem()), tree().GetItemText(pTVDispInfo->item.hItem));
		}
	} else {
		// ラベルが追加された場合
		if (editString.GetLength() > 256) {
			lstrcpy(pTVDispInfo->item.pszText, editString.Left(255));
		}
		
		if (editString != "") {
			pTVDispInfo->item.mask = TVIF_TEXT;
			tree().SetItem(&pTVDispInfo->item);
		}
		
		label l;
		l.name = tree().GetItemText(pTVDispInfo->item.hItem);
		l.key = pDoc->getUniqKey();
	//	tree().SetItemData(tree().GetSelectedItem(), l.key); // バグの温床 curItem() != NewItemの場合がある
		tree().SetItemData(m_HNew, l.key);
		l.parent = tree().GetItemData(tree().GetParentItem(m_HNew));
		
		DWORD keyInherit = l.parent;
		if (!m_bAddingChild) {
			keyInherit = tree().GetItemData(tree().GetPrevSiblingItem(tree().GetSelectedItem()));
		}
		bool binherit = false;
		CiEditApp* pApp = (CiEditApp*)AfxGetApp();
		if (pApp->m_rgsNode.bInheritParent && m_bAddingChild) {
			binherit = true;
		}
		if (pApp->m_rgsNode.bInheritSibling && !m_bAddingChild) {
			binherit = true;
		}
		
		pDoc->addNode(l, keyInherit, binherit);
		
		m_bAdding = false;
		int branchMode = getBranchMode();
		if (branchMode == 0) {
			pDoc->selChanged(tree().GetItemData(curItem()));
		} else {
			if (isPosterityOF(m_hItemShowRoot, curItem())) {
				// showBranchモードのiEditDocメソッドを呼ぶ
				pDoc->selChanged(tree().GetItemData(curItem()), m_bNodeSel, true);
			}
		}
	}
	GetDocument()->disableUndo();
	clearUndo();
	*pResult = 0;
}

void OutlineView::treeToSequence(Labels &ls)
{
	if (m_opTreeOut == 0) {
		setAllNodeLevels(); // level 設定
		treeview_for_each(tree(), copyLabels(ls), tree().GetRootItem());
	} else if (m_opTreeOut == 1) {
		setSubNodeLevels(); // level 設定
		label l;
		l.key = tree().GetItemData(tree().GetSelectedItem());
		if (tree().GetSelectedItem() != tree().GetRootItem()) {
			l.parent = tree().GetItemData(tree().GetParentItem(tree().GetSelectedItem()));
		}
		l.name = tree().GetItemText(tree().GetSelectedItem());
		l.state = tree().GetItemState(tree().GetSelectedItem(), TVIS_EXPANDED | TVIS_SELECTED);
		ls.push_back(l);
		if (tree().ItemHasChildren(tree().GetSelectedItem())) {
			treeview_for_each(tree(), copyLabels(ls), tree().GetChildItem(tree().GetSelectedItem()));
		}
	} else if (m_opTreeOut == 2) {
		setSubNodeLevels(); // level 設定
		label l;
		l.key = tree().GetItemData(tree().GetSelectedItem());
		if (tree().GetSelectedItem() != tree().GetRootItem()) {
			l.parent = tree().GetItemData(tree().GetParentItem(tree().GetSelectedItem()));
		}
		l.name = tree().GetItemText(tree().GetSelectedItem());
		l.state = tree().GetItemState(tree().GetSelectedItem(), TVIS_EXPANDED | TVIS_SELECTED);
		ls.push_back(l);
		if (tree().ItemHasChildren(tree().GetSelectedItem())) {
			treeview_for_each2(tree(), copyLabels(ls), tree().GetChildItem(tree().GetSelectedItem()));
		}
	}
}

void OutlineView::treeToSequence0(Labels &ls)
{
	setAllNodeLevels();
	treeview_for_each(tree(), copyLabels(ls), tree().GetRootItem());
}

serialVec OutlineView::getDrawOrder(const bool bShowSubBranch) const
{
	serialVec vec;
	if (!bShowSubBranch) {
		if (tree().GetRootItem() == tree().GetSelectedItem()) {
			vec.push_back(0);
			treeview_for_each2(tree(), copyKeyVec(vec), tree().GetChildItem(tree().GetSelectedItem()));
		} else {
			HTREEITEM hParent = tree().GetParentItem(tree().GetSelectedItem());
			if (hParent == tree().GetRootItem()) {
				vec.push_back(0);
			}
			treeview_for_each2(tree(), copyKeyVec(vec), tree().GetChildItem(hParent));
		}
	} else {
		int branchMode = getBranchMode();
		if (branchMode == 2) {
			treeview_for_each(tree(), copyKeyVec(vec), m_hItemShowRoot);
		} else if (branchMode == 1) {
			treeview_for_each2(tree(), copyKeyVec(vec), tree().GetChildItem(m_hItemShowRoot));
		}
	}
	return vec;
}

void OutlineView::OnEditLabel() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	tree().EditLabel(curItem());
}

void OutlineView::OnUpdateEditLabel(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	
}

void OutlineView::OnLebelUp() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (m_bAdding) return;
	HTREEITEM hParent = tree().GetParentItem(curItem());
	HTREEITEM hGrdParent = tree().GetParentItem(hParent);
	HTREEITEM hcur = curItem();
	
	m_hParentPreMove = tree().GetParentItem(curItem());
	m_hSiblingPreMove = tree().GetPrevSiblingItem(curItem());
	
	HTREEITEM hNew = tree().InsertItem(tree().GetItemText(hcur), 0, 0, hGrdParent, hParent);
	tree().SetItemState(hNew, tree().GetItemState(hcur, TVIS_EXPANDED), TVIS_EXPANDED);
	tree().SetItemData(hNew, tree().GetItemData(hcur));
	int nImage; tree().GetItemImage(hcur, nImage, nImage);
	tree().SetItemImage(hNew, nImage, nImage);
	tree().Expand(hGrdParent, TVE_EXPAND);
	GetDocument()->setKeyNodeParent(tree().GetItemData(hcur), tree().GetItemData(hGrdParent));
	
	if (tree().ItemHasChildren(hcur)) {
		copySubNodes(tree().GetChildItem(hcur), hNew);
	}
	tree().DeleteItem(hcur);
	GetDocument()->disableUndo();
	m_hItemMoved = hNew; // Undo Info
	tree().SelectItem(hNew);
}

void OutlineView::OnUpdateLebelUp(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(curItem() != tree().GetRootItem() && tree().GetParentItem(curItem()) != tree().GetRootItem());
}

void OutlineView::OnLebelDown() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (m_bAdding) return;
	HTREEITEM hcur = curItem();
	
	m_hParentPreMove = tree().GetParentItem(curItem());
	m_hSiblingPreMove = tree().GetPrevSiblingItem(curItem());
	
	HTREEITEM hNew = tree().InsertItem(tree().GetItemText(hcur), 0, 0, tree().GetPrevSiblingItem(hcur));
	tree().SetItemState(hNew, tree().GetItemState(hcur, TVIS_EXPANDED), TVIS_EXPANDED);
	tree().SetItemData(hNew, tree().GetItemData(hcur));
	int nImage; tree().GetItemImage(hcur, nImage, nImage);
	tree().SetItemImage(hNew, nImage, nImage);
	tree().Expand(tree().GetPrevSiblingItem(hcur), TVE_EXPAND);
	GetDocument()->setKeyNodeParent(tree().GetItemData(hcur), tree().GetItemData(tree().GetPrevSiblingItem(hcur)));
	
	if (tree().ItemHasChildren(hcur)) {
		copySubNodes(tree().GetChildItem(hcur), hNew);
	}
	tree().SelectItem(hNew); // 先に選択状態にすることで、親ノード選択を防ぐ
	tree().DeleteItem(hcur);
	GetDocument()->disableUndo();
	m_hItemMoved = hNew; // Undo Info
	tree().SelectItem(hNew);
}

void OutlineView::OnUpdateLebelDown(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(curItem() != tree().GetRootItem() && tree().GetPrevSiblingItem(curItem()) != NULL);
}

void OutlineView::OnOrderUp() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (m_bAdding) return;
	HTREEITEM hcur = curItem();
	HTREEITEM hpre = tree().GetPrevSiblingItem(curItem());
	HTREEITEM hprepre = tree().GetPrevSiblingItem(hpre);
	HTREEITEM hNew;
	
	// Undo Info
	m_hParentPreMove = tree().GetParentItem(curItem());
	m_hSiblingPreMove = hpre;
	
	if (hprepre != NULL) {
		hNew = tree().InsertItem(tree().GetItemText(hcur), 0, 0, tree().GetParentItem(hcur), hprepre);
	} else {
		hNew = tree().InsertItem(tree().GetItemText(hcur), 0, 0, tree().GetParentItem(hcur), TVI_FIRST);
	}
	tree().SetItemData(hNew, tree().GetItemData(hcur));
	int nImage; tree().GetItemImage(hcur, nImage, nImage);
	tree().SetItemImage(hNew, nImage, nImage);
	tree().SetItemState(hNew, tree().GetItemState(hcur, TVIS_EXPANDED), TVIS_EXPANDED);
	if (tree().ItemHasChildren(hcur)) {
		copySubNodes(tree().GetChildItem(hcur), hNew);
	}
	tree().SelectItem(hNew);
	tree().DeleteItem(hcur);
	GetDocument()->disableUndo();
	m_hItemMoved = hNew; // Undo Info
	tree().SelectItem(hNew);
	GetDocument()->SetModifiedFlag();
}

void OutlineView::OnUpdateOrderUp(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(tree().GetPrevSiblingItem(curItem()) != NULL);
}

void OutlineView::OnOrderDown() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (m_bAdding) return;
	HTREEITEM hcur = curItem();
	
	// Undo Info
	m_hParentPreMove = tree().GetParentItem(curItem());
	m_hSiblingPreMove = tree().GetPrevSiblingItem(curItem());
	
	HTREEITEM hNew = tree().InsertItem(tree().GetItemText(hcur), 0, 0, tree().GetParentItem(hcur), tree().GetNextSiblingItem(hcur));
	tree().SetItemData(hNew, tree().GetItemData(hcur));
	int nImage; tree().GetItemImage(hcur, nImage, nImage);
	tree().SetItemImage(hNew, nImage, nImage);
	tree().SetItemState(hNew, tree().GetItemState(hcur, TVIS_EXPANDED), TVIS_EXPANDED);
	if (tree().ItemHasChildren(hcur)) {
		copySubNodes(tree().GetChildItem(hcur), hNew);
	}
	tree().DeleteItem(hcur);
	GetDocument()->disableUndo();
	m_hItemMoved = hNew; // Undo Info
	tree().SelectItem(hNew);
	GetDocument()->SetModifiedFlag();
}

void OutlineView::OnUpdateOrderDown(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(tree().GetNextSiblingItem(curItem()) != NULL);
}

void OutlineView::copySubNodes(HTREEITEM hOrg, HTREEITEM hNewParent)
{
	HTREEITEM hItem, item;
	item = hOrg;
	while (item != NULL) {
		hItem = item;
		HTREEITEM hNew = tree().InsertItem(tree().GetItemText(hItem), 0, 0, hNewParent);
		tree().SetItemData(hNew, tree().GetItemData(hItem));
		int nImage; tree().GetItemImage(hItem, nImage, nImage);
		tree().SetItemImage(hNew, nImage, nImage);
		tree().SetItemState(hNew, tree().GetItemState(hItem, TVIS_EXPANDED), TVIS_EXPANDED);
		if (tree().ItemHasChildren(hItem)) {
			copySubNodes(tree().GetChildItem(hItem), hNew);
		}
		item = tree().GetNextSiblingItem(item);
	}
}

void OutlineView::OnDelete() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	GetDocument()->disableUndo();
	clearUndo();
	deleteNode();
}

void OutlineView::OnUpdateDelete(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(curItem() != tree().GetRootItem() && !m_bLabelEditting ||
		           m_bLabelEditting);
}

void OutlineView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	bool bShowBranch = false;
	int branchMode = getBranchMode();
	
	if (branchMode == 1) {
		if (tree().GetParentItem(curItem()) != 	m_hItemShowRoot && m_hItemShowRoot != curItem()) {
			resetShowBranch();
			GetDocument()->resetShowBranch();
		} else {
			bShowBranch = true;
		}
	} else if (branchMode == 2) {
		if (!isPosterityOF(m_hItemShowRoot, curItem()) && m_hItemShowRoot != curItem()) {
			resetShowBranch();
			GetDocument()->resetShowBranch();
		} else {
			bShowBranch = true;
		}
	}
	
	if (!m_bAdding) {
		GetDocument()->selChanged(tree().GetItemData(curItem()), true, bShowBranch);
	}
	*pResult = 0;
}

void OutlineView::OnEditUndo() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (m_bLabelEditting) {
		tree().GetEditControl()->Undo();
		return;
	}
	if (m_hItemMoved == NULL && GetDocument()->canResumeUndo()) {
		GetDocument()->resumeUndoNodes();
		GetDocument()->resumeUndoLinks();
		iHint h; h.event = iHint::reflesh;
		GetDocument()->UpdateAllViews(NULL, (LPARAM)tree().GetItemData(curItem()), &h);
		return;
	}
	
	HTREEITEM hNew;
	if (m_hSiblingPreMove != NULL) {
		hNew = tree().InsertItem(tree().GetItemText(m_hItemMoved), 0, 0, m_hParentPreMove, m_hSiblingPreMove);
	} else {
		hNew = tree().InsertItem(tree().GetItemText(m_hItemMoved), 0, 0, m_hParentPreMove, TVI_FIRST);
	}
	tree().SetItemData(hNew, tree().GetItemData(m_hItemMoved));
	int nImage; tree().GetItemImage(m_hItemMoved, nImage, nImage);
	tree().SetItemImage(hNew, nImage, nImage);
	tree().SetItemState(hNew, tree().GetItemState(m_hItemMoved, TVIS_EXPANDED), TVIS_EXPANDED);
	GetDocument()->setKeyNodeParent(tree().GetItemData(m_hItemMoved), tree().GetItemData(m_hParentPreMove));
	if (tree().ItemHasChildren(m_hItemMoved)) {
		copySubNodes(tree().GetChildItem(m_hItemMoved), hNew);
	}
	tree().DeleteItem(m_hItemMoved);
	GetDocument()->disableUndo();
	clearUndo();
	tree().SelectItem(hNew);
	GetDocument()->SetModifiedFlag();
}

void OutlineView::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(m_bLabelEditting || m_hItemMoved != NULL || GetDocument()->canResumeUndo());

}

void OutlineView::OnEditCut() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (m_bLabelEditting) {
		tree().GetEditControl()->Cut();
	}
}

void OutlineView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	
}

void OutlineView::OnEditCopy() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (m_bLabelEditting) {
		tree().GetEditControl()->Copy();
	} else {
		GetDocument()->setSelectedNodeCopyOrg();
	}
}

void OutlineView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	
}

void OutlineView::OnEditPaste() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (m_bLabelEditting) {
		tree().GetEditControl()->Paste();
	} else {
		GetDocument()->disableUndo();
		GetDocument()->makeCopyNode(CPoint(0, 0));
	}
}

void OutlineView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	if (!m_bLabelEditting) {
		pCmdUI->Enable(GetDocument()->canCopyNode());
	}
}

void OutlineView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	if (pHint == NULL) return;
	m_bNodeSel = true;
	
	DWORD key = (DWORD)lHint;
	HTREEITEM item;
	iHint* ph = reinterpret_cast<iHint*>(pHint);
	switch (ph->event) {
	case iHint::nodeDelete:
		clearUndo();
		deleteNode();
		break;
	case iHint::nodeDeleteMulti:
		clearUndo();
		deleteKeyNode(key, ph->keyParent);
		break;
	case iHint::nodeSel:
	case iHint::parentSel:
		item = findKeyItem(key, tree().GetRootItem());
		if (item != NULL) {
			tree().SelectItem(item);
		}
		break;
	case iHint::linkSelRet:
	case iHint::linkSel:
		m_bNodeSel = false;
		item = findKeyItem(key, tree().GetRootItem());
		if (item != NULL) {
			tree().SelectItem(item);
		}
		break;
	case iHint::arcAdd:
	case iHint::rectAdd:
		clearUndo();
		HTREEITEM newItem;
		if (tree().GetSelectedItem() == tree().GetRootItem()) {
			newItem = tree().InsertItem(ph->str, 0, 0, curItem());
		} else {
			newItem = tree().InsertItem(ph->str, 0, 0, tree().GetParentItem(curItem()), curItem());
		}
		tree().SetItemData(newItem, key);
		tree().SetItemImage(newItem, ph->treeIconId, ph->treeIconId);
	//	tree().Expand(curItem(), TVE_EXPAND);
		tree().SelectItem(newItem);
		break;
	case iHint::nodeLabelChanged:
		clearUndo();
		tree().SetItemText(curItem(), ph->str);
		break;
	case iHint::selectChild:
		if (tree().ItemHasChildren(curItem())) {
			tree().SelectItem(tree().GetChildItem(curItem()));
		}
		break;
	case iHint::selectParent:
		break;
	case iHint::linkAdd:
	case iHint::linkDelete:
	case iHint::linkModified:
	case iHint::lelax:
	case iHint::linkCurved:
	case iHint::linkStraight:
	case iHint::nodeStyleChanged:
		clearUndo();
		break;
	case iHint::nodeDeleteByKey:
		break;
	case iHint::viewSettingChanged:
		doColorSetting();
		setViewFont();
		break;
	
	case iHint::groupMigrate:
		moveNodes(ph->keyTarget, key);
		break;
	case iHint::nextNodeSibling:
		HTREEITEM hNext = tree().GetNextSiblingItem(curItem());
		if (hNext != NULL) {
			tree().SelectItem(hNext);
		} else {
			HTREEITEM hParent = tree().GetParentItem(curItem());
			tree().SelectItem(tree().GetNextItem(hParent, TVGN_CHILD));
		}
		break;

	}
}

void OutlineView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	int branchMode = getBranchMode();
	if (bActivate) {
		GetDocument()->selChanged(tree().GetItemData(curItem()), true, branchMode != 0);
	}
	
	CTreeView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void OutlineView::OnAddLink() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	m_bAddingLink = !m_bAddingLink;
}

void OutlineView::OnUpdateAddLink(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->SetCheck(m_bAddingLink);
}

BOOL OutlineView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	if (m_bItemDragging) {
		::SetCursor(m_hCsrMove);
		return TRUE;
	}
	if (m_bAddingLink && pWnd == this && nHitTest == HTCLIENT) {
		::SetCursor(m_hLinkCsr);
		return TRUE;
	}
	return CTreeView::OnSetCursor(pWnd, nHitTest, message);
}

void OutlineView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	TV_HITTESTINFO hitTestInfo;
	hitTestInfo.pt = point;
	tree().HitTest(&hitTestInfo);
	if (hitTestInfo.flags & TVHT_ONITEMRIGHT || hitTestInfo.flags & TVHT_ONITEMINDENT ||
		hitTestInfo.flags & TVHT_ONITEMBUTTON || hitTestInfo.flags & TVHT_NOWHERE) {
		CTreeView::OnLButtonDown(nFlags, point);
		return;
	}
	if (m_bAddingLink) {
		LinkInfoDlg dlg;
		dlg.strFrom = tree().GetItemText(curItem());
		dlg.strTo = tree().GetItemText(hitTestInfo.hItem);
		dlg.styleArrow = 0;
		if (dlg.DoModal() == IDOK) {
			// iEditDocのリンク追加処理
			GetDocument()->setNewLinkInfo(tree().GetItemData(curItem()), tree().GetItemData(hitTestInfo.hItem),
				                          dlg.strComment, dlg.styleArrow);
		}
		m_bAddingLink = false;
		return;
	} else if (!m_bItemDragging) {
		m_hitemDragPre = hitTestInfo.hItem;
	}
	CTreeView::OnLButtonDown(nFlags, point);
}

void OutlineView::deleteNode()
{
	if (m_bLabelEditting) {
		CEdit* pEdit = tree().GetEditControl();
		pEdit->SendMessage(WM_KEYDOWN, VK_DELETE, VK_DELETE);
		return;
	}
	HTREEITEM hcur = curItem();
	CString m = "<" + tree().GetItemText(hcur) + ">" + '\n' + "削除しますか？";
	if (MessageBox(m, "ノードの削除", MB_YESNO) != IDYES) return;
	GetDocument()->backupDeleteBound();
	if (tree().GetNextSiblingItem(curItem()) == NULL) {
		if (tree().GetPrevSiblingItem(curItem()) != NULL) {
			tree().SelectItem(tree().GetPrevSiblingItem(curItem()));
		}
	}
	GetDocument()->deleteKeyItem(tree().GetItemData(hcur));
	if (tree().ItemHasChildren(hcur)) {
		Labels ls;
		treeview_for_each(tree(), copyLabels(ls), tree().GetChildItem(hcur));
		for (unsigned int i = 0; i < ls.size(); i++) {
			GetDocument()->deleteKeyItem(ls[i].key);
		}
	}
	tree().DeleteItem(hcur);
}

void OutlineView::deleteKeyNode(DWORD key, DWORD parentKey)
{
	if (key == 0) return;
	HTREEITEM hParent = findKeyItem(parentKey);
	if (hParent == NULL) return;
	HTREEITEM hDeleteItem = findKeyItem(key, hParent);
	//if (tree().GetNextSiblingItem(hDeleteItem) == NULL) {
	//	if (tree().GetPrevSiblingItem(hDeleteItem) != NULL) {
	//		tree().SelectItem(tree().GetPrevSiblingItem(hDeleteItem));
	//	}
	//}
	if (hDeleteItem == NULL) return;
	GetDocument()->deleteKeyItem(tree().GetItemData(hDeleteItem));
	if (tree().ItemHasChildren(hDeleteItem)) {
		Labels ls;
		treeview_for_each(tree(), copyLabels(ls), tree().GetChildItem(hDeleteItem));
		for (unsigned int i = 0; i < ls.size(); i++) {
			GetDocument()->deleteKeyItem(ls[i].key);
		}
	}
	tree().DeleteItem(hDeleteItem);
}

void OutlineView::OnAddUrl() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	LinkInfo2Dlg dlg;
	dlg.strOrg = tree().GetItemText(curItem());
	if (dlg.DoModal() != IDOK) return;
	GetDocument()->disableUndo();
	clearUndo();
	if (dlg.strComment == "" && dlg.strPath != "") {
		char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
		_splitpath_s(dlg.strPath, drive, dir, fname, ext);
		dlg.strComment.Format("%s%s", fname, ext);
	}
	GetDocument()->addURLLink(dlg.strPath, dlg.strComment);
}

void OutlineView::OnUpdateAddUrl(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	
}

void OutlineView::OnSelectChild() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	tree().SelectItem(tree().GetChildItem(curItem()));
}

void OutlineView::OnUpdateSelectChild(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(tree().ItemHasChildren(curItem()));
}

void OutlineView::OnSelectParent() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	tree().SelectItem(tree().GetParentItem(curItem()));
}

void OutlineView::OnUpdateSelectParent(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(curItem() != tree().GetRootItem());
}

void OutlineView::setViewFont()
{
	LOGFONT lf;
	CFont *pFont = GetFont();
	if (pFont != NULL) {
		pFont->GetObject(sizeof(LOGFONT), &lf);
	} else {
		::GetObject(GetStockObject(SYSTEM_FIXED_FONT), sizeof(LOGFONT), &lf);
	}
	::lstrcpy(lf.lfFaceName, AfxGetApp()->GetProfileString(REGS_FRAME, "Font1 Name", "ＭＳ Ｐゴシック"));
	lf.lfHeight = AfxGetApp()->GetProfileInt(REGS_FRAME, "Font1 Height", 0xfffffff3);
	lf.lfWidth = AfxGetApp()->GetProfileInt(REGS_FRAME, "Font1 Width", 0);
	lf.lfItalic = AfxGetApp()->GetProfileInt(REGS_FRAME, "Font1 Italic", FALSE);
	lf.lfUnderline = AfxGetApp()->GetProfileInt(REGS_FRAME, "Font1 UnderLine", FALSE);
	lf.lfStrikeOut = AfxGetApp()->GetProfileInt(REGS_FRAME, "Font1 StrikeOut", FALSE);
	lf.lfCharSet= AfxGetApp()->GetProfileInt(REGS_FRAME, "Font1 CharSet", SHIFTJIS_CHARSET);
	lf.lfWeight = AfxGetApp()->GetProfileInt(REGS_FRAME, "Font1 Weight", FW_NORMAL);
	m_font.CreateFontIndirect(&lf);
	SetFont(&m_font, TRUE);
}

void OutlineView::OnSortChildren() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	tree().SortChildren(curItem());
}

void OutlineView::OnUpdateSortChildren(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(tree().ItemHasChildren(curItem()));
}

void OutlineView::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_bItemDragging = true;
	
	CPoint		ptAction;
	UINT		nFlags;
	
	GetCursorPos(&ptAction);
	ScreenToClient(&ptAction);
	if (m_hitemDragPre != NULL) {
		m_hitemDrag = m_hitemDragPre;
		m_hitemDragPre = NULL;
	} else {
		m_hitemDrag = tree().HitTest(ptAction, &nFlags);
	}
	tree().SelectItem(m_hitemDrag);
	m_hitemDrop = NULL;
	*pResult = 0;
}

void OutlineView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	if (m_bItemDragging) {
		tree().SelectDropTarget(NULL);
		if (m_nDropStatus == OutlineView::drop_child) {
			if (m_hitemDrag != m_hitemDrop && m_hitemDrop != NULL && !IsChildNodeOf(m_hitemDrop, m_hitemDrag) && 
															tree().GetParentItem(m_hitemDrag) != m_hitemDrop) {
				
				m_hParentPreMove = tree().GetParentItem(m_hitemDrag);
				m_hSiblingPreMove = tree().GetPrevSiblingItem(m_hitemDrag);
				
				
				HTREEITEM hNew = tree().InsertItem(tree().GetItemText(m_hitemDrag), 0, 0, m_hitemDrop);
				tree().SetItemState(hNew, tree().GetItemState(m_hitemDrag, TVIS_EXPANDED), TVIS_EXPANDED);
				tree().SetItemData(hNew, tree().GetItemData(m_hitemDrag));
				int nImage; tree().GetItemImage(m_hitemDrag, nImage, nImage);
				tree().SetItemImage(hNew, nImage, nImage);
				tree().Expand(m_hitemDrop, TVE_EXPAND);
				GetDocument()->setKeyNodeParent(tree().GetItemData(m_hitemDrag), tree().GetItemData(m_hitemDrop));
				if (tree().ItemHasChildren(m_hitemDrag)) {
					copySubNodes(tree().GetChildItem(m_hitemDrag), hNew);
				}
			//	tree().DeleteItem(m_hitemDrag);
				GetDocument()->disableUndo();
				m_hItemMoved = hNew; // Undo Info
				tree().SelectItem(hNew);         // ここで1度選択しておかないと
				tree().DeleteItem(m_hitemDrag);  // 一番下端にあるノードを消した時にF6状態くずれる
				tree().SelectItem(hNew);         // 再度選択
			} else {
				tree().SelectItem(m_hitemDrag);
			}
		} else if (m_nDropStatus == OutlineView::drop_sibling) {
			if (m_hitemDrag != m_hitemDrop && m_hitemDrop != NULL &&
			//	tree().GetRootItem() != m_hitemDrop  &&
				!IsChildNodeOf(m_hitemDrop, m_hitemDrag)) {
				
				m_hParentPreMove = tree().GetParentItem(m_hitemDrag);
				m_hSiblingPreMove = tree().GetPrevSiblingItem(m_hitemDrag);
				
				if (tree().ItemHasChildren(m_hitemDrop) && 
					(TVIS_EXPANDED & tree().GetItemState(m_hitemDrop, TVIS_EXPANDED))) {
					// Drop先ノードが子階層を持っているときに、子階層の先頭にするか
					// Drop先ノードと同一階層にするか、メニューで選ぶようにした。
					// 開いているときだけコンテキストメニューを出すようにしたいのだが・・・
					CPoint sPt = point;
					ClientToScreen(&sPt);
					CMenu menu;
					menu.LoadMenu(IDR_CONTEXT);
					CMenu* pPopup = menu.GetSubMenu(7);
					pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, sPt.x, sPt.y, AfxGetMainWnd());
					return;
				}
				HTREEITEM hParent = tree().GetParentItem(m_hitemDrop);
				HTREEITEM hNew = tree().InsertItem(tree().GetItemText(m_hitemDrag), 0, 0, hParent, m_hitemDrop);
				tree().SetItemData(hNew, tree().GetItemData(m_hitemDrag));
				int nImage; tree().GetItemImage(m_hitemDrag, nImage, nImage);
				tree().SetItemImage(hNew, nImage, nImage);
				GetDocument()->setKeyNodeParent(tree().GetItemData(m_hitemDrag), tree().GetItemData(hParent));
				if (tree().ItemHasChildren(m_hitemDrag)) {
					copySubNodes(tree().GetChildItem(m_hitemDrag), hNew);
				}
				
				GetDocument()->disableUndo();
				m_hItemMoved = hNew; // Undo Info
				tree().SelectItem(hNew);
				tree().DeleteItem(m_hitemDrag);
				tree().SelectItem(hNew);
			} else {
				tree().SelectItem(m_hitemDrag);
			}
		}
		
		tree().SetInsertMark(NULL);
		tree().SelectDropTarget(NULL);
		m_bItemDragging = false;
	//	Invalidate();
	}
	CTreeView::OnLButtonUp(nFlags, point);
}

void OutlineView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	if (m_bItemDragging)
	{
		if (!nFlags & MK_LBUTTON) {
			tree().SetInsertMark(NULL);
			tree().SelectDropTarget(NULL);
			m_bItemDragging = false;
			CTreeView::OnMouseMove(nFlags, point);
			return;
		}
		
		TV_HITTESTINFO hitTestInfo;
		hitTestInfo.pt = point;
		tree().HitTest(&hitTestInfo);
		
		m_nDropStatus = OutlineView::drop_none;
		if (hitTestInfo.flags & TVHT_ONITEMLABEL) {
			m_nDropStatus = OutlineView::drop_child;  // child
		//	tree().SetInsertMarkColor(RGB(127, 0, 0));
			tree().SetInsertMark(NULL);
			tree().SelectDropTarget(hitTestInfo.hItem);
		} else if (hitTestInfo.flags & TVHT_ONITEMINDENT ||
			        hitTestInfo.flags & TVHT_ONITEMBUTTON ||
					hitTestInfo.flags & TVHT_ONITEMICON ||
					hitTestInfo.flags & TVHT_ONITEMRIGHT) {
			m_nDropStatus = OutlineView::drop_sibling; // sibling
		//	tree().SetInsertMarkColor(RGB(0, 0, 0));
			tree().SelectDropTarget(NULL);
			tree().SetInsertMark(hitTestInfo.hItem, TRUE);
		} else if (hitTestInfo.flags &  TVHT_NOWHERE) {
			m_nDropStatus = OutlineView::drop_none;
			tree().SetInsertMark(NULL);
			tree().SelectDropTarget(NULL);
		} else if (hitTestInfo.flags & TVHT_ABOVE ||
			        hitTestInfo.flags & TVHT_BELOW ||
					hitTestInfo.flags & TVHT_TOLEFT ||
					hitTestInfo.flags & TVHT_TORIGHT) {
			m_nDropStatus = OutlineView::drop_none;
			tree().SetInsertMark(NULL);
			tree().SelectDropTarget(NULL);
			m_bItemDragging = false;
		}
		
		CRect rc; GetClientRect(rc);
		if (m_nDropStatus == 2 || m_nDropStatus == 1)
		{
			m_hitemDrop = hitTestInfo.hItem;
			if (point.y < tree().GetItemHeight()) {
				tree().Select(tree().GetPrevVisibleItem(tree().GetFirstVisibleItem()), TVGN_FIRSTVISIBLE);
			} else if (point.y > rc.Height() - tree().GetItemHeight()) {
				tree().Select(tree().GetNextVisibleItem(tree().GetFirstVisibleItem()), TVGN_FIRSTVISIBLE);
			}
		} else {
			m_hitemDrop = NULL;
		}
	}

	CTreeView::OnMouseMove(nFlags, point);
}

void OutlineView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	if (nChar == VK_ESCAPE) {
		if (m_bItemDragging) {
			m_bItemDragging = false;
			ReleaseCapture();
			tree().SelectDropTarget(NULL);
			tree().SetInsertMark(NULL);
			Invalidate();
		}
		int branchMode = getBranchMode();
		if (branchMode == 0) {
			GetDocument()->selChanged(tree().GetItemData(curItem()));
		}
	}
	CTreeView::OnKeyUp(nChar, nRepCnt, nFlags);
}

BOOL OutlineView::IsChildNodeOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent)
{
	do
	{
		if (hitemChild == hitemSuspectedParent)
			break;
	}
	while ((hitemChild = tree().GetParentItem(hitemChild)) != NULL);
	
	return (hitemChild != NULL);
}

void OutlineView::OnImportData() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	CString txtpath;
	char szFilters[] = "テキストファイル (*.txt)|*.txt|xmlファイル (*.xml)|*.xml||";
	CFileDialog dlg(TRUE, "txt", txtpath, OFN_HIDEREADONLY, szFilters, this);
	if (dlg.DoModal() != IDOK) return;
	CString infileName = dlg.GetPathName();
	
	
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath_s(infileName, drive, dir, fname, ext );
	CString extent = ext;
	extent.MakeLower();
	
	CString caption;
	int TextLevelCharNum = 0;
	if (extent == ".txt") {
		ImportTextDlg dlg;
		dlg.m_fileName = CString(fname) + CString(ext);
		dlg.m_charSelection = TextLevelCharNum;
		if (dlg.DoModal() != IDOK) {
			return;
		}
		TextLevelCharNum = dlg.m_charSelection;
	} else if (extent == ".xml") {
		caption = "XMLファイルのインポート";
		CString mes = infileName + "をインポートしますか";
		if (MessageBox(mes, caption, MB_YESNO) != IDYES) {
			return;
		}
	} else {
		return;
	}
	
	int mode = getBranchMode();
	HTREEITEM hShowRoot;
	if (mode != 0) {
		hShowRoot = m_hItemShowRoot;
		resetShowBranch();
		GetDocument()->resetShowBranch();
	}
	bool ret;
	if (extent == ".txt") {
		nVec addNodes;
		char levelChar = '.';
		if (TextLevelCharNum == 1) {
			levelChar = '\t';
		}
		ret = ImportText(infileName, addNodes, levelChar);
		if (ret) {
			treeAddBranch2(tree().GetItemData(curItem()), addNodes);
		}
	} else if (extent == ".xml") {
		ret = ImportXML(infileName);
		if (ret) {
			treeAddBranch(tree().GetItemData(curItem()));
		}
	}
	
	if (mode == 1) {
		tree().SelectItem(hShowRoot);
		OnShowSelectedChildren();
	} else if (mode == 2) {
		tree().SelectItem(hShowRoot);
		OnShowSelectedBranch();
	}
	if (ret) {
		MessageBox("終了しました", caption, MB_OK);
	} else {
		MessageBox("失敗しました", caption, MB_OK);
	}
}

void OutlineView::OnUpdateImportData(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	
}

void OutlineView::OutputHTML()
{
	SetHtmlExportDlg eDlg;
	eDlg.m_xvRdTree = m_exportOption.htmlOutOption;
	eDlg.m_xvRdNav = m_exportOption.navOption;
	eDlg.m_xvRdImg = m_exportOption.imgOption;
	eDlg.m_xvRdText = m_exportOption.textOption;
	eDlg.m_xvEdPrfIndex = m_exportOption.prfIndex;
	eDlg.m_xvEdPrfToc = m_exportOption.prfToc;
	eDlg.m_xvEdPrfNet = m_exportOption.prfNet;
	eDlg.m_xvEdPrfTextSingle = m_exportOption.prfTextSingle;
	eDlg.m_xvEdPrfTextEverynode = m_exportOption.prfTextEverynode;
	eDlg.m_pathTextSingle = m_exportOption.pathTextSingle;
	eDlg.m_sDocTitle = GetDocument()->getTitleFromPath();
	eDlg.m_NameOfRoot = tree().GetItemText(tree().GetRootItem());
	if (GetDocument()->isShowSubBranch()) {
		eDlg.m_NameOfVisibleRoot = tree().GetItemText(m_hItemShowRoot);
		eDlg.m_xvRdTree = 1;
	} else {
		if (tree().GetSelectedItem() == tree().GetRootItem()) {
			eDlg.m_NameOfVisibleRoot = tree().GetItemText(tree().GetRootItem());
		} else {
			eDlg.m_NameOfVisibleRoot = 
				tree().GetItemText(tree().GetParentItem(tree().GetSelectedItem()));
		}
	}
	if (eDlg.DoModal() != IDOK) return;
	m_exportOption.htmlOutOption = eDlg.m_xvRdTree;
	m_exportOption.navOption = eDlg.m_xvRdNav;
	m_exportOption.imgOption = eDlg.m_xvRdImg;
	m_exportOption.textOption = eDlg.m_xvRdText;
	m_exportOption.prfIndex = eDlg.m_xvEdPrfIndex;
	m_exportOption.prfToc = eDlg.m_xvEdPrfToc;
	m_exportOption.prfNet = eDlg.m_xvEdPrfNet;
	m_exportOption.prfTextSingle = eDlg.m_xvEdPrfTextSingle;
	m_exportOption.prfTextEverynode = eDlg.m_xvEdPrfTextEverynode;
	m_exportOption.pathTextSingle = eDlg.m_pathTextSingle;

	TCHAR szBuff[MAX_PATH];
	BROWSEINFO bi;
	bi.hwndOwner = m_hWnd;
	
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szBuff;
	bi.lpszTitle = _T("HTML出力先フォルダー選択");
	
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = (BFFCALLBACK)FolderDlgCallBackProc;
	bi.lParam = (LPARAM)m_exportOption.htmlOutDir.GetBuffer();
	bi.ulFlags &= BIF_DONTGOBELOWDOMAIN;
	bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS | BIF_EDITBOX;
	bi.iImage = 0;
	CString folder = AfxGetApp()->GetProfileString(_T("Settings"), _T("HTML OutputDir"), _T(""));
	bi.lParam = (LPARAM)folder.GetBuffer(folder.GetLength());
	
	CString outdir;
	LPITEMIDLIST pList = ::SHBrowseForFolder(&bi);
	if (pList == NULL) return;
    if (::SHGetPathFromIDList(pList, szBuff)) {
		//szBuffに選択したフォルダ名が入る
		outdir = CString(szBuff);
	} else {
		MessageBox("出力先フォルダーを指定して下さい");
		return;
	}
	m_exportOption.htmlOutDir = outdir;
	
	CWaitCursor wc;
	CStdioFile f;
	CFileStatus status;
	CFileException e;
	
	CString indexFilePath = outdir + "\\" + eDlg.m_pathIndex;
	CFileFind find;
	if (find.FindFile(indexFilePath)) {
		if (MessageBox(
			indexFilePath + "\n既存のファイルを上書きしてよいですか", "HTML出力",
			MB_YESNO) != IDYES) {
			return;
		}
	}
	CString textDir = outdir + "\\text";
	if (m_exportOption.textOption == 1) {
		if (!find.FindFile(textDir)) {
			if (!::CreateDirectory(textDir, NULL)) {
				MessageBox("フォルダー作成に失敗しました");
				return;
			}
		}
	}
	
	AfxGetApp()->WriteProfileString(_T("Settings"), _T("HTML OutputDir"), outdir);
	HTREEITEM root;
	if (m_exportOption.htmlOutOption == 0) {
		root = tree().GetRootItem();
	} else {
		if (GetDocument()->isShowSubBranch()) {
			root = m_hItemShowRoot;
		} else {
			if (tree().GetSelectedItem() != tree().GetRootItem()) {
				root = tree().GetParentItem(tree().GetSelectedItem());
			} else {
				root = tree().GetRootItem();
			}
		}
	}
	CString keystr;
	keystr.Format("%d", tree().GetItemData(root));
	
	////////////////////////
	////// create frame
	////////////////////////
	if (!f.Open(indexFilePath, CFile::typeText | CFile::modeCreate | CFile::modeWrite, &e)) {
		return;
	}
	writeHtmlHeader(f);
	CString title = GetDocument()->getTitleFromPath();
	if (m_exportOption.prfIndex != "") {
		title = m_exportOption.prfIndex;
	}
	f.WriteString("<title>" + title + "</title>\n");
	f.WriteString("</head>\n");
	if (eDlg.m_xvRdNav != 1) {
		f.WriteString("  <frameset cols=\"35%,*\" >\n");
		f.WriteString("    <frame src=\"" + eDlg.m_pathOutline + "\" name=\"outline\">\n");
	}
	if (eDlg.m_xvRdNav == 1 || eDlg.m_xvRdNav == 2) {
		f.WriteString("    <frameset rows=\"65%,*\">\n");
		f.WriteString("      <frame src=\"" + eDlg.m_pathNetwork + "\" name=\"network\">\n  ");
	}
	CString textLink = eDlg.m_pathTextSingle;
	if (m_exportOption.textOption == 1) {
		textLink = "text/" + eDlg.m_xvEdPrfTextEverynode + keystr + ".html";
	}
	f.WriteString("    <frame src=\"" + textLink + "\" name=\"text\">\n");
	if (eDlg.m_xvRdNav == 1 || eDlg.m_xvRdNav == 2) {
		f.WriteString("    </frameset>\n");
	}
	if (eDlg.m_xvRdNav != 1) {
		f.WriteString("  </frameset>\n");
	}
	f.WriteString("</html>\n");
	f.Close();
	
	CStdioFile olf;
	if (eDlg.m_xvRdNav != 1) {
		CString olName = outdir + "\\" + eDlg.m_pathOutline;
		if (!olf.Open(olName, CFile::typeText | CFile::modeCreate | CFile::modeWrite, &e)) {
			MessageBox(olName + " : 作成に失敗しました");
			return;
		}
		writeHtmlHeader(olf);
		olf.WriteString("<style type=\"text/css\">\n");
		olf.WriteString(" h1 {font-size: 100%; background: #F3F3F3; padding: 5px 5px 5px;}\n");
		olf.WriteString(" li {font-size: 95%; padding: 0px;}\n");
		olf.WriteString("</style>\n");
		olf.WriteString("</head>\n");
		olf.WriteString("<body>\n<h1>");
		olf.WriteString("<a href=");
		if (m_exportOption.textOption == 0) {
			olf.WriteString("\"" + m_exportOption.pathTextSingle + "#" + keystr);
		} else {
			olf.WriteString("\"text/" + m_exportOption.prfTextEverynode + keystr + ".html");
		}
		olf.WriteString("\" target=text>");
		CString rootStr = Utilities::removeCR(GetDocument()->getKeyNodeLabel(tree().GetItemData(root)));
		olf.WriteString(rootStr);
		olf.WriteString("</a></h3>\n");
		olf.WriteString("<ul>\n");
	}
	CStdioFile tf;
	if (m_exportOption.textOption == 0) {
		CString arName = outdir + "\\" + m_exportOption.pathTextSingle;
		if (!tf.Open(arName, CFile::typeText | CFile::modeCreate | CFile::modeWrite, &e)) {
			MessageBox(arName + " : 作成に失敗しました");
			olf.Close();
			return;
		}
		writeHtmlHeader(tf);
		writeTextStyle(tf);
		tf.WriteString("</head>\n<body>\n");
		GetDocument()->writeTextHtml(tree().GetItemData(root), &tf);
	} else {
		CStdioFile rootTf;
		CString arName = textDir + "\\" + m_exportOption.prfTextEverynode + keystr + ".html";
		if (!rootTf.Open(arName, CFile::typeText | CFile::modeCreate | CFile::modeWrite, &e)) {
			MessageBox(arName + " : 作成に失敗しました");
			olf.Close();
			return;
		}
		writeHtmlHeader(rootTf);
		writeTextStyle(rootTf, false);
		rootTf.WriteString("</head>\n<body>\n");
		GetDocument()->writeTextHtml(tree().GetItemData(root), &rootTf, true, m_exportOption.prfTextEverynode);
		rootTf.WriteString("</body>\n</html>\n");
		rootTf.Close();
	}
	/////////////////// output SubTree
	if (tree().ItemHasChildren(root)) {
		HTREEITEM child = tree().GetNextItem(root, TVGN_CHILD);
		htmlOutTree(root, child, &olf, &tf);
	}
	
	if (eDlg.m_xvRdNav != 1) {
		olf.WriteString("</body>\n</html>\n");
		olf.Close();
		if (m_exportOption.textOption == 0) {
			tf.WriteString("</body>\n</html>\n");
			tf.Close();
		}
	}
	
	///////////////////// create network.html
	if (eDlg.m_xvRdNav > 0) {
		CStdioFile nf;
		CString nName = outdir + "\\" + eDlg.m_pathNetwork;
		if (!nf.Open(nName, CFile::typeText | CFile::modeCreate | CFile::modeWrite, &e)) {
			MessageBox(nName + " : 作成に失敗しました");
			return;
		}
		writeHtmlHeader(nf);
		nf.WriteString("</head>\n");
		nf.WriteString("<body>\n");
		
		CString sWidth; sWidth.Format("width=\"%d\"", GetDocument()->getMaxPt().x);
		CString sHeight; sHeight.Format("height=\"%d\"", GetDocument()->getMaxPt().y);
		CString sWidthMgn; sWidthMgn.Format("width=\"%d\"", GetDocument()->getMaxPt().x + 50);
		CString sHeightMgn; sHeightMgn.Format("height=\"%d\"", GetDocument()->getMaxPt().y + 50);
		if (eDlg.m_xvRdImg == 0) {
			CString svgPath = outdir + "\\" + eDlg.m_pathSvg;
			if (m_exportOption.textOption == 0) {
				GetDocument()->exportSVG(false, svgPath, true, m_exportOption.pathTextSingle);
			} else {
				GetDocument()->exportSVG(false, svgPath, true, m_exportOption.prfTextEverynode, false);
			}
			nf.WriteString("<object type=\"image/svg+xml\" data=\""
				+ eDlg.m_pathSvg +
				"\" classid=\"clsid:377B5106-3B4E-4A2D-8520-8767590CAC86 "
				+ sWidth + " " + sHeight + " />\n");
			nf.WriteString("<embed src=\""
				+ eDlg.m_pathSvg + "\"" +
				"type=\"image/svg+xml\" "
				+ sWidthMgn + " " + sHeightMgn + " />\n");
		} else {
			GetDocument()->saveCurrentImage(outdir + "\\" + eDlg.m_pathPng);
			nf.WriteString("<img src=\"" + eDlg.m_pathPng + "\" border=\"0\" usemap=\"#nodes\" />\n");
			nf.WriteString("<map name=\"nodes\">\n");
			if (m_exportOption.textOption == 0) {
				GetDocument()->writeClickableMap(nf, m_exportOption.pathTextSingle);
			} else {
				GetDocument()->writeClickableMap(nf, m_exportOption.prfTextEverynode, false);
			}
			nf.WriteString("</map>\n");
		}
		nf.WriteString("</body>\n</html>\n");
		nf.Close();
	}
	
	if (MessageBox("生成したHTMLファイルを開きますか?", "HTMLの閲覧", MB_YESNO) != IDYES) return;
	ShellExecute(m_hWnd, "open", indexFilePath, NULL, NULL, SW_SHOW);
}

void OutlineView::htmlOutTree(HTREEITEM hRoot, HTREEITEM hItem, CStdioFile *foutline, CStdioFile* ftext)
{
	CString keystr;
	keystr.Format("%d", tree().GetItemData(hItem));
	// アウトライン書き込み
	if (m_exportOption.navOption != 1) {
		foutline->WriteString("<li>");
		CString itemStr = Utilities::removeCR(GetDocument()->getKeyNodeLabel(tree().GetItemData(hItem)));
		foutline->WriteString("<a href=");
		if (m_exportOption.textOption == 0) {
			foutline->WriteString("\"" + m_exportOption.pathTextSingle + "#");
			foutline->WriteString(keystr);
		} else {
			foutline->WriteString("\"text/" + m_exportOption.prfTextEverynode + keystr + ".html");
		}
		foutline->WriteString("\" target=text>");
		// 見出し書き込み
		foutline->WriteString(itemStr);
		foutline->WriteString("</a>\n");
	}
	
	// Text出力
	DWORD key = tree().GetItemData(hItem);
	if (m_exportOption.textOption == 0) {
		GetDocument()->writeTextHtml(key, ftext);
	} else {
		CStdioFile tf;
		CFileException e;
		CString fName = m_exportOption.htmlOutDir + "\\text\\" 
			+ m_exportOption.prfTextEverynode + keystr + ".html";
		if (!tf.Open(fName, CFile::typeText | CFile::modeCreate | CFile::modeWrite, &e)) {
			MessageBox(fName + " : 作成に失敗しました");
			return;
		}
		writeHtmlHeader(tf);
		writeTextStyle(tf, false);
		tf.WriteString("</head>\n<body>\n");
		GetDocument()->writeTextHtml(key, &tf, true, m_exportOption.prfTextEverynode);
		tf.WriteString("</body>\n</html>\n");
		tf.Close();
	}
	
	bool nested = m_exportOption.htmlOutOption == 0 || 
		m_exportOption.htmlOutOption == 1 && GetDocument()->isShowSubBranch();
	if (tree().ItemHasChildren(hItem) && nested) {
		if (m_exportOption.navOption != 1) {
			foutline->WriteString("<ul>\n");
		}
		HTREEITEM hchildItem = tree().GetNextItem(hItem, TVGN_CHILD);
		htmlOutTree(hRoot, hchildItem, foutline, ftext);
	} else {
		HTREEITEM hnextItem = tree().GetNextItem(hItem, TVGN_NEXT);
		if (hnextItem == NULL) {    // 次に兄弟がいない
			HTREEITEM hi = hItem;
			HTREEITEM hParent = hItem;
			while (tree().GetParentItem(hParent) != hRoot) {
				hParent = tree().GetParentItem(hi);
				HTREEITEM hnextParent;
				if (m_exportOption.navOption != 1) {
					foutline->WriteString("\n</ul>\n");
				}
				if ((hnextParent = tree().GetNextItem(hParent, TVGN_NEXT)) != NULL) {
					htmlOutTree(hRoot, hnextParent, foutline, ftext);
					return;
				}
				hi = hParent;
			}                                   // 兄弟のいる親まで戻る
		} else {
			htmlOutTree(hRoot, hnextItem, foutline, ftext);
		}                                       // 兄弟に移動
	}
}

void OutlineView::writeHtmlHeader(CStdioFile &f)
{
	f.WriteString("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"); 
	f.WriteString("<html>\n<head>\n");
	f.WriteString("<meta http-equiv=\"content-Type\" content=\"text/html; charset=Shift_JIS\" />\n");
}

void OutlineView::writeTextStyle(CStdioFile &f, bool single)
{
	f.WriteString("<style type=\"text/css\">\n");
	if (single) {
		f.WriteString(" h1 {font-size: 120%; border-bottom:2pt solid #9999FF; border-left:7pt solid #9999FF; padding: 5px 5px 5px;}\n");
	} else {
		f.WriteString(" h1 {font-size: 120%; background: #F3F3F3; padding: 5px 5px 5px;}\n");
	}
	f.WriteString(" h2 {font-size: 110%; border-bottom:2pt solid #9999FF; border-left:3pt solid #9999FF; padding: 5px 5px 5px;}\n");
	f.WriteString(" h3 {font-size: 100%; border-bottom:1pt solid #9999FF; padding: 5px 5px 5px;}\n");
	f.WriteString(" h4 {font-size: 100%; border-bottom: 1px dashed #999999; padding: 5px 5px 5px;}\n");
	f.WriteString(" li {font-size: 80%; padding: 0px;}\n");
	f.WriteString("</style>\n");
}

void OutlineView::textOutTree(HTREEITEM hItem, CStdioFile *f, int tab, BOOL bOutText)
{
	if (tree().GetPrevSiblingItem(hItem) == tree().GetSelectedItem() && m_opTreeOut != 0) {
		return;
	}
	
	f->WriteString(".");
	for (int i = 0; i < tab; i++) {
		f->WriteString(".");
	}
	f->WriteString(Utilities::removeCR(tree().GetItemText(hItem)) + "\n");
	
	if (bOutText) {
		f->WriteString(GetDocument()->procCR(GetDocument()->getKeyNodeText(tree().GetItemData(hItem))));
		f->WriteString("\n");
	}
	
	if (tree().ItemHasChildren(hItem) && m_opTreeOut != 2) {           // 子どもに移動
		HTREEITEM hchildItem = tree().GetNextItem(hItem, TVGN_CHILD);
		textOutTree(hchildItem, f, ++tab, bOutText);
	} else {
		HTREEITEM hnextItem = tree().GetNextItem(hItem, TVGN_NEXT);	
		if (hnextItem == NULL) {    // 次に兄弟がいない
			HTREEITEM hi = hItem;
			HTREEITEM hParent = hItem;
			while (hParent != tree().GetRootItem()) {
				hParent = tree().GetParentItem(hi);
				HTREEITEM hnextParent;
				--tab;
				if ((hnextParent = tree().GetNextItem(hParent, TVGN_NEXT)) != NULL) {
					textOutTree(hnextParent, f, tab, bOutText);
					return;
				}
				hi = hParent;
			}                                   // 兄弟のいる親まで戻る
		} else {
			textOutTree(hnextItem, f, tab, bOutText);
		}                                       // 兄弟に移動
	}
}

bool OutlineView::ImportText(const CString &inPath, nVec &addNodes, const char levelChar)
{
	CStdioFile f;
	CFileStatus status;
	CFileException e;
	
	
	if (!f.Open(inPath, CFile::typeText | CFile::modeRead, &e)) {
		return false;
	}

	CString line;
	vector<CString> lines;
	while (f.ReadString(line) != NULL) {
		lines.push_back(line);
	}
	return levelToNode(lines, addNodes, levelChar);
}

bool OutlineView::levelToNode(const vector<CString> &lines, nVec &addNodes, const char levelChar)
{
	COnProcDlg prcdlg;
	prcdlg.Create(IDD_ONPROC);
	prcdlg.m_ProcName.SetWindowText("インポート中");
	prcdlg.m_ProgProc.SetStep(1);              // プログレスバーの初期設定
	prcdlg.m_ProgProc.SetRange(0, lines.size() - 1);
	
	CSize mvSz(30, 30);
	CString label;
	CString text;
	int curLevel = 0;
	bool nodeCreated = false;
	for (unsigned int i = 0; i < lines.size(); i++) {
		int level = countLineIndentLevel(lines[i], levelChar);
		if (level > curLevel && level - curLevel > 1 && nodeCreated) {
			CString mes; mes.Format("%d行目 : %s", i + 1, lines[i]);
			MessageBox(mes, "インポートエラー:階層が正しくありません", MB_ICONSTOP);
			return false;
		}
		if (level == 0) {
			if (text == "" && lines[i] == "") {
				continue;
			}
			if (nodeCreated) {
				text.Append(lines[i] + _T("\r\n"));
				continue;
			}
		} else {
			if (label == "") {
				CString line = lines[i];
				label = line.TrimLeft(_T("."));
				continue;
			}
			nodeCreated = true;
			iNode node(label);
			node.setText(text);
			node.setKey(GetDocument()->getUniqKey());
			node.setParent(tree().GetItemData(curItem()));
			node.moveBound(mvSz);
			node.setLevel(curLevel);
			addNodes.push_back(node);
			
			text = "";
			CString line = lines[i];
			label = line.TrimLeft(_T("\t."));
			curLevel = level;
			if (label == "") {
				label = "<名称未設定>";
			}
			mvSz.cx += 20; mvSz.cy += 30;
			prcdlg.m_ProgProc.StepIt();  // プログレスバーを更新
		}
	}
	
	if (label != "") {
		iNode node;
		node.setName(label);
		node.setText(text);
		node.setKey(GetDocument()->getUniqKey());
		node.setParent(tree().GetItemData(curItem()));
		node.moveBound(mvSz);
		node.setLevel(curLevel);
		addNodes.push_back(node);
	}
	
	return true;
}

int OutlineView::countLineIndentLevel(const CString& line, const char levelChar) const
{
	int i = 0;
	for (; i < line.GetLength(); i++) {
		if (line.GetAt(i) != levelChar) {
			break;
		}
	}
	return i;
}

bool OutlineView::ImportXML(const CString &inPath)
{
	CWaitCursor wc;
	SelImportDlg dlg;
	dlg.m_importMode = 0;
	if (dlg.DoModal() != IDOK) return true;
	bool rep = (dlg.m_importMode == 0);
	return GetDocument()->loadXML(inPath, rep);
}

void OutlineView::OnEditFind() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	m_pSrchDlg->ShowWindow(SW_SHOWNORMAL);
	m_pSrchDlg->SetFocus();
}

void OutlineView::OnUpdateEditFind(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	
}

LRESULT OutlineView::OnHideSrchDlg(UINT wParam, LONG lParam)
{
	m_pSrchDlg->ShowWindow(SW_HIDE);
	return 0;
}

LRESULT OutlineView::OnSearchNode(UINT wParam, LONG lParam)
{
	HTREEITEM sItem = findKeyItem((DWORD)wParam, tree().GetRootItem());
	tree().SelectItem(sItem);
	return 0;
}

void OutlineView::OnDestroy() 
{
	CTreeView::OnDestroy();
	
	// TODO: この位置にメッセージ ハンドラ用のコードを追加してください
	m_pSrchDlg->DestroyWindow();
}

LRESULT OutlineView::OnListUpNodes(UINT wParam, LONG lParam)
{
	// リストアップ用のデータをnodeSrchDlgに受け渡す
	if (m_pSrchDlg->m_labels.size() > 0) return 0;
	GetDocument()->listUpNodes(m_pSrchDlg->m_srchString, m_pSrchDlg->m_labels,
		                       m_pSrchDlg->m_bLabel, m_pSrchDlg->m_bText, m_pSrchDlg->m_bLinks,
							   m_pSrchDlg->m_bUpper);
	m_pSrchDlg->displayResult();
	return 0;
}

void OutlineView::OnEditReplace() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	
}

void OutlineView::hideModeless()
{
	m_pSrchDlg->ShowWindow(SW_HIDE);
}

void OutlineView::clearUndo()
{
	m_hItemMoved = NULL;
	m_hParentPreMove = NULL;
	m_hSiblingPreMove = NULL;
}

void OutlineView::OnSetFoldup() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	setFoldUpDlg dlg;
	dlg.m_level = 1;
	if (dlg.DoModal() != IDOK) return;
	foldUpTree(tree().GetRootItem(), 0, dlg.m_level-1);
	tree().SelectItem(tree().GetRootItem());
}

void OutlineView::OnUpdateSetFoldup(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	
}

void OutlineView::foldUpTree(HTREEITEM hItem, int curLevel, int levelSet)
{
	if (curLevel <= levelSet) {
		if (tree().ItemHasChildren(hItem)) {
			tree().Expand(hItem, TVE_EXPAND);
		}
	} else {
		if (tree().ItemHasChildren(hItem)) {
			tree().Expand(hItem, TVE_COLLAPSE);
		}
	}
	if (tree().ItemHasChildren(hItem)) {
		HTREEITEM hChild = tree().GetNextItem(hItem, TVGN_CHILD);
		foldUpTree(hChild, ++curLevel, levelSet);
	} else {
		HTREEITEM hNext = tree().GetNextItem(hItem, TVGN_NEXT);
		if (hNext == NULL) {
			HTREEITEM hi = hItem;
			HTREEITEM hParent = hItem;
			while (hParent != tree().GetRootItem()) {
				hParent = tree().GetParentItem(hi);
				HTREEITEM hnextParent;
				--curLevel;
				if ((hnextParent = tree().GetNextItem(hParent, TVGN_NEXT)) != NULL) {
					foldUpTree(hnextParent, curLevel, levelSet);
					return;
				}
				hi = hParent;
			}
		} else {
			foldUpTree(hNext, curLevel, levelSet);
		}
	}
}


void OutlineView::doColorSetting()
{
	COLORREF colorBG = AfxGetApp()->GetProfileInt(REGS_FRAME, "Outline bgColor", RGB(255, 255, 255));
	COLORREF colorFor = AfxGetApp()->GetProfileInt(REGS_FRAME, "Outline forColor", RGB(0, 0, 0));
	COLORREF colorIM = AfxGetApp()->GetProfileInt(REGS_FRAME, "InsertMark Color", RGB(0, 0, 0));
	TreeView_SetBkColor(m_hWnd, colorBG);
	TreeView_SetTextColor(m_hWnd, colorFor);
	TreeView_SetInsertMarkColor(m_hWnd, colorIM);
}

void OutlineView::OnAddChild2()
{
	// TODO : ここにコマンド ハンドラ コードを追加します。
	CInpcnDlg dlg;
	dlg.m_iniPt.x = -1;
	dlg.m_iniPt.y = -1;
	if (dlg.DoModal() != IDOK) return;
	if (dlg.m_strcn == "") return;
	GetDocument()->disableUndo();
	clearUndo();
	HTREEITEM newItem = tree().InsertItem(dlg.m_strcn, 0, 0, curItem());
	m_HNew = newItem;
	
	iEditDoc* pDoc = GetDocument();
	label l;
	l.name = dlg.m_strcn;
	l.key = pDoc->getUniqKey();
	tree().SetItemData(m_HNew, l.key);
	l.parent = tree().GetItemData(tree().GetParentItem(m_HNew));
		
	DWORD keyInherit = tree().GetItemData(tree().GetSelectedItem());
	
	bool binherit = false;
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	if (pApp->m_rgsNode.bInheritParent) {
		binherit = true;
	}
	pDoc->addNode(l, keyInherit, binherit);
	tree().Expand(curItem(), TVE_EXPAND);
	tree().SelectItem(newItem);
}

void OutlineView::OnUpdateAddChild2(CCmdUI *pCmdUI)
{
	// TODO : ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->Enable(!tree().ItemHasChildren(tree().GetSelectedItem()));
}

void OutlineView::OnShowSelectedBranch() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	KeySet ks;
	ks.insert(tree().GetItemData(tree().GetSelectedItem()));
	treeview_for_each(tree(), copyKeys(ks), tree().GetChildItem(curItem()));
	GetDocument()->setVisibleNodes(ks);
	GetDocument()->setShowBranch(tree().GetItemData(curItem()));
	int branchMode = getBranchMode();
	if (branchMode != 0) {
		resetShowBranch();
	}
	tree().SetItemImage(curItem(), 2, 2);
	tree().Expand(curItem(), TVE_EXPAND);
	m_hItemShowRoot = curItem();
	tree().SelectItem(tree().GetChildItem(curItem()));
}

void OutlineView::OnUpdateShowSelectedBranch(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(tree().ItemHasChildren(curItem()));
}

BOOL OutlineView::isPosterityOF(HTREEITEM hRoot, HTREEITEM hChild) const
{
	HTREEITEM hCurItem = hChild;
	HTREEITEM hParent = hChild;
	while (hParent != tree().GetRootItem()) {
		hParent = tree().GetParentItem(hCurItem);
		if (hParent == hRoot) {
			return TRUE;
		}
		hCurItem = hParent;
	}
	return FALSE;
}

int OutlineView::getBranchMode() const
{
	int index, selectedIndex;
	tree().GetItemImage(m_hItemShowRoot, index, selectedIndex);
	// 戻り値：0:通常、1:下の階層のみ、2:下全部
	return selectedIndex;
}

void OutlineView::resetShowBranch()
{
	tree().SetItemImage(m_hItemShowRoot, 0, 0);
}

void OutlineView::OnShowSelectedChildren() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	KeySet ks;
	ks.insert(tree().GetItemData(tree().GetSelectedItem()));
	treeview_for_each2(tree(), copyKeys(ks), tree().GetChildItem(curItem()));
	GetDocument()->setVisibleNodes(ks);
	GetDocument()->setShowBranch(tree().GetItemData(curItem()));
	int branchMode = getBranchMode();
	if (branchMode != 0) {
		resetShowBranch();
	}
	tree().SetItemImage(curItem(), 1, 1);
	tree().Expand(curItem(), TVE_EXPAND);
	m_hItemShowRoot = curItem();
	tree().SelectItem(tree().GetChildItem(curItem()));
}

void OutlineView::OnUpdateShowSelectedChildren(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(tree().ItemHasChildren(curItem()));
}

void OutlineView::setNodeLevels(HTREEITEM hItem, int curLevel)
{
	iEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->setNodeLevel(tree().GetItemData(hItem), curLevel);
	if (tree().ItemHasChildren(hItem)) {
		HTREEITEM hChild = tree().GetNextItem(hItem, TVGN_CHILD);
		setNodeLevels(hChild, ++curLevel);
	} else {
		HTREEITEM hNext = tree().GetNextItem(hItem, TVGN_NEXT);
		if (hNext == NULL) {
			HTREEITEM hi = hItem;
			HTREEITEM hParent = hItem;
			while (hParent != tree().GetRootItem()) {
				hParent = tree().GetParentItem(hi);
				HTREEITEM hnextParent;
				--curLevel;
				if ((hnextParent = tree().GetNextItem(hParent, TVGN_NEXT)) != NULL) {
					setNodeLevels(hnextParent, curLevel);
					return;
				}
				hi = hParent;
			}
		} else {
			setNodeLevels(hNext, curLevel);
		}
	}
}

void OutlineView::setAllNodeLevels()
{
	setNodeLevels(tree().GetRootItem(), 0);
}

void OutlineView::setSubNodeLevels()
{
	setNodeLevels(tree().GetSelectedItem(), 0);
}

void OutlineView::OnDropFirstOrder() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	HTREEITEM hNew = tree().InsertItem(tree().GetItemText(m_hitemDrag), 0, 0, m_hitemDrop, TVI_FIRST);
	tree().SetItemData(hNew, tree().GetItemData(m_hitemDrag));
	int nImage; tree().GetItemImage(m_hitemDrag, nImage, nImage);
	tree().SetItemImage(hNew, nImage, nImage);
	
	GetDocument()->setKeyNodeParent(tree().GetItemData(m_hitemDrag), tree().GetItemData(m_hitemDrop));
	if (tree().ItemHasChildren(m_hitemDrag)) {
		copySubNodes(tree().GetChildItem(m_hitemDrag), hNew);
	}
	GetDocument()->disableUndo();
	m_hItemMoved = hNew; // Undo Info
	tree().SelectItem(hNew);
	tree().DeleteItem(m_hitemDrag);
	tree().SelectItem(hNew);
	
	m_bItemDragging = false;
	tree().SetInsertMark(NULL);
	tree().SelectDropTarget(NULL);
}

void OutlineView::OnUpdateDropFirstOrder(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(m_bItemDragging == true);
}

void OutlineView::OnDropLevelUp() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	HTREEITEM hParent = tree().GetParentItem(m_hitemDrop);
	HTREEITEM hNew = tree().InsertItem(tree().GetItemText(m_hitemDrag), 0, 0, hParent, m_hitemDrop);
	tree().SetItemData(hNew, tree().GetItemData(m_hitemDrag));
	
	int nImage; tree().GetItemImage(m_hitemDrag, nImage, nImage);
	tree().SetItemImage(hNew, nImage, nImage);
	
	GetDocument()->setKeyNodeParent(tree().GetItemData(m_hitemDrag), tree().GetItemData(hParent));
	if (tree().ItemHasChildren(m_hitemDrag)) {
		copySubNodes(tree().GetChildItem(m_hitemDrag), hNew);
	}
	GetDocument()->disableUndo();
	m_hItemMoved = hNew; // Undo Info
	tree().SelectItem(hNew);
	tree().DeleteItem(m_hitemDrag);
	tree().SelectItem(hNew);
	
	m_bItemDragging = false;
	tree().SetInsertMark(NULL);
	tree().SelectDropTarget(NULL);
}

void OutlineView::OnUpdateDropLevelUp(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(m_bItemDragging == true && m_hitemDrop != tree().GetRootItem());
}

void OutlineView::OnCopyTreeToClipboard() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	setSubNodeLevels();
	CString strData;
	catTreeLabel(tree().GetSelectedItem(), strData);
	
	// クリップボードにコピー
	if (OpenClipboard()) {
		EmptyClipboard();
		HGLOBAL hClipboardData;
		hClipboardData = GlobalAlloc(GMEM_DDESHARE, strData.GetLength()+1);

		char * pchData;
		pchData = (char*)GlobalLock(hClipboardData);
		strcpy_s(pchData, strData.GetLength()+1, strData.GetBuffer());
		GlobalUnlock(hClipboardData);
		SetClipboardData(CF_TEXT,hClipboardData);
		CloseClipboard();
	//	GlobalFree(hClipboardData);
	}
	
}

void OutlineView::catTreeLabel(HTREEITEM hItem, CString &text)
{
	if (tree().GetPrevSiblingItem(hItem) == tree().GetSelectedItem()) {
		return;
	}
	DWORD key = tree().GetItemData(hItem);
	int level = GetDocument()->getKeyNodeLevelNumber(key);
	if (level != -1) {
		for (int i = 1; i <= level; i++) {
			text += "\t";
		}
		CString label = GetDocument()->getKeyNodeLabel(key);
		text += Utilities::removeCR(label);
		text += "\r\n";
	}
	if (tree().ItemHasChildren(hItem)) {
		HTREEITEM hchildItem = tree().GetNextItem(hItem, TVGN_CHILD);
		catTreeLabel(hchildItem, text);
	} else {
		HTREEITEM hnextItem = tree().GetNextItem(hItem, TVGN_NEXT);
		if (hnextItem == NULL) {
			HTREEITEM hi = hItem;
			HTREEITEM hParent = hItem;
			while (hParent != tree().GetSelectedItem()) {
				hParent = tree().GetParentItem(hi);
				HTREEITEM hnextParent = tree().GetNextItem(hParent, TVGN_NEXT);
				if (hnextParent != NULL) {
					catTreeLabel(hnextParent, text);
					return;
				}
				hi = hParent;
			}
		} else {
			catTreeLabel(hnextItem, text);
		}
	}
}

void OutlineView::OnUpdateCopyTreeToClipboard(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(tree().ItemHasChildren(tree().GetSelectedItem()));
}

void OutlineView::moveNodes(DWORD keyTarget, DWORD keyMove)
{
	HTREEITEM hTarget;
	if (keyTarget != -1) {
		hTarget = findKeyItem(keyTarget);
	} else {
		hTarget = this->m_hItemShowRoot;
	}
	HTREEITEM hMove = findKeyItem(keyMove);
	if (hTarget == NULL || hMove == NULL) return;
	if (IsChildNodeOf(hTarget, hMove)) return;
	if (tree().GetParentItem(hMove) == hTarget) return;
	HTREEITEM hNew = tree().InsertItem(tree().GetItemText(hMove), 0, 0, hTarget);
	tree().SetItemState(hNew, tree().GetItemState(hMove, TVIS_EXPANDED), TVIS_EXPANDED);
	tree().SetItemData(hNew, tree().GetItemData(hMove));
	int nImage; tree().GetItemImage(hMove, nImage, nImage);
	tree().SetItemImage(hNew, nImage, nImage);
	
	tree().Expand(hTarget, TVE_EXPAND);
	GetDocument()->setKeyNodeParent(keyMove, keyTarget);
	if (tree().ItemHasChildren(hMove)) {
		copySubNodes(tree().GetChildItem(hMove), hNew);
	}
	GetDocument()->disableUndo();
	tree().SelectItem(hNew);
	tree().DeleteItem(hMove);
	tree().SelectItem(hNew);
}

void OutlineView::OnCreateClone()
{
	// TODO: Add your command handler code here
	DWORD key = tree().GetItemData(tree().GetSelectedItem());
	CString label = GetDocument()->getKeyNodeLabel(key);
	DWORD newKey = GetDocument()->duplicateKeyNode(key);
	HTREEITEM hSelected = tree().GetSelectedItem();
	HTREEITEM hNew = tree().InsertItem(label, tree().GetParentItem(hSelected), hSelected);
	tree().SetItemData(hNew, newKey);
	int nImage; tree().GetItemImage(hSelected, nImage, nImage);
	tree().SetItemImage(hNew, nImage, nImage);
	
	tree().SetItemState(hNew, tree().GetItemState(hSelected, TVIS_EXPANDED), TVIS_EXPANDED);
	IdMap idm;
	idm[key] = newKey;
	if (tree().ItemHasChildren(hSelected)) {
		cloneTree(tree().GetChildItem(hSelected), hNew, idm);
	}
	GetDocument()->duplicateLinks(idm);
	// 指定配下のノードを全部見せるモードの場合、クローンした一連のノードとリンクをvisibleに
	if (GetDocument()->isShowSubBranch()) {
		KeySet ks;
		ks.insert(tree().GetItemData(m_hItemShowRoot));
		treeview_for_each(tree(), copyKeys(ks), tree().GetChildItem(m_hItemShowRoot));
		iEditDoc* pDoc = GetDocument();
		pDoc->setVisibleNodes(ks);
		pDoc->setShowBranch(tree().GetItemData(m_hItemShowRoot));
	}
	// 既存のノードと重ならないようにずらす
	tree().SelectItem(hNew);
}

void OutlineView::OnUpdateCreateClone(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(tree().GetSelectedItem() != tree().GetRootItem());
}

void OutlineView::cloneTree(const HTREEITEM& curItem, HTREEITEM targetParent, IdMap& idm)
{
	HTREEITEM hItem, item;
	item = curItem;
	while (item != NULL) {
		hItem = item;
		DWORD key = tree().GetItemData(hItem);
		CString label = GetDocument()->getKeyNodeLabel(key);
		DWORD newKey = GetDocument()->duplicateKeyNode(key);
		GetDocument()->setKeyNodeParent(newKey, tree().GetItemData(targetParent));
		HTREEITEM hNew = tree().InsertItem(label, 0, 0, targetParent);
		tree().SetItemData(hNew, newKey);
		int nImage; tree().GetItemImage(hItem, nImage, nImage);
		tree().SetItemImage(hNew, nImage, nImage);

		tree().SetItemState(hNew, tree().GetItemState(hItem, TVIS_EXPANDED), TVIS_EXPANDED);
		idm[key] = newKey;
		if (tree().ItemHasChildren(hItem)) {
			cloneTree(tree().GetChildItem(hItem), hNew, idm);
		}
		item = tree().GetNextSiblingItem(item);
	}
}

void OutlineView::OnResetShowSubbranch()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	tree().SelectItem(m_hItemShowRoot);
	resetShowBranch();
	GetDocument()->resetShowBranch();
}

void OutlineView::OnUpdateResetShowSubbranch(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	int mode = getBranchMode();
	pCmdUI->Enable(mode == 1 || mode == 2);
}

void OutlineView::OnTreeImageChcek()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	tree().SetItemImage(curItem(), OutlineView::check, OutlineView::check);
	GetDocument()->setSelectedNodeTreeIconId(OutlineView::check);
}

void OutlineView::OnUpdateTreeImageChcek(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	int nImage;
	tree().GetItemImage(curItem(), nImage, nImage);
	pCmdUI->Enable(!GetDocument()->isOldBinary() &&
		tree().GetSelectedItem() != m_hItemShowRoot &&
		nImage != OutlineView::check &&
		curItem() != tree().GetRootItem());
}

void OutlineView::OnTreeImageBlue()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	tree().SetItemImage(curItem(), OutlineView::blue, OutlineView::blue);
	GetDocument()->setSelectedNodeTreeIconId(OutlineView::blue);
}

void OutlineView::OnUpdateTreeImageBlue(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	int nImage;
	tree().GetItemImage(curItem(), nImage, nImage);
	pCmdUI->Enable(!GetDocument()->isOldBinary() &&
		tree().GetSelectedItem() != m_hItemShowRoot &&
		nImage != OutlineView::blue &&
		curItem() != tree().GetRootItem());
}

void OutlineView::OnTreeImageRed()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	tree().SetItemImage(curItem(), OutlineView::red, OutlineView::red);
	GetDocument()->setSelectedNodeTreeIconId(OutlineView::red);
}

void OutlineView::OnUpdateTreeImageRed(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	int nImage;
	tree().GetItemImage(curItem(), nImage, nImage);
	pCmdUI->Enable(!GetDocument()->isOldBinary() &&
		tree().GetSelectedItem() != m_hItemShowRoot &&
		nImage != OutlineView::red &&
		curItem() != tree().GetRootItem());
}

void OutlineView::OnTreeImageYealow()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	tree().SetItemImage(curItem(), OutlineView::yellow, OutlineView::yellow);
	GetDocument()->setSelectedNodeTreeIconId(OutlineView::yellow);
}

void OutlineView::OnUpdateTreeImageYealow(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	int nImage;
	tree().GetItemImage(curItem(), nImage, nImage);
	pCmdUI->Enable(!GetDocument()->isOldBinary() &&
		tree().GetSelectedItem() != m_hItemShowRoot &&
		nImage != OutlineView::yellow &&
		curItem() != tree().GetRootItem());
}

void OutlineView::OnTreeImageCancel()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	tree().SetItemImage(curItem(), OutlineView::cancel, OutlineView::cancel);
	GetDocument()->setSelectedNodeTreeIconId(OutlineView::cancel);
}

void OutlineView::OnUpdateTreeImageCancel(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	int nImage;
	tree().GetItemImage(curItem(), nImage, nImage);
	pCmdUI->Enable(!GetDocument()->isOldBinary() &&
		tree().GetSelectedItem() != m_hItemShowRoot &&
		nImage != OutlineView::cancel &&
		curItem() != tree().GetRootItem());
}

void OutlineView::OnTreeImageQuestion()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	tree().SetItemImage(curItem(), OutlineView::question, OutlineView::question);
	GetDocument()->setSelectedNodeTreeIconId(OutlineView::question);
}

void OutlineView::OnUpdateTreeImageQuestion(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	int nImage;
	tree().GetItemImage(curItem(), nImage, nImage);
	pCmdUI->Enable(!GetDocument()->isOldBinary() &&
		tree().GetSelectedItem() != m_hItemShowRoot &&
		nImage != OutlineView::question &&
		curItem() != tree().GetRootItem());
}

void OutlineView::OnTreeImageWarning()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	tree().SetItemImage(curItem(), OutlineView::warning, OutlineView::warning);
	GetDocument()->setSelectedNodeTreeIconId(OutlineView::warning);
}

void OutlineView::OnUpdateTreeImageWarning(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	int nImage;
	tree().GetItemImage(curItem(), nImage, nImage);
	pCmdUI->Enable(!GetDocument()->isOldBinary() &&
		tree().GetSelectedItem() != m_hItemShowRoot &&
		nImage != OutlineView::warning &&
		curItem() != tree().GetRootItem());
}

void OutlineView::OnTreeImageFace()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	tree().SetItemImage(curItem(), OutlineView::face, OutlineView::face);
	GetDocument()->setSelectedNodeTreeIconId(OutlineView::face);
}

void OutlineView::OnUpdateTreeImageFace(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	int nImage;
	tree().GetItemImage(curItem(), nImage, nImage);
	pCmdUI->Enable(!GetDocument()->isOldBinary() &&
		tree().GetSelectedItem() != m_hItemShowRoot &&
		nImage != OutlineView::face &&
		curItem() != tree().GetRootItem());
}

void OutlineView::OnTreeImageIdea()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	tree().SetItemImage(curItem(), OutlineView::idea, OutlineView::idea);
	GetDocument()->setSelectedNodeTreeIconId(OutlineView::idea);
}

void OutlineView::OnUpdateTreeImageIdea(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	int nImage;
	tree().GetItemImage(curItem(), nImage, nImage);
	pCmdUI->Enable(!GetDocument()->isOldBinary() &&
		tree().GetSelectedItem() != m_hItemShowRoot &&
		nImage != OutlineView::idea &&
		curItem() != tree().GetRootItem());
}

void OutlineView::OnPasteTreeFromClipboard()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	CString ClipText;
	if (::IsClipboardFormatAvailable(CF_TEXT)) {
		if (!OpenClipboard()) {
			::showLastErrorMessage();
			return;
		}
		
		HGLOBAL hData;
		hData = ::GetClipboardData(CF_TEXT);
		if (hData == NULL) {
			::CloseClipboard();
			return;
		}
		char* pszData = (char*)GlobalLock(hData);
		ClipText = CString(pszData);
		
		GlobalUnlock(hData);
		::CloseClipboard();
	}
	
	int mode = getBranchMode();
	HTREEITEM hShowRoot;
	if (mode != 0) {
		hShowRoot = m_hItemShowRoot;
		resetShowBranch();
		GetDocument()->resetShowBranch();
	}
	
	ClipText += "\n";
	CToken tok(ClipText);
	tok.SetToken("\n");
	vector<CString> lines;
	while(tok.MoreTokens()) {
		CString s = tok.GetNextToken();
		lines.push_back(s);
	}
	nVec addNodes;
	if (levelToNode(lines, addNodes, '\t')) {
		treeAddBranch2(tree().GetItemData(curItem()), addNodes);
	}
	if (mode == 1) {
		tree().SelectItem(hShowRoot);
		OnShowSelectedChildren();
	} else if (mode == 2) {
		tree().SelectItem(hShowRoot);
		OnShowSelectedBranch();
	}
}

void OutlineView::OnUpdatePasteTreeFromClipboard(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->Enable(::IsClipboardFormatAvailable(CF_TEXT));
}

void OutlineView::OnExportToHtml()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	OutputHTML();
}

void OutlineView::OnUpdateExportToHtml(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
}

void OutlineView::OnExportToText()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	SelExportDlg dlg;
	dlg.m_bPrintText = TRUE;
	dlg.m_nTreeOp = m_exportOption.treeOption;
	dlg.m_bShowChekPrintText = true;
	if (dlg.DoModal() != IDOK) return;
	m_opTreeOut = dlg.m_nTreeOp;
	m_exportOption.treeOption = dlg.m_nTreeOp;
	
	CString outfile = GetDocument()->getTitleFromPath();	
	if (dlg.m_nTreeOp != 0) {
		CString label = Utilities::getSafeFileName(tree().GetItemText(tree().GetSelectedItem()));
		if (label != "") {
			outfile = label;
		}
	}
	
	char szFilters[] = "テキストファイル (*.txt)|*.txt";	
	CFileDialog fdlg(FALSE, "txt", outfile, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, this);
	if (fdlg.DoModal() != IDOK) return;
	CString outfileName = fdlg.GetPathName();
	
	
	CWaitCursor wc;
	CStdioFile f;
	CFileStatus status;
	CFileException e;
	
	if (!f.Open(outfileName, CFile::typeText | CFile::modeCreate | CFile::modeWrite, &e)) {
		return;
	}
	
	if (m_opTreeOut == 0) {
		textOutTree(tree().GetRootItem(), &f, 0, dlg.m_bPrintText);
	} else {
		f.WriteString(".");
		f.WriteString(Utilities::removeCR(tree().GetItemText(tree().GetSelectedItem())) + "\n");
		if (dlg.m_bPrintText) {
			f.WriteString(GetDocument()->procCR(GetDocument()->getKeyNodeText(tree().GetItemData(tree().GetSelectedItem()))));
			f.WriteString("\n");
		}
		if (tree().ItemHasChildren(tree().GetSelectedItem())) {
			textOutTree(tree().GetChildItem(tree().GetSelectedItem()), &f, 1, dlg.m_bPrintText);
		}
	}
	f.Close();
	ShellExecute(m_hWnd, "open", outfileName, NULL, NULL, SW_SHOW);

}

void OutlineView::OnUpdateExportToText(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
}

void OutlineView::OnExportToXml()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	SelExportDlg dlg;
	dlg.m_bPrintText = TRUE;
	dlg.m_nTreeOp = m_exportOption.treeOption;
	if (dlg.DoModal() != IDOK) return;
	m_opTreeOut = dlg.m_nTreeOp;
	m_exportOption.treeOption = dlg.m_nTreeOp;
	
	CString outfile = GetDocument()->getTitleFromPath();
	if (dlg.m_nTreeOp != 0) {
		CString label = Utilities::getSafeFileName(tree().GetItemText(tree().GetSelectedItem()));
		if (label != "") {
			outfile = label;
		}
	}
	char szFilters[] = "XMLファイル (*.xml)|*.xml";	
	CFileDialog fdlg(FALSE, "xml", outfile, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, this);
	if (fdlg.DoModal() != IDOK) return;
	CString outfileName = fdlg.GetPathName();
	
	if (GetDocument()->saveXML(outfileName)) {
		MessageBox("終了しました", "XMLへのエクスポート", MB_OK);
	}
}

void OutlineView::OnUpdateExportToXml(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
}
