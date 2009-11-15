// PageOther.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "iEdit.h"
#include "PageOther.h"
#include "SetMFSizeDlg.h"
#include "MfSizer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGS_OTHER _T("Settings")

/////////////////////////////////////////////////////////////////////////////
// PageOther �_�C�A���O


PageOther::PageOther(CWnd* pParent /*=NULL*/)
	: CDialog(PageOther::IDD, pParent)
{
	//{{AFX_DATA_INIT(PageOther)
	m_tabSelect = -1;
	m_bShowHS = FALSE;
	m_bInheritParent = FALSE;
	m_bInheritSibling = FALSE;
	m_bAccelmove = FALSE;
	m_bDrawUnderLine = FALSE;
	m_bSetStylesheet = FALSE;
	m_strStylesheet = _T("");
	//}}AFX_DATA_INIT
}


void PageOther::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PageOther)
	DDX_Control(pDX, IDC_CHK_SET_STYLESHEET, m_chkStylesheet);
	DDX_Control(pDX, IDC_EDIT_STYLESHEET, m_edStylesheet);
	DDX_Control(pDX, IDC_CHK_DRW_UNDRLN, m_chkDrawUnderLine);
	DDX_Control(pDX, IDC_CHK_ACCEL, m_chkAccelMove);
	DDX_Control(pDX, IDC_CHK_INHERIT_SIBLING, m_chkInheritSibling);
	DDX_Control(pDX, IDC_CHK_INHERIT_PARENT, m_chkInheritParent);
	DDX_Control(pDX, IDC_CHK_SHOW_HS, m_chkShowHS);
	DDX_Radio(pDX, IDC_RD_TAB1, m_tabSelect);
	DDX_Check(pDX, IDC_CHK_SHOW_HS, m_bShowHS);
	DDX_Check(pDX, IDC_CHK_INHERIT_PARENT, m_bInheritParent);
	DDX_Check(pDX, IDC_CHK_INHERIT_SIBLING, m_bInheritSibling);
	DDX_Check(pDX, IDC_CHK_ACCEL, m_bAccelmove);
	DDX_Check(pDX, IDC_CHK_DRW_UNDRLN, m_bDrawUnderLine);
	DDX_Check(pDX, IDC_CHK_SET_STYLESHEET, m_bSetStylesheet);
	DDX_Text(pDX, IDC_EDIT_STYLESHEET, m_strStylesheet);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PageOther, CDialog)
	//{{AFX_MSG_MAP(PageOther)
	ON_BN_CLICKED(IDC_RD_TAB1, OnRdTab1)
	ON_BN_CLICKED(IDC_RD_TAB2, OnRdTab2)
	ON_BN_CLICKED(IDC_RD_TAB3, OnRdTab3)
	ON_BN_CLICKED(IDC_CHK_SHOW_HS, OnChkShowHs)
	ON_BN_CLICKED(IDC_CHK_INHERIT_PARENT, OnChkInheritParent)
	ON_BN_CLICKED(IDC_CHK_INHERIT_SIBLING, OnChkInheritSibling)
	ON_BN_CLICKED(IDC_CHK_ACCEL, OnChkAccel)
	ON_BN_CLICKED(IDC_BTN_SETMFSIZE, OnBtnSetmfsize)
	ON_BN_CLICKED(IDC_CHK_DRW_UNDRLN, OnChkDrwUndrln)
	ON_BN_CLICKED(IDC_CHK_SET_STYLESHEET, OnChkSetStylesheet)
	ON_EN_CHANGE(IDC_EDIT_STYLESHEET, OnChangeEditStylesheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PageOther ���b�Z�[�W �n���h��

void PageOther::OnCancel() 
{
	// TODO: ���̈ʒu�ɓ��ʂȌ㏈����ǉ����Ă��������B
	return;
	CDialog::OnCancel();
}

BOOL PageOther::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: ���̈ʒu�ɏ������̕⑫������ǉ����Ă�������
	m_edStylesheet.EnableWindow(m_bSetStylesheet);
	
	return TRUE;  // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
	              // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}

void PageOther::OnRdTab1() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_tabSelect = 0;
}

void PageOther::OnRdTab2() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_tabSelect = 1;
}

void PageOther::OnRdTab3() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_tabSelect = 2;
}

void PageOther::OnChkShowHs() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_bShowHS = m_chkShowHS.GetCheck();
}

void PageOther::OnChkInheritParent() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_bInheritParent = m_chkInheritParent.GetCheck();
}

void PageOther::OnChkInheritSibling() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_bInheritSibling = m_chkInheritSibling.GetCheck();
}

void PageOther::OnChkAccel() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_bAccelmove = m_chkAccelMove.GetCheck();
}

void PageOther::OnBtnSetmfsize() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	SetMFSizeDlg dlg;
	double mfWidth = AfxGetApp()->GetProfileInt(REGS_OTHER, _T("MF rWidth"), 0)/10.0;
	double mfHeight = AfxGetApp()->GetProfileInt(REGS_OTHER, _T("MF rHeight"), 0)/10.0;
	
	
	if (mfWidth != 0 && mfHeight != 0) {
		dlg.m_rx = mfWidth;
		dlg.m_ry = mfHeight;
	} else {
		CSize szMF = MfSizer::getMFSize();
		dlg.m_rx = ((double)szMF.cx)/10.0;
		dlg.m_ry = ((double)szMF.cy)/10.0;;
	}
	
	if (dlg.DoModal() == IDOK) {
		AfxGetApp()->WriteProfileInt(REGS_OTHER, _T("MF rWidth"), (int)(dlg.m_rx*10));
		AfxGetApp()->WriteProfileInt(REGS_OTHER, _T("MF rHeight"), (int)(dlg.m_ry*10));
		MessageBox(_T("���̐ݒ�͍ċN����ɔ��f����܂�"));
	}
}

void PageOther::OnChkDrwUndrln() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_bDrawUnderLine = m_chkDrawUnderLine.GetCheck();
}

void PageOther::OnChkSetStylesheet() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_bSetStylesheet = m_chkStylesheet.GetCheck();
	m_edStylesheet.EnableWindow(m_bSetStylesheet);
}

void PageOther::OnChangeEditStylesheet() 
{
	// TODO: ���ꂪ RICHEDIT �R���g���[���̏ꍇ�A�R���g���[���́A lParam �}�X�N
	// ���ł̘_���a�� ENM_CHANGE �t���O�t���� CRichEditCrtl().SetEventMask()
	// ���b�Z�[�W���R���g���[���֑��邽�߂� CDialog::OnInitDialog() �֐����I�[�o�[
	// ���C�h���Ȃ����肱�̒ʒm�𑗂�܂���B
	
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_edStylesheet.GetWindowText(m_strStylesheet);
	
}
