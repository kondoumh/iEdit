// SelFileDropDlg.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "iEdit.h"
#include "SelFileDropDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SelFileDropDlg �_�C�A���O


SelFileDropDlg::SelFileDropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SelFileDropDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SelFileDropDlg)
	m_nDropProc = -1;
	//}}AFX_DATA_INIT
}


void SelFileDropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SelFileDropDlg)
	DDX_Radio(pDX, IDC_RD_LINK, m_nDropProc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SelFileDropDlg, CDialog)
	//{{AFX_MSG_MAP(SelFileDropDlg)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ��܂��͍폜���܂��B
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelFileDropDlg ���b�Z�[�W �n���h��
