// setFoldUpDlg.cpp : �C���v�������e�[�V���� �t�@�C��
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
// setFoldUpDlg �_�C�A���O


setFoldUpDlg::setFoldUpDlg(CWnd* pParent /*=NULL*/)
	: CDialog(setFoldUpDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(setFoldUpDlg)
	m_level = 0;
	//}}AFX_DATA_INIT
}


void setFoldUpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(setFoldUpDlg)
	DDX_Control(pDX, IDC_SPIN_LEVEL, m_spin);
	DDX_Text(pDX, IDC_EDITLEVEL, m_level);
	DDV_MinMaxInt(pDX, m_level, 1, 100000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(setFoldUpDlg, CDialog)
	//{{AFX_MSG_MAP(setFoldUpDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// setFoldUpDlg ���b�Z�[�W �n���h��

BOOL setFoldUpDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: ���̈ʒu�ɏ������̕⑫������ǉ����Ă�������
	m_spin.SetRange(1, 10000);
	return TRUE;  // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
	              // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}
