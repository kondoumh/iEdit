// OnProcDlg.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "iEdit.h"
#include "OnProcDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COnProcDlg �_�C�A���O


COnProcDlg::COnProcDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COnProcDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COnProcDlg)
	//}}AFX_DATA_INIT
}


void COnProcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COnProcDlg)
	DDX_Control(pDX, IDC_PROC_NAME, m_ProcName);
	DDX_Control(pDX, IDC_PROGPROC, m_ProgProc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COnProcDlg, CDialog)
	//{{AFX_MSG_MAP(COnProcDlg)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ��܂��͍폜���܂��B
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COnProcDlg ���b�Z�[�W �n���h��
