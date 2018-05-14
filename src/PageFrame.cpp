// OptionPageForFrame.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "PageFrame.h"

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
// OptionPageForFrame メッセージ ハンドラ

void OptionPageForFrame::OnCancel() 
{
	// TODO: この位置に特別な後処理を追加してください。
	return;
	CDialog::OnCancel();
}

void OptionPageForFrame::OnBtnLink() 
{
	updateFont(&lfLink, fntLink);
}

void OptionPageForFrame::OnBtnOutline() 
{
	updateFont(&lfOutline, fntOutline);
}

void OptionPageForFrame::OnBtnText() 
{
	updateFont(&lfText, fntText);
}

void OptionPageForFrame::updateFont(LOGFONT* plf, CFont& font)
{
	CFontDialog dlg(plf);
	//dlg.m_cf.Flags |= CF_SELECTSCRIPT;
	if (dlg.DoModal() != IDOK) return;
	font.CreateFontIndirect(plf);
	Invalidate();
}

void OptionPageForFrame::OnChkSaveBarState() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_bSaveBarState = m_ChkSaveBarState.GetCheck();
}

void OptionPageForFrame::OnChkSaveFrame() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_bSaveFrame = m_chkSaveFrame.GetCheck();
}

void OptionPageForFrame::OnPaint() 
{
	CPaintDC dc(this); // 描画用のデバイス コンテキスト
	
	// TODO: この位置にメッセージ ハンドラ用のコードを追加してください
	drawOLPreView(&dc);
	drawLNPreView(&dc);
	drawTextPreView(&dc);
	drawNWPreView(&dc);
}

void OptionPageForFrame::drawOLPreView(CDC *pDC)
{
	CRect rc(200, 28, 310, 78);
	drawFontPreview(pDC, rc, fntOutline, m_colorOLBG, m_colorOLFor);
}

void OptionPageForFrame::drawLNPreView(CDC *pDC)
{
	CRect rc(200, 103, 310, 153);
	drawFontPreview(pDC, rc, fntLink, m_colorLNBG, m_colorLNFor);
}

void OptionPageForFrame::drawTextPreView(CDC *pDC)
{
	CRect rc(200, 180, 310, 230);
	drawFontPreview(pDC, rc, fntText, m_colorEditBG, m_colorEditFor);
}

void OptionPageForFrame::drawFontPreview(CDC *pDC, CRect& rc, CFont& font, COLORREF bgColor, COLORREF fontColor) {
	
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
	} else {
		sSample = _T("AaBbYyZz");
	}
	pDC->DrawText(sSample, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->SelectObject(pOldFont);
	pDC->SetTextColor(preColor);
	pDC->SetBkMode(oldBkMode);
}

void OptionPageForFrame::drawNWPreView(CDC *pDC)
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
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CColorDialog dlg(m_colorOLFor);
	if (dlg.DoModal() != IDOK) return;
	m_colorOLFor = dlg.GetColor();
	Invalidate();
}

void OptionPageForFrame::OnBtnBclrOl() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CColorDialog dlg(m_colorOLBG);
	if (dlg.DoModal() != IDOK) return;
	m_colorOLBG = dlg.GetColor();
	Invalidate();
}

void OptionPageForFrame::OnBtnFclrLn() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CColorDialog dlg(m_colorLNFor);
	if (dlg.DoModal() != IDOK) return;
	m_colorLNFor = dlg.GetColor();
	Invalidate();
}

void OptionPageForFrame::OnBtnBclrLn() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CColorDialog dlg(m_colorLNBG);
	if (dlg.DoModal() != IDOK) return;
	m_colorLNBG = dlg.GetColor();
	Invalidate();
}

void OptionPageForFrame::OnBtnFclrTx() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CColorDialog dlg(m_colorEditFor);
	if (dlg.DoModal() != IDOK) return;
	m_colorEditFor = dlg.GetColor();
	Invalidate();
}

void OptionPageForFrame::OnBtnBclrTx() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CColorDialog dlg(m_colorEditBG);
	if (dlg.DoModal() != IDOK) return;
	m_colorEditBG = dlg.GetColor();
	Invalidate();
}

void OptionPageForFrame::OnBtnBclrNw() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CColorDialog dlg(m_colorNWBG);
	if (dlg.DoModal() != IDOK) return;
	m_colorNWBG = dlg.GetColor();
	Invalidate();
}

void OptionPageForFrame::OnBtnInsertmark() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CColorDialog dlg(m_colorInsrtMrk);
	if (dlg.DoModal() != IDOK) return;
	m_colorInsrtMrk = dlg.GetColor();
}
