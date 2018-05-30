#if !defined(AFX_LINKINFODLG_H__6557B4B6_290A_11D3_80A7_00A0C9B72FDD__INCLUDED_)
#define AFX_LINKINFODLG_H__6557B4B6_290A_11D3_80A7_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LinkPropertiesDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// LinkPropertiesDlg ダイアログ

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

	//{{AFX_DATA(LinkPropertiesDlg)
	enum { IDD = IDD_LINKINFO };
	CComboBox	m_cmbLineStyle;
	CStatic	m_LabelTo;
	CStatic	m_LabelFrom;
	CEdit	m_edit;
	CComboBox	m_combo;
	//}}AFX_DATA

protected:
	//{{AFX_VIRTUAL(LinkPropertiesDlg)
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(LinkPropertiesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCombo();
	virtual void OnOK();
	afx_msg void OnBtnColor();
	afx_msg void OnBtnFont();
	afx_msg void OnSelchangeCmbLine();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_LINKINFODLG_H__6557B4B6_290A_11D3_80A7_00A0C9B72FDD__INCLUDED_)
