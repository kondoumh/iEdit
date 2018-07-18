// NetView.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "NetView.h"
#include "iEditDoc.h"
#include "CreateNodeDlg.h"
#include "LinkPropertiesDlg.h"
#include "NodePropertiesDlg.h"
#include "ProceedingDlg.h"
#include "ShapesManagementDlg.h"
#include "MainFrm.h"
#include "StringUtil.h"
#include "imm.h"
#include "RectTrackerPlus.h"
#include "NodeMarginSettingsDlg.h"
#include "SystemConfiguration.h"

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
	m_hAplyForm = AfxGetApp()->LoadCursor(IDC_APLY_FORM);
	m_hLinkCsr = AfxGetApp()->LoadCursor(IDC_LINK);
	m_hRectCsr = AfxGetApp()->LoadCursor(IDC_RECT);
	m_hRRectCsr = AfxGetApp()->LoadCursor(IDC_RRECT);
	m_hArcCsr = AfxGetApp()->LoadCursor(IDC_ARC);
	m_hHandOpen = AfxGetApp()->LoadCursor(IDC_HAND_OPEN);
	m_hHandClose = AfxGetApp()->LoadCursor(IDC_HAND_CLOSE);
	m_hLayout = AfxGetApp()->LoadCursor(IDC_LAYOUT);
	m_hZoomCsr = AfxGetApp()->LoadCursor(IDC_ZOOM);
	m_hLabelCsr = AfxGetApp()->LoadCursor(IDC_LABELONLY);
	m_pShapesDlg = NULL;
	m_fZoomScale = 1.0f; // 0.0fよりよいんじゃ？
	m_fZoomScalePrev = 1.0f;
	m_bAplyForm = false;

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
	m_bFormCopied = FALSE;
	m_bGrpOlCoupled = FALSE;
	m_bLinkAction = false;
	m_nodeKeyDrop = -1;
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
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, &OnTtnNeedText)
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
	ON_COMMAND(ID_VALIGN_TOP, &NetView::OnValignTop)
	ON_UPDATE_COMMAND_UI(ID_VALIGN_TOP, &NetView::OnUpdateValignTop)
	ON_COMMAND(ID_ALIGN_BOTTOM, &NetView::OnAlignBottom)
	ON_UPDATE_COMMAND_UI(ID_ALIGN_BOTTOM, &NetView::OnUpdateAlignBottom)
	ON_COMMAND(ID_HALIGN_RIGHT, &NetView::OnHalignRight)
	ON_UPDATE_COMMAND_UI(ID_HALIGN_RIGHT, &NetView::OnUpdateHalignRight)
	ON_COMMAND(ID_HALIGN_LEFT, &NetView::OnHalignLeft)
	ON_UPDATE_COMMAND_UI(ID_HALIGN_LEFT, &NetView::OnUpdateHalignLeft)
	ON_COMMAND(ID_VALIGN_CENTER, &NetView::OnValignCenter)
	ON_UPDATE_COMMAND_UI(ID_VALIGN_CENTER, &NetView::OnUpdateValignCenter)
	ON_COMMAND(ID_HALIGN_CENTER, &NetView::OnHalignCenter)
	ON_UPDATE_COMMAND_UI(ID_HALIGN_CENTER, &NetView::OnUpdateHalignCenter)
	ON_COMMAND(ID_BTN_NODE_FILL_COLOR, &NetView::OnBtnNodeFillColor)
	ON_UPDATE_COMMAND_UI(ID_BTN_NODE_FILL_COLOR, &NetView::OnUpdateBtnNodeFillColor)
	ON_COMMAND(ID_BTN_LINE_COLOR, &NetView::OnBtnLineColor)
	ON_UPDATE_COMMAND_UI(ID_BTN_LINE_COLOR, &NetView::OnUpdateBtnLineColor)
	ON_COMMAND(ID_BTN_TEXT_COLOR, &NetView::OnBtnTextColor)
	ON_UPDATE_COMMAND_UI(ID_BTN_TEXT_COLOR, &NetView::OnUpdateBtnTextColor)
	ON_COMMAND(ID_BTN_LINK_ARROW, &NetView::OnBtnLinkArrow)
	ON_UPDATE_COMMAND_UI(ID_BTN_LINK_ARROW, &NetView::OnUpdateBtnLinkArrow)
	ON_COMMAND(ID_BTN_LINK_LINE_STYLE, &NetView::OnBtnLinkLineStyle)
	ON_UPDATE_COMMAND_UI(ID_BTN_LINK_LINE_STYLE, &NetView::OnUpdateBtnLinkLineStyle)
	ON_COMMAND(ID_SAVE_FORMAT, &NetView::OnSaveFormat)
	ON_UPDATE_COMMAND_UI(ID_SAVE_FORMAT, &NetView::OnUpdateSaveFormat)
	ON_COMMAND(ID_APLY_FORMAT, &NetView::OnAplyFormat)
	ON_UPDATE_COMMAND_UI(ID_APLY_FORMAT, &NetView::OnUpdateAplyFormat)
	ON_COMMAND(ID_GRP_OL_COUPLED, &NetView::OnGrpOlCoupled)
	ON_UPDATE_COMMAND_UI(ID_GRP_OL_COUPLED, &NetView::OnUpdateGrpOlCoupled)
	ON_COMMAND(ID_DELETE_SELECTED_NODES, &NetView::OnDeleteSelectedNodes)
	ON_UPDATE_COMMAND_UI(ID_DELETE_SELECTED_NODES, &NetView::OnUpdateDeleteSelectedNodes)
	ON_COMMAND(ID_DELETE_SELECTED_LINKS, &NetView::OnDeleteSelectedLinks)
	ON_UPDATE_COMMAND_UI(ID_DELETE_SELECTED_LINKS, &NetView::OnUpdateDeleteSelectedLinks)
	ON_COMMAND(ID_SET_LINK_ANGLED, &NetView::OnSetLinkAngled)
	ON_UPDATE_COMMAND_UI(ID_SET_LINK_ANGLED, &NetView::OnUpdateSetLinkAngled)
	ON_COMMAND(ID_EXPORT_EMF, &NetView::OnExportEmf)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_EMF, &NetView::OnUpdateExportEmf)
	ON_COMMAND(ID_EXPORT_PNG, &NetView::OnExportPng)
	ON_UPDATE_COMMAND_UI(ID_EXPORT_PNG, &NetView::OnUpdateExportPng)
	ON_COMMAND(ID_SET_MARGIN, &NetView::OnSetMargin)
	ON_UPDATE_COMMAND_UI(ID_SET_MARGIN, &NetView::OnUpdateSetMargin)
	ON_COMMAND(ID_RESIZE_TOFIT, &NetView::OnResizeTofit)
	ON_UPDATE_COMMAND_UI(ID_RESIZE_TOFIT, &NetView::OnUpdateResizeTofit)
	ON_COMMAND(ID_REPLACE_METAFILE, &NetView::OnReplaceMetafile)
	ON_UPDATE_COMMAND_UI(ID_REPLACE_METAFILE, &NetView::OnUpdateReplaceMetafile)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void NetView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	m_selectRect = GetDocument()->GetSelectedNodeRect();
	if (!m_selectRect.IsRectEmpty()) {
		m_selectStatus = NetView::single;
	}

	AdjustScrollArea();

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
	::SetRect(&r, 10, 10, 10, 10);
	m_toolTip.SetMargin(&r);
	m_toolTip.SetDelayTime(TTDT_AUTOPOP, 3000);
	m_toolTip.SendMessage(TTM_SETMAXTIPWIDTH, 0, 300);

}

void NetView::OnDraw(CDC* pDC)
{
	iEditDoc* pDoc = GetDocument();

	pDoc->DrawNodes(pDC);
	if (!m_bDragRelax) {
		DrawSelection(pDC);
	}
	pDoc->DrawLinks(pDC, false);

	if (m_bStartAdd) {
		DrawLinkAdding(pDC);
	}

	if (m_bLinkCurving) {
		DrawCurve(pDC);
	}

	if (m_bAlteringLinkFrom) {
		DrawAlterLinkFrom(pDC);
	}

	if (m_bAlteringLinkTo) {
		DrawAlterLinkTo(pDC);
	}

}

BOOL NetView::OnEraseBkgnd(CDC* pDC)
{
	CBrush backBrush(m_bkColor);

	// 古いブラシを保存する。
	CBrush* pOldBrush = pDC->SelectObject(&backBrush);
	CRect rect;
	pDC->GetClipBox(&rect);     // 対象領域を消去。

	pDC->PatBlt(rect.left, rect.top, rect.Width(),
		rect.Height(), PATCOPY);
	pDC->SelectObject(pOldBrush);
	return TRUE;
}

void NetView::DrawSelection(CDC *pDC)
{
	if (m_selectStatus == NetView::none) return;

	CRectTracker tracker;
	tracker.m_rect = m_selectRect;
	pDC->LPtoDP(&(tracker.m_rect));
	tracker.m_rect.left += 1;
	tracker.m_rect.top += 1;

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
			GetDocument()->DrawLinkSelection(pDC);
		}
		break;
	case NetView::linkTermFrom:
		GetDocument()->DrawLinkSelectionFrom(pDC);
		break;
	case NetView::linkTermTo:
		GetDocument()->DrawLinkSelectionTo(pDC);
		break;
	}
}

void NetView::DrawCurve(CDC *pDC)
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

void NetView::DrawLinkAdding(CDC *pDC)
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

void NetView::DrawAlterLinkFrom(CDC *pDC)
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

void NetView::DrawAlterLinkTo(CDC *pDC)
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

void NetView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (m_bZooming) {
		return;
	}
	if (m_addMode != normal) {
		m_addMode = normal;
		return;
	}
	if (m_bAplyForm) {
		m_bAplyForm = false;
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
	}
	else if (m_selectStatus == NetView::single) {
		pPopup = menu.GetSubMenu(2);
	}
	else if (m_selectStatus == NetView::link) {
		pPopup = menu.GetSubMenu(3);
	}
	else if (m_selectStatus == NetView::multi) {
		pPopup = menu.GetSubMenu(4);
	}

	if (point.x < 0 || point.y < 0) {
		CRect r;
		GetWindowRect(&r);
		point = r.TopLeft();
		if (m_selectStatus == NetView::single || m_selectStatus == NetView::multi) {
			CPoint p;
			p.x = (m_selectRect.left + m_selectRect.right) / 2;
			p.y = (m_selectRect.top + m_selectRect.bottom) / 2;
			point += p;
			point -= GetScrollPosition();
		}
		else if (m_selectStatus == NetView::link) {

		}
	}

	ASSERT(pPopup != NULL);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
}

BOOL NetView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_bGrasp) {
		if (m_bDragView) {
			::SetCursor(m_hHandClose);
		}
		else {
			::SetCursor(m_hHandOpen);
		}
		return TRUE;
	}

	if (m_bAplyForm) {
		::SetCursor(m_hAplyForm);
		return TRUE;
	}

	if (m_bZooming) {
		::SetCursor(m_hZoomCsr);
		return TRUE;
	}


	if (m_selectStatus == NetView::single && m_addMode == NetView::normal) {
		CRectTracker tracker; tracker.m_rect = m_selectRect;

		LPtoDP(tracker.m_rect);

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
	return CScrollView::OnSetCursor(pWnd, nHitTest, message);
}

iEditDoc* NetView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(iEditDoc)));
	return (iEditDoc*)m_pDocument;
}

void NetView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CPoint logPt = point; DPtoLP(&logPt);

	PreparePastePoint(logPt); // ノードのペースト先を用意しておく
	m_ptScreen = point + GetScrollPosition(); // スクリーン座標も保持しておく

	if (m_bGrasp) {     // ハンドモードの処理
		m_ptPrePos = point;
		return;
	}

	if (m_bZooming) {
		ZoomIn(&logPt, 0.1f);
		return;
	}

	if (m_bAplyForm) {
		ApplyFormat(logPt);
		return;
	}

	if (AddingNode()) { // ノード追加の処理
		CPoint spt(point);
		ClientToScreen(&spt);
		AddNode(logPt, spt);
		return;
	}

	if (AddingLink()) { // リンク追加開始の処理
		CRect r;
		if (GetDocument()->HitTest(logPt, r)) {
			m_ptPrePos = point;
			StartAddingLink(logPt);
		}
		return;
	}

	// 芋づるモード開始
	if (nFlags & MK_CONTROL) {
		CRect r;
		if (GetDocument()->HitTest(logPt, r)) {
			// ノードの矩形内のオフセットを計算
			CPoint selTopLeft = GetDocument()->GetSelectedNodeRect().TopLeft();
			m_dragOffset = selTopLeft - logPt;

			GetDocument()->DisableUndo();
			GetDocument()->BackupNodesForUndo();
			GetDocument()->BackupLinksForUndo();
			m_bDragRelax = true;
			PrepareDragRelax();
		}
		return;
	}

	// ホイールボタンの代替アクション
	if (nFlags & MK_SHIFT) {
		m_addMode = NetView::link0;
		StartAddingLink(logPt);
		return;
	}

	// リンク分割挿入アクション開始
	if (GetAsyncKeyState(VK_MENU) & 0x8000) {
		CRect r;
		if (GetDocument()->HitTest(logPt, r)) {
			// ノードの矩形内のオフセットを計算
			CPoint selTopLeft = GetDocument()->GetSelectedNodeRect().TopLeft();
			m_dragOffset = selTopLeft - logPt;

			GetDocument()->DisableUndo();
			GetDocument()->BackupNodesForUndo();
			GetDocument()->BackupLinksForUndo();
			GetDocument()->SetSelectedNodeDragging();
			m_bLinkAction = true;
		}
		return;
	}

	// 複数選択中にトラッカを選択した → 複数トラック処理
	if (m_selectStatus == NetView::multi) {
		if (m_selectRect.PtInRect(logPt)) {
			CWindowDC dc(this);
			OnPrepareDC(&dc);
			TrackMultiple(logPt, point, &dc);
			return;
		}
	}

	// 単独選択中にトラッカを選択した → 単独トラック処理
	if (m_selectStatus == NetView::single) {
		CRect selectRect = m_selectRect;
		m_testRect = m_selectRect;
		m_testRect.OffsetRect(CPoint(20, 20));
		AdjustRedrawBound(selectRect);
		CRectTracker tracker; tracker.m_rect = selectRect;
		tracker.m_nStyle = CRectTracker::resizeInside;
		int trackState = tracker.HitTest(logPt);
		CClientDC dc(this);
		OnPrepareDC(&dc);
		bool hit = trackState != CRectTracker::hitNothing;
		BOOL priorSelectionDragging = ((CiEditApp*)AfxGetApp())->m_rgsNode.bPriorSelectionDragging;
		if (!priorSelectionDragging) {
			hit &= trackState != CRectTracker::hitMiddle;
		}
		if (hit) {
			TrackSingle(logPt, point, &dc, nFlags & MK_SHIFT);
			return;
		}
	}

	// 選択が更新された場合
	DoUpdateSelection(logPt);

	// 選択更新後のトラック処理及び選択用ラバーバンドの処理
	DoPostSelection(logPt, nFlags & MK_SHIFT);

	CScrollView::OnLButtonDown(nFlags, point);
}

bool NetView::AddingNode() const
{
	return (m_addMode == NetView::rect ||
		m_addMode == NetView::rRect ||
		m_addMode == NetView::arc ||
		m_addMode == NetView::label);
}

bool NetView::AddingLink() const
{
	return (m_addMode == NetView::link0 ||
		m_addMode == NetView::link1 ||
		m_addMode == NetView::link2);
}

void NetView::PreparePastePoint(const CPoint &point)
{
	if (GetDocument()->CanDuplicateNodes() ||
		::IsClipboardFormatAvailable(CF_ENHMETAFILE) ||
		::IsClipboardFormatAvailable(CF_METAFILEPICT) ||
		::IsClipboardFormatAvailable(CF_DIB) ||
		::IsClipboardFormatAvailable(CF_TEXT)) {
		m_ptPaste = point;
	}
	else {
		m_ptPaste = CPoint(0, 0);
	}

}

// 選択が更新される場合の処理
void NetView::DoUpdateSelection(const CPoint &logPt)
{
	CRect old = m_selectRect; AdjustRedrawBound(old);
	CRect r;
	if (GetDocument()->HitTestLinks(logPt)) {
		// リンクの選択が更新された
		m_selectStatus = NetView::link;
		GetDocument()->GetSelectedLinkEndPoints(m_linkStart, m_linkEnd);
		m_selectRect = GetDocument()->GetSelectedLinkBound();
		CRect nw = m_selectRect; AdjustRedrawBound(nw);
		InvalidateRect(nw);
		InvalidateRect(old);
	}
	else if (GetDocument()->SelectLinkStartIfHit(logPt, false)) {
		// リンクのリンク元を選択した
		m_selectStatus = NetView::linkTermFrom;
		m_selectRect = GetDocument()->GetSelectedLinkBound();
		CRect nw = m_selectRect; AdjustRedrawBound(nw);
		InvalidateRect(nw);
		InvalidateRect(old);
		StartAlterFrom(logPt);
	}
	else if (GetDocument()->SelectLinkEndIfHit(logPt, false)) {
		// リンクのリンク先を選択した
		m_selectStatus = NetView::linkTermTo;
		m_selectRect = GetDocument()->GetSelectedLinkBound();
		CRect nw = m_selectRect; AdjustRedrawBound(nw);
		InvalidateRect(nw);
		InvalidateRect(old);
		StartAlterTo(logPt);
	}
	else if (GetDocument()->HitTest(logPt, r)) {
		// ノードの選択が更新された
		m_selectStatus = NetView::single;
		m_selectRect = r;
		CRect nw = m_selectRect; AdjustRedrawBound(nw);
		InvalidateRect(nw);
		InvalidateRect(old);
	}
	else {
		// 選択が解除された
		CRect old = m_selectRect; AdjustRedrawBound(old);
		m_selectStatus = NetView::none;
		m_selectRect = CRect(0, 0, 0, 0);
		InvalidateRect(old);
	}
}

// 選択が更新された後の処理 --トラック処理とラバーバンド処理
void NetView::DoPostSelection(const CPoint &logPt, BOOL shiftPressed)
{
	CPoint logicalPt = logPt;
	CPoint point = logPt;
	LPtoDP(&point);
	if (m_selectStatus == NetView::single) {
		// 単独選択の場合 → トラック処理にすぐ入る
		CWindowDC dc(this);
		OnPrepareDC(&dc);
		TrackSingle(logicalPt, point, &dc, shiftPressed);
	}
	else if (m_selectStatus == NetView::none) {
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
		int selcnt = GetDocument()->SelectNodesInBound(trackerRect, selRect);
		CRect old = m_selectRect; AdjustRedrawBound(old);
		if (selcnt > 0) {
			m_selectRect = selRect;
			if (selcnt > 1) {
				m_selectStatus = NetView::multi;
				GetDocument()->SelectLinksInBound(trackerRect);// 含まれるlinkも選択する
				m_selectRect |= GetDocument()->GetSelectedLinkBound();
			}
			else if (selcnt == 1) {
				m_selectStatus = NetView::single;
			}
			CRect rdRect = m_selectRect; AdjustRedrawBound(rdRect);
			InvalidateRect(rdRect);
			InvalidateRect(old);
		}
		else {
			m_selectStatus = NetView::none;
			m_selectRect = CRect(0, 0, 0, 0);
			InvalidateRect(old);
		}
	}
}

void NetView::TrackMultiple(CPoint &logPt, CPoint &point, CDC *pDC)
{
	CRectTracker tracker;
	tracker.m_rect = m_selectRect;
	tracker.m_nStyle = CRectTracker::hatchedBorder | CRectTracker::resizeInside;
	pDC->LPtoDP(&(tracker.m_rect));

	CRect old = GetDocument()->GetRelatedBound(); AdjustRedrawBound(old);
	if (tracker.Track(this, point, TRUE)) {
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

		GetDocument()->BackupNodesForUndo();
		GetDocument()->BackupLinksForUndo();
		GetDocument()->MoveSelectedNode(CSize(moveX, moveY));
		GetDocument()->MoveSelectedLink(CSize(moveX, moveY));
		CRect nwrd = GetDocument()->GetRelatedBound(); AdjustRedrawBound(nwrd);

		CRect rc;
		if (!rc.IntersectRect(old, nwrd)) {
			InvalidateRect(old);
			InvalidateRect(nwrd);
		}
		else {
			CRect rd = old | nwrd;
			InvalidateRect(rd);
		}
		AdjustScrollArea();
	}
}

void NetView::TrackSingle(CPoint &logPt, CPoint& point, CDC* pDC, BOOL keepRatio)
{
	CRectTrackerPlus tracker;
	CRect org = m_selectRect;
	CRect selectRect = m_selectRect;
	LPtoDP(selectRect);

	tracker.m_rect = selectRect;
	if (keepRatio) {
		tracker.SetKeepRatio();
		tracker.SetInitialRect(selectRect);
	}

	tracker.m_nStyle = CRectTracker::resizeInside;
	CRect old = GetDocument()->GetRelatedBound(); AdjustRedrawBound(old);
	if (tracker.Track(this, point, TRUE)) {
		GetDocument()->BackupNodesForUndo();
		GetDocument()->BackupLinksForUndo();

		CRect nw;
		DPtoLP(tracker.m_rect);
		tracker.GetTrueRect(&nw);
		if (nw.left < 0) {
			nw.left = 0;
			nw.right = m_selectRect.Width();
		}
		if (nw.top < 0) {
			nw.top = 0;
			nw.bottom = m_selectRect.Height();
		}
		bool resized = false;
		if (m_selectRect.Width() != nw.Width() || m_selectRect.Height() != nw.Height()) {
			resized = true;
		}
		m_selectRect = nw;
		GetDocument()->SetSelectedNodeBound(m_selectRect);
		int moveX = m_selectRect.left - org.left;
		int moveY = m_selectRect.top - org.top;
		GetDocument()->MoveSelectedLink(CSize(moveX, moveY));
		CRect rdnw = GetDocument()->GetRelatedBound(); AdjustRedrawBound(rdnw);
		// getRelatedBoundAndに変更
		CRect rc;
		if (!rc.IntersectRect(old, rdnw)) {
			InvalidateRect(old);
			rdnw.InflateRect((int)(rdnw.Width()*1.5), (int)(rdnw.Height()*1.5));
			InvalidateRect(rdnw);
		}
		else {
			CRect rdRec = old | rdnw;
			rdRec.InflateRect((int)(rdRec.Width()*1.5), (int)(rdRec.Height()*1.5));
			InvalidateRect(rdRec);
		}
		AdjustScrollArea();

		if (((CiEditApp*)AfxGetApp())->m_rgsNode.bEnableGroup && !resized) {
			GetDocument()->MoveNodesInBound(org, CSize(moveX, moveY));
			if (GetDocument()->ShowSubBranch() && m_bGrpOlCoupled) {
				GetDocument()->MigrateGroup();
			}
		}
	}
}

void NetView::OnAddLink0()
{
	if (m_addMode == NetView::link0) {
		m_addMode = NetView::normal;
	}
	else {
		m_addMode = NetView::link0;
	}
}

void NetView::OnUpdateAddLink0(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_bGrasp && !m_bZooming);
	pCmdUI->SetCheck(m_addMode == NetView::link0);
}

void NetView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	AdjustScrollArea();

	int prevSelectStatus = m_selectStatus; // 事前の選択状態保持

	m_selectStatus = NetView::none;

	CRect oldBound = m_selectRect;
	AdjustRedrawBound(oldBound);

	if (IsScaleChanged()) {
		UpdateViewport(NULL);
		m_fZoomScalePrev = m_fZoomScale;
	}

	CClientDC dc(this);
	OnPrepareDC(&dc);
	CRect old, nw;

	if (GetDocument()->ShowSubBranch() && GetDocument()->CurKeyInBranch() ||
		!GetDocument()->ShowSubBranch()) {
		m_selectRect = GetDocument()->GetSelectedNodeRect();
	}
	else {
		m_selectRect = CRect(0, 0, 0, 0);
	}

	if (!m_selectRect.IsRectEmpty()) {
		m_selectStatus = NetView::single;
	}
	CRect nwBound, rdRC;

	if (pHint == NULL) {
		InvalidateRect(&oldBound);
		nwBound = m_selectRect; AdjustRedrawBound(nwBound);
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
		ApplyColorSetting();
		Invalidate();
		break;

	case iHint::linkSelRet:
		break;
	case iHint::linkSel:
	case iHint::linkCurved:
	case iHint::linkStraight:
	case iHint::linkListSel:
	case iHint::linkModified:
		m_selectStatus = NetView::link;
		InvalidateRect(oldBound);

		// 再描画領域の計算ロジックを作ってみた
		nwBound = GetDocument()->GetSelectedLinkBound();
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
		AdjustRedrawBound(old);
		AdjustRedrawBound(nw);
		//	dc.DrawDragRect(nw, sz, old, sz);
		InvalidateRect(old | nw);
		break;

	case iHint::nodeDeleteByKey:
		if (!GetDocument()->ShowSubBranch()) {
			old = GetDocument()->RestoreDeleteBound();
			AdjustRedrawBound(old);
			InvalidateRect(old);
		}
		else {
			Invalidate();
		}
		break;
	case iHint::nodeDeleteMulti:
		break;
	case iHint::linkDeleteMulti:
		Invalidate();
		break;
	case iHint::reflesh:
	case iHint::groupMoved:
		Invalidate();
		break;
	case iHint::parentSel:
		if (GetDocument()->ShowSubBranch()) {
			break;
		}
		Invalidate();
		SendMessage(WM_SIZE); // こうでもしないと反映してくれない。暫定的に
		break;
	case iHint::nodeLabelChanged:
	case iHint::nodeStyleChanged:
		m_selectStatus = prevSelectStatus;
		if (m_selectStatus == NetView::multi) {
			m_selectRect = GetDocument()->GetRelatedBoundAnd(false);
		}
		InvalidateRect(&oldBound);
		nwBound = GetDocument()->GetRelatedBound(); AdjustRedrawBound(nwBound);
		InvalidateRect(nwBound);
		break;
	case iHint::linkAdd:
		nwBound = GetDocument()->GetRelatedBound(); AdjustRedrawBound(nwBound);
		InvalidateRect(nwBound);
		break;
	case iHint::showSubBranch:
		m_selectRect = CRect(0, 0, 0, 0);
		Invalidate();
		break;
	case iHint::resetShowSubBranch:
		Invalidate();
		break;
	case iHint::linkDelete:
		old = GetDocument()->RestoreDeleteBound();
		AdjustRedrawBound(old);
		InvalidateRect(old);
		break;

	case iHint::nodeFontResize:
		m_selectStatus = prevSelectStatus;
		if (m_selectStatus == NetView::multi) {
			m_selectRect = GetDocument()->GetRelatedBoundAnd(false);
		}
		nwBound = GetDocument()->GetRelatedBound(); AdjustRedrawBound(nwBound);
		nwBound |= oldBound;
		InvalidateRect(nwBound);
		break;

	case iHint::nodeSel:
		InvalidateRect(&oldBound);
		nwBound = m_selectRect; AdjustRedrawBound(nwBound);
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

void NetView::StartAddingLink(const CPoint pt)
{
	iEditDoc* pDoc = GetDocument();
	m_bStartAdd = pDoc->SetStartLink(pt);
	if (m_bStartAdd) {
		m_preRedrawBound = CRect(pt, pt);
		m_ptLinkFrom = pt;
	}
}

void NetView::StartAlterFrom(const CPoint& pt)
{
	const iLink* pl = GetDocument()->GetSelectedLink(false);
	if (pl != NULL) {
		m_bAlteringLinkFrom = true;
		m_alterLinkStartPt = pt;
		m_ptAlterLinkFrom = pl->GetPtFrom();
		m_ptAlterLinkTo = pl->GetPtTo();
	}
}

void NetView::StartAlterTo(const CPoint &pt)
{
	const iLink* pl = GetDocument()->GetSelectedLink(false);
	if (pl != NULL) {
		m_bAlteringLinkTo = true;
		m_alterLinkStartPt = pt;
		m_ptAlterLinkFrom = pl->GetPtFrom();
		m_ptAlterLinkTo = pl->GetPtTo();
	}
}

void NetView::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint logPt = point;
	DPtoLP(&logPt);

	/////////////////////
	// ツールチップ表示用
	/////////////////////
	if (m_addMode == normal && m_selectStatus != NetView::multi
		&& !m_bDragRelax) {
		CRect r;
		iEditDoc* pDoc = GetDocument();
		iNode node = pDoc->GetHitNode(logPt);
		if (node.GetText() != _T("")) {
			CString strTipNew = node.GetText().Left(300);
			if (node.GetText().GetLength() > 300) {
				strTipNew += _T("....");
			}
			if (m_strTip != strTipNew) {
				m_strTip = strTipNew;
				m_toolTip.Update();
			}
		}
		else {
			m_strTip = _T("");
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
		GetDocument()->RelaxSingleStep(logPt, m_dragOffset);
		CRect rBound = GetDocument()->GetChaindNodesBound();
		LPtoDP(rBound);
		InvalidateRect(rBound);
		return;
	}

	///////////////////////////////
	// リンク分割挿入アクション
	///////////////////////////////
	if (GetAsyncKeyState(VK_MENU) & 0x8000 && m_bLinkAction) {
		CRect rc = GetDocument()->GetSelectedNodeRect();
		CRect prevRc = rc;
		int height = rc.Height();
		int width = rc.Width();
		rc.left = logPt.x;
		rc.top = logPt.y;
		rc.right = rc.left + width;
		rc.bottom = rc.top + height;
		rc.OffsetRect(m_dragOffset);

		if (rc.left < 0) {
			rc.left = 0;
			rc.right = width;
		}
		if (rc.top < 0) {
			rc.top = 0;
			rc.bottom = height;
		}
		GetDocument()->SetSelectedNodeBound(rc, false, true);
		GetDocument()->SetConnectionPoint();

		DWORD hitKey = GetDocument()->HitTestDropTarget(rc.CenterPoint(), GetDocument()->GetSelectedNodeKey());
		if (hitKey != -1) {
			m_nodeKeyDrop = GetDocument()->GetSelectedNodeKey();
		}
		else {
			m_nodeKeyDrop = -1;
		}
		CRect rBound = prevRc + rc;
		LPtoDP(rBound);
		InvalidateRect(rBound);
		return;
	}

	/////////////////////
	// link の Curve
	/////////////////////
	if (!m_bStartAdd && m_selectStatus == NetView::link && nFlags & MK_LBUTTON) {
		m_linkPath = logPt;
		if (m_bLinkCurving == false) {
			m_bLinkCurving = true;
			m_curveStartPt = logPt;
		}
		CRect rc = GetCurveBound(logPt);
		CRect nw = rc; AdjustRedrawBound(nw);
		AdjustRedrawBound(m_preRedrawBound);
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
		CRect old = m_preRedrawBound; AdjustRedrawBound(old);

		CRect rc(m_ptAlterLinkTo, m_ptAlteringAnker);
		rc.NormalizeRect();
		AdjustRedrawBound(rc);
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
		AdjustRedrawBound(rc);
		CRect rdRc = rc | m_preRedrawBound;
		InvalidateRect(rdRc);
		m_preRedrawBound = rc;
		CScrollView::OnMouseMove(nFlags, point);
		return;
	}

	///////////////////
	// link の追加
	///////////////////
	if (AddingLink()) {
		if (!nFlags & MK_LBUTTON) {
			CScrollView::OnMouseMove(nFlags, point);
			m_bStartAdd = false;
		}
		if (!m_bStartAdd) return;
		m_ptLinkTo = logPt;
		CRect old = m_preRedrawBound; AdjustRedrawBound(old);

		CRect rc(m_ptLinkFrom, m_ptLinkTo);
		rc.NormalizeRect();
		AdjustRedrawBound(rc);
		CRect rdRc = rc | m_preRedrawBound;
		InvalidateRect(rdRc);
		m_preRedrawBound = rc;
		CScrollView::OnMouseMove(nFlags, point);
		return;
	}
}

void NetView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Mouseの左ボタンリリース時の処理

	if (m_bGrasp) { // ハンドモードの解除
		m_bDragView = false;
		return;
	}

	CPoint logPt = point;
	DPtoLP(&logPt);

	// 芋蔓モードの解除
	if (m_bDragRelax) {
		CancelDragRelax();
		return;
	}

	// ホイールボタンの代替
	if (nFlags & MK_SHIFT) {
		PointedLinkEndPosition(point);
		return;
	}

	// リンク分割挿入アクションの処理
	if (m_bLinkAction) {
		GetDocument()->SetSelectedNodeDragging(false);
		if (m_nodeKeyDrop != -1) {
			GetDocument()->DivideTargetLink(m_nodeKeyDrop);
		}
		else {
			GetDocument()->SetConnectionPoint();
		}
		m_bLinkAction = false;
		GetDocument()->HitTestDropTarget(CPoint(-1, -1), -1);
		Invalidate();
		return;
	}

	// リンク元の張替え
	if (m_bAlteringLinkFrom) {
		m_bAlteringLinkFrom = false;
		CSize sz = m_alterLinkStartPt - logPt;
		if (abs(sz.cx) < 3 && abs(sz.cy) < 3) {
			Invalidate();
			return;
		}
		if (GetDocument()->SwitchLinkStartNodeAt(logPt)) {
			CRect rcOld = CRect(m_ptAlterLinkTo, m_ptAlterLinkTo);
			rcOld.NormalizeRect();
			CRect rcNew = GetDocument()->GetSelectedLinkBound();
			rcNew.NormalizeRect();
			InvalidateRect(rcOld | rcNew);
		}
		else {
			CRect rcDel = CRect(m_ptAlterLinkTo, m_ptAlteringAnker);
			rcDel.NormalizeRect();
			InvalidateRect(rcDel);
		}
		return;
	}

	// リンク先の張替え
	if (m_bAlteringLinkTo) {
		m_bAlteringLinkTo = false;
		CSize sz = m_alterLinkStartPt - logPt;
		if (abs(sz.cx) < 3 && abs(sz.cy) < 3) {
			Invalidate();
			return;
		}
		if (GetDocument()->SwitchLinkEndNodeAt(logPt)) {
			CRect rcOld = CRect(m_ptAlterLinkFrom, m_ptAlterLinkTo);
			rcOld.NormalizeRect();
			CRect rcNew = GetDocument()->GetSelectedLinkBound();
			rcNew.NormalizeRect();
			InvalidateRect(rcOld | rcNew);
		}
		else {
			CRect rcDel = CRect(m_ptAlterLinkFrom, m_ptAlteringAnker);
			rcDel.NormalizeRect();
			InvalidateRect(rcDel);
		}
		return;
	}

	// リンクを曲げた
	if (!m_bStartAdd) {
		if (m_bLinkCurving && m_selectStatus == NetView::link) {
			m_bLinkCurving = false;
			CSize sz = m_curveStartPt - logPt;
			if (abs(sz.cx) < 3 && abs(sz.cy) < 3) {
				Invalidate();
				return;
			}
			GetDocument()->DisableUndo();
			GetDocument()->BackupLinksForUndo();
			GetDocument()->CurveSelectedLink(logPt, true);
			Invalidate();
		}
		return;
	}

	// リンクを追加し終わった(ノード上でマウスをリリースしたか)
	if (m_ptPrePos == point) return;
	bool linked = false;
	switch (m_addMode) {
	case NetView::link0:
		linked = GetDocument()->SetEndLink(logPt, 0, true);
		m_bStartAdd = false;
		break;
	case NetView::link1:
		linked = GetDocument()->SetEndLink(logPt, 1, true);
		m_bStartAdd = false;
		break;
	case NetView::link2:
		linked = GetDocument()->SetEndLink(logPt, 2, true);
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
	CPoint logPt = point;
	DPtoLP(&logPt);

	if (m_bZooming) {
		ZoomOut(&logPt, 0.1f);
		return;
	}

	CRect r;
	iEditDoc* pDoc = GetDocument();
	CRect old = m_selectRect; AdjustRedrawBound(old);
	if (m_selectStatus != NetView::multi) {
		// HitTest, linkHitTest
		if (pDoc->HitTestLinks(logPt)) {
			m_selectStatus = NetView::link;
			m_selectRect = GetDocument()->GetSelectedLinkBound();
		}
		else if (pDoc->HitTest(logPt, r)) {
			m_selectStatus = NetView::single;
			m_selectRect = r;
		}
		else {
			m_selectStatus = NetView::none;
			m_selectRect = CRect(0, 0, 0, 0);
		}
	}
	else {
		// m_selectRect HitTest, HitTest, linkHitTest
		if (!m_selectRect.PtInRect(logPt)) {
			if (pDoc->HitTestLinks(logPt)) {
				m_selectStatus = NetView::link;
				m_selectRect = GetDocument()->GetSelectedLinkBound();
			}
			else if (pDoc->HitTest(logPt, r)) {
				m_selectStatus = NetView::single;
				m_selectRect = r;
			}
			else {
				m_selectStatus = NetView::none;
				m_selectRect = CRect(0, 0, 0, 0);
			}
		}
	}
	CRect nw = m_selectRect; AdjustRedrawBound(nw);
	InvalidateRect(nw);
	InvalidateRect(old);
	if (GetDocument()->CanDuplicateNodes()) {
		m_ptPaste = logPt;
	}

	CScrollView::OnRButtonDown(nFlags, point);
}

void NetView::OnSetNodeFont()
{
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));

	CString defaultFont = _T("MS UI Gothic");
	if (SystemConfiguration::IsMeiryoAvailable()) {
		defaultFont = _T("メイリオ");
	}

	::lstrcpy(lf.lfFaceName, defaultFont);
	lf.lfHeight = 0xfffffff3;
	lf.lfWidth = 0;
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = SHIFTJIS_CHARSET;

	GetDocument()->BackupNodesForUndo();
	GetDocument()->BackupLinksForUndo();
	if (m_selectStatus == NetView::single) {
		GetDocument()->GetSelectedNodeFont(lf);
		CFontDialog dlg(&lf);
		//dlg.m_cf.Flags |= CF_SELECTSCRIPT;
		dlg.m_cf.rgbColors = GetDocument()->GetSelectedNodeFontColor();
		if (dlg.DoModal() != IDOK) return;
		GetDocument()->SetSelectedNodeFont(lf);
		GetDocument()->SetSelectedNodeFontColor(dlg.GetColor());
	}
	else if (m_selectStatus == NetView::link) {
		GetDocument()->GetSelectedLinkFont(lf);
		CFontDialog dlg(&lf);
		//dlg.m_cf.Flags |= CF_SELECTSCRIPT;
		if (dlg.DoModal() != IDOK) return;
		GetDocument()->SetSelectedLinkFont(lf);
	}
	else if (m_selectStatus == NetView::multi) {
		CFontDialog dlg(&lf);
		//dlg.m_cf.Flags |= CF_SELECTSCRIPT;
		if (dlg.DoModal() != IDOK) return;
		GetDocument()->SetSelectedNodeFont(lf);
		GetDocument()->SetSelectedNodeFontColor(dlg.GetColor());
		GetDocument()->SetSelectedLinkFont(lf);
	}

	Invalidate();
}

void NetView::OnUpdateSetNodeFont(CCmdUI* pCmdUI)
{
}

void NetView::OnSetNodeLineColor()
{
	GetDocument()->BackupNodesForUndo();
	GetDocument()->BackupLinksForUndo();
	if (m_selectStatus == NetView::single) {
		CColorDialog dlg(GetDocument()->GetSelectedNodeLineColor());
		if (dlg.DoModal() != IDOK) return;
		GetDocument()->SetSelectedNodeLineColor(dlg.GetColor());
	}
	else if (m_selectStatus == NetView::link) {
		CColorDialog dlg(GetDocument()->GetSelectedLinkLineColor());
		if (dlg.DoModal() != IDOK) return;
		GetDocument()->SetSelectedLinkLineColor(dlg.GetColor());
	}
	else if (m_selectStatus == NetView::multi) {
		CColorDialog dlg;
		if (dlg.DoModal() != IDOK) return;
		GetDocument()->SetSelectedNodeLineColor(dlg.GetColor());
		GetDocument()->SetSelectedLinkLineColor(dlg.GetColor());
	}
}

void NetView::OnUpdateSetNodeLineColor(CCmdUI* pCmdUI)
{
}

void NetView::OnSetLineNull()
{
	GetDocument()->BackupNodesForUndo();
	GetDocument()->SetSelectedNodeLineWidth(0);
	GetDocument()->SetSelectedNodeLineStyle(PS_NULL);
}

void NetView::OnUpdateSetLineNull(CCmdUI* pCmdUI)
{
	if (m_selectStatus == NetView::single) {
		BOOL state = GetDocument()->GetSelectedNodeLineStyle() == PS_NULL &&
			GetDocument()->GetSelectedNodeLineWidth() == 0;
		pCmdUI->SetCheck(state);
	}
}

void NetView::OnSetLineSolid0()
{
	bool bDrwAll = false;
	GetDocument()->BackupNodesForUndo();
	GetDocument()->BackupLinksForUndo();
	switch (m_selectStatus) {
	case NetView::single:
		GetDocument()->SetSelectedNodeLineWidth(0);
		GetDocument()->SetSelectedNodeLineStyle(PS_SOLID);
		break;
	case NetView::link:
		GetDocument()->SetSelectedLinkWidth(0);
		GetDocument()->SetSelectedLinkLineStyle(PS_SOLID);
		break;
	case NetView::multi:
		GetDocument()->SetSelectedNodeLineWidth(0);
		GetDocument()->SetSelectedNodeLineStyle(PS_SOLID);
		GetDocument()->SetSelectedLinkWidth(0);
		GetDocument()->SetSelectedLinkLineStyle(PS_SOLID);
		break;
	}
}

void NetView::OnUpdateSetLineSolid0(CCmdUI* pCmdUI)
{
	if (m_selectStatus == NetView::single) {
		BOOL state = GetDocument()->GetSelectedNodeLineStyle() == PS_SOLID &&
			GetDocument()->GetSelectedNodeLineWidth() == 0;
		pCmdUI->SetCheck(state);
	}
	else if (m_selectStatus == NetView::link) {
		BOOL state = GetDocument()->GetSelectedLinkLineStyle() == PS_SOLID &&
			GetDocument()->GetSelectedLinkWidth() == 0;
		pCmdUI->SetCheck(state);
	}
}

void NetView::OnSetLineSolid1()
{
	bool bDrwAll = false;
	GetDocument()->BackupNodesForUndo();
	GetDocument()->BackupLinksForUndo();
	switch (m_selectStatus) {
	case NetView::single:
		GetDocument()->SetSelectedNodeLineWidth(2);
		GetDocument()->SetSelectedNodeLineStyle(PS_SOLID);
		break;
	case NetView::link:
		GetDocument()->SetSelectedLinkWidth(2);
		GetDocument()->SetSelectedLinkLineStyle(PS_SOLID);
		break;
	case NetView::multi:
		GetDocument()->SetSelectedNodeLineWidth(2);
		GetDocument()->SetSelectedNodeLineStyle(PS_SOLID);
		GetDocument()->SetSelectedLinkWidth(2);
		GetDocument()->SetSelectedLinkLineStyle(PS_SOLID);
		break;
	}
}

void NetView::OnUpdateSetLineSolid1(CCmdUI* pCmdUI)
{
	if (m_selectStatus == NetView::single) {
		BOOL state = GetDocument()->GetSelectedNodeLineStyle() == PS_SOLID &&
			GetDocument()->GetSelectedNodeLineWidth() == 2;
		pCmdUI->SetCheck(state);
	}
	else if (m_selectStatus == NetView::link) {
		BOOL state = GetDocument()->GetSelectedLinkLineStyle() == PS_SOLID &&
			GetDocument()->GetSelectedLinkWidth() == 2;
		pCmdUI->SetCheck(state);
	}
}

void NetView::OnSetLineSolid2()
{
	bool bDrwAll = false;
	GetDocument()->BackupNodesForUndo();
	GetDocument()->BackupLinksForUndo();
	switch (m_selectStatus) {
	case NetView::single:
		GetDocument()->SetSelectedNodeLineWidth(3);
		GetDocument()->SetSelectedNodeLineStyle(PS_SOLID);
		break;
	case NetView::link:
		GetDocument()->SetSelectedLinkWidth(3);
		GetDocument()->SetSelectedLinkLineStyle(PS_SOLID);
		break;
	case NetView::multi:
		GetDocument()->SetSelectedNodeLineWidth(3);
		GetDocument()->SetSelectedNodeLineStyle(PS_SOLID);
		GetDocument()->SetSelectedLinkWidth(3);
		GetDocument()->SetSelectedLinkLineStyle(PS_SOLID);
		break;
	}
}

void NetView::OnUpdateSetLineSolid2(CCmdUI* pCmdUI)
{
	if (m_selectStatus == NetView::single) {
		BOOL state = GetDocument()->GetSelectedNodeLineStyle() == PS_SOLID &&
			GetDocument()->GetSelectedNodeLineWidth() == 3;
		pCmdUI->SetCheck(state);
	}
	else if (m_selectStatus == NetView::link) {
		BOOL state = GetDocument()->GetSelectedLinkLineStyle() == PS_SOLID &&
			GetDocument()->GetSelectedLinkWidth() == 3;
		pCmdUI->SetCheck(state);
	}
}

void NetView::OnSetLineSolid3()
{
	bool bDrwAll = false;
	GetDocument()->BackupNodesForUndo();
	GetDocument()->BackupLinksForUndo();
	switch (m_selectStatus) {
	case NetView::single:
		GetDocument()->SetSelectedNodeLineWidth(4);
		GetDocument()->SetSelectedNodeLineStyle(PS_SOLID);
		break;
	case NetView::link:
		GetDocument()->SetSelectedLinkWidth(4);
		GetDocument()->SetSelectedLinkLineStyle(PS_SOLID);
		break;
	case NetView::multi:
		GetDocument()->SetSelectedNodeLineWidth(4);
		GetDocument()->SetSelectedNodeLineStyle(PS_SOLID);
		GetDocument()->SetSelectedLinkWidth(4);
		GetDocument()->SetSelectedLinkLineStyle(PS_SOLID);
		break;
	}
}

void NetView::OnUpdateSetLineSolid3(CCmdUI* pCmdUI)
{
	if (m_selectStatus == NetView::single) {
		BOOL state = GetDocument()->GetSelectedNodeLineStyle() == PS_SOLID &&
			GetDocument()->GetSelectedNodeLineWidth() == 4;
		pCmdUI->SetCheck(state);
	}
	else if (m_selectStatus == NetView::link) {
		BOOL state = GetDocument()->GetSelectedLinkLineStyle() == PS_SOLID &&
			GetDocument()->GetSelectedLinkWidth() == 4;
		pCmdUI->SetCheck(state);
	}
}

void NetView::OnSetLineSolid4()
{
	bool bDrwAll = false;
	GetDocument()->BackupNodesForUndo();
	GetDocument()->BackupLinksForUndo();
	switch (m_selectStatus) {
	case NetView::single:
		GetDocument()->SetSelectedNodeLineWidth(5);
		GetDocument()->SetSelectedNodeLineStyle(PS_SOLID);
		break;
	case NetView::link:
		GetDocument()->SetSelectedLinkWidth(5);
		GetDocument()->SetSelectedLinkLineStyle(PS_SOLID);
		break;
	case NetView::multi:
		GetDocument()->SetSelectedNodeLineWidth(5);
		GetDocument()->SetSelectedNodeLineStyle(PS_SOLID);
		GetDocument()->SetSelectedLinkWidth(5);
		GetDocument()->SetSelectedLinkLineStyle(PS_SOLID);
		break;
	}
}

void NetView::OnUpdateSetLineSolid4(CCmdUI* pCmdUI)
{
	if (m_selectStatus == NetView::single) {
		BOOL state = GetDocument()->GetSelectedNodeLineStyle() == PS_SOLID &&
			GetDocument()->GetSelectedNodeLineWidth() == 5;
		pCmdUI->SetCheck(state);
	}
	else if (m_selectStatus == NetView::link) {
		BOOL state = GetDocument()->GetSelectedLinkLineStyle() == PS_SOLID &&
			GetDocument()->GetSelectedLinkWidth() == 5;
		pCmdUI->SetCheck(state);
	}
}

void NetView::OnSetLineDot()
{
	bool bDrwAll = false;
	GetDocument()->BackupNodesForUndo();
	GetDocument()->BackupLinksForUndo();
	switch (m_selectStatus) {
	case NetView::single:
		GetDocument()->SetSelectedNodeLineWidth(0);
		GetDocument()->SetSelectedNodeLineStyle(PS_DOT);
		break;
	case NetView::link:
		GetDocument()->SetSelectedLinkWidth(0);
		GetDocument()->SetSelectedLinkLineStyle(PS_DOT);
		break;
	case NetView::multi:
		GetDocument()->SetSelectedNodeLineWidth(0);
		GetDocument()->SetSelectedNodeLineStyle(PS_DOT);
		GetDocument()->SetSelectedLinkWidth(0);
		GetDocument()->SetSelectedLinkLineStyle(PS_DOT);
		break;
	}
}

void NetView::OnUpdateSetLineDot(CCmdUI* pCmdUI)
{
	if (m_selectStatus == NetView::single) {
		BOOL state = GetDocument()->GetSelectedNodeLineStyle() == PS_DOT &&
			GetDocument()->GetSelectedNodeLineWidth() == 0;
		pCmdUI->SetCheck(state);
	}
	else if (m_selectStatus == NetView::link) {
		BOOL state = GetDocument()->GetSelectedLinkLineStyle() == PS_DOT &&
			GetDocument()->GetSelectedLinkWidth() == 0;
		pCmdUI->SetCheck(state);
	}
}

void NetView::OnSetMultiLine()
{
	GetDocument()->SetSelectedNodeMultiLine(true);
}

void NetView::OnUpdateSetMultiLine(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(GetDocument()->IsSelectedNodeMultiLine() && m_selectStatus == NetView::single);
	pCmdUI->Enable(m_selectStatus == NetView::single || m_selectStatus == NetView::multi);
}

void NetView::OnSetSingleLine()
{
	GetDocument()->SetSelectedNodeMultiLine(false);
}

void NetView::OnUpdateSetSingleLine(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(!GetDocument()->IsSelectedNodeMultiLine() && m_selectStatus == NetView::single);
	pCmdUI->Enable(m_selectStatus == NetView::single || m_selectStatus == NetView::multi);
}

void NetView::OnAddRect()
{
	if (m_addMode != NetView::rect) {
		m_addMode = NetView::rect;
	}
	else if (m_addMode == NetView::rect) {
		m_addMode = NetView::normal;
	}
}

void NetView::OnUpdateAddRect(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_bGrasp && !m_bZooming);
	pCmdUI->SetCheck(m_addMode == NetView::rect);
}

void NetView::OnAddRoundRect()
{
	if (m_addMode != NetView::rRect) {
		m_addMode = NetView::rRect;
	}
	else if (m_addMode == NetView::rRect) {
		m_addMode = NetView::normal;
	}
}

void NetView::OnUpdateAddRoundRect(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_bGrasp && !m_bZooming);
	pCmdUI->SetCheck(m_addMode == NetView::rRect);
}

void NetView::OnAddArc()
{
	if (m_addMode != NetView::arc) {
		m_addMode = NetView::arc;
	}
	else if (m_addMode == NetView::arc) {
		m_addMode = NetView::normal;
	}
}

void NetView::OnUpdateAddArc(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_bGrasp && !m_bZooming);
	pCmdUI->SetCheck(m_addMode == NetView::arc);
}

void NetView::AddNode(const CPoint &logPt, const CPoint& screenPt, const CString& s)
{
	CreateNodeDlg dlg;
	CPoint curPt = GetScrollPosition();

	dlg.m_initialPt.x = screenPt.x;
	dlg.m_initialPt.y = screenPt.y;
	if (s != _T("")) {
		dlg.m_strcn = s;
	}

	if (dlg.DoModal() != IDOK) {
		m_addMode = NetView::normal;
		return;
	}
	if (dlg.m_strcn == _T("")) {
		m_addMode = NetView::normal;
		return;
	}
	GetDocument()->DisableUndo();
	if (m_addMode == NetView::rect) {
		GetDocument()->AddNodeRect(dlg.m_strcn, logPt);
	}
	else if (m_addMode == NetView::arc) {
		GetDocument()->AddNodeArc(dlg.m_strcn, logPt);
	}
	else if (m_addMode == NetView::rRect) {
		GetDocument()->AddNodeRoundedRect(dlg.m_strcn, logPt);
	}
	else if (m_addMode == NetView::label) {
		GetDocument()->AddNodeRect(dlg.m_strcn, logPt, true, true);
	}
	m_addMode = NetView::normal;
}

void NetView::OnAddLink1()
{
	if (m_addMode == NetView::link1) {
		m_addMode = NetView::normal;
	}
	else {
		m_addMode = NetView::link1;
	}
}

void NetView::OnUpdateAddLink1(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_bGrasp && !m_bZooming);
	pCmdUI->SetCheck(m_addMode == NetView::link1);
}

void NetView::OnAddLink2()
{
	if (m_addMode == NetView::link2) {
		m_addMode = NetView::normal;
	}
	else {
		m_addMode = NetView::link2;
	}
}

void NetView::OnUpdateAddLink2(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_bGrasp && !m_bZooming);
	pCmdUI->SetCheck(m_addMode == NetView::link2);
}

void NetView::OnSetLinkInfo()
{
	EditLinkProps();
}

void NetView::EditLinkProps()
{
	CString sFrom, sTo, comment;
	int arrowType = 0;
	GetDocument()->GetSelectedLinkInfo(sFrom, sTo, comment, arrowType);
	LinkPropertiesDlg dlg;
	dlg.strFrom = sFrom;
	dlg.strTo = sTo;
	dlg.strComment = comment;
	dlg.styleArrow = arrowType;
	dlg.lineWidth = GetDocument()->GetSelectedLinkWidth();
	dlg.colorLine = GetDocument()->GetSelectedLinkLineColor();
	dlg.styleLine = GetDocument()->GetSelectedLinkLineStyle();
	GetDocument()->GetSelectedLinkFont(dlg.lf);

	if (dlg.DoModal() != IDOK) return;
	GetDocument()->DisableUndo();
	GetDocument()->BackupLinksForUndo();

	GetDocument()->SetSelectedLinkInfo(dlg.strComment, dlg.styleArrow);
	GetDocument()->SetSelectedLinkFont(dlg.lf);
	GetDocument()->SetSelectedLinkLineColor(dlg.colorLine);
	GetDocument()->SetSelectedLinkLineStyle(dlg.styleLine);
	GetDocument()->SetSelectedLinkWidth(dlg.lineWidth);
}

void NetView::OnUpdateSetLinkInfo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::link);
}

void NetView::OnDelete()
{
	GetDocument()->DisableUndo();
	if (m_selectStatus == NetView::single) {
		CRect old = GetDocument()->GetRelatedBound(); AdjustRedrawBound(old);
		GetDocument()->DeleteSelectedNode();
		InvalidateRect(old);
	}
	else if (m_selectStatus == NetView::link) {
		CString s = '<' + GetDocument()->GetSelectedLinkLabel() + _T(">\n" + "削除しますか");
		if (MessageBox(s, _T("リンクの削除"), MB_YESNO) != IDYES) return;
		GetDocument()->DeleteSelectedLink();
	}
	else if (m_selectStatus == NetView::multi) {
		DeleteSelectedNodes();
	}
}

void NetView::DeleteSelectedNodes() {
	if (MessageBox(_T("選択したノードおよび配下のノードがすべて削除されます"), _T("ノードの削除"), MB_YESNO) != IDYES) return;
	if (GetDocument()->ShowSubBranch()) {
		CString mes = _T("「") + GetDocument()->GetSubBranchRootLabel() + _T("」");
		mes += _T("配下のノードをすべて表示するモードです。");
		mes += _T("選択したノード以外にも表示されているノードが削除される可能性があります。\n続行しますか?");
		if (MessageBox(mes, _T("選択範囲のノードを削除"), MB_YESNO) != IDYES) return;
	}
	GetDocument()->DeleteSelectedNodes();
	Invalidate();
}

void NetView::OnUpdateDelete(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(
		m_selectStatus == NetView::single &&  GetDocument()->CanDeleteNode() ||
		m_selectStatus == NetView::multi ||
		m_selectStatus == NetView::link);
}

void NetView::AdjustRedrawBound(CRect &rc)
{
	rc.InflateRect(5, 5);
	LPtoDP(rc);
}

void NetView::OnSetNodeProp()
{
	EditNodeProps();
}

void NetView::OnUpdateSetNodeProp(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::single || m_selectStatus == NetView::multi);
}

void NetView::EditNodeProps()
{
	NodePropertiesDlg dlg;
	dlg.colorLine = GetDocument()->GetSelectedNodeLineColor();
	dlg.colorFill = GetDocument()->GetSelectedNodeBrsColor();
	dlg.colorFont = GetDocument()->GetSelectedNodeFontColor();
	dlg.lineWidth = GetDocument()->GetSelectedNodeLineWidth();
	dlg.styleLine = GetDocument()->GetSelectedNodeLineStyle();
	dlg.bMultiLine = GetDocument()->IsSelectedNodeMultiLine();
	dlg.m_bNoBrush = !GetDocument()->IsSelectedNodeFilled();
	dlg.bOldBynary = GetDocument()->IsOldBinary();
	int shape = GetDocument()->GetSelectedNodeShape();
	if (shape == iNode::rectangle) {
		dlg.m_shape = 0;
	}
	else if (shape == iNode::roundRect) {
		dlg.m_shape = 1;
	}
	else if (shape == iNode::arc) {
		dlg.m_shape = 2;
	}
	bool mline = GetDocument()->IsSelectedNodeMultiLine();
	if (mline) {
		dlg.m_TLine = 1;
	}
	else {
		dlg.m_TLine = 0;
	}

	int ts = GetDocument()->GetSelectedNodeTextStyle();
	if (ts == iNode::notext) {
		dlg.m_TLine = 2;
	}

	if (ts == iNode::s_cc || ts == iNode::s_tc || ts == iNode::s_bc || ts == iNode::m_c) {
		dlg.horiz = 1;
	}
	else if (ts == iNode::s_cl || ts == iNode::s_tl || ts == iNode::s_bl || ts == iNode::m_l) {
		dlg.horiz = 0;
	}
	else if (ts == iNode::s_cr || ts == iNode::s_tr || ts == iNode::s_br || ts == iNode::m_r) {
		dlg.horiz = 2;
	}
	else {
		dlg.horiz = 1;
	}

	if (ts == iNode::s_tc || ts == iNode::s_tl || ts == iNode::s_tr) {
		dlg.vert = 0;
	}
	else if (ts == iNode::s_cc || ts == iNode::s_cl || ts == iNode::s_cr) {
		dlg.vert = 1;
	}
	else if (ts == iNode::s_bc || ts == iNode::s_bl || ts == iNode::s_br) {
		dlg.vert = 2;
	}
	else {
		dlg.vert = 1;
	}
	dlg.m_strLabel = GetDocument()->GetSelectedNodeLabel();
	GetDocument()->GetSelectedNodeFont(dlg.lf);
	GetDocument()->GetSelectedNodeMargin(
		dlg.margins.l, dlg.margins.r, dlg.margins.t, dlg.margins.b);

	if (dlg.DoModal() != IDOK) return;
	GetDocument()->BackupNodesForUndo();

	BOOL oldSetting = ((CiEditApp*)AfxGetApp())->m_rgsNode.bDisableNodeResize;
	((CiEditApp*)AfxGetApp())->m_rgsNode.bDisableNodeResize = TRUE;

	GetDocument()->SetSelectedNodeLineColor(dlg.colorLine);
	GetDocument()->SetSelectedNodeBrush(dlg.colorFill);
	GetDocument()->SetSelectedNodeFontColor(dlg.colorFont);
	GetDocument()->SetSelectedNodeLineWidth(dlg.lineWidth);
	GetDocument()->SetSelectedNodeLineStyle(dlg.styleLine);
	if (dlg.m_bNoBrush) {
		GetDocument()->SetSelectedNodeNoBrush(FALSE);
	}
	else {
		GetDocument()->SetSelectedNodeNoBrush(TRUE);
	}
	if (dlg.m_shape == 0) {
		GetDocument()->SetSelectedNodeShape(iNode::rectangle);
	}
	else if (dlg.m_shape == 1) {
		GetDocument()->SetSelectedNodeShape(iNode::roundRect);
	}
	else if (dlg.m_shape == 2) {
		GetDocument()->SetSelectedNodeShape(iNode::arc);
	}
	if (dlg.m_TLine == 1) {
		GetDocument()->SetSelectedNodeMultiLine();
	}
	else if (dlg.m_TLine == 0) {
		GetDocument()->SetSelectedNodeMultiLine(false);
	}
	if (dlg.m_TLine == 2) {
		GetDocument()->SetSelectedNodeTextStyle(iNode::notext);
	}

	if (dlg.m_TLine == 0) {
		if (dlg.vert == 0 && dlg.horiz == 0) {
			GetDocument()->SetSelectedNodeTextStyle(iNode::s_tl);
		}
		else if (dlg.vert == 1 && dlg.horiz == 0) {
			GetDocument()->SetSelectedNodeTextStyle(iNode::s_cl);
		}
		else if (dlg.vert == 2 && dlg.horiz == 0) {
			GetDocument()->SetSelectedNodeTextStyle(iNode::s_bl);
		}
		else if (dlg.vert == 0 && dlg.horiz == 1) {
			GetDocument()->SetSelectedNodeTextStyle(iNode::s_tc);
		}
		else if (dlg.vert == 1 && dlg.horiz == 1) {
			GetDocument()->SetSelectedNodeTextStyle(iNode::s_cc);
		}
		else if (dlg.vert == 2 && dlg.horiz == 1) {
			GetDocument()->SetSelectedNodeTextStyle(iNode::s_bc);
		}
		else if (dlg.vert == 0 && dlg.horiz == 2) {
			GetDocument()->SetSelectedNodeTextStyle(iNode::s_tr);
		}
		else if (dlg.vert == 1 && dlg.horiz == 2) {
			GetDocument()->SetSelectedNodeTextStyle(iNode::s_cr);
		}
		else if (dlg.vert == 2 && dlg.horiz == 2) {
			GetDocument()->SetSelectedNodeTextStyle(iNode::s_br);
		}
	}
	else if (dlg.m_TLine == 1) {
		if (dlg.horiz == 0) {
			GetDocument()->SetSelectedNodeTextStyle(iNode::m_l);
		}
		else if (dlg.horiz == 1) {
			GetDocument()->SetSelectedNodeTextStyle(iNode::m_c);
		}
		else if (dlg.horiz == 2) {
			GetDocument()->SetSelectedNodeTextStyle(iNode::m_r);
		}
	}
	if (!GetDocument()->IsOldBinary()) {
		GetDocument()->SetSelectedNodeMargin(
			dlg.margins.l, dlg.margins.r, dlg.margins.t, dlg.margins.b);
	}
	GetDocument()->SetSelectedNodeFont(dlg.lf);
	((CiEditApp*)AfxGetApp())->m_rgsNode.bDisableNodeResize = oldSetting;
	GetDocument()->SetSelectedNodeLabel(dlg.m_strLabel);
}

void NetView::OnGraspMode()
{
	m_bGrasp = !m_bGrasp;
}

void NetView::OnUpdateGraspMode(CCmdUI* pCmdUI)
{
	CPoint maxPt;
	maxPt = GetDocument()->GetMaxPt();
	CRect rc; CScrollView::GetWindowRect(&rc);

	pCmdUI->Enable((rc.Width() < (int)(maxPt.x*m_fZoomScale)
		|| rc.Height() < (int)(maxPt.y*m_fZoomScale))
		&& m_addMode == NetView::normal);
	pCmdUI->SetCheck(m_bGrasp);
}

void NetView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	if (m_bGrasp) {
		m_bGrasp = false;
	}
}

void NetView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_bZooming) {
		CPoint logPt = point; DPtoLP(&logPt);
		ZoomIn(&logPt, 0.1f);
		return;
	}

	if (m_selectStatus == NetView::single) {
		EditNodeProps();
	}
	else if (m_selectStatus == NetView::link) {
		EditLinkProps();
	}
	else if (m_selectStatus == NetView::none) {
		CPoint logPt = point; DPtoLP(&logPt);
		CPoint spt(point);
		ClientToScreen(&spt);
		int shape = ((CiEditApp*)AfxGetApp())->m_rgsNode.shape;
		switch (shape) {
		case iNode::rectangle:
			m_addMode = NetView::rect;
			break;
		case iNode::arc:
			m_addMode = NetView::arc;
			break;
		case iNode::roundRect:
			m_addMode = NetView::rRect;
			break;
		}
		AddNode(logPt, spt);
	}
	CScrollView::OnLButtonDblClk(nFlags, point);
}

void NetView::OnLinkStraight()
{
	CPoint pt(0, 0);
	GetDocument()->CurveSelectedLink(pt, false);
}

void NetView::OnUpdateLinkStraight(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->IsSelectedLinkCurved() && !GetDocument()->IsSelectedLinkSelfReferential());
}

CRect NetView::GetCurveBound(const CPoint &pt)
{
	CPoint start, end;
	GetDocument()->GetSelectedLinkEndPoints(start, end);
	CRect rc1(start, pt); rc1.NormalizeRect();
	CRect rc2(pt, end);   rc2.NormalizeRect();
	return rc1 | rc2;
}

void NetView::CopyEmf()
{
	HENHMETAFILE hmetafile = NULL;
	CClientDC dc(this);
	if (m_selectStatus == NetView::none || m_selectStatus == NetView::link) {
		CRect selRect;
		CRect allR;
		allR = CRect(CPoint(0, 0), GetDocument()->GetMaxPt());
		int selcnt = GetDocument()->SelectNodesInBound(allR, selRect);
		CRect nwBound = allR;
		GetDocument()->SelectLinksInBound(nwBound);
		selRect |= GetDocument()->GetSelectedLinkBound();

		CMetaFileDC mfDC;
		CPoint p1(0, 0);
		CPoint p2(nwBound.Width(), nwBound.Height());
		CRect rc(0, 0, (int)((double)(p2.x)*m_mfWidth), (int)((double)(p2.y)*m_mfHeight));
		mfDC.CreateEnhanced(&dc, NULL, &rc, _T("iEdit"));

		GetDocument()->DrawNodes(&mfDC);
		GetDocument()->DrawLinks(&mfDC, true);

		hmetafile = mfDC.CloseEnhanced();
	}
	else if (m_selectStatus == NetView::single) {
		CMetaFileDC mfDC;
		CPoint p1(0, 0);
		CPoint p2(m_selectRect.Width(), m_selectRect.Height());
		CRect rc(0, 0, (int)((double)(p2.x)*m_mfWidth), (int)((double)(p2.y)*m_mfHeight));
		mfDC.CreateEnhanced(&dc, NULL, &rc, _T("iEdit"));
		mfDC.SetViewportOrg(-m_selectRect.left, -m_selectRect.top);
		GetDocument()->DrawNodesSelected(&mfDC);
		hmetafile = mfDC.CloseEnhanced();
	}
	else if (m_selectStatus == NetView::multi) {
		CRect selRect;

		int selcnt = GetDocument()->SelectNodesInBound(m_selectRect, selRect);
		CRect nwBound = GetDocument()->GetRelatedBoundAnd(false);
		GetDocument()->SelectLinksInBound(nwBound);
		selRect |= GetDocument()->GetSelectedLinkBound();

		CMetaFileDC mfDC;
		CPoint p1(0, 0);
		CPoint p2(nwBound.Width(), nwBound.Height());
		CRect rc(0, 0, (int)((double)(p2.x)*m_mfWidth), (int)((double)(p2.y)*m_mfHeight));
		mfDC.CreateEnhanced(&dc, NULL, &rc, _T("iEdit"));
		mfDC.SetViewportOrg(-nwBound.left, -nwBound.top);

		GetDocument()->DrawNodesSelected(&mfDC);
		GetDocument()->DrawLinksSelected(&mfDC, true);

		hmetafile = mfDC.CloseEnhanced();
	}

	if (m_selectStatus == NetView::none || m_selectStatus == NetView::link) {
		CRect rc;
		GetDocument()->SelectNodesInBound(CRect(0, 0, 0, 0), rc);
		GetDocument()->SelectLinksInBound(CRect(0, 0, 0, 0));
	}

	// ------------------------------
	// クリップボードにデータをセット
	// ------------------------------
	if (hmetafile == NULL) return;
	SendEmfToClipboard(hmetafile);
}

void NetView::OnNoBrush()
{
	GetDocument()->BackupNodesForUndo();

	if (GetDocument()->IsSelectedNodeFilled()) {
		GetDocument()->SetSelectedNodeNoBrush(FALSE);
	}
	else {
		GetDocument()->SetSelectedNodeNoBrush(TRUE);
	}
}

void NetView::OnUpdateNoBrush(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::single);
	pCmdUI->SetCheck(!GetDocument()->IsSelectedNodeFilled() && m_selectStatus == NetView::single);
}

void NetView::OnSetNodeBrush()
{
	if (m_selectStatus == NetView::single || m_selectStatus == NetView::multi) {
		CColorDialog dlg(GetDocument()->GetSelectedNodeBrsColor());
		if (dlg.DoModal() != IDOK) return;
		GetDocument()->BackupNodesForUndo();
		GetDocument()->SetSelectedNodeBrush(dlg.GetColor());
	}
}

void NetView::OnUpdateSetNodeBrush(CCmdUI* pCmdUI)
{
}

void NetView::OnEditCopy()
{
	if (!m_selectRect.IsRectEmpty()) {
		GetDocument()->AddSelectedNodesToCopyOrg();
	}

	CopyEmf();
}

void NetView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void NetView::OnEditPaste()
{
	if (!GetDocument()->CanDuplicateNodes() &&
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
			ptDrop = CPoint(rand() % 150, rand() % 150);
		}
		else {
			ptDrop = m_ptPaste;
		}

		HENHMETAFILE hm = NULL;

		if (::IsClipboardFormatAvailable(CF_ENHMETAFILE)) {
			hm = (HENHMETAFILE)::GetClipboardData(CF_ENHMETAFILE);
		}
		else if (::IsClipboardFormatAvailable(CF_DIB) || ::IsClipboardFormatAvailable(CF_BITMAP)) {
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
		}
		GetDocument()->DisableUndo();
		GetDocument()->AddShapeNode(_T("図形"), ptDrop, iNode::MetaFile, hm);
		if (!EmptyClipboard()) {
			AfxMessageBox(_T("Cannot empty the Clipboard"));
			return;
		}
		CloseClipboard();
		return;
	}

	if (m_ptPaste == CPoint(0, 0)) {
		GetDocument()->DuplicateNodes(CPoint(0, 0));
	}
	else {
		GetDocument()->DuplicateNodes(m_ptPaste, false);
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
	pCmdUI->Enable(GetDocument()->CanDuplicateNodes() ||
		::IsClipboardFormatAvailable(CF_ENHMETAFILE) ||
		::IsClipboardFormatAvailable(CF_METAFILEPICT) ||
		::IsClipboardFormatAvailable(CF_DIB) ||
		::IsClipboardFormatAvailable(CF_BITMAP)
	);
}

void NetView::OnSetNodeRect()
{
	GetDocument()->BackupNodesForUndo();
	GetDocument()->SetSelectedNodeShape(iNode::rectangle);
}

void NetView::OnUpdateSetNodeRect(CCmdUI* pCmdUI)
{
	int shape = GetDocument()->GetSelectedNodeShape();
	pCmdUI->Enable(shape != iNode::rectangle && shape != iNode::MetaFile);
}

void NetView::OnSetNodeRoundRect()
{
	GetDocument()->BackupNodesForUndo();
	GetDocument()->SetSelectedNodeShape(iNode::roundRect);
}

void NetView::OnUpdateSetNodeRoundRect(CCmdUI* pCmdUI)
{
	int shape = GetDocument()->GetSelectedNodeShape();
	pCmdUI->Enable(shape != iNode::roundRect && shape != iNode::MetaFile);
}

void NetView::OnSetNodeArc()
{
	GetDocument()->BackupNodesForUndo();
	GetDocument()->SetSelectedNodeShape(iNode::arc);
}

void NetView::OnUpdateSetNodeArc(CCmdUI* pCmdUI)
{
	int shape = GetDocument()->GetSelectedNodeShape();
	pCmdUI->Enable(shape != iNode::arc && shape != iNode::MetaFile);
}

void NetView::OnFixNode()
{
	BOOL fix = GetDocument()->IsSelectedNodeFixed();
	if (fix) {
		GetDocument()->SetSelectedNodeFixed(FALSE);
	}
	else {
		GetDocument()->SetSelectedNodeFixed();
	}
}

void NetView::OnUpdateFixNode(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(GetDocument()->IsSelectedNodeFixed());
}

void NetView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (::GetKeyState(VK_CONTROL) & 0x8000 || ::GetKeyState(VK_MENU) & 0x8000) {
		CScrollView::OnKeyUp(nChar, nRepCnt, nFlags);
		return;
	}

	if (nChar == VK_ESCAPE) {
		if (m_bLinkCurving) {
			m_bLinkCurving = false;
			AdjustRedrawBound(m_preRedrawBound);
			InvalidateRect(m_preRedrawBound);
		}
	}
	else if (nChar == VK_TAB) {
		bool bDrwAll = false;
		iHint h;
		if (bDrwAll) {
			h.event = iHint::nextNode;
		}
		else {
			h.event = iHint::nextNodeSibling;
		}
		GetDocument()->UpdateAllViews(NULL, NULL, &h);
	}
	else if (nChar == VK_UP || nChar == VK_DOWN || nChar == VK_LEFT || nChar == VK_RIGHT) {
		m_cntUp = 0;
		m_cntDown = 0;
		m_cntLeft = 0;
		m_cntRight = 0;
	}
	else if (nChar == VK_CONTROL) {
		if (m_bDragRelax) {
			CancelDragRelax();
		}
	}
	else if (nChar >= 0x30 && nChar <= 0x5A || nChar == VK_SPACE) {
		int shape = ((CiEditApp*)AfxGetApp())->m_rgsNode.shape;
		switch (shape) {
		case iNode::rectangle:
			m_addMode = NetView::rect;
			break;
		case iNode::arc:
			m_addMode = NetView::arc;
			break;
		case iNode::roundRect:
			m_addMode = NetView::rRect;
			break;
		}
		CPoint logPt = m_ptScreen;
		logPt -= GetScrollPosition();
		DPtoLP(&logPt);
		CPoint spt(m_ptScreen);
		ClientToScreen(&spt);
		spt -= GetScrollPosition();
		AddNode(logPt, spt);
		m_ptScreen += CPoint((int)(30 * m_fZoomScale), (int)(30 * m_fZoomScale));
	}
	CScrollView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void NetView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int dif;
	if (nChar == VK_UP) {
		if (m_cntUp == 0) {
			GetDocument()->DisableUndo();
			GetDocument()->BackupNodesForUndo();
			GetDocument()->BackupLinksForUndo();
		}
		m_cntUp++;
		if (m_bAccel) {
			dif = 1 + m_cntUp / 5;
		}
		else {
			dif = 1;
		}
		MoveByCursorKey(0, -dif);
	}
	else if (nChar == VK_DOWN) {
		if (m_cntDown == 0) {
			GetDocument()->DisableUndo();
			GetDocument()->BackupNodesForUndo();
			GetDocument()->BackupLinksForUndo();
		}
		m_cntDown++;
		if (m_bAccel) {
			dif = 1 + m_cntDown / 5;
		}
		else {
			dif = 1;
		}
		MoveByCursorKey(0, dif);
	}
	else if (nChar == VK_LEFT) {
		if (m_cntLeft == 0) {
			GetDocument()->DisableUndo();
			GetDocument()->BackupNodesForUndo();
			GetDocument()->BackupLinksForUndo();
		}
		m_cntLeft++;
		if (m_bAccel) {
			dif = 1 + m_cntLeft / 5;
		}
		else {
			dif = 1;
		}
		MoveByCursorKey(-dif, 0);
	}
	else if (nChar == VK_RIGHT) {
		if (m_cntRight == 0) {
			GetDocument()->DisableUndo();
			GetDocument()->BackupNodesForUndo();
			GetDocument()->BackupLinksForUndo();
		}
		m_cntRight++;
		if (m_bAccel) {
			dif = 1 + m_cntRight / 5;
		}
		else {
			dif = 1;
		}
		MoveByCursorKey(dif, 0);
	}
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void NetView::MoveByCursorKey(int dx, int dy)
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

	CRect old = GetDocument()->GetRelatedBound(); AdjustRedrawBound(old);

	if (m_selectStatus == NetView::single) {
		tracker.m_nStyle = CRectTracker::resizeInside;
		GetDocument()->SetSelectedNodeBound(m_selectRect);
		int moveX = m_selectRect.left - org.left;
		int moveY = m_selectRect.top - org.top;
		GetDocument()->MoveSelectedLink(CSize(moveX, moveY));
	}
	else if (m_selectStatus == NetView::multi) {
		tracker.m_nStyle = CRectTracker::hatchedBorder | CRectTracker::resizeInside;
		GetDocument()->MoveSelectedNode(CSize(dx, dy));
		GetDocument()->MoveSelectedLink(CSize(dx, dy));
	}
	CRect rdnw = GetDocument()->GetRelatedBound(); AdjustRedrawBound(rdnw);

	CRect rc;
	if (!rc.IntersectRect(old, rdnw)) {
		InvalidateRect(old);
		InvalidateRect(rdnw);
	}
	else {
		CRect rdRec = old | rdnw;
		InvalidateRect(rdRec);
	}

	CPoint maxPt;
	maxPt = GetDocument()->GetMaxPt();
	AdjustScrollArea();
}

void NetView::OnNormalCursor()
{
	m_addMode = NetView::normal;
}

void NetView::OnUpdateNormalCursor(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_addMode == NetView::normal && !m_bGrasp && !m_bZooming);
}

int NetView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pDC = new CClientDC(this);
	m_bAccel = AfxGetApp()->GetProfileInt(REGS_OTHER, _T("Accel Move"), TRUE);
	m_pShapesDlg = new ShapesManagementDlg;
	m_pShapesDlg->Create(_T(""), _T(""), SW_HIDE, CRect(0, 0, 0, 0), this, IDD_SHAPES);
	ApplyColorSetting();
	SetMetaFileSize();
	EnableToolTips();
	return 0;
}

void NetView::OnDestroy()
{
	CScrollView::OnDestroy();

	m_pDC = NULL;
	m_pShapesDlg->DestroyWindow();
	delete m_pShapesDlg;
}

void NetView::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	pDC->SetMapMode(MM_ISOTROPIC);
	CPoint pt = GetDocument()->GetMaxPt();
	CRect pr = pInfo->m_rectDraw;

	int wx = (int)(pt.x / ((pr.Width() / 2) / (double)pDC->GetDeviceCaps(LOGPIXELSX)));
	int wy = (int)(pt.y / ((pr.Height() / 2) / (double)pDC->GetDeviceCaps(LOGPIXELSY)));

	CSize oldWnExt = pDC->SetWindowExt(wx, wy);
	pDC->SetViewportExt(CSize(pt.x, pt.y));
	pDC->SetViewportOrg(0, 0);

	GetDocument()->DrawNodes(pDC);
	GetDocument()->DrawLinks(pDC);

	pDC->SetWindowExt(oldWnExt);
}

BOOL NetView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void NetView::OnUpdateFilePrint(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void NetView::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void NetView::OnRandomize()
{
	CRect rc; GetClientRect(rc);
	GetDocument()->DisableUndo();
	GetDocument()->BackupNodesForUndo();
	GetDocument()->BackupLinksForUndo();
	GetDocument()->RandomizeNodesPos(CSize((int)(rc.Width()*0.8), (int)(rc.Height()*0.8)));
	m_selectRect = CRect(0, 0, 0, 0);
	AdjustScrollArea();
	Invalidate();
}

void NetView::OnUpdateRandomize(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void NetView::AdjustScrollArea()
{
	CPoint maxPt = GetDocument()->GetMaxPt();
	CSize sizeTotal;
	sizeTotal.cx = (LONG)(maxPt.x*1.05);
	sizeTotal.cy = (LONG)(maxPt.y*1.05);

	SetScrollSizes(MM_ANISOTROPIC, sizeTotal);
	UpdateViewport(NULL);
	SendMessage(WM_SIZE); // update時に必要
}

void NetView::OnEditSelectAll()
{
	SelectAll();
}

void NetView::SelectAll()
{
	bool bDrwAll = false;
	CRect selRect;
	CRect allR;
	allR = CRect(CPoint(0, 0), GetDocument()->GetMaxPt());
	int selcnt = GetDocument()->SelectNodesInBound(allR, selRect);
	CRect old = m_selectRect; AdjustRedrawBound(old);

	m_selectRect = selRect;
	if (selcnt > 1) {
		m_selectStatus = NetView::multi;
		GetDocument()->SelectLinksInBound(allR);
		m_selectRect |= GetDocument()->GetSelectedLinkBound();
	}
	else if (selcnt == 1) {
		m_selectStatus = NetView::single;
	}
	CRect rdRect = m_selectRect; AdjustRedrawBound(rdRect);
	InvalidateRect(rdRect);
	InvalidateRect(old);
}

void NetView::OnUpdateEditSelectAll(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_addMode == NetView::normal);
}

void NetView::OnRefreshNetView()
{
	GetDocument()->SetConnectionPoint();
	Invalidate();
}

void NetView::OnUpdateRefreshNetView(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void NetView::OnShowNodeShapes()
{
	m_pShapesDlg->ShowWindow(SW_SHOWNORMAL);
	m_pShapesDlg->SetFocus();
}

void NetView::OnUpdateShowNodeShapes(CCmdUI* pCmdUI)
{
}

LRESULT NetView::OnAddMetaFileID(UINT wParam, LONG lParam)
{
	if (m_selectStatus == NetView::multi || m_selectStatus == NetView::link) return 0;

	CiEditApp* pApp = (CiEditApp*)AfxGetApp();

	GetDocument()->DisableUndo();
	if (m_selectStatus == NetView::none) {
		GetDocument()->AddShapeNode(_T("図形"), CPoint(rand() % 150, rand() % 150), iNode::MetaFile, pApp->m_hMetaFiles[(int)wParam]);
	}
	else if (m_selectStatus == NetView::single) {
		GetDocument()->BackupNodesForUndo();
		GetDocument()->SetSelectedNodeShape(iNode::MetaFile, (int)wParam);
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
}

void NetView::HideChildWindow()
{
	m_pShapesDlg->ShowWindow(SW_HIDE);
}

LRESULT NetView::OnRegNodeMetaFile(UINT wParam, LONG lParam)
{
	if (m_selectStatus == NetView::multi || m_selectStatus == NetView::link) return 0;
	if (GetDocument()->GetSelectedNodeShape() == iNode::MetaFile) {
		m_pShapesDlg->regNodeShape(GetDocument()->GetSelectedNodeMetaFile());
	}
	else {
		AfxMessageBox(_T("シェイプではありません"));
	}
	return 0;
}

void NetView::OnCopyToClipbrd()
{
	CPoint p1(0, 0);
	CPoint p2 = GetDocument()->GetMaxPt();

	CMetaFileDC* pMfDC = new CMetaFileDC();
	CRect rc(0, 0, (int)(abs(p2.x - p1.x)*m_mfWidth), (int)(abs(p2.y - p1.y)*m_mfHeight));
	CClientDC dc(this);
	pMfDC->CreateEnhanced(&dc, NULL, &rc, _T("iEdit"));
	pMfDC->SetAttribDC(dc);

	GetDocument()->DrawNodes(pMfDC);
	GetDocument()->DrawLinks(pMfDC, true);
	HENHMETAFILE hmetafile = pMfDC->CloseEnhanced();
	delete pMfDC;

	// クリップボードにデータをセット
	SendEmfToClipboard(hmetafile);
}

void NetView::OnUpdateCopyToClipbrd(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void NetView::OnEditUndo()
{
	// ビューポート調整に問題あり

	GetDocument()->RestoreNodesForUndo();
	GetDocument()->RestoreLinksForUndo();
	m_selectRect = GetDocument()->GetSelectedNodeRect();
	m_selectStatus = NetView::single;

	Invalidate();

	CPoint maxPt;
	maxPt = GetDocument()->GetMaxPt();
	AdjustScrollArea();
}

void NetView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDocument()->CanUndo());
}

void NetView::OnAdjustTop()
{
	AlignNodesSide(_T("top"));
}

void NetView::OnUpdateAdjustTop(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::multi);
}

void NetView::OnAdjustBottom()
{
	AlignNodesSide(_T("bottom"));
}

void NetView::OnUpdateAdjustBottom(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::multi);
}

void NetView::OnAdjustLeft()
{
	AlignNodesSide(_T("left"));
}

void NetView::OnUpdateAdjustLeft(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::multi);
}

void NetView::OnAdjustRight()
{
	AlignNodesSide(_T("right"));
}

void NetView::OnUpdateAdjustRight(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::multi);
}

void NetView::AlignNodesSide(const CString& side)
{
	CRect old = GetDocument()->GetRelatedBound(); AdjustRedrawBound(old);
	GetDocument()->BackupNodesForUndo();
	GetDocument()->BackupLinksForUndo();

	GetDocument()->AlignNodesInBoundTo(side, m_selectRect);

	CRect nwrd = GetDocument()->GetRelatedBoundAnd(false);
	m_selectRect = nwrd;
	AdjustRedrawBound(nwrd);
	CRect rc;
	if (!rc.IntersectRect(old, nwrd)) {
		InvalidateRect(old);
		InvalidateRect(nwrd);
	}
	else {
		CRect rd = old | nwrd;
		InvalidateRect(rd);
	}

	AdjustScrollArea();
}

void NetView::OnSameHeight()
{
	AlignNodesSize(_T("height"));
}

void NetView::OnUpdateSameHeight(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::multi);
}

void NetView::OnSameRect()
{
	AlignNodesSize(_T("rect"));
}

void NetView::OnUpdateSameRect(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::multi);
}

void NetView::OnSameWidth()
{
	AlignNodesSize(_T("width"));
}

void NetView::OnUpdateSameWidth(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::multi);
}

void NetView::AlignNodesSize(const CString &strSize)
{
	CRect old = GetDocument()->GetRelatedBound(); AdjustRedrawBound(old);
	GetDocument()->BackupNodesForUndo();
	GetDocument()->BackupLinksForUndo();

	GetDocument()->AlignSelectedNodesToSameSize(strSize);

	CRect nwrd = GetDocument()->GetRelatedBoundAnd(false);
	m_selectRect = nwrd;
	AdjustRedrawBound(nwrd);
	CRect rc;
	if (!rc.IntersectRect(old, nwrd)) {
		InvalidateRect(old);
		InvalidateRect(nwrd);
	}
	else {
		CRect rd = old | nwrd;
		InvalidateRect(rd);
	}

	AdjustScrollArea();
}

void NetView::OnReverseLinkDirection()
{
	GetDocument()->BackupLinksForUndo();
	GetDocument()->ReverseSelectedLinkDirection();
}

void NetView::OnUpdateReverseLinkDirection(CCmdUI* pCmdUI)
{
}

void NetView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	pDC->SetMapMode(m_nMapMode);
	pDC->SetWindowExt(m_totalLog);

	CPoint ptVpOrg;
	pDC->SetViewportExt(m_totalDev); // in device coordinates

	ASSERT(pDC->GetWindowOrg() == CPoint(0, 0));
	ptVpOrg = -GetDeviceScrollPosition();

	pDC->SetViewportOrg(ptVpOrg);
	CView::OnPrepareDC(pDC, pInfo);
}

void NetView::SetScrollSizes(int nMapMode, SIZE sizeTotal, const SIZE &sizePage, const SIZE &sizeLine)
{
	if (sizeTotal.cx == 0 && sizeTotal.cy == 0) {
		sizeTotal.cx = 1;
		sizeTotal.cy = 1;
	}

	m_nMapMode = MM_ANISOTROPIC;
	m_totalLog = sizeTotal;

	//BLOCK for DC
	{
		CWindowDC dc(NULL);
		dc.SetMapMode(m_nMapMode);

		// total size
		m_totalDev = m_totalLog;
		dc.LPtoDP((LPPOINT)&m_totalDev);
	} // Release DC

	m_szOrigTotalDev = m_totalDev;
	m_szOrigPageDev = sizePage;
	m_szOrigLineDev = sizeLine;
	if (IsScaleChanged()) {
		ReCalcBars(); // 毎回全再描画になってしまうので条件付き
	}
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

		m_pageDev = m_szOrigPageDev;
		dc.LPtoDP((LPPOINT)&m_pageDev);
		m_lineDev = m_szOrigLineDev;
		dc.LPtoDP((LPPOINT)&m_lineDev);
	} // Free DC

	if (m_pageDev.cy < 0)  m_pageDev.cy = -m_pageDev.cy;
	if (m_lineDev.cy < 0)  m_lineDev.cy = -m_lineDev.cy;

	if (m_pageDev.cx == 0) m_pageDev.cx = m_totalDev.cx / 10;
	if (m_pageDev.cy == 0) m_pageDev.cy = m_totalDev.cy / 10;
	if (m_lineDev.cx == 0) m_lineDev.cx = m_pageDev.cx / 10;
	if (m_lineDev.cy == 0) m_lineDev.cy = m_pageDev.cy / 10;

	if (m_hWnd != NULL) {
		if (IsScaleChanged()) {
			UpdateBars();
			Invalidate(TRUE);
		}
	}
}

CPoint NetView::GetLogicalCenterPt()
{
	CPoint pt;
	CRect rect;

	GetClientRect(&rect);
	pt.x = (rect.Width() / 2);
	pt.y = (rect.Height() / 2);

	DPtoLP(&pt);
	return pt;
}

void NetView::DPtoLP(LPPOINT lpPoints, int nCount)
{
	CWindowDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(lpPoints, nCount);
}

void NetView::LPtoDP(LPPOINT lpPoints, int nCount)
{
	CWindowDC dc(this);
	OnPrepareDC(&dc);
	dc.LPtoDP(lpPoints, nCount);
}

void NetView::LPtoDP(CRect& rect)
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.LPtoDP(&rect);
}


void NetView::DPtoLP(CRect &rect)
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	dc.DPtoLP(&rect);
}

void NetView::UpdateViewport(CPoint *ptNewCenter)
{
	CPoint ptCenter;

	if (!ptNewCenter)
		ptCenter = GetLogicalCenterPt();
	else
		ptCenter = *ptNewCenter;

	m_totalDev.cx = (int)((float)m_szOrigTotalDev.cx * m_fZoomScale);
	m_totalDev.cy = (int)((float)m_szOrigTotalDev.cy * m_fZoomScale);
	ReCalcBars();

	if (ptNewCenter != NULL) {
		CenterOnLogicalPoint(ptCenter);
	}
}

bool NetView::IsScaleChanged() const
{
	return (m_fZoomScale != m_fZoomScalePrev && m_fZoomScale != 1.0f);
}

BOOL NetView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_bZooming || nFlags & MK_CONTROL) {
		CPoint logPt = pt; DPtoLP(&logPt);
		if (zDelta < 0) {
			ZoomIn(/*NULL*/&pt, 0.05);
		}
		else {
			ZoomOut(/*NULL*/&pt, 0.05);
		}
	}

	if (zDelta < 0) {
		SendMessage(WM_VSCROLL, SB_PAGEDOWN);
	}
	else {
		SendMessage(WM_VSCROLL, SB_PAGEUP);
	}
	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}

void NetView::CenterOnLogicalPoint(CPoint ptCenter)
{
	LPtoDP(&ptCenter);
	CScrollView::CenterOnPoint(ptCenter);
}

void NetView::OnUpdateZoomMode(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bZooming);
}

void NetView::OnZoomMode()
{
	m_bZooming = !m_bZooming;
}

void NetView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	if (m_bZooming) {
		CPoint logPt = point; DPtoLP(&logPt);
		ZoomOut(&logPt);
	}
	CScrollView::OnRButtonDblClk(nFlags, point);
}

void NetView::OnSelchangeDropdown()
{
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
		maxPt = GetDocument()->GetMaxPt();
		CRect rc; CScrollView::GetWindowRect(&rc);
		if (rc.Width() < (int)(maxPt.x*m_fZoomScale) ||
			rc.Height() < (int)(maxPt.y*m_fZoomScale)) {

			float zoomWidth = (float)(((double)rc.Width()) / ((double)maxPt.x));
			float zoomHeight = (float)(((double)rc.Height()) / ((double)maxPt.y));
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

void NetView::ApplyColorSetting()
{
	CiEditApp* app = (CiEditApp*)AfxGetApp();
	m_bkColor = app->GetProfileInt(REGS_FRAME, _T("Net bgColor"), app->m_colorNetViewBg);
}

void NetView::OnExportSvg()
{
	CString path = GetDocument()->GetPathName();
	CString outfile;
	if (path == _T("")) {
		outfile = GetDocument()->GetTitle();
	}
	else {
		WCHAR drive[_MAX_DRIVE];
		WCHAR dir[_MAX_DIR];
		WCHAR fileName[_MAX_FNAME];
		WCHAR ext[_MAX_EXT];
		ZeroMemory(drive, _MAX_DRIVE);
		ZeroMemory(dir, _MAX_DIR);
		ZeroMemory(fileName, _MAX_FNAME);
		ZeroMemory(ext, _MAX_EXT);

		_wsplitpath_s((const wchar_t *)path, drive, _MAX_DRIVE, dir, _MAX_DIR, fileName, _MAX_FNAME, ext, _MAX_EXT);
		CString title(fileName);
		outfile = title;
	}

	WCHAR szFilters[] = _T("SVGファイル (*.svg)|*.svg");
	CFileDialog dlg(FALSE, _T("svg"), outfile, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, this);
	if (dlg.DoModal() != IDOK) return;
	CString outfileName = dlg.GetPathName();

	bool bDrwAll = false;
	GetDocument()->ExportSvg(outfileName);
	MessageBox(_T("終了しました"), _T("SVG出力"));
}

void NetView::OnUpdateExportSvg(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_bGrasp && !m_bZooming);
}

void NetView::SetMetaFileSize()
{
	m_mfWidth = AfxGetApp()->GetProfileInt(REGS_OTHER, _T("MF rWidth"), 0) / 10.0;
	m_mfHeight = AfxGetApp()->GetProfileInt(REGS_OTHER, _T("MF rHeight"), 0) / 10.0;

	if (m_mfWidth != 0 && m_mfHeight != 0) return; // レジストリの値を採用

	CSize szMF = SystemConfiguration::GetMetafileSize();
	m_mfWidth = ((double)szMF.cx) / 10.0;
	m_mfHeight = ((double)szMF.cy) / 10.0;
}

BOOL NetView::PreTranslateMessage(MSG* pMsg)
{
	m_toolTip.RelayEvent(pMsg);
	return CScrollView::PreTranslateMessage(pMsg);
}

void NetView::OnTipDispInfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTTDISPINFO lpttd;

	if (pNMHDR->code == TTN_GETDISPINFO)
	{
		if (!m_strTip.IsEmpty())
		{
			lpttd = (LPNMTTDISPINFO)pNMHDR;
			lpttd->lpszText = (LPWSTR)(LPCTSTR)m_strTip;
			*pResult = 0;
		}
	}
}


void NetView::PrepareDragRelax()
{
	GetDocument()->BackupNodesForUndo();
	GetDocument()->BackupLinksForUndo();
	// 連鎖ノードだけを動かすためのフィルタ処理
	GetDocument()->ListupChainNodes();
	GetDocument()->CalcEdges();
}


void NetView::CancelDragRelax()
{
	m_bDragRelax = false;
	m_selectRect = GetDocument()->GetSelectedNodeRect();
	GetDocument()->RecalcArea();
	GetDocument()->SetModifiedFlag();
	Invalidate();
	AdjustScrollArea();
}

void NetView::OnMButtonDown(UINT nFlags, CPoint point)
{
	CPoint logPt = point; DPtoLP(&logPt);
	m_ptPrePos = point;
	CRect r;
	if (GetDocument()->HitTest(logPt, r)) {
		m_addMode = NetView::link0;
		StartAddingLink(logPt);
	}
}

void NetView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// リンクの追加(ノード上でマウスをリリース)
	if (!m_bStartAdd) {
		CScrollView::OnRButtonUp(nFlags, point);
		return;
	}
	PointedLinkEndPosition(point);
}

void NetView::PointedLinkEndPosition(CPoint point)
{
	m_bStartAdd = false;
	CPoint logPt = point; DPtoLP(&logPt);

	if (point == m_ptPrePos) return;

	if (GetDocument()->HitTestSilently(logPt)) {
		GetDocument()->SetEndLink(logPt);
	}
	else {
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
	GetDocument()->DisableUndo();
	int shape = ((CiEditApp*)AfxGetApp())->m_rgsNode.shape;
	CRect nwRect = GetDocument()->AddNodeWithLink(shape, GetDocument()->GetSelectedNodeKey());
	if (!nwRect.IsRectEmpty()) {
		RenameNewlyLinkedNode(nwRect);
	}
}

void NetView::OnUpdateInsertChild(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::single);
}

void NetView::OnInsertSibling()
{
	GetDocument()->DisableUndo();
	int shape = ((CiEditApp*)AfxGetApp())->m_rgsNode.shape;
	CRect nwRect = GetDocument()->AddNodeWithLink2(shape, GetDocument()->GetSelectedNodeKey());
	if (!nwRect.IsRectEmpty()) {
		RenameNewlyLinkedNode(nwRect);
	}
}

void NetView::OnUpdateInsertSibling(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::single);
}

// 描画済みメタファイルをクリップボードにセットする
// メタファイルハンドルのdeleteはこのメソッド内で行う
void NetView::SendEmfToClipboard(HENHMETAFILE emf)
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

	if (m_ptPaste.x <= 0) {
		m_ptPaste.x = 70;
	}

	ClipText += _T("\n");

	int pos = 0;
	int initialX = m_ptPaste.x;
	for (CString token = ClipText.Tokenize(_T("\n"), pos); !token.IsEmpty(); ) {
		int indent = StringUtil::GetIndent(token);
		CString s = StringUtil::TrimLeft(token);
		if (s != _T("") && s != _T("\n") && s != '\r') {
			CString label;
			m_ptPaste.x = initialX + 60 * (indent - 1);
			GetDocument()->AddNodeRect(s, m_ptPaste, false);
			m_ptPaste.y += 40;
			CPoint maxPt;
			maxPt = GetDocument()->GetMaxPt();
			AdjustScrollArea();
		}
		token = ClipText.Tokenize(_T("\n"), pos);
	}
}

void NetView::OnUpdateAddNodesFromCfText(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(::IsClipboardFormatAvailable(CF_TEXT));
}

void NetView::OnSetLinkArrowNone()
{
	CString sFrom, sTo, sComment;
	int arrowType;
	GetDocument()->GetSelectedLinkInfo(sFrom, sTo, sComment, arrowType);
	GetDocument()->SetSelectedLinkInfo(sComment, iLink::line);
}

void NetView::OnUpdateSetLinkArrowNone(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::link &&
		GetDocument()->GetSelectedLink() != NULL &&
		GetDocument()->GetSelectedLink()->GetArrowStyle() != iLink::line);
}

void NetView::OnSetLinkArrowSingle()
{
	CString sFrom, sTo, sComment;
	int arrowType;
	GetDocument()->GetSelectedLinkInfo(sFrom, sTo, sComment, arrowType);
	GetDocument()->SetSelectedLinkInfo(sComment, iLink::arrow);
}

void NetView::OnUpdateSetLinkArrowSingle(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::link &&
		GetDocument()->GetSelectedLink() != NULL &&
		GetDocument()->GetSelectedLink()->GetArrowStyle() != iLink::arrow);
}

void NetView::OnSetLinkArrowDouble()
{
	CString sFrom, sTo, sComment;
	int arrowType;
	GetDocument()->GetSelectedLinkInfo(sFrom, sTo, sComment, arrowType);
	GetDocument()->SetSelectedLinkInfo(sComment, iLink::arrow2);
}

void NetView::OnUpdateSetLinkArrowDouble(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::link &&
		GetDocument()->GetSelectedLink() != NULL &&
		GetDocument()->GetSelectedLink()->GetArrowStyle() != iLink::arrow2);
}



void NetView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	HIMC hime = ::ImmGetContext(m_hWnd);
	BOOL bImeOn = ::ImmGetOpenStatus(hime);
	::ImmReleaseContext(m_hWnd, hime);
	if (isalpha(nChar) != 0 || nChar >= 0x30 && nChar <= 0x5A || nChar == VK_SPACE) {
		if (!bImeOn) {
			CString s;
			s.Format(_T("%c"), nChar);
			m_addMode = NetView::rect;
			CPoint logPt = m_ptScreen;
			logPt -= GetScrollPosition();
			DPtoLP(&logPt);
			CPoint spt(m_ptScreen);
			ClientToScreen(&spt);
			spt -= GetScrollPosition();
			// TODO:リファクタリング
			int shape = ((CiEditApp*)AfxGetApp())->m_rgsNode.shape;
			switch (shape) {
			case iNode::rectangle:
				m_addMode = NetView::rect;
				break;
			case iNode::arc:
				m_addMode = NetView::arc;
				break;
			case iNode::roundRect:
				m_addMode = NetView::rRect;
				break;
			}
			AddNode(logPt, spt, s);
			m_ptScreen += CPoint((int)(30 * m_fZoomScale), (int)(30 * m_fZoomScale));
		}
	}
	CScrollView::OnChar(nChar, nRepCnt, nFlags);
}


void NetView::OnAddlinkednodeArc()
{
	GetDocument()->DisableUndo();
	CRect nwRect = GetDocument()->AddNodeWithLink(iNode::arc, GetDocument()->GetSelectedNodeKey(), -1, m_ptNew, false);
	if (!nwRect.IsRectEmpty()) {
		RenameNewlyLinkedNode(nwRect);
	}
}

void NetView::OnUpdateAddlinkednodeArc(CCmdUI *pCmdUI)
{
}

void NetView::OnAddlinkednodeRect()
{
	GetDocument()->DisableUndo();
	CRect nwRect = GetDocument()->AddNodeWithLink(iNode::rectangle, GetDocument()->GetSelectedNodeKey(), -1, m_ptNew, false);
	if (!nwRect.IsRectEmpty()) {
		RenameNewlyLinkedNode(nwRect);

	}
}

void NetView::OnUpdateAddlinkednodeRect(CCmdUI *pCmdUI)
{
}

void NetView::OnAddlinkednodeRndrect()
{
	GetDocument()->DisableUndo();
	CRect nwRect = GetDocument()->AddNodeWithLink(iNode::roundRect, GetDocument()->GetSelectedNodeKey(), -1, m_ptNew, false);
	if (!nwRect.IsRectEmpty()) {
		RenameNewlyLinkedNode(nwRect);
	}
}

void NetView::OnUpdateAddlinkednodeRndrect(CCmdUI *pCmdUI)
{
}

void NetView::RenameNewlyLinkedNode(const CRect& nodeBound)
{
	GetDocument()->RecalcArea();
	AdjustScrollArea();
	Invalidate();
	CreateNodeDlg dlg;

	CPoint spt(nodeBound.CenterPoint());
	LPtoDP(&spt, 1);
	ClientToScreen(&spt);
	dlg.m_initialPt = spt;
	dlg.m_strcn = GetDocument()->GetSelectedNodeLabel();

	if (dlg.DoModal() != IDOK) return;
	if (dlg.m_strcn == _T("")) return;
	GetDocument()->SetSelectedNodeLabel(dlg.m_strcn);
}


void NetView::OnAddLabelOnly()
{
	if (m_addMode != NetView::label) {
		m_addMode = NetView::label;
	}
	else if (m_addMode == NetView::label) {
		m_addMode = NetView::normal;
	}
}

void NetView::OnUpdateAddLabelOnly(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bGrasp && !m_bZooming);
	pCmdUI->SetCheck(m_addMode == NetView::label);
}

void NetView::OnFontEnlarge()
{
	GetDocument()->ResizeSelectedNodeFont(true);
	GetDocument()->ResizeSelectedLinkFont(true);
}

void NetView::OnUpdateFontEnlarge(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_selectStatus != NetView::none);
}

void NetView::OnFontEnsmall()
{
	GetDocument()->ResizeSelectedNodeFont(false);
	GetDocument()->ResizeSelectedLinkFont(false);
}


void NetView::OnUpdateFontEnsmall(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_selectStatus != NetView::none);
}

void NetView::OnSetLinkDependSingle()
{
	CString sFrom, sTo, sComment;
	int arrowType;
	GetDocument()->GetSelectedLinkInfo(sFrom, sTo, sComment, arrowType);
	GetDocument()->SetSelectedLinkInfo(sComment, iLink::depend);
}

void NetView::OnUpdateSetLinkDependSingle(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::link &&
		GetDocument()->GetSelectedLink() != NULL &&
		GetDocument()->GetSelectedLink()->GetArrowStyle() != iLink::depend);
}

void NetView::OnSetLinkDependDouble()
{
	CString sFrom, sTo, sComment;
	int arrowType;
	GetDocument()->GetSelectedLinkInfo(sFrom, sTo, sComment, arrowType);
	GetDocument()->SetSelectedLinkInfo(sComment, iLink::depend2);
}

void NetView::OnUpdateSetLinkDependDouble(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::link &&
		GetDocument()->GetSelectedLink() != NULL &&
		GetDocument()->GetSelectedLink()->GetArrowStyle() != iLink::depend2);
}

void NetView::OnSetLinkInherit()
{
	CString sFrom, sTo, sComment;
	int arrowType;
	GetDocument()->GetSelectedLinkInfo(sFrom, sTo, sComment, arrowType);
	GetDocument()->SetSelectedLinkInfo(sComment, iLink::inherit);
}


void NetView::OnUpdateSetLinkInherit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::link &&
		GetDocument()->GetSelectedLink() != NULL &&
		GetDocument()->GetSelectedLink()->GetArrowStyle() != iLink::inherit);
}

void NetView::OnSetLinkAgregat()
{
	CString sFrom, sTo, sComment;
	int arrowType;
	GetDocument()->GetSelectedLinkInfo(sFrom, sTo, sComment, arrowType);
	GetDocument()->SetSelectedLinkInfo(sComment, iLink::aggregat);
}

void NetView::OnUpdateSetLinkAgregat(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::link &&
		GetDocument()->GetSelectedLink() != NULL &&
		GetDocument()->GetSelectedLink()->GetArrowStyle() != iLink::aggregat);
}

void NetView::OnSetLinkComposit()
{
	CString sFrom, sTo, sComment;
	int arrowType;
	GetDocument()->GetSelectedLinkInfo(sFrom, sTo, sComment, arrowType);
	GetDocument()->SetSelectedLinkInfo(sComment, iLink::composit);
}

void NetView::OnUpdateSetLinkComposit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::link &&
		GetDocument()->GetSelectedLink() != NULL &&
		GetDocument()->GetSelectedLink()->GetArrowStyle() != iLink::composit);
}

void NetView::ChangeSelectedLinkArrow()
{
	if (m_selectStatus == NetView::link) {
		CString sFrom, sTo, sComment;
		int arrowType;
		GetDocument()->GetSelectedLinkInfo(sFrom, sTo, sComment, arrowType);
		GetDocument()->SetSelectedLinkInfo(sComment, GetDocument()->GetAppLinkArrow());
	}
}

void NetView::ChangeSelectedLineWidth()
{
	int style = GetDocument()->GetAppLinkWidth();
	GetDocument()->BackupNodesForUndo();
	GetDocument()->BackupLinksForUndo();
	if (m_selectStatus == NetView::link) {
		if (style == -1) {
			GetDocument()->SetSelectedLinkWidth(0);
			GetDocument()->SetSelectedLinkLineStyle(PS_DOT);
		}
		else {
			GetDocument()->SetSelectedLinkWidth(style);
			GetDocument()->SetSelectedLinkLineStyle(PS_SOLID);
		}
	}
	else if (m_selectStatus == NetView::single ||
		m_selectStatus == NetView::multi) {
		if (style == -1) {
			GetDocument()->SetSelectedNodeLineWidth(0);
			GetDocument()->SetSelectedNodeLineStyle(PS_DOT);
		}
		else {
			GetDocument()->SetSelectedNodeLineWidth(style);
			GetDocument()->SetSelectedNodeLineStyle(PS_SOLID);
		}
	}
}

void NetView::OnDrawOrderInfo()
{
	GetDocument()->SetDrawOrderInfo(!GetDocument()->DrawOrderInfo());
	Invalidate();
}

void NetView::OnUpdateDrawOrderInfo(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(((CiEditApp*)AfxGetApp())->m_rgsNode.bSyncOrder);
	pCmdUI->SetCheck(GetDocument()->DrawOrderInfo());
}

void NetView::OnSetNodeMm()
{
	GetDocument()->BackupNodesForUndo();
	GetDocument()->SetSelectedNodeShape(iNode::MindMapNode);
}

void NetView::OnUpdateSetNodeMm(CCmdUI *pCmdUI)
{
	int shape = GetDocument()->GetSelectedNodeShape();
	pCmdUI->Enable(shape != iNode::MindMapNode && shape != iNode::MetaFile);
}

void NetView::OnValignTop()
{
	int st = GetDocument()->GetSelectedNodeTextStyle();
	int stNew = st;
	if (st == iNode::s_bc) stNew = iNode::s_tc;
	if (st == iNode::s_bl) stNew = iNode::s_tl;
	if (st == iNode::s_br) stNew = iNode::s_tr;
	if (st == iNode::s_cc) stNew = iNode::s_tc;
	if (st == iNode::s_cl) stNew = iNode::s_tl;
	if (st == iNode::s_cr) stNew = iNode::s_tr;
	GetDocument()->BackupNodesForUndo();
	GetDocument()->SetSelectedNodeTextStyle(stNew);
}

void NetView::OnUpdateValignTop(CCmdUI *pCmdUI)
{
	int st = GetDocument()->GetSelectedNodeTextStyle();
	pCmdUI->Enable(m_selectStatus == NetView::single &&
		st != iNode::s_tc && st != iNode::s_tl && st != iNode::s_tr &&
		st != iNode::m_c && st != iNode::m_l && st != iNode::m_r);
}

void NetView::OnAlignBottom()
{
	int st = GetDocument()->GetSelectedNodeTextStyle();
	int stNew = st;
	if (st == iNode::s_tc) stNew = iNode::s_bc;
	if (st == iNode::s_tl) stNew = iNode::s_bl;
	if (st == iNode::s_tr) stNew = iNode::s_br;
	if (st == iNode::s_cc) stNew = iNode::s_bc;
	if (st == iNode::s_cl) stNew = iNode::s_bl;
	if (st == iNode::s_cr) stNew = iNode::s_br;
	GetDocument()->BackupNodesForUndo();
	GetDocument()->SetSelectedNodeTextStyle(stNew);
}

void NetView::OnUpdateAlignBottom(CCmdUI *pCmdUI)
{
	int st = GetDocument()->GetSelectedNodeTextStyle();
	pCmdUI->Enable(m_selectStatus == NetView::single &&
		st != iNode::s_bc && st != iNode::s_bl && st != iNode::s_br &&
		st != iNode::m_c && st != iNode::m_l && st != iNode::m_r);
}

void NetView::OnHalignRight()
{
	int st = GetDocument()->GetSelectedNodeTextStyle();
	int stNew = st;
	if (st == iNode::s_tc) stNew = iNode::s_tr;
	if (st == iNode::s_tl) stNew = iNode::s_tr;
	if (st == iNode::s_cc) stNew = iNode::s_cr;
	if (st == iNode::s_cl) stNew = iNode::s_cr;
	if (st == iNode::s_bc) stNew = iNode::s_br;
	if (st == iNode::s_bl) stNew = iNode::s_br;
	if (st == iNode::m_c) stNew = iNode::m_r;
	if (st == iNode::m_l) stNew = iNode::m_r;
	GetDocument()->BackupNodesForUndo();
	GetDocument()->SetSelectedNodeTextStyle(stNew);
}

void NetView::OnUpdateHalignRight(CCmdUI *pCmdUI)
{
	int st = GetDocument()->GetSelectedNodeTextStyle();
	pCmdUI->Enable(m_selectStatus == NetView::single &&
		st != iNode::s_tr && st != iNode::s_cr && st != iNode::s_br && st != iNode::m_r);
}


void NetView::OnHalignLeft()
{
	int st = GetDocument()->GetSelectedNodeTextStyle();
	int stNew = st;
	if (st == iNode::s_tc) stNew = iNode::s_tl;
	if (st == iNode::s_tr) stNew = iNode::s_tl;
	if (st == iNode::s_cc) stNew = iNode::s_cl;
	if (st == iNode::s_cr) stNew = iNode::s_cl;
	if (st == iNode::s_bc) stNew = iNode::s_bl;
	if (st == iNode::s_br) stNew = iNode::s_bl;
	if (st == iNode::m_c) stNew = iNode::m_l;
	if (st == iNode::m_r) stNew = iNode::m_l;
	GetDocument()->BackupNodesForUndo();
	GetDocument()->SetSelectedNodeTextStyle(stNew);
}

void NetView::OnUpdateHalignLeft(CCmdUI *pCmdUI)
{
	int st = GetDocument()->GetSelectedNodeTextStyle();
	pCmdUI->Enable(m_selectStatus == NetView::single &&
		st != iNode::s_tl && st != iNode::s_cl && st != iNode::s_bl && st != iNode::m_l);
}

void NetView::OnValignCenter()
{
	int st = GetDocument()->GetSelectedNodeTextStyle();
	int stNew = st;
	if (st == iNode::s_bc) stNew = iNode::s_cc;
	if (st == iNode::s_bl) stNew = iNode::s_cl;
	if (st == iNode::s_br) stNew = iNode::s_cr;
	if (st == iNode::s_tc) stNew = iNode::s_cc;
	if (st == iNode::s_tl) stNew = iNode::s_cl;
	if (st == iNode::s_tr) stNew = iNode::s_cr;
	GetDocument()->BackupNodesForUndo();
	GetDocument()->SetSelectedNodeTextStyle(stNew);
}

void NetView::OnUpdateValignCenter(CCmdUI *pCmdUI)
{
	int st = GetDocument()->GetSelectedNodeTextStyle();
	pCmdUI->Enable(m_selectStatus == NetView::single &&
		st != iNode::s_cc && st != iNode::s_cl && st != iNode::s_cr &&
		st != iNode::m_c && st != iNode::m_l && st != iNode::m_r);
}

void NetView::OnHalignCenter()
{
	int st = GetDocument()->GetSelectedNodeTextStyle();
	int stNew = st;
	if (st == iNode::s_tr) stNew = iNode::s_tc;
	if (st == iNode::s_tl) stNew = iNode::s_tc;
	if (st == iNode::s_cr) stNew = iNode::s_cc;
	if (st == iNode::s_cl) stNew = iNode::s_cc;
	if (st == iNode::s_br) stNew = iNode::s_bc;
	if (st == iNode::s_bl) stNew = iNode::s_bc;
	if (st == iNode::m_r) stNew = iNode::m_c;
	if (st == iNode::m_l) stNew = iNode::m_c;
	GetDocument()->BackupNodesForUndo();
	GetDocument()->SetSelectedNodeTextStyle(stNew);
}

void NetView::OnUpdateHalignCenter(CCmdUI *pCmdUI)
{
	int st = GetDocument()->GetSelectedNodeTextStyle();
	pCmdUI->Enable(m_selectStatus == NetView::single &&
		st != iNode::s_tc && st != iNode::s_cc && st != iNode::s_bc && st != iNode::m_c);
}

void NetView::OnBtnNodeFillColor()
{
	GetDocument()->BackupNodesForUndo();
	GetDocument()->SetSelectedNodeBrush(((CiEditApp*)AfxGetApp())->m_colorNodeBtn);
}

void NetView::OnUpdateBtnNodeFillColor(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::single || m_selectStatus == NetView::multi);
}

void NetView::OnBtnLineColor()
{
	GetDocument()->BackupNodesForUndo();
	GetDocument()->BackupLinksForUndo();
	GetDocument()->SetSelectedNodeLineColor(((CiEditApp*)AfxGetApp())->m_colorLineBtn);
	GetDocument()->SetSelectedLinkLineColor(((CiEditApp*)AfxGetApp())->m_colorLineBtn);
}

void NetView::OnUpdateBtnLineColor(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_selectStatus != NetView::none);
}

void NetView::OnBtnTextColor()
{
	GetDocument()->BackupNodesForUndo();
	GetDocument()->SetSelectedNodeFontColor(((CiEditApp*)AfxGetApp())->m_colorFontBtn);
}

void NetView::OnUpdateBtnTextColor(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::single || m_selectStatus == NetView::multi);
}

void NetView::ChangeSelectedNodeColor()
{
	GetDocument()->SetSelectedNodeBrush(((CiEditApp*)AfxGetApp())->m_colorNodeBtn);
}

void NetView::ChangeSelectedFontColor()
{
	GetDocument()->BackupNodesForUndo();
	GetDocument()->SetSelectedNodeFontColor(((CiEditApp*)AfxGetApp())->m_colorFontBtn);
}

void NetView::ChangeSelectedLineColor()
{
	GetDocument()->BackupNodesForUndo();
	GetDocument()->BackupLinksForUndo();
	GetDocument()->SetSelectedNodeLineColor(((CiEditApp*)AfxGetApp())->m_colorLineBtn);
	GetDocument()->SetSelectedLinkLineColor(((CiEditApp*)AfxGetApp())->m_colorLineBtn);
}
void NetView::OnBtnLinkArrow()
{
	ChangeSelectedLinkArrow();
}

void NetView::OnUpdateBtnLinkArrow(CCmdUI *pCmdUI)
{
}

void NetView::OnBtnLinkLineStyle()
{
	this->ChangeSelectedLineWidth();
}

void NetView::OnUpdateBtnLinkLineStyle(CCmdUI *pCmdUI)
{
}

void NetView::OnSaveFormat()
{
	if (m_selectStatus == NetView::single) {
		GetDocument()->SaveSelectedNodeFormat();
		m_bFormCopied = TRUE;
	}
	else if (m_selectStatus == NetView::link) {
		GetDocument()->SaveSelectedLinkFormat();
		m_bFormCopied = TRUE;
	}
}

void NetView::OnUpdateSaveFormat(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::single || m_selectStatus == NetView::link);
}

void NetView::OnAplyFormat()
{
	m_bAplyForm = !m_bAplyForm;
}

void NetView::OnUpdateAplyFormat(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_bFormCopied);
	pCmdUI->SetCheck(m_bAplyForm);
}

void NetView::ApplyFormat(CPoint& pt)
{
	CRect r;
	if (GetDocument()->HitTestLinks(pt)) {
		GetDocument()->ApplyFormatToSelectedLink();
	}
	else if (GetDocument()->HitTest(pt, r)) {
		GetDocument()->ApplyFormatToSelectedNode();
	}
}

void NetView::OnGrpOlCoupled()
{
	m_bGrpOlCoupled = !m_bGrpOlCoupled;
}

void NetView::OnUpdateGrpOlCoupled(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(GetDocument()->ShowSubBranch());
	pCmdUI->SetCheck(m_bGrpOlCoupled);
}

void NetView::OnDeleteSelectedNodes()
{
	DeleteSelectedNodes();
}

void NetView::OnUpdateDeleteSelectedNodes(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::multi);
}

void NetView::OnDeleteSelectedLinks()
{
	if (MessageBox(_T("選択範囲にあるリンクを削除しますか?"),
		_T("選択範囲のリンクを削除"), MB_YESNO) != IDYES) return;
	GetDocument()->DeleteLinksInBound(m_selectRect);
}

void NetView::OnUpdateDeleteSelectedLinks(CCmdUI *pCmdUI)
{
	CRect rc = GetDocument()->GetSelectedLinkBound();
	pCmdUI->Enable(m_selectStatus == NetView::multi && !rc.IsRectEmpty());
}

void NetView::OnSetLinkAngled()
{
	const iLink* pLink = GetDocument()->GetSelectedLink();
	if (pLink == NULL) return;
	GetDocument()->AngleSelectedLink(!pLink->IsAngled());
}

void NetView::OnUpdateSetLinkAngled(CCmdUI *pCmdUI)
{
	const iLink* pLink = GetDocument()->GetSelectedLink();
	pCmdUI->Enable(!GetDocument()->IsOldBinary() && pLink != NULL && pLink->IsCurved());
	pCmdUI->SetCheck(pLink != NULL && pLink->IsAngled());
}

void NetView::OnExportEmf()
{
	CString path = GetDocument()->GetPathName();
	CString outfile;
	if (path == _T("")) {
		outfile = GetDocument()->GetTitle();
	}
	else {
		WCHAR drive[_MAX_DRIVE];
		WCHAR dir[_MAX_DIR];
		WCHAR fileName[_MAX_FNAME];
		WCHAR ext[_MAX_EXT];
		ZeroMemory(drive, _MAX_DRIVE);
		ZeroMemory(dir, _MAX_DIR);
		ZeroMemory(fileName, _MAX_FNAME);
		ZeroMemory(ext, _MAX_EXT);

		_wsplitpath_s((const wchar_t *)path, drive, _MAX_DRIVE, dir, _MAX_DIR, fileName, _MAX_FNAME, ext, _MAX_EXT);
		CString title(fileName);
		outfile = title;
	}

	WCHAR szFilters[] = _T("EMFファイル (*.emf)|*.emf");
	CFileDialog dlg(FALSE, _T("emf"), outfile, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, this);
	if (dlg.DoModal() != IDOK) return;
	CString outfileName = dlg.GetPathName();

	CPoint p1(0, 0);
	CPoint p2 = GetDocument()->GetMaxPt();

	CMetaFileDC* pMfDC = new CMetaFileDC();
	CRect rc(0, 0, (int)(abs(p2.x - p1.x)*m_mfWidth), (int)(abs(p2.y - p1.y)*m_mfHeight));
	CClientDC dc(this);
	pMfDC->CreateEnhanced(&dc, outfileName, &rc, _T("iEdit"));
	pMfDC->SetAttribDC(dc);

	GetDocument()->DrawNodes(pMfDC);
	GetDocument()->DrawLinks(pMfDC, true);
	HENHMETAFILE hmetafile = pMfDC->CloseEnhanced();
	delete pMfDC;
}

void NetView::OnUpdateExportEmf(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bGrasp && !m_bZooming);
}

void NetView::OnExportPng()
{
	CString path = GetDocument()->GetPathName();
	CString outfile;
	if (path == _T("")) {
		outfile = GetDocument()->GetTitle();
	}
	else {
		WCHAR drive[_MAX_DRIVE];
		WCHAR dir[_MAX_DIR];
		WCHAR fileName[_MAX_FNAME];
		WCHAR ext[_MAX_EXT];
		ZeroMemory(drive, _MAX_DRIVE);
		ZeroMemory(dir, _MAX_DIR);
		ZeroMemory(fileName, _MAX_FNAME);
		ZeroMemory(ext, _MAX_EXT);

		_wsplitpath_s((const wchar_t *)path, drive, _MAX_DRIVE, dir, _MAX_DIR, fileName, _MAX_FNAME, ext, _MAX_EXT);
		CString title(fileName);
		outfile = title;
	}

	WCHAR szFilters[] = _T("PNGファイル (*.png)|*.png");
	CFileDialog dlg(FALSE, _T("png"), outfile, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, this);
	if (dlg.DoModal() != IDOK) return;
	CString outfileName = dlg.GetPathName();
	GetDocument()->SaveCurrentImage(outfileName);
}

void NetView::OnUpdateExportPng(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!m_bGrasp && !m_bZooming);
}

void NetView::OnSetMargin()
{
	NodeMarginSettingsDlg dlg;
	int l, r, t, b;
	if (m_selectStatus == NetView::single) {
		GetDocument()->GetSelectedNodeMargin(l, r, t, b);
		dlg.m_nLeft = l;
		dlg.m_nRight = r;
		dlg.m_nTop = t;
		dlg.m_nBottom = b;
	}
	if (dlg.DoModal() != IDOK) return;
	l = dlg.m_nLeft;
	r = dlg.m_nRight;
	t = dlg.m_nTop;
	b = dlg.m_nBottom;
	GetDocument()->SetSelectedNodeMargin(l, r, t, b);
}

void NetView::OnUpdateSetMargin(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(
		!GetDocument()->IsOldBinary() &&
		(m_selectStatus == NetView::single || m_selectStatus == NetView::multi));
}

void NetView::OnResizeTofit()
{
	GetDocument()->FitSelectedNodeSize();
}

void NetView::OnUpdateResizeTofit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_selectStatus == NetView::single || m_selectStatus == NetView::multi);
}

void NetView::OnReplaceMetafile()
{
	CPoint ptDrop = GetDocument()->GetSelectedNodeRect().TopLeft();

	if (!OpenClipboard()) {
		::showLastErrorMessage();
		return;
	}
	HENHMETAFILE hm = NULL;
	if (::IsClipboardFormatAvailable(CF_ENHMETAFILE)) {
		hm = (HENHMETAFILE)::GetClipboardData(CF_ENHMETAFILE);
	}
	else if (::IsClipboardFormatAvailable(CF_DIB) || ::IsClipboardFormatAvailable(CF_BITMAP)) {
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
	}
	if (hm != NULL) {
		GetDocument()->SetSelectedNodeMetaFile(hm);
	}
	if (!EmptyClipboard()) {
		AfxMessageBox(_T("Cannot empty the Clipboard"));
		return;
	}
	CloseClipboard();
}

void NetView::OnUpdateReplaceMetafile(CCmdUI *pCmdUI)
{
	BOOL canAvailable = ::IsClipboardFormatAvailable(CF_ENHMETAFILE) ||
		::IsClipboardFormatAvailable(CF_DIB) ||
		::IsClipboardFormatAvailable(CF_BITMAP);

	pCmdUI->Enable(m_selectStatus == NetView::single && canAvailable && !GetDocument()->CanDuplicateNodes());
}


BOOL NetView::OnTtnNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_strTip == _T("")) return FALSE;
	UNREFERENCED_PARAMETER(id);

	TOOLTIPTEXT *pTTT = (TOOLTIPTEXT *)pNMHDR;
	UINT_PTR nID = pNMHDR->idFrom;
	BOOL bRet = FALSE;

	if (pTTT->uFlags & TTF_IDISHWND)
	{
		nID = ::GetDlgCtrlID((HWND)nID);
		if (nID)
		{
			pTTT->lpszText = (LPWSTR)(LPCTSTR)m_strTip;
			pTTT->hinst = AfxGetResourceHandle();
			bRet = TRUE;
		}
	}

	*pResult = 0;

	return bRet;
}
