#if !defined(AFX_SETALPHADLG_H__B956C603_D213_44F3_8CE3_785ECD40795B__INCLUDED_)
#define AFX_SETALPHADLG_H__B956C603_D213_44F3_8CE3_785ECD40795B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetAlphaDlg.h : ヘッダー ファイル
//

#define WM_SLIDEALPHA WM_USER + 11

/////////////////////////////////////////////////////////////////////////////
// SetAlphaDlg ダイアログ

class SetAlphaDlg : public CDialog
{
// コンストラクション
public:
	long m_nLevel;
	SetAlphaDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(SetAlphaDlg)
	enum { IDD = IDD_ALPHASLIDE };
	CStatic	m_lbLevel;
	CSliderCtrl	m_sldrAlpha;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(SetAlphaDlg)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(SetAlphaDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CWnd* m_pParent;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_SETALPHADLG_H__B956C603_D213_44F3_8CE3_785ECD40795B__INCLUDED_)
