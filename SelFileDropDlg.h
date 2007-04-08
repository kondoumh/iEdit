#if !defined(AFX_SELFILEDROPDLG_H__B17C7D51_E20B_11D5_9ACE_00105A6E744E__INCLUDED_)
#define AFX_SELFILEDROPDLG_H__B17C7D51_E20B_11D5_9ACE_00105A6E744E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelFileDropDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// SelFileDropDlg ダイアログ

class SelFileDropDlg : public CDialog
{
// コンストラクション
public:
	SelFileDropDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(SelFileDropDlg)
	enum { IDD = IDD_SELFILEDROP };
	int		m_nDropProc;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(SelFileDropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(SelFileDropDlg)
		// メモ: ClassWizard はこの位置にメンバ関数を追加します。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_SELFILEDROPDLG_H__B17C7D51_E20B_11D5_9ACE_00105A6E744E__INCLUDED_)
