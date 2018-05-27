// MainFrm.cpp : CMainFrame クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "iEdit.h"

#include "MainFrm.h"
#include "Splash.h"

#include "OptionSettingsDlg.h"
#include "NodeSearchDlg.h"
#include "WndTransparencySettingsDlg.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGS_FRAME _T("Frame Options")

#define IDX_TB_LINE_STYLE 17
#define IDX_TB_ARROW 18
#define IDX_TB_ZOOMCOMBO 24

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_SET_PROPERTIES, OnSetProperties)
	ON_UPDATE_COMMAND_UI(ID_SET_PROPERTIES, OnUpdateSetProperties)
	ON_WM_CLOSE()
	ON_WM_DROPFILES()
	ON_COMMAND(ID_ADJUST_ALPHA, OnAdjustAlpha)
	ON_UPDATE_COMMAND_UI(ID_ADJUST_ALPHA, OnUpdateAdjustAlpha)
	ON_WM_DESTROY()
	ON_COMMAND(ID_TRANSPARENT_MODE, OnTransparentMode)
	ON_UPDATE_COMMAND_UI(ID_TRANSPARENT_MODE, OnUpdateTransparentMode)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SLIDEALPHA, OnSlideAlpha)
	ON_COMMAND(ID_MNU_LS_R0, &CMainFrame::OnMnuLsR0)
	ON_UPDATE_COMMAND_UI(ID_MNU_LS_R0, &CMainFrame::OnUpdateMnuLsR0)
	ON_COMMAND(ID_MNU_LS_DOT, &CMainFrame::OnMnuLsDot)
	ON_UPDATE_COMMAND_UI(ID_MNU_LS_DOT, &CMainFrame::OnUpdateMnuLsDot)
	ON_COMMAND(ID_MNU_LS_R1, &CMainFrame::OnMnuLsR1)
	ON_UPDATE_COMMAND_UI(ID_MNU_LS_R1, &CMainFrame::OnUpdateMnuLsR1)
	ON_COMMAND(ID_MNU_LS_R2, &CMainFrame::OnMnuLsR2)
	ON_UPDATE_COMMAND_UI(ID_MNU_LS_R2, &CMainFrame::OnUpdateMnuLsR2)
	ON_COMMAND(ID_MNU_LS_R3, &CMainFrame::OnMnuLsR3)
	ON_UPDATE_COMMAND_UI(ID_MNU_LS_R3, &CMainFrame::OnUpdateMnuLsR3)
	ON_COMMAND(ID_MNU_LS_R4, &CMainFrame::OnMnuLsR4)
	ON_UPDATE_COMMAND_UI(ID_MNU_LS_R4, &CMainFrame::OnUpdateMnuLsR4)
	ON_COMMAND(ID_LINK_ARROW_NONE, &CMainFrame::OnLinkArrowNone)
	ON_UPDATE_COMMAND_UI(ID_LINK_ARROW_NONE, &CMainFrame::OnUpdateLinkArrowNone)
	ON_COMMAND(ID_LINK_ARROW_SINGLE, &CMainFrame::OnLinkArrowSingle)
	ON_UPDATE_COMMAND_UI(ID_LINK_ARROW_SINGLE, &CMainFrame::OnUpdateLinkArrowSingle)
	ON_COMMAND(ID_LINK_ARROW_DOUBLE, &CMainFrame::OnLinkArrowDouble)
	ON_UPDATE_COMMAND_UI(ID_LINK_ARROW_DOUBLE, &CMainFrame::OnUpdateLinkArrowDouble)
	ON_COMMAND(ID_LINK_DEPEND_SINGLE, &CMainFrame::OnLinkDependSingle)
	ON_UPDATE_COMMAND_UI(ID_LINK_DEPEND_SINGLE, &CMainFrame::OnUpdateLinkDependSingle)
	ON_COMMAND(ID_LINK_DEPEND_DOUBLE, &CMainFrame::OnLinkDependDouble)
	ON_UPDATE_COMMAND_UI(ID_LINK_DEPEND_DOUBLE, &CMainFrame::OnUpdateLinkDependDouble)
	ON_COMMAND(ID_LINK_INHERIT, &CMainFrame::OnLinkArrowInherit)
	ON_UPDATE_COMMAND_UI(ID_LINK_INHERIT, &CMainFrame::OnUpdateLinkArrowInherit)
	ON_COMMAND(ID_LINK_AGREGAT, &CMainFrame::OnLinkArrowAgregat)
	ON_UPDATE_COMMAND_UI(ID_LINK_AGREGAT, &CMainFrame::OnUpdateLinkArrowAgregat)
	ON_COMMAND(ID_LINK_COMPOSIT, &CMainFrame::OnLinkArrowComposit)
	ON_UPDATE_COMMAND_UI(ID_LINK_COMPOSIT, &CMainFrame::OnUpdateLinkArrowComposit)
	ON_COMMAND(ID_VIEW_FORM_BAR, &CMainFrame::OnViewFormBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FORM_BAR, &CMainFrame::OnUpdateViewFormBar)
	ON_COMMAND(ID_SELECT_NODE_COLOR, &CMainFrame::OnSelectNodeColor)
	ON_COMMAND(ID_SELECT_LINE_COLOR, &CMainFrame::OnSelectLineColor)
	ON_COMMAND(ID_SELECT_FONT_COLOR, &CMainFrame::OnSelectFontColor)
	ON_COMMAND(ID_APP_HELP, &CMainFrame::OnAppHelp)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,
	ID_INDICATOR_KANA,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame クラスの構築/消滅

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (m_bTransparent) {
		MakeWindowTransparent();
	}
		
	/////////// ToolBar
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0(_T("Failed to create toolbar\n"));
		return -1;
	}
	
	if (!m_wndFormPalette.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(0,0,0,0), IDR_FORM_PALLETE) ||
		!m_wndFormPalette.LoadToolBar(IDR_FORM_PALLETE))
	{
		TRACE0(_T("Failed to create form pallete\n"));
		return -1;
	}
	
	////////////// StatusBar
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0(_T("Failed to create status bar\n"));
		return -1;
	}
	
	/////// ドロップダウンスタイルボタンの設定
	m_wndToolBar.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
	TBBUTTONINFO tbButtonInfo;
	tbButtonInfo.dwMask= TBIF_STYLE;
	tbButtonInfo.cbSize= sizeof(TBBUTTONINFO);
	// リンク線の種類
	m_wndToolBar.GetToolBarCtrl().GetButtonInfo(ID_BTN_LINK_LINE_STYLE, &tbButtonInfo);
	tbButtonInfo.fsStyle |= TBSTYLE_DROPDOWN;
	m_wndToolBar.GetToolBarCtrl().SetButtonInfo(ID_BTN_LINK_LINE_STYLE, &tbButtonInfo);
	// リンク矢印のつき方
	m_wndToolBar.GetToolBarCtrl().GetButtonInfo(ID_BTN_LINK_ARROW, &tbButtonInfo);
	tbButtonInfo.fsStyle |= TBSTYLE_DROPDOWN;
	m_wndToolBar.GetToolBarCtrl().SetButtonInfo(ID_BTN_LINK_ARROW, &tbButtonInfo);
	
	m_wndToolBar.SetWindowText(_T("編集"));
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	// ズーム変更用コンボボックスの追加
	addComboZoom();
	
	m_wndFormPalette.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);
	// ノード色ボタン
	m_wndFormPalette.GetToolBarCtrl().GetButtonInfo(ID_BTN_NODE_FILL_COLOR, &tbButtonInfo);
	tbButtonInfo.fsStyle |= TBSTYLE_DROPDOWN;
	m_wndFormPalette.GetToolBarCtrl().SetButtonInfo(ID_BTN_NODE_FILL_COLOR, &tbButtonInfo);
	// ライン色ボタン
	m_wndFormPalette.GetToolBarCtrl().GetButtonInfo(ID_BTN_LINE_COLOR, &tbButtonInfo);
	tbButtonInfo.fsStyle |= TBSTYLE_DROPDOWN;
	m_wndFormPalette.GetToolBarCtrl().SetButtonInfo(ID_BTN_LINE_COLOR, &tbButtonInfo);
	// 文字色ボタン
	m_wndFormPalette.GetToolBarCtrl().GetButtonInfo(ID_BTN_TEXT_COLOR, &tbButtonInfo);
	tbButtonInfo.fsStyle |= TBSTYLE_DROPDOWN;
	m_wndFormPalette.GetToolBarCtrl().SetButtonInfo(ID_BTN_TEXT_COLOR, &tbButtonInfo);

	m_wndFormPalette.SetWindowText(_T("書式"));
	m_wndFormPalette.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	JointCBLine(this,&m_wndFormPalette,&m_wndToolBar);
	
	
	BOOL saveBar = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Save bar status"), TRUE);
	if (saveBar) {
		LoadBarState(_T("BarSettings"));
	}
	
	CSplashWnd::ShowSplashScreen(this);
	
	BOOL saveFrame = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Save Frame Sizes"), TRUE);
	if (saveFrame) {
		loadFramePosition();
	}	
	
	m_pSetAlphaDlg = NULL;
	m_tbPlDwnImage.Create(IDB_TB_PLDWN, 16, 5, RGB(255, 0, 255));
	
	((CiEditApp*)AfxGetApp())->m_colorNodeBtn = RGB(255, 255, 255);
	((CiEditApp*)AfxGetApp())->m_colorLineBtn = RGB(0, 0, 0);
	((CiEditApp*)AfxGetApp())->m_colorFontBtn = RGB(0, 0, 0);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame クラスの診断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame メッセージ ハンドラ


void CMainFrame::OnSetProperties() 
{
	OptionSettingsDlg dlg;
	if (dlg.DoModal() == IDOK) {
		CiEditApp* pApp = (CiEditApp*)AfxGetApp();
		pApp->notifyViewSetting();
	}
}

void CMainFrame::OnUpdateSetProperties(CCmdUI* pCmdUI) 
{
}

void CMainFrame::OnClose() 
{
	BOOL saveBar = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Save bar status"), TRUE);
	if (saveBar) {
		SaveBarState(_T("BarSettings"));
	}	
	
	BOOL saveFrame = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Save Frame Sizes"), TRUE);
	if (saveFrame) {
		saveFramePosition();
	}
	
	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnDropFiles(HDROP hDropInfo) 
{
	WCHAR fileName[MAX_PATH];
	DragQueryFile(hDropInfo, 0, fileName, MAX_PATH);
	
	WCHAR drive[_MAX_DRIVE];
	WCHAR dir[_MAX_DIR];
	WCHAR fname[_MAX_FNAME];
	WCHAR ext[_MAX_EXT];
	ZeroMemory(drive, _MAX_DRIVE);
	ZeroMemory(dir, _MAX_DIR);
	ZeroMemory(fname, _MAX_FNAME);
	ZeroMemory(ext, _MAX_EXT);
	
	_wsplitpath_s((const wchar_t *)fileName, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
	
	CString extent = ext;
	extent.MakeLower();
	if (extent != _T(".iedx") && extent != _T(".ied") && extent != _T(".xml")) {
		AfxMessageBox(_T("iEditファイルではありません"));
		return;
	}
	
	CMDIFrameWnd::OnDropFiles(hDropInfo);
}

void CMainFrame::saveFramePosition()
{
	WINDOWPLACEMENT wndpl;
	GetWindowPlacement(&wndpl);
	if (wndpl.showCmd == SW_SHOWNORMAL || wndpl.showCmd == SW_SHOWMAXIMIZED) {
		CRect rc(&wndpl.rcNormalPosition);
		if (rc.left >= -10 && rc.top >= -10 && rc.Width() < 10000 && rc.Height() < 10000) {
			AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Window Left"), rc.left);
			AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Window Top"), rc.top);
			AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Window Width"), rc.Width());
			AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Window Height"), rc.Height());
		}
	}
}

void CMainFrame::loadFramePosition()
{
	int left = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Window Left"), 0);
	int top = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Window Top"), 0);
	int width = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Window Width"), 640);
	int height = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Window Height"), 480);
	MoveWindow(left, top, width, height);
}

CComboBox* CMainFrame::getZoomComboBox()
{
	return &m_comboZoom;
}

void CMainFrame::addComboZoom()
{
	CRect rcCombo(-75, -300, 0, 0);
	DWORD dwStyle = CBS_DROPDOWNLIST;
	dwStyle |= WS_CHILD|WS_VISIBLE;
	m_comboZoom.Create(dwStyle,rcCombo,&m_wndToolBar, ID_DROPDOWN);
	m_fntComboZoom.CreatePointFont(100,_T("ＭＳ ゴシック"));
	m_comboZoom.SetFont(&m_fntComboZoom);
	m_wndToolBar.SetButtonInfo(IDX_TB_ZOOMCOMBO, ID_DROPDOWN, TBBS_SEPARATOR,75);
	m_wndToolBar.GetItemRect(IDX_TB_ZOOMCOMBO, &rcCombo);
	m_comboZoom.SetWindowPos(NULL, rcCombo.left,rcCombo.top+1,0,0,
		SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOCOPYBITS);
	m_comboZoom.ModifyStyleEx(0,WS_EX_CLIENTEDGE);
	
	m_comboZoom.AddString(_T("10%"));
	m_comboZoom.AddString(_T("25%"));
	m_comboZoom.AddString(_T("50%"));
	m_comboZoom.AddString(_T("75%"));
	m_comboZoom.AddString(_T("100%"));
	m_comboZoom.AddString(_T("150%"));
	m_comboZoom.AddString(_T("200%"));
	m_comboZoom.AddString(_T("300%"));
	m_comboZoom.AddString(_T("全体表示"));
	
	m_comboZoom.SetCurSel(4);
}

void CMainFrame::JointCBLine(CFrameWnd *pFrameWnd, CControlBar *pBar, CControlBar *pCB)
{
	DWORD dw;
	UINT n=0;
	CRect rect;
	
	pFrameWnd->RecalcLayout();
	pCB->GetWindowRect(&rect);
	rect.OffsetRect(1,0);
	dw = pCB->GetBarStyle();
	n = (dw & CBRS_ALIGN_TOP) ? AFX_IDW_DOCKBAR_TOP : n;
	n = (dw & CBRS_ALIGN_BOTTOM && n==0) ? AFX_IDW_DOCKBAR_BOTTOM : n;
	n = (dw & CBRS_ALIGN_LEFT && n==0) ? AFX_IDW_DOCKBAR_LEFT : n;
	n = (dw & CBRS_ALIGN_RIGHT && n==0) ? AFX_IDW_DOCKBAR_RIGHT : n;
	
	pFrameWnd->DockControlBar(pBar,n,&rect);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) 
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	OSVERSIONINFO vinfo;
	vinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&vinfo);
	if (vinfo.dwMajorVersion >= 5) {
		m_bCanBeTransparent = TRUE;
	} else {
		m_bCanBeTransparent = FALSE;
	}
	
	m_bTransparent = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Transparent Mode"), FALSE);
	m_nAlphaValue = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Alpha Value"),  200);
	if (m_bTransparent) {
		cs.dwExStyle |= WS_EX_LAYERED;
	}
	return TRUE;
}

void CMainFrame::OnAdjustAlpha() 
{
	m_pSetAlphaDlg = new WndTransparencySettingsDlg;
	m_pSetAlphaDlg->m_nLevel = m_nAlphaValue;
	m_pSetAlphaDlg->Create(_T(""), _T(""), SW_HIDE, CRect(0, 0, 0, 0), this, IDD_ALPHASLIDE);
	m_pSetAlphaDlg->ShowWindow(SW_SHOWNORMAL);
	m_pSetAlphaDlg->SetFocus();
}

void CMainFrame::OnUpdateAdjustAlpha(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bTransparent);
}

void CMainFrame::OnDestroy() 
{
	CMDIFrameWnd::OnDestroy();
	
	if (m_pSetAlphaDlg != NULL) {
		if (m_nAlphaValue > 255) {
			m_nAlphaValue = 255;
		} else if (m_nAlphaValue < 40) {
			m_nAlphaValue = 40;
		}
		AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Alpha Value"), m_nAlphaValue);
		
		m_pSetAlphaDlg->DestroyWindow();
		delete m_pSetAlphaDlg;
	}
}

LRESULT CMainFrame::OnSlideAlpha(UINT wParam, LONG lParam)
{
	m_nAlphaValue = lParam;
	MakeWindowTransparent();
	return 0;
}

void CMainFrame::MakeWindowTransparent()
{
	SetLayeredWindowAttributes(0, (BYTE)m_nAlphaValue, LWA_ALPHA);
}



void CMainFrame::OnTransparentMode() 
{
	m_bTransparent = !m_bTransparent;
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Transparent Mode"), m_bTransparent);
	if (m_bTransparent) {
		SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		MakeWindowTransparent();
	} else {
		SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
	}
}

void CMainFrame::OnUpdateTransparentMode(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bCanBeTransparent);
	pCmdUI->SetCheck(m_bTransparent && m_bCanBeTransparent);
}


BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	LPNMHDR lpNMHDR = (LPNMHDR)lParam;
	LPNMTOOLBAR lpNMTOOLBAR = (LPNMTOOLBAR)lParam;
	if (lpNMHDR->code == TBN_DROPDOWN) {
		HWND hwnd = lpNMTOOLBAR->hdr.hwndFrom;
		CMenu menu;
		CMenu* pPopup = NULL;
		menu.LoadMenu(IDR_TB_PLDWN);
		int tbID = 0;
		switch (lpNMTOOLBAR->iItem) {
		case ID_BTN_LINK_LINE_STYLE:
			pPopup = menu.GetSubMenu(0);
			break;
		case ID_BTN_LINK_ARROW:
			pPopup = menu.GetSubMenu(1);
			break;
		case ID_BTN_NODE_FILL_COLOR:
			pPopup = menu.GetSubMenu(2);
			tbID = 1;
			break;
		case ID_BTN_LINE_COLOR:
			pPopup = menu.GetSubMenu(3);
			tbID = 1;
			break;
		case ID_BTN_TEXT_COLOR:
			pPopup = menu.GetSubMenu(4);
			tbID = 1;
			break;
		}
		if (pPopup != NULL) {
			RECT rc;
			if (tbID == 0) {
				m_wndToolBar.GetItemRect(m_wndToolBar.CommandToIndex(lpNMTOOLBAR->iItem), &rc);
				m_wndToolBar.ClientToScreen(&rc);
			} else if (tbID == 1) {
				m_wndFormPalette.GetItemRect(m_wndFormPalette.CommandToIndex(lpNMTOOLBAR->iItem), &rc);
				m_wndFormPalette.ClientToScreen(&rc);
			}
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rc.left, rc.bottom + 1, this);
			return FALSE;
		} 
	}

	return CMDIFrameWnd::OnNotify(wParam, lParam, pResult);
}


void CMainFrame::OnMnuLsR0()
{
	((CiEditApp*)AfxGetApp())->m_curLinkLineStyle = CiEditApp::LS_R0;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_LINE_STYLE, m_tbPlDwnImage .ExtractIcon(0));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLineWidth();
}

void CMainFrame::OnUpdateMnuLsR0(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkLineStyle == CiEditApp::LS_R0);
}

void CMainFrame::OnMnuLsDot()
{
	((CiEditApp*)AfxGetApp())->m_curLinkLineStyle = CiEditApp::LS_DOT;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_LINE_STYLE, m_tbPlDwnImage.ExtractIcon(1));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLineWidth();
}

void CMainFrame::OnUpdateMnuLsDot(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkLineStyle == CiEditApp::LS_DOT);
}

void CMainFrame::OnMnuLsR1()
{
	((CiEditApp*)AfxGetApp())->m_curLinkLineStyle = CiEditApp::LS_R1;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_LINE_STYLE, m_tbPlDwnImage.ExtractIcon(2));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLineWidth();
}

void CMainFrame::OnUpdateMnuLsR1(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkLineStyle == CiEditApp::LS_R1);
}

void CMainFrame::OnMnuLsR2()
{
	((CiEditApp*)AfxGetApp())->m_curLinkLineStyle = CiEditApp::LS_R2;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_LINE_STYLE, m_tbPlDwnImage.ExtractIcon(3));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLineWidth();
}

void CMainFrame::OnUpdateMnuLsR2(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkLineStyle == CiEditApp::LS_R2);
}

void CMainFrame::OnMnuLsR3()
{
	((CiEditApp*)AfxGetApp())->m_curLinkLineStyle = CiEditApp::LS_R3;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_LINE_STYLE, m_tbPlDwnImage.ExtractIcon(4));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLineWidth();
}

void CMainFrame::OnUpdateMnuLsR3(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkLineStyle == CiEditApp::LS_R3);
}

void CMainFrame::OnMnuLsR4()
{
	((CiEditApp*)AfxGetApp())->m_curLinkLineStyle = CiEditApp::LS_R4;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_LINE_STYLE, m_tbPlDwnImage.ExtractIcon(5));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLineWidth();
}

void CMainFrame::OnUpdateMnuLsR4(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkLineStyle == CiEditApp::LS_R4);
}

void CMainFrame::OnLinkArrowNone()
{
	((CiEditApp*)AfxGetApp())->m_curLinkArrow = CiEditApp::LA_NONE;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_ARROW, m_tbPlDwnImage.ExtractIcon(6));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLinkArrow();
}

void CMainFrame::OnUpdateLinkArrowNone(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkArrow == CiEditApp::LA_NONE);
}

void CMainFrame::OnLinkArrowSingle()
{
	((CiEditApp*)AfxGetApp())->m_curLinkArrow = CiEditApp::LA_SINGLE;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_ARROW, m_tbPlDwnImage.ExtractIcon(7));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLinkArrow();
}

void CMainFrame::OnUpdateLinkArrowSingle(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkArrow == CiEditApp::LA_SINGLE);
}


void CMainFrame::OnLinkArrowDouble()
{
	((CiEditApp*)AfxGetApp())->m_curLinkArrow = CiEditApp::LA_DOUBLE;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_ARROW, m_tbPlDwnImage.ExtractIcon(8));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLinkArrow();
}

void CMainFrame::OnUpdateLinkArrowDouble(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkArrow == CiEditApp::LA_DOUBLE);
}

void CMainFrame::OnLinkDependSingle()
{
	((CiEditApp*)AfxGetApp())->m_curLinkArrow = CiEditApp::LA_DEPEND_SINGLE;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_ARROW, m_tbPlDwnImage.ExtractIcon(9));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLinkArrow();
}

void CMainFrame::OnUpdateLinkDependSingle(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkArrow == CiEditApp::LA_DEPEND_SINGLE);
}

void CMainFrame::OnLinkDependDouble()
{
	((CiEditApp*)AfxGetApp())->m_curLinkArrow = CiEditApp::LA_DEPEND_DOUBLE;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_ARROW, m_tbPlDwnImage.ExtractIcon(10));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLinkArrow();
}

void CMainFrame::OnUpdateLinkDependDouble(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkArrow == CiEditApp::LA_DEPEND_DOUBLE);
}

void CMainFrame::OnLinkArrowInherit()
{
	((CiEditApp*)AfxGetApp())->m_curLinkArrow = CiEditApp::LA_INHERIT;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_ARROW, m_tbPlDwnImage.ExtractIcon(11));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLinkArrow();
}

void CMainFrame::OnUpdateLinkArrowInherit(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkArrow == CiEditApp::LA_INHERIT);
}

void CMainFrame::OnLinkArrowAgregat()
{
	((CiEditApp*)AfxGetApp())->m_curLinkArrow = CiEditApp::LA_AGGREGAT;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_ARROW, m_tbPlDwnImage.ExtractIcon(12));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLinkArrow();
}

void CMainFrame::OnUpdateLinkArrowAgregat(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkArrow == CiEditApp::LA_AGGREGAT);
}

void CMainFrame::OnLinkArrowComposit()
{
	((CiEditApp*)AfxGetApp())->m_curLinkArrow = CiEditApp::LA_COMPOSIT;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_ARROW, m_tbPlDwnImage.ExtractIcon(13));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLinkArrow();
}

void CMainFrame::OnUpdateLinkArrowComposit(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkArrow == CiEditApp::LA_COMPOSIT);
}

void CMainFrame::OnViewFormBar()
{
	ShowControlBar(&m_wndFormPalette, !m_wndFormPalette.IsWindowVisible(), FALSE);
}

void CMainFrame::OnUpdateViewFormBar(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_wndFormPalette.IsWindowVisible());
}

void CMainFrame::createBrushedBitmap(CBitmap* pBitmap, COLORREF rgb)
{
	CRect bmpRect(0, 0, 16, 15);
	CBrush bkBrush(rgb);
	CDC* pDC = GetDC();
	CDC dc;
	dc.CreateCompatibleDC(pDC);
	
	pBitmap->CreateCompatibleBitmap(pDC, bmpRect.Width(), bmpRect.Height());
	CBitmap * oldBmp = dc.SelectObject(pBitmap);
	dc.FillRect(bmpRect, &bkBrush);
	dc.SelectObject(oldBmp);
	ReleaseDC(pDC);
	bkBrush.DeleteObject();
}

void CMainFrame::OnSelectNodeColor()
{
	CColorDialog dlg(((CiEditApp*)AfxGetApp())->m_colorNodeBtn);
	if (dlg.DoModal() != IDOK) return;
	((CiEditApp*)AfxGetApp())->m_colorNodeBtn = dlg.GetColor();
	CBitmap bmpImage;
	this->createBrushedBitmap(&bmpImage, ((CiEditApp*)AfxGetApp())->m_colorNodeBtn);
	m_wndFormPalette.GetToolBarCtrl().GetImageList()->Replace(10, &bmpImage, NULL);
	m_wndFormPalette.Invalidate();
	bmpImage.DeleteObject();
	((CChildFrame*)MDIGetActive())->ChangeSelectedNodeColor();
}

void CMainFrame::OnSelectLineColor()
{
	CColorDialog dlg(((CiEditApp*)AfxGetApp())->m_colorLineBtn);
	if (dlg.DoModal() != IDOK) return;
	((CiEditApp*)AfxGetApp())->m_colorLineBtn = dlg.GetColor();
	CBitmap bmpImage;
	this->createBrushedBitmap(&bmpImage, ((CiEditApp*)AfxGetApp())->m_colorLineBtn);
	m_wndFormPalette.GetToolBarCtrl().GetImageList()->Replace(11, &bmpImage, NULL);
	m_wndFormPalette.Invalidate();
	bmpImage.DeleteObject();
	((CChildFrame*)MDIGetActive())->ChangeSelectedLineColor();
}

void CMainFrame::OnSelectFontColor()
{
	CColorDialog dlg(((CiEditApp*)AfxGetApp())->m_colorFontBtn);
	if (dlg.DoModal() != IDOK) return;
	((CiEditApp*)AfxGetApp())->m_colorFontBtn = dlg.GetColor();
	CBitmap bmpImage;
	this->createBrushedBitmap(&bmpImage, ((CiEditApp*)AfxGetApp())->m_colorFontBtn);
	m_wndFormPalette.GetToolBarCtrl().GetImageList()->Replace(12, &bmpImage, NULL);
	m_wndFormPalette.Invalidate();
	bmpImage.DeleteObject();
	((CChildFrame*)MDIGetActive())->ChangeSelectedFontColor();
}

void CMainFrame::OnAppHelp()
{
	ShellExecute(m_hWnd, _T("open"), _T("http://kondoumh.com/software/ieditman/index.html"), NULL, _T(""), SW_SHOW);
}
