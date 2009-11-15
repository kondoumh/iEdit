// InpcnDlg.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "iEdit.h"
#include "InpcnDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_SETCHARDATA WM_USER + 12

/////////////////////////////////////////////////////////////////////////////
// CInpcnDlg �_�C�A���O


CInpcnDlg::CInpcnDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInpcnDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInpcnDlg)
	m_strcn = _T("");
	//}}AFX_DATA_INIT
}


void CInpcnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInpcnDlg)
	DDX_Control(pDX, IDC_EDIT1, m_editName);
	DDX_Text(pDX, IDC_EDIT1, m_strcn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInpcnDlg, CDialog)
	//{{AFX_MSG_MAP(CInpcnDlg)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETCHARDATA, OnSetCharData)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInpcnDlg ���b�Z�[�W �n���h��

BOOL CInpcnDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	GetWindowRect(&iniRect);
	if (m_iniPt.x != -1 && m_iniPt.y != -1) {
		MoveWindow(m_iniPt.x, m_iniPt.y, iniRect.Width(), iniRect.Height(), TRUE);
	}
	
	// TODO: ���̈ʒu�ɏ������̕⑫������ǉ����Ă�������
	if (m_strcn == _T("")) {
		m_editName.SetWindowText(_T("�V�����m�[�h"));
	} else if (m_strcn == _T("�m�[�h")) {
		m_editName.SetWindowText(_T("�m�[�h"));
	} else {
		PostMessage(WM_SETCHARDATA, 0);
	}
	return TRUE;  // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
	              // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}

void CInpcnDlg::OnClose() 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������
	MoveWindow(iniRect.left, iniRect.top, iniRect.Width(), iniRect.Height(), TRUE);
	CDialog::OnClose();
}

LRESULT CInpcnDlg::OnSetCharData(UINT wParam, LONG lParam)
{
	m_editName.ReplaceSel(m_strcn);
	return 0;
}
