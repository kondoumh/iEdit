/////////////////////////////////////////////////////////////////////////////
// OptionPageForFrame ダイアログ

#pragma once

class OptionPageForFrame : public CDialog
{
public:
	CFont fntText;
	CFont fntLink;
	CFont fntOutline;
	COLORREF m_colorInsrtMrk;
	COLORREF m_colorEditBG;
	COLORREF m_colorEditFor;
	COLORREF m_colorOLBG;
	COLORREF m_colorOLFor;
	COLORREF m_colorLNBG;
	COLORREF m_colorLNFor;
	COLORREF m_colorNWBG;
	LOGFONT lfText;
	LOGFONT lfLink;
	LOGFONT lfOutline;
	OptionPageForFrame(CWnd* pParent = NULL);

	enum { IDD = IDD_PG_FRAME };
	CButton	m_ChkSaveBarState;
	CButton	m_chkSaveFrame;
	BOOL	m_bSaveFrame;
	BOOL	m_bSaveBarState;

protected:

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnCancel();

	afx_msg void OnBtnLink();
	afx_msg void OnBtnOutline();
	afx_msg void OnBtnText();
	afx_msg void OnChkSaveBarState();
	afx_msg void OnChkSaveFrame();
	afx_msg void OnPaint();
	afx_msg void OnBtnFclrOl();
	afx_msg void OnBtnBclrOl();
	afx_msg void OnBtnFclrLn();
	afx_msg void OnBtnBclrLn();
	afx_msg void OnBtnBclrNw();
	afx_msg void OnBtnFclrTx();
	afx_msg void OnBtnBclrTx();
	afx_msg void OnBtnInsertmark();
	DECLARE_MESSAGE_MAP()

private:
	void PreviewNetView(CDC* pDC);
	void PreviewLinkView(CDC* pDC);
	void PreviewOutlineView(CDC* pDC);
	void PreviewEditorView(CDC* pDC);
	void UpdateFont(LOGFONT* plf, CFont& font);
	void PreviewFont(CDC *pDC, CRect& rc, CFont& font, COLORREF bgColor, COLORREF fontColor);
};
