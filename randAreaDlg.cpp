// randAreaDlg.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "iEdit.h"
#include "randAreaDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// randAreaDlg �_�C�A���O


randAreaDlg::randAreaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(randAreaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(randAreaDlg)
	m_height = 0;
	m_width = 0;
	//}}AFX_DATA_INIT
}


void randAreaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(randAreaDlg)
	DDX_Text(pDX, IDC_HEIGHT, m_height);
	DDV_MinMaxInt(pDX, m_height, 100, 300000);
	DDX_Text(pDX, IDC_WIDTH, m_width);
	DDV_MinMaxInt(pDX, m_width, 100, 300000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(randAreaDlg, CDialog)
	//{{AFX_MSG_MAP(randAreaDlg)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ��܂��͍폜���܂��B
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// randAreaDlg ���b�Z�[�W �n���h��
