/////////////////////////////////////////////////////////////////////////////
// ImportXmlDlg ダイアログ

#pragma once

class ImportXmlDlg : public CDialog
{
public:
	ImportXmlDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_SELIMPORT };
	int		m_importMode;
	CString m_titleMessage;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	afx_msg void OnRdRep();
	afx_msg void OnRdSub();
	DECLARE_MESSAGE_MAP()
};
