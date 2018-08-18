// OutLineView.cpp : OutlineView クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "iEdit.h"

#include "iEditDoc.h"
#include "OutLineView.h"
#include "LinkPropertiesDlg.h"
#include "LinkForPathDlg.h"
#include "ProceedingDlg.h"
#include "ImportXmlDlg.h"
#include "ExportXmlDlg.h"
#include "ImportTextDlg.h"

#include "NodeSearchDlg.h"
#include "FoldingSettingsDlg.h"
#include "CreateNodeDlg.h"
#include "ExportHtmlDlg.h"
#include "ExportTextDlg.h"
#include "StringUtil.h"
#include "SystemConfiguration.h"
#include "HtmlWriter.h"
#include "FileUtil.h"
#include <shlobj.h>
#include <locale>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGS_FRAME _T("Frame Options")

// vector にラベルとキーを詰め込む関数オブジェクト
struct copy_key_label_vec : std::binary_function<CTreeCtrl&, HTREEITEM, int> {
	NodePropsVec& ls_;
	copy_key_label_vec(NodePropsVec& ls) : ls_(ls) {}
	int operator() (const CTreeCtrl& tree, HTREEITEM item) const {
		NodeProps l;
		l.key = tree.GetItemData(item);
		if (tree.GetParentItem(item) != NULL) {
			l.parent = tree.GetItemData(tree.GetParentItem(item));
		}
		else {
			l.parent = tree.GetItemData(tree.GetRootItem());
		}
		l.name = tree.GetItemText(item);
		l.state = tree.GetItemState(item, TVIS_EXPANDED | TVIS_SELECTED);
		ls_.push_back(l);
		return 0;
	}
};

// NodeKeySetにキーを詰め込む関数オブジェクト
struct copy_key_set : std::binary_function<CTreeCtrl&, HTREEITEM, int> {
	NodeKeySet& ks_;
	copy_key_set(NodeKeySet& ks) : ks_(ks) {}
	int operator() (const CTreeCtrl& tree, HTREEITEM item) const {
		DWORD key;
		key = tree.GetItemData(item);
		ks_.insert(key);
		return 0;
	}
};

// vector にキーを詰め込む関数オブジェクト
struct copy_key_vec : std::binary_function<CTreeCtrl&, HTREEITEM, int> {
	NodeKeyVec& svec_;
	copy_key_vec(NodeKeyVec& svec) : svec_(svec) {}
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
BinaryFunction treeview_for_each(CTreeCtrl& tree, BinaryFunction function, HTREEITEM item = 0)
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
BinaryFunction treeview_for_each2(CTreeCtrl& tree, BinaryFunction function, HTREEITEM item = 0)
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
HTREEITEM treeview_find(CTreeCtrl& tree, Predicate pred, HTREEITEM item = 0)
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
	if (uiMsg == BFFM_INITIALIZED) {
		if (lpData != NULL) {
			SendMessage(hWnd,
				BFFM_SETSELECTION,
				TRUE,
				lpData);
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
	ON_COMMAND(ID_EXPORT_TO_JSON, &OutlineView::OnExportToJson)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_TO_JSON, &OutlineView::OnUpdateExportToJson)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OutlineView クラスの構築/消滅

OutlineView::OutlineView()
{
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
	m_exportOption = { 0, 0, 0, 0, 0, _T("") };
	m_textExportOption = { 0, 0, 0, FALSE, FALSE };
}

OutlineView::~OutlineView()
{
}

BOOL OutlineView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS;
	return CTreeView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// OutlineView クラスの描画
void OutlineView::OnDraw(CDC* pDC)
{
	iEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

void OutlineView::OnInitialUpdate()
{
	CTreeView::OnInitialUpdate();

	if (GetDocument()->IsOldBinary() || GetDocument()->GetSerialVersion() <= 1) {
		ConstructTree();
	}
	else {
		ConstructTree2();
	}

	ApplyColorSetting(); // 背景色や文字色の設定

	// SubBranch表示状態のリストア
	if (GetDocument()->ShowSubBranch()) {
		int branchMode = GetDocument()->GetInitialBranchMode();
		m_hItemShowRoot = FindKeyItem(GetDocument()->GetBranchRootKey(), Tree().GetRootItem());
		if (m_hItemShowRoot == NULL) return;
		NodeKeySet ks;
		ks.insert(Tree().GetItemData(m_hItemShowRoot));
		if (branchMode == 1) {
			treeview_for_each2(Tree(), copy_key_set(ks), Tree().GetChildItem(m_hItemShowRoot));
			GetDocument()->SetVisibleNodes(ks);
			GetDocument()->SetShowBranch(Tree().GetItemData(m_hItemShowRoot));
			Tree().SetItemImage(m_hItemShowRoot, 1, 1);
		}
		else if (branchMode == 2) {
			treeview_for_each(Tree(), copy_key_set(ks), Tree().GetChildItem(m_hItemShowRoot));
			GetDocument()->SetVisibleNodes(ks);
			GetDocument()->SetShowBranch(Tree().GetItemData(m_hItemShowRoot));
			Tree().SetItemImage(m_hItemShowRoot, 2, 2);
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
}

void OutlineView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
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

	m_imgList.Create(16, 16, ILC_COLOR24 | ILC_MASK, 0, 1);
	CBitmap images;
	images.LoadBitmap(IDB_TREE);
	m_imgList.SetBkColor(CLR_NONE);
	m_imgList.Add(&images, RGB(255, 0, 255));
	Tree().SetImageList(&m_imgList, TVSIL_NORMAL);
	SetViewFont();
	m_pSrchDlg = new NodeSearchDlg;
	m_pSrchDlg->Create(_T(""), _T(""), SW_HIDE, CRect(0, 0, 0, 0), this, IDD_NODESRCH);
	m_hCsrCopy = AfxGetApp()->LoadCursor(IDC_POINTER_COPY);
	m_hCsrMove = AfxGetApp()->LoadCursor(IDC_POINTER_MOVE);
	return 0;
}

void OutlineView::ConstructTree()
{
	iEditDoc* pDoc = GetDocument();
	NodePropsVec ls;
	pDoc->CopyNodeLabels(ls);

	HTREEITEM hRoot = Tree().InsertItem(ls[0].name, 0, 0);
	Tree().SetItemData(hRoot, ls[0].key);
	Tree().SetItemState(hRoot, ls[0].state, TVIS_EXPANDED);
	DWORD preKey = 0;
	HTREEITEM hPreParent = Tree().GetRootItem();
	HTREEITEM hsel = hPreParent;
	for (unsigned int i = 1; i < ls.size(); i++) {
		if (ls[i].parent != preKey) {
			HTREEITEM hParent = FindKeyItem(ls[i].parent, Tree().GetRootItem());
			if (hParent != NULL) {
				hPreParent = hParent;
				preKey = ls[i].parent;
			}
		}

		HTREEITEM hnew = Tree().InsertItem(ls[i].name, 0, 0, hPreParent);
		Tree().SetItemData(hnew, ls[i].key);
		Tree().SetItemState(hnew, ls[i].state, TVIS_EXPANDED);
		if (ls[i].state & TVIS_SELECTED) hsel = hnew;
	}
	Tree().SelectItem(hsel);
	m_hItemShowRoot = Tree().GetRootItem();
}

void OutlineView::ConstructTree2()
{
	iEditDoc* pDoc = GetDocument();
	NodePropsVec ls;
	pDoc->CopyNodeLabels(ls);

	HTREEITEM hRoot = Tree().InsertItem(ls[0].name, 0, 0);
	Tree().SetItemData(hRoot, ls[0].key);
	Tree().SetItemState(hRoot, ls[0].state, TVIS_EXPANDED);
	//	tree().SetItemImage(hRoot, ls[0].treeIconId, ls[0].treeIconId);
		// Rootのイメージをシリアル化で再現するとネットワークビューに何も表示されないのでコメントアウト

	DWORD preKey = 0;
	HTREEITEM hParent = Tree().GetRootItem();
	HTREEITEM hSel = hParent;
	HTREEITEM hPrevNew = Tree().GetRootItem();
	int prevLevel = 0;

	HTREEITEM hNew = hRoot;
	for (unsigned int i = 1; i < ls.size(); i++) {
		if (prevLevel > ls[i].level) {
			unsigned int diff = prevLevel - ls[i].level;

			HTREEITEM hIt = hPrevNew;
			HTREEITEM hItParent = Tree().GetParentItem(hIt);
			for (unsigned int u = 0; u <= diff; u++) {
				hItParent = Tree().GetParentItem(hIt);
				hIt = hItParent;
			}
			if (ls[i].parent == Tree().GetItemData(hItParent)) {
				hNew = Tree().InsertItem(ls[i].name, 0, 0, hItParent);
				hParent = hItParent;
			}
			else {
				AfxMessageBox(_T("キーが一致しませんa"));
			}
			preKey = ls[i].parent;
		}
		else if (prevLevel < ls[i].level) {
			if (ls[i].parent == preKey) {
				hNew = Tree().InsertItem(ls[i].name, 0, 0, hPrevNew);
				hParent = hPrevNew;
			}
			else {
				AfxMessageBox(_T("キーが一致しませんb"));
			}
		}
		else {
			hNew = Tree().InsertItem(ls[i].name, 0, 0, hParent, hPrevNew);
		}
		Tree().SetItemData(hNew, ls[i].key);
		Tree().SetItemState(hNew, ls[i].state, TVIS_EXPANDED);
		Tree().SetItemImage(hNew, ls[i].treeIconId, ls[i].treeIconId);
		if (ls[i].state & TVIS_SELECTED) hSel = hNew;

		preKey = ls[i].key;
		hPrevNew = hNew;
		prevLevel = ls[i].level;
	}
	Tree().SelectItem(hSel);
	m_hItemShowRoot = Tree().GetRootItem();
}


void OutlineView::AddBranch(const DWORD rootKey)
{
	iEditDoc* pDoc = GetDocument();
	NodePropsVec ls;
	pDoc->CopyNodeLabels(ls);

	int loop = ls.size();
	ASSERT(loop >= 0);
	ProceedingDlg prcdlg;
	prcdlg.Create(IDD_ONPROC);
	prcdlg.m_ProcName.SetWindowText(_T("登録中"));
	prcdlg.m_ProgProc.SetStep(1); // プログレスバーの初期設定
	prcdlg.m_ProgProc.SetRange(0, loop);

	HTREEITEM hRoot = FindKeyItem(rootKey, Tree().GetRootItem());
	DWORD preKey = rootKey;
	HTREEITEM hPreParent = hRoot;
	HTREEITEM hsel = hPreParent;

	for (unsigned int i = 0; i < ls.size(); i++) {
		if (ls[i].parent != preKey) {
			HTREEITEM hParent = FindKeyItem(ls[i].parent, Tree().GetRootItem());
			if (hParent != NULL) {
				hPreParent = hParent;
				preKey = ls[i].parent;
			}
		}
		HTREEITEM hnew = Tree().InsertItem(ls[i].name, 0, 0, hPreParent);
		Tree().SetItemData(hnew, ls[i].key);
		prcdlg.m_ProgProc.StepIt(); // プログレスバーを更新
	}
	Tree().SelectItem(hsel);
	Tree().Expand(hsel, TVIS_EXPANDED);
}

void OutlineView::AddBranch2(const DWORD rootKey, node_vec &addNodes)
{
	iEditDoc* pDoc = GetDocument();

	HTREEITEM hRoot = FindKeyItem(rootKey, Tree().GetRootItem());
	HTREEITEM hNew;
	if (addNodes.size() > 0) {
		hNew = Tree().InsertItem(addNodes[0].GetName(), 0, 0, hRoot);
		Tree().SetItemData(hNew, addNodes[0].GetKey());
		Tree().SetItemState(hNew, TVIS_EXPANDED, TVIS_EXPANDED);
		Tree().SetItemImage(hNew, 0, 0);
		addNodes[0].SetParentKey(Tree().GetItemData(hRoot));
		GetDocument()->addNode2(addNodes[0]);
	}

	int loop = addNodes.size() - 1;
	if (loop <= 0) return;

	ProceedingDlg prcdlg;
	prcdlg.Create(IDD_ONPROC);
	prcdlg.m_ProcName.SetWindowText(_T("登録中"));
	prcdlg.m_ProgProc.SetStep(1); // プログレスバーの初期設定
	prcdlg.m_ProgProc.SetRange(0, loop);

	DWORD preKey = rootKey;
	HTREEITEM hParent = hRoot;
	HTREEITEM hSel = hParent;
	int prevLevel = addNodes[0].GetLevel();
	HTREEITEM hPrevNew = hNew;
	for (unsigned int i = 1; i < addNodes.size(); i++) {
		preKey = addNodes[i - 1].GetKey();
		hPrevNew = hNew;
		prevLevel = addNodes[i - 1].GetLevel();
		if (prevLevel > addNodes[i].GetLevel()) {
			unsigned int diff = prevLevel - addNodes[i].GetLevel();

			HTREEITEM hIt = hPrevNew;
			HTREEITEM hItParent = Tree().GetParentItem(hIt);
			for (unsigned int u = 0; u <= diff; u++) {
				hItParent = Tree().GetParentItem(hIt);
				hIt = hItParent;
			}
			hNew = Tree().InsertItem(addNodes[i].GetName(), 0, 0, hItParent);
			hParent = hItParent;
		}
		else if (prevLevel < addNodes[i].GetLevel()) {
			hNew = Tree().InsertItem(addNodes[i].GetName(), 0, 0, hPrevNew);
			hParent = hPrevNew;
		}
		else {
			hNew = Tree().InsertItem(addNodes[i].GetName(), 0, 0, hParent, hPrevNew);
		}
		addNodes[i].SetParentKey(Tree().GetItemData(hParent));
		GetDocument()->addNode2(addNodes[i]);
		Tree().SetItemData(hNew, addNodes[i].GetKey());
		Tree().SetItemImage(hNew, 0, 0);

		prcdlg.m_ProgProc.StepIt(); // プログレスバーを更新
	}

	Tree().SelectItem(hSel);
	iHint h; h.event = iHint::reflesh;
	GetDocument()->UpdateAllViews(NULL, (LPARAM)Tree().GetItemData(Selected()), &h);
}


HTREEITEM OutlineView::FindKeyItem(DWORD key, HTREEITEM item = NULL)
{
	HTREEITEM hItem;
	if (item == 0) {
		item = Tree().GetRootItem();
	}
	while (item != NULL) {
		hItem = item;
		if (Tree().GetItemData(hItem) == key) {
			return hItem;
		}
		if (Tree().ItemHasChildren(hItem)) {
			HTREEITEM cItem = FindKeyItem(key, Tree().GetChildItem(item));
			if (cItem != NULL) {
				return cItem;
			}
		}
		item = Tree().GetNextSiblingItem(item);
	}
	return NULL;
}

void OutlineView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CEdit* pEdit = Tree().GetEditControl();
	if (pEdit != NULL) return;
	CMenu menu;
	menu.LoadMenu(IDR_CONTEXT);

	CMenu* pPopup;
	if (!m_bHitR && point.x > 0 && point.y > 0) {
		return;
	}
	else {
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
		Tree().GetItemRect(Selected(), &r, TRUE);
		CPoint p; p.x = (r.left + r.right) / 2; p.y = (r.top + r.bottom) / 2;
		point += p;
	}
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
}

void OutlineView::OnRButtonDown(UINT nFlags, CPoint point)
{
	CEdit* pEdit = Tree().GetEditControl();
	if (pEdit != NULL) return;

	if (m_bAddingLink) {
		m_bAddingLink = false;
	}

	m_bHitR = false;

	TV_HITTESTINFO hitTestInfo;
	hitTestInfo.pt = point;
	Tree().HitTest(&hitTestInfo);

	if (hitTestInfo.flags & TVHT_ONITEMRIGHT || hitTestInfo.flags & TVHT_ONITEMINDENT ||
		hitTestInfo.flags & TVHT_ONITEMBUTTON || hitTestInfo.flags & TVHT_NOWHERE) {
		CTreeView::OnRButtonDown(nFlags, point);
		return;
	}

	if (hitTestInfo.hItem != 0) m_bHitR = true;
	Tree().SelectItem(hitTestInfo.hItem);
}

///////////////////////////
// 子ノード追加
void OutlineView::OnAddChild()
{
	if (m_bAdding) return;
	CEdit* pEdit = Tree().GetEditControl();
	if (pEdit != NULL) return;

	m_bAdding = true;
	m_bAddingChild = true;
	GetDocument()->DisableUndo();
	DisableUndo();
	HTREEITEM newItem = Tree().InsertItem(_T("新しいノード"), 0, 0, Selected());
	m_HNew = newItem;
	Tree().Expand(Selected(), TVE_EXPAND);
	Tree().SelectItem(newItem);
	Tree().EditLabel(newItem);
}

void OutlineView::OnUpdateAddChild(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_bAdding);
}

void OutlineView::OnAddSibling()
{
	if (m_bAdding) return;

	CEdit* pEdit = Tree().GetEditControl();
	if (pEdit != NULL) return;

	m_bAdding = true;
	m_bAddingChild = false;
	GetDocument()->DisableUndo();
	DisableUndo();
	CString cur = Tree().GetItemText(Selected());
	int cr;
	if ((cr = cur.Find('\r')) != -1) {
		cur = cur.Left(cr);
	}
	HTREEITEM newItem = Tree().InsertItem(cur, 0, 0, Tree().GetParentItem(Selected()), Selected());
	m_HNew = newItem;
	Tree().SelectItem(newItem);
	Tree().EditLabel(newItem);
}

void OutlineView::OnUpdateAddSibling(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(Selected() != Tree().GetRootItem());
}

void OutlineView::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	int style = GetDocument()->GetSelectedNodeTextStyle();
	bool multiline = GetDocument()->IsSelectedNodeMultiLine();
	CString label = GetDocument()->GetSelectedNodeLabel();
	bool multilineActual = label.Find(_T("\n")) != -1;

	if (multiline && multilineActual && !m_bAdding) {
		m_bLabelEditting = false;
		GetDocument()->RouteCmdToAllViews(NULL, ID_SET_NODE_PROP, CN_COMMAND, NULL, NULL);
		*pResult = 1;
		return;
	}

	m_bLabelEditting = true;
	*pResult = 0;
}

void OutlineView::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	m_bLabelEditting = false;
	CString editString = pTVDispInfo->item.pszText;

	iEditDoc* pDoc = GetDocument();
	if (!m_bAdding) {
		// ラベルが編集された場合
		if (editString != _T("")) {
			pTVDispInfo->item.mask = TVIF_TEXT;
			Tree().SetItem(&pTVDispInfo->item);
			pDoc->SetKeyNodeName(Tree().GetItemData(Selected()), Tree().GetItemText(pTVDispInfo->item.hItem));
		}
	}
	else {
		// ラベルが追加された場合
		if (editString.GetLength() > 256) {
			lstrcpy(pTVDispInfo->item.pszText, editString.Left(255));
		}

		if (editString != _T("")) {
			pTVDispInfo->item.mask = TVIF_TEXT;
			Tree().SetItem(&pTVDispInfo->item);
		}

		NodeProps l;
		l.name = Tree().GetItemText(pTVDispInfo->item.hItem);
		l.key = pDoc->AssignNewKey();
		//	tree().SetItemData(tree().GetSelectedItem(), l.key); // バグの温床 curItem() != NewItemの場合がある
		Tree().SetItemData(m_HNew, l.key);
		l.parent = Tree().GetItemData(Tree().GetParentItem(m_HNew));

		DWORD keyInherit = l.parent;
		if (!m_bAddingChild) {
			keyInherit = Tree().GetItemData(Tree().GetPrevSiblingItem(Tree().GetSelectedItem()));
		}
		bool binherit = false;
		CiEditApp* pApp = (CiEditApp*)AfxGetApp();
		if (pApp->m_rgsNode.bInheritParent && m_bAddingChild) {
			binherit = true;
		}
		if (pApp->m_rgsNode.bInheritSibling && !m_bAddingChild) {
			binherit = true;
		}

		pDoc->AddNode(l, keyInherit, binherit);

		m_bAdding = false;
		int branchMode = GetBranchMode();
		if (branchMode == 0) {
			pDoc->SelectionChanged(Tree().GetItemData(Selected()));
		}
		else {
			if (IsPosterityOF(m_hItemShowRoot, Selected())) {
				// showBranchモードのiEditDocメソッドを呼ぶ
				pDoc->SelectionChanged(Tree().GetItemData(Selected()), m_bNodeSel, true);
			}
		}
	}
	GetDocument()->DisableUndo();
	DisableUndo();
	*pResult = 0;
}

void OutlineView::SerializeTree(NodePropsVec &ls)
{
	if (m_opTreeOut == 0) {
		RecalcAllNodeLevels(); // level 設定
		treeview_for_each(Tree(), copy_key_label_vec(ls), Tree().GetRootItem());
	}
	else if (m_opTreeOut == 1) {
		RecalcSubNodeLevels(); // level 設定
		NodeProps l;
		l.key = Tree().GetItemData(Tree().GetSelectedItem());
		if (Tree().GetSelectedItem() != Tree().GetRootItem()) {
			l.parent = Tree().GetItemData(Tree().GetParentItem(Tree().GetSelectedItem()));
		}
		l.name = Tree().GetItemText(Tree().GetSelectedItem());
		l.state = Tree().GetItemState(Tree().GetSelectedItem(), TVIS_EXPANDED | TVIS_SELECTED);
		ls.push_back(l);
		if (Tree().ItemHasChildren(Tree().GetSelectedItem())) {
			treeview_for_each(Tree(), copy_key_label_vec(ls), Tree().GetChildItem(Tree().GetSelectedItem()));
		}
	}
	else if (m_opTreeOut == 2) {
		RecalcSubNodeLevels(); // level 設定
		NodeProps l;
		l.key = Tree().GetItemData(Tree().GetSelectedItem());
		if (Tree().GetSelectedItem() != Tree().GetRootItem()) {
			l.parent = Tree().GetItemData(Tree().GetParentItem(Tree().GetSelectedItem()));
		}
		l.name = Tree().GetItemText(Tree().GetSelectedItem());
		l.state = Tree().GetItemState(Tree().GetSelectedItem(), TVIS_EXPANDED | TVIS_SELECTED);
		ls.push_back(l);
		if (Tree().ItemHasChildren(Tree().GetSelectedItem())) {
			treeview_for_each2(Tree(), copy_key_label_vec(ls), Tree().GetChildItem(Tree().GetSelectedItem()));
		}
	}
}

void OutlineView::SerializeTree0(NodePropsVec &ls)
{
	RecalcAllNodeLevels();
	treeview_for_each(Tree(), copy_key_label_vec(ls), Tree().GetRootItem());
}

NodeKeyVec OutlineView::GetDrawOrder(const bool bShowSubBranch) const
{
	NodeKeyVec vec;
	if (!bShowSubBranch) {
		if (Tree().GetRootItem() == Tree().GetSelectedItem()) {
			vec.push_back(0);
			treeview_for_each2(Tree(), copy_key_vec(vec), Tree().GetChildItem(Tree().GetSelectedItem()));
		}
		else {
			HTREEITEM hParent = Tree().GetParentItem(Tree().GetSelectedItem());
			if (hParent == Tree().GetRootItem()) {
				vec.push_back(0);
			}
			treeview_for_each2(Tree(), copy_key_vec(vec), Tree().GetChildItem(hParent));
		}
	}
	else {
		int branchMode = GetBranchMode();
		if (branchMode == 2) {
			treeview_for_each(Tree(), copy_key_vec(vec), m_hItemShowRoot);
		}
		else if (branchMode == 1) {
			treeview_for_each2(Tree(), copy_key_vec(vec), Tree().GetChildItem(m_hItemShowRoot));
		}
	}
	return vec;
}

void OutlineView::OnEditLabel()
{
	Tree().EditLabel(Selected());
}

void OutlineView::OnUpdateEditLabel(CCmdUI* pCmdUI)
{
}

void OutlineView::OnLebelUp()
{
	if (m_bAdding) return;
	HTREEITEM hParent = Tree().GetParentItem(Selected());
	HTREEITEM hGrdParent = Tree().GetParentItem(hParent);
	HTREEITEM hcur = Selected();

	m_hParentPreMove = Tree().GetParentItem(Selected());
	m_hSiblingPreMove = Tree().GetPrevSiblingItem(Selected());

	HTREEITEM hNew = Tree().InsertItem(Tree().GetItemText(hcur), 0, 0, hGrdParent, hParent);
	Tree().SetItemState(hNew, Tree().GetItemState(hcur, TVIS_EXPANDED), TVIS_EXPANDED);
	Tree().SetItemData(hNew, Tree().GetItemData(hcur));
	int nImage; Tree().GetItemImage(hcur, nImage, nImage);
	Tree().SetItemImage(hNew, nImage, nImage);
	Tree().Expand(hGrdParent, TVE_EXPAND);
	GetDocument()->SetKeyNodeParent(Tree().GetItemData(hcur), Tree().GetItemData(hGrdParent));

	if (Tree().ItemHasChildren(hcur)) {
		CopySubNodes(Tree().GetChildItem(hcur), hNew);
	}
	Tree().DeleteItem(hcur);
	GetDocument()->DisableUndo();
	m_hItemMoved = hNew; // Undo Info
	Tree().SelectItem(hNew);
}

void OutlineView::OnUpdateLebelUp(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(Selected() != Tree().GetRootItem() && Tree().GetParentItem(Selected()) != Tree().GetRootItem());
}

void OutlineView::OnLebelDown()
{
	if (m_bAdding) return;
	HTREEITEM hcur = Selected();

	m_hParentPreMove = Tree().GetParentItem(Selected());
	m_hSiblingPreMove = Tree().GetPrevSiblingItem(Selected());

	HTREEITEM hNew = Tree().InsertItem(Tree().GetItemText(hcur), 0, 0, Tree().GetPrevSiblingItem(hcur));
	Tree().SetItemState(hNew, Tree().GetItemState(hcur, TVIS_EXPANDED), TVIS_EXPANDED);
	Tree().SetItemData(hNew, Tree().GetItemData(hcur));
	int nImage; Tree().GetItemImage(hcur, nImage, nImage);
	Tree().SetItemImage(hNew, nImage, nImage);
	Tree().Expand(Tree().GetPrevSiblingItem(hcur), TVE_EXPAND);
	GetDocument()->SetKeyNodeParent(Tree().GetItemData(hcur), Tree().GetItemData(Tree().GetPrevSiblingItem(hcur)));

	if (Tree().ItemHasChildren(hcur)) {
		CopySubNodes(Tree().GetChildItem(hcur), hNew);
	}
	Tree().SelectItem(hNew); // 先に選択状態にすることで、親ノード選択を防ぐ
	Tree().DeleteItem(hcur);
	GetDocument()->DisableUndo();
	m_hItemMoved = hNew; // Undo Info
	Tree().SelectItem(hNew);
}

void OutlineView::OnUpdateLebelDown(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(Selected() != Tree().GetRootItem() && Tree().GetPrevSiblingItem(Selected()) != NULL);
}

void OutlineView::OnOrderUp()
{
	if (m_bAdding) return;
	HTREEITEM hcur = Selected();
	HTREEITEM hpre = Tree().GetPrevSiblingItem(Selected());
	HTREEITEM hprepre = Tree().GetPrevSiblingItem(hpre);
	HTREEITEM hNew;

	// Undo Info
	m_hParentPreMove = Tree().GetParentItem(Selected());
	m_hSiblingPreMove = hpre;

	if (hprepre != NULL) {
		hNew = Tree().InsertItem(Tree().GetItemText(hcur), 0, 0, Tree().GetParentItem(hcur), hprepre);
	}
	else {
		hNew = Tree().InsertItem(Tree().GetItemText(hcur), 0, 0, Tree().GetParentItem(hcur), TVI_FIRST);
	}
	Tree().SetItemData(hNew, Tree().GetItemData(hcur));
	int nImage; Tree().GetItemImage(hcur, nImage, nImage);
	Tree().SetItemImage(hNew, nImage, nImage);
	Tree().SetItemState(hNew, Tree().GetItemState(hcur, TVIS_EXPANDED), TVIS_EXPANDED);
	if (Tree().ItemHasChildren(hcur)) {
		CopySubNodes(Tree().GetChildItem(hcur), hNew);
	}
	Tree().SelectItem(hNew);
	Tree().DeleteItem(hcur);
	GetDocument()->DisableUndo();
	m_hItemMoved = hNew; // Undo Info
	Tree().SelectItem(hNew);
	GetDocument()->SetModifiedFlag();
}

void OutlineView::OnUpdateOrderUp(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(Tree().GetPrevSiblingItem(Selected()) != NULL);
}

void OutlineView::OnOrderDown()
{
	if (m_bAdding) return;
	HTREEITEM hcur = Selected();

	// Undo Info
	m_hParentPreMove = Tree().GetParentItem(Selected());
	m_hSiblingPreMove = Tree().GetPrevSiblingItem(Selected());

	HTREEITEM hNew = Tree().InsertItem(Tree().GetItemText(hcur), 0, 0, Tree().GetParentItem(hcur), Tree().GetNextSiblingItem(hcur));
	Tree().SetItemData(hNew, Tree().GetItemData(hcur));
	int nImage; Tree().GetItemImage(hcur, nImage, nImage);
	Tree().SetItemImage(hNew, nImage, nImage);
	Tree().SetItemState(hNew, Tree().GetItemState(hcur, TVIS_EXPANDED), TVIS_EXPANDED);
	if (Tree().ItemHasChildren(hcur)) {
		CopySubNodes(Tree().GetChildItem(hcur), hNew);
	}
	Tree().DeleteItem(hcur);
	GetDocument()->DisableUndo();
	m_hItemMoved = hNew; // Undo Info
	Tree().SelectItem(hNew);
	GetDocument()->SetModifiedFlag();
}

void OutlineView::OnUpdateOrderDown(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(Tree().GetNextSiblingItem(Selected()) != NULL);
}

void OutlineView::CopySubNodes(HTREEITEM hOrg, HTREEITEM hNewParent)
{
	HTREEITEM hItem, item;
	item = hOrg;
	while (item != NULL) {
		hItem = item;
		HTREEITEM hNew = Tree().InsertItem(Tree().GetItemText(hItem), 0, 0, hNewParent);
		Tree().SetItemData(hNew, Tree().GetItemData(hItem));
		int nImage; Tree().GetItemImage(hItem, nImage, nImage);
		Tree().SetItemImage(hNew, nImage, nImage);
		Tree().SetItemState(hNew, Tree().GetItemState(hItem, TVIS_EXPANDED), TVIS_EXPANDED);
		if (Tree().ItemHasChildren(hItem)) {
			CopySubNodes(Tree().GetChildItem(hItem), hNew);
		}
		item = Tree().GetNextSiblingItem(item);
	}
}

void OutlineView::OnDelete()
{
	GetDocument()->DisableUndo();
	DisableUndo();
	DeleteNode();
}

void OutlineView::OnUpdateDelete(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(Selected() != Tree().GetRootItem() && !m_bLabelEditting || m_bLabelEditting);
}

void OutlineView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	CEdit* pEdit = Tree().GetEditControl();
	if (pEdit != NULL) return;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	bool bShowBranch = false;
	int branchMode = GetBranchMode();

	if (branchMode == 1) {
		if (Tree().GetParentItem(Selected()) != m_hItemShowRoot && m_hItemShowRoot != Selected()) {
			ResetBranchMode();
			GetDocument()->ResetShowBranch();
		}
		else {
			bShowBranch = true;
		}
	}
	else if (branchMode == 2) {
		if (!IsPosterityOF(m_hItemShowRoot, Selected()) && m_hItemShowRoot != Selected()) {
			ResetBranchMode();
			GetDocument()->ResetShowBranch();
		}
		else {
			bShowBranch = true;
		}
	}

	if (!m_bAdding) {
		GetDocument()->SelectionChanged(Tree().GetItemData(Selected()), true, bShowBranch);
	}
	*pResult = 0;
}

void OutlineView::OnEditUndo()
{
	if (m_bLabelEditting) {
		Tree().GetEditControl()->Undo();
		return;
	}
	if (m_hItemMoved == NULL && GetDocument()->CanUndo()) {
		GetDocument()->RestoreNodesForUndo();
		GetDocument()->RestoreLinksForUndo();
		iHint h; h.event = iHint::reflesh;
		GetDocument()->UpdateAllViews(NULL, (LPARAM)Tree().GetItemData(Selected()), &h);
		return;
	}

	HTREEITEM hNew;
	if (m_hSiblingPreMove != NULL) {
		hNew = Tree().InsertItem(Tree().GetItemText(m_hItemMoved), 0, 0, m_hParentPreMove, m_hSiblingPreMove);
	}
	else {
		hNew = Tree().InsertItem(Tree().GetItemText(m_hItemMoved), 0, 0, m_hParentPreMove, TVI_FIRST);
	}
	Tree().SetItemData(hNew, Tree().GetItemData(m_hItemMoved));
	int nImage; Tree().GetItemImage(m_hItemMoved, nImage, nImage);
	Tree().SetItemImage(hNew, nImage, nImage);
	Tree().SetItemState(hNew, Tree().GetItemState(m_hItemMoved, TVIS_EXPANDED), TVIS_EXPANDED);
	GetDocument()->SetKeyNodeParent(Tree().GetItemData(m_hItemMoved), Tree().GetItemData(m_hParentPreMove));
	if (Tree().ItemHasChildren(m_hItemMoved)) {
		CopySubNodes(Tree().GetChildItem(m_hItemMoved), hNew);
	}
	Tree().DeleteItem(m_hItemMoved);
	GetDocument()->DisableUndo();
	DisableUndo();
	Tree().SelectItem(hNew);
	GetDocument()->SetModifiedFlag();
}

void OutlineView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bLabelEditting || m_hItemMoved != NULL || GetDocument()->CanUndo());

}

void OutlineView::OnEditCut()
{
	if (m_bLabelEditting) {
		Tree().GetEditControl()->Cut();
	}
}

void OutlineView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
}

void OutlineView::OnEditCopy()
{
	if (m_bLabelEditting) {
		Tree().GetEditControl()->Copy();
	}
	else {
		GetDocument()->AddSelectedNodesToCopyOrg();
	}
}

void OutlineView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
}

void OutlineView::OnEditPaste()
{
	if (m_bLabelEditting) {
		Tree().GetEditControl()->Paste();
	}
	else {
		GetDocument()->DisableUndo();
		GetDocument()->DuplicateNodes(CPoint(0, 0));
	}
}

void OutlineView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	if (!m_bLabelEditting) {
		pCmdUI->Enable(GetDocument()->CanDuplicateNodes());
	}
}

void OutlineView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (pHint == NULL) return;
	m_bNodeSel = true;

	DWORD key = (DWORD)lHint;
	HTREEITEM item;
	iHint* ph = reinterpret_cast<iHint*>(pHint);
	switch (ph->event) {
	case iHint::nodeDelete:
		DisableUndo();
		DeleteNode();
		break;
	case iHint::nodeDeleteMulti:
		DisableUndo();
		DeleteKeyNode(key, ph->keyParent);
		break;
	case iHint::nodeSel:
	case iHint::parentSel:
		item = FindKeyItem(key, Tree().GetRootItem());
		if (item != NULL) {
			Tree().SelectItem(item);
		}
		break;
	case iHint::linkSelRet:
	case iHint::linkSel:
		m_bNodeSel = false;
		item = FindKeyItem(key, Tree().GetRootItem());
		if (item != NULL) {
			Tree().SelectItem(item);
		}
		break;
	case iHint::arcAdd:
	case iHint::rectAdd:
		DisableUndo();
		HTREEITEM newItem;
		if (Tree().GetSelectedItem() == Tree().GetRootItem()) {
			newItem = Tree().InsertItem(ph->str, 0, 0, Selected());
		}
		else {
			newItem = Tree().InsertItem(ph->str, 0, 0, Tree().GetParentItem(Selected()), Selected());
		}
		Tree().SetItemData(newItem, key);
		Tree().SetItemImage(newItem, ph->treeIconId, ph->treeIconId);
		//	tree().Expand(curItem(), TVE_EXPAND);
		Tree().SelectItem(newItem);
		break;
	case iHint::nodeLabelChanged:
		DisableUndo();
		Tree().SetItemText(Selected(), ph->str);
		break;
	case iHint::selectChild:
		if (Tree().ItemHasChildren(Selected())) {
			Tree().SelectItem(Tree().GetChildItem(Selected()));
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
		DisableUndo();
		break;
	case iHint::nodeDeleteByKey:
		break;
	case iHint::viewSettingChanged:
		ApplyColorSetting();
		SetViewFont();
		break;

	case iHint::groupMigrate:
		MoveNodes(ph->keyTarget, key);
		break;
	case iHint::nextNodeSibling:
		HTREEITEM hNext = Tree().GetNextSiblingItem(Selected());
		if (hNext != NULL) {
			Tree().SelectItem(hNext);
		}
		else {
			HTREEITEM hParent = Tree().GetParentItem(Selected());
			Tree().SelectItem(Tree().GetNextItem(hParent, TVGN_CHILD));
		}
		break;
	}
}

void OutlineView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	int branchMode = GetBranchMode();
	if (bActivate) {
		GetDocument()->SelectionChanged(Tree().GetItemData(Selected()), true, branchMode != 0);
	}

	CTreeView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void OutlineView::OnAddLink()
{
	m_bAddingLink = !m_bAddingLink;
}

void OutlineView::OnUpdateAddLink(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bAddingLink);
}

BOOL OutlineView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
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
	CEdit* pEdit = Tree().GetEditControl();
	if (pEdit != NULL) return;
	TV_HITTESTINFO hitTestInfo;
	hitTestInfo.pt = point;
	Tree().HitTest(&hitTestInfo);
	if (hitTestInfo.flags & TVHT_ONITEMRIGHT || hitTestInfo.flags & TVHT_ONITEMINDENT ||
		hitTestInfo.flags & TVHT_ONITEMBUTTON || hitTestInfo.flags & TVHT_NOWHERE) {
		CTreeView::OnLButtonDown(nFlags, point);
		return;
	}
	if (m_bAddingLink) {
		LinkPropertiesDlg dlg;
		dlg.strFrom = Tree().GetItemText(Selected());
		dlg.strTo = Tree().GetItemText(hitTestInfo.hItem);
		dlg.styleArrow = 0;
		if (dlg.DoModal() == IDOK) {
			// iEditDocのリンク追加処理
			GetDocument()->SetNewLinkInfo(Tree().GetItemData(Selected()), Tree().GetItemData(hitTestInfo.hItem),
				dlg.strComment, dlg.styleArrow);
		}
		m_bAddingLink = false;
		return;
	}
	else if (!m_bItemDragging) {
		m_hitemDragPre = hitTestInfo.hItem;
	}
	CTreeView::OnLButtonDown(nFlags, point);
}

void OutlineView::DeleteNode()
{
	if (m_bLabelEditting) {
		CEdit* pEdit = Tree().GetEditControl();
		pEdit->SendMessage(WM_KEYDOWN, VK_DELETE, VK_DELETE);
		return;
	}
	HTREEITEM hcur = Selected();
	CString m = "<" + Tree().GetItemText(hcur) + _T(">") + '\n' + _T("削除しますか？");
	if (MessageBox(m, _T("ノードの削除"), MB_YESNO) != IDYES) return;
	GetDocument()->BackupDeleteBound();
	if (Tree().GetNextSiblingItem(Selected()) == NULL) {
		if (Tree().GetPrevSiblingItem(Selected()) != NULL) {
			Tree().SelectItem(Tree().GetPrevSiblingItem(Selected()));
		}
	}
	GetDocument()->DeleteKeyItem(Tree().GetItemData(hcur));
	if (Tree().ItemHasChildren(hcur)) {
		NodePropsVec ls;
		treeview_for_each(Tree(), copy_key_label_vec(ls), Tree().GetChildItem(hcur));
		for (unsigned int i = 0; i < ls.size(); i++) {
			GetDocument()->DeleteKeyItem(ls[i].key);
		}
	}
	Tree().DeleteItem(hcur);
}

void OutlineView::DeleteKeyNode(DWORD key, DWORD parentKey)
{
	if (key == 0) return;
	HTREEITEM hParent = FindKeyItem(parentKey);
	if (hParent == NULL) return;
	HTREEITEM hDeleteItem = FindKeyItem(key, hParent);
	if (hDeleteItem == NULL) return;
	GetDocument()->DeleteKeyItem(Tree().GetItemData(hDeleteItem));
	if (Tree().ItemHasChildren(hDeleteItem)) {
		NodePropsVec ls;
		treeview_for_each(Tree(), copy_key_label_vec(ls), Tree().GetChildItem(hDeleteItem));
		for (unsigned int i = 0; i < ls.size(); i++) {
			GetDocument()->DeleteKeyItem(ls[i].key);
		}
	}
	Tree().DeleteItem(hDeleteItem);
}

void OutlineView::OnAddUrl()
{
	LinkForPathDlg dlg;
	dlg.strOrg = Tree().GetItemText(Selected());
	if (dlg.DoModal() != IDOK) return;
	GetDocument()->DisableUndo();
	DisableUndo();
	if (dlg.strComment == _T("") && dlg.strPath != _T("")) {
		CString drive, dir, fileName, ext;
		FileUtil::SplitPath(dlg.strPath, drive, dir, fileName, ext);
		dlg.strComment.Format(_T("%s%s"), fileName, ext);
	}
	GetDocument()->AddUrlLink(dlg.strPath, dlg.strComment);
}

void OutlineView::OnUpdateAddUrl(CCmdUI* pCmdUI)
{
}

void OutlineView::OnSelectChild()
{
	Tree().SelectItem(Tree().GetChildItem(Selected()));
}

void OutlineView::OnUpdateSelectChild(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(Tree().ItemHasChildren(Selected()));
}

void OutlineView::OnSelectParent()
{
	Tree().SelectItem(Tree().GetParentItem(Selected()));
}

void OutlineView::OnUpdateSelectParent(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(Selected() != Tree().GetRootItem());
}

void OutlineView::SetViewFont()
{
	LOGFONT lf;
	CFont *pFont = GetFont();
	if (pFont != NULL) {
		pFont->GetObject(sizeof(LOGFONT), &lf);
	}
	else {
		::GetObject(GetStockObject(SYSTEM_FIXED_FONT), sizeof(LOGFONT), &lf);
	}
	CString defaultFont = _T("MS UI Gothic");
	if (SystemConfiguration::MeiryoUiAvailable()) {
		defaultFont = _T("Meiryo UI");
	}
	::lstrcpy(lf.lfFaceName, AfxGetApp()->GetProfileString(REGS_FRAME, _T("Font1 Name"), defaultFont));
	lf.lfHeight = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font1 Height"), 0xfffffff3);
	lf.lfWidth = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font1 Width"), 0);
	lf.lfItalic = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font1 Italic"), FALSE);
	lf.lfUnderline = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font1 UnderLine"), FALSE);
	lf.lfStrikeOut = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font1 StrikeOut"), FALSE);
	lf.lfCharSet = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font1 CharSet"), SHIFTJIS_CHARSET);
	lf.lfWeight = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font1 Weight"), FW_NORMAL);
	m_font.CreateFontIndirect(&lf);
	SetFont(&m_font, TRUE);
}

void OutlineView::OnSortChildren()
{
	if (Tree().SortChildren(Selected())) {
		GetDocument()->SetModifiedFlag();
	}
}

void OutlineView::OnUpdateSortChildren(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(Tree().ItemHasChildren(Selected()));
}

void OutlineView::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	m_bItemDragging = true;

	CPoint		ptAction;
	UINT		nFlags;

	GetCursorPos(&ptAction);
	ScreenToClient(&ptAction);
	if (m_hitemDragPre != NULL) {
		m_hitemDrag = m_hitemDragPre;
		m_hitemDragPre = NULL;
	}
	else {
		m_hitemDrag = Tree().HitTest(ptAction, &nFlags);
	}
	Tree().SelectItem(m_hitemDrag);
	m_hitemDrop = NULL;
	*pResult = 0;
}

void OutlineView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bItemDragging) {
		Tree().SelectDropTarget(NULL);
		if (m_nDropStatus == OutlineView::drop_child) {
			if (m_hitemDrag != m_hitemDrop && m_hitemDrop != NULL && !IsChildNodeOf(m_hitemDrop, m_hitemDrag) &&
				Tree().GetParentItem(m_hitemDrag) != m_hitemDrop) {

				m_hParentPreMove = Tree().GetParentItem(m_hitemDrag);
				m_hSiblingPreMove = Tree().GetPrevSiblingItem(m_hitemDrag);


				HTREEITEM hNew = Tree().InsertItem(Tree().GetItemText(m_hitemDrag), 0, 0, m_hitemDrop);
				Tree().SetItemState(hNew, Tree().GetItemState(m_hitemDrag, TVIS_EXPANDED), TVIS_EXPANDED);
				Tree().SetItemData(hNew, Tree().GetItemData(m_hitemDrag));
				int nImage; Tree().GetItemImage(m_hitemDrag, nImage, nImage);
				Tree().SetItemImage(hNew, nImage, nImage);
				Tree().Expand(m_hitemDrop, TVE_EXPAND);
				GetDocument()->SetKeyNodeParent(Tree().GetItemData(m_hitemDrag), Tree().GetItemData(m_hitemDrop));
				if (Tree().ItemHasChildren(m_hitemDrag)) {
					CopySubNodes(Tree().GetChildItem(m_hitemDrag), hNew);
				}
				GetDocument()->DisableUndo();
				m_hItemMoved = hNew; // Undo Info
				Tree().SelectItem(hNew);         // ここで1度選択しておかないと
				Tree().DeleteItem(m_hitemDrag);  // 一番下端にあるノードを消した時にF6状態くずれる
				Tree().SelectItem(hNew);         // 再度選択
			}
			else {
				Tree().SelectItem(m_hitemDrag);
			}
		}
		else if (m_nDropStatus == OutlineView::drop_sibling) {
			if (m_hitemDrag != m_hitemDrop && m_hitemDrop != NULL &&
				!IsChildNodeOf(m_hitemDrop, m_hitemDrag)) {

				m_hParentPreMove = Tree().GetParentItem(m_hitemDrag);
				m_hSiblingPreMove = Tree().GetPrevSiblingItem(m_hitemDrag);

				if (Tree().ItemHasChildren(m_hitemDrop) &&
					(TVIS_EXPANDED & Tree().GetItemState(m_hitemDrop, TVIS_EXPANDED))) {
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
				HTREEITEM hParent = Tree().GetParentItem(m_hitemDrop);
				HTREEITEM hNew = Tree().InsertItem(Tree().GetItemText(m_hitemDrag), 0, 0, hParent, m_hitemDrop);
				Tree().SetItemData(hNew, Tree().GetItemData(m_hitemDrag));
				int nImage; Tree().GetItemImage(m_hitemDrag, nImage, nImage);
				Tree().SetItemImage(hNew, nImage, nImage);
				GetDocument()->SetKeyNodeParent(Tree().GetItemData(m_hitemDrag), Tree().GetItemData(hParent));
				if (Tree().ItemHasChildren(m_hitemDrag)) {
					CopySubNodes(Tree().GetChildItem(m_hitemDrag), hNew);
				}

				GetDocument()->DisableUndo();
				m_hItemMoved = hNew; // Undo Info
				Tree().SelectItem(hNew);
				Tree().DeleteItem(m_hitemDrag);
				Tree().SelectItem(hNew);
			}
			else {
				Tree().SelectItem(m_hitemDrag);
			}
		}

		Tree().SetInsertMark(NULL);
		Tree().SelectDropTarget(NULL);
		m_bItemDragging = false;
	}
	CTreeView::OnLButtonUp(nFlags, point);
}

void OutlineView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bItemDragging)
	{
		if (!nFlags & MK_LBUTTON) {
			Tree().SetInsertMark(NULL);
			Tree().SelectDropTarget(NULL);
			m_bItemDragging = false;
			CTreeView::OnMouseMove(nFlags, point);
			return;
		}

		TV_HITTESTINFO hitTestInfo;
		hitTestInfo.pt = point;
		Tree().HitTest(&hitTestInfo);

		m_nDropStatus = OutlineView::drop_none;
		if (hitTestInfo.flags & TVHT_ONITEMLABEL) {
			m_nDropStatus = OutlineView::drop_child;  // child
			Tree().SetInsertMark(NULL);
			Tree().SelectDropTarget(hitTestInfo.hItem);
		}
		else if (hitTestInfo.flags & TVHT_ONITEMINDENT ||
			hitTestInfo.flags & TVHT_ONITEMBUTTON ||
			hitTestInfo.flags & TVHT_ONITEMICON ||
			hitTestInfo.flags & TVHT_ONITEMRIGHT) {
			m_nDropStatus = OutlineView::drop_sibling; // sibling
			Tree().SelectDropTarget(NULL);
			Tree().SetInsertMark(hitTestInfo.hItem, TRUE);
		}
		else if (hitTestInfo.flags &  TVHT_NOWHERE) {
			m_nDropStatus = OutlineView::drop_none;
			Tree().SetInsertMark(NULL);
			Tree().SelectDropTarget(NULL);
		}
		else if (hitTestInfo.flags & TVHT_ABOVE ||
			hitTestInfo.flags & TVHT_BELOW ||
			hitTestInfo.flags & TVHT_TOLEFT ||
			hitTestInfo.flags & TVHT_TORIGHT) {
			m_nDropStatus = OutlineView::drop_none;
			Tree().SetInsertMark(NULL);
			Tree().SelectDropTarget(NULL);
			m_bItemDragging = false;
		}

		CRect rc; GetClientRect(rc);
		if (m_nDropStatus == 2 || m_nDropStatus == 1)
		{
			m_hitemDrop = hitTestInfo.hItem;
			if (point.y < Tree().GetItemHeight()) {
				Tree().Select(Tree().GetPrevVisibleItem(Tree().GetFirstVisibleItem()), TVGN_FIRSTVISIBLE);
			}
			else if (point.y > rc.Height() - Tree().GetItemHeight()) {
				Tree().Select(Tree().GetNextVisibleItem(Tree().GetFirstVisibleItem()), TVGN_FIRSTVISIBLE);
			}
		}
		else {
			m_hitemDrop = NULL;
		}
	}

	CTreeView::OnMouseMove(nFlags, point);
}

void OutlineView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ESCAPE) {
		if (m_bItemDragging) {
			m_bItemDragging = false;
			ReleaseCapture();
			Tree().SelectDropTarget(NULL);
			Tree().SetInsertMark(NULL);
			Invalidate();
		}
		int branchMode = GetBranchMode();
		if (branchMode == 0) {
			GetDocument()->SelectionChanged(Tree().GetItemData(Selected()));
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
	} while ((hitemChild = Tree().GetParentItem(hitemChild)) != NULL);

	return (hitemChild != NULL);
}

void OutlineView::OnImportData()
{
	CString txtpath;
	WCHAR szFilters[] = _T("テキストファイル (*.txt)|*.txt|XML ファイル (*.xml)|*.xml|JSON ファイル (*.json)|*.json||");
	CFileDialog dlg(TRUE, _T("txt"), txtpath, OFN_HIDEREADONLY, szFilters, this);
	if (dlg.DoModal() != IDOK) return;
	CString infileName = dlg.GetPathName();
	CString drive, dir, fileName, extent;
	FileUtil::SplitPath(infileName, drive, dir, fileName, extent);
	extent.MakeLower();

	CString caption;
	int TextLevelCharNum = 0;
	if (extent == _T(".txt")) {
		ImportTextDlg dlg;
		dlg.m_fileName = CString(fileName) + extent;
		dlg.m_charSelection = TextLevelCharNum;
		if (dlg.DoModal() != IDOK) {
			return;
		}
		TextLevelCharNum = dlg.m_charSelection;
	}
	else if (extent == _T(".xml")) {
		caption = _T("XML ファイルのインポート");
		CString mes = infileName + _T(" をインポートしますか");
		if (MessageBox(mes, caption, MB_YESNO) != IDYES) {
			return;
		}
	}
	else if (extent == _T(".json")) {
		caption = _T("JSON ファイルのインポート");
		CString mes = infileName + _T(" をインポートしますか");
		if (MessageBox(mes, caption, MB_YESNO) != IDYES) {
			return;
		}
	}
	else {
		return;
	}

	int mode = GetBranchMode();
	HTREEITEM hShowRoot;
	if (mode != 0) {
		hShowRoot = m_hItemShowRoot;
		ResetBranchMode();
		GetDocument()->ResetShowBranch();
	}
	bool ret;
	if (extent == _T(".txt")) {
		node_vec addNodes;
		char levelChar = '.';
		if (TextLevelCharNum == 1) {
			levelChar = '\t';
		}
		ret = ImportText(infileName, addNodes, levelChar);
		if (ret) {
			AddBranch2(Tree().GetItemData(Selected()), addNodes);
		}
	}
	else if (extent == _T(".xml")) {
		ret = ImportXML(infileName);
		if (ret) {
			AddBranch(Tree().GetItemData(Selected()));
		}
	}
	else if (extent == _T(".json")) {
		return;
	}

	if (mode == 1) {
		Tree().SelectItem(hShowRoot);
		OnShowSelectedChildren();
	}
	else if (mode == 2) {
		Tree().SelectItem(hShowRoot);
		OnShowSelectedBranch();
	}
	if (ret) {
		MessageBox(_T("終了しました"), caption, MB_OK);
	}
	else {
		MessageBox(_T("失敗しました"), caption, MB_OK);
	}
}

void OutlineView::OnUpdateImportData(CCmdUI* pCmdUI)
{
}

void OutlineView::OutputHtml()
{
	if (!InputExportOptions()) return;
	if (!InputHtmlExportFolder()) return;

	HTREEITEM root;
	if (m_exportOption.htmlOutOption == 0) {
		root = Tree().GetRootItem();
	}
	else {
		if (GetDocument()->ShowSubBranch()) {
			root = m_hItemShowRoot;
		}
		else {
			if (Tree().GetSelectedItem() != Tree().GetRootItem()) {
				root = Tree().GetParentItem(Tree().GetSelectedItem());
			}
			else {
				root = Tree().GetRootItem();
			}
		}
	}
	CString keystr;
	keystr.Format(_T("%d"), Tree().GetItemData(root));

	CWaitCursor wc;
	_wsetlocale(LC_ALL, _T("jpn"));

	/////////////////// create frame
	CString indexFilePath = m_exportOption.htmlOutDir + _T("\\") + m_exportOption.pathIndex;
	FILE* pFp;
	if ((pFp = FileUtil::CreateStdioFile(indexFilePath)) == NULL) return;
	CStdioFile f(pFp);
	HtmlWriter::WriteHtmlHeader(f);
	CString title = GetDocument()->GetFileNameFromPath();
	if (m_exportOption.prfIndex != _T("")) {
		title = m_exportOption.prfIndex;
	}
	HtmlWriter::CreateFrame(f, title, keystr, m_exportOption);
	f.Close();

	CString olName = m_exportOption.htmlOutDir + _T("\\") + m_exportOption.pathOutline;
	FILE* pOf;
	if ((pOf = FileUtil::CreateStdioFile(olName)) == NULL) return;
	CStdioFile olf(pOf);
	HtmlWriter::WriteOutlineStart(olf, keystr, GetDocument()->GetKeyNodeLabel(Tree().GetItemData(root)), m_exportOption);

	/////////////////// output text
	CString arName = m_exportOption.htmlOutDir + _T("\\") + m_exportOption.pathTextSingle;
	FILE* pf;
	if ((pf = FileUtil::CreateStdioFile(arName)) == NULL) return;
	CStdioFile tf(pf);
	if (m_exportOption.textOption == 0) {
		HtmlWriter::WriteHtmlHeader(tf);
		HtmlWriter::WriteTextStyle(tf);
		HtmlWriter::WriteBodyStart(tf);
		GetDocument()->WriteKeyNodeToHtml(Tree().GetItemData(root), tf);
	}
	else {
		CString arName = m_exportOption.htmlOutDir + _T("\\text\\") + m_exportOption.prfTextEverynode + keystr + _T(".html");
		FILE* pRf;
		if ((pRf = FileUtil::CreateStdioFile(arName)) == NULL) return;
		CStdioFile rootTf(pRf);
		HtmlWriter::WriteHtmlHeader(rootTf);
		HtmlWriter::WriteTextStyle(rootTf, false);
		HtmlWriter::WriteBodyStart(rootTf);
		GetDocument()->WriteKeyNodeToHtml(Tree().GetItemData(root), rootTf, true, m_exportOption.prfTextEverynode);
		HtmlWriter::WriteBodyEnd(rootTf);
		rootTf.Close();
	}

	/////////////////// output SubTree
	if (Tree().ItemHasChildren(root)) {
		HTREEITEM child = Tree().GetNextItem(root, TVGN_CHILD);
		OutputOutlineHtml(root, child, olf, tf);
	}

	if (m_exportOption.navOption != 1) {
		HtmlWriter::WriteOutlineEnd(olf);
		olf.Close();
		if (m_exportOption.textOption == 0) {
			HtmlWriter::WriteBodyEnd(tf);
			tf.Close();
		}
	}
	olf.Close();
	tf.Close();

	///////////////////// create network
	if (m_exportOption.navOption > 0) {
		CString nName = m_exportOption.htmlOutDir + _T("\\") + m_exportOption.pathNetwork;
		FILE* pNf;
		if ((pNf = FileUtil::CreateStdioFile(nName)) == NULL) return;
		CStdioFile nf(pNf);
		HtmlWriter::WriteHtmlHeader(nf);
		HtmlWriter::WriteBodyStart(nf);

		if (m_exportOption.imgOption == 0) {
			CString svgPath = m_exportOption.htmlOutDir + _T("\\") + m_exportOption.pathSvg;
			if (m_exportOption.textOption == 0) {
				GetDocument()->ExportSvg(svgPath, true, m_exportOption.pathTextSingle);
			}
			else {
				GetDocument()->ExportSvg(svgPath, true, m_exportOption.prfTextEverynode, false);
			}
			HtmlWriter::WriteSvgNetwork(nf, GetDocument()->GetMaxPt(), m_exportOption.pathSvg);
		}
		else {
			GetDocument()->SaveCurrentImage(m_exportOption.htmlOutDir + _T("\\") + m_exportOption.pathPng);
			HtmlWriter::WritePngNetworkStart(nf, m_exportOption.pathPng);
			if (m_exportOption.textOption == 0) {
				GetDocument()->WriteClickableMap(nf, m_exportOption.pathTextSingle);
			}
			else {
				GetDocument()->WriteClickableMap(nf, m_exportOption.prfTextEverynode, false);
			}
			HtmlWriter::WritePngNetworkEnd(nf);
		}
		HtmlWriter::WriteBodyEnd(nf);
		nf.Close();
	}
	_wsetlocale(LC_ALL, _T(""));

	if (((CiEditApp*)AfxGetApp())->m_rgsOther.bOpenFilesAfterExport) {
		ShellExecute(m_hWnd, _T("open"), indexFilePath, NULL, NULL, SW_SHOW);
	}
}

bool OutlineView::InputExportOptions() {
	ExportHtmlDlg eDlg;

	eDlg.m_xvRdTree = m_exportOption.htmlOutOption;
	eDlg.m_xvRdNav = m_exportOption.navOption;
	eDlg.m_xvRdImg = m_exportOption.imgOption;
	eDlg.m_xvRdText = m_exportOption.textOption;
	eDlg.m_xvEdPrfIndex = m_exportOption.prfIndex;
	eDlg.m_xvEdPrfToc = m_exportOption.prfToc;
	eDlg.m_xvEdPrfNet = m_exportOption.prfNet;
	eDlg.m_xvEdPrfTextSingle = m_exportOption.prfTextSingle;
	eDlg.m_xvEdPrfTextEverynode = m_exportOption.prfTextEverynode;
	eDlg.m_pathIndex = m_exportOption.pathIndex;
	eDlg.m_pathTextSingle = m_exportOption.pathTextSingle;
	eDlg.m_pathNetwork = m_exportOption.pathNetwork;
	eDlg.m_pathOutline = m_exportOption.pathOutline;
	eDlg.m_pathSvg = m_exportOption.pathSvg;
	eDlg.m_pathPng = m_exportOption.pathPng;
	eDlg.m_docTitle = GetDocument()->GetFileNameFromPath();
	eDlg.m_nameOfRoot = Tree().GetItemText(Tree().GetRootItem());
	if (GetDocument()->ShowSubBranch()) {
		eDlg.m_nameOfVisibleRoot = Tree().GetItemText(m_hItemShowRoot);
		eDlg.m_xvRdTree = 1;
	}
	else {
		if (Tree().GetSelectedItem() == Tree().GetRootItem()) {
			eDlg.m_nameOfVisibleRoot = Tree().GetItemText(Tree().GetRootItem());
		}
		else {
			eDlg.m_nameOfVisibleRoot =
				Tree().GetItemText(Tree().GetParentItem(Tree().GetSelectedItem()));
		}
	}
	if (eDlg.DoModal() != IDOK) return false;
	m_exportOption.htmlOutOption = eDlg.m_xvRdTree;
	m_exportOption.navOption = eDlg.m_xvRdNav;
	m_exportOption.imgOption = eDlg.m_xvRdImg;
	m_exportOption.textOption = eDlg.m_xvRdText;
	m_exportOption.prfIndex = eDlg.m_xvEdPrfIndex;
	m_exportOption.prfToc = eDlg.m_xvEdPrfToc;
	m_exportOption.prfNet = eDlg.m_xvEdPrfNet;
	m_exportOption.prfTextSingle = eDlg.m_xvEdPrfTextSingle;
	m_exportOption.prfTextEverynode = eDlg.m_xvEdPrfTextEverynode;
	m_exportOption.pathIndex = eDlg.m_pathIndex;
	m_exportOption.pathNetwork = eDlg.m_pathNetwork;
	m_exportOption.pathOutline = eDlg.m_pathOutline;
	m_exportOption.pathSvg = eDlg.m_pathSvg;
	m_exportOption.pathPng = eDlg.m_pathPng;
	m_exportOption.pathTextSingle = eDlg.m_pathTextSingle;

	return true;
}

bool OutlineView::InputHtmlExportFolder()
{
	CString defaultPath = AfxGetApp()->GetProfileString(_T("Settings"), _T("HTML OutputDir"), _T(""));

	if (!FileUtil::SelectFolder(m_exportOption.htmlOutDir, defaultPath, m_hWnd)) {
		return false;
	}

	CString indexFilePath = m_exportOption.htmlOutDir + _T("\\") + m_exportOption.pathIndex;
	CFileFind find;
	if (find.FindFile(indexFilePath)) {
		if (MessageBox(
			indexFilePath + _T("\n既存のファイルを上書きしてよいですか"), _T("HTML出力"),
			MB_YESNO) != IDYES) {
			return false;
		}
	}

	CString textDir = m_exportOption.htmlOutDir + _T("\\text");
	if (m_exportOption.textOption == 1) {
		if (!find.FindFile(textDir)) {
			if (!::CreateDirectory(textDir, NULL)) {
				MessageBox(_T("フォルダー作成に失敗しました"));
				return false;
			}
		}
	}

	AfxGetApp()->WriteProfileString(_T("Settings"), _T("HTML OutputDir"), m_exportOption.htmlOutDir);

	return true;
}

void OutlineView::OutputOutlineHtml(HTREEITEM hRoot, HTREEITEM hItem, CStdioFile& foutline, CStdioFile& ftext)
{
	CString keystr;
	keystr.Format(_T("%d"), Tree().GetItemData(hItem));
	// アウトライン書き込み
	HtmlWriter::WriteSubTree(foutline, keystr, GetDocument()->GetKeyNodeLabel(Tree().GetItemData(hItem)), m_exportOption);

	// Text出力
	DWORD key = Tree().GetItemData(hItem);
	if (m_exportOption.textOption == 0) {
		GetDocument()->WriteKeyNodeToHtml(key, ftext);
	}
	else {
		CString fName = m_exportOption.htmlOutDir + _T("\\text\\")
			+ m_exportOption.prfTextEverynode + keystr + _T(".html");
		FILE* pRf;
		if ((pRf = FileUtil::CreateStdioFile(fName)) == NULL) return;
		CStdioFile tf(pRf);
		HtmlWriter::WriteHtmlHeader(tf);
		HtmlWriter::WriteTextStyle(tf, false);
		HtmlWriter::WriteBodyStart(tf);
		GetDocument()->WriteKeyNodeToHtml(key, tf, true, m_exportOption.prfTextEverynode);
		HtmlWriter::WriteBodyEnd(tf);
		tf.Close();
	}

	bool nested = m_exportOption.htmlOutOption == 0 ||
		m_exportOption.htmlOutOption == 1 && GetDocument()->ShowSubBranch();
	if (Tree().ItemHasChildren(hItem) && nested) {
		if (m_exportOption.navOption != 1) {
			HtmlWriter::WriteChildrenStart(foutline);
		}
		HTREEITEM hchildItem = Tree().GetNextItem(hItem, TVGN_CHILD);
		OutputOutlineHtml(hRoot, hchildItem, foutline, ftext);
	}
	else {
		HtmlWriter::WriteSiblingEnd(foutline);
		HTREEITEM hnextItem = Tree().GetNextItem(hItem, TVGN_NEXT);
		if (hnextItem == NULL) {    // 次に兄弟がいない
			HTREEITEM hi = hItem;
			HTREEITEM hParent = hItem;
			while (Tree().GetParentItem(hParent) != hRoot) {
				hParent = Tree().GetParentItem(hi);
				HTREEITEM hnextParent;
				if (m_exportOption.navOption != 1) {
					HtmlWriter::WriteSiblingStart(foutline);
				}
				if ((hnextParent = Tree().GetNextItem(hParent, TVGN_NEXT)) != NULL) {
					OutputOutlineHtml(hRoot, hnextParent, foutline, ftext);
					return;
				}
				hi = hParent;
			}                                   // 兄弟のいる親まで戻る
		}
		else {
			OutputOutlineHtml(hRoot, hnextItem, foutline, ftext);
		}                                       // 兄弟に移動
	}
}

void OutlineView::OutputOutlineText(HTREEITEM hItem, CStdioFile *f, int tab)
{
	if (Tree().GetPrevSiblingItem(hItem) == Tree().GetSelectedItem() && m_textExportOption.treeOption != 0) {
		return;
	}

	CString indentChar = _T(".");
	if (m_textExportOption.formatOption == 3) {
		indentChar = _T("#");
	}
	else if (m_textExportOption.formatOption == 4) {
		indentChar = _T("*");
	}
	if (m_textExportOption.chapterNumberOption == 0 || m_textExportOption.formatOption == 3 || m_textExportOption.formatOption == 4) {
		f->WriteString(indentChar);
		for (int i = 0; i < tab; i++) {
			f->WriteString(indentChar);
		}
		f->WriteString(_T(" "));
	}
	else {
		CString chapNum = GetDocument()->GetKeyNodeChapterNumber(Tree().GetItemData(hItem));
		if (chapNum.GetLength() > 0) {
			if (m_textExportOption.formatOption != 1) {
				f->WriteString(_T("\n"));
			}
			f->WriteString(chapNum + _T(" "));
		}
	}

	CString label = StringUtil::RemoveMachineDependentChar(Tree().GetItemText(hItem));
	label = StringUtil::RemoveCr(label);
	f->WriteString(label + _T("\n"));

	if (m_textExportOption.formatOption != 1) {
		CString text = StringUtil::RemoveMachineDependentChar(GetDocument()->GetKeyNodeText(Tree().GetItemData(hItem)));
		f->WriteString(StringUtil::ReplaceCrToLf(text));
		f->WriteString(_T("\n"));
	}

	if (Tree().ItemHasChildren(hItem) && m_textExportOption.treeOption != 2) {           // 子どもに移動
		HTREEITEM hchildItem = Tree().GetNextItem(hItem, TVGN_CHILD);
		OutputOutlineText(hchildItem, f, ++tab);
	}
	else {
		HTREEITEM hnextItem = Tree().GetNextItem(hItem, TVGN_NEXT);
		if (hnextItem == NULL) {    // 次に兄弟がいない
			HTREEITEM hi = hItem;
			HTREEITEM hParent = hItem;
			while (hParent != Tree().GetRootItem()) {
				hParent = Tree().GetParentItem(hi);
				HTREEITEM hnextParent;
				--tab;
				if ((hnextParent = Tree().GetNextItem(hParent, TVGN_NEXT)) != NULL) {
					OutputOutlineText(hnextParent, f, tab);
					return;
				}
				hi = hParent;
			}                                   // 兄弟のいる親まで戻る
		}
		else {
			OutputOutlineText(hnextItem, f, tab);
		}                                       // 兄弟に移動
	}
}

bool OutlineView::ImportText(const CString &inPath, node_vec &addNodes, const char levelChar)
{
	_wsetlocale(LC_ALL, _T("jpn"));

	FILE* fp;
	if ((fp = FileUtil::OpenStdioFile(inPath)) == NULL) return false;
	CStdioFile f(fp);

	CString line;
	vector<CString> lines;
	while (f.ReadString(line) != NULL) {
		lines.push_back(line);
	}
	_wsetlocale(LC_ALL, _T(""));
	f.Close();
	return AddTreeAcordingToLevel(lines, addNodes, levelChar);
}

bool OutlineView::AddTreeAcordingToLevel(const vector<CString> &lines, node_vec &addNodes, const char levelChar)
{
	ProceedingDlg prcdlg;
	prcdlg.Create(IDD_ONPROC);
	prcdlg.m_ProcName.SetWindowText(_T("インポート中"));
	prcdlg.m_ProgProc.SetStep(1);              // プログレスバーの初期設定
	prcdlg.m_ProgProc.SetRange(0, (short)(lines.size() - 1));

	CSize mvSz(30, 30);
	CString label;
	CString text;
	int curLevel = 0;
	bool nodeCreated = false;
	for (unsigned int i = 0; i < lines.size(); i++) {
		int level = GetIndentCount(lines[i], levelChar);
		if (level > curLevel && level - curLevel > 1 && nodeCreated) {
			CString mes; mes.Format(_T("%d行目 : %s"), i + 1, lines[i]);
			MessageBox(mes, _T("インポートエラー:階層が正しくありません"), MB_ICONSTOP);
			return false;
		}
		if (level == 0) {
			if (text == _T("") && lines[i] == _T("")) {
				continue;
			}
			text.Append(lines[i] + _T("\r\n"));
			continue;
		}
		else {
			if (label == _T("")) {
				CString line = lines[i];
				label = line.TrimLeft(_T("\t."));
				continue;
			}
			nodeCreated = true;
			iNode node(label);
			node.SetText(text);
			node.SetKey(GetDocument()->AssignNewKey());
			node.SetParentKey(Tree().GetItemData(Selected()));
			node.MoveBound(mvSz);
			node.SetLevel(curLevel);
			addNodes.push_back(node);

			text = _T("");
			CString line = lines[i];
			label = line.TrimLeft(_T("\t."));
			curLevel = level;
			if (label == _T("")) {
				label = _T("<名称未設定>");
			}
			mvSz.cx += 20; mvSz.cy += 30;
			prcdlg.m_ProgProc.StepIt();  // プログレスバーを更新
		}
	}

	if (label != _T("")) {
		iNode node;
		node.SetName(label);
		node.SetText(text);
		node.SetKey(GetDocument()->AssignNewKey());
		node.SetParentKey(Tree().GetItemData(Selected()));
		node.MoveBound(mvSz);
		node.SetLevel(curLevel);
		addNodes.push_back(node);
	}

	return true;
}

int OutlineView::GetIndentCount(const CString& line, const char levelChar) const
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
	ImportXmlDlg dlg;
	dlg.m_importMode = 0;
	if (dlg.DoModal() != IDOK) return true;
	bool rep = (dlg.m_importMode == 0);
	return GetDocument()->ImportXml(inPath, rep);
}

void OutlineView::OnEditFind()
{
	m_pSrchDlg->ShowWindow(SW_SHOWNORMAL);
	m_pSrchDlg->SetFocus();
}

void OutlineView::OnUpdateEditFind(CCmdUI* pCmdUI)
{
}

LRESULT OutlineView::OnHideSrchDlg(UINT wParam, LONG lParam)
{
	m_pSrchDlg->ShowWindow(SW_HIDE);
	return 0;
}

LRESULT OutlineView::OnSearchNode(UINT wParam, LONG lParam)
{
	HTREEITEM sItem = FindKeyItem((DWORD)wParam, Tree().GetRootItem());
	Tree().SelectItem(sItem);
	return 0;
}

void OutlineView::OnDestroy()
{
	CTreeView::OnDestroy();

	m_pSrchDlg->DestroyWindow();
}

LRESULT OutlineView::OnListUpNodes(UINT wParam, LONG lParam)
{
	// リストアップ用のデータをNodeSearchDlgに受け渡す
	if (m_pSrchDlg->m_labels.size() > 0) return 0;
	GetDocument()->ListUpNodes(m_pSrchDlg->m_srchString, m_pSrchDlg->m_labels,
		m_pSrchDlg->m_bLabel, m_pSrchDlg->m_bText, m_pSrchDlg->m_bLinks,
		m_pSrchDlg->m_bUpper);
	m_pSrchDlg->ShowResult();
	return 0;
}

void OutlineView::OnEditReplace()
{
}

void OutlineView::HideChildWindow()
{
	m_pSrchDlg->ShowWindow(SW_HIDE);
}

void OutlineView::DisableUndo()
{
	m_hItemMoved = NULL;
	m_hParentPreMove = NULL;
	m_hSiblingPreMove = NULL;
}

void OutlineView::OnSetFoldup()
{
	FoldingSettingsDlg dlg;
	dlg.m_level = 1;
	if (dlg.DoModal() != IDOK) return;
	FoldupTree(Tree().GetRootItem(), 0, dlg.m_level - 1);
	Tree().SelectItem(Tree().GetRootItem());
}

void OutlineView::OnUpdateSetFoldup(CCmdUI* pCmdUI)
{
}

void OutlineView::FoldupTree(HTREEITEM hItem, int curLevel, int levelSet)
{
	if (curLevel <= levelSet) {
		if (Tree().ItemHasChildren(hItem)) {
			Tree().Expand(hItem, TVE_EXPAND);
		}
	}
	else {
		if (Tree().ItemHasChildren(hItem)) {
			Tree().Expand(hItem, TVE_COLLAPSE);
		}
	}
	if (Tree().ItemHasChildren(hItem)) {
		HTREEITEM hChild = Tree().GetNextItem(hItem, TVGN_CHILD);
		FoldupTree(hChild, ++curLevel, levelSet);
	}
	else {
		HTREEITEM hNext = Tree().GetNextItem(hItem, TVGN_NEXT);
		if (hNext == NULL) {
			HTREEITEM hi = hItem;
			HTREEITEM hParent = hItem;
			while (hParent != Tree().GetRootItem()) {
				hParent = Tree().GetParentItem(hi);
				HTREEITEM hnextParent;
				--curLevel;
				if ((hnextParent = Tree().GetNextItem(hParent, TVGN_NEXT)) != NULL) {
					FoldupTree(hnextParent, curLevel, levelSet);
					return;
				}
				hi = hParent;
			}
		}
		else {
			FoldupTree(hNext, curLevel, levelSet);
		}
	}
}


void OutlineView::ApplyColorSetting()
{
	CiEditApp* app = (CiEditApp*)AfxGetApp();
	COLORREF colorBG = app->GetProfileInt(REGS_FRAME, _T("Outline bgColor"), app->m_colorOutlineViewBg);
	COLORREF colorFor = app->GetProfileInt(REGS_FRAME, _T("Outline forColor"), app->m_colorOutlineViewFg);
	COLORREF colorIM = app->GetProfileInt(REGS_FRAME, _T("InsertMark Color"), RGB(0, 0, 0));
	TreeView_SetBkColor(m_hWnd, colorBG);
	TreeView_SetTextColor(m_hWnd, colorFor);
	TreeView_SetInsertMarkColor(m_hWnd, colorIM);
}

void OutlineView::OnAddChild2()
{
	CreateNodeDlg dlg;
	dlg.m_initialPt.x = -1;
	dlg.m_initialPt.y = -1;
	if (dlg.DoModal() != IDOK) return;
	if (dlg.m_strcn == _T("")) return;
	GetDocument()->DisableUndo();
	DisableUndo();
	HTREEITEM newItem = Tree().InsertItem(dlg.m_strcn, 0, 0, Selected());
	m_HNew = newItem;

	iEditDoc* pDoc = GetDocument();
	NodeProps l;
	l.name = dlg.m_strcn;
	l.key = pDoc->AssignNewKey();
	Tree().SetItemData(m_HNew, l.key);
	l.parent = Tree().GetItemData(Tree().GetParentItem(m_HNew));

	DWORD keyInherit = Tree().GetItemData(Tree().GetSelectedItem());

	bool binherit = false;
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	if (pApp->m_rgsNode.bInheritParent) {
		binherit = true;
	}
	pDoc->AddNode(l, keyInherit, binherit);
	Tree().Expand(Selected(), TVE_EXPAND);
	Tree().SelectItem(newItem);
}

void OutlineView::OnUpdateAddChild2(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!Tree().ItemHasChildren(Tree().GetSelectedItem()));
}

void OutlineView::OnShowSelectedBranch()
{
	NodeKeySet ks;
	ks.insert(Tree().GetItemData(Tree().GetSelectedItem()));
	treeview_for_each(Tree(), copy_key_set(ks), Tree().GetChildItem(Selected()));
	GetDocument()->SetVisibleNodes(ks);
	GetDocument()->SetShowBranch(Tree().GetItemData(Selected()));
	int branchMode = GetBranchMode();
	if (branchMode != 0) {
		ResetBranchMode();
	}
	Tree().SetItemImage(Selected(), 2, 2);
	Tree().Expand(Selected(), TVE_EXPAND);
	m_hItemShowRoot = Selected();
	Tree().SelectItem(Tree().GetChildItem(Selected()));
}

void OutlineView::OnUpdateShowSelectedBranch(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(Tree().ItemHasChildren(Selected()));
}

BOOL OutlineView::IsPosterityOF(HTREEITEM hRoot, HTREEITEM hChild) const
{
	HTREEITEM hCurItem = hChild;
	HTREEITEM hParent = hChild;
	while (hParent != Tree().GetRootItem()) {
		hParent = Tree().GetParentItem(hCurItem);
		if (hParent == hRoot) {
			return TRUE;
		}
		hCurItem = hParent;
	}
	return FALSE;
}

int OutlineView::GetBranchMode() const
{
	int index, selectedIndex;
	Tree().GetItemImage(m_hItemShowRoot, index, selectedIndex);
	// 戻り値：0:通常、1:下の階層のみ、2:下全部
	return selectedIndex;
}

void OutlineView::ResetBranchMode()
{
	Tree().SetItemImage(m_hItemShowRoot, 0, 0);
}

void OutlineView::OnShowSelectedChildren()
{
	NodeKeySet ks;
	ks.insert(Tree().GetItemData(Tree().GetSelectedItem()));
	treeview_for_each2(Tree(), copy_key_set(ks), Tree().GetChildItem(Selected()));
	GetDocument()->SetVisibleNodes(ks);
	GetDocument()->SetShowBranch(Tree().GetItemData(Selected()));
	int branchMode = GetBranchMode();
	if (branchMode != 0) {
		ResetBranchMode();
	}
	Tree().SetItemImage(Selected(), 1, 1);
	Tree().Expand(Selected(), TVE_EXPAND);
	m_hItemShowRoot = Selected();
	Tree().SelectItem(Tree().GetChildItem(Selected()));
}

void OutlineView::OnUpdateShowSelectedChildren(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(Tree().ItemHasChildren(Selected()));
}

void OutlineView::RecalcNodeLevels(HTREEITEM hItem, int curLevel)
{
	iEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pDoc->SetNodeLevel(Tree().GetItemData(hItem), curLevel);
	if (Tree().ItemHasChildren(hItem)) {
		HTREEITEM hChild = Tree().GetNextItem(hItem, TVGN_CHILD);
		RecalcNodeLevels(hChild, ++curLevel);
	}
	else {
		HTREEITEM hNext = Tree().GetNextItem(hItem, TVGN_NEXT);
		if (hNext == NULL) {
			HTREEITEM hi = hItem;
			HTREEITEM hParent = hItem;
			while (hParent != Tree().GetRootItem()) {
				hParent = Tree().GetParentItem(hi);
				HTREEITEM hnextParent;
				--curLevel;
				if ((hnextParent = Tree().GetNextItem(hParent, TVGN_NEXT)) != NULL) {
					RecalcNodeLevels(hnextParent, curLevel);
					return;
				}
				hi = hParent;
			}
		}
		else {
			RecalcNodeLevels(hNext, curLevel);
		}
	}
}

void OutlineView::RecalcAllNodeLevels()
{
	RecalcNodeLevels(Tree().GetRootItem(), 0);
}

void OutlineView::RecalcSubNodeLevels()
{
	RecalcNodeLevels(Tree().GetSelectedItem(), 0);
}

void OutlineView::OnDropFirstOrder()
{
	HTREEITEM hNew = Tree().InsertItem(Tree().GetItemText(m_hitemDrag), 0, 0, m_hitemDrop, TVI_FIRST);
	Tree().SetItemData(hNew, Tree().GetItemData(m_hitemDrag));
	int nImage; Tree().GetItemImage(m_hitemDrag, nImage, nImage);
	Tree().SetItemImage(hNew, nImage, nImage);

	GetDocument()->SetKeyNodeParent(Tree().GetItemData(m_hitemDrag), Tree().GetItemData(m_hitemDrop));
	if (Tree().ItemHasChildren(m_hitemDrag)) {
		CopySubNodes(Tree().GetChildItem(m_hitemDrag), hNew);
	}
	GetDocument()->DisableUndo();
	m_hItemMoved = hNew; // Undo Info
	Tree().SelectItem(hNew);
	Tree().DeleteItem(m_hitemDrag);
	Tree().SelectItem(hNew);

	m_bItemDragging = false;
	Tree().SetInsertMark(NULL);
	Tree().SelectDropTarget(NULL);
}

void OutlineView::OnUpdateDropFirstOrder(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bItemDragging == true);
}

void OutlineView::OnDropLevelUp()
{
	HTREEITEM hParent = Tree().GetParentItem(m_hitemDrop);
	HTREEITEM hNew = Tree().InsertItem(Tree().GetItemText(m_hitemDrag), 0, 0, hParent, m_hitemDrop);
	Tree().SetItemData(hNew, Tree().GetItemData(m_hitemDrag));

	int nImage; Tree().GetItemImage(m_hitemDrag, nImage, nImage);
	Tree().SetItemImage(hNew, nImage, nImage);

	GetDocument()->SetKeyNodeParent(Tree().GetItemData(m_hitemDrag), Tree().GetItemData(hParent));
	if (Tree().ItemHasChildren(m_hitemDrag)) {
		CopySubNodes(Tree().GetChildItem(m_hitemDrag), hNew);
	}
	GetDocument()->DisableUndo();
	m_hItemMoved = hNew; // Undo Info
	Tree().SelectItem(hNew);
	Tree().DeleteItem(m_hitemDrag);
	Tree().SelectItem(hNew);

	m_bItemDragging = false;
	Tree().SetInsertMark(NULL);
	Tree().SelectDropTarget(NULL);
}

void OutlineView::OnUpdateDropLevelUp(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_bItemDragging == true && m_hitemDrop != Tree().GetRootItem());
}

void OutlineView::OnCopyTreeToClipboard()
{
	RecalcSubNodeLevels();
	CString strData;
	TreeToText(Tree().GetSelectedItem(), strData);

	// クリップボードにコピー
	if (OpenClipboard()) {
		EmptyClipboard();
		HGLOBAL hClipboardData;
		size_t len = (strData.GetLength() + 1) * sizeof(TCHAR);
		hClipboardData = GlobalAlloc(GMEM_DDESHARE, len);

		TCHAR* pchData;
		pchData = (TCHAR*)GlobalLock(hClipboardData);
		CopyMemory(pchData, strData, len);
		GlobalUnlock(hClipboardData);
		SetClipboardData(CF_UNICODETEXT, hClipboardData);
		CloseClipboard();
	}

}

void OutlineView::TreeToText(HTREEITEM hItem, CString &text)
{
	if (Tree().GetPrevSiblingItem(hItem) == Tree().GetSelectedItem()) {
		return;
	}
	DWORD key = Tree().GetItemData(hItem);
	int level = GetDocument()->GetKeyNodeLevelNumber(key);
	if (level != -1) {
		for (int i = 1; i <= level; i++) {
			text += _T("\t");
		}
		CString label = GetDocument()->GetKeyNodeLabel(key);
		text += StringUtil::RemoveCr(label);
		text += _T("\r\n");
	}
	if (Tree().ItemHasChildren(hItem)) {
		HTREEITEM hchildItem = Tree().GetNextItem(hItem, TVGN_CHILD);
		TreeToText(hchildItem, text);
	}
	else {
		HTREEITEM hnextItem = Tree().GetNextItem(hItem, TVGN_NEXT);
		if (hnextItem == NULL) {
			HTREEITEM hi = hItem;
			HTREEITEM hParent = hItem;
			while (hParent != Tree().GetSelectedItem()) {
				hParent = Tree().GetParentItem(hi);
				HTREEITEM hnextParent = Tree().GetNextItem(hParent, TVGN_NEXT);
				if (hnextParent != NULL) {
					TreeToText(hnextParent, text);
					return;
				}
				hi = hParent;
			}
		}
		else {
			TreeToText(hnextItem, text);
		}
	}
}

void OutlineView::OnUpdateCopyTreeToClipboard(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(Tree().ItemHasChildren(Tree().GetSelectedItem()));
}

void OutlineView::MoveNodes(DWORD keyTarget, DWORD keyMove)
{
	HTREEITEM hTarget;
	if (keyTarget != -1) {
		hTarget = FindKeyItem(keyTarget);
	}
	else {
		hTarget = this->m_hItemShowRoot;
	}
	HTREEITEM hMove = FindKeyItem(keyMove);
	if (hTarget == NULL || hMove == NULL) return;
	if (IsChildNodeOf(hTarget, hMove)) return;
	if (Tree().GetParentItem(hMove) == hTarget) return;
	HTREEITEM hNew = Tree().InsertItem(Tree().GetItemText(hMove), 0, 0, hTarget);
	Tree().SetItemState(hNew, Tree().GetItemState(hMove, TVIS_EXPANDED), TVIS_EXPANDED);
	Tree().SetItemData(hNew, Tree().GetItemData(hMove));
	int nImage; Tree().GetItemImage(hMove, nImage, nImage);
	Tree().SetItemImage(hNew, nImage, nImage);

	Tree().Expand(hTarget, TVE_EXPAND);
	GetDocument()->SetKeyNodeParent(keyMove, Tree().GetItemData(hTarget));
	if (Tree().ItemHasChildren(hMove)) {
		CopySubNodes(Tree().GetChildItem(hMove), hNew);
	}
	GetDocument()->DisableUndo();
	Tree().SelectItem(hNew);
	Tree().DeleteItem(hMove);
	Tree().SelectItem(hNew);
}

void OutlineView::OnCreateClone()
{
	// TODO: Add your command handler code here
	DWORD key = Tree().GetItemData(Tree().GetSelectedItem());
	CString label = GetDocument()->GetKeyNodeLabel(key);
	DWORD newKey = GetDocument()->DuplicateKeyNode(key);
	HTREEITEM hSelected = Tree().GetSelectedItem();
	HTREEITEM hNew = Tree().InsertItem(label, Tree().GetParentItem(hSelected), hSelected);
	Tree().SetItemData(hNew, newKey);
	int nImage; Tree().GetItemImage(hSelected, nImage, nImage);
	Tree().SetItemImage(hNew, nImage, nImage);

	Tree().SetItemState(hNew, Tree().GetItemState(hSelected, TVIS_EXPANDED), TVIS_EXPANDED);
	NodeKeyMap idm;
	idm[key] = newKey;
	if (Tree().ItemHasChildren(hSelected)) {
		CloneTree(Tree().GetChildItem(hSelected), hNew, idm);
	}
	GetDocument()->DuplicateLinks(idm);
	// 指定配下のノードを全部見せるモードの場合、クローンした一連のノードとリンクをvisibleに
	if (GetDocument()->ShowSubBranch()) {
		NodeKeySet ks;
		ks.insert(Tree().GetItemData(m_hItemShowRoot));
		treeview_for_each(Tree(), copy_key_set(ks), Tree().GetChildItem(m_hItemShowRoot));
		iEditDoc* pDoc = GetDocument();
		pDoc->SetVisibleNodes(ks);
		pDoc->SetShowBranch(Tree().GetItemData(m_hItemShowRoot));
	}
	// 既存のノードと重ならないようにずらす
	Tree().SelectItem(hNew);
}

void OutlineView::OnUpdateCreateClone(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(Tree().GetSelectedItem() != Tree().GetRootItem());
}

void OutlineView::CloneTree(const HTREEITEM& curItem, HTREEITEM targetParent, NodeKeyMap& idm)
{
	HTREEITEM hItem, item;
	item = curItem;
	while (item != NULL) {
		hItem = item;
		DWORD key = Tree().GetItemData(hItem);
		CString label = GetDocument()->GetKeyNodeLabel(key);
		DWORD newKey = GetDocument()->DuplicateKeyNode(key);
		GetDocument()->SetKeyNodeParent(newKey, Tree().GetItemData(targetParent));
		HTREEITEM hNew = Tree().InsertItem(label, 0, 0, targetParent);
		Tree().SetItemData(hNew, newKey);
		int nImage; Tree().GetItemImage(hItem, nImage, nImage);
		Tree().SetItemImage(hNew, nImage, nImage);

		Tree().SetItemState(hNew, Tree().GetItemState(hItem, TVIS_EXPANDED), TVIS_EXPANDED);
		idm[key] = newKey;
		if (Tree().ItemHasChildren(hItem)) {
			CloneTree(Tree().GetChildItem(hItem), hNew, idm);
		}
		item = Tree().GetNextSiblingItem(item);
	}
}

void OutlineView::OnResetShowSubbranch()
{
	Tree().SelectItem(m_hItemShowRoot);
	ResetBranchMode();
	GetDocument()->ResetShowBranch();
}

void OutlineView::OnUpdateResetShowSubbranch(CCmdUI *pCmdUI)
{
	int mode = GetBranchMode();
	pCmdUI->Enable(mode == 1 || mode == 2);
}

void OutlineView::OnTreeImageChcek()
{
	Tree().SetItemImage(Selected(), OutlineView::check, OutlineView::check);
	GetDocument()->SetSelectedNodeTreeIconId(OutlineView::check);
}

void OutlineView::OnUpdateTreeImageChcek(CCmdUI *pCmdUI)
{
	int nImage;
	Tree().GetItemImage(Selected(), nImage, nImage);
	pCmdUI->Enable(!GetDocument()->IsOldBinary() &&
		Tree().GetSelectedItem() != m_hItemShowRoot &&
		nImage != OutlineView::check &&
		Selected() != Tree().GetRootItem());
}

void OutlineView::OnTreeImageBlue()
{
	Tree().SetItemImage(Selected(), OutlineView::blue, OutlineView::blue);
	GetDocument()->SetSelectedNodeTreeIconId(OutlineView::blue);
}

void OutlineView::OnUpdateTreeImageBlue(CCmdUI *pCmdUI)
{
	int nImage;
	Tree().GetItemImage(Selected(), nImage, nImage);
	pCmdUI->Enable(!GetDocument()->IsOldBinary() &&
		Tree().GetSelectedItem() != m_hItemShowRoot &&
		nImage != OutlineView::blue &&
		Selected() != Tree().GetRootItem());
}

void OutlineView::OnTreeImageRed()
{
	Tree().SetItemImage(Selected(), OutlineView::red, OutlineView::red);
	GetDocument()->SetSelectedNodeTreeIconId(OutlineView::red);
}

void OutlineView::OnUpdateTreeImageRed(CCmdUI *pCmdUI)
{
	int nImage;
	Tree().GetItemImage(Selected(), nImage, nImage);
	pCmdUI->Enable(!GetDocument()->IsOldBinary() &&
		Tree().GetSelectedItem() != m_hItemShowRoot &&
		nImage != OutlineView::red &&
		Selected() != Tree().GetRootItem());
}

void OutlineView::OnTreeImageYealow()
{
	Tree().SetItemImage(Selected(), OutlineView::yellow, OutlineView::yellow);
	GetDocument()->SetSelectedNodeTreeIconId(OutlineView::yellow);
}

void OutlineView::OnUpdateTreeImageYealow(CCmdUI *pCmdUI)
{
	int nImage;
	Tree().GetItemImage(Selected(), nImage, nImage);
	pCmdUI->Enable(!GetDocument()->IsOldBinary() &&
		Tree().GetSelectedItem() != m_hItemShowRoot &&
		nImage != OutlineView::yellow &&
		Selected() != Tree().GetRootItem());
}

void OutlineView::OnTreeImageCancel()
{
	Tree().SetItemImage(Selected(), OutlineView::cancel, OutlineView::cancel);
	GetDocument()->SetSelectedNodeTreeIconId(OutlineView::cancel);
}

void OutlineView::OnUpdateTreeImageCancel(CCmdUI *pCmdUI)
{
	int nImage;
	Tree().GetItemImage(Selected(), nImage, nImage);
	pCmdUI->Enable(!GetDocument()->IsOldBinary() &&
		Tree().GetSelectedItem() != m_hItemShowRoot &&
		nImage != OutlineView::cancel &&
		Selected() != Tree().GetRootItem());
}

void OutlineView::OnTreeImageQuestion()
{
	Tree().SetItemImage(Selected(), OutlineView::question, OutlineView::question);
	GetDocument()->SetSelectedNodeTreeIconId(OutlineView::question);
}

void OutlineView::OnUpdateTreeImageQuestion(CCmdUI *pCmdUI)
{
	int nImage;
	Tree().GetItemImage(Selected(), nImage, nImage);
	pCmdUI->Enable(!GetDocument()->IsOldBinary() &&
		Tree().GetSelectedItem() != m_hItemShowRoot &&
		nImage != OutlineView::question &&
		Selected() != Tree().GetRootItem());
}

void OutlineView::OnTreeImageWarning()
{
	Tree().SetItemImage(Selected(), OutlineView::warning, OutlineView::warning);
	GetDocument()->SetSelectedNodeTreeIconId(OutlineView::warning);
}

void OutlineView::OnUpdateTreeImageWarning(CCmdUI *pCmdUI)
{
	int nImage;
	Tree().GetItemImage(Selected(), nImage, nImage);
	pCmdUI->Enable(!GetDocument()->IsOldBinary() &&
		Tree().GetSelectedItem() != m_hItemShowRoot &&
		nImage != OutlineView::warning &&
		Selected() != Tree().GetRootItem());
}

void OutlineView::OnTreeImageFace()
{
	Tree().SetItemImage(Selected(), OutlineView::face, OutlineView::face);
	GetDocument()->SetSelectedNodeTreeIconId(OutlineView::face);
}

void OutlineView::OnUpdateTreeImageFace(CCmdUI *pCmdUI)
{
	int nImage;
	Tree().GetItemImage(Selected(), nImage, nImage);
	pCmdUI->Enable(!GetDocument()->IsOldBinary() &&
		Tree().GetSelectedItem() != m_hItemShowRoot &&
		nImage != OutlineView::face &&
		Selected() != Tree().GetRootItem());
}

void OutlineView::OnTreeImageIdea()
{
	Tree().SetItemImage(Selected(), OutlineView::idea, OutlineView::idea);
	GetDocument()->SetSelectedNodeTreeIconId(OutlineView::idea);
}

void OutlineView::OnUpdateTreeImageIdea(CCmdUI *pCmdUI)
{
	int nImage;
	Tree().GetItemImage(Selected(), nImage, nImage);
	pCmdUI->Enable(!GetDocument()->IsOldBinary() &&
		Tree().GetSelectedItem() != m_hItemShowRoot &&
		nImage != OutlineView::idea &&
		Selected() != Tree().GetRootItem());
}

void OutlineView::OnPasteTreeFromClipboard()
{
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

	int mode = GetBranchMode();
	HTREEITEM hShowRoot;
	if (mode != 0) {
		hShowRoot = m_hItemShowRoot;
		ResetBranchMode();
		GetDocument()->ResetShowBranch();
	}

	ClipText += _T("\n");
	int pos = 0;
	vector<CString> lines;
	for (CString token = ClipText.Tokenize(_T("\n"), pos); !token.IsEmpty(); ) {
		lines.push_back(token);
		token = ClipText.Tokenize(_T("\n"), pos);
	}

	node_vec addNodes;

	ImportTextDlg dlg;
	dlg.m_fileName = _T("<クリップボードから>");
	dlg.m_charSelection = 1;
	if (dlg.DoModal() != IDOK) {
		return;
	}
	int levelChar = '.';
	if (dlg.m_charSelection == 1) {
		levelChar = '\t';
	}

	if (AddTreeAcordingToLevel(lines, addNodes, levelChar)) {
		AddBranch2(Tree().GetItemData(Selected()), addNodes);
	}
	if (mode == 1) {
		Tree().SelectItem(hShowRoot);
		OnShowSelectedChildren();
	}
	else if (mode == 2) {
		Tree().SelectItem(hShowRoot);
		OnShowSelectedBranch();
	}
}

void OutlineView::OnUpdatePasteTreeFromClipboard(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(::IsClipboardFormatAvailable(CF_TEXT));
}

void OutlineView::OnExportToHtml()
{
	OutputHtml();
}

void OutlineView::OnUpdateExportToHtml(CCmdUI *pCmdUI)
{
}

void OutlineView::OnExportToText()
{	
	ExportTextDlg dlg;
	dlg.m_rdTreeOption = m_textExportOption.treeOption;
	dlg.m_rdFormatOption = m_textExportOption.formatOption;
	dlg.m_rdChapterNumberOption = m_textExportOption.chapterNumberOption;
	dlg.m_excludeLabelFromFileName = m_textExportOption.excludeLabelFromFileName;
	dlg.m_excludeLabelFromContent = m_textExportOption.excludeLabelFromContent;
	if (dlg.DoModal() != IDOK) return;
	m_textExportOption.treeOption = dlg.m_rdTreeOption;
	m_textExportOption.formatOption = dlg.m_rdFormatOption;
	m_textExportOption.chapterNumberOption = dlg.m_rdChapterNumberOption;
	m_textExportOption.excludeLabelFromFileName = dlg.m_excludeLabelFromFileName;
	m_textExportOption.excludeLabelFromContent = dlg.m_excludeLabelFromContent;

	if (m_textExportOption.chapterNumberOption != 0) {
		AssignChapterNumbers();
	}

	CString outfile = GetDocument()->GetFileNameFromPath();
	if (dlg.m_rdTreeOption != 0) {
		CString label = StringUtil::GetSafeFileName(Tree().GetItemText(Tree().GetSelectedItem()));
		if (label != _T("")) {
			outfile = label;
		}
	}

	if (m_textExportOption.formatOption != 2) {
		CString ext = _T("txt");
		CString filters = _T("テキストファイル (*.txt)|*.txt");
		if (m_textExportOption.formatOption == 3) {
			ext = _T("md");
			filters = _T("Marddown ファイル (*.md)|*.md");
		}
		else if (m_textExportOption.formatOption == 4) {
			ext = _T("org");
			filters = _T("OrgMode ファイル (*.org)|*.org");
		}
		CFileDialog fdlg(FALSE, ext, outfile, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, filters, this);
		if (fdlg.DoModal() != IDOK) return;
		CString outfileName = fdlg.GetPathName();

		FILE* fp;
		if ((fp = FileUtil::CreateStdioFile(outfileName)) == NULL) return;
		CStdioFile f(fp);
		_wsetlocale(LC_ALL, _T("jpn"));
		if (dlg.m_rdTreeOption == 0) {
			OutputOutlineText(Tree().GetRootItem(), &f, 0);
		}
		else {
			if (dlg.m_rdChapterNumberOption == 0) {
				f.WriteString(_T("."));
			}
			f.WriteString(StringUtil::RemoveCr(Tree().GetItemText(Tree().GetSelectedItem())) + _T("\n"));
			if (dlg.m_rdFormatOption != 1) {
				f.WriteString(StringUtil::ReplaceCrToLf(GetDocument()->GetKeyNodeText(Tree().GetItemData(Tree().GetSelectedItem()))));
				f.WriteString(_T("\n"));
			}
			if (Tree().ItemHasChildren(Tree().GetSelectedItem())) {
				OutputOutlineText(Tree().GetChildItem(Tree().GetSelectedItem()), &f, 1);
			}
		}
		_wsetlocale(LC_ALL, _T(""));
		f.Close();
		if (((CiEditApp*)AfxGetApp())->m_rgsOther.bOpenFilesAfterExport && ext == _T("txt")) {
			ShellExecute(m_hWnd, _T("open"), outfileName, NULL, NULL, SW_SHOW);
		}
		else {
			MessageBox(outfileName + _T(" に出力しました。"));
		}
	}
	else {
		CString defaultPath = AfxGetApp()->GetProfileString(_T("Settings"), _T("Text OutputDir"), _T(""));
		if (!FileUtil::SelectFolder(m_textExportOption.outDir, defaultPath, m_hWnd)) return;

		if (dlg.m_rdTreeOption == 0) {
			OutputOutlineTextByNode(Tree().GetRootItem());
		}
		else {
			OutputOutlineTextByNode(Tree().GetSelectedItem());
		}
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("Text OutputDir"), m_textExportOption.outDir);

		MessageBox(m_textExportOption.outDir + _T("に出力しました。"));
	}
}

void OutlineView::OutputOutlineTextByNode(HTREEITEM hItem)
{
	if (Tree().GetPrevSiblingItem(hItem) == Tree().GetSelectedItem() && m_textExportOption.treeOption != 0) {
		return;
	}

	CString chapNum = GetDocument()->GetKeyNodeChapterNumber(Tree().GetItemData(hItem));
	CString label = chapNum + " " + StringUtil::GetSafeFileName(StringUtil::RemoveMachineDependentChar(Tree().GetItemText(hItem)));
	label = StringUtil::RemoveCr(label);
	CString text = StringUtil::RemoveMachineDependentChar(GetDocument()->GetKeyNodeText(Tree().GetItemData(hItem)));
	CreateNodeTextFile(label, text);

	if (Tree().ItemHasChildren(hItem)) {
		HTREEITEM hchildItem = Tree().GetNextItem(hItem, TVGN_CHILD);
		OutputOutlineTextByNode(hchildItem);
	}
	else {
		HTREEITEM hnextItem = Tree().GetNextItem(hItem, TVGN_NEXT);
		if (hnextItem == NULL) {    // 次に兄弟がいない
			HTREEITEM hi = hItem;
			HTREEITEM hParent = hItem;
			while (hParent != Tree().GetRootItem()) {
				hParent = Tree().GetParentItem(hi);
				HTREEITEM hnextParent;
				if ((hnextParent = Tree().GetNextItem(hParent, TVGN_NEXT)) != NULL) {
					OutputOutlineTextByNode(hnextParent);
					return;
				}
				hi = hParent;
			}                                   // 兄弟のいる親まで戻る
		}
		else {
			OutputOutlineTextByNode(hnextItem);
		}                                       // 兄弟に移動
	}
}

void OutlineView::CreateNodeTextFile(const CString& title, const CString& text) {

	CString path = m_textExportOption.outDir + _T("\\") + title + _T(".txt");
	FILE* fp;
	if ((fp = FileUtil::CreateStdioFile(path)) == NULL) return;
	CStdioFile f(fp);
	_wsetlocale(LC_ALL, _T("jpn"));
	if (!m_textExportOption.excludeLabelFromContent) {
		f.WriteString(title);
		f.WriteString(_T("\n\n"));
	}
	f.WriteString(text);
	_wsetlocale(LC_ALL, _T(""));
	f.Close();
}

void OutlineView::AssignChapterNumbers() {
	char separator = '-';
	if (m_textExportOption.chapterNumberOption == 2) {
		separator = '.';
	}
	vector<int> numbers;
	HTREEITEM hItem = Tree().GetRootItem();
	if (m_textExportOption.treeOption != 0) {
		hItem = Tree().GetSelectedItem();
	}
	AssignChapterNumber(numbers, separator, hItem);
}

void OutlineView::AssignChapterNumber(vector<int>& numbers, const char separator, HTREEITEM hItem) {

	if (Tree().GetPrevSiblingItem(hItem) == Tree().GetSelectedItem() && m_textExportOption.treeOption != 0) {
		return;
	}

	if (Tree().GetPrevSiblingItem(hItem) != NULL && numbers.size() > 0) {
		numbers.back()++;
	}
	CString chapNum;
	vector<int>::iterator it = numbers.begin();
	for (; it != numbers.end(); it++)
	{
		CString s;
		s.Format(_T("%d%c"), (*it), separator);
		chapNum += s;
	}
	chapNum = chapNum.TrimRight(separator);
	GetDocument()->SetKeyNodeChapterNumber(Tree().GetItemData(hItem), chapNum);

	if (Tree().ItemHasChildren(hItem) && m_textExportOption.treeOption != 2) {
		HTREEITEM hchildItem = Tree().GetNextItem(hItem, TVGN_CHILD);
		numbers.push_back(1);
		AssignChapterNumber(numbers, separator, hchildItem);
	}
	else {
		HTREEITEM hNextItem = Tree().GetNextItem(hItem, TVGN_NEXT);
		if (hNextItem == NULL) {
			HTREEITEM hi = hItem;
			HTREEITEM hParent = hItem;
			while (hParent != Tree().GetRootItem()) {
				hParent = Tree().GetParentItem(hi);
				HTREEITEM hNextParent;
				if (numbers.size() != 0) {
					numbers.pop_back();
				}
				if ((hNextParent = Tree().GetNextItem(hParent, TVGN_NEXT)) != NULL) {
					AssignChapterNumber(numbers, separator, hNextParent);
					return;
				}
				hi = hParent;
			}
		}
		else {
			AssignChapterNumber(numbers, separator, hNextItem);
		}
	}
}

void OutlineView::OnUpdateExportToText(CCmdUI *pCmdUI)
{
}

void OutlineView::OnExportToXml()
{
	ExportXmlDlg dlg;
	dlg.m_nTreeOp = m_exportOption.treeOption;
	if (dlg.DoModal() != IDOK) return;
	m_opTreeOut = dlg.m_nTreeOp;
	m_exportOption.treeOption = dlg.m_nTreeOp;

	CString outfile = GetDocument()->GetFileNameFromPath();
	if (dlg.m_nTreeOp != 0) {
		CString label = StringUtil::GetSafeFileName(Tree().GetItemText(Tree().GetSelectedItem()));
		if (label != _T("")) {
			outfile = label;
		}
	}
	WCHAR szFilters[] = _T("XML ファイル (*.xml)|*.xml");
	CFileDialog fdlg(FALSE, _T("xml"), outfile, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, this);
	if (fdlg.DoModal() != IDOK) return;
	CString outfileName = fdlg.GetPathName();
	_wsetlocale(LC_ALL, _T("jpn"));

	if (GetDocument()->SaveXml(outfileName)) {
		MessageBox(_T("終了しました"), _T("XML へのエクスポート"), MB_OK);
	}
	_wsetlocale(LC_ALL, _T(""));
}

void OutlineView::OnUpdateExportToXml(CCmdUI *pCmdUI)
{
}


void OutlineView::OnExportToJson()
{
	ExportXmlDlg dlg;
	dlg.m_nTreeOp = m_exportOption.treeOption;
	if (dlg.DoModal() != IDOK) return;
	m_opTreeOut = dlg.m_nTreeOp;
	m_exportOption.treeOption = dlg.m_nTreeOp;

	CString outfile = GetDocument()->GetFileNameFromPath();
	if (dlg.m_nTreeOp != 0) {
		CString label = StringUtil::GetSafeFileName(Tree().GetItemText(Tree().GetSelectedItem()));
		if (label != _T("")) {
			outfile = label;
		}
	}
	WCHAR szFilters[] = _T("JSON ファイル (*.json)|*.json");
	CFileDialog fdlg(FALSE, _T("json"), outfile, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, this);
	if (fdlg.DoModal() != IDOK) return;
	CString outfileName = fdlg.GetPathName();
	_wsetlocale(LC_ALL, _T("jpn"));

	if (GetDocument()->SaveJson(outfileName)) {
		MessageBox(_T("終了しました"), _T("JSON へのエクスポート"), MB_OK);
	}
	_wsetlocale(LC_ALL, _T(""));
}


void OutlineView::OnUpdateExportToJson(CCmdUI *pCmdUI)
{
}
