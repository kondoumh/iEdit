// FoldingSettingsDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "setFoldUpDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// FoldingSettingsDlg ダイアログ


FoldingSettingsDlg::FoldingSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(FoldingSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(FoldingSettingsDlg)
	m_level = 0;
	//}}AFX_DATA_INIT
}


void FoldingSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FoldingSettingsDlg)
	DDX_Control(pDX, IDC_SPIN_LEVEL, m_spin);
	DDX_Text(pDX, IDC_EDITLEVEL, m_level);
	DDV_MinMaxInt(pDX, m_level, 1, 100000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FoldingSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(FoldingSettingsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FoldingSettingsDlg メッセージ ハンドラ

BOOL FoldingSettingsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	m_spin.SetRange(1, 10000);
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}
