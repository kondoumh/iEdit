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
	LPSTR lpBuffer;
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
	//{{AFX_MSG_MAP(CiEditApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
	// 標準のファイル基本ドキュメント コマンド
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// 標準の印刷セットアップ コマンド
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_FILE_NEW, &CiEditApp::OnFileNew)
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
	m_pDocTemplate2 = new CMultiDocTemplate(
		IDR_IEDITTYPE_OLD,
		RUNTIME_CLASS(iEditDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(OutlineView));
	AddDocTemplate(m_pDocTemplate2);
	
	// メイン MDI フレーム ウィンドウを作成
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	
	// OLEの初期化
	AfxOleInit();
	
	// ドラッグ/ドロップ のオープンを許可します
	m_pMainWnd->DragAcceptFiles();
	
	// DDE Execute open を使用可能にします。
	HKEY hkResult;
	if (::RegOpenKey(HKEY_CURRENT_USER, "Software\\Classes", &hkResult) == ERROR_SUCCESS) {
		RegOverridePredefKey(HKEY_CLASSES_ROOT, hkResult);	
		EnableShellOpen();
		RegisterShellFileTypes(TRUE);
		::RegOverridePredefKey(HKEY_CLASSES_ROOT, NULL);
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
	afx_msg void OnTomh();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CBitmap m_bmp;
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
	ON_BN_CLICKED(IDC_TOMH, OnTomh)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
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
	m_rgsNode.colorFill = AfxGetApp()->GetProfileInt(REGS_NODE, "Fill Color", RGB(255, 255, 255));
	m_rgsNode.colorLine = AfxGetApp()->GetProfileInt(REGS_NODE, "Line Color", RGB(0, 0, 0));
	m_rgsNode.colorFont = AfxGetApp()->GetProfileInt(REGS_NODE, "Font Color", RGB(0, 0, 0));
	m_rgsNode.bFillColor = AfxGetApp()->GetProfileInt(REGS_NODE, "Paint", FALSE);
	m_rgsNode.lineWidth = AfxGetApp()->GetProfileInt(REGS_NODE, "Line Width", 0);
	m_rgsNode.styleLine = AfxGetApp()->GetProfileInt(REGS_NODE, "Line Style", PS_SOLID);
	m_rgsNode.shape= AfxGetApp()->GetProfileInt(REGS_NODE, "Node Shape", iNode::rectangle);
	m_rgsNode.styleText = AfxGetApp()->GetProfileInt(REGS_NODE, "Text Align", iNode::s_cc);
	::lstrcpy(m_rgsNode.lf.lfFaceName, AfxGetApp()->GetProfileString(REGS_NODE, "Font Name", "ＭＳ ゴシック"));
	m_rgsNode.lf.lfHeight = AfxGetApp()->GetProfileInt(REGS_NODE, "Font Height", 0xfffffff3);
	m_rgsNode.lf.lfWidth = AfxGetApp()->GetProfileInt(REGS_NODE, "Font Width", 0);
	m_rgsNode.lf.lfItalic = AfxGetApp()->GetProfileInt(REGS_NODE, "Font Italic", FALSE);
	m_rgsNode.lf.lfUnderline = AfxGetApp()->GetProfileInt(REGS_NODE, "Font UnderLine", FALSE);
	m_rgsNode.lf.lfStrikeOut = AfxGetApp()->GetProfileInt(REGS_NODE, "Font StrikeOut", FALSE);
	m_rgsNode.lf.lfCharSet= AfxGetApp()->GetProfileInt(REGS_NODE, "Font CharSet", SHIFTJIS_CHARSET);
	m_rgsNode.lf.lfWeight = AfxGetApp()->GetProfileInt(REGS_NODE, "Font Weight", FW_NORMAL);
	
	m_rgsNode.bInheritParent = AfxGetApp()->GetProfileInt(REGS_NODE, "Inherit Parent", FALSE);
	m_rgsNode.bInheritSibling = AfxGetApp()->GetProfileInt(REGS_NODE, "Inherit Sibling", FALSE);
	m_rgsNode.bSyncOrder = AfxGetApp()->GetProfileInt(REGS_NODE, "Sync Order", FALSE);
	m_rgsNode.orderDirection = AfxGetApp()->GetProfileInt(REGS_NODE, "Order Direction", 0);
	m_rgsNode.bEnableGroup = AfxGetApp()->GetProfileInt(REGS_NODE, "Enable Grouping", FALSE);
	m_rgsNode.bDisableNodeResize = AfxGetApp()->GetProfileInt(REGS_NODE, "Disable NodeResize", FALSE);
	m_rgsNode.margin_l = AfxGetApp()->GetProfileIntA(REGS_NODE, "Margin Left", 0);
	m_rgsNode.margin_r = AfxGetApp()->GetProfileIntA(REGS_NODE, "Margin Right", 0);
	m_rgsNode.margin_t = AfxGetApp()->GetProfileIntA(REGS_NODE, "Margin Top", 0);
	m_rgsNode.margin_b = AfxGetApp()->GetProfileIntA(REGS_NODE, "Margin Bottom", 0);
}

void CiEditApp::getLinkProfile()
{
	m_rgsLink.colorLine = AfxGetApp()->GetProfileInt(REGS_LINK, "Line Color", RGB(0, 0, 0));
	m_rgsLink.lineWidth = AfxGetApp()->GetProfileInt(REGS_LINK, "Line Width", 0);
	m_rgsLink.strength  = AfxGetApp()->GetProfileInt(REGS_LINK, "Strength", 10);
	m_rgsLink.styleLine = AfxGetApp()->GetProfileInt(REGS_LINK, "Line Style", PS_SOLID);
	m_rgsLink.bSetStrength = AfxGetApp()->GetProfileInt(REGS_LINK, "Set Strength", TRUE);
	
	::lstrcpy(m_rgsLink.lf.lfFaceName, AfxGetApp()->GetProfileString(REGS_LINK, "Font Name", "ＭＳ ゴシック"));
	m_rgsLink.lf.lfHeight = AfxGetApp()->GetProfileInt(REGS_LINK, "Font Height", 0xfffffff3);
	m_rgsLink.lf.lfWidth = AfxGetApp()->GetProfileInt(REGS_LINK, "Font Width", 0);
	m_rgsLink.lf.lfItalic = AfxGetApp()->GetProfileInt(REGS_LINK, "Font Italic", FALSE);
	m_rgsLink.lf.lfUnderline = AfxGetApp()->GetProfileInt(REGS_LINK, "Font UnderLine", FALSE);
	m_rgsLink.lf.lfStrikeOut = AfxGetApp()->GetProfileInt(REGS_LINK, "Font StrikeOut", FALSE);
	m_rgsLink.lf.lfCharSet= AfxGetApp()->GetProfileInt(REGS_LINK, "Font CharSet", SHIFTJIS_CHARSET);
	m_rgsLink.lf.lfWeight = AfxGetApp()->GetProfileInt(REGS_LINK, "Font Weight", FW_NORMAL);
}

int CiEditApp::ExitInstance() 
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	DeleteCriticalSection(&CRelaxThrd::m_csGDILock);
	CloseHandle(CRelaxThrd::m_hAnotherDead);
	
	return CWinApp::ExitInstance();
}

void CAboutDlg::OnTomh() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	ShellExecute(m_hWnd, "open", "http://homepage3.nifty.com/kondoumh/", NULL, "", SW_SHOW);
	EndDialog(MB_OK);
}

void CiEditApp::loadMetaFiles(const CString& fname)
{
	for (int i = 0; i < 10; i++) {
		CString cname; cname.Format("%02d", i+1);
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
		AfxGetApp()->WriteProfileString(REGS_SHAPES, "recent file", "");
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
	WriteProfileString(REGS_SHAPES, "recent file", fname);
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
	WriteProfileString(REGS_SHAPES, "recent file", pf->GetFilePath());
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
	CString szFilter = "iEditファイル(*.iedx)|*.iedx|iEditファイル(旧)(*.ied)|*.ied|XMLファイル(*.xml)|*.xml||";
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


void CAboutDlg::OnPaint() 
{
	CPaintDC paintDC(this); // 描画用のデバイス コンテキスト
	
	// TODO: この位置にメッセージ ハンドラ用のコードを追加してください
	if( m_bmp.GetSafeHandle() == NULL) {
		// 適当にビットマップを作成
		// ビットマップをロード
		m_bmp.LoadBitmap(IDB_LOGO);
	}
	// ビットマップの大きさを取得
	BITMAP bitmap;
	m_bmp.GetBitmap( &bitmap);
	CSize sz( bitmap.bmWidth, bitmap.bmHeight);
	CDC dc;
	dc.CreateCompatibleDC( &paintDC);
	// ディバイスコンテキストで選択
	CBitmap *pOld = dc.SelectObject( &m_bmp);
	// もともとのディバイスコンテキストに
	// ビットマップを透過で転送
	if (::TransparentBlt( paintDC, 0, 0, sz.cx, sz.cy, dc, 0, 0, sz.cx, sz.cy, (UINT)RGB( 255, 255, 255)) == FALSE) {
		// エラー番号を取得
		DWORD dwErr = ::GetLastError();
		// ここにエラー時の処理を記述すること
	}
	// ビットマップ選択解除
	dc.SelectObject( pOld);
}

void CiEditApp::getOtherProfile()
{
	m_rgsOther.bSetStylesheet = AfxGetApp()->GetProfileInt(REGS_OTHER, "XML StyleSheet", TRUE);
	m_rgsOther.strStyleSheetFile = AfxGetApp()->GetProfileString(REGS_OTHER, "XML StyleSheet Name", "iedit.xsl");
}

