// SetMarginDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "SetMarginDlg.h"


// SetMarginDlg ダイアログ

IMPLEMENT_DYNAMIC(SetMarginDlg, CDialog)

SetMarginDlg::SetMarginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SetMarginDlg::IDD, pParent)
	, m_nLeft(0)
	, m_nRight(0)
	, m_nTop(0)
	, m_nBottom(0)
{

}

SetMarginDlg::~SetMarginDlg()
{
}

void SetMarginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPIN_LEFT, m_spinLeft);
	DDX_Control(pDX, IDC_SPIN_RIGHT, m_spinRight);
	DDX_Control(pDX, IDC_SPIN_TOP, m_spinTop);
	DDX_Control(pDX, IDC_SPIN_BOTTOM, m_spinBottom);
}


BEGIN_MESSAGE_MAP(SetMarginDlg, CDialog)
	ON_EN_CHANGE(IDC_ED_LEFT, &SetMarginDlg::OnEnChangeEdLeft)
	ON_EN_CHANGE(IDC_ED_RIGHT, &SetMarginDlg::OnEnChangeEdRight)
	ON_EN_CHANGE(IDC_ED_TOP, &SetMarginDlg::OnEnChangeEdTop)
	ON_EN_CHANGE(IDC_ED_BOTTOM, &SetMarginDlg::OnEnChangeEdBottom)
	ON_BN_CLICKED(IDOK, &SetMarginDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// SetMarginDlg メッセージ ハンドラ

BOOL SetMarginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ここに初期化を追加してください
	m_spinLeft.SetRange32(0, 50);
	m_spinLeft.SetPos32(m_nLeft);
	m_spinRight.SetRange32(0, 50);
	m_spinRight.SetPos32(m_nRight);
	m_spinTop.SetRange32(0, 50);
	m_spinTop.SetPos32(m_nTop);
	m_spinBottom.SetRange32(0, 50);
	m_spinBottom.SetPos32(m_nBottom);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

void SetMarginDlg::OnEnChangeEdLeft()
{
	if (GetDlgItemInt(IDC_ED_LEFT) > 50) m_spinLeft.SetPos32(50);
}

void SetMarginDlg::OnEnChangeEdRight()
{
	if (GetDlgItemInt(IDC_ED_RIGHT) > 50) m_spinRight.SetPos32(50);
}

void SetMarginDlg::OnEnChangeEdTop()
{
	if (GetDlgItemInt(IDC_ED_TOP) > 50) m_spinTop.SetPos32(50);
}

void SetMarginDlg::OnEnChangeEdBottom()
{
	if (GetDlgItemInt(IDC_ED_BOTTOM) > 50) m_spinBottom.SetPos32(50);
}

void SetMarginDlg::OnBnClickedOk()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	m_nLeft = m_spinLeft.GetPos32();
	m_nRight = m_spinRight.GetPos32();
	m_nTop = m_spinTop.GetPos32();
	m_nBottom = m_spinBottom.GetPos32();
	OnOK();
}
