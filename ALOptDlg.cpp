// ALOptDlg.cpp : �C���v�������e�[�V���� �t�@�C��
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
// ALOptDlg �_�C�A���O


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
// ALOptDlg ���b�Z�[�W �n���h��

BOOL ALOptDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: ���̈ʒu�ɏ������̕⑫������ǉ����Ă�������
	m_spin.SetRange(0, 10000);
	if (m_showMode == 0) {
		m_spin.EnableWindow(FALSE);
		m_editCY.EnableWindow(FALSE);
	}

	return TRUE;  // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
	              // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}

void ALOptDlg::OnOK() 
{
	// TODO: ���̈ʒu�ɂ��̑��̌��ؗp�̃R�[�h��ǉ����Ă�������
	m_cycle = m_spin.GetPos();
	CDialog::OnOK();
}

void ALOptDlg::OnRdAni() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_spin.EnableWindow(FALSE);
	m_editCY.EnableWindow(FALSE);
	m_showMode = 0;
}

void ALOptDlg::OnRdCy() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_spin.EnableWindow(TRUE);
	m_editCY.EnableWindow(TRUE);
	m_showMode = 1;
}
