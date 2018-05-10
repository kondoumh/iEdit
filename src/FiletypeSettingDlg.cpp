// FiletypeSettingDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "FiletypeSettingDlg.h"
#include "afxdialogex.h"


// FiletypeSettingDlg ダイアログ

IMPLEMENT_DYNAMIC(FiletypeSettingDlg, CDialog)

FiletypeSettingDlg::FiletypeSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(FiletypeSettingDlg::IDD, pParent)
	, m_registFileType(0)
	, m_bRegistOldtype(FALSE)
{
	
}

FiletypeSettingDlg::~FiletypeSettingDlg()
{
}

void FiletypeSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RD_NOT_REGIST, m_registFileType);
	DDX_Check(pDX, IDC_CHK_REGIST_OLDTYPE, m_bRegistOldtype);
}


BEGIN_MESSAGE_MAP(FiletypeSettingDlg, CDialog)
	ON_BN_CLICKED(IDC_RD_NOT_REGIST, &FiletypeSettingDlg::OnBnClickedRdNotRegist)
	ON_BN_CLICKED(IDC_RD_REGIST, &FiletypeSettingDlg::OnBnClickedRdRegist)
END_MESSAGE_MAP()


// FiletypeSettingDlg メッセージ ハンドラー


BOOL FiletypeSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ここに初期化を追加してください
	if (m_registFileType == 0) {
		GetDlgItem(IDC_CHK_REGIST_OLDTYPE)->EnableWindow(FALSE);
	} else {
		GetDlgItem(IDC_CHK_REGIST_OLDTYPE)->EnableWindow(TRUE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}


void FiletypeSettingDlg::OnBnClickedRdNotRegist()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	GetDlgItem(IDC_CHK_REGIST_OLDTYPE)->EnableWindow(FALSE);
}


void FiletypeSettingDlg::OnBnClickedRdRegist()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	GetDlgItem(IDC_CHK_REGIST_OLDTYPE)->EnableWindow(TRUE);
}
