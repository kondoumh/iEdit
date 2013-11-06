#if !defined(AFX_INPCATDLG_H__2ACE9F19_2A02_11D4_AE6A_00A0C9B72FDD__INCLUDED_)
#define AFX_INPCATDLG_H__2ACE9F19_2A02_11D4_AE6A_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InpcatDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CInpcatDlg ダイアログ

class CInpcatDlg : public CDialog
{
// コンストラクション
public:
	CInpcatDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(CInpcatDlg)
	enum { IDD = IDD_SHPCATDLG };
	CEdit	m_edit;
	CString	m_strName;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CInpcatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(CInpcatDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEdit1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_INPCATDLG_H__2ACE9F19_2A02_11D4_AE6A_00A0C9B72FDD__INCLUDED_)
