/////////////////////////////////////////////////////////////////////////////
// NodePropertiesDlg ダイアログ

#pragma once

class NodePropertiesDlg : public CDialog
{
public:
	struct Margins
	{
		int l;
		int r;
		int t;
		int b;
	} margins;

	int vert;
	int horiz;
	BOOL bOldBynary;
	BOOL bMultiLine;
	int styleText;
	int styleLine;
	int lineWidth;
	COLORREF colorFill;
	COLORREF colorLine;
	COLORREF colorFont;
	LOGFONT lf;
	NodePropertiesDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_PRPNODE };
	CString m_strLabel;
	CComboBox	m_cmbHoriz;
	CComboBox	m_cmbVert;
	CButton	m_chkBoxNoBrs;
	CButton	m_BtnBrsColor;
	CComboBox	m_CombTLine;
	CEdit	m_editLabel;
	int		m_shape;
	int		m_TLine;
	BOOL	m_bNoBrush;

protected:

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	virtual BOOL OnInitDialog();

	afx_msg void OnSelchangeComboline();
	afx_msg void OnBtnLineColor();
	afx_msg void OnBtnBrs();
	afx_msg void OnBtnFont();
	afx_msg void OnRadioTline1();
	afx_msg void OnRadioTline2();
	afx_msg void OnRadioShape();
	afx_msg void OnRadioShape2();
	afx_msg void OnChkNoBrs();
	afx_msg void OnRadioTline3();
	afx_msg void OnSelchangeComboHoriz();
	afx_msg void OnSelchangeComboVert();
	afx_msg void OnRadioShape3();
	afx_msg void OnBnClickedBtnSetMargin();
	DECLARE_MESSAGE_MAP()
};
