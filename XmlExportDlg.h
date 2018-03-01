#if !defined(AFX_SELEXPORTDLG_H__52A28553_FB9A_11D3_AE3F_00A0C9B72FDD__INCLUDED_)
#define AFX_SELEXPORTDLG_H__52A28553_FB9A_11D3_AE3F_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExportXmlDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// ExportXmlDlg ダイアログ

class ExportXmlDlg : public CDialog
{
// コンストラクション
public:
	ExportXmlDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ
	
// ダイアログ データ
	//{{AFX_DATA(ExportXmlDlg)
	enum { IDD = IDD_SELEXPORT };
	int		m_nTreeOp;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(ExportXmlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(ExportXmlDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_SELEXPORTDLG_H__52A28553_FB9A_11D3_AE3F_00A0C9B72FDD__INCLUDED_)
