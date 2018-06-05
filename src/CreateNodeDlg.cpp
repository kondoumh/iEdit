#include "stdafx.h"
#include "iEdit.h"
#include "CreateNodeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_SETCHARDATA WM_USER + 12

/////////////////////////////////////////////////////////////////////////////
// CreateNodeDlg ダイアログ

CreateNodeDlg::CreateNodeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CreateNodeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CreateNodeDlg)
	m_strcn = _T("");
	//}}AFX_DATA_INIT
}

void CreateNodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CreateNodeDlg)
	DDX_Control(pDX, IDC_EDIT1, m_editName);
	DDX_Text(pDX, IDC_EDIT1, m_strcn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CreateNodeDlg, CDialog)
	//{{AFX_MSG_MAP(CreateNodeDlg)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETCHARDATA, OnSetCharData)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL CreateNodeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	GetWindowRect(&iniRect);
	if (m_iniPt.x != -1 && m_iniPt.y != -1) {
		MoveWindow(m_iniPt.x, m_iniPt.y, iniRect.Width(), iniRect.Height(), TRUE);
	}

	if (m_strcn == _T("")) {
		m_editName.SetWindowText(_T("新しいノード"));
	}
	else if (m_strcn == _T("ノード")) {
		m_editName.SetWindowText(_T("ノード"));
	}
	else {
		PostMessage(WM_SETCHARDATA, 0);
	}
	return TRUE;
}

void CreateNodeDlg::OnClose()
{
	MoveWindow(iniRect.left, iniRect.top, iniRect.Width(), iniRect.Height(), TRUE);
	CDialog::OnClose();
}

LRESULT CreateNodeDlg::OnSetCharData(UINT wParam, LONG lParam)
{
	m_editName.ReplaceSel(m_strcn);
	return 0;
}
