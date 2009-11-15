// PageLink.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "iEdit.h"
#include "PageLink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PageLink �_�C�A���O


PageLink::PageLink(CWnd* pParent /*=NULL*/)
	: CDialog(PageLink::IDD, pParent)
{
	//{{AFX_DATA_INIT(PageLink)
	m_bSetStrength = FALSE;
	//}}AFX_DATA_INIT
}


void PageLink::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PageLink)
	DDX_Control(pDX, IDC_STRN, m_strn);
	DDX_Control(pDX, IDC_SLID, m_Slider);
	DDX_Control(pDX, IDC_CHK_STRENGTH, m_chkSetStreangth);
	DDX_Control(pDX, IDC_CMB_LINE, m_cmbLine);
	DDX_Check(pDX, IDC_CHK_STRENGTH, m_bSetStrength);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PageLink, CDialog)
	//{{AFX_MSG_MAP(PageLink)
	ON_BN_CLICKED(IDC_BTN_COLOR, OnBtnColor)
	ON_BN_CLICKED(IDC_BTN_FONT, OnBtnFont)
	ON_CBN_SELCHANGE(IDC_CMB_LINE, OnSelchangeCmbLine)
	ON_BN_CLICKED(IDC_CHK_STRENGTH, OnChkStrength)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLID, OnReleasedcaptureSlid)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PageLink ���b�Z�[�W �n���h��

void PageLink::OnCancel() 
{
	// TODO: ���̈ʒu�ɓ��ʂȌ㏈����ǉ����Ă��������B
	return;
	CDialog::OnCancel();
}

BOOL PageLink::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: ���̈ʒu�ɏ������̕⑫������ǉ����Ă�������
	if (styleLine == PS_DOT) {
		m_cmbLine.SetCurSel(5);
	} else {
		if (lineWidth == 0) {
			m_cmbLine.SetCurSel(0);
		} else {
			m_cmbLine.SetCurSel(lineWidth - 1);
		}
	}
	m_Slider.SetTic(10);
	m_Slider.SetTicFreq(10);
	m_Slider.SetPageSize(10);
	m_Slider.SetLineSize(1);
	m_Slider.SetRangeMax(100, TRUE);
	m_Slider.SetRangeMin(0, TRUE);
	m_Slider.SetPos(strength);
	CString mes;
	mes.Format(_T("%.1f"), (double)m_Slider.GetPos()/10.0);
	m_strn.SetWindowText(mes);
	
	return TRUE;  // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
	              // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}

void PageLink::OnBtnColor() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	CColorDialog dlg(colorLine);
	if (dlg.DoModal() != IDOK) return;
	colorLine = dlg.GetColor();
}

void PageLink::OnBtnFont() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	CFontDialog dlg(&lf);
	dlg.m_cf.Flags |= CF_SELECTSCRIPT;
	if (dlg.DoModal() != IDOK) return;
}

void PageLink::OnSelchangeCmbLine() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	int index = m_cmbLine.GetCurSel();
	if (index == 0 || index == 5) {
		lineWidth = 0;
	} else {
		lineWidth = index+1;
	}
	if (index < 5) {
		styleLine = PS_SOLID;
	} else if (index == 5) {
		styleLine = PS_DOT;
	}

}

void PageLink::OnChkStrength() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_bSetStrength = m_chkSetStreangth.GetCheck();
}

void PageLink::OnReleasedcaptureSlid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	CString mes;
	strength = m_Slider.GetPos();
	mes.Format(_T("%.1f"), (double)strength/10.0);
	m_strn.SetWindowText(mes);
	*pResult = 0;
}

void PageLink::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������
	CString mes;
	strength = m_Slider.GetPos();
	mes.Format(_T("%.1f"), (double)strength/10.0);
	m_strn.SetWindowText(mes);
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
