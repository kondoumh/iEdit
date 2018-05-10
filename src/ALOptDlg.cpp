// ALOptDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "ALOptDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ALOptDlg ダイアログ


ALOptDlg::ALOptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ALOptDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ALOptDlg)
	m_showMode = -1;
	m_cycle = 0;
	//}}AFX_DATA_INIT
}


void ALOptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ALOptDlg)
	DDX_Control(pDX, IDC_SPIN_CYCLE, m_spin);
	DDX_Control(pDX, IDC_EDITCY, m_editCY);
	DDX_Radio(pDX, IDC_RD_ANI, m_showMode);
	DDX_Text(pDX, IDC_EDITCY, m_cycle);
	DDV_MinMaxInt(pDX, m_cycle, 1, 10000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ALOptDlg, CDialog)
	//{{AFX_MSG_MAP(ALOptDlg)
	ON_BN_CLICKED(IDC_RD_ANI, OnRdAni)
	ON_BN_CLICKED(IDC_RD_CY, OnRdCy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ALOptDlg メッセージ ハンドラ

BOOL ALOptDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	m_spin.SetRange(0, 10000);
	if (m_showMode == 0) {
		m_spin.EnableWindow(FALSE);
		m_editCY.EnableWindow(FALSE);
	}

	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void ALOptDlg::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
	m_cycle = m_spin.GetPos();
	CDialog::OnOK();
}

void ALOptDlg::OnRdAni() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_spin.EnableWindow(FALSE);
	m_editCY.EnableWindow(FALSE);
	m_showMode = 0;
}

void ALOptDlg::OnRdCy() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_spin.EnableWindow(TRUE);
	m_editCY.EnableWindow(TRUE);
	m_showMode = 1;
}
