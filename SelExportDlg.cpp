// SelExportDlg.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "iEdit.h"
#include "SelExportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SelExportDlg �_�C�A���O


SelExportDlg::SelExportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SelExportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SelExportDlg)
	m_bPrintText = FALSE;
	m_nTreeOp = -1;
	//}}AFX_DATA_INIT
	m_bShowChekPrintText = false;
}


void SelExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SelExportDlg)
	DDX_Control(pDX, IDC_CHK_PRINT_TEXT, m_chkPrintText);
	DDX_Check(pDX, IDC_CHK_PRINT_TEXT, m_bPrintText);
	DDX_Radio(pDX, IDC_RD_TREE, m_nTreeOp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SelExportDlg, CDialog)
	//{{AFX_MSG_MAP(SelExportDlg)
	ON_BN_CLICKED(IDC_CHK_PRINT_TEXT, OnChkPrintText)
	ON_BN_CLICKED(IDC_RD_TREE, OnRdTree1)
	ON_BN_CLICKED(IDC_RD_TREE2, OnRdTree2)
	ON_BN_CLICKED(IDC_RD_TREE3, OnRdTree3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelExportDlg ���b�Z�[�W �n���h��

void SelExportDlg::OnChkPrintText() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_bPrintText = m_chkPrintText.GetCheck();
}

BOOL SelExportDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: ���̈ʒu�ɏ������̕⑫������ǉ����Ă�������
	if (!m_bShowChekPrintText) {
		m_chkPrintText.EnableWindow(FALSE);
	}
	
	return TRUE;  // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
	              // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}

void SelExportDlg::OnRdTree1() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	
}

void SelExportDlg::OnRdTree2() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	
}

void SelExportDlg::OnRdTree3() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	
}
