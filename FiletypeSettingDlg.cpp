// FiletypeSettingDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "iEdit.h"
#include "FiletypeSettingDlg.h"
#include "afxdialogex.h"


// FiletypeSettingDlg �_�C�A���O

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


// FiletypeSettingDlg ���b�Z�[�W �n���h���[


BOOL FiletypeSettingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �����ɏ�������ǉ����Ă�������
	if (m_registFileType == 0) {
		GetDlgItem(IDC_CHK_REGIST_OLDTYPE)->EnableWindow(FALSE);
	} else {
		GetDlgItem(IDC_CHK_REGIST_OLDTYPE)->EnableWindow(TRUE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}


void FiletypeSettingDlg::OnBnClickedRdNotRegist()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	GetDlgItem(IDC_CHK_REGIST_OLDTYPE)->EnableWindow(FALSE);
}


void FiletypeSettingDlg::OnBnClickedRdRegist()
{
	// TODO: �����ɃR���g���[���ʒm�n���h���[ �R�[�h��ǉ����܂��B
	GetDlgItem(IDC_CHK_REGIST_OLDTYPE)->EnableWindow(TRUE);
}
