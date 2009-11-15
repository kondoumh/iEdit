// PageLink.cpp : インプリメンテーション ファイル
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
// PageLink ダイアログ


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
// PageLink メッセージ ハンドラ

void PageLink::OnCancel() 
{
	// TODO: この位置に特別な後処理を追加してください。
	return;
	CDialog::OnCancel();
}

BOOL PageLink::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
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
	
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void PageLink::OnBtnColor() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CColorDialog dlg(colorLine);
	if (dlg.DoModal() != IDOK) return;
	colorLine = dlg.GetColor();
}

void PageLink::OnBtnFont() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CFontDialog dlg(&lf);
	dlg.m_cf.Flags |= CF_SELECTSCRIPT;
	if (dlg.DoModal() != IDOK) return;
}

void PageLink::OnSelchangeCmbLine() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
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
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_bSetStrength = m_chkSetStreangth.GetCheck();
}

void PageLink::OnReleasedcaptureSlid(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CString mes;
	strength = m_Slider.GetPos();
	mes.Format(_T("%.1f"), (double)strength/10.0);
	m_strn.SetWindowText(mes);
	*pResult = 0;
}

void PageLink::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	CString mes;
	strength = m_Slider.GetPos();
	mes.Format(_T("%.1f"), (double)strength/10.0);
	m_strn.SetWindowText(mes);
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
