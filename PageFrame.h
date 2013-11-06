#if !defined(AFX_PAGEFRAME_H__79C43546_7FE6_11D3_9860_006097789FE5__INCLUDED_)
#define AFX_PAGEFRAME_H__79C43546_7FE6_11D3_9860_006097789FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageFrame.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// PageFrame ダイアログ

class PageFrame : public CDialog
{
// コンストラクション
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
	PageFrame(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(PageFrame)
	enum { IDD = IDD_PG_FRAME };
	CButton	m_ChkSaveBarState;
	CButton	m_chkSaveFrame;
	BOOL	m_bSaveFrame;
	BOOL	m_bSaveBarState;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(PageFrame)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	void drawNWPreView(CDC* pDC);
	void drawLNPreView(CDC* pDC);
	void drawOLPreView(CDC* pDC);
	void drawTextPreView(CDC* pDC);

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(PageFrame)
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
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_PAGEFRAME_H__79C43546_7FE6_11D3_9860_006097789FE5__INCLUDED_)
