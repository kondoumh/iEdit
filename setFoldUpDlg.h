#if !defined(AFX_SETFOLDUPDLG_H__9B186546_554F_11D4_AB29_006097789FE5__INCLUDED_)
#define AFX_SETFOLDUPDLG_H__9B186546_554F_11D4_AB29_006097789FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FoldingSettingsDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// FoldingSettingsDlg ダイアログ

class FoldingSettingsDlg : public CDialog
{
// コンストラクション
public:
	FoldingSettingsDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(FoldingSettingsDlg)
	enum { IDD = IDD_SETFOLDUP };
	CSpinButtonCtrl	m_spin;
	int		m_level;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(FoldingSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(FoldingSettingsDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_SETFOLDUPDLG_H__9B186546_554F_11D4_AB29_006097789FE5__INCLUDED_)
