#if !defined(AFX_SELEXPORTDLG_H__52A28553_FB9A_11D3_AE3F_00A0C9B72FDD__INCLUDED_)
#define AFX_SELEXPORTDLG_H__52A28553_FB9A_11D3_AE3F_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelExportDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// SelExportDlg ダイアログ

class SelExportDlg : public CDialog
{
// コンストラクション
public:
	SelExportDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ
	bool m_bShowChekPrintText;
	
// ダイアログ データ
	//{{AFX_DATA(SelExportDlg)
	enum { IDD = IDD_SELEXPORT };
	CButton	m_chkPrintText;
	BOOL	m_bPrintText;
	int		m_nTreeOp;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(SelExportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(SelExportDlg)
	afx_msg void OnChkPrintText();
	virtual BOOL OnInitDialog();
	afx_msg void OnRdTree1();
	afx_msg void OnRdTree2();
	afx_msg void OnRdTree3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_SELEXPORTDLG_H__52A28553_FB9A_11D3_AE3F_00A0C9B72FDD__INCLUDED_)
