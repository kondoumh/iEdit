/////////////////////////////////////////////////////////////////////////////
// EditShapeCategoryDlg ダイアログ

#pragma once

class EditShapeCategoryDlg : public CDialog
{
public:
	CString m_name;
	EditShapeCategoryDlg(CWnd* pParent = NULL);

protected:
	enum { IDD = IDD_SHPCATDLG };
	CEdit	m_edit;

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	afx_msg void OnChangeEdit1();
	DECLARE_MESSAGE_MAP()
};
