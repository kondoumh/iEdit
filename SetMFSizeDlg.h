#if !defined(AFX_SETMFSIZEDLG_H__4DDA3CFA_849A_4875_87EE_0CACC6A6E93F__INCLUDED_)
#define AFX_SETMFSIZEDLG_H__4DDA3CFA_849A_4875_87EE_0CACC6A6E93F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetMFSizeDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// SetMFSizeDlg ダイアログ

class SetMFSizeDlg : public CDialog
{
// コンストラクション
public:
	SetMFSizeDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ
	double m_ry;
	double m_rx;

// ダイアログ データ
	//{{AFX_DATA(SetMFSizeDlg)
	enum { IDD = IDD_MFSIZE };
	CStatic	m_stInfo;
	CStatic	m_stMF;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(SetMFSizeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	void showSizeInfo();
	HENHMETAFILE createMF();

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(SetMFSizeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnLeft();
	afx_msg void OnBtnRight();
	afx_msg void OnBtnDown();
	afx_msg void OnBtnUp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnReset();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_SETMFSIZEDLG_H__4DDA3CFA_849A_4875_87EE_0CACC6A6E93F__INCLUDED_)
