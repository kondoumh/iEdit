#if !defined(AFX_SELIMPORTDLG_H__A62F3CA6_D771_11D3_AE0C_00A0C9B72FDD__INCLUDED_)
#define AFX_SELIMPORTDLG_H__A62F3CA6_D771_11D3_AE0C_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelImportDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// SelImportDlg ダイアログ

class SelImportDlg : public CDialog
{
// コンストラクション
public:
	SelImportDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(SelImportDlg)
	enum { IDD = IDD_SELIMPORT };
	int		m_importMode;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(SelImportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(SelImportDlg)
	afx_msg void OnRdRep();
	afx_msg void OnRdSub();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_SELIMPORTDLG_H__A62F3CA6_D771_11D3_AE0C_00A0C9B72FDD__INCLUDED_)
