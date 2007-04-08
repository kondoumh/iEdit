// NetView.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "NetView.h"
#include "iEditDoc.h"
#include "InpcnDlg.h"
#include "LinkInfoDlg.h"
#include "nodePropDlg.h"
#include "OnProcDlg.h"
#include "RelaxThrd.h"
#include "randAreaDlg.h"
#include "shapesDlg.h"
#include "MainFrm.h"
#include "MfSizer.h"
#include "Token.h"
#include "imm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGS_FRAME _T("Frame Options")
#define TILT_TIMER 1234

/////////////////////////////////////////////////////////////////////////////
// NetView
#define REGS_OTHER _T("Settings")
#define REGS_SHAPES _T("Shapes")

IMPLEMENT_DYNCREATE(NetView, CScrollView)

NetView::NetView()
{
	m_hLinkCsr = AfxGetApp()->LoadCursor(IDC_LINK);
	m_hRectCsr = AfxGetApp()->LoadCursor(IDC_RECT);
	m_hRRectCsr = AfxGetApp()->LoadCursor(IDC_RRECT);
	m_hArcCsr = AfxGetApp()->LoadCursor(IDC_ARC);
	m_hHandOpen = AfxGetApp()->LoadCursor(IDC_HAND_OPEN);
	m_hHandClose = AfxGetApp()->LoadCursor(IDC_HAND_CLOSE);
	m_hLayout = AfxGetApp()->LoadCursor(IDC_LAYOUT);
	m_hZoomCsr = AfxGetApp()->LoadCursor(IDC_ZOOM);
	m_hLabelCsr = AfxGetApp()->LoadCursor(IDC_LABELONLY);
	m_bLayouting = false;
	m_pShapesDlg = NULL;
	m_fZoomScale = 1.0f; // 0.0fよりよいんじゃ？
	m_fZoomScalePrev = 1.0f;
	
	m_selectStatus = NetView::none;
	m_addMode = NetView::normal;
	m_bStartAdd = false;
	m_bLinkCurving = false;
	m_bGrasp = false;
	m_bDragView = false;
	m_bAlteringLinkFrom = false;
	m_bAlteringLinkTo = false;
	m_bZooming = FALSE;
	m_ptPaste = CPoint(0, 0);
	m_ptScreen = CPoint(0, 0);
	m_cntUp = 0; m_cntDown = 0; m_cntLeft = 0; m_cntRight = 0;
	m_bDragRelax = false;
}

NetView::~NetView()
{
}


BEGIN_MESSAGE_MAP(NetView, CScrollView)
	//{{AFX_MSG_MAP(NetView)
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_ADD_LINK0, OnAddLink0)
	ON_UPDATE_COMMAND_UI(ID_ADD_LINK0, OnUpdateAddLink0)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(ID_SET_NODE_FONT, OnSetNodeFont)
	ON_UPDATE_COMMAND_UI(ID_SET_NODE_FONT, OnUpdateSetNodeFont)
	ON_COMMAND(ID_SET_NODE_LINE_COLOR, OnSetNodeLineColor)
	ON_UPDATE_COMMAND_UI(ID_SET_NODE_LINE_COLOR, OnUpdateSetNodeLineColor)
	ON_COMMAND(ID_SET_LINE_SOLID_0, OnSetLineSolid0)
	ON_UPDATE_COMMAND_UI(ID_SET_LINE_SOLID_0, OnUpdateSetLineSolid0)
	ON_COMMAND(ID_SET_LINE_SOLID_1, OnSetLineSolid1)
	ON_UPDATE_COMMAND_UI(ID_SET_LINE_SOLID_1, OnUpdateSetLineSolid1)
	ON_COMMAND(ID_SET_LINE_SOLID_2, OnSetLineSolid2)
	ON_UPDATE_COMMAND_UI(ID_SET_LINE_SOLID_2, OnUpdateSetLineSolid2)
	ON_COMMAND(ID_SET_LINE_SOLID_3, OnSetLineSolid3)
	ON_UPDATE_COMMAND_UI(ID_SET_LINE_SOLID_3, OnUpdateSetLineSolid3)
	ON_COMMAND(ID_SET_LINE_SOLID_4, OnSetLineSolid4)
	ON_UPDATE_COMMAND_UI(ID_SET_LINE_SOLID_4, OnUpdateSetLineSolid4)
	ON_COMMAND(ID_SET_LINE_DOT, OnSetLineDot)
	ON_UPDATE_COMMAND_UI(ID_SET_LINE_DOT, OnUpdateSetLineDot)
	ON_COMMAND(ID_SET_MULTI_LINE, OnSetMultiLine)
	ON_UPDATE_COMMAND_UI(ID_SET_MULTI_LINE, OnUpdateSetMultiLine)
	ON_COMMAND(ID_SET_SINGLE_LINE, OnSetSingleLine)
	ON_UPDATE_COMMAND_UI(ID_SET_SINGLE_LINE, OnUpdateSetSingleLine)
	ON_COMMAND(ID_ADD_RECT, OnAddRect)
	ON_UPDATE_COMMAND_UI(ID_ADD_RECT, OnUpdateAddRect)
	ON_COMMAND(ID_ADD_ARC, OnAddArc)
	ON_UPDATE_COMMAND_UI(ID_ADD_ARC, OnUpdateAddArc)
	ON_COMMAND(ID_ADD_LINK1, OnAddLink1)
	ON_UPDATE_COMMAND_UI(ID_ADD_LINK1, OnUpdateAddLink1)
	ON_COMMAND(ID_ADD_LINK2, OnAddLink2)
	ON_UPDATE_COMMAND_UI(ID_ADD_LINK2, OnUpdateAddLink2)
	ON_COMMAND(ID_SET_LINE_NULL, OnSetLineNull)
	ON_UPDATE_COMMAND_UI(ID_SET_LINE_NULL, OnUpdateSetLineNull)
	ON_COMMAND(ID_SET_LINK_INFO, OnSetLinkInfo)
	ON_UPDATE_COMMAND_UI(ID_SET_LINK_INFO, OnUpdateSetLinkInfo)
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_UPDATE_COMMAND_UI(ID_DELETE, OnUpdateDelete)
	ON_COMMAND(ID_AUTO_LAYOUT, OnAutoLayout)
	ON_UPDATE_COMMAND_UI(ID_AUTO_LAYOUT, OnUpdateAutoLayout)
	ON_COMMAND(ID_SET_NODE_PROP, OnSetNodeProp)
	ON_UPDATE_COMMAND_UI(ID_SET_NODE_PROP, OnUpdateSetNodeProp)
	ON_COMMAND(ID_GRASP_MODE, OnGraspMode)
	ON_UPDATE_COMMAND_UI(ID_GRASP_MODE, OnUpdateGraspMode)
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_LINK_STRAIGHT, OnLinkStraight)
	ON_UPDATE_COMMAND_UI(ID_LINK_STRAIGHT, OnUpdateLinkStraight)
	ON_COMMAND(ID_NO_BRUSH, OnNoBrush)
	ON_UPDATE_COMMAND_UI(ID_NO_BRUSH, OnUpdateNoBrush)
	ON_COMMAND(ID_SET_NODE_BRUSH, OnSetNodeBrush)
	ON_UPDATE_COMMAND_UI(ID_SET_NODE_BRUSH, OnUpdateSetNodeBrush)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_SET_NODE_RECT, OnSetNodeRect)
	ON_UPDATE_COMMAND_UI(ID_SET_NODE_RECT, OnUpdateSetNodeRect)
	ON_COMMAND(ID_SET_NODE_ARC, OnSetNodeArc)
	ON_UPDATE_COMMAND_UI(ID_SET_NODE_ARC, OnUpdateSetNodeArc)
	ON_COMMAND(ID_FIX_NODE, OnFixNode)
	ON_UPDATE_COMMAND_UI(ID_FIX_NODE, OnUpdateFixNode)
	ON_WM_KEYUP()
	ON_COMMAND(ID_NORMAL_CURSOR, OnNormalCursor)
	ON_UPDATE_COMMAND_UI(ID_NORMAL_CURSOR, OnUpdateNormalCursor)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	ON_COMMAND(ID_ADD_ROUND_RECT, OnAddRoundRect)
	ON_UPDATE_COMMAND_UI(ID_ADD_ROUND_RECT, OnUpdateAddRoundRect)
	ON_COMMAND(ID_SET_NODE_ROUND_RECT, OnSetNodeRoundRect)
	ON_UPDATE_COMMAND_UI(ID_SET_NODE_ROUND_RECT, OnUpdateSetNodeRoundRect)
	ON_COMMAND(ID_RANDOMIZE, OnRandomize)
	ON_UPDATE_COMMAND_UI(ID_RANDOMIZE, OnUpdateRandomize)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECT_ALL, OnUpdateEditSelectAll)
	ON_COMMAND(ID_REFRESH_NET_VIEW, OnRefreshNetView)
	ON_UPDATE_COMMAND_UI(ID_REFRESH_NET_VIEW, OnUpdateRefreshNetView)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_SHOW_NODE_SHAPES, OnShowNodeShapes)
	ON_UPDATE_COMMAND_UI(ID_SHOW_NODE_SHAPES, OnUpdateShowNodeShapes)
	ON_COMMAND(ID_EDIT_REPLACE, OnEditReplace)
	ON_COMMAND(ID_COPY_TO_CLIPBRD, OnCopyToClipbrd)
	ON_UPDATE_COMMAND_UI(ID_COPY_TO_CLIPBRD, OnUpdateCopyToClipbrd)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_ADJUST_TOP, OnAdjustTop)
	ON_UPDATE_COMMAND_UI(ID_ADJUST_TOP, OnUpdateAdjustTop)
	ON_COMMAND(ID_ADJUST_BOTTOM, OnAdjustBottom)
	ON_UPDATE_COMMAND_UI(ID_ADJUST_BOTTOM, OnUpdateAdjustBottom)
	ON_COMMAND(ID_ADJUST_LEFT, OnAdjustLeft)
	ON_UPDATE_COMMAND_UI(ID_ADJUST_LEFT, OnUpdateAdjustLeft)
	ON_COMMAND(ID_ADJUST_RIGHT, OnAdjustRight)
	ON_UPDATE_COMMAND_UI(ID_ADJUST_RIGHT, OnUpdateAdjustRight)
	ON_COMMAND(ID_SAME_HEIGHT, OnSameHeight)
	ON_UPDATE_COMMAND_UI(ID_SAME_HEIGHT, OnUpdateSameHeight)
	ON_COMMAND(ID_SAME_RECT, OnSameRect)
	ON_UPDATE_COMMAND_UI(ID_SAME_RECT, OnUpdateSameRect)
	ON_COMMAND(ID_SAME_WIDTH, OnSameWidth)
	ON_UPDATE_COMMAND_UI(ID_SAME_WIDTH, OnUpdateSameWidth)
	ON_COMMAND(ID_REVERSE_LINK_DIRECTION, OnReverseLinkDirection)
	ON_UPDATE_COMMAND_UI(ID_REVERSE_LINK_DIRECTION, OnUpdateReverseLinkDirection)
	ON_WM_ERASEBKGND()
	ON_WM_MOUSEWHEEL()
	ON_UPDATE_COMMAND_UI(ID_ZOOM_MODE, OnUpdateZoomMode)
	ON_COMMAND(ID_ZOOM_MODE, OnZoomMode)
	ON_WM_RBUTTONDBLCLK()
	ON_CBN_SELCHANGE(ID_DROPDOWN, OnSelchangeDropdown)
	ON_COMMAND(ID_EXPORT_SVG, OnExportSvg)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_SVG, OnUpdateExportSvg)
	ON_COMMAND(ID_INSERT_CHILD, OnInsertChild)
	ON_UPDATE_COMMAND_UI(ID_INSERT_CHILD, OnUpdateInsertChild)
	ON_COMMAND(ID_INSERT_SIBLING, OnInsertSibling)
	ON_UPDATE_COMMAND_UI(ID_INSERT_SIBLING, OnUpdateInsertSibling)
	ON_COMMAND(ID_ADD_NODES_FROM_CF_TEXT, OnAddNodesFromCfText)
	ON_UPDATE_COMMAND_UI(ID_ADD_NODES_FROM_CF_TEXT, OnUpdateAddNodesFromCfText)
	ON_COMMAND(ID_SET_LINK_ARROW_NONE, OnSetLinkArrowNone)
	ON_UPDATE_COMMAND_UI(ID_SET_LINK_ARROW_NONE, OnUpdateSetLinkArrowNone)
	ON_COMMAND(ID_SET_LINK_ARROW_SINGLE, OnSetLinkArrowSingle)
	ON_UPDATE_COMMAND_UI(ID_SET_LINK_ARROW_SINGLE, OnUpdateSetLinkArrowSingle)
	ON_COMMAND(ID_SET_LINK_ARROW_DOUBLE, OnSetLinkArrowDouble)
	ON_UPDATE_COMMAND_UI(ID_SET_LINK_ARROW_DOUBLE, OnUpdateSetLinkArrowDouble)
	//}}AFX_MSG_MAP
	// 標準印刷コマンド
	ON_NOTIFY( TTN_GETDISPINFO, 0, OnTipDispInfo)
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
	ON_MESSAGE(WM_ADDSHAPE, OnAddMetaFileID)
	ON_MESSAGE(WM_CLOSESHAPEWINDOW, OnHideShapesDlg)
	ON_MESSAGE(WM_GETSHAPE, OnRegNodeMetaFile)
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_CHAR()
	ON_COMMAND(ID_ADDLINKEDNODE_ARC, &NetView::OnAddlinkednodeArc)
	ON_UPDATE_COMMAND_UI(ID_ADDLINKEDNODE_ARC, &NetView::OnUpdateAddlinkednodeArc)
	ON_COMMAND(ID_ADDLINKEDNODE_RECT, &NetView::OnAddlinkednodeRect)
	ON_UPDATE_COMMAND_UI(ID_ADDLINKEDNODE_RECT, &NetView::OnUpdateAddlinkednodeRect)
	ON_COMMAND(ID_ADDLINKEDNODE_RNDRECT, &NetView::OnAddlinkednodeRndrect)
	ON_UPDATE_COMMAND_UI(ID_ADDLINKEDNODE_RNDRECT, &NetView::OnUpdateAddlinkednodeRndrect)
	ON_COMMAND(ID_ADD_LABEL_ONLY, &NetView::OnAddLabelOnly)
	ON_UPDATE_COMMAND_UI(ID_ADD_LABEL_ONLY, &NetView::OnUpdateAddLabelOnly)
	ON_COMMAND(ID_FONT_ENLARGE, &NetView::OnFontEnlarge)
	ON_COMMAND(ID_FONT_ENSMALL, &NetView::OnFontEnsmall)
	ON_UPDATE_COMMAND_UI(ID_FONT_ENLARGE, &NetView::OnUpdateFontEnlarge)
	ON_UPDATE_COMMAND_UI(ID_FONT_ENSMALL, &NetView::OnUpdateFontEnsmall)
	ON_COMMAND(ID_SET_LINK_DEPEND_SINGLE, &NetView::OnSetLinkDependSingle)
	ON_UPDATE_COMMAND_UI(ID_SET_LINK_DEPEND_SINGLE, &NetView::OnUpdateSetLinkDependSingle)
	ON_COMMAND(ID_SET_LINK_DEPEND_DOUBLE, &NetView::OnSetLinkDependDouble)
	ON_UPDATE_COMMAND_UI(ID_SET_LINK_DEPEND_DOUBLE, &NetView::OnUpdateSetLinkDependDouble)
	ON_COMMAND(ID_SET_LINK_INHERIT, &NetView::OnSetLinkInherit)
	ON_UPDATE_COMMAND_UI(ID_SET_LINK_INHERIT, &NetView::OnUpdateSetLinkInherit)
	ON_COMMAND(ID_SET_LINK_AGREGAT, &NetView::OnSetLinkAgregat)
	ON_UPDATE_COMMAND_UI(ID_SET_LINK_AGREGAT, &NetView::OnUpdateSetLinkAgregat)
	ON_COMMAND(ID_SET_LINK_COMPOSIT, &NetView::OnSetLinkComposit)
	ON_UPDATE_COMMAND_UI(ID_SET_LINK_COMPOSIT, &NetView::OnUpdateSetLinkComposit)
	ON_COMMAND(ID_DRAW_ORDER_INFO, &NetView::OnDrawOrderInfo)
	ON_UPDATE_COMMAND_UI(ID_DRAW_ORDER_INFO, &NetView::OnUpdateDrawOrderInfo)
	ON_COMMAND(ID_SET_NODE_MM, &NetView::OnSetNodeMm)
	ON_UPDATE_COMMAND_UI(ID_SET_NODE_MM, &NetView::OnUpdateSetNodeMm)
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NetView 描画

void NetView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();
	
	m_selectRect = GetDocument()->getSelectedNodeRect();
	if (!m_selectRect.IsRectEmpty()) {
		m_selectStatus = NetView::single;
	}
	
	adjustScrollArea();
	
	SetZoomLevel(1.0f);
	UpdateViewport(NULL);
	ReCalcBars();
	m_fZoomScale = 1.0f;
	m_fZoomScalePrev = 1.0f;
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CComboBox* pComboZoom = pFrame->getZoomComboBox();
	pComboZoom->SetCurSel(4);

	m_toolTip.Create(this);
	m_toolTip.AddTool(this, LPSTR_TEXTCALLBACK);
	RECT r;
	::SetRect( &r, 10, 10, 10, 10 );
	m_toolTip.SetMargin( &r );
	m_toolTip.SetDelayTime(TTDT_AUTOPOP, 3000);
//	m_toolTip.SetDelayTime(TTDT_INITIAL, 100);
//	m_toolTip.SetDelayTime(TTDT_RESHOW, 2000);
	m_toolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, 300);

}

void NetView::OnDraw(CDC* pDC)
{
	if (m_bLayouting) return;
	iEditDoc* pDoc = GetDocument();
	
	// TODO: この位置に描画用のコードを追加してください
	
	bool bDrwAll = false;
	pDoc->drawNodes(pDC, bDrwAll);
	if (!m_bDragRelax) {
		drawSelection(pDC);
	}
	pDoc->drawLinks(pDC, bDrwAll);
	
	if (m_bStartAdd) {
		drawAddLink(pDC);
	}
	
	if (m_bLinkCurving) {
		drawCurve(pDC);
	}
	
	if (m_bAlteringLinkFrom) {
		drawAlterLinkFrom(pDC);
	}
	
	if (m_bAlteringLinkTo) {
		drawAlterLinkTo(pDC);
	}
	
}

BOOL NetView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	// ブラシを使いたい背景色に設定する。
	
	if (!m_bLayouting) {
		CBrush backBrush(m_bkColor);
	
		// 古いブラシを保存する。
		CBrush* pOldBrush = pDC->SelectObject(&backBrush);
		CRect rect;
		pDC->GetClipBox(&rect);     // 対象領域を消去。
		
		pDC->PatBlt(rect.left, rect.top, rect.Width(), 
		rect.Height(), PATCOPY);
		pDC->SelectObject(pOldBrush);
		return TRUE;
	} else {
		return CScrollView::OnEraseBkgnd(pDC);
	}
}

void NetView::drawSelection(CDC *pDC)
{
	if (m_selectStatus == NetView::none) return;
	
	CRectTracker tracker;
	tracker.m_rect = m_selectRect;
	pDC->LPtoDP( &(tracker.m_rect) );
	tracker.m_rect.left += 1;
	tracker.m_rect.top  += 1;
	
	switch (m_selectStatus) {
	case NetView::single:
		tracker.m_nStyle = CRectTracker::resizeInside;
		tracker.Draw(pDC);
		break;
	case NetView::multi:
		tracker.m_nStyle = CRectTracker::hatchedBorder | CRectTracker::resizeInside;
		tracker.Draw(pDC);
		break;
	case NetView::link:
		if (!m_bLinkCurving) {
			GetDocument()->drawLinkSelection(pDC, false);
		}	
		break;
	case NetView::linkTermFrom:
		GetDocument()->drawLinkSelectionFrom(pDC, false);
		break;
	case NetView::linkTermTo:
		GetDocument()->drawLinkSelectionTo(pDC, false);
		break;
	}
}

void NetView::drawCurve(CDC *pDC)
{
	if (m_selectStatus == NetView::link) {
		m_linkPen.CreatePen(PS_DOT, 0, RGB(0, 31, 31));
		CPen* poldPen = pDC->SelectObject(&m_linkPen);
		pDC->MoveTo(m_linkStart);
		if (m_linkPath != CPoint(0, 0)) {
			pDC->LineTo(m_linkPath);
		}	
		pDC->LineTo(m_linkEnd);
		pDC->SelectObject(poldPen);
		m_linkPen.DeleteObject();
	}
}

void NetView::drawAddLink(CDC *pDC)
{
	if (m_addMode == NetView::link0 || m_addMode == NetView::link1 || m_addMode == NetView::link2) {
		m_linkPen.CreatePen(PS_DOT, 0, RGB(0, 31, 31));
		CPen* poldPen = pDC->SelectObject(&m_linkPen);
		pDC->MoveTo(m_ptLinkFrom);
		pDC->LineTo(m_ptLinkTo);
		pDC->SelectObject(poldPen);
		m_linkPen.DeleteObject();
	}
}

void NetView::drawAlterLinkFrom(CDC *pDC)
{
	if (m_selectStatus == NetView::linkTermFrom) {
		m_linkPen.CreatePen(PS_DOT, 0, RGB(0, 31, 31));
		CPen* poldPen = pDC->SelectObject(&m_linkPen);
		pDC->MoveTo(m_ptAlterLinkTo);
		pDC->LineTo(m_ptAlteringAnker);
		
		const int mrgn = 5;
		CPoint topleft(m_ptAlteringAnker.x - mrgn, m_ptAlteringAnker.y - mrgn);
		CPoint bottomright(m_ptAlteringAnker.x + mrgn, m_ptAlteringAnker.y + mrgn);
		CRect rc(topleft, bottomright);
		pDC->Arc(rc, topleft, topleft);

		pDC->SelectObject(poldPen);
		m_linkPen.DeleteObject();
	}
}

void NetView::drawAlterLinkTo(CDC *pDC)
{
	if (m_selectStatus == NetView::linkTermTo) {
		m_linkPen.CreatePen(PS_DOT, 0, RGB(0, 31, 31));
		CPen* poldPen = pDC->SelectObject(&m_linkPen);
		pDC->MoveTo(m_ptAlterLinkFrom);
		pDC->LineTo(m_ptAlteringAnker);
		
		const int mrgn = 5;
		CPoint topleft(m_ptAlteringAnker.x - mrgn, m_ptAlteringAnker.y - mrgn);
		CPoint bottomright(m_ptAlteringAnker.x + mrgn, m_ptAlteringAnker.y + mrgn);
		CRect rc(topleft, bottomright);
		pDC->Arc(rc, topleft, topleft);
		
		pDC->SelectObject(poldPen);
		m_linkPen.DeleteObject();
	}
}

/////////////////////////////////////////////////////////////////////////////
// NetView 診断

#ifdef _DEBUG
void NetView::AssertValid() const
{
	CScrollView::AssertValid();
}

void NetView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// NetView メッセージ ハンドラ

void NetView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加してください
	if (m_bLayouting) {
		return;
	}
	if (m_bZooming) {
		return;
	}
	if (m_addMode != normal) {
		m_addMode = normal;
		return;
	}
	if (m_bGrasp) {
		m_bGrasp = false;
		return;
	}
	CMenu menu;
	menu.LoadMenu(IDR_CONTEXT);
	
	CMenu* pPopup;
	if (m_selectStatus == NetView::none) {
		pPopup = menu.GetSubMenu(1);
	} else if (m_selectStatus == NetView::single) {
		pPopup = menu.GetSubMenu(2);
	} else if (m_selectStatus == NetView::link) {
		pPopup = menu.GetSubMenu(3);
	} else if (m_selectStatus == NetView::multi) {
		pPopup = menu.GetSubMenu(4);
	}
	
	if (point.x < 0 || point.y < 0) {
		CRect r;
		GetWindowRect(&r);
		point = r.TopLeft();
		if (m_selectStatus == NetView::single || m_selectStatus == NetView::multi) {
			CPoint p;
			p.x = (m_selectRect.left + m_selectRect.right)/2;
			p.y = (m_selectRect.top + m_selectRect.bottom)/2;
			point += p;
			point -= GetScrollPosition();
		} else if (m_selectStatus == NetView::link) {
			
		}
	}
	
	ASSERT(pPopup != NULL);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
}

BOOL NetView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	if (m_bGrasp) {
		if (m_bDragView) {
			::SetCursor(m_hHandClose);
		} else {
			::SetCursor(m_hHandOpen);
		}
		return TRUE;
	}
	
	if (m_bZooming) {
		::SetCursor(m_hZoomCsr);
		return TRUE;
	}
	
	
	if (m_selectStatus == NetView::single && m_addMode == NetView::normal) {
		CRectTracker tracker; tracker.m_rect = m_selectRect;
		
		ViewLPtoDP(tracker.m_rect);
		
		tracker.m_nStyle = CRectTracker::resizeInside;
		if (pWnd == this && tracker.SetCursor(this, nHitTest)) {
			return TRUE;
		}
	}
	if (pWnd == this && nHitTest == HTCLIENT) {
		switch (m_addMode) {
		case NetView::rect:
			::SetCursor(m_hRectCsr);
			return TRUE;
		case NetView::rRect:
			::SetCursor(m_hRRectCsr);
			return TRUE;
		case NetView::arc:
			::SetCursor(m_hArcCsr);
			return TRUE;
		case NetView::label:
			::SetCursor(m_hLabelCsr);
			return TRUE;
		case NetView::link0:
		case NetView::link1:
		case NetView::link2:
			::SetCursor(m_hLinkCsr);
			return TRUE;
		}
	}
	if (m_bLayouting) {
		::SetCursor(m_hLayout);
		return TRUE;
	}
	return CScrollView::OnSetCursor(pWnd, nHitTest, message);
}

iEditDoc* NetView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(iEditDoc)));
	return (iEditDoc*)m_pDocument;
}

void NetView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	///////////////////////////////////////////////////
	// TODO: 左ボタン押下時処理
	//////////////////////////////////////////////////
	if (m_bLayouting) { // 自動レイアウト中なら止めてリターン
		stopLayouting();
		return;
	}
	
	CPoint logPt = point; ViewDPtoLP(&logPt);
	
	preparePastePoint(logPt); // ノードのペースト先を用意しておく
	m_ptScreen = point + GetScrollPosition(); // スクリーン座標も保持しておく
	
	if (m_bGrasp) {     // ハンドモードの処理
		m_ptPrePos = point;
		return;
	}
	
	if (m_bZooming) {
		ZoomIn(&logPt, 0.1f);
		return;
	}
	
	if (isAddingNode()) { // ノード追加の処理
		CPoint spt(point);
		ClientToScreen(&spt);
		addNode(logPt, spt);
		return;
	}
	
	if (isAddingLink()) { // リンク追加開始の処理
		CRect r;
		if (GetDocument()->hitTest(logPt, r, false)) {
			startLink(logPt);
		}
		return;
	}
	
	// 芋づるモード開始
	if (nFlags & MK_CONTROL) {
		CRect r;
		if (GetDocument()->hitTest(logPt, r, false)) {
			// ノードの矩形内のオフセットを計算
			CPoint selTopLeft = GetDocument()->getSelectedNodeRect().TopLeft();
			m_dragOffset = selTopLeft - logPt;
			
			GetDocument()->disableUndo();
			GetDocument()->backUpUndoNodes();
			GetDocument()->backUpUndoLinks();
			m_bDragRelax = true;
			prepareDragRelax();
		}
		return;
	}
	
	
	// 複数選択中にトラッカを選択した → 複数トラック処理
	if (m_selectStatus == NetView::multi) {
		if (m_selectRect.PtInRect(logPt)) {
			CWindowDC dc(this);
			OnPrepareDC(&dc);
			trackMulti(logPt, point, &dc);
			return;
		}
	}
	
	// 単独選択中にトラッカを選択した → 単独トラック処理
	if (m_selectStatus == NetView::single) {
		CRect selectRect = m_selectRect;
		m_testRect = m_selectRect;
		m_testRect.OffsetRect(CPoint(20, 20));
		adjustRedrawBound(selectRect);
		CRectTracker tracker; tracker.m_rect = selectRect;
	//	tracker.m_rect.OffsetRect(-GetScrollPosition());
		tracker.m_nStyle = CRectTracker::resizeInside;
		int trackState = tracker.HitTest(logPt);
		CClientDC dc(this);
		OnPrepareDC(&dc);
		if (trackState != CRectTracker::hitNothing && trackState != CRectTracker::hitMiddle) {
			trackSingle(logPt, point, &dc);
			return;
		}
	}
	
	// 選択が更新された場合
	doUpdateSelection(logPt);
	
	// 選択更新後のトラック処理及び選択用ラバーバンドの処理
	doPostSelection(logPt);
	
	CScrollView::OnLButtonDown(nFlags, point);
}

bool NetView::isAddingNode() const
{
	return (m_addMode == NetView::rect || 
		    m_addMode == NetView::rRect ||
			m_addMode == NetView::arc ||
			m_addMode == NetView::label);
}

bool NetView::isAddingLink() const
{
	return (m_addMode == NetView::link0 ||
		    m_addMode == NetView::link1 ||
			m_addMode == NetView::link2);
}

void NetView::preparePastePoint(const CPoint &point)
{
	if (GetDocument()->canCopyNode() || 
		::IsClipboardFormatAvailable(CF_ENHMETAFILE) ||
		::IsClipboardFormatAvailable(CF_METAFILEPICT) ||
		::IsClipboardFormatAvailable(CF_DIB) ||
		::IsClipboardFormatAvailable(CF_TEXT)) {
		m_ptPaste = point;
	} else {
		m_ptPaste = CPoint(0, 0);
	}

}

// 選択が更新される場合の処理
void NetView::doUpdateSelection(const CPoint &logPt)
{
	CRect old = m_selectRect; adjustRedrawBound(old);
	CRect r;
	if (GetDocument()->hitTestLinks(logPt, false)) {
		// リンクの選択が更新された
		m_selectStatus = NetView::link;
		GetDocument()->getSelectedLinkPts(m_linkStart, m_linkEnd, false);
		m_selectRect = GetDocument()->getSelectedLinkBound(false);
		CRect nw = m_selectRect; adjustRedrawBound(nw);
		InvalidateRect(nw);
		InvalidateRect(old);
	} else if (GetDocument()->hitTestLinksFrom(logPt, false)) {
		// リンクのリンク元を選択した
		m_selectStatus = NetView::linkTermFrom;
		m_selectRect = GetDocument()->getSelectedLinkBound(false);
		CRect nw = m_selectRect; adjustRedrawBound(nw);
		InvalidateRect(nw);
		InvalidateRect(old);
		startAlterFrom(logPt);
	} else if (GetDocument()->hitTestLinksTo(logPt, false)) {
		// リンクのリンク先を選択した
		m_selectStatus = NetView::linkTermTo;
		m_selectRect = GetDocument()->getSelectedLinkBound(false);
		CRect nw = m_selectRect; adjustRedrawBound(nw);
		InvalidateRect(nw);
		InvalidateRect(old);
		startAlterTo(logPt);
	} else if (GetDocument()->hitTest(logPt, r, false)) {
		// ノードの選択が更新された
		m_selectStatus = NetView::single;
		m_selectRect = r;
		CRect nw = m_selectRect; adjustRedrawBound(nw);
		InvalidateRect(nw);
		InvalidateRect(old);
	} else {
		// 選択が解除された
		CRect old = m_selectRect; adjustRedrawBound(old);
		m_selectStatus = NetView::none;
		m_selectRect = CRect(0, 0, 0, 0);
		InvalidateRect(old);
	}
}

// 選択が更新された後の処理 --トラック処理とラバーバンド処理
void NetView::doPostSelection(const CPoint &logPt)
{
	CPoint logicalPt = logPt;
	CPoint point = logPt;
	ViewLPtoDP(&point);
	if (m_selectStatus == NetView::single) {
		// 単独選択の場合 → トラック処理にすぐ入る
		CWindowDC dc(this);
		OnPrepareDC(&dc);
		trackSingle(logicalPt, point, &dc);
	} else if (m_selectStatus == NetView::none) {
		// 何も選択されていない場合は 選択用ラバーバンドの処理
		CRectTracker tracker;
		m_selectStatus = NetView::none;
		tracker.TrackRubberBand(this, point);
		CRect trackerRect;
		tracker.GetTrueRect(&trackerRect);
		CWindowDC dc(this);
		OnPrepareDC(&dc);
		dc.DPtoLP(&trackerRect);
		CRect selRect;
		int selcnt = GetDocument()->selectNodesInBound(trackerRect, selRect, false);
		CRect old = m_selectRect; adjustRedrawBound(old);
		if ( selcnt > 0) {
			m_selectRect = selRect;
			if (selcnt > 1) {
				m_selectStatus = NetView::multi;
				GetDocument()->selectLinksInBound(trackerRect, false);// 含まれるlinkも選択する
				m_selectRect |= GetDocument()->getSelectedLinkBound(false);
			} else if (selcnt == 1) {
				m_selectStatus = NetView::single;
			}
			CRect rdRect = m_selectRect; adjustRedrawBound(rdRect);
			InvalidateRect(rdRect);
			InvalidateRect(old);
		} else {
			m_selectStatus = NetView::none;
			m_selectRect = CRect(0, 0, 0, 0);
			InvalidateRect(old);
		}
	}
}

void NetView::trackMulti(CPoint &logPt, CPoint &point, CDC *pDC)
{
	CRectTracker tracker;
	tracker.m_rect = m_selectRect;
	tracker.m_nStyle = CRectTracker::hatchedBorder | CRectTracker::resizeInside;
	pDC->LPtoDP(&(tracker.m_rect));
	
	CRect old = GetDocument()->getRelatedBound(false); adjustRedrawBound(old);
	if(tracker.Track(this, point, TRUE) ) {
		CRect org = m_selectRect;
		pDC->DPtoLP(&(tracker.m_rect));
		CRect nw;
		tracker.GetTrueRect(&nw);
		if (nw.left < 0) nw.left = 0;
		if (nw.top < 0) nw.top = 0;
		nw.right = nw.left + org.Width();
		nw.bottom = nw.top + org.Height();
		m_selectRect = nw;
		int moveX = m_selectRect.left - org.left;
		int moveY = m_selectRect.top - org.top;
		
		GetDocument()->backUpUndoNodes();
		GetDocument()->backUpUndoLinks();
		GetDocument()->moveSelectedNode(CSize(moveX, moveY));
		GetDocument()->moveSelectedLink(CSize(moveX, moveY));
		CRect nwrd = GetDocument()->getRelatedBound(false); adjustRedrawBound(nwrd);
		
		CRect rc;
		if (!rc.IntersectRect(old, nwrd)) {
			InvalidateRect(old);
			InvalidateRect(nwrd);
		} else {
			CRect rd = old | nwrd;
			InvalidateRect(rd);
		}
		adjustScrollArea();
	}
}

void NetView::trackSingle(CPoint &logPt, CPoint& point, CDC* pDC)
{
	CRectTracker tracker;
	CRect org = m_selectRect;
	CRect selectRect = m_selectRect;
	ViewLPtoDP(selectRect);
	tracker.m_rect = selectRect;
	tracker.m_nStyle = CRectTracker::resizeInside;
	CRect old = GetDocument()->getRelatedBound(false); adjustRedrawBound(old);
	// getRelatedBoundAndに変更
	if(tracker.Track(this, point, TRUE) ) {
		
		GetDocument()->backUpUndoNodes();
		GetDocument()->backUpUndoLinks();
		
		CRect nw;
		ViewDPtoLP(tracker.m_rect);
		tracker.GetTrueRect(&nw);
		if (nw.left < 0) {
			nw.left = 0;
			nw.right = m_selectRect.Width();
		}
		if (nw.top < 0) {
			nw.top = 0;
			nw.bottom = m_selectRect.Height();
		}
		m_selectRect = nw;
		GetDocument()->setSelectedNodeBound(m_selectRect);
		int moveX = m_selectRect.left - org.left;
		int moveY = m_selectRect.top - org.top;
		GetDocument()->moveSelectedLink(CSize(moveX, moveY));
		if (((CiEditApp*)AfxGetApp())->m_rgsNode.bEnableGroup) {
			GetDocument()->moveNodesInBound(org, CSize(moveX, moveY));
		}
		CRect rdnw = GetDocument()->getRelatedBound(false); adjustRedrawBound(rdnw);
		// getRelatedBoundAndに変更
		CRect rc;
		if (!rc.IntersectRect(old, rdnw)) {
			InvalidateRect(old);
			rdnw.InflateRect((int)(rdnw.Width()*1.5), (int)(rdnw.Height()*1.5));
			InvalidateRect(rdnw);
		} else {
			CRect rdRec = old | rdnw;
			rdRec.InflateRect((int)(rdRec.Width()*1.5), (int)(rdRec.Height()*1.5));
			InvalidateRect(rdRec);
		}
		adjustScrollArea();
	}
}

void NetView::OnAddLink0() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (m_addMode == NetView::link0) {
		m_addMode = NetView::normal;
	} else {
		m_addMode = NetView::link0;
	}
}

void NetView::OnUpdateAddLink0(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting && !m_bGrasp && !m_bZooming);
	pCmdUI->SetCheck(m_addMode == NetView::link0);
}

void NetView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	stopLayouting();
	adjustScrollArea();
	
	int prevSelectStatus = m_selectStatus; // 事前の選択状態保持
	
	m_selectStatus = NetView::none;
	
	CRect oldBound = m_selectRect;
	adjustRedrawBound(oldBound);
	
	if (isScaleChanged()) {
		UpdateViewport(NULL);
		m_fZoomScalePrev = m_fZoomScale;
	}
	
	CClientDC dc(this);
	OnPrepareDC(&dc);
//	CSize sz(1, 1);
	CRect old, nw;
	
	if (GetDocument()->isShowSubBranch() && GetDocument()->isCurKeyInBranch() ||
		!GetDocument()->isShowSubBranch()) {
		m_selectRect = GetDocument()->getSelectedNodeRect();
	} else {
		m_selectRect = CRect(0, 0, 0, 0);
	}
	
	if (!m_selectRect.IsRectEmpty()) {
		m_selectStatus = NetView::single;
	}
	CRect nwBound, rdRC;
	
	if (pHint == NULL) {
		InvalidateRect(&oldBound);
		nwBound = m_selectRect; adjustRedrawBound(nwBound);
		InvalidateRect(nwBound);
		return;
	}
	
	iHint* ph = NULL;
	if (pHint != NULL) {
		ph = reinterpret_cast<iHint*>(pHint);
	}
	if (ph == NULL) return;
	switch (ph->event) {
	case iHint::viewSettingChanged:
		doColorSetting();
		Invalidate();
		break;
	
	case iHint::linkSelRet:
		break;
	case iHint::linkSel:
	case iHint::linkCurved:
	case iHint::linkStraight:
	case iHint::linkListSel:
	case iHint::linkModified:
		InvalidateRect(oldBound);
		
		// 再描画領域の計算ロジックを作ってみた
		nwBound = GetDocument()->getSelectedLinkBound(false);
		nwBound.left = (int)(nwBound.left*m_fZoomScale);
		nwBound.top = (int)(nwBound.top*m_fZoomScale);
		nwBound.right = (int)(nwBound.right*m_fZoomScale);
		nwBound.bottom = (int)(nwBound.bottom*m_fZoomScale);
		nwBound.OffsetRect(GetScrollPosition());
		
		m_selectRect = nwBound;
		m_selectStatus = NetView::link;
		InvalidateRect(nwBound);
		
		break;
	case iHint::lelax:
		old = ph->preRC;
		nw = ph->curRC;
		adjustRedrawBound(old);
		adjustRedrawBound(nw);
	//	dc.DrawDragRect(nw, sz, old, sz);
		InvalidateRect(old | nw);
		break;
	
	case iHint::nodeDeleteByKey:
		if (!GetDocument()->isShowSubBranch()) {
			old = GetDocument()->restoreDeleteBound();
			adjustRedrawBound(old);
			InvalidateRect(old);
		} else {
			Invalidate();
		}
		break;
	
	case iHint::reflesh:
		Invalidate();
		break;
	case iHint::parentSel:
		if (GetDocument()->isShowSubBranch()) {
			break;
		}
		Invalidate();
		SendMessage(WM_SIZE); // こうでもしないと反映してくれない。暫定的に
		break;
	case iHint::nodeLabelChanged:
	case iHint::nodeStyleChanged:
		m_selectStatus = prevSelectStatus;
		if (m_selectStatus == NetView::multi) {
			m_selectRect = GetDocument()->getRelatedBoundAnd(false);
		}
		InvalidateRect(&oldBound);
		nwBound = GetDocument()->getRelatedBound(false); adjustRedrawBound(nwBound);
		InvalidateRect(nwBound);
		break;
	case iHint::linkAdd:
		nwBound = GetDocument()->getRelatedBound(false); adjustRedrawBound(nwBound);
		InvalidateRect(nwBound);
		break;
	case iHint::showSubBranch:
		m_selectRect=CRect(0,0,0,0);
		Invalidate();
		break;
	case iHint::resetShowSubBranch:
		Invalidate();
		break;
	case iHint::linkDelete:
		old = GetDocument()->restoreDeleteBound();
		adjustRedrawBound(old);
		InvalidateRect(old);
		break;
	
	case iHint::nodeFontResize:
		m_selectStatus = prevSelectStatus;
		if (m_selectStatus == NetView::multi) {
			m_selectRect = GetDocument()->getRelatedBoundAnd(false);
		}
		nwBound = GetDocument()->getRelatedBound(); adjustRedrawBound(nwBound);
		nwBound |= oldBound;
		InvalidateRect(nwBound);
		break;
	
	case iHint::nodeSel:
		InvalidateRect(&oldBound);
		nwBound = m_selectRect; adjustRedrawBound(nwBound);
		InvalidateRect(nwBound);
		
		CRect viewPort; GetClientRect(&viewPort);
		CPoint pos = GetScrollPosition();
		
		CRect selRect; // 選択矩形の見えの大きさ格納用
		selRect.left = (LONG)(((double)m_selectRect.left)*m_fZoomScale);
		selRect.top = (LONG)(((double)m_selectRect.top)*m_fZoomScale);
		selRect.right = (LONG)(((double)m_selectRect.right)*m_fZoomScale);
		selRect.bottom = (LONG)(((double)m_selectRect.bottom)*m_fZoomScale);
		
		viewPort.OffsetRect(pos.x, pos.y);
		if (!viewPort.IntersectRect(viewPort, selRect)) {
			ScrollToPosition(CPoint(selRect.TopLeft()));
			SendMessage(WM_SIZE); // こうでもしないと反映してくれない。暫定的に
			Invalidate();
		}
		break;

	}
}

void NetView::startLink(const CPoint pt)
{
	iEditDoc* pDoc = GetDocument();
	m_bStartAdd = pDoc->setStartLink(pt);
	if (m_bStartAdd) {
		m_preRedrawBound = CRect(pt, pt);
		m_ptLinkFrom = pt;
	}
}

void NetView::startAlterFrom(const CPoint& pt)
{
	m_bAlteringLinkFrom = true;
	iLink l = GetDocument()->getSelectedLink(false);
	m_ptAlterLinkFrom = l.getPtFrom();
	m_ptAlterLinkTo = l.getPtTo();
}

void NetView::startAlterTo(const CPoint &pt)
{
	m_bAlteringLinkTo = true;
	iLink l = GetDocument()->getSelectedLink(false);
	m_ptAlterLinkFrom = l.getPtFrom();
	m_ptAlterLinkTo = l.getPtTo();
}

void NetView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	CPoint logPt = point;
	ViewDPtoLP(&logPt);
	
	/////////////////////
	// ツールチップ表示用
	/////////////////////
	if (m_addMode == normal && !m_bLayouting && m_selectStatus != NetView::multi
		 && !m_bDragRelax) {
		CRect r;
		iEditDoc* pDoc = GetDocument();
		iNode node = pDoc->getHitNode(logPt, false);
		if (node.getText() != "") {
			CString strTipNew = node.getText().Left(300) + "....";
			if (m_strTip != strTipNew) {
				m_strTip = strTipNew;
				m_toolTip.Update();
			}
		} else {
			m_strTip = "";
			m_toolTip.Pop();
		}
	}
	
	
	/////////////////////
	// View のGraspScroll
	/////////////////////
	if (m_bGrasp) {
		m_bDragView = (nFlags & MK_LBUTTON);
		CSize sz = GetTotalSize();
		CRect rc; CScrollView::GetWindowRect(&rc);
		if (m_bDragView) {
			int dx = point.x - m_ptPrePos.x;
			if ((int)(sz.cx*m_fZoomScale) < rc.Width()) {
				dx = 0;
			}
			int dy = point.y - m_ptPrePos.y;
			if ((int)(sz.cy*m_fZoomScale) < rc.Height()) {
				dy = 0;
			}
			
			CPoint curPt = GetScrollPosition();
			CPoint toPt(curPt.x - dx, curPt.y - dy);
			ScrollToPosition(toPt);
			m_ptPrePos = point;
		}
		return;
	}
	
	/////////////////////
	// 芋蔓処理
	/////////////////////
	if (nFlags & MK_CONTROL && m_bDragRelax) {
		GetDocument()->relaxSingleStep(logPt, m_dragOffset);
		CRect rBound = GetDocument()->getChaindNodesBound();
		ViewLPtoDP(rBound);
		InvalidateRect(rBound);
		return;
	}
	
	
	/////////////////////
	// link の Curve
	/////////////////////
	if (!m_bStartAdd && m_selectStatus == NetView::link && nFlags & MK_LBUTTON) {
		m_linkPath = logPt;
		m_bLinkCurving = true;
		CRect rc = getCurveBound(logPt);
		CRect nw = rc; adjustRedrawBound(nw);
		adjustRedrawBound(m_preRedrawBound);
		CRect rdRc = nw | m_preRedrawBound;
		InvalidateRect(rdRc);
		m_preRedrawBound = rc;
		CScrollView::OnMouseMove(nFlags, point);
		return;
	}
	
	//////////////////////
	//  リンクの付け替え
	//////////////////////
	if (m_bAlteringLinkFrom && m_selectStatus == NetView::linkTermFrom && nFlags & MK_LBUTTON) {
		m_ptAlteringAnker = logPt;
		CRect old = m_preRedrawBound; adjustRedrawBound(old);
		
		CRect rc(m_ptAlterLinkTo, m_ptAlteringAnker);
		rc.NormalizeRect();
		adjustRedrawBound(rc);
		CRect rdRc = rc | m_preRedrawBound;
		InvalidateRect(rdRc);
		m_preRedrawBound = rc;
		CScrollView::OnMouseMove(nFlags, point);
		return;
	}
	
	if (m_bAlteringLinkTo && m_selectStatus == NetView::linkTermTo && nFlags & MK_LBUTTON) {
		m_ptAlteringAnker = logPt;
		CRect rc(m_ptAlterLinkFrom, m_ptAlteringAnker);
		rc.NormalizeRect();
		adjustRedrawBound(rc);
		CRect rdRc = rc | m_preRedrawBound;
		InvalidateRect(rdRc);
		m_preRedrawBound = rc;
		CScrollView::OnMouseMove(nFlags, point);
		return;
	}
	
	///////////////////
	// link の追加
	///////////////////
	if (isAddingLink()) {
		if (!nFlags & MK_LBUTTON) {
			CScrollView::OnMouseMove(nFlags, point);
			m_bStartAdd = false;
		}
		if (!m_bStartAdd) return;
		m_ptLinkTo = logPt;
		CRect old = m_preRedrawBound; adjustRedrawBound(old);
		
		CRect rc(m_ptLinkFrom, m_ptLinkTo);
		rc.NormalizeRect();
		adjustRedrawBound(rc);
		CRect rdRc = rc | m_preRedrawBound;
		InvalidateRect(rdRc);
		m_preRedrawBound = rc;
		CScrollView::OnMouseMove(nFlags, point);
		return;
	}
}

void NetView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	////////////////////////////////////////
	// TODO: Mouseの左ボタンリリース時の処理
	////////////////////////////////////////
	
	if (m_bGrasp) { // ハンドモードの解除
		m_bDragView = false;
		return;
	}
	
	CPoint logPt = point;
	ViewDPtoLP(&logPt);
	
	// 芋蔓モードの解除
	if (m_bDragRelax) {
		cancelDragRelax();
		return;
	}
	
	// リンク元の張替え
	if (m_bAlteringLinkFrom) {
		m_bAlteringLinkFrom = false;
		iLink l = GetDocument()->getSelectedLink(false);
		if (GetDocument()->setAlterLinkFrom(logPt, false)) {
			CRect rcOld = CRect(m_ptAlterLinkTo, m_ptAlterLinkTo);
			rcOld.NormalizeRect();
			CRect rcNew = GetDocument()->getSelectedLinkBound(false);
			rcNew.NormalizeRect();
			InvalidateRect(rcOld | rcNew);
		} else {
			CRect rcDel = CRect(m_ptAlterLinkTo, m_ptAlteringAnker);
			rcDel.NormalizeRect();
			InvalidateRect(rcDel);
		}
		return;
	}
	
	// リンク先の張替え
	if (m_bAlteringLinkTo) {
		m_bAlteringLinkTo = false;
		iLink l = GetDocument()->getSelectedLink(false);
		if (GetDocument()->setAlterLinkTo(logPt, false)) {
			CRect rcOld = CRect(m_ptAlterLinkFrom, m_ptAlterLinkTo);
			rcOld.NormalizeRect();
			CRect rcNew = GetDocument()->getSelectedLinkBound(false);
			rcNew.NormalizeRect();
			InvalidateRect(rcOld | rcNew);
		} else {
			CRect rcDel = CRect(m_ptAlterLinkFrom, m_ptAlteringAnker);
			rcDel.NormalizeRect();
			InvalidateRect(rcDel);
		}
		return;
	}
	
	
	// リンクの追加(ノード上でマウスをリリース)
	if (!m_bStartAdd) {
		if (m_bLinkCurving && m_selectStatus == NetView::link) {
			m_bLinkCurving = false;
			GetDocument()->disableUndo();
			GetDocument()->backUpUndoLinks();
			GetDocument()->setSelectedLinkCurve(logPt, true, false);
			Invalidate();
		}
		return;
	}
	
	bool linked = false;
	switch (m_addMode) {
	case NetView::link0:
		linked = GetDocument()->setEndLink(logPt, 0, false, true);
		m_bStartAdd = false;
		break;
	case NetView::link1:
		linked = GetDocument()->setEndLink(logPt, 1, false, true);
		m_bStartAdd = false;
		break;
	case NetView::link2:
		linked = GetDocument()->setEndLink(logPt, 2, false, true);
		m_bStartAdd = false;
		break;
	}
	if (!linked) {
		CRect rc(m_ptLinkFrom, m_ptLinkTo);
		rc.NormalizeRect();
		InvalidateRect(rc);
	}
}

void NetView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	if (m_bLayouting) return;
	CPoint logPt = point;
	ViewDPtoLP(&logPt);
	
	if (m_bZooming) {
		ZoomOut(&logPt, 0.1f);
		return;
	}
	
	CRect r;
	iEditDoc* pDoc = GetDocument();
	CRect old = m_selectRect; adjustRedrawBound(old);
	if (m_selectStatus != NetView::multi) {
		// hitTest, linkHitTest
		if (pDoc->hitTestLinks(logPt, false)) {
			m_selectStatus = NetView::link;
			m_selectRect = GetDocument()->getSelectedLinkBound(false);		
		} else if (pDoc->hitTest(logPt, r, false)) {
			m_selectStatus = NetView::single;
			m_selectRect = r;
		} else {
			m_selectStatus = NetView::none;
			m_selectRect = CRect(0, 0, 0, 0);
		}
	} else {
		// m_selectRect hitTest, hitTest, linkHitTest
		if (!m_selectRect.PtInRect(logPt)) {
			 if (pDoc->hitTestLinks(logPt, false)) {
				m_selectStatus = NetView::link;
				m_selectRect = GetDocument()->getSelectedLinkBound(false);			
			} else if (pDoc->hitTest(logPt, r, false)) {
				m_selectStatus = NetView:: single;
				m_selectRect = r;
			} else {
				m_selectStatus = NetView::none;
				m_selectRect = CRect(0, 0, 0, 0);
			}
		}
	}
	CRect nw = m_selectRect; adjustRedrawBound(nw);
	InvalidateRect(nw);
	InvalidateRect(old);
	if (GetDocument()->canCopyNode()) {
		m_ptPaste = logPt;
	}
	
	CScrollView::OnRButtonDown(nFlags, point);
}

void NetView::OnSetNodeFont() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	::lstrcpy(lf.lfFaceName, "ＭＳ ゴシック");
	lf.lfHeight = 0xfffffff3;
	lf.lfWidth = 0;
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet= SHIFTJIS_CHARSET;
	
	GetDocument()->backUpUndoNodes();
	GetDocument()->backUpUndoLinks();
	if (m_selectStatus == NetView::single) {
		GetDocument()->getSelectedNodeFont(lf);
		CFontDialog dlg(&lf);
		dlg.m_cf.rgbColors = GetDocument()->getSelectedNodeFontColor();
		if (dlg.DoModal() != IDOK) return;
		GetDocument()->setSelectedNodeFont(lf);
		GetDocument()->setSelectedNodeFontColor(dlg.GetColor());
	} else if (m_selectStatus == NetView::link) {
		GetDocument()->getSelectedLinkFont(lf, false);
		CFontDialog dlg(&lf);
		if (dlg.DoModal() != IDOK) return;
		GetDocument()->setSelectedLinkFont(lf, false);
	} else if (m_selectStatus == NetView::multi) {
		CFontDialog dlg(&lf);
		if (dlg.DoModal() != IDOK) return;
		GetDocument()->setSelectedNodeFont(lf);
		GetDocument()->setSelectedNodeFontColor(dlg.GetColor());
		GetDocument()->setSelectedLinkFont(lf, false);
	}
	
	Invalidate();
}

void NetView::OnUpdateSetNodeFont(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	
}

void NetView::OnSetNodeLineColor() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	bool bDrwAll = false;
	GetDocument()->backUpUndoNodes();
	GetDocument()->backUpUndoLinks();
	if (m_selectStatus == NetView::single) {
		CColorDialog dlg(GetDocument()->getSelectedNodeLineColor());
		if (dlg.DoModal() != IDOK) return;
		GetDocument()->setSelectedNodeLineColor(dlg.GetColor());
	} else if (m_selectStatus == NetView::link) {
		CColorDialog dlg(GetDocument()->getSelectedLinkLineColor(bDrwAll));
		if (dlg.DoModal() != IDOK) return;
		GetDocument()->setSelectedLinkLineColor(dlg.GetColor(), bDrwAll);
	} else if (m_selectStatus == NetView::multi) {
		CColorDialog dlg;
		if (dlg.DoModal() != IDOK) return;
		GetDocument()->setSelectedNodeLineColor(dlg.GetColor());
		GetDocument()->setSelectedLinkLineColor(dlg.GetColor(), bDrwAll);
	}
}

void NetView::OnUpdateSetNodeLineColor(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	
}

void NetView::OnSetLineNull() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	GetDocument()->setSelectedNodeLineWidth(0);
	GetDocument()->setSelectedNodeLineStyle(PS_NULL);
}

void NetView::OnUpdateSetLineNull(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	if (m_selectStatus == NetView::single) {
		BOOL state = GetDocument()->getSelectedNodeLineStyle() == PS_NULL &&
			         GetDocument()->getSelectedNodeLineWidth() == 0;
		pCmdUI->SetCheck(state);
	}
}

void NetView::OnSetLineSolid0() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	bool bDrwAll = false;
	GetDocument()->backUpUndoNodes();
	GetDocument()->backUpUndoLinks();
	switch (m_selectStatus) {
	case NetView::single:
		GetDocument()->setSelectedNodeLineWidth(0);
		GetDocument()->setSelectedNodeLineStyle(PS_SOLID);
		break;
	case NetView::link:
		GetDocument()->setSelectedLinkWidth(0, bDrwAll);
		GetDocument()->setSelectedLinkLineStyle(PS_SOLID, bDrwAll);
		break;
	case NetView::multi:
		GetDocument()->setSelectedNodeLineWidth(0);
		GetDocument()->setSelectedNodeLineStyle(PS_SOLID);
		GetDocument()->setSelectedLinkWidth(0, bDrwAll);
		GetDocument()->setSelectedLinkLineStyle(PS_SOLID, bDrwAll);
		break;
	}
}

void NetView::OnUpdateSetLineSolid0(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	if (m_selectStatus == NetView::single) {
		BOOL state = GetDocument()->getSelectedNodeLineStyle() == PS_SOLID &&
			         GetDocument()->getSelectedNodeLineWidth() == 0;
		pCmdUI->SetCheck(state);
	} else if (m_selectStatus == NetView::link) {
		BOOL state = GetDocument()->getSelectedLinkLineStyle() == PS_SOLID &&
			         GetDocument()->getSelectedLinkWidth() == 0;
		pCmdUI->SetCheck(state);
	}
}

void NetView::OnSetLineSolid1() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	bool bDrwAll = false;
	GetDocument()->backUpUndoNodes();
	GetDocument()->backUpUndoLinks();
	switch (m_selectStatus) {
	case NetView::single:
		GetDocument()->setSelectedNodeLineWidth(2);
		GetDocument()->setSelectedNodeLineStyle(PS_SOLID);
		break;
	case NetView::link:
		GetDocument()->setSelectedLinkWidth(2, bDrwAll);
		GetDocument()->setSelectedLinkLineStyle(PS_SOLID, bDrwAll);
		break;
	case NetView::multi:
		GetDocument()->setSelectedNodeLineWidth(2);
		GetDocument()->setSelectedNodeLineStyle(PS_SOLID);
		GetDocument()->setSelectedLinkWidth(2, bDrwAll);
		GetDocument()->setSelectedLinkLineStyle(PS_SOLID, bDrwAll);
		break;
	}
}

void NetView::OnUpdateSetLineSolid1(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	if (m_selectStatus == NetView::single) {
		BOOL state = GetDocument()->getSelectedNodeLineStyle() == PS_SOLID &&
			         GetDocument()->getSelectedNodeLineWidth() == 2;
		pCmdUI->SetCheck(state);
	} else if (m_selectStatus == NetView::link) {
		BOOL state = GetDocument()->getSelectedLinkLineStyle() == PS_SOLID &&
			         GetDocument()->getSelectedLinkWidth() == 2;
		pCmdUI->SetCheck(state);
	}
}

void NetView::OnSetLineSolid2() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	bool bDrwAll = false;
	GetDocument()->backUpUndoNodes();
	GetDocument()->backUpUndoLinks();
	switch (m_selectStatus) {
	case NetView::single:
		GetDocument()->setSelectedNodeLineWidth(3);
		GetDocument()->setSelectedNodeLineStyle(PS_SOLID);
		break;
	case NetView::link:
		GetDocument()->setSelectedLinkWidth(3, bDrwAll);
		GetDocument()->setSelectedLinkLineStyle(PS_SOLID, bDrwAll);
		break;
	case NetView::multi:
		GetDocument()->setSelectedNodeLineWidth(3);
		GetDocument()->setSelectedNodeLineStyle(PS_SOLID);
		GetDocument()->setSelectedLinkWidth(3, bDrwAll);
		GetDocument()->setSelectedLinkLineStyle(PS_SOLID, bDrwAll);
		break;
	}
}

void NetView::OnUpdateSetLineSolid2(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	if (m_selectStatus == NetView::single) {
		BOOL state = GetDocument()->getSelectedNodeLineStyle() == PS_SOLID &&
			         GetDocument()->getSelectedNodeLineWidth() == 3;
		pCmdUI->SetCheck(state);
	} else if (m_selectStatus == NetView::link) {
		BOOL state = GetDocument()->getSelectedLinkLineStyle() == PS_SOLID &&
			         GetDocument()->getSelectedLinkWidth() == 3;
		pCmdUI->SetCheck(state);
	}
}

void NetView::OnSetLineSolid3() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	bool bDrwAll = false;
	GetDocument()->backUpUndoNodes();
	GetDocument()->backUpUndoLinks();
	switch (m_selectStatus) {
	case NetView::single:
		GetDocument()->setSelectedNodeLineWidth(4);
		GetDocument()->setSelectedNodeLineStyle(PS_SOLID);
		break;
	case NetView::link:
		GetDocument()->setSelectedLinkWidth(4, bDrwAll);
		GetDocument()->setSelectedLinkLineStyle(PS_SOLID, bDrwAll);
		break;
	case NetView::multi:
		GetDocument()->setSelectedNodeLineWidth(4);
		GetDocument()->setSelectedNodeLineStyle(PS_SOLID);
		GetDocument()->setSelectedLinkWidth(4, bDrwAll);
		GetDocument()->setSelectedLinkLineStyle(PS_SOLID, bDrwAll);
		break;
	}
}

void NetView::OnUpdateSetLineSolid3(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	if (m_selectStatus == NetView::single) {
		BOOL state = GetDocument()->getSelectedNodeLineStyle() == PS_SOLID &&
			         GetDocument()->getSelectedNodeLineWidth() == 4;
		pCmdUI->SetCheck(state);
	} else if (m_selectStatus == NetView::link) {
		BOOL state = GetDocument()->getSelectedLinkLineStyle() == PS_SOLID &&
			         GetDocument()->getSelectedLinkWidth() == 4;
		pCmdUI->SetCheck(state);
	}
}

void NetView::OnSetLineSolid4() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	bool bDrwAll = false;
	GetDocument()->backUpUndoNodes();
	GetDocument()->backUpUndoLinks();
	switch (m_selectStatus) {
	case NetView::single:
		GetDocument()->setSelectedNodeLineWidth(5);
		GetDocument()->setSelectedNodeLineStyle(PS_SOLID);
		break;
	case NetView::link:
		GetDocument()->setSelectedLinkWidth(5, bDrwAll);
		GetDocument()->setSelectedLinkLineStyle(PS_SOLID, bDrwAll);
		break;
	case NetView::multi:
		GetDocument()->setSelectedNodeLineWidth(5);
		GetDocument()->setSelectedNodeLineStyle(PS_SOLID);
		GetDocument()->setSelectedLinkWidth(5, bDrwAll);
		GetDocument()->setSelectedLinkLineStyle(PS_SOLID, bDrwAll);
		break;
	}
}

void NetView::OnUpdateSetLineSolid4(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	if (m_selectStatus == NetView::single) {
		BOOL state = GetDocument()->getSelectedNodeLineStyle() == PS_SOLID &&
			         GetDocument()->getSelectedNodeLineWidth() == 5;
		pCmdUI->SetCheck(state);
	} else if (m_selectStatus == NetView::link) {
		BOOL state = GetDocument()->getSelectedLinkLineStyle() == PS_SOLID &&
			         GetDocument()->getSelectedLinkWidth() == 5;
		pCmdUI->SetCheck(state);
	}
}

void NetView::OnSetLineDot() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	bool bDrwAll = false;
	switch (m_selectStatus) {
	case NetView::single:
		GetDocument()->setSelectedNodeLineWidth(0);
		GetDocument()->setSelectedNodeLineStyle(PS_DOT);
		break;
	case NetView::link:
		GetDocument()->setSelectedLinkWidth(0, bDrwAll);
		GetDocument()->setSelectedLinkLineStyle(PS_DOT, bDrwAll);
		break;
	case NetView::multi:
		GetDocument()->setSelectedNodeLineWidth(0);
		GetDocument()->setSelectedNodeLineStyle(PS_DOT);
		GetDocument()->setSelectedLinkWidth(0, bDrwAll);
		GetDocument()->setSelectedLinkLineStyle(PS_DOT, bDrwAll);
		break;
	}
}

void NetView::OnUpdateSetLineDot(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	if (m_selectStatus == NetView::single) {
		BOOL state = GetDocument()->getSelectedNodeLineStyle() == PS_DOT &&
			         GetDocument()->getSelectedNodeLineWidth() == 0;
		pCmdUI->SetCheck(state);
	} else if (m_selectStatus == NetView::link) {
		BOOL state = GetDocument()->getSelectedLinkLineStyle() == PS_DOT &&
			         GetDocument()->getSelectedLinkWidth() == 0;
		pCmdUI->SetCheck(state);
	}
}

void NetView::OnSetMultiLine() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	GetDocument()->setSelectedNodeMultiLine(true);
}

void NetView::OnUpdateSetMultiLine(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->SetCheck(GetDocument()->isSelectedNodeMultiLine() && m_selectStatus == NetView::single);
	pCmdUI->Enable(m_selectStatus == NetView::single || m_selectStatus == NetView::multi);
}

void NetView::OnSetSingleLine() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	GetDocument()->setSelectedNodeMultiLine(false);
}

void NetView::OnUpdateSetSingleLine(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->SetCheck(!GetDocument()->isSelectedNodeMultiLine() && m_selectStatus == NetView::single);
	pCmdUI->Enable(m_selectStatus == NetView::single || m_selectStatus == NetView::multi);
}

void NetView::OnAddRect() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (m_addMode != NetView::rect) {
		m_addMode = NetView::rect;
	} else if (m_addMode == NetView::rect) {
		m_addMode = NetView::normal;
	}
}

void NetView::OnUpdateAddRect(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting && !m_bGrasp && !m_bZooming);
	pCmdUI->SetCheck(m_addMode == NetView::rect);
}

void NetView::OnAddRoundRect() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (m_addMode != NetView::rRect) {
		m_addMode = NetView::rRect;
	} else if (m_addMode == NetView::rRect) {
		m_addMode = NetView::normal;
	}
}

void NetView::OnUpdateAddRoundRect(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting && !m_bGrasp && !m_bZooming);
	pCmdUI->SetCheck(m_addMode == NetView::rRect);
}

void NetView::OnAddArc() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (m_addMode != NetView::arc) {
		m_addMode = NetView::arc;
	} else if (m_addMode == NetView::arc) {
		m_addMode = NetView::normal;
	}
}

void NetView::OnUpdateAddArc(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting && !m_bGrasp && !m_bZooming);
	pCmdUI->SetCheck(m_addMode == NetView::arc);
}

void NetView::addNode(const CPoint &logPt, const CPoint& screenPt, const CString& s)
{
	CInpcnDlg dlg;
	CPoint curPt = GetScrollPosition();
	
	dlg.m_iniPt.x = screenPt.x;
	dlg.m_iniPt.y = screenPt.y;
	if (s != "") {
		dlg.m_strcn = s;
	}
	
	if (dlg.DoModal() != IDOK) {
		m_addMode = NetView::normal;
		return;
	}
	if (dlg.m_strcn == "") {
		m_addMode = NetView::normal;
		return;
	}
	GetDocument()->disableUndo();
	if (m_addMode == NetView::rect) {
		GetDocument()->addNodeRect(dlg.m_strcn, logPt);
	} else if (m_addMode == NetView::arc) {
		GetDocument()->addNodeArc(dlg.m_strcn, logPt);
	} else if (m_addMode == NetView::rRect) {
		GetDocument()->addNodeRoundRect(dlg.m_strcn, logPt);
	} else if (m_addMode == NetView::label) {
		GetDocument()->addNodeRect(dlg.m_strcn, logPt, true, true);
	}
	m_addMode = NetView::normal;
}

void NetView::OnAddLink1() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (m_addMode == NetView::link1) {
		m_addMode = NetView::normal;
	} else {
		m_addMode = NetView::link1;
	}
}

void NetView::OnUpdateAddLink1(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting && !m_bGrasp && !m_bZooming);
	pCmdUI->SetCheck(m_addMode == NetView::link1);	
}

void NetView::OnAddLink2() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (m_addMode == NetView::link2) {
		m_addMode = NetView::normal;
	} else {
		m_addMode = NetView::link2;
	}
}

void NetView::OnUpdateAddLink2(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting && !m_bGrasp && !m_bZooming);
	pCmdUI->SetCheck(m_addMode == NetView::link2);
}

void NetView::OnSetLinkInfo() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	setLinkInfo();
}

void NetView::setLinkInfo()
{
	bool bDrwAll = false;

	CString sFrom, sTo, comment;
	int arrowType = 0;
	GetDocument()->getSelectedLinkInfo(sFrom, sTo, comment, arrowType, bDrwAll);
	LinkInfoDlg dlg;
	dlg.strFrom = sFrom;
	dlg.strTo = sTo;
	dlg.strComment = comment;
	dlg.styleArrow = arrowType;
	dlg.lineWidth = GetDocument()->getSelectedLinkWidth(bDrwAll);
	dlg.colorLine = GetDocument()->getSelectedLinkLineColor(bDrwAll);
	dlg.styleLine = GetDocument()->getSelectedLinkLineStyle(bDrwAll);
	GetDocument()->getSelectedLinkFont(dlg.lf, bDrwAll);
	
	if (dlg.DoModal() != IDOK) return;
	GetDocument()->disableUndo();
	GetDocument()->backUpUndoLinks();
	
	GetDocument()->setSelectedLinkInfo(dlg.strComment, dlg.styleArrow, bDrwAll);
	GetDocument()->setSelectedLinkFont(dlg.lf, bDrwAll);
	GetDocument()->setSelectedLinkLineColor(dlg.colorLine, bDrwAll);
	GetDocument()->setSelectedLinkLineStyle(dlg.styleLine, bDrwAll);
	GetDocument()->setSelectedLinkWidth(dlg.lineWidth, bDrwAll);
}

void NetView::OnUpdateSetLinkInfo(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(m_selectStatus == NetView::link);
}

void NetView::OnDelete() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	GetDocument()->disableUndo();
	if (m_selectStatus == NetView::single) {
		bool bDrwAll = false;
		CRect old = GetDocument()->getRelatedBound(bDrwAll); adjustRedrawBound(old);
		GetDocument()->deleteSelecedNode();
		InvalidateRect(old);
	} else if (m_selectStatus == NetView::link) {
		CString s = '<' + GetDocument()->getSelectedLinkLabel() + ">\n" + "削除しますか";
		if (MessageBox(s, "リンクの削除", MB_YESNO) != IDYES) return;
		GetDocument()->deleteSelectedLink();
	}
}

void NetView::OnUpdateDelete(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(m_selectStatus == NetView::single &&  GetDocument()->canDeleteNode() ||
		           m_selectStatus == NetView::link);
}

void NetView::OnAutoLayout() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (m_bLayouting) {
		stopLayouting();
		return;
	}
	
	if (!m_bLayouting) {
	//	int ret = MessageBox("自動レイアウトを開始しますか？\nあらかじめ文書を保存しておくことをお勧めします",
	//		"自動レイアウトの開始", MB_OKCANCEL);
	//	if (ret	== IDCANCEL) return;
		m_bLayouting = true;
		CSize sz(GetDocument()->getMaxPt().x, GetDocument()->getMaxPt().y);
		bool bDrwAll = false;
		GetDocument()->backUpUndoNodes2(bDrwAll);
		GetDocument()->backUpUndoLinks();
		
		CRelaxThrd* pRelaxThrd = new CRelaxThrd(this, m_pDC->GetSafeHdc(), sz, false, GetScrollPosition());
		pRelaxThrd->m_pThreadParams = NULL;
		GetDocument()->setNodeRelax(pRelaxThrd, bDrwAll);
		if (!pRelaxThrd->CreateThread(CREATE_SUSPENDED))
		{
			AfxMessageBox("Cannnot Create Thread");
			delete pRelaxThrd;
			return;
		}
		
		// If you want to make the sample more sprightly, set the thread priority here
		// a little higher. It has been set at idle priority to keep from bogging down
		// other apps that may also be running.
		VERIFY(pRelaxThrd->SetThreadPriority(THREAD_PRIORITY_IDLE));
		// Now the thread can run wild
		m_relaxStack.push(pRelaxThrd);
		Invalidate();
		
		pRelaxThrd->ResumeThread();
		
	/*	CRelaxThrd* pRelaxThrd 
			= (CRelaxThrd*)AfxBeginThread(RUNTIME_CLASS(CRelaxThrd), THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
		m_relaxStack.push(pRelaxThrd);
		Invalidate();
		pRelaxThrd->ResumeThread();*/
	}
}

void NetView::OnUpdateAutoLayout(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	bool bDrwAll = false;
	pCmdUI->Enable(GetDocument()->linkExist(bDrwAll) && m_addMode == NetView::normal && !m_bGrasp);
	pCmdUI->SetCheck(m_bLayouting);
}

void NetView::stopLayouting()
{
	if (m_bLayouting) {
		m_bLayouting = false;
		CRelaxThrd* pRelaxThrd = m_relaxStack.top();
		GetDocument()->setResultRelax(pRelaxThrd->bounds);
		SetEvent(pRelaxThrd->m_hEventKill);
		Sleep(10);
		m_selectRect = CRect(0, 0, 0, 0);
		adjustScrollArea();
		Invalidate();
	}
}

void NetView::adjustRedrawBound(CRect &rc)
{
	rc.InflateRect(5, 5);
	ViewLPtoDP(rc);
}

void NetView::OnSetNodeProp() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	setNodeProp();
}

void NetView::OnUpdateSetNodeProp(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(m_selectStatus == NetView::single || m_selectStatus == NetView::multi);
}

void NetView::setNodeProp()
{
	nodePropDlg dlg;
	dlg.colorLine = GetDocument()->getSelectedNodeLineColor();
	dlg.colorFill = GetDocument()->getSelectedNodeBrsColor();
	dlg.colorFont = GetDocument()->getSelectedNodeFontColor();
	dlg.lineWidth = GetDocument()->getSelectedNodeLineWidth();
	dlg.styleLine = GetDocument()->getSelectedNodeLineStyle();
	dlg.bMultiLine = GetDocument()->isSelectedNodeMultiLine();
	dlg.m_bNoBrush = !GetDocument()->isSelectedNodeFilled();
	int shape = GetDocument()->getSelectedNodeShape();
	if (shape == iNode::rectangle) {
		dlg.m_shape = 0;
	} else if (shape == iNode::roundRect) {
		dlg.m_shape = 1;
	} else if (shape == iNode::arc) {
		dlg.m_shape = 2;
	}
	bool mline = GetDocument()->isSelectedNodeMultiLine();
	if (mline) {
		dlg.m_TLine = 1;
	} else {
		dlg.m_TLine = 0;
	}
	
	int ts = GetDocument()->getSelectedNodeTextStyle();
	if (ts == iNode::notext) {
		dlg.m_TLine = 2;
	}
	
	if (ts == iNode::s_cc || ts == iNode::s_tc || ts == iNode::s_bc || ts == iNode::m_c) {
		dlg.horiz = 1;
	} else if (ts == iNode::s_cl || ts == iNode::s_tl || ts == iNode::s_bl || ts == iNode::m_l) {
		dlg.horiz = 0;
	} else if (ts == iNode::s_cr || ts == iNode::s_tr || ts == iNode::s_br || ts == iNode::m_r) {
		dlg.horiz = 2;
	} else {
		dlg.horiz = 1;
	}
	
	if (ts == iNode::s_tc || ts == iNode::s_tl || ts == iNode::s_tr) {
		dlg.vert = 0;
	} else if (ts == iNode::s_cc || ts == iNode::s_cl || ts == iNode::s_cr) {
		dlg.vert = 1;
	} else if (ts == iNode::s_bc || ts == iNode::s_bl || ts == iNode::s_br) {
		dlg.vert = 2;
	} else {
		dlg.vert = 1;
	}
	dlg.m_strLabel = GetDocument()->getSelectedNodeLabel();
	GetDocument()->getSelectedNodeFont(dlg.lf);
	
	if (dlg.DoModal() != IDOK) return;
	GetDocument()->backUpUndoNodes();
	
	GetDocument()->setSelectedNodeLineColor(dlg.colorLine);
	GetDocument()->setSelectedNodeBrush(dlg.colorFill);
	GetDocument()->setSelectedNodeFontColor(dlg.colorFont);
	GetDocument()->setSelectedNodeLineWidth(dlg.lineWidth);
	GetDocument()->setSelectedNodeLineStyle(dlg.styleLine);
	if (dlg.m_bNoBrush) {
		GetDocument()->setSelectedNodeNoBrush(FALSE);
	} else {
		GetDocument()->setSelectedNodeNoBrush(TRUE);
	}
	if (dlg.m_shape == 0) {
		GetDocument()->setSelectedNodeShape(iNode::rectangle);
	} else if (dlg.m_shape == 1) {
		GetDocument()->setSelectedNodeShape(iNode::roundRect);
	} else if (dlg.m_shape == 2) {
		GetDocument()->setSelectedNodeShape(iNode::arc);
	}
	if (dlg.m_TLine == 1) {
		GetDocument()->setSelectedNodeMultiLine();
	} else if (dlg.m_TLine == 0) {
		GetDocument()->setSelectedNodeMultiLine(false);
	}
	if (dlg.m_TLine == 2) {
		GetDocument()->setSelectedNodeTextStyle(iNode::notext);
	}
	
	if (dlg.m_TLine == 0) {
		if (dlg.vert == 0 && dlg.horiz == 0) {
			GetDocument()->setSelectedNodeTextStyle(iNode::s_tl);
		} else if (dlg.vert == 1 && dlg.horiz == 0) {
			GetDocument()->setSelectedNodeTextStyle(iNode::s_cl);
		} else if (dlg.vert == 2 && dlg.horiz == 0) {
			GetDocument()->setSelectedNodeTextStyle(iNode::s_bl);
		} else if (dlg.vert == 0 && dlg.horiz == 1) {
			GetDocument()->setSelectedNodeTextStyle(iNode::s_tc);
		} else if (dlg.vert == 1 && dlg.horiz == 1) {
			GetDocument()->setSelectedNodeTextStyle(iNode::s_cc);
		} else if (dlg.vert == 2 && dlg.horiz == 1) {
			GetDocument()->setSelectedNodeTextStyle(iNode::s_bc);
		} else if (dlg.vert == 0 && dlg.horiz == 2) {
			GetDocument()->setSelectedNodeTextStyle(iNode::s_tr);
		} else if (dlg.vert == 1 && dlg.horiz == 2) {
			GetDocument()->setSelectedNodeTextStyle(iNode::s_cr);
		} else if (dlg.vert == 2 && dlg.horiz == 2) {
			GetDocument()->setSelectedNodeTextStyle(iNode::s_br);
		}
	} else if (dlg.m_TLine == 1) {
		if (dlg.horiz == 0) {
			GetDocument()->setSelectedNodeTextStyle(iNode::m_l);
		} else if (dlg.horiz == 1) {
			GetDocument()->setSelectedNodeTextStyle(iNode::m_c);
		} else if (dlg.horiz == 2) {
			GetDocument()->setSelectedNodeTextStyle(iNode::m_r);
		}
	}
	GetDocument()->setSelectedNodeLabel(dlg.m_strLabel);
	GetDocument()->setSelectedNodeFont(dlg.lf);
}

void NetView::OnGraspMode() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	m_bGrasp = !m_bGrasp;
}

void NetView::OnUpdateGraspMode(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	CPoint maxPt;
	maxPt = GetDocument()->getMaxPt();
	CRect rc; CScrollView::GetWindowRect(&rc);
	
	pCmdUI->Enable((rc.Width() < (int)(maxPt.x*m_fZoomScale)
		         || rc.Height() < (int)(maxPt.y*m_fZoomScale)) 
		         && !m_bLayouting && m_addMode == NetView::normal);
	pCmdUI->SetCheck(m_bGrasp);
}

void NetView::OnSize(UINT nType, int cx, int cy) 
{
	CScrollView::OnSize(nType, cx, cy);
	
	// TODO: この位置にメッセージ ハンドラ用のコードを追加してください
	if (m_bGrasp) {
		m_bGrasp = false;
	}
}

void NetView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	if (m_bZooming) {
		CPoint logPt = point; ViewDPtoLP(&logPt);
		ZoomIn(&logPt, 0.1f);
		return;
	}
	
	if (m_selectStatus == NetView::single) {
		setNodeProp();
	} else if (m_selectStatus == NetView::link) {
		setLinkInfo();
	} else if (m_selectStatus == NetView::none) {
		CPoint logPt = point; ViewDPtoLP(&logPt);
		CPoint spt(point);
		ClientToScreen(&spt);
		m_addMode = NetView::rect;
		addNode(logPt, spt);
	}
	CScrollView::OnLButtonDblClk(nFlags, point);
}

void NetView::OnLinkStraight() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	bool bDrwAll = false;
	CPoint pt(0, 0);
	GetDocument()->setSelectedLinkCurve(pt, false, bDrwAll);
}

void NetView::OnUpdateLinkStraight(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	bool bDrwAll = false;
	pCmdUI->Enable(GetDocument()->isSelectedLinkCurved(bDrwAll));
}

CRect NetView::getCurveBound(const CPoint &pt)
{
	CPoint start, end;
	bool bDrwAll = false;
	GetDocument()->getSelectedLinkPts(start, end, bDrwAll);
	CRect rc1(start, pt); rc1.NormalizeRect();
	CRect rc2(pt, end);   rc2.NormalizeRect();
	return rc1 | rc2;
}

void NetView::copyMFtoClpbrd()
{
	HENHMETAFILE hmetafile = NULL;
	CClientDC dc(this);
	if (m_selectStatus == NetView::none || m_selectStatus == NetView::link) {
		CRect selRect;
		CRect allR;
		allR = CRect(CPoint(0, 0), GetDocument()->getMaxPt());
		int selcnt = GetDocument()->selectNodesInBound(allR, selRect, false);
		CRect nwBound = allR; // GetDocument()->getRelatedBound(bDrwAll);
		GetDocument()->selectLinksInBound(nwBound, false);
		selRect |= GetDocument()->getSelectedLinkBound(false);
		
		CMetaFileDC mfDC;
		CPoint p1(0, 0);
		CPoint p2(nwBound.Width(), nwBound.Height());
		CRect rc(0, 0, (int)((double)(p2.x)*m_mfWidth) , (int)((double)(p2.y)*m_mfHeight));
		mfDC.CreateEnhanced(&dc, NULL, &rc, _T("iEdit"));
		
		GetDocument()->drawLinks(&mfDC, false, true);
		GetDocument()->drawNodes(&mfDC, false);
		
		hmetafile = mfDC.CloseEnhanced();
	} else if (m_selectStatus == NetView::single) {
		CMetaFileDC mfDC;
		CPoint p1(0, 0);
		CPoint p2(m_selectRect.Width(), m_selectRect.Height());
		CRect rc(0, 0, (int)((double)(p2.x)*m_mfWidth) , (int)((double)(p2.y)*m_mfHeight));
		mfDC.CreateEnhanced(&dc, NULL, &rc, _T("iEdit"));
		mfDC.SetViewportOrg(-m_selectRect.left, -m_selectRect.top);
		GetDocument()->drawNodesSelected(&mfDC, false);
		hmetafile = mfDC.CloseEnhanced();
	} else if (m_selectStatus == NetView::multi) {
		CRect selRect;
		
		int selcnt = GetDocument()->selectNodesInBound(m_selectRect, selRect, false);
		CRect nwBound = GetDocument()->getRelatedBoundAnd(false);
		GetDocument()->selectLinksInBound(nwBound, false);
		selRect |= GetDocument()->getSelectedLinkBound(false);
		
		CMetaFileDC mfDC;
		CPoint p1(0, 0);
		CPoint p2(nwBound.Width(), nwBound.Height());
		CRect rc(0, 0, (int)((double)(p2.x)*m_mfWidth) , (int)((double)(p2.y)*m_mfHeight));
		mfDC.CreateEnhanced(&dc, NULL, &rc, _T("iEdit"));
		mfDC.SetViewportOrg(-nwBound.left, -nwBound.top);
		
		GetDocument()->drawLinksSelected(&mfDC, false, true);
		GetDocument()->drawNodesSelected(&mfDC, false);
		
		hmetafile = mfDC.CloseEnhanced();
	}
	
	if (m_selectStatus == NetView::none || m_selectStatus == NetView::link) {
		CRect rc;
		GetDocument()->selectNodesInBound(CRect(0, 0, 0, 0), rc, false);
		GetDocument()->selectLinksInBound(CRect(0, 0, 0, 0), false);
	}
	
	// ------------------------------
	// クリップボードにデータをセット
	// ------------------------------
	if (hmetafile == NULL) return;
	setEMF2Clpbrd(hmetafile);
}

void NetView::OnNoBrush() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (GetDocument()->isSelectedNodeFilled()) {
		GetDocument()->setSelectedNodeNoBrush(FALSE);
	} else {
		GetDocument()->setSelectedNodeNoBrush(TRUE);
	}
}

void NetView::OnUpdateNoBrush(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(m_selectStatus == NetView::single);
	pCmdUI->SetCheck(!GetDocument()->isSelectedNodeFilled() && m_selectStatus == NetView::single);
}

void NetView::OnSetNodeBrush() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (m_selectStatus == NetView::single || m_selectStatus == NetView::multi) {
		CColorDialog dlg(GetDocument()->getSelectedNodeBrsColor());
		if (dlg.DoModal() != IDOK) return;
		GetDocument()->backUpUndoNodes();
		GetDocument()->setSelectedNodeBrush(dlg.GetColor());
	}
}

void NetView::OnUpdateSetNodeBrush(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	
}

void NetView::OnEditCopy() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (!m_selectRect.IsRectEmpty()) {
		GetDocument()->setSelectedNodeCopyOrg();
	}
	
	copyMFtoClpbrd();
}

void NetView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting);
}

void NetView::OnEditPaste() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	if (!GetDocument()->canCopyNode() && 
		(::IsClipboardFormatAvailable(CF_ENHMETAFILE) ||
		 ::IsClipboardFormatAvailable(CF_METAFILEPICT) || 
		 ::IsClipboardFormatAvailable(CF_DIB) ||
		 ::IsClipboardFormatAvailable(CF_BITMAP)
		 )) {
		 
		if (!OpenClipboard()) {
			::showLastErrorMessage();
			return;
		}
		
		CPoint ptDrop;
		if (m_ptPaste == CPoint(0, 0)) {
			ptDrop = CPoint(rand()%150, rand()%150);
		} else {
			ptDrop = m_ptPaste;
		}
		
		HENHMETAFILE hm = NULL;
		if (::IsClipboardFormatAvailable(CF_DIB) || ::IsClipboardFormatAvailable(CF_BITMAP)) {
			HBITMAP hb = (HBITMAP)::GetClipboardData(CF_BITMAP);
			CBitmap * pBitmap = CBitmap::FromHandle(hb);
			CClientDC dc(this);
			CDC memDC;
			memDC.CreateCompatibleDC(&dc);
			CBitmap* tmpBitmap = memDC.GetCurrentBitmap();
			memDC.SelectObject(pBitmap);
			
			CSize szBitmap(100, 100);
			
			BITMAP bitMap;
			pBitmap->GetBitmap(&bitMap);
			if (pBitmap != 0) {
				szBitmap.cx = bitMap.bmWidth;
				szBitmap.cy = bitMap.bmHeight;
			}
			
			CMetaFileDC* pMfDC = new CMetaFileDC;
			pMfDC->CreateEnhanced(&dc, NULL, NULL, _T("iEdit"));
			pMfDC->BitBlt(ptDrop.x, ptDrop.y, szBitmap.cx, szBitmap.cy, &memDC, 0, 0, SRCCOPY);
			pMfDC->SelectObject(tmpBitmap);
			hm = pMfDC->CloseEnhanced();
			delete pMfDC;
		} else {
			hm = (HENHMETAFILE)::GetClipboardData(CF_ENHMETAFILE);
		}
		GetDocument()->disableUndo();
		GetDocument()->addNodeMF("図形", ptDrop, iNode::MetaFile, hm);
		if (!EmptyClipboard()) {
			AfxMessageBox( "Cannot empty the Clipboard" );
			return;
		}
		CloseClipboard();
		return;
	}
	
	if (m_ptPaste == CPoint(0, 0)) {
		GetDocument()->makeCopyNode(CPoint(0, 0));
	} else {
		GetDocument()->makeCopyNode(m_ptPaste, false);
	}
	m_ptPaste = CPoint(0, 0);
	
	// ノードイメージの消去
	if (!(::IsClipboardFormatAvailable(CF_ENHMETAFILE) ||
		 ::IsClipboardFormatAvailable(CF_METAFILEPICT) ||
		 ::IsClipboardFormatAvailable(CF_DIB)) ||
		 ::IsClipboardFormatAvailable(CF_BITMAP)) {
		return;
	}
	if (!OpenClipboard()) {
		return;
	}
	if (!EmptyClipboard()) {
		return;
	}
	CloseClipboard();
}

void NetView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(GetDocument()->canCopyNode() || 
		::IsClipboardFormatAvailable(CF_ENHMETAFILE) ||
		::IsClipboardFormatAvailable(CF_METAFILEPICT)  ||
		::IsClipboardFormatAvailable(CF_DIB) ||
		::IsClipboardFormatAvailable(CF_BITMAP)
		);
}

void NetView::OnSetNodeRect() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	GetDocument()->backUpUndoNodes();
	GetDocument()->setSelectedNodeShape(iNode::rectangle);
}

void NetView::OnUpdateSetNodeRect(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	int shape = GetDocument()->getSelectedNodeShape();
	pCmdUI->Enable(shape!= iNode::rectangle && shape != iNode::MetaFile);
}

void NetView::OnSetNodeRoundRect() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	GetDocument()->backUpUndoNodes();
	GetDocument()->setSelectedNodeShape(iNode::roundRect);
}

void NetView::OnUpdateSetNodeRoundRect(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	int shape = GetDocument()->getSelectedNodeShape();
	pCmdUI->Enable(shape!= iNode::roundRect && shape != iNode::MetaFile);
}

void NetView::OnSetNodeArc() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	GetDocument()->backUpUndoNodes();
	GetDocument()->setSelectedNodeShape(iNode::arc);
}

void NetView::OnUpdateSetNodeArc(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	int shape = GetDocument()->getSelectedNodeShape();
	pCmdUI->Enable(shape!= iNode::arc && shape != iNode::MetaFile);
}

void NetView::OnFixNode() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	BOOL fix = GetDocument()->isSelectedNodeFixed();
	if (fix) {
		GetDocument()->setSelectedNodeFixed(FALSE);
	} else {
		GetDocument()->setSelectedNodeFixed();
	}
}

void NetView::OnUpdateFixNode(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->SetCheck(GetDocument()->isSelectedNodeFixed());
}

void NetView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	if (nChar == VK_ESCAPE) {
		if (m_bLinkCurving) {
			m_bLinkCurving = false;
			adjustRedrawBound(m_preRedrawBound);
			InvalidateRect(m_preRedrawBound);
		}
		if (m_bLayouting) {
			stopLayouting();
			Invalidate();
		}
	} else if (nChar == VK_TAB) {
		bool bDrwAll = false;
		iHint h;
		if (bDrwAll) {
			h.event = iHint::nextNode;
		} else {
			h.event = iHint::nextNodeSibling;
		}
		GetDocument()->UpdateAllViews(NULL, NULL, &h);
	} else if (nChar == VK_UP || nChar == VK_DOWN || nChar == VK_LEFT || nChar == VK_RIGHT) {
		m_cntUp = 0;
		m_cntDown = 0;
		m_cntLeft = 0;
		m_cntRight = 0;
	} else if (nChar == VK_CONTROL) {
		if (m_bDragRelax) {
			cancelDragRelax();
		}
	} else if (nChar >= 0x30 && nChar <= 0x5A || nChar == VK_SPACE) {
		if (!(nFlags & MK_CONTROL) && !(nFlags & MK_ALT)) {
			m_addMode = NetView::rect;
			CPoint logPt = m_ptScreen;
			logPt -= GetScrollPosition();
			ViewDPtoLP(&logPt);
			CPoint spt(m_ptScreen);
			ClientToScreen(&spt);
			spt -= GetScrollPosition();
			addNode(logPt, spt);
			m_ptScreen += CPoint((int)(30*m_fZoomScale), (int)(30*m_fZoomScale));
		}
	}
	CScrollView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void NetView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	if (m_bLayouting) {
		return;
	}
	int dif;
	if (nChar == VK_UP) {
		if (m_cntUp == 0) {
			GetDocument()->disableUndo();
			GetDocument()->backUpUndoNodes();
			GetDocument()->backUpUndoLinks();
		}
		m_cntUp++;
		if (m_bAccel) {
			dif = 1+m_cntUp/5;
		} else {
			dif = 1;
		}
		cursorMove(0, -dif);
	} else if (nChar == VK_DOWN) {
		if (m_cntDown == 0) {
			GetDocument()->disableUndo();
			GetDocument()->backUpUndoNodes();
			GetDocument()->backUpUndoLinks();
		}
		m_cntDown++;
		if (m_bAccel) {
			dif = 1+m_cntDown/5;
		} else {
			dif = 1;
		}
		cursorMove(0, dif);
	} else if (nChar == VK_LEFT) {
		if (m_cntLeft == 0) {
			GetDocument()->disableUndo();
			GetDocument()->backUpUndoNodes();
			GetDocument()->backUpUndoLinks();
		}
		m_cntLeft++;
		if (m_bAccel) {
			dif = 1+m_cntLeft/5;
		} else {
			dif = 1;
		}
		cursorMove(-dif, 0);
	} else if (nChar == VK_RIGHT) {
		if (m_cntRight == 0) {
			GetDocument()->disableUndo();
			GetDocument()->backUpUndoNodes();
			GetDocument()->backUpUndoLinks();
		}
		m_cntRight++;
		if (m_bAccel) {
			dif = 1+m_cntRight/5;
		} else {
			dif = 1;
		}
		cursorMove(dif, 0);
	}
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void NetView::cursorMove(int dx, int dy)
{
	if (m_selectStatus == NetView::none || m_selectStatus == NetView::link) return;
	CClientDC dc(this);
	
	if (m_selectRect.left + dx < 0) {
		dx = -m_selectRect.left;
	}
	if (m_selectRect.top + dy < 0) {
		dy = -m_selectRect.top;
	}
	CRect org = m_selectRect;
	CRectTracker tracker;
	m_selectRect.OffsetRect(dx, dy);
	tracker.m_rect = m_selectRect;
	dc.DPtoLP(&(tracker.m_rect));
	
	bool bDrwAll = false;
	CRect old = GetDocument()->getRelatedBound(bDrwAll); adjustRedrawBound(old);
	
	if (m_selectStatus == NetView::single) {
		tracker.m_nStyle = CRectTracker::resizeInside;
		GetDocument()->setSelectedNodeBound(m_selectRect);
		int moveX = m_selectRect.left - org.left;
		int moveY = m_selectRect.top - org.top;
		GetDocument()->moveSelectedLink(CSize(moveX, moveY));
	} else if (m_selectStatus == NetView::multi) {
		tracker.m_nStyle = CRectTracker::hatchedBorder | CRectTracker::resizeInside;
		GetDocument()->moveSelectedNode(CSize(dx, dy));
		GetDocument()->moveSelectedLink(CSize(dx, dy));
	}
	CRect rdnw = GetDocument()->getRelatedBound(bDrwAll); adjustRedrawBound(rdnw);
	
	CRect rc;
	if (!rc.IntersectRect(old, rdnw)) {
		InvalidateRect(old);
		InvalidateRect(rdnw);
	} else {
		CRect rdRec = old | rdnw;
		InvalidateRect(rdRec);
	}
	
	CPoint maxPt;
	maxPt = GetDocument()->getMaxPt();
	adjustScrollArea();
}

void NetView::OnNormalCursor() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	m_addMode = NetView::normal;
}

void NetView::OnUpdateNormalCursor(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->SetCheck(m_addMode == NetView::normal && !m_bGrasp && !m_bZooming);
}

int NetView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: この位置に固有の作成用コードを追加してください
	m_pDC = new CClientDC(this);
	m_bAccel = AfxGetApp()->GetProfileInt(REGS_OTHER, "Accel Move", FALSE);
	m_pShapesDlg = new shapesDlg;
	m_pShapesDlg->Create("", "", SW_HIDE, CRect(0, 0, 0, 0), this, IDD_SHAPES);	
	doColorSetting();
	setMFSize();
	
	return 0;
}

void NetView::KillRelaxThrd()
{
	// tell all threads to shutdown
	while (m_relaxStack.size() != 0) {
		CRelaxThrd* pThrd = m_relaxStack.top();
		SetEvent(pThrd->m_hEventKill);
		Sleep(2);
	//	delete pThrd;
		m_relaxStack.pop();
	}
}

void NetView::OnDestroy() 
{
	CScrollView::OnDestroy();
	
	// TODO: この位置にメッセージ ハンドラ用のコードを追加してください
	KillRelaxThrd();
	m_pDC = NULL;
	m_pShapesDlg->DestroyWindow();
	delete m_pShapesDlg;
	
}

void NetView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	pDC->SetMapMode(MM_ISOTROPIC);
	CPoint pt = GetDocument()->getMaxPt();
	CRect pr = pInfo->m_rectDraw;
	
	int wx = (int)(pt.x/((pr.Width()/2)/(double)pDC->GetDeviceCaps(LOGPIXELSX)));
	int wy = (int)(pt.y/((pr.Height()/2)/(double)pDC->GetDeviceCaps(LOGPIXELSY)));
	
	
//	CString test; test.Format("%d %d", wx, wy); MessageBox(test);
	
	CSize oldWnExt = pDC->SetWindowExt(wx, wy);
	pDC->SetViewportExt(CSize(pt.x, pt.y));
	pDC->SetViewportOrg(0, 0);
	
	bool bDrwAll = false;
	GetDocument()->drawLinks(pDC, bDrwAll);
	GetDocument()->drawNodes(pDC, bDrwAll);
	
	pDC->SetWindowExt(oldWnExt);
}

BOOL NetView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TODO: 印刷ダイアログ ボックスを表示するために DoPreparePrinting を呼び出してください
	return DoPreparePrinting(pInfo);
	
//	return CScrollView::OnPreparePrinting(pInfo);
}

void NetView::OnUpdateFilePrint(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting);
}

void NetView::OnUpdateFilePrintPreview(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting);
}

void NetView::OnRandomize() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	bool bDrwAll = false;
	randAreaDlg dlg;
	dlg.m_height = 768;
	dlg.m_width = 1024;
	if (dlg.DoModal() != IDOK) return;
	GetDocument()->disableUndo();
	GetDocument()->backUpUndoNodes2(bDrwAll);
	GetDocument()->backUpUndoLinks();
	GetDocument()->randomNodesPos(CSize(dlg.m_width, dlg.m_height), bDrwAll);
	m_selectRect = CRect(0, 0, 0, 0);
	adjustScrollArea();
}

void NetView::OnUpdateRandomize(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting);
}

void NetView::adjustScrollArea()
{
//	GetDocument()->recalcArea();
	CPoint maxPt = GetDocument()->getMaxPt();	
	CSize sizeTotal;
	sizeTotal.cx = (LONG)(maxPt.x*1.05);
	sizeTotal.cy = (LONG)(maxPt.y*1.05);
	
	SetScrollSizes(MM_ANISOTROPIC, sizeTotal);
	UpdateViewport(NULL);
	SendMessage(WM_SIZE); // update時に必要
}

void NetView::OnEditSelectAll() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	selectAll();
}

void NetView::selectAll()
{
	bool bDrwAll = false;
	CRect selRect;
	CRect allR;
	allR = CRect(CPoint(0, 0), GetDocument()->getMaxPt());
	int selcnt = GetDocument()->selectNodesInBound(allR, selRect, bDrwAll);
	CRect old = m_selectRect; adjustRedrawBound(old);
	
	m_selectRect = selRect;
	if (selcnt > 1) {
		m_selectStatus = NetView::multi;
		GetDocument()->selectLinksInBound(allR, bDrwAll);
		m_selectRect |= GetDocument()->getSelectedLinkBound(bDrwAll);
	} else if (selcnt == 1) {
		m_selectStatus = NetView::single;
	}
	CRect rdRect = m_selectRect; adjustRedrawBound(rdRect);
	InvalidateRect(rdRect);
	InvalidateRect(old);
}

void NetView::OnUpdateEditSelectAll(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting && m_addMode == NetView::normal);
}

void NetView::OnRefreshNetView() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	GetDocument()->setConnectPoint();
	Invalidate();
}

void NetView::OnUpdateRefreshNetView(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting);
}

void NetView::OnShowNodeShapes() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	m_pShapesDlg->ShowWindow(SW_SHOWNORMAL);
	m_pShapesDlg->SetFocus();
}

void NetView::OnUpdateShowNodeShapes(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	
}

LRESULT NetView::OnAddMetaFileID(UINT wParam, LONG lParam)
{
	if (m_bLayouting) return 0;
	if (m_selectStatus == NetView::multi || m_selectStatus == NetView::link) return 0;
	
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	
	GetDocument()->disableUndo();
	if (m_selectStatus == NetView::none) {
		GetDocument()->addNodeMF("図形", CPoint(rand()%150, rand()%150), iNode::MetaFile, pApp->m_hMetaFiles[(int)wParam]);
	} else if (m_selectStatus == NetView::single) {
		GetDocument()->backUpUndoNodes();
		GetDocument()->setSelectedNodeShape(iNode::MetaFile, (int)wParam);
	}
	return 0;
}

LRESULT NetView::OnHideShapesDlg(UINT wParam, LONG lParam)
{
	m_pShapesDlg->ShowWindow(SW_HIDE);
	return 0;
}

void NetView::OnEditReplace() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	
}

void NetView::hideModeless()
{
	m_pShapesDlg->ShowWindow(SW_HIDE);
}

LRESULT NetView::OnRegNodeMetaFile(UINT wParam, LONG lParam)
{
	if (m_bLayouting) return 0;
	if (m_selectStatus == NetView::multi || m_selectStatus == NetView::link) return 0;
	if (GetDocument()->getSelectedNodeShape()  == iNode::MetaFile) {
		m_pShapesDlg->regNodeShape(GetDocument()->getSelectedNodeMetaFile());
	} else {
		AfxMessageBox("シェイプではありません");
	}
	return 0;
}

void NetView::OnCopyToClipbrd() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	CPoint p1(0, 0);
	CPoint p2 = GetDocument()->getMaxPt();
	
	bool bDrwAll = false;
	
	CMetaFileDC* pMfDC = new CMetaFileDC();
	CRect rc(0, 0, (int)(abs(p2.x - p1.x)*m_mfWidth) , (int)(abs(p2.y - p1.y)*m_mfHeight));
	CClientDC dc(this);
	pMfDC->CreateEnhanced(&dc, NULL /*"C:\\test.emf"*/, &rc, _T("iEdit"));
	pMfDC->SetAttribDC(dc);

	GetDocument()->drawLinks(pMfDC, bDrwAll, true);
	GetDocument()->drawNodes(pMfDC, bDrwAll);
	HENHMETAFILE hmetafile = pMfDC->CloseEnhanced();
	delete pMfDC;
	
	// クリップボードにデータをセット
	setEMF2Clpbrd(hmetafile);
}

void NetView::OnUpdateCopyToClipbrd(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting);
}

void NetView::OnEditUndo() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	
	
	// ビューポート調整に問題あり
	
	GetDocument()->resumeUndoNodes();
	GetDocument()->resumeUndoLinks();
	m_selectRect = GetDocument()->getSelectedNodeRect();
	m_selectStatus = NetView::single;
	
	Invalidate();
	
	CPoint maxPt;
	maxPt = GetDocument()->getMaxPt();
	adjustScrollArea();
}

void NetView::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting && GetDocument()->canResumeUndo());
}

void NetView::OnAdjustTop() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	adjustNodesEnd("top");
}

void NetView::OnUpdateAdjustTop(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting && m_selectStatus == NetView::multi);
}

void NetView::OnAdjustBottom() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	adjustNodesEnd("bottom");
}

void NetView::OnUpdateAdjustBottom(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting && m_selectStatus == NetView::multi);
}

void NetView::OnAdjustLeft() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	adjustNodesEnd("left");
}

void NetView::OnUpdateAdjustLeft(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting && m_selectStatus == NetView::multi);
}

void NetView::OnAdjustRight() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	adjustNodesEnd("right");
}

void NetView::OnUpdateAdjustRight(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting && m_selectStatus == NetView::multi);
}

void NetView::adjustNodesEnd(const CString& side)
{
	bool bDrwAll = false;
	CRect old = GetDocument()->getRelatedBound(bDrwAll); adjustRedrawBound(old);
	GetDocument()->backUpUndoNodes();
	GetDocument()->backUpUndoLinks();

	GetDocument()->adjustNodesEnd(side, m_selectRect, bDrwAll);
	
	CRect nwrd = GetDocument()->getRelatedBoundAnd(bDrwAll);
	m_selectRect = nwrd;
	adjustRedrawBound(nwrd);
	CRect rc;
	if (!rc.IntersectRect(old, nwrd)) {
		InvalidateRect(old);
		InvalidateRect(nwrd);
	} else {
		CRect rd = old | nwrd;
		InvalidateRect(rd);
	}
	
	adjustScrollArea();
}

void NetView::OnSameHeight() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	sameNodesSize("height");
}

void NetView::OnUpdateSameHeight(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting && m_selectStatus == NetView::multi);
}

void NetView::OnSameRect() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	sameNodesSize("rect");
}

void NetView::OnUpdateSameRect(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting && m_selectStatus == NetView::multi);
}

void NetView::OnSameWidth() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	sameNodesSize("width");
}

void NetView::OnUpdateSameWidth(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting && m_selectStatus == NetView::multi);
}

void NetView::sameNodesSize(const CString &strSize)
{
	bool bDrwAll = false;
	CRect old = GetDocument()->getRelatedBound(bDrwAll); adjustRedrawBound(old);
	GetDocument()->backUpUndoNodes();
	GetDocument()->backUpUndoLinks();
	
	GetDocument()->sameNodesSize(strSize, bDrwAll);
	
	CRect nwrd = GetDocument()->getRelatedBoundAnd(bDrwAll);
	m_selectRect = nwrd;
	adjustRedrawBound(nwrd);
	CRect rc;
	if (!rc.IntersectRect(old, nwrd)) {
		InvalidateRect(old);
		InvalidateRect(nwrd);
	} else {
		CRect rd = old | nwrd;
		InvalidateRect(rd);
	}
	
	adjustScrollArea();
}

void NetView::OnReverseLinkDirection() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	bool bDrwAll = false;
	GetDocument()->backUpUndoLinks();
	GetDocument()->setSelectedLinkReverse(bDrwAll);
}

void NetView::OnUpdateReverseLinkDirection(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	
}

void NetView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	pDC->SetMapMode(m_nMapMode);
	pDC->SetWindowExt(m_totalLog);  //Set up the logical window
	
	//Now figure out the origin for the viewport, depends on
	//This code is from CSCrollView
	CPoint ptVpOrg;
	pDC->SetViewportExt(m_totalDev); // in device coordinates
	
	// by default shift viewport origin in negative direction of scroll
	ASSERT(pDC->GetWindowOrg() == CPoint(0,0));
	ptVpOrg = -GetDeviceScrollPosition();
	
	// Set the new viewport origin, call CView for printing behavior
	pDC->SetViewportOrg(ptVpOrg);
	CView::OnPrepareDC(pDC, pInfo);
	
//	CScrollView::OnPrepareDC(pDC, pInfo);
}

void NetView::SetScrollSizes(int nMapMode, SIZE sizeTotal, const SIZE &sizePage, const SIZE &sizeLine)
{
	// Set up the defaults
	if (sizeTotal.cx == 0 && sizeTotal.cy == 0){
		sizeTotal.cx = 1;
		sizeTotal.cy = 1;
	}
	
	m_nMapMode    = MM_ANISOTROPIC; // mandatory for arbitrary scaling
	m_totalLog    = sizeTotal;
	
	// Setup default Viewport extent to be conversion of Window extent
	// into device units.
	
	//BLOCK for DC
	{
	CWindowDC dc(NULL);
	dc.SetMapMode(m_nMapMode);
	
	// total size
	m_totalDev = m_totalLog;
		dc.LPtoDP((LPPOINT)&m_totalDev);
	} // Release DC here
	
	m_szOrigTotalDev = m_totalDev;
	m_szOrigPageDev  = sizePage;
	m_szOrigLineDev  = sizeLine;
	if (isScaleChanged()) {
		ReCalcBars(); // 毎回全再描画になってしまうので条件付き
	}
//	CScrollView::SetScrollSizes(nMapMode, sizeTotal, sizePage, sizeLine);
}

void NetView::SetZoomLevel(double fNewScale)
{
	m_fZoomScalePrev = m_fZoomScale;
	m_fZoomScale = fNewScale;
}

double NetView::GetZoomLevel()
{
	return m_fZoomScale;
}

void NetView::ZoomIn(CPoint *point, double delta)
{
	if (m_fZoomScale >= 5.0) {
		m_fZoomScale = 5.0;
	}
	SetZoomLevel(m_fZoomScale + delta);
	UpdateViewport(point);
}

void NetView::ZoomOut(CPoint *point, double delta)
{
	if (m_fZoomScale <= 0.01) {
		m_fZoomScale = 0.01;
	}
	SetZoomLevel(m_fZoomScale - delta);
	UpdateViewport(point);
}

void NetView::ReCalcBars()
{
	{  // BLOCK for DC
		CWindowDC dc(NULL);
		dc.SetMapMode(m_nMapMode);
		
		// Calculate new device units for scrollbar
		// Start with original logical units from SetScrollPos
		m_pageDev = m_szOrigPageDev;
		dc.LPtoDP((LPPOINT)&m_pageDev);
		m_lineDev = m_szOrigLineDev;
		dc.LPtoDP((LPPOINT)&m_lineDev);
	} // Free DC
	
	// Make sure of the range
	if (m_pageDev.cy < 0)  m_pageDev.cy = -m_pageDev.cy;
	if (m_lineDev.cy < 0)  m_lineDev.cy = -m_lineDev.cy;
	
	// If none specified - use one tenth, Just like CScrollView
	  
	if (m_pageDev.cx == 0) m_pageDev.cx = m_totalDev.cx / 10;
	if (m_pageDev.cy == 0) m_pageDev.cy = m_totalDev.cy / 10;
	if (m_lineDev.cx == 0) m_lineDev.cx = m_pageDev.cx  / 10;
	if (m_lineDev.cy == 0) m_lineDev.cy = m_pageDev.cy  / 10;
	
	// Now update the scrollbars
	if (m_hWnd != NULL) {
		if (isScaleChanged()) {
			UpdateBars();
			Invalidate(TRUE); // Zoom scale changed, redraw all
		}
	}

}

CPoint NetView::GetLogicalCenterPoint()
{
	CPoint pt;
	CRect rect;
	
	GetClientRect(&rect);
	pt.x = (rect.Width()  / 2);
	pt.y = (rect.Height() / 2);
	
	ViewDPtoLP(&pt);
	return pt;
}

void NetView::ViewDPtoLP(LPPOINT lpPoints, int nCount)
{
	CWindowDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(lpPoints, nCount);
}

void NetView::ViewLPtoDP(LPPOINT lpPoints, int nCount)
{
	CWindowDC dc(this);
	OnPrepareDC(&dc);
	dc.LPtoDP(lpPoints, nCount);
}

void NetView::ViewLPtoDP(CRect& rect)
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.LPtoDP(&rect);
}


void NetView::ViewDPtoLP(CRect &rect)
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&rect);
}

void NetView::UpdateViewport(CPoint *ptNewCenter)
{
	CPoint ptCenter;
	 
	if (!ptNewCenter)
		ptCenter = GetLogicalCenterPoint();
	else 
		ptCenter = *ptNewCenter;
     
	// Modify the Viewport extent
	m_totalDev.cx = (int) ((float) m_szOrigTotalDev.cx * m_fZoomScale);
	m_totalDev.cy = (int) ((float) m_szOrigTotalDev.cy * m_fZoomScale);
	ReCalcBars();
	
	// Set the current center point.
	if (ptNewCenter != NULL) {
		CenterOnLogicalPoint(ptCenter);
	}
}

bool NetView::isScaleChanged() const
{
	return (m_fZoomScale != m_fZoomScalePrev && m_fZoomScale != 1.0f);
}

BOOL NetView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	if (m_bZooming) {
		CPoint logPt = pt; ViewDPtoLP(&logPt);
		if (zDelta < 0) {
			ZoomIn(/*NULL*/&pt, 0.05);
		} else {
			ZoomOut(/*NULL*/&pt, 0.05);
		}
	}
	
	if (zDelta < 0) {
		SendMessage(WM_VSCROLL, SB_PAGEDOWN);
	} else {
		SendMessage(WM_VSCROLL, SB_PAGEUP);
	}
	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}

void NetView::CenterOnLogicalPoint(CPoint ptCenter)
{
	ViewLPtoDP(&ptCenter);
	CScrollView::CenterOnPoint(ptCenter);
}

void NetView::OnUpdateZoomMode(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->SetCheck(m_bZooming);
}

void NetView::OnZoomMode() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	m_bZooming = !m_bZooming;
}

void NetView::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	if (m_bZooming) {
		CPoint logPt = point; ViewDPtoLP(&logPt);
		ZoomOut(&logPt);
	}
	CScrollView::OnRButtonDblClk(nFlags, point);
}

void NetView::OnSelchangeDropdown() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	CComboBox* pComboZoom = pFrame->getZoomComboBox();
	int index = pComboZoom->GetCurSel();
	double zoomScale = m_fZoomScale;
	switch (index) {
	case 0:
		zoomScale = 0.1;
		break;
	case 1:
		zoomScale = 0.25;
		break;
	case 2:
		zoomScale = 0.50;
		break;
	case 3:
		zoomScale = 0.75;
		break;
	case 4:
		zoomScale = 1.0;
		break;
	case 5:
		zoomScale = 1.5;
		break;
	case 6:
		zoomScale = 2.0;
		break;
	case 7:
		zoomScale = 3.0;
		break;
	case 8:
		CPoint maxPt;
		maxPt = GetDocument()->getMaxPt();
		CRect rc; CScrollView::GetWindowRect(&rc);
		if (rc.Width() < (int)(maxPt.x*m_fZoomScale) ||
			rc.Height() < (int)(maxPt.y*m_fZoomScale)) {
			
			float zoomWidth = (float)(((double)rc.Width())/((double)maxPt.x));
			float zoomHeight = (float)(((double)rc.Height())/((double)maxPt.y));
			zoomScale = (zoomWidth < zoomHeight) ? zoomWidth : zoomHeight;
		}		
		break;
	}
	SetZoomLevel(zoomScale);
	UpdateViewport(NULL);
	
	if (zoomScale = 1.0f) { // 等倍の時だけ独自にスクロールバーを再描画
		ReCalcBars();
		UpdateBars();
		Invalidate();
	}
	this->SetFocus();
	if (index == 8) {
		ScrollToPosition(CPoint(0, 0));
	}
}

void NetView::doColorSetting()
{
	m_bkColor = AfxGetApp()->GetProfileInt(REGS_FRAME, "Net bgColor", RGB(255, 255, 255));
}

void NetView::OnExportSvg() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	CString path = GetDocument()->GetPathName();
	CString outfile;
	if (path == "") {
		outfile = GetDocument()->GetTitle();
	} else {
		char drive[_MAX_DRIVE];
		char dir[_MAX_DIR];
		char fname[_MAX_FNAME];
		char ext[_MAX_EXT];
		_splitpath_s(path, drive, dir, fname, ext );
		CString title(fname);
		outfile = title;
	}
	
	char szFilters[] = "SVGﾌｧｲﾙ (*.svg)|*.svg";
	CFileDialog dlg(FALSE, "xml", outfile, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, this);
	if (dlg.DoModal() != IDOK) return;
	CString outfileName = dlg.GetPathName();
	
	bool bDrwAll = false;
	GetDocument()->exportSVG(bDrwAll, outfileName);
}

void NetView::OnUpdateExportSvg(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(!m_bLayouting && !m_bGrasp && !m_bZooming);
}

void NetView::setMFSize()
{
	m_mfWidth = AfxGetApp()->GetProfileInt(REGS_OTHER, "MF rWidth", 0)/10.0;
	m_mfHeight = AfxGetApp()->GetProfileInt(REGS_OTHER, "MF rHeight", 0)/10.0;
	
	if (m_mfWidth != 0 && m_mfHeight != 0) return; // レジストリの値を採用
	
	MfSizer sizer;
	CSize szMF = sizer.getMFSize();
	m_mfWidth = ((double)szMF.cx)/10.0;
	m_mfHeight = ((double)szMF.cy)/10.0;
}

BOOL NetView::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	m_toolTip.RelayEvent( pMsg );
	return CScrollView::PreTranslateMessage(pMsg);
}

void NetView::OnTipDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTTDISPINFO lpttd;
	
	if (pNMHDR->code == TTN_GETDISPINFO) //意味のないif
	{
		if (!m_strTip.IsEmpty())
        {
            //表示すべき文字列はm_strTipに入っています。これが空のときは
            //表示すべきではありません。
			lpttd = (LPNMTTDISPINFO)pNMHDR;
			lpttd->lpszText = (LPSTR)(LPCTSTR)m_strTip;
		}
	}
	*pResult = 0;
}


void NetView::prepareDragRelax()
{
	GetDocument()->backUpUndoNodes2(false);
	GetDocument()->backUpUndoLinks();
	// 連鎖ノードだけを動かすためのフィルタ処理
	GetDocument()->listupChainNodes();
	GetDocument()->calcEdges();
}


void NetView::cancelDragRelax()
{
	m_bDragRelax = false;
	m_selectRect = GetDocument()->getSelectedNodeRect();
	GetDocument()->recalcArea();
	GetDocument()->SetModifiedFlag();
	Invalidate();
	adjustScrollArea();
}

void NetView::OnMButtonDown(UINT nFlags, CPoint point)
{
	CPoint logPt = point; ViewDPtoLP(&logPt);
	CRect r;
	if (GetDocument()->hitTest(logPt, r, false)) {
		m_addMode = NetView::link0;
		startLink(logPt);
	}
}

void NetView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// リンクの追加(ノード上でマウスをリリース)
	if (!m_bStartAdd) {
		CScrollView::OnRButtonUp(nFlags, point);
		return;
	}

	m_bStartAdd = false;
	CPoint logPt = point; ViewDPtoLP(&logPt);
	
	if (GetDocument()->hitTest2(logPt)) {
		GetDocument()->setEndLink(logPt);
	} else {
		CMenu menu;
		menu.LoadMenu(IDR_CONTEXT);
		CMenu* pPopup = menu.GetSubMenu(8);
		CPoint sPt = point;
		ClientToScreen(&sPt);
		m_ptNew = logPt;
		pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, sPt.x, sPt.y, AfxGetMainWnd());
	}
	CRect rc(m_ptLinkFrom, m_ptLinkTo);
	rc.NormalizeRect();
	InvalidateRect(rc);
	
	m_addMode = NetView::normal;
}

void NetView::OnInsertChild() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	GetDocument()->disableUndo();
	CRect nwRect = GetDocument()->addNodeWithLink(iNode::rectangle, GetDocument()->getSelectedNodeKey());
	if (!nwRect.IsRectEmpty()) {
		procRenameDialog(nwRect);
	}
}

void NetView::OnUpdateInsertChild(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(m_selectStatus == NetView::single);
}

void NetView::OnInsertSibling() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	GetDocument()->disableUndo();
	CRect nwRect = GetDocument()->addNodeWithLink2(iNode::rectangle, GetDocument()->getSelectedNodeKey());
	if (!nwRect.IsRectEmpty()) {
		procRenameDialog(nwRect);
	}
}

void NetView::OnUpdateInsertSibling(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(m_selectStatus == NetView::single);
}

// 描画済みメタファイルをクリップボードにセットする
// メタファイルハンドルのdeleteはこのメソッド内で行う
void NetView::setEMF2Clpbrd(HENHMETAFILE emf)
{
	if (!OpenClipboard()) {
		::showLastErrorMessage();
		DeleteEnhMetaFile(emf);
		return;
	}
	if (!EmptyClipboard()) {
		::showLastErrorMessage();
		CloseClipboard();
		DeleteEnhMetaFile(emf);
		return;
	}
	if (::SetClipboardData(CF_ENHMETAFILE, emf) == NULL) {
		::showLastErrorMessage();
	}
	CloseClipboard();
	DeleteEnhMetaFile(emf);
}

void NetView::OnAddNodesFromCfText() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
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
	ClipText += "\n";
	CToken tok(ClipText);
	tok.SetToken("\n");
	
	if (m_ptPaste.x <= 0) {
		m_ptPaste.x = 70;
	}
	
	int initialX = m_ptPaste.x;
	while(tok.MoreTokens()) {
		CString s = tok.GetNextToken();
		int indent = CToken::GetIndent(s);
		CString s2 = CToken::TrimLeft(s);
		if (s2 != "" && s2 != "\n" && s2 != '\r') {
			CString label;
			m_ptPaste.x = initialX +60*(indent-1);
			GetDocument()->addNodeRect(s2, m_ptPaste, false);
			m_ptPaste.y  += 40;
			CPoint maxPt;
			maxPt = GetDocument()->getMaxPt();
			adjustScrollArea();
		}
	}
}

void NetView::OnUpdateAddNodesFromCfText(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(::IsClipboardFormatAvailable(CF_TEXT));
}

void NetView::OnSetLinkArrowNone() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	CString sFrom, sTo, sComment;
	int arrowType;
	GetDocument()->getSelectedLinkInfo(sFrom, sTo, sComment, arrowType);
	GetDocument()->setSelectedLinkInfo(sComment, iLink::line);
}

void NetView::OnUpdateSetLinkArrowNone(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(m_selectStatus == NetView::link && GetDocument()->getSelectedLink().getArrowStyle() != iLink::line);
}

void NetView::OnSetLinkArrowSingle() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	CString sFrom, sTo, sComment;
	int arrowType;
	GetDocument()->getSelectedLinkInfo(sFrom, sTo, sComment, arrowType);
	GetDocument()->setSelectedLinkInfo(sComment, iLink::arrow);
}

void NetView::OnUpdateSetLinkArrowSingle(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(m_selectStatus == NetView::link && GetDocument()->getSelectedLink().getArrowStyle() != iLink::arrow);
}

void NetView::OnSetLinkArrowDouble() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	CString sFrom, sTo, sComment;
	int arrowType;
	GetDocument()->getSelectedLinkInfo(sFrom, sTo, sComment, arrowType);
	GetDocument()->setSelectedLinkInfo(sComment, iLink::arrow2);
}

void NetView::OnUpdateSetLinkArrowDouble(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(m_selectStatus == NetView::link && GetDocument()->getSelectedLink().getArrowStyle() != iLink::arrow2);
}



void NetView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	HIMC hime = ::ImmGetContext(m_hWnd);
	BOOL bImeOn = ::ImmGetOpenStatus(hime);
	::ImmReleaseContext(m_hWnd, hime);
	if (isalpha(nChar) != 0 || nChar >= 0x30 && nChar <= 0x5A || nChar == VK_SPACE) {
		if (!bImeOn) {
			CString s;
			s.Format("%c", nChar);
			m_addMode = NetView::rect;
			CPoint logPt = m_ptScreen;
			logPt -= GetScrollPosition();
			ViewDPtoLP(&logPt);
			CPoint spt(m_ptScreen);
			ClientToScreen(&spt);
			spt -= GetScrollPosition();
			addNode(logPt, spt, s);
			m_ptScreen += CPoint((int)(30*m_fZoomScale), (int)(30*m_fZoomScale));
		}
	}
	CScrollView::OnChar(nChar, nRepCnt, nFlags);
}


void NetView::OnAddlinkednodeArc()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	GetDocument()->disableUndo();
	CRect nwRect = GetDocument()->addNodeWithLink(iNode::arc, GetDocument()->getSelectedNodeKey(), -1, m_ptNew, false);
	if (!nwRect.IsRectEmpty()) {
		procRenameDialog(nwRect);
	}
}

void NetView::OnUpdateAddlinkednodeArc(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
}

void NetView::OnAddlinkednodeRect()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	GetDocument()->disableUndo();
	CRect nwRect = GetDocument()->addNodeWithLink(iNode::rectangle, GetDocument()->getSelectedNodeKey(), -1, m_ptNew, false);
	if (!nwRect.IsRectEmpty()) {
		procRenameDialog(nwRect);

	}
}

void NetView::OnUpdateAddlinkednodeRect(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
}

void NetView::OnAddlinkednodeRndrect()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	GetDocument()->disableUndo();
	CRect nwRect = GetDocument()->addNodeWithLink(iNode::roundRect, GetDocument()->getSelectedNodeKey(), -1, m_ptNew, false);
	if (!nwRect.IsRectEmpty()) {
		procRenameDialog(nwRect);
	}
}

void NetView::OnUpdateAddlinkednodeRndrect(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
}

void NetView::procRenameDialog(const CRect& nodeBound)
{
	GetDocument()->recalcArea();
	adjustScrollArea();
	Invalidate();
	CInpcnDlg dlg;
	
	CPoint spt(nodeBound.CenterPoint());
	ViewLPtoDP(&spt, 1);
	ClientToScreen(&spt);
	dlg.m_iniPt = spt;
	dlg.m_strcn = GetDocument()->getSelectedNodeLabel();
	
	if (dlg.DoModal() != IDOK) return;
	if (dlg.m_strcn == "") return;
	GetDocument()->setSelectedNodeLabel(dlg.m_strcn);
}


void NetView::OnAddLabelOnly()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	if (m_addMode != NetView::label) {
		m_addMode = NetView::label;
	} else if (m_addMode == NetView::label) {
		m_addMode = NetView::normal;
	}
}

void NetView::OnUpdateAddLabelOnly(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->Enable(!m_bLayouting && !m_bGrasp && !m_bZooming);
	pCmdUI->SetCheck(m_addMode == NetView::label);
}

void NetView::OnFontEnlarge()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	GetDocument()->resizeSelectedNodeFont(true);
}

void NetView::OnUpdateFontEnlarge(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->Enable(m_selectStatus == NetView::single || m_selectStatus == NetView::multi);
}

void NetView::OnFontEnsmall()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	GetDocument()->resizeSelectedNodeFont(false);
}


void NetView::OnUpdateFontEnsmall(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->Enable(m_selectStatus == NetView::single || m_selectStatus == NetView::multi);
}

void NetView::OnSetLinkDependSingle()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	CString sFrom, sTo, sComment;
	int arrowType;
	GetDocument()->getSelectedLinkInfo(sFrom, sTo, sComment, arrowType);
	GetDocument()->setSelectedLinkInfo(sComment, iLink::depend);
}

void NetView::OnUpdateSetLinkDependSingle(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->Enable(m_selectStatus == NetView::link && GetDocument()->getSelectedLink().getArrowStyle() != iLink::depend);
}

void NetView::OnSetLinkDependDouble()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	CString sFrom, sTo, sComment;
	int arrowType;
	GetDocument()->getSelectedLinkInfo(sFrom, sTo, sComment, arrowType);
	GetDocument()->setSelectedLinkInfo(sComment, iLink::depend2);
}

void NetView::OnUpdateSetLinkDependDouble(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->Enable(m_selectStatus == NetView::link && GetDocument()->getSelectedLink().getArrowStyle() != iLink::depend2);
}

void NetView::OnSetLinkInherit()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	CString sFrom, sTo, sComment;
	int arrowType;
	GetDocument()->getSelectedLinkInfo(sFrom, sTo, sComment, arrowType);
	GetDocument()->setSelectedLinkInfo(sComment, iLink::inherit);
}


void NetView::OnUpdateSetLinkInherit(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->Enable(m_selectStatus == NetView::link && GetDocument()->getSelectedLink().getArrowStyle() != iLink::inherit);
}

void NetView::OnSetLinkAgregat()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	CString sFrom, sTo, sComment;
	int arrowType;
	GetDocument()->getSelectedLinkInfo(sFrom, sTo, sComment, arrowType);
	GetDocument()->setSelectedLinkInfo(sComment, iLink::aggregat);
}

void NetView::OnUpdateSetLinkAgregat(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->Enable(m_selectStatus == NetView::link && GetDocument()->getSelectedLink().getArrowStyle() != iLink::aggregat);
}

void NetView::OnSetLinkComposit()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	CString sFrom, sTo, sComment;
	int arrowType;
	GetDocument()->getSelectedLinkInfo(sFrom, sTo, sComment, arrowType);
	GetDocument()->setSelectedLinkInfo(sComment, iLink::composit);
}

void NetView::OnUpdateSetLinkComposit(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->Enable(m_selectStatus == NetView::link && GetDocument()->getSelectedLink().getArrowStyle() != iLink::composit);
}

void NetView::changeSelectedLinkArrow()
{
	if (m_selectStatus == NetView::link) {
		CString sFrom, sTo, sComment;
		int arrowType;
		GetDocument()->getSelectedLinkInfo(sFrom, sTo, sComment, arrowType);
		GetDocument()->setSelectedLinkInfo(sComment, GetDocument()->getAppLinkArrow());
	}
}

void NetView::changeSelectedLinkWidth()
{
	if (m_selectStatus == NetView::link) {
		int style = GetDocument()->getAppLinkStyle();
		if (style == -1) {
			GetDocument()->setSelectedLinkWidth(0);
			GetDocument()->setSelectedLinkLineStyle(PS_DOT);
		} else {
			GetDocument()->setSelectedLinkWidth(style);
			GetDocument()->setSelectedLinkLineStyle(PS_SOLID);
		}
	}
}

void NetView::OnDrawOrderInfo()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	GetDocument()->setDrawOrderInfo(!GetDocument()->isDrawOrderInfo());
	Invalidate();
}

void NetView::OnUpdateDrawOrderInfo(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->Enable(((CiEditApp*)AfxGetApp())->m_rgsNode.bSyncOrder);
	pCmdUI->SetCheck(GetDocument()->isDrawOrderInfo());
}

void NetView::OnSetNodeMm()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	GetDocument()->backUpUndoNodes();
	GetDocument()->setSelectedNodeShape(iNode::MindMapNode);
}

void NetView::OnUpdateSetNodeMm(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	int shape = GetDocument()->getSelectedNodeShape();
	pCmdUI->Enable(shape!= iNode::MindMapNode && shape != iNode::MetaFile);
}
