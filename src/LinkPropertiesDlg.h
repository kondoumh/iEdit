#if !defined(AFX_LINKINFODLG_H__6557B4B6_290A_11D3_80A7_00A0C9B72FDD__INCLUDED_)
#define AFX_LINKINFODLG_H__6557B4B6_290A_11D3_80A7_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LinkPropertiesDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// LinkPropertiesDlg ダイアログ

class LinkPropertiesDlg : public CDialog
{
// コンストラクション
public:
	int lineWidth;
	int styleLine;
	COLORREF colorLine;
	LOGFONT lf;
	int styleArrow;
	CString strTo;
	CString strFrom;
	CString strComment;
	LinkPropertiesDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(LinkPropertiesDlg)
	enum { IDD = IDD_LINKINFO };
	CComboBox	m_cmbLineStyle;
	CStatic	m_LabelTo;
	CStatic	m_LabelFrom;
	CEdit	m_edit;
	CComboBox	m_combo;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(LinkPropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(LinkPropertiesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCombo();
	virtual void OnOK();
	afx_msg void OnBtnColor();
	afx_msg void OnBtnFont();
	afx_msg void OnSelchangeCmbLine();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_LINKINFODLG_H__6557B4B6_290A_11D3_80A7_00A0C9B72FDD__INCLUDED_)
