/////////////////////////////////////////////////////////////////////////////
// LinkForPathDlg ダイアログ

#pragma once

class LinkForPathDlg : public CDialog
{
public:
	CString strComment;
	CString strPath;
	CString strOrg;
	LinkForPathDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_LINKINFO2 };
	CEdit	m_Comment;
	CEdit	m_edit;
	CStatic	m_Org;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	afx_msg void OnBrowse();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
