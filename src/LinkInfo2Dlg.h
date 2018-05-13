#if !defined(AFX_LINKINFO2DLG_H__BE7CAEF3_2AA4_11D3_80A9_00A0C9B72FDD__INCLUDED_)
#define AFX_LINKINFO2DLG_H__BE7CAEF3_2AA4_11D3_80A9_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LinkForPathDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// LinkForPathDlg ダイアログ

class LinkForPathDlg : public CDialog
{
// コンストラクション
public:
	CString strComment;
	CString strPath;
	CString strOrg;
	LinkForPathDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(LinkForPathDlg)
	enum { IDD = IDD_LINKINFO2 };
	CEdit	m_Comment;
	CEdit	m_edit;
	CStatic	m_Org;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(LinkForPathDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(LinkForPathDlg)
	afx_msg void OnBrowse();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_LINKINFO2DLG_H__BE7CAEF3_2AA4_11D3_80A9_00A0C9B72FDD__INCLUDED_)
