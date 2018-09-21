/////////////////////////////////////////////////////////////////////////////
// OptionPageForNode ダイアログ

#pragma once

class OptionPageForNode : public CDialog
{
public:
	struct Margins { int l; int r; int t; int b; } margins;
	BOOL m_bSyncOrder;
	BOOL m_bEnableGroup;
	int m_orderDirection;
	int vert;
	int horiz;
	BOOL bMultiLine;
	int styleText;
	int styleLine;
	int lineWidth;
	COLORREF colorFill;
	COLORREF colorLine;
	COLORREF colorFont;
	LOGFONT lf;
	OptionPageForNode(CWnd* pParent = NULL);

	enum { IDD = IDD_PG_NODE };
	CComboBox	m_cmbVert;
	CComboBox	m_cmbHoriz;
	CButton	m_chkNoBrs;
	CButton	m_BtnBrsColor;
	CComboBox	m_cmbLineStyle;
	int		m_rdTLine;
	int		m_rdShape;
	BOOL	m_bNoBrs;
	CButton m_chkSyncOrder;
	CButton m_chkEnableGroup;
	int m_rdAscending;
	int m_rdDescending;
	CButton m_btnAscending;
	CButton m_btnDescending;
	CButton m_ChkDisableNodeResize;
	BOOL m_bDisableNodeResize;
	BOOL m_bPriorSelectionDragging;
	CButton m_chkPriorSelectionDragging;
	//}}AFX_DATA


protected:

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnCancel();
	virtual BOOL OnInitDialog();

	afx_msg void OnBtnLineColor();
	afx_msg void OnBtnBrs();
	afx_msg void OnBtnFont();
	afx_msg void OnRadioTline1();
	afx_msg void OnRadioTline2();
	afx_msg void OnRadioShape();
	afx_msg void OnRadioShape2();
	afx_msg void OnChkNoBrs();
	afx_msg void OnSelchangeComboline();
	afx_msg void OnRadioTline3();
	afx_msg void OnSelchangeComboHoriz();
	afx_msg void OnSelchangeComboVert();
	afx_msg void OnRadioShape3();
	afx_msg void OnBnClickedChkSyncOrder();
	afx_msg void OnBnClickedChkEnableGrouping();
	afx_msg void OnBnClickedRadioAscending();
	afx_msg void OnBnClickedRadioDescending();
	afx_msg void OnBnClickedDisableNodeResize();
	afx_msg void OnBnClickedBtnSetMargin();
	afx_msg void OnBnClickedPriorSelectionDragging();

	DECLARE_MESSAGE_MAP()
};
