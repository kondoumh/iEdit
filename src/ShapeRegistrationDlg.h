/////////////////////////////////////////////////////////////////////////////
// ShapeRegistrationDlg ダイアログ

#pragma once

class ShapeRegistrationDlg : public CDialog
{
public:
	ShapeRegistrationDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_SHAPEINS };
	CEdit	m_edPath;
	CStatic	m_iShape;
	CButton	m_BtnBrowse;
	CString	m_strPath;

protected:

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();

	afx_msg void OnBtnBrowse();
	DECLARE_MESSAGE_MAP()
};
