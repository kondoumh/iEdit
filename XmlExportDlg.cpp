// XmlExportDlg.cpp : インプリメンテーション ファイル
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
// XmlExportDlg ダイアログ


XmlExportDlg::XmlExportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(XmlExportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(XmlExportDlg)
	m_nTreeOp = -1;
	//}}AFX_DATA_INIT
}


void XmlExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(XmlExportDlg)
	DDX_Radio(pDX, IDC_RD_TREE, m_nTreeOp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(XmlExportDlg, CDialog)
	//{{AFX_MSG_MAP(XmlExportDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
