// OptionPageForFrame.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "OptionPageForFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGS_FRAME _T("Frame Options")

/////////////////////////////////////////////////////////////////////////////
// OptionPageForFrame ダイアログ

OptionPageForFrame::OptionPageForFrame(CWnd* pParent /*=NULL*/)
	: CDialog(OptionPageForFrame::IDD, pParent)
{
	//{{AFX_DATA_INIT(OptionPageForFrame)
	m_bSaveFrame = FALSE;
	m_bSaveBarState = FALSE;
	//}}AFX_DATA_INIT
}

void OptionPageForFrame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OptionPageForFrame)
	DDX_Control(pDX, IDC_CHK_SAVE_BAR_STATE, m_ChkSaveBarState);
	DDX_Control(pDX, IDC_CHK_SAVE_FRAME, m_chkSaveFrame);
	DDX_Check(pDX, IDC_CHK_SAVE_FRAME, m_bSaveFrame);
	DDX_Check(pDX, IDC_CHK_SAVE_BAR_STATE, m_bSaveBarState);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(OptionPageForFrame, CDialog)
	//{{AFX_MSG_MAP(OptionPageForFrame)
	ON_BN_CLICKED(IDC_BTN_LINK, OnBtnLink)
	ON_BN_CLICKED(IDC_BTN_OUTLINE, OnBtnOutline)
	ON_BN_CLICKED(IDC_BTN_TEXT, OnBtnText)
	ON_BN_CLICKED(IDC_CHK_SAVE_BAR_STATE, OnChkSaveBarState)
	ON_BN_CLICKED(IDC_CHK_SAVE_FRAME, OnChkSaveFrame)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN__FCLR_OL, OnBtnFclrOl)
	ON_BN_CLICKED(IDC_BTN__BCLR_OL, OnBtnBclrOl)
	ON_BN_CLICKED(IDC_BTN_FCLR_LN, OnBtnFclrLn)
	ON_BN_CLICKED(IDC_BTN_BCLR_LN, OnBtnBclrLn)
	ON_BN_CLICKED(IDC_BTN_BCLR_NW, OnBtnBclrNw)
	ON_BN_CLICKED(IDC_BTN_FCLR_TX, OnBtnFclrTx)
	ON_BN_CLICKED(IDC_BTN_BCLR_TX, OnBtnBclrTx)
	ON_BN_CLICKED(IDC_BTN__INSERTMARK, OnBtnInsertmark)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void OptionPageForFrame::OnCancel()
{
	return;
	CDialog::OnCancel();
}

void OptionPageForFrame::OnBtnLink()
{
	UpdateFont(&lfLink, fntLink);
}

void OptionPageForFrame::OnBtnOutline()
{
	UpdateFont(&lfOutline, fntOutline);
}

void OptionPageForFrame::OnBtnText()
{
	UpdateFont(&lfText, fntText);
}

void OptionPageForFrame::UpdateFont(LOGFONT* plf, CFont& font)
{
	CFontDialog dlg(plf);
	if (dlg.DoModal() != IDOK) return;
	font.CreateFontIndirect(plf);
	Invalidate();
}

void OptionPageForFrame::OnChkSaveBarState()
{
	m_bSaveBarState = m_ChkSaveBarState.GetCheck();
}

void OptionPageForFrame::OnChkSaveFrame()
{
	m_bSaveFrame = m_chkSaveFrame.GetCheck();
}

void OptionPageForFrame::OnPaint()
{
	CPaintDC dc(this);

	PreviewOutlineView(&dc);
	PreviewLinkView(&dc);
	PreviewEditorView(&dc);
	PreviewNetView(&dc);
}

void OptionPageForFrame::PreviewOutlineView(CDC *pDC)
{
	CRect rc(200, 28, 310, 78);
	PreviewFont(pDC, rc, fntOutline, m_colorOLBG, m_colorOLFor);
}

void OptionPageForFrame::PreviewLinkView(CDC *pDC)
{
	CRect rc(200, 103, 310, 153);
	PreviewFont(pDC, rc, fntLink, m_colorLNBG, m_colorLNFor);
}

void OptionPageForFrame::PreviewEditorView(CDC *pDC)
{
	CRect rc(200, 180, 310, 230);
	PreviewFont(pDC, rc, fntText, m_colorEditBG, m_colorEditFor);
}

void OptionPageForFrame::PreviewFont(CDC *pDC, CRect& rc, CFont& font, COLORREF bgColor, COLORREF fontColor) {

	CBrush brs(bgColor);
	CBrush* brsOld = pDC->SelectObject(&brs);
	pDC->FillRect(rc, &brs);
	pDC->SelectObject(brsOld);

	pDC->MoveTo(rc.TopLeft());
	pDC->LineTo(CPoint(rc.right, rc.top));
	pDC->LineTo(rc.BottomRight());
	pDC->LineTo(CPoint(rc.left, rc.bottom));
	pDC->LineTo(rc.TopLeft());

	CFont* pOldFont = pDC->SelectObject(&font);
	COLORREF preColor = pDC->SetTextColor(fontColor);
	int oldBkMode = pDC->SetBkMode(TRANSPARENT);
	LOGFONT lf;
	font.GetLogFont(&lf);
	CString sSample;
	if (lf.lfCharSet == SHIFTJIS_CHARSET) {
		sSample = _T("A あ 亜");
	}
	else {
		sSample = _T("AaBbYyZz");
	}
	pDC->DrawText(sSample, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->SelectObject(pOldFont);
	pDC->SetTextColor(preColor);
	pDC->SetBkMode(oldBkMode);
}

void OptionPageForFrame::PreviewNetView(CDC *pDC)
{
	CRect rc(200, 255, 310, 280);

	CBrush brs(m_colorNWBG);
	CBrush* brsOld = pDC->SelectObject(&brs);
	pDC->FillRect(rc, &brs);
	pDC->SelectObject(brsOld);

	pDC->MoveTo(rc.TopLeft());
	pDC->LineTo(CPoint(rc.right, rc.top));
	pDC->LineTo(rc.BottomRight());
	pDC->LineTo(CPoint(rc.left, rc.bottom));
	pDC->LineTo(rc.TopLeft());
}

void OptionPageForFrame::OnBtnFclrOl()
{
	CColorDialog dlg(m_colorOLFor);
	if (dlg.DoModal() != IDOK) return;
	m_colorOLFor = dlg.GetColor();
	Invalidate();
}

void OptionPageForFrame::OnBtnBclrOl()
{
	CColorDialog dlg(m_colorOLBG);
	if (dlg.DoModal() != IDOK) return;
	m_colorOLBG = dlg.GetColor();
	Invalidate();
}

void OptionPageForFrame::OnBtnFclrLn()
{
	CColorDialog dlg(m_colorLNFor);
	if (dlg.DoModal() != IDOK) return;
	m_colorLNFor = dlg.GetColor();
	Invalidate();
}

void OptionPageForFrame::OnBtnBclrLn()
{
	CColorDialog dlg(m_colorLNBG);
	if (dlg.DoModal() != IDOK) return;
	m_colorLNBG = dlg.GetColor();
	Invalidate();
}

void OptionPageForFrame::OnBtnFclrTx()
{
	CColorDialog dlg(m_colorEditFor);
	if (dlg.DoModal() != IDOK) return;
	m_colorEditFor = dlg.GetColor();
	Invalidate();
}

void OptionPageForFrame::OnBtnBclrTx()
{
	CColorDialog dlg(m_colorEditBG);
	if (dlg.DoModal() != IDOK) return;
	m_colorEditBG = dlg.GetColor();
	Invalidate();
}

void OptionPageForFrame::OnBtnBclrNw()
{
	CColorDialog dlg(m_colorNWBG);
	if (dlg.DoModal() != IDOK) return;
	m_colorNWBG = dlg.GetColor();
	Invalidate();
}

void OptionPageForFrame::OnBtnInsertmark()
{
	CColorDialog dlg(m_colorInsrtMrk);
	if (dlg.DoModal() != IDOK) return;
	m_colorInsrtMrk = dlg.GetColor();
}
