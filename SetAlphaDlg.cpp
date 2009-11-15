// SetAlphaDlg.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "iEdit.h"
#include "SetAlphaDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SetAlphaDlg �_�C�A���O


SetAlphaDlg::SetAlphaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SetAlphaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SetAlphaDlg)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ��܂��͍폜���܂��B
	//}}AFX_DATA_INIT
}


void SetAlphaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SetAlphaDlg)
	DDX_Control(pDX, IDC_LB_LEVEL, m_lbLevel);
	DDX_Control(pDX, IDC_SLIDER_ALPHA, m_sldrAlpha);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SetAlphaDlg, CDialog)
	//{{AFX_MSG_MAP(SetAlphaDlg)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SetAlphaDlg ���b�Z�[�W �n���h��

BOOL SetAlphaDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: ���̈ʒu�ɌŗL�̏�����ǉ����邩�A�܂��͊�{�N���X���Ăяo���Ă�������
	m_pParent = pParentWnd;
	return CDialog::Create(IDD, pParentWnd);
}

BOOL SetAlphaDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: ���̈ʒu�ɏ������̕⑫������ǉ����Ă�������
	m_sldrAlpha.SetRange(50, 255);
	m_sldrAlpha.SetPos(m_nLevel);
	CString s; s.Format(_T("%d"), m_sldrAlpha.GetPos());
	m_lbLevel.SetWindowText(s);
	return TRUE;  // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
	              // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}

void SetAlphaDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������
	CString s; s.Format(_T("%d"), m_sldrAlpha.GetPos());
	m_lbLevel.SetWindowText(s);
	m_pParent->PostMessage(WM_SLIDEALPHA, 0, m_sldrAlpha.GetPos());
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
