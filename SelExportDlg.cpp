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
	m_bPrintText = FALSE;
	m_nTreeOp = -1;
	//}}AFX_DATA_INIT
	m_bShowChekPrintText = false;
}


void SelExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SelExportDlg)
	DDX_Control(pDX, IDC_CHK_PRINT_TEXT, m_chkPrintText);
	DDX_Check(pDX, IDC_CHK_PRINT_TEXT, m_bPrintText);
	DDX_Radio(pDX, IDC_RD_TREE, m_nTreeOp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SelExportDlg, CDialog)
	//{{AFX_MSG_MAP(SelExportDlg)
	ON_BN_CLICKED(IDC_CHK_PRINT_TEXT, OnChkPrintText)
	ON_BN_CLICKED(IDC_RD_TREE, OnRdTree1)
	ON_BN_CLICKED(IDC_RD_TREE2, OnRdTree2)
	ON_BN_CLICKED(IDC_RD_TREE3, OnRdTree3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelExportDlg メッセージ ハンドラ

void SelExportDlg::OnChkPrintText() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_bPrintText = m_chkPrintText.GetCheck();
}

BOOL SelExportDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	if (!m_bShowChekPrintText) {
		m_chkPrintText.EnableWindow(FALSE);
	}
	
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void SelExportDlg::OnRdTree1() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	
}

void SelExportDlg::OnRdTree2() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	
}

void SelExportDlg::OnRdTree3() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	
}
