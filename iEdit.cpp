// iEdit.cpp : アプリケーション用クラスの機能定義を行います。
//

#include "stdafx.h"
#include "iEdit.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "iEditDoc.h"
#include "OutLineView.h"
#include "Splash.h"
#include "RelaxThrd.h"
#include "afxwin.h"

#include "FiletypeSettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGS_ROOT _T("KondohMH")
#define REGS_NODE _T("Node Properties")
#define REGS_LINK _T("Link Properties")
#define REGS_OTHER _T("Settings")
#define REGS_SHAPES _T("Shapes")

void showLastErrorMessage()
{
	LPTSTR lpBuffer;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		LANG_USER_DEFAULT,
		(LPTSTR)&lpBuffer,
		0,
		NULL);
	AfxMessageBox(lpBuffer);
	LocalFree(lpBuffer);
}

/////////////////////////////////////////////////////////////////////////////
// CiEditApp

BEGIN_MESSAGE_MAP(CiEditApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// 標準のファイル基本ドキュメント コマンド
	ON_COMMAND(ID_FILE_OPEN, CiEditApp::OnFileOpen)
	ON_COMMAND(ID_FILE_NEW, CiEditApp::OnFileNew)
	// 標準の印刷セットアップ コマンド
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_FILETYPE_REG_DEL, &CiEditApp::OnFiletypeRegDel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CiEditApp クラスの構築

CiEditApp::CiEditApp()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
	m_curLinkLineStyle = CiEditApp::LS_R0;
	m_curLinkArrow = CiEditApp::LA_NONE;
}

/////////////////////////////////////////////////////////////////////////////
// 唯一の CiEditApp オブジェクト

CiEditApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CiEditApp クラスの初期化

BOOL CiEditApp::InitInstance()
{
	// CG: 以下のブロックはスプラッシュ スクリーン コンポーネントによって追加されました
	
	{

		CCommandLineInfo cmdInfo;

		ParseCommandLine(cmdInfo);



		CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);

	}
	AfxEnableControlContainer();
	
	// 標準的な初期化処理
	// もしこれらの機能を使用せず、実行ファイルのサイズを小さく
	// したければ以下の特定の初期化ルーチンの中から不必要なもの
	// を削除してください。

/* この記述はMFC 7.0においては不要のようだ
#ifdef _AFXDLL
	Enable3dControls();		// 共有 DLL の中で MFC を使用する場合にはここを呼び出してください。
#else
	Enable3dControlsStatic();	// MFC と静的にリンクしている場合にはここを呼び出してください。
#endif
*/	
	// 設定が保存される下のレジストリ キーを変更します。
	// TODO: この文字列を、会社名または所属など適切なものに
	// 変更してください。
	SetRegistryKey(REGS_ROOT);
	
	LoadStdProfileSettings(9);  // 標準の INI ファイルのオプションをローﾄﾞします (MRU を含む)
	
	getNodeProfile();
	getLinkProfile();
	getOtherProfile();
	getOptionsProfile();
	m_bShapeModified = false;
	// アプリケーション用のドキュメント テンプレートを登録します。ドキュメント テンプレート
	//  はドキュメント、フレーム ウィンドウとビューを結合するために機能します。
	
	// Initialize static members of CGDIThread
	InitializeCriticalSection(&CRelaxThrd::m_csGDILock);
	
	// CMultiDocTemplate* pDocTemplate;
	m_pDocTemplate = new CMultiDocTemplate(
		IDR_IEDITTYPE,
		RUNTIME_CLASS(iEditDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(OutlineView));
	AddDocTemplate(m_pDocTemplate);
	
	// CMultiDocTemplate* pDocTemplate;
	if (m_rgsOptions.registOldFiletype) {
		m_pDocTemplate2 = new CMultiDocTemplate(
			IDR_IEDITTYPE_OLD,
			RUNTIME_CLASS(iEditDoc),
			RUNTIME_CLASS(CChildFrame),
			RUNTIME_CLASS(OutlineView));
		AddDocTemplate(m_pDocTemplate2);
	}
	
	// メイン MDI フレーム ウィンドウを作成
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	
	// OLEの初期化
	AfxOleInit();
	
	// ドラッグ/ドロップ のオープンを許可します
	m_pMainWnd->DragAcceptFiles();
	
	if (m_rgsOptions.registFiletype) {
		// DDE Execute open を使用可能にします。
		HKEY hkResult;
		if (::RegOpenKey(HKEY_CURRENT_USER, _T("Software\\Classes"), &hkResult) == ERROR_SUCCESS) {
			RegOverridePredefKey(HKEY_CLASSES_ROOT, hkResult);
			EnableShellOpen();
			RegisterShellFileTypes(TRUE);
			::RegOverridePredefKey(HKEY_CLASSES_ROOT, NULL);
		}
	}
	
	// DDE、file open など標準のシェル コマンドのコマンドラインを解析します。
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	
	// 起動時にMDIﾁｬｲﾙﾄﾞｳｨﾝﾄﾞｳを表示させない
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew) {
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	}
	
	// コマンドラインでディスパッチ コマンドを指定します。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	
	// メイン ウィンドウが初期化されたので、表示と更新を行います。
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// アプリケーションのバージョン情報で使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard 仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV のサポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CBitmap m_bmp;
	CBrush m_brsDlg;
	CFont m_webSiteFont;
	LOGFONT m_logFont;
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnStnClickedWebsite();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
//	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_CTLCOLOR()
	ON_WM_SETCURSOR()
	ON_STN_CLICKED(IDC_WEBSITE, &CAboutDlg::OnStnClickedWebsite)
END_MESSAGE_MAP()

// ダイアログを実行するためのアプリケーション コマンド
void CiEditApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CiEditApp メッセージ ハンドラ

BOOL CiEditApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: 以下のブロックはスプラッシュ スクリーン コンポーネントによって追加されました
	if (CSplashWnd::PreTranslateAppMessage(pMsg))
		return TRUE;

	return CWinApp::PreTranslateMessage(pMsg);
}

void CiEditApp::getNodeProfile()
{
	m_rgsNode.colorFill = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Fill Color"), RGB(255, 255, 255));
	m_rgsNode.colorLine = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Line Color"), RGB(0, 0, 0));
	m_rgsNode.colorFont = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Font Color"), RGB(0, 0, 0));
	m_rgsNode.bFillColor = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Paint"), FALSE);
	m_rgsNode.lineWidth = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Line Width"), 0);
	m_rgsNode.styleLine = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Line Style"), PS_SOLID);
	m_rgsNode.shape= AfxGetApp()->GetProfileInt(REGS_NODE, _T("Node Shape"), iNode::rectangle);
	m_rgsNode.styleText = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Text Align"), iNode::s_cc);
	::lstrcpy(m_rgsNode.lf.lfFaceName, AfxGetApp()->GetProfileString(REGS_NODE, _T("Font Name"), _T("ＭＳ ゴシック")));
	m_rgsNode.lf.lfHeight = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Font Height"), 0xfffffff3);
	m_rgsNode.lf.lfWidth = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Font Width"), 0);
	m_rgsNode.lf.lfItalic = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Font Italic"), FALSE);
	m_rgsNode.lf.lfUnderline = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Font UnderLine"), FALSE);
	m_rgsNode.lf.lfStrikeOut = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Font StrikeOut"), FALSE);
	m_rgsNode.lf.lfCharSet= AfxGetApp()->GetProfileInt(REGS_NODE, _T("Font CharSet"), SHIFTJIS_CHARSET);
	m_rgsNode.lf.lfWeight = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Font Weight"), FW_NORMAL);
	
	m_rgsNode.bInheritParent = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Inherit Parent"), FALSE);
	m_rgsNode.bInheritSibling = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Inherit Sibling"), FALSE);
	m_rgsNode.bSyncOrder = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Sync Order"), FALSE);
	m_rgsNode.orderDirection = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Order Direction"), 0);
	m_rgsNode.bEnableGroup = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Enable Grouping"), FALSE);
	m_rgsNode.bDisableNodeResize = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Disable NodeResize"), FALSE);
	m_rgsNode.margin_l = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Margin Left"), 0);
	m_rgsNode.margin_r = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Margin Right"), 0);
	m_rgsNode.margin_t = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Margin Top"), 0);
	m_rgsNode.margin_b = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Margin Bottom"), 0);
	m_rgsNode.bPriorSelectionDragging = AfxGetApp()->GetProfileInt(REGS_NODE, _T("Prior Selected Node Dragging"), FALSE);
}

void CiEditApp::getLinkProfile()
{
	m_rgsLink.colorLine = AfxGetApp()->GetProfileInt(REGS_LINK, _T("Line Color"), RGB(0, 0, 0));
	m_rgsLink.lineWidth = AfxGetApp()->GetProfileInt(REGS_LINK, _T("Line Width"), 0);
	m_rgsLink.strength  = AfxGetApp()->GetProfileInt(REGS_LINK, _T("Strength"), 10);
	m_rgsLink.styleLine = AfxGetApp()->GetProfileInt(REGS_LINK, _T("Line Style"), PS_SOLID);
	m_rgsLink.bSetStrength = AfxGetApp()->GetProfileInt(REGS_LINK, _T("Set Strength"), TRUE);
	
	::lstrcpy(m_rgsLink.lf.lfFaceName, AfxGetApp()->GetProfileString(REGS_LINK, _T("Font Name"), _T("ＭＳ ゴシック")));
	m_rgsLink.lf.lfHeight = AfxGetApp()->GetProfileInt(REGS_LINK, _T("Font Height"), 0xfffffff3);
	m_rgsLink.lf.lfWidth = AfxGetApp()->GetProfileInt(REGS_LINK, _T("Font Width"), 0);
	m_rgsLink.lf.lfItalic = AfxGetApp()->GetProfileInt(REGS_LINK, _T("Font Italic"), FALSE);
	m_rgsLink.lf.lfUnderline = AfxGetApp()->GetProfileInt(REGS_LINK, _T("Font UnderLine"), FALSE);
	m_rgsLink.lf.lfStrikeOut = AfxGetApp()->GetProfileInt(REGS_LINK, _T("Font StrikeOut"), FALSE);
	m_rgsLink.lf.lfCharSet= AfxGetApp()->GetProfileInt(REGS_LINK, _T("Font CharSet"), SHIFTJIS_CHARSET);
	m_rgsLink.lf.lfWeight = AfxGetApp()->GetProfileInt(REGS_LINK, _T("Font Weight"), FW_NORMAL);
}

void CiEditApp::getOptionsProfile()
{
	m_rgsOptions.registFiletype = AfxGetApp()->GetProfileInt(REGS_OTHER, _T("Register Filetypes"), TRUE);
	m_rgsOptions.registOldFiletype = AfxGetApp()->GetProfileInt(REGS_OTHER, _T("Register Old Filetype"), TRUE);
}

int CiEditApp::ExitInstance() 
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	DeleteCriticalSection(&CRelaxThrd::m_csGDILock);
	CloseHandle(CRelaxThrd::m_hAnotherDead);
	
	return CWinApp::ExitInstance();
}

void CiEditApp::loadMetaFiles(const CString& fname)
{
	for (int i = 0; i < 10; i++) {
		CString cname; cname.Format(_T("%02d"), i+1);
		m_mfCategories[i] = cname;
		for (int j = 0; j < 20; j++) {
			HENHMETAFILE hm = NULL;
			m_hMetaFiles[i*100+j] = hm;
		}
	}	
	CFile f;
	BOOL res = f.Open(fname, CFile::modeRead);
	if (res) {
		CArchive ar(&f, CArchive::load);
		loadmfs(ar);
	} else {
		AfxGetApp()->WriteProfileString(REGS_SHAPES, _T("recent file"), _T(""));
	}
}

void CiEditApp::saveMetaFiles(const CString &fname)
{
	CFile f(fname, CFile::modeWrite | CFile::modeCreate);
	CArchive ar(&f, CArchive::store);
	
	for (int i = 0; i < 10; i++) {
		ar << m_mfCategories[i];
		for (int j = 0; j < 20; j++) {
			ar << (i+1)*100+j;
			UINT hBits = GetEnhMetaFileBits(m_hMetaFiles[i*100+j], NULL, NULL);
			ar << hBits;
			
			if (hBits > 0) {
				BYTE *pData = new BYTE[hBits];
				UINT ret = GetEnhMetaFileBits(m_hMetaFiles[i*100+j], hBits, pData);
				for (unsigned int i = 0; i < hBits; i++) {
					ar << pData[i];
				}
				delete pData;
			}
		}
	}
	WriteProfileString(REGS_SHAPES, _T("recent file"), fname);
}

void CiEditApp::loadmfs(CArchive &ar)
{
	for (int i = 0; i < 10; i++) {
		CString cname;
		ar >> cname;
		m_mfCategories[i] = cname;
		for (int j = 0; j < 20; j++) {
			int key;
			ar >> key;
			UINT hBits;
			ar >> hBits;
			HENHMETAFILE hm = NULL;
			if (hBits > 0) {
				BYTE *pData = new BYTE[hBits];
				for (unsigned int i = 0; i < hBits; i++) {
					ar >> pData[i];
				}
				hm = SetEnhMetaFileBits(hBits, pData);
				delete pData;
			}
			m_hMetaFiles[i*100+j] = hm;
		}
	}
	CFile* pf = ar.GetFile();
	WriteProfileString(REGS_SHAPES, _T("recent file"), pf->GetFilePath());
}

void CiEditApp::notifyViewSetting()
{
	POSITION posDocTempl = GetFirstDocTemplatePosition();
	CDocTemplate* pDocTemplate = GetNextDocTemplate(posDocTempl);
	POSITION pos =  pDocTemplate->GetFirstDocPosition();
	while (pos != NULL) {
		iEditDoc* pDoc = (iEditDoc*)pDocTemplate->GetNextDoc(pos);
		pDoc->viewSettingChanged();
	}
}

void CiEditApp::OnFileOpen() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	CString szFilter = _T("iEditファイル(*.iedx)|*.iedx|iEditファイル(旧)(*.ied)|*.ied|XMLファイル(*.xml)|*.xml||");
	CFileDialog cfDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |
		OFN_FILEMUSTEXIST | OFN_EXPLORER, szFilter, AfxGetMainWnd());
	if (cfDlg.DoModal() == IDOK) {
		DWORD index = cfDlg.m_ofn.nFilterIndex;
		m_pDocTemplate->OpenDocumentFile(cfDlg.GetPathName());
	}
}

void CiEditApp::OnFileNew()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	m_pDocTemplate->OpenDocumentFile(NULL);
}

void CiEditApp::getOtherProfile()
{
	m_rgsOther.bSetStylesheet = AfxGetApp()->GetProfileInt(REGS_OTHER, _T("XML StyleSheet"), TRUE);
	m_rgsOther.strStyleSheetFile = AfxGetApp()->GetProfileString(REGS_OTHER, _T("XML StyleSheet Name"), _T("iedit.xsl"));
	m_rgsOther.bOpenFilesAfterExport = AfxGetApp()->GetProfileInt(REGS_OTHER, _T("Open Files After Export"), TRUE);
	m_rgsOther.bOutputFileLinksOnExport = AfxGetApp()->GetProfileInt(REGS_OTHER, _T("Output Filelinks On Export"), FALSE);
}

void CiEditApp::DebugWriteLine(const CString &message)
{
	((CMainFrame*)m_pMainWnd)->ShowDebugMessage(message);
}

HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	//HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	//// TODO:  ここで DC の属性を変更してください。

	switch(nCtlColor){
	case CTLCOLOR_DLG:
		return (HBRUSH) m_brsDlg;
	case CTLCOLOR_STATIC:
		pDC->SetBkMode(TRANSPARENT);
		if (pWnd->GetDlgCtrlID() == IDC_WEBSITE) {
			pDC->SelectObject(&m_webSiteFont);
			pDC->SetTextColor(RGB(0, 0, 255));
		} else {
			pDC->SetTextColor(RGB(0, 0, 0));
		}
		return (HBRUSH) m_brsDlg;
	default:
		break;
	}
	return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ここに初期化を追加してください
	m_brsDlg.CreateSolidBrush(RGB(255,255,255));
	GetDlgItem(IDC_WEBSITE)->ModifyStyle(0, SS_NOTIFY);
	HFONT hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	::GetObject( hFont, sizeof( LOGFONT), &m_logFont);
	m_logFont.lfUnderline = TRUE;
	m_webSiteFont.CreateFontIndirect(&m_logFont);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

BOOL CAboutDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: ここにメッセージ ハンドラ コードを追加するか、既定の処理を呼び出します。
	if (pWnd->GetDlgCtrlID() == IDC_WEBSITE) {
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
		return TRUE;
	} else {
		return CDialog::OnSetCursor(pWnd, nHitTest, message);
	}
}

void CAboutDlg::OnStnClickedWebsite()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	ShellExecute(m_hWnd, _T("open"), _T("http://kondoumh.com/software/iedit.html"), NULL, _T(""), SW_SHOW);
	EndDialog(MB_OK);
}


void CiEditApp::OnFiletypeRegDel()
{
	// TODO: ここにコマンド ハンドラー コードを追加します。
	FiletypeSettingDlg dlg;
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	BOOL bRegist = pApp->m_rgsOptions.registFiletype;
	dlg.m_registFileType = bRegist ? 1 : 0;
	dlg.m_bRegistOldtype = pApp->m_rgsOptions.registOldFiletype;
	if (dlg.DoModal() != IDOK) return;
	if (dlg.m_registFileType == 1) {
		bRegist = TRUE;
	} else {
		bRegist = FALSE;
	}
	pApp->m_rgsOptions.registFiletype = bRegist;
	pApp->m_rgsOptions.registOldFiletype = dlg.m_bRegistOldtype;
	pApp->WriteProfileInt(REGS_OTHER, _T("Register Filetypes"), bRegist);
	pApp->WriteProfileInt(REGS_OTHER, _T("Register Old Filetype"), dlg.m_bRegistOldtype);
	if (!bRegist) {
		UnregisterShellFileTypes();
	}
}
