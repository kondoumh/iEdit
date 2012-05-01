// iEdit.cpp : �A�v���P�[�V�����p�N���X�̋@�\��`���s���܂��B
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
	// �W���̃t�@�C����{�h�L�������g �R�}���h
	ON_COMMAND(ID_FILE_OPEN, CiEditApp::OnFileOpen)
	ON_COMMAND(ID_FILE_NEW, CiEditApp::OnFileNew)
	// �W���̈���Z�b�g�A�b�v �R�}���h
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND(ID_FILETYPE_REG_DEL, &CiEditApp::OnFiletypeRegDel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CiEditApp �N���X�̍\�z

CiEditApp::CiEditApp()
{
	// TODO: ���̈ʒu�ɍ\�z�p�R�[�h��ǉ����Ă��������B
	// ������ InitInstance ���̏d�v�ȏ��������������ׂċL�q���Ă��������B
	m_curLinkLineStyle = CiEditApp::LS_R0;
	m_curLinkArrow = CiEditApp::LA_NONE;
}

/////////////////////////////////////////////////////////////////////////////
// �B��� CiEditApp �I�u�W�F�N�g

CiEditApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CiEditApp �N���X�̏�����

BOOL CiEditApp::InitInstance()
{
	// CG: �ȉ��̃u���b�N�̓X�v���b�V�� �X�N���[�� �R���|�[�l���g�ɂ���Ēǉ�����܂���
	
	{

		CCommandLineInfo cmdInfo;

		ParseCommandLine(cmdInfo);



		CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);

	}
	AfxEnableControlContainer();
	
	// �W���I�ȏ���������
	// ���������̋@�\���g�p�����A���s�t�@�C���̃T�C�Y��������
	// ��������Έȉ��̓���̏��������[�`���̒�����s�K�v�Ȃ���
	// ���폜���Ă��������B

/* ���̋L�q��MFC 7.0�ɂ����Ă͕s�v�̂悤��
#ifdef _AFXDLL
	Enable3dControls();		// ���L DLL �̒��� MFC ���g�p����ꍇ�ɂ͂������Ăяo���Ă��������B
#else
	Enable3dControlsStatic();	// MFC �ƐÓI�Ƀ����N���Ă���ꍇ�ɂ͂������Ăяo���Ă��������B
#endif
*/	
	// �ݒ肪�ۑ�����鉺�̃��W�X�g�� �L�[��ύX���܂��B
	// TODO: ���̕�������A��Ж��܂��͏����ȂǓK�؂Ȃ��̂�
	// �ύX���Ă��������B
	SetRegistryKey(REGS_ROOT);
	
	LoadStdProfileSettings(9);  // �W���� INI �t�@�C���̃I�v�V���������[�ނ��܂� (MRU ���܂�)
	
	getNodeProfile();
	getLinkProfile();
	getOtherProfile();
	getOptionsProfile();
	m_bShapeModified = false;
	// �A�v���P�[�V�����p�̃h�L�������g �e���v���[�g��o�^���܂��B�h�L�������g �e���v���[�g
	//  �̓h�L�������g�A�t���[�� �E�B���h�E�ƃr���[���������邽�߂ɋ@�\���܂��B
	
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
	
	// ���C�� MDI �t���[�� �E�B���h�E���쐬
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;
	
	// OLE�̏�����
	AfxOleInit();
	
	// �h���b�O/�h���b�v �̃I�[�v���������܂�
	m_pMainWnd->DragAcceptFiles();
	
	if (m_rgsOptions.registFiletype) {
		// DDE Execute open ���g�p�\�ɂ��܂��B
		HKEY hkResult;
		if (::RegOpenKey(HKEY_CURRENT_USER, _T("Software\\Classes"), &hkResult) == ERROR_SUCCESS) {
			RegOverridePredefKey(HKEY_CLASSES_ROOT, hkResult);
			EnableShellOpen();
			RegisterShellFileTypes(TRUE);
			::RegOverridePredefKey(HKEY_CLASSES_ROOT, NULL);
		}
	}
	
	// DDE�Afile open �ȂǕW���̃V�F�� �R�}���h�̃R�}���h���C������͂��܂��B
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	
	// �N������MDI�����޳���޳��\�������Ȃ�
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew) {
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	}
	
	// �R�}���h���C���Ńf�B�X�p�b�` �R�}���h���w�肵�܂��B
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	
	// ���C�� �E�B���h�E�����������ꂽ�̂ŁA�\���ƍX�V���s���܂��B
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// �A�v���P�[�V�����̃o�[�W�������Ŏg���� CAboutDlg �_�C�A���O

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard ���z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �̃T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
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

// �_�C�A���O�����s���邽�߂̃A�v���P�[�V���� �R�}���h
void CiEditApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CiEditApp ���b�Z�[�W �n���h��

BOOL CiEditApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: �ȉ��̃u���b�N�̓X�v���b�V�� �X�N���[�� �R���|�[�l���g�ɂ���Ēǉ�����܂���
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
	::lstrcpy(m_rgsNode.lf.lfFaceName, AfxGetApp()->GetProfileString(REGS_NODE, _T("Font Name"), _T("�l�r �S�V�b�N")));
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
	
	::lstrcpy(m_rgsLink.lf.lfFaceName, AfxGetApp()->GetProfileString(REGS_LINK, _T("Font Name"), _T("�l�r �S�V�b�N")));
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
	// TODO: ���̈ʒu�ɌŗL�̏�����ǉ����邩�A�܂��͊�{�N���X���Ăяo���Ă�������
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
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	CString szFilter = _T("iEdit�t�@�C��(*.iedx)|*.iedx|iEdit�t�@�C��(��)(*.ied)|*.ied|XML�t�@�C��(*.xml)|*.xml||");
	CFileDialog cfDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |
		OFN_FILEMUSTEXIST | OFN_EXPLORER, szFilter, AfxGetMainWnd());
	if (cfDlg.DoModal() == IDOK) {
		DWORD index = cfDlg.m_ofn.nFilterIndex;
		m_pDocTemplate->OpenDocumentFile(cfDlg.GetPathName());
	}
}

void CiEditApp::OnFileNew()
{
	// TODO: �����ɃR�}���h �n���h�� �R�[�h��ǉ����܂��B
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

	//// TODO:  ������ DC �̑�����ύX���Ă��������B

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

	// TODO:  �����ɏ�������ǉ����Ă�������
	m_brsDlg.CreateSolidBrush(RGB(255,255,255));
	GetDlgItem(IDC_WEBSITE)->ModifyStyle(0, SS_NOTIFY);
	HFONT hFont = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);
	::GetObject( hFont, sizeof( LOGFONT), &m_logFont);
	m_logFont.lfUnderline = TRUE;
	m_webSiteFont.CreateFontIndirect(&m_logFont);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

BOOL CAboutDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: �����Ƀ��b�Z�[�W �n���h�� �R�[�h��ǉ����邩�A����̏������Ăяo���܂��B
	if (pWnd->GetDlgCtrlID() == IDC_WEBSITE) {
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
		return TRUE;
	} else {
		return CDialog::OnSetCursor(pWnd, nHitTest, message);
	}
}

void CAboutDlg::OnStnClickedWebsite()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	ShellExecute(m_hWnd, _T("open"), _T("http://kondoumh.com/software/iedit.html"), NULL, _T(""), SW_SHOW);
	EndDialog(MB_OK);
}


void CiEditApp::OnFiletypeRegDel()
{
	// TODO: �����ɃR�}���h �n���h���[ �R�[�h��ǉ����܂��B
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
