#if !defined(AFX_NODEPROPDLG_H__86676B23_35C1_11D3_80BC_00A0C9B72FDD__INCLUDED_)
#define AFX_NODEPROPDLG_H__86676B23_35C1_11D3_80BC_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NodePropertiesDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// NodePropertiesDlg ダイアログ

class NodePropertiesDlg : public CDialog
{
// コンストラクション
public:
	struct Margins
	{
		int l;
		int r;
		int t;
		int b;
	} margins;
	int vert;
	int horiz;
	BOOL bOldBynary;
	BOOL bMultiLine;
	int styleText;
	int styleLine;
	int lineWidth;
	COLORREF colorFill;
	COLORREF colorLine;
	COLORREF colorFont;
	LOGFONT lf;
	CString m_strLabel;
	NodePropertiesDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(NodePropertiesDlg)
	enum { IDD = IDD_PRPNODE };
	CComboBox	m_cmbHoriz;
	CComboBox	m_cmbVert;
	CButton	m_chkBoxNoBrs;
	CButton	m_BtnBrsColor;
	CComboBox	m_CombTLine;
	CEdit	m_editLabel;
	int		m_shape;
	int		m_TLine;
	BOOL	m_bNoBrush;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(NodePropertiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(NodePropertiesDlg)
	afx_msg void OnSelchangeComboline();
	afx_msg void OnBtnLineColor();
	afx_msg void OnBtnBrs();
	afx_msg void OnBtnFont();
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioTline1();
	afx_msg void OnRadioTline2();
	afx_msg void OnRadioShape();
	afx_msg void OnRadioShape2();
	virtual void OnOK();
	afx_msg void OnChkNoBrs();
	afx_msg void OnRadioTline3();
	afx_msg void OnSelchangeComboHoriz();
	afx_msg void OnSelchangeComboVert();
	afx_msg void OnRadioShape3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSetMargin();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_NODEPROPDLG_H__86676B23_35C1_11D3_80BC_00A0C9B72FDD__INCLUDED_)
