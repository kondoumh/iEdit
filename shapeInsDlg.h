#if !defined(AFX_SHAPEINSDLG_H__598871A3_2A0E_11D4_AE6A_00A0C9B72FDD__INCLUDED_)
#define AFX_SHAPEINSDLG_H__598871A3_2A0E_11D4_AE6A_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// shapeInsDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// shapeInsDlg ダイアログ

class shapeInsDlg : public CDialog
{
// コンストラクション
public:
	shapeInsDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(shapeInsDlg)
	enum { IDD = IDD_SHAPEINS };
	CEdit	m_edPath;
	CStatic	m_iShape;
	CButton	m_BtnBrowse;
	CString	m_strPath;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(shapeInsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(shapeInsDlg)
	afx_msg void OnBtnBrowse();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_SHAPEINSDLG_H__598871A3_2A0E_11D4_AE6A_00A0C9B72FDD__INCLUDED_)
