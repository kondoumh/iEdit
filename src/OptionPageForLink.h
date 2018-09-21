/////////////////////////////////////////////////////////////////////////////
// OptionPageForLink ダイアログ

#pragma once

class OptionPageForLink : public CDialog
{
public:
	int strength;
	int lineWidth;
	int styleLine;
	COLORREF colorLine;
	LOGFONT lf;
	OptionPageForLink(CWnd* pParent = NULL);

	enum { IDD = IDD_PG_LINK };
	CStatic	m_strn;
	CSliderCtrl	m_Slider;
	CButton	m_chkSetStreangth;
	CComboBox	m_cmbLine;
	BOOL	m_bSetStrength;

protected:

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnCancel();
	virtual BOOL OnInitDialog();

	afx_msg void OnBtnColor();
	afx_msg void OnBtnFont();
	afx_msg void OnSelchangeCmbLine();
	afx_msg void OnChkStrength();
	afx_msg void OnReleasedcaptureSlid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()
};
