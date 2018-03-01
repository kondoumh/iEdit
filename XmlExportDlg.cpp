// ExportXmlDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "XmlExportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ExportXmlDlg ダイアログ


ExportXmlDlg::ExportXmlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ExportXmlDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ExportXmlDlg)
	m_nTreeOp = -1;
	//}}AFX_DATA_INIT
}


void ExportXmlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ExportXmlDlg)
	DDX_Radio(pDX, IDC_RD_TREE, m_nTreeOp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ExportXmlDlg, CDialog)
	//{{AFX_MSG_MAP(ExportXmlDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
