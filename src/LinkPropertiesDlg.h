/////////////////////////////////////////////////////////////////////////////
// LinkPropertiesDlg ダイアログ

#pragma once

class LinkPropertiesDlg : public CDialog
{
public:
	int lineWidth;
	int styleLine;
	COLORREF colorLine;
	LOGFONT lf;
	int styleArrow;
	CString strTo;
	CString strFrom;
	CString strComment;
	LinkPropertiesDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_LINKINFO };
	CComboBox	m_cmbLineStyle;
	CStatic	m_LabelTo;
	CStatic	m_LabelFrom;
	CEdit	m_edit;
	CComboBox	m_combo;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCombo();
	virtual void OnOK();
	afx_msg void OnBtnColor();
	afx_msg void OnBtnFont();
	afx_msg void OnSelchangeCmbLine();
	DECLARE_MESSAGE_MAP()
};
