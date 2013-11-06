#if !defined(AFX_NODESRCHDLG_H__1D979736_3121_11D4_AE77_00A0C9B72FDD__INCLUDED_)
#define AFX_NODESRCHDLG_H__1D979736_3121_11D4_AE77_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// nodeSrchDlg.h : ヘッダー ファイル
//

#define WM_CLOSESRCHWINDOW WM_USER + 7
#define WM_SRCHNODE WM_USER + 8
#define WM_LISTUPNODES WM_USER + 9

/////////////////////////////////////////////////////////////////////////////
// nodeSrchDlg ダイアログ

class nodeSrchDlg : public CDialog
{
// コンストラクション
public:
	void displayResult();
	Labels m_labels;
	CString m_srchString;
	nodeSrchDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(nodeSrchDlg)
	enum { IDD = IDD_NODESRCH };
	CButton	m_ckUpper;
	CButton	m_btnGo;
	CButton	m_btnStart;
	CComboBox	m_combSrch;
	CListCtrl	m_lcResult;
	CButton	m_ckText;
	CButton	m_ckLiinks;
	CButton	m_chLabel;
	BOOL	m_bLabel;
	BOOL	m_bLinks;
	BOOL	m_bText;
	BOOL	m_bUpper;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(nodeSrchDlg)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	void srchNode();

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(nodeSrchDlg)
	afx_msg void OnOk();
	virtual void OnCancel();
	afx_msg void OnStart();
	afx_msg void OnBtngo();
	afx_msg void OnChklabel();
	afx_msg void OnChklinks();
	afx_msg void OnChktext();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChkupper();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEditchangeCombo();
	afx_msg void OnSelchangeCombo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CRect m_iniLCRect;
	CWnd* m_pParent;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_NODESRCHDLG_H__1D979736_3121_11D4_AE77_00A0C9B72FDD__INCLUDED_)
