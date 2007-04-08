// SelImportDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "SelImportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SelImportDlg ダイアログ


SelImportDlg::SelImportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SelImportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SelImportDlg)
	m_importMode = -1;
	//}}AFX_DATA_INIT
}


void SelImportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SelImportDlg)
	DDX_Radio(pDX, IDC_RD_REP, m_importMode);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SelImportDlg, CDialog)
	//{{AFX_MSG_MAP(SelImportDlg)
	ON_BN_CLICKED(IDC_RD_REP, OnRdRep)
	ON_BN_CLICKED(IDC_RD_SUB, OnRdSub)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelImportDlg メッセージ ハンドラ

void SelImportDlg::OnRdRep() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_importMode = 0;
}

void SelImportDlg::OnRdSub() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_importMode = 1;
}
