// EditShapeCategoryDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "EditShapeCategoryDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// EditShapeCategoryDlg ダイアログ


EditShapeCategoryDlg::EditShapeCategoryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(EditShapeCategoryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(EditShapeCategoryDlg)
	m_name = _T("");
	//}}AFX_DATA_INIT
}


void EditShapeCategoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(EditShapeCategoryDlg)
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Text(pDX, IDC_EDIT1, m_name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(EditShapeCategoryDlg, CDialog)
	//{{AFX_MSG_MAP(EditShapeCategoryDlg)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditShapeCategoryDlg メッセージ ハンドラ

BOOL EditShapeCategoryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_edit.SetSel(0, -1);
	return TRUE;
}

void EditShapeCategoryDlg::OnChangeEdit1()
{
	m_edit.GetWindowText(m_name);
}
