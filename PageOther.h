#if !defined(AFX_PAGEOTHER_H__023C7320_8088_11D3_B4D3_00A0C9B72FDD__INCLUDED_)
#define AFX_PAGEOTHER_H__023C7320_8088_11D3_B4D3_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageOther.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// PageOther ダイアログ

class PageOther : public CDialog
{
// コンストラクション
public:
	PageOther(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(PageOther)
	enum { IDD = IDD_PG_OTHER };
	CButton	m_chkStylesheet;
	CEdit	m_edStylesheet;
	CButton	m_chkDrawUnderLine;
	CButton	m_chkAccelMove;
	CButton	m_chkInheritSibling;
	CButton	m_chkInheritParent;
	CButton	m_chkShowHS;
	int		m_tabSelect;
	BOOL	m_bShowHS;
	BOOL	m_bInheritParent;
	BOOL	m_bInheritSibling;
	BOOL	m_bAccelmove;
	BOOL	m_bDrawUnderLine;
	BOOL	m_bSetStylesheet;
	CString	m_strStylesheet;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(PageOther)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(PageOther)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnRdTab1();
	afx_msg void OnRdTab2();
	afx_msg void OnRdTab3();
	afx_msg void OnChkShowHs();
	afx_msg void OnChkInheritParent();
	afx_msg void OnChkInheritSibling();
	afx_msg void OnChkAccel();
	afx_msg void OnBtnSetmfsize();
	afx_msg void OnChkDrwUndrln();
	afx_msg void OnChkSetStylesheet();
	afx_msg void OnChangeEditStylesheet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_PAGEOTHER_H__023C7320_8088_11D3_B4D3_00A0C9B72FDD__INCLUDED_)
