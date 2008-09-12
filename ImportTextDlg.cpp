// ImportTextDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "ImportTextDlg.h"


// ImportTextDlg ダイアログ

IMPLEMENT_DYNAMIC(ImportTextDlg, CDialog)

ImportTextDlg::ImportTextDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ImportTextDlg::IDD, pParent)
	, m_charSelection(0)
{

}

ImportTextDlg::~ImportTextDlg()
{
}

void ImportTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO1, m_charSelection);
}


BEGIN_MESSAGE_MAP(ImportTextDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO1, &ImportTextDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &ImportTextDlg::OnBnClickedRadio2)
END_MESSAGE_MAP()


// ImportTextDlg メッセージ ハンドラ

void ImportTextDlg::OnBnClickedRadio1()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	m_charSelection = 0;
}

void ImportTextDlg::OnBnClickedRadio2()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	m_charSelection = 1;
}
