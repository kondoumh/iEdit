#if !defined(AFX_RANDAREADLG_H__5FB3CA53_D7F0_11D3_AA35_006097789FE5__INCLUDED_)
#define AFX_RANDAREADLG_H__5FB3CA53_D7F0_11D3_AA35_006097789FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// randAreaDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// randAreaDlg ダイアログ

class randAreaDlg : public CDialog
{
// コンストラクション
public:
	randAreaDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(randAreaDlg)
	enum { IDD = IDD_RANDAREA };
	int		m_height;
	int		m_width;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(randAreaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(randAreaDlg)
		// メモ: ClassWizard はこの位置にメンバ関数を追加します。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_RANDAREADLG_H__5FB3CA53_D7F0_11D3_AA35_006097789FE5__INCLUDED_)
