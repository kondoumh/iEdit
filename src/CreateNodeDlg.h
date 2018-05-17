#if !defined(AFX_INPCNDLG_H__3912E181_B749_11D2_BB6F_F08B54D33F0D__INCLUDED_)
#define AFX_INPCNDLG_H__3912E181_B749_11D2_BB6F_F08B54D33F0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CreateNodeDlg ダイアログ

class CreateNodeDlg : public CDialog
{
// コンストラクション
public:
	CRect iniRect;
	CPoint m_iniPt;
	CreateNodeDlg(CWnd* pParent = NULL);

// ダイアログ データ
	//{{AFX_DATA(CreateNodeDlg)
	enum { IDD = IDD_INPCN };
	CEdit	m_editName;
	CString	m_strcn;
	//}}AFX_DATA


// オーバーライド
	//{{AFX_VIRTUAL(CreateNodeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	LRESULT afx_msg OnSetCharData(UINT wParam, LONG lParam);

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CreateNodeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_INPCNDLG_H__3912E181_B749_11D2_BB6F_F08B54D33F0D__INCLUDED_)
