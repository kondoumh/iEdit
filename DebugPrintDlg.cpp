// DebugPrintDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "iEdit.h"
#include "DebugPrintDlg.h"


// DebugPrintDlg �_�C�A���O

IMPLEMENT_DYNAMIC(DebugPrintDlg, CDialog)

DebugPrintDlg::DebugPrintDlg(CWnd* pParent /*=NULL*/)
	: CDialog(DebugPrintDlg::IDD, pParent)
{

}

DebugPrintDlg::~DebugPrintDlg()
{
}

void DebugPrintDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PRINT, m_edConsole);
}

BOOL DebugPrintDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, 
						   DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, 
						   UINT nID, CCreateContext *pContext)
{
	m_pParent = pParentWnd;
	return CDialog::Create(IDD, pParentWnd);
}

void DebugPrintDlg::OnOK()
{
}

void DebugPrintDlg::OnCancel()
{
}

BEGIN_MESSAGE_MAP(DebugPrintDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &DebugPrintDlg::OnBnClickedBtnClear)
	ON_BN_CLICKED(IDC_BTN_COPY, &DebugPrintDlg::OnBnClickedBtnCopy)
END_MESSAGE_MAP()


// DebugPrintDlg ���b�Z�[�W �n���h��

void DebugPrintDlg::OnBnClickedBtnClear()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	m_edConsole.SetSel(0, -1);
	m_edConsole.Clear();
}

void DebugPrintDlg::OnBnClickedBtnCopy()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	m_edConsole.SetSel(0, -1);
	m_edConsole.Copy();
}
