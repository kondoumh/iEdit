#if !defined(AFX_PAGELINK_H__79C43545_7FE6_11D3_9860_006097789FE5__INCLUDED_)
#define AFX_PAGELINK_H__79C43545_7FE6_11D3_9860_006097789FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionPageForLink.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// OptionPageForLink ダイアログ

class OptionPageForLink : public CDialog
{
public:
	int strength;
	int lineWidth;
	int styleLine;
	COLORREF colorLine;
	LOGFONT lf;
	OptionPageForLink(CWnd* pParent = NULL);

	//{{AFX_DATA(OptionPageForLink)
	enum { IDD = IDD_PG_LINK };
	CStatic	m_strn;
	CSliderCtrl	m_Slider;
	CButton	m_chkSetStreangth;
	CComboBox	m_cmbLine;
	BOOL	m_bSetStrength;
	//}}AFX_DATA

protected:

	//{{AFX_VIRTUAL(OptionPageForLink)
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(OptionPageForLink)
	afx_msg void OnBtnColor();
	afx_msg void OnBtnFont();
	afx_msg void OnSelchangeCmbLine();
	afx_msg void OnChkStrength();
	afx_msg void OnReleasedcaptureSlid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PAGELINK_H__79C43545_7FE6_11D3_9860_006097789FE5__INCLUDED_)
