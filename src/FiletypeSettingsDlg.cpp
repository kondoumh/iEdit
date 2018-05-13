// FiletypeSettingsDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "FiletypeSettingsDlg.h"
#include "afxdialogex.h"


// FiletypeSettingsDlg ダイアログ

IMPLEMENT_DYNAMIC(FiletypeSettingsDlg, CDialog)

FiletypeSettingsDlg::FiletypeSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(FiletypeSettingsDlg::IDD, pParent)
	, m_registFileType(0)
	, m_bRegistOldtype(FALSE)
{
	
}

FiletypeSettingsDlg::~FiletypeSettingsDlg()
{
}

void FiletypeSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RD_NOT_REGIST, m_registFileType);
	DDX_Check(pDX, IDC_CHK_REGIST_OLDTYPE, m_bRegistOldtype);
}


BEGIN_MESSAGE_MAP(FiletypeSettingsDlg, CDialog)
	ON_BN_CLICKED(IDC_RD_NOT_REGIST, &FiletypeSettingsDlg::OnBnClickedRdNotRegist)
	ON_BN_CLICKED(IDC_RD_REGIST, &FiletypeSettingsDlg::OnBnClickedRdRegist)
END_MESSAGE_MAP()


// FiletypeSettingsDlg メッセージ ハンドラー


BOOL FiletypeSettingsDlg::OnInitDialog()
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


void FiletypeSettingsDlg::OnBnClickedRdNotRegist()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	GetDlgItem(IDC_CHK_REGIST_OLDTYPE)->EnableWindow(FALSE);
}


void FiletypeSettingsDlg::OnBnClickedRdRegist()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	GetDlgItem(IDC_CHK_REGIST_OLDTYPE)->EnableWindow(TRUE);
}
