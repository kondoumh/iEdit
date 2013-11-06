#if !defined(AFX_ALOPTDLG_H__A1873983_92DF_11D3_9892_006097789FE5__INCLUDED_)
#define AFX_ALOPTDLG_H__A1873983_92DF_11D3_9892_006097789FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ALOptDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// ALOptDlg ダイアログ

class ALOptDlg : public CDialog
{
// コンストラクション
public:
	ALOptDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(ALOptDlg)
	enum { IDD = IDD_SEL_LAYOUT };
	CSpinButtonCtrl	m_spin;
	CEdit	m_editCY;
	int		m_showMode;
	int		m_cycle;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(ALOptDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(ALOptDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRdAni();
	afx_msg void OnRdCy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_ALOPTDLG_H__A1873983_92DF_11D3_9892_006097789FE5__INCLUDED_)
