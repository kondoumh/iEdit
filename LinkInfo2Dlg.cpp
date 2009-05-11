// LinkInfo2Dlg.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "iEdit.h"
#include "LinkInfo2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// LinkInfo2Dlg �_�C�A���O


LinkInfo2Dlg::LinkInfo2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(LinkInfo2Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(LinkInfo2Dlg)
	//}}AFX_DATA_INIT
}


void LinkInfo2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(LinkInfo2Dlg)
	DDX_Control(pDX, IDC_EDIT2, m_Comment);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_ORG, m_Org);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(LinkInfo2Dlg, CDialog)
	//{{AFX_MSG_MAP(LinkInfo2Dlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LinkInfo2Dlg ���b�Z�[�W �n���h��

void LinkInfo2Dlg::OnBrowse() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	char szFilters[] = "�S�Ẵt�@�C�� (*.*)|*.*|";
	CFileDialog dlg(TRUE, "*.*", NULL, OFN_HIDEREADONLY, szFilters, NULL);
	if (dlg.DoModal() != IDOK) return;
	m_edit.SetSel(0, -1);
	m_edit.ReplaceSel(dlg.GetPathName());
}

void LinkInfo2Dlg::OnOK() 
{
	// TODO: ���̈ʒu�ɂ��̑��̌��ؗp�̃R�[�h��ǉ����Ă�������
	m_edit.GetWindowText(strPath);
	m_Comment.GetWindowText(strComment);
	if (strComment.GetLength() > 260) {
		strComment = strComment.Left(260);
	}
	if (strPath == "") {
		MessageBox("�t�@�C�����܂��́AURL����͂��Ă�������");
		return;
	}
	CDialog::OnOK();
}

BOOL LinkInfo2Dlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: ���̈ʒu�ɏ������̕⑫������ǉ����Ă�������
	m_Org.SetWindowText(strOrg);
	m_Comment.ReplaceSel(strComment);
	m_edit.ReplaceSel(strPath);
	return TRUE;  // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
	              // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}
