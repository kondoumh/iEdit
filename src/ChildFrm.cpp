// ChildFrm.cpp : CChildFrame クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "iEdit.h"

#include "ChildFrm.h"

#include "iEditDoc.h"
#include "OutlineView.h"
#include "EditorView.h"
#include "NetView.h"
#include "LinkView.h"

#include "OptionSettingsDlg.h"
#include "NodeSearchDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGS_FRAME _T("Frame Options")
#define REGS_SHAPES _T("Shapes")

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_CLOSE()
	ON_COMMAND(ID_CHANGE_VIEW_FOCUS, OnChangeViewFocus)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_VIEW_FOCUS, OnUpdateChangeViewFocus)
	ON_WM_MDIACTIVATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame クラスの構築/消滅

CChildFrame::CChildFrame()
{
	// TODO: メンバ初期化コードをこの位置に追加してください。
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: この位置で CREATESTRUCT cs の設定を行って、Window クラスまたは
	//       スタイルを変更してください。
	cs.style |= WS_CHILD | WS_VISIBLE |WS_OVERLAPPEDWINDOW | WS_MAXIMIZE;
	
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;
	
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame クラスの診断

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame クラスのメッセージハンドラ

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	long cxLeft = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Outline Width"), lpcs->cx*1/3);
	long cyTree = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Outline Height"), lpcs->cy*2/3);
	long cyNet = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Net Height"), lpcs->cy*1/2);
	
	m_Splitter1.CreateStatic(this, 1, 2);
//	m_Splitter1.SetColumnInfo(0, cxLeft, 0);
	m_Splitter2.CreateStatic(&m_Splitter1, 2, 1, WS_CHILD | WS_VISIBLE | WS_BORDER, m_Splitter1.IdFromRowCol(0, 0));
	m_Splitter2.CreateView(0, 0, RUNTIME_CLASS(OutlineView), CSize(cxLeft, cyTree), pContext);
	m_Splitter2.CreateView(1, 0, RUNTIME_CLASS(LinkView), CSize(cxLeft, 0), pContext);
	m_Splitter3.CreateStatic(&m_Splitter1, 2, 1, WS_CHILD | WS_VISIBLE | WS_BORDER, m_Splitter1.IdFromRowCol(0, 1));
	m_Splitter3.CreateView(0, 0, RUNTIME_CLASS(NetView), CSize(0, cyNet), pContext);
	m_Splitter3.CreateView(1, 0, RUNTIME_CLASS(EditorView), CSize(0, 0), pContext);
	
	m_Splitter1.SetColumnInfo(0, cxLeft, 0);
	m_Splitter1.RecalcLayout();
	m_Splitter2.SetRowInfo(0, cyTree, 0);
	m_Splitter2.RecalcLayout();
	m_Splitter3.SetRowInfo(0, cyNet, 0);
	m_Splitter3.RecalcLayout();
	return TRUE;
}

BOOL CChildFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	if (CMDIChildWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) {
		return TRUE;
	}
	iEditDoc* pDoc = (iEditDoc*)GetActiveDocument();
	ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(iEditDoc)));
	
	// コマンドフィルタ (全部のビューにルーティングしたくないときはここに追加する
	if (nID == ID_INSERT_SIBLING || nID == ID_INSERT_CHILD) {
		return FALSE;
	}
	
	if (pDoc != NULL) {
		if (pDoc->RouteCmdToAllViews(GetActiveView(), nID, nCode, pExtra, pHandlerInfo)) {
			return TRUE;
		}
	}
	return FALSE;
}

/*void CChildFrame::OnSetProperties() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	OptionSettingsDlg dlg;
	if (dlg.DoModal() != IDOK) return;
	
	// 各ビューの取得
	iEditDoc* pDoc = (iEditDoc*)GetActiveView()->GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	OutlineView* pOutlineView = (OutlineView*)pDoc->GetNextView(pos);
	LinkView* pLinkView = (LinkView*)pDoc->GetNextView(pos);
	NetView* pNetView = (NetView*)pDoc->GetNextView(pos);
	EditorView* pEditorView = (EditorView*)pDoc->GetNextView(pos);
	
	// 各ビューの再設定
	pOutlineView->setViewFont();
	pLinkView->setViewFont();
	pEditorView->setViewFont();
	
	pEditorView->setTabStop();
}*/


void CChildFrame::OnDestroy() 
{
	CMDIChildWnd::OnDestroy();
	
	// TODO: この位置にメッセージ ハンドラ用のコードを追加してください
	CMDIFrameWnd* pFrame = GetMDIFrame();
	WINDOWPLACEMENT wndplm;
	pFrame->GetWindowPlacement(&wndplm);
	
	BOOL saveFrame = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Save Frame Sizes"), TRUE);
	if (saveFrame && 
		  (wndplm.showCmd == SW_SHOWNORMAL || wndplm.showCmd == SW_SHOWMAXIMIZED)) {
		WINDOWPLACEMENT wndpl;
		GetWindowPlacement(&wndpl);
		int cur1, min1, cur2, min2, cur3, min3;
		m_Splitter1.GetColumnInfo(0, cur1, min1);
		m_Splitter2.GetRowInfo(0, cur2, min2);
		m_Splitter3.GetRowInfo(0, cur3, min3);
		if (cur1 > 0 && cur1 < 1600 && cur2 > 0 && cur2 <1600 && cur3 > 0 && cur3 < 1600 && 
			wndpl.showCmd == SW_SHOWMAXIMIZED) {
			AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Outline Width"), cur1);
			AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Outline Height"), cur2);
			AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Net Height"), cur3);
		}
	}
}


void CChildFrame::OnClose() 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	CMDIChildWnd::OnClose();
}

void CChildFrame::OnChangeViewFocus() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	iEditDoc* pDoc = (iEditDoc*)GetActiveView()->GetDocument();
	
	POSITION pos = pDoc->GetFirstViewPosition();
	CView* pfirstView = pDoc->GetNextView(pos);
	CView* psecondView = pDoc->GetNextView(pos);
	CView* pthirdView = pDoc->GetNextView(pos);
	CView* pforthView = pDoc->GetNextView(pos);
	
	CView* pcurView = GetActiveView();
	if (pcurView == pfirstView) {
		SetActiveView(pthirdView, TRUE);
	} else if (pcurView == psecondView) {
		SetActiveView(pfirstView, TRUE);
	} else if (pcurView == pthirdView) {
		SetActiveView(pforthView);
	} else if (pcurView == pforthView) {
		SetActiveView(psecondView);
	}
}

void CChildFrame::OnUpdateChangeViewFocus(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	
}

void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd) 
{
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
	
	// TODO: この位置にメッセージ ハンドラ用のコードを追加してください
	if (!bActivate) {
		CView* pView = GetActiveView();
		if (pView == NULL) return;
		iEditDoc* pDoc = (iEditDoc*)pView->GetDocument();
		POSITION pos = pDoc->GetFirstViewPosition();
		OutlineView* pOutlineView = (OutlineView*)pDoc->GetNextView(pos);
		LinkView* pLinkView = (LinkView*)pDoc->GetNextView(pos);
		NetView* pNetView = (NetView*)pDoc->GetNextView(pos);
		EditorView* pEditorView = (EditorView*)pDoc->GetNextView(pos);
		pOutlineView->hideModeless();
		pNetView->hideModeless();
	}
}

void CChildFrame::changeSelectedLinkArrow()
{
	iEditDoc* pDoc = (iEditDoc*)GetActiveView()->GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	pDoc->GetNextView(pos);
	pDoc->GetNextView(pos);
	NetView* pNetView = (NetView*)pDoc->GetNextView(pos);
	pNetView->changeSelectedLinkArrow();
}

void CChildFrame::changeSelectedLineWidth()
{
	iEditDoc* pDoc = (iEditDoc*)GetActiveView()->GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	pDoc->GetNextView(pos);
	pDoc->GetNextView(pos);
	NetView* pNetView = (NetView*)pDoc->GetNextView(pos);
	pNetView->changeSelectedLineWidth();
}

void CChildFrame::changeSelectedNodeColor(void)
{
	iEditDoc* pDoc = (iEditDoc*)GetActiveView()->GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	pDoc->GetNextView(pos);
	pDoc->GetNextView(pos);
	NetView* pNetView = (NetView*)pDoc->GetNextView(pos);
	pNetView->changeSelectedNodeColor();
}

void CChildFrame::changeSelectedLineColor(void)
{
	iEditDoc* pDoc = (iEditDoc*)GetActiveView()->GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	pDoc->GetNextView(pos);
	pDoc->GetNextView(pos);
	NetView* pNetView = (NetView*)pDoc->GetNextView(pos);
	pNetView->changeSelectedLineColor();
}

void CChildFrame::changeSelectedFontColor(void)
{
	iEditDoc* pDoc = (iEditDoc*)GetActiveView()->GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	pDoc->GetNextView(pos);
	pDoc->GetNextView(pos);
	NetView* pNetView = (NetView*)pDoc->GetNextView(pos);
	pNetView->changeSelectedFontColor();
}
