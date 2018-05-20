#if !defined(AFX_PAGEFRAME_H__79C43546_7FE6_11D3_9860_006097789FE5__INCLUDED_)
#define AFX_PAGEFRAME_H__79C43546_7FE6_11D3_9860_006097789FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionPageForFrame.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// OptionPageForFrame ダイアログ

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

	//{{AFX_DATA(OptionPageForFrame)
	enum { IDD = IDD_PG_FRAME };
	CButton	m_ChkSaveBarState;
	CButton	m_chkSaveFrame;
	BOOL	m_bSaveFrame;
	BOOL	m_bSaveBarState;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(OptionPageForFrame)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	void drawNWPreView(CDC* pDC);
	void drawLNPreView(CDC* pDC);
	void drawOLPreView(CDC* pDC);
	void drawTextPreView(CDC* pDC);

	//{{AFX_MSG(OptionPageForFrame)
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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void updateFont(LOGFONT* plf, CFont& font);
	void drawFontPreview(CDC *pDC, CRect& rc, CFont& font, COLORREF bgColor, COLORREF fontColor);
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PAGEFRAME_H__79C43546_7FE6_11D3_9860_006097789FE5__INCLUDED_)
