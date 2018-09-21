/////////////////////////////////////////////////////////////////////////////
// CreateNodeDlg ダイアログ

#pragma once

class CreateNodeDlg : public CDialog
{
	CRect m_initialRect;

public:
	CPoint m_initialPt;
	CreateNodeDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_INPCN };
	CEdit	m_editName;
	CString	m_strcn;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	LRESULT afx_msg OnSetCharData(UINT wParam, LONG lParam);

	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
};
