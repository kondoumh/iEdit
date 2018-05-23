// LinkForPathDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "LinkForPathDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// LinkForPathDlg ダイアログ

LinkForPathDlg::LinkForPathDlg(CWnd* pParent /*=NULL*/)
	: CDialog(LinkForPathDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(LinkForPathDlg)
	//}}AFX_DATA_INIT
}

void LinkForPathDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(LinkForPathDlg)
	DDX_Control(pDX, IDC_EDIT2, m_Comment);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_ORG, m_Org);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(LinkForPathDlg, CDialog)
	//{{AFX_MSG_MAP(LinkForPathDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void LinkForPathDlg::OnBrowse() 
{
	WCHAR szFilters[] = _T("全てのファイル (*.*)|*.*|");
	CFileDialog dlg(TRUE, _T("*.*"), NULL, OFN_HIDEREADONLY, szFilters, NULL);
	if (dlg.DoModal() != IDOK) return;
	m_edit.SetSel(0, -1);
	m_edit.ReplaceSel(dlg.GetPathName());
}

void LinkForPathDlg::OnOK() 
{
	m_edit.GetWindowText(strPath);
	m_Comment.GetWindowText(strComment);
	if (strPath == _T("")) {
		MessageBox(_T("ファイル名または、URLを入力してください"));
		return;
	}
	CDialog::OnOK();
}

BOOL LinkForPathDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_Org.SetWindowText(strOrg);
	m_Comment.ReplaceSel(strComment);
	m_edit.ReplaceSel(strPath);
	return TRUE;
}
