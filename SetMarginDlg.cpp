// SetMarginDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "iEdit.h"
#include "SetMarginDlg.h"


// SetMarginDlg �_�C�A���O

IMPLEMENT_DYNAMIC(SetMarginDlg, CDialog)

SetMarginDlg::SetMarginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SetMarginDlg::IDD, pParent)
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
END_MESSAGE_MAP()


// SetMarginDlg ���b�Z�[�W �n���h��

BOOL SetMarginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �����ɏ�������ǉ����Ă�������
	m_spinLeft.SetRange32(0, 50);
	m_spinLeft.SetPos32(0);
	m_spinRight.SetRange32(0, 50);
	m_spinRight.SetPos32(0);
	m_spinTop.SetRange32(0, 50);
	m_spinTop.SetPos32(0);
	m_spinBottom.SetRange32(0, 50);
	m_spinBottom.SetPos32(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
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
