// SelExportDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "SelExportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SelExportDlg ダイアログ


SelExportDlg::SelExportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SelExportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SelExportDlg)
	m_nTreeOp = -1;
	//}}AFX_DATA_INIT
}


void SelExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SelExportDlg)
	DDX_Radio(pDX, IDC_RD_TREE, m_nTreeOp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SelExportDlg, CDialog)
	//{{AFX_MSG_MAP(SelExportDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
