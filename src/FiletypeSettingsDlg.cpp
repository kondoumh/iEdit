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


BOOL FiletypeSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_registFileType == 0) {
		GetDlgItem(IDC_CHK_REGIST_OLDTYPE)->EnableWindow(FALSE);
	} else {
		GetDlgItem(IDC_CHK_REGIST_OLDTYPE)->EnableWindow(TRUE);
	}
	return TRUE;
}


void FiletypeSettingsDlg::OnBnClickedRdNotRegist()
{
	GetDlgItem(IDC_CHK_REGIST_OLDTYPE)->EnableWindow(FALSE);
}


void FiletypeSettingsDlg::OnBnClickedRdRegist()
{
	GetDlgItem(IDC_CHK_REGIST_OLDTYPE)->EnableWindow(TRUE);
}
