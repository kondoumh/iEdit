// MainFrm.cpp : CMainFrame クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "iEdit.h"

#include "MainFrm.h"
#include "Splash.h"

#include "PropertyDlg.h"
#include "nodeSrchDlg.h"
#include "SetAlphaDlg.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGS_FRAME _T("Frame Options")

#define IDX_TB_LINE_STYLE 17
#define IDX_TB_ARROW 18
#define IDX_TB_ZOOMCOMBO 29

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
ON_COMMAND(ID_BTN_LINK_LINE_STYLE, &CMainFrame::OnBtnLinkLineStyle)
ON_COMMAND(ID_BTN_LINK_ARROW, &CMainFrame::OnBtnLinkArrow)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ステータス ライン インジケータ
	ID_INDICATOR_KANA,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame クラスの構築/消滅

CMainFrame::CMainFrame()
{
	// TODO: この位置にメンバの初期化処理コードを追加してください。
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
		TRACE0("Failed to create toolbar\n");
		return -1;      // 作成に失敗
	}
	
/*	if (!m_wndUMLPalette.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC, CRect(0,0,0,0), IDR_UML_PALLETE) ||
		!m_wndUMLPalette.LoadToolBar(IDR_UML_PALLETE))
	{
		TRACE0("Failed to create uml pallete\n");
		return -1;      // 作成に失敗
	}*/
	
	////////////// StatusBar
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // 作成に失敗
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
	
	m_wndToolBar.SetWindowText("編集");
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	
	// ズーム変更用コンボボックスの追加
	addComboZoom();

//	m_wndUMLPalette.SetWindowText("UML パレット");
//	m_wndUMLPalette.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
//	JointCBLine(this,&m_wndUMLPalette,&m_wndToolBar);
	
	
	BOOL saveBar = AfxGetApp()->GetProfileInt(REGS_FRAME, "Save bar status", FALSE);
	if (saveBar) {
		LoadBarState("BarSettings");
	}
	
	// CG: 以下のブロックはスプラッシュ スクリーン コンポーネントによって追加されました
	CSplashWnd::ShowSplashScreen(this);
	
	BOOL saveFrame = AfxGetApp()->GetProfileInt(REGS_FRAME, "Save Frame Sizes", FALSE);
	if (saveFrame) {
		loadFramePosition();
	}	
	
	// メンバー初期化
	m_pSetAlphaDlg = NULL;
	m_tbPlDwnImage.Create(IDB_TB_PLDWN, 16, 5, RGB(255, 0, 255));
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
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	CPropertyDlg dlg;
	if (dlg.DoModal() == IDOK) {
		CiEditApp* pApp = (CiEditApp*)AfxGetApp();
		pApp->notifyViewSetting();
	}
}

void CMainFrame::OnUpdateSetProperties(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	
}

void CMainFrame::OnClose() 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	BOOL saveBar = AfxGetApp()->GetProfileInt(REGS_FRAME, "Save bar status", FALSE);
	if (saveBar) {
		SaveBarState("BarSettings");
	}	
	
	BOOL saveFrame = AfxGetApp()->GetProfileInt(REGS_FRAME, "Save Frame Sizes", FALSE);
	if (saveFrame) {
		saveFramePosition();
	}
	
	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	char fileName[MAX_PATH];
	DragQueryFile(hDropInfo, 0, fileName, MAX_PATH);
	
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath_s(fileName, drive, dir, fname, ext );
	CString extent = ext;
	extent.MakeLower();
	if (extent != ".ied" && extent != ".xml") {
		AfxMessageBox("iEditファイルではありません");
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
		if (wndpl.showCmd == SW_SHOWMAXIMIZED) {
			GetWindowRect(&rc);
		}
		if (rc.left >= -10 && rc.top >= -10 && rc.Width() < 10000 && rc.Height() < 10000) {
			AfxGetApp()->WriteProfileInt(REGS_FRAME, "Window Left", rc.left);
			AfxGetApp()->WriteProfileInt(REGS_FRAME, "Window Top", rc.top);
			AfxGetApp()->WriteProfileInt(REGS_FRAME, "Window Width", rc.Width());
			AfxGetApp()->WriteProfileInt(REGS_FRAME, "Window Height", rc.Height());
		}
	}
}

void CMainFrame::loadFramePosition()
{
	int left = AfxGetApp()->GetProfileInt(REGS_FRAME, "Window Left", 0);
	int top = AfxGetApp()->GetProfileInt(REGS_FRAME, "Window Top", 0);
	int width = AfxGetApp()->GetProfileInt(REGS_FRAME, "Window Width", 640);
	int height = AfxGetApp()->GetProfileInt(REGS_FRAME, "Window Height", 480);
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
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。
	OSVERSIONINFO vinfo;
	vinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	::GetVersionEx(&vinfo);
	if (vinfo.dwMajorVersion >= 5) {
		m_bCanBeTransparent = TRUE;
	} else {
		m_bCanBeTransparent = FALSE;
	}
	
	m_bTransparent = AfxGetApp()->GetProfileInt(REGS_FRAME, "Transparent Mode", FALSE);
	m_nAlphaValue = AfxGetApp()->GetProfileInt(REGS_FRAME, "Alpha Value",  200);
	if (m_bTransparent) {
		cs.dwExStyle |= WS_EX_LAYERED;
	}
	return TRUE;
}

void CMainFrame::OnAdjustAlpha() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	m_pSetAlphaDlg = new SetAlphaDlg;
	m_pSetAlphaDlg->m_nLevel = m_nAlphaValue;
	m_pSetAlphaDlg->Create("", "", SW_HIDE, CRect(0, 0, 0, 0), this, IDD_ALPHASLIDE);
	m_pSetAlphaDlg->ShowWindow(SW_SHOWNORMAL);
	m_pSetAlphaDlg->SetFocus();

}

void CMainFrame::OnUpdateAdjustAlpha(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(m_bTransparent);
}

void CMainFrame::OnDestroy() 
{
	CMDIFrameWnd::OnDestroy();
	
	// TODO: この位置にメッセージ ハンドラ用のコードを追加してください
	if (m_pSetAlphaDlg != NULL) {
		if (m_nAlphaValue > 255) {
			m_nAlphaValue = 255;
		} else if (m_nAlphaValue < 40) {
			m_nAlphaValue = 40;
		}
		AfxGetApp()->WriteProfileInt(REGS_FRAME, "Alpha Value", m_nAlphaValue);
		
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
	FWINLAYER pfWin;
	HINSTANCE hDLL = LoadLibrary("user32.dll");
	pfWin=(FWINLAYER)GetProcAddress(hDLL,"SetLayeredWindowAttributes");
	if (pfWin!=NULL) { /* SetLayeredWindowAttributes実行 */
		pfWin(m_hWnd, 0 ,(BYTE)m_nAlphaValue, 2);
	}
	FreeLibrary(hDLL);
//	SetLayeredWindowAttributes(0, (BYTE)m_nAlphaValue, LWA_ALPHA); // 2000 以降ならLoadLibrary不要
}



void CMainFrame::OnTransparentMode() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	m_bTransparent = !m_bTransparent;
	AfxGetApp()->WriteProfileInt(REGS_FRAME, "Transparent Mode", m_bTransparent);
	if (m_bTransparent) {
		SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		MakeWindowTransparent();
	} else {
		SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
	}
}

void CMainFrame::OnUpdateTransparentMode(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->Enable(m_bCanBeTransparent);
	pCmdUI->SetCheck(m_bTransparent && m_bCanBeTransparent);
}


BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: ここに特定なコードを追加するか、もしくは基本クラスを呼び出してください。
	LPNMHDR lpNMHDR = (LPNMHDR)lParam;
	LPNMTOOLBAR lpNMTOOLBAR = (LPNMTOOLBAR)lParam;
	if (lpNMHDR->code == TBN_DROPDOWN) {
		HWND hwnd = lpNMTOOLBAR->hdr.hwndFrom;
		CMenu menu;
		CMenu* pPopup = NULL;
		menu.LoadMenu(IDR_TB_PLDWN);
		switch (lpNMTOOLBAR->iItem) {
		case ID_BTN_LINK_LINE_STYLE:
			pPopup = menu.GetSubMenu(0);
			break;
		case ID_BTN_LINK_ARROW:
			pPopup = menu.GetSubMenu(1);
			break;
		}
		if (pPopup != NULL) {
			RECT rc;
			m_wndToolBar.GetItemRect(m_wndToolBar.CommandToIndex(lpNMTOOLBAR->iItem), &rc);
			m_wndToolBar.ClientToScreen(&rc);
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, rc.left, rc.bottom + 1, this);
			return FALSE;
		}
	}

	return CMDIFrameWnd::OnNotify(wParam, lParam, pResult);
}


void CMainFrame::OnMnuLsR0()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	((CiEditApp*)AfxGetApp())->m_curLinkLineStyle = CiEditApp::LS_R0;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_LINE_STYLE, m_tbPlDwnImage .ExtractIcon(0));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLineWidth();
}

void CMainFrame::OnUpdateMnuLsR0(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkLineStyle == CiEditApp::LS_R0);
}

void CMainFrame::OnMnuLsDot()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	((CiEditApp*)AfxGetApp())->m_curLinkLineStyle = CiEditApp::LS_DOT;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_LINE_STYLE, m_tbPlDwnImage.ExtractIcon(1));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLineWidth();
}

void CMainFrame::OnUpdateMnuLsDot(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkLineStyle == CiEditApp::LS_DOT);
}

void CMainFrame::OnMnuLsR1()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	((CiEditApp*)AfxGetApp())->m_curLinkLineStyle = CiEditApp::LS_R1;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_LINE_STYLE, m_tbPlDwnImage.ExtractIcon(2));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLineWidth();
}

void CMainFrame::OnUpdateMnuLsR1(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkLineStyle == CiEditApp::LS_R1);
}

void CMainFrame::OnMnuLsR2()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	((CiEditApp*)AfxGetApp())->m_curLinkLineStyle = CiEditApp::LS_R2;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_LINE_STYLE, m_tbPlDwnImage.ExtractIcon(3));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLineWidth();
}

void CMainFrame::OnUpdateMnuLsR2(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkLineStyle == CiEditApp::LS_R2);
}

void CMainFrame::OnMnuLsR3()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	((CiEditApp*)AfxGetApp())->m_curLinkLineStyle = CiEditApp::LS_R3;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_LINE_STYLE, m_tbPlDwnImage.ExtractIcon(4));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLineWidth();
}

void CMainFrame::OnUpdateMnuLsR3(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkLineStyle == CiEditApp::LS_R3);
}

void CMainFrame::OnMnuLsR4()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	((CiEditApp*)AfxGetApp())->m_curLinkLineStyle = CiEditApp::LS_R4;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_LINE_STYLE, m_tbPlDwnImage.ExtractIcon(5));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLineWidth();
}

void CMainFrame::OnUpdateMnuLsR4(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkLineStyle == CiEditApp::LS_R4);
}

void CMainFrame::OnLinkArrowNone()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	((CiEditApp*)AfxGetApp())->m_curLinkArrow = CiEditApp::LA_NONE;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_ARROW, m_tbPlDwnImage.ExtractIcon(6));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLinkArrow();
}

void CMainFrame::OnUpdateLinkArrowNone(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkArrow == CiEditApp::LA_NONE);
}

void CMainFrame::OnLinkArrowSingle()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	((CiEditApp*)AfxGetApp())->m_curLinkArrow = CiEditApp::LA_SINGLE;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_ARROW, m_tbPlDwnImage.ExtractIcon(7));
	m_wndToolBar.Invalidate();

//	m_wndToolBar.GetToolBarCtrl().SetCmdID(IDX_TB_ARROW, ID_SET_LINK_ARROW_SINGLE);

//	TBBUTTONINFO tbi;
//	m_wndToolBar.GetToolBarCtrl().GetButtonInfo(IDX_TB_ARROW, &tbi);
//	tbi.idCommand = ID_SET_LINK_ARROW_SINGLE;
//	m_wndToolBar.GetToolBarCtrl().SetButtonInfo(IDX_TB_ARROW, &tbi);
	((CChildFrame*)MDIGetActive())->changeSelectedLinkArrow();
}

void CMainFrame::OnUpdateLinkArrowSingle(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkArrow == CiEditApp::LA_SINGLE);
}


void CMainFrame::OnLinkArrowDouble()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	((CiEditApp*)AfxGetApp())->m_curLinkArrow = CiEditApp::LA_DOUBLE;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_ARROW, m_tbPlDwnImage.ExtractIcon(8));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLinkArrow();
}

void CMainFrame::OnUpdateLinkArrowDouble(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkArrow == CiEditApp::LA_DOUBLE);
}

void CMainFrame::OnLinkDependSingle()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	((CiEditApp*)AfxGetApp())->m_curLinkArrow = CiEditApp::LA_DEPEND_SINGLE;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_ARROW, m_tbPlDwnImage.ExtractIcon(9));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLinkArrow();
}

void CMainFrame::OnUpdateLinkDependSingle(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkArrow == CiEditApp::LA_DEPEND_SINGLE);
}

void CMainFrame::OnLinkDependDouble()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	((CiEditApp*)AfxGetApp())->m_curLinkArrow = CiEditApp::LA_DEPEND_DOUBLE;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_ARROW, m_tbPlDwnImage.ExtractIcon(10));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLinkArrow();
}

void CMainFrame::OnUpdateLinkDependDouble(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkArrow == CiEditApp::LA_DEPEND_DOUBLE);
}

void CMainFrame::OnLinkArrowInherit()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	((CiEditApp*)AfxGetApp())->m_curLinkArrow = CiEditApp::LA_INHERIT;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_ARROW, m_tbPlDwnImage.ExtractIcon(11));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLinkArrow();
}

void CMainFrame::OnUpdateLinkArrowInherit(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkArrow == CiEditApp::LA_INHERIT);
}

void CMainFrame::OnLinkArrowAgregat()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	((CiEditApp*)AfxGetApp())->m_curLinkArrow = CiEditApp::LA_AGGREGAT;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_ARROW, m_tbPlDwnImage.ExtractIcon(12));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLinkArrow();
}

void CMainFrame::OnUpdateLinkArrowAgregat(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkArrow == CiEditApp::LA_AGGREGAT);
}

void CMainFrame::OnLinkArrowComposit()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	((CiEditApp*)AfxGetApp())->m_curLinkArrow = CiEditApp::LA_COMPOSIT;
	m_wndToolBar.GetToolBarCtrl().GetImageList()->Replace(IDX_TB_ARROW, m_tbPlDwnImage.ExtractIcon(13));
	m_wndToolBar.Invalidate();
	((CChildFrame*)MDIGetActive())->changeSelectedLinkArrow();
}

void CMainFrame::OnUpdateLinkArrowComposit(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
	pCmdUI->SetCheck(((CiEditApp*)AfxGetApp())->m_curLinkArrow == CiEditApp::LA_COMPOSIT);
}

void CMainFrame::OnBtnLinkLineStyle()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	((CChildFrame*)MDIGetActive())->changeSelectedLineWidth();
}

void CMainFrame::OnBtnLinkArrow()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	((CChildFrame*)MDIGetActive())->changeSelectedLinkArrow();
}
