// OptionPageForLink.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "OptionPageForLink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OptionPageForLink ダイアログ

OptionPageForLink::OptionPageForLink(CWnd* pParent /*=NULL*/)
	: CDialog(OptionPageForLink::IDD, pParent)
{
	//{{AFX_DATA_INIT(OptionPageForLink)
	m_bSetStrength = FALSE;
	//}}AFX_DATA_INIT
}


void OptionPageForLink::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OptionPageForLink)
	DDX_Control(pDX, IDC_STRN, m_strn);
	DDX_Control(pDX, IDC_SLID, m_Slider);
	DDX_Control(pDX, IDC_CHK_STRENGTH, m_chkSetStreangth);
	DDX_Control(pDX, IDC_CMB_LINE, m_cmbLine);
	DDX_Check(pDX, IDC_CHK_STRENGTH, m_bSetStrength);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OptionPageForLink, CDialog)
	//{{AFX_MSG_MAP(OptionPageForLink)
	ON_BN_CLICKED(IDC_BTN_COLOR, OnBtnColor)
	ON_BN_CLICKED(IDC_BTN_FONT, OnBtnFont)
	ON_CBN_SELCHANGE(IDC_CMB_LINE, OnSelchangeCmbLine)
	ON_BN_CLICKED(IDC_CHK_STRENGTH, OnChkStrength)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLID, OnReleasedcaptureSlid)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void OptionPageForLink::OnCancel()
{
	return;
	CDialog::OnCancel();
}

BOOL OptionPageForLink::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (styleLine == PS_DOT) {
		m_cmbLine.SetCurSel(5);
	}
	else {
		if (lineWidth == 0) {
			m_cmbLine.SetCurSel(0);
		}
		else {
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
	mes.Format(_T("%.1f"), (double)m_Slider.GetPos() / 10.0);
	m_strn.SetWindowText(mes);

	return TRUE;
}

void OptionPageForLink::OnBtnColor()
{
	CColorDialog dlg(colorLine);
	if (dlg.DoModal() != IDOK) return;
	colorLine = dlg.GetColor();
}

void OptionPageForLink::OnBtnFont()
{
	CFontDialog dlg(&lf);
	if (dlg.DoModal() != IDOK) return;
}

void OptionPageForLink::OnSelchangeCmbLine()
{
	int index = m_cmbLine.GetCurSel();
	if (index == 0 || index == 5) {
		lineWidth = 0;
	}
	else {
		lineWidth = index + 1;
	}
	if (index < 5) {
		styleLine = PS_SOLID;
	}
	else if (index == 5) {
		styleLine = PS_DOT;
	}
}

void OptionPageForLink::OnChkStrength()
{
	m_bSetStrength = m_chkSetStreangth.GetCheck();
}

void OptionPageForLink::OnReleasedcaptureSlid(NMHDR* pNMHDR, LRESULT* pResult)
{
	CString mes;
	strength = m_Slider.GetPos();
	mes.Format(_T("%.1f"), (double)strength / 10.0);
	m_strn.SetWindowText(mes);
	*pResult = 0;
}

void OptionPageForLink::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CString mes;
	strength = m_Slider.GetPos();
	mes.Format(_T("%.1f"), (double)strength / 10.0);
	m_strn.SetWindowText(mes);

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
