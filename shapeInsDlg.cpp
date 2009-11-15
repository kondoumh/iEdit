// shapeInsDlg.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "iEdit.h"
#include "shapeInsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// shapeInsDlg �_�C�A���O


shapeInsDlg::shapeInsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(shapeInsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(shapeInsDlg)
	m_strPath = _T("");
	//}}AFX_DATA_INIT
}


void shapeInsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(shapeInsDlg)
	DDX_Control(pDX, IDC_EDPATH, m_edPath);
	DDX_Control(pDX, IDC_SHAPE, m_iShape);
	DDX_Control(pDX, IDC_BTN_BROWSE, m_BtnBrowse);
	DDX_Text(pDX, IDC_EDPATH, m_strPath);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(shapeInsDlg, CDialog)
	//{{AFX_MSG_MAP(shapeInsDlg)
	ON_BN_CLICKED(IDC_BTN_BROWSE, OnBtnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// shapeInsDlg ���b�Z�[�W �n���h��

void shapeInsDlg::OnBtnBrowse() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	CString empath;
	WCHAR szFilters[] = _T("�g�����^�t�@�C�� (*.emf)|*.emf|�S�Ẵt�@�C�� (*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("emf"), empath, OFN_HIDEREADONLY, szFilters, this);
	if (dlg.DoModal() != IDOK) return;
	CString infileName = dlg.GetPathName();
	if (infileName != _T("")) {
		HENHMETAFILE hm = ::GetEnhMetaFile(infileName);
		if (hm != NULL) {
			m_iShape.SetEnhMetaFile(hm);
			m_strPath = infileName;
			m_edPath.SetWindowText(m_strPath);
		} else {
			AfxMessageBox(_T("�L���ȃ��^�t�@�C���`���ł͂���܂���"));
		}
	}
}

void shapeInsDlg::OnOK() 
{
	// TODO: ���̈ʒu�ɂ��̑��̌��ؗp�̃R�[�h��ǉ����Ă�������
	if (m_strPath == _T("")) {
		AfxMessageBox(_T("���^�t�@�C�����w�肳��Ă��܂���"));
		return;
	}
	CDialog::OnOK();
}
