#if !defined(AFX_INPCNDLG_H__3912E181_B749_11D2_BB6F_F08B54D33F0D__INCLUDED_)
#define AFX_INPCNDLG_H__3912E181_B749_11D2_BB6F_F08B54D33F0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CreateNodeDlg ダイアログ

class CreateNodeDlg : public CDialog
{
public:
	CRect iniRect;
	CPoint m_iniPt;
	CreateNodeDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(CreateNodeDlg)
	enum { IDD = IDD_INPCN };
	CEdit	m_editName;
	CString	m_strcn;
	//}}AFX_DATA

protected:
	//{{AFX_VIRTUAL(CreateNodeDlg)
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	LRESULT afx_msg OnSetCharData(UINT wParam, LONG lParam);

	//{{AFX_MSG(CreateNodeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_INPCNDLG_H__3912E181_B749_11D2_BB6F_F08B54D33F0D__INCLUDED_)
