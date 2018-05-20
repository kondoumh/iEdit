#include "afxwin.h"
#if !defined(AFX_PAGENODE_H__79C43544_7FE6_11D3_9860_006097789FE5__INCLUDED_)
#define AFX_PAGENODE_H__79C43544_7FE6_11D3_9860_006097789FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionPageForNode.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// OptionPageForNode ダイアログ

class OptionPageForNode : public CDialog
{
public:
	struct Margins { int l; int r; int t; int b; } margins;
	BOOL m_bSyncOrder;
	BOOL m_bEnableGroup;
	int m_orderDirection;
	int vert;
	int horiz;
	BOOL bMultiLine;
	int styleText;
	int styleLine;
	int lineWidth;
	COLORREF colorFill;
	COLORREF colorLine;
	COLORREF colorFont;
	LOGFONT lf;
	OptionPageForNode(CWnd* pParent = NULL);

	//{{AFX_DATA(OptionPageForNode)
	enum { IDD = IDD_PG_NODE };
	CComboBox	m_cmbVert;
	CComboBox	m_cmbHoriz;
	CButton	m_chkNoBrs;
	CButton	m_BtnBrsColor;
	CComboBox	m_cmbLineStyle;
	int		m_rdTLine;
	int		m_rdShape;
	BOOL	m_bNoBrs;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(OptionPageForNode)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(OptionPageForNode)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnLineColor();
	afx_msg void OnBtnBrs();
	afx_msg void OnBtnFont();
	afx_msg void OnRadioTline1();
	afx_msg void OnRadioTline2();
	afx_msg void OnRadioShape();
	afx_msg void OnRadioShape2();
	afx_msg void OnChkNoBrs();
	afx_msg void OnSelchangeComboline();
	afx_msg void OnRadioTline3();
	afx_msg void OnSelchangeComboHoriz();
	afx_msg void OnSelchangeComboVert();
	afx_msg void OnRadioShape3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedChkSyncOrder();
	afx_msg void OnBnClickedChkEnableGrouping();
	CButton m_chkSyncOrder;
	CButton m_chkEnableGroup;
	int m_rdAscending;
	int m_rdDescending;
	afx_msg void OnBnClickedRadioAscending();
	afx_msg void OnBnClickedRadioDescending();
	CButton m_btnAscending;
	CButton m_btnDescending;
	CButton m_ChkDisableNodeResize;
	BOOL m_bDisableNodeResize;
	afx_msg void OnBnClickedDisableNodeResize();
	afx_msg void OnBnClickedBtnSetMargin();
	BOOL m_bPriorSelectionDragging;
	CButton m_chkPriorSelectionDragging;
	afx_msg void OnBnClickedPriorSelectionDragging();
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PAGENODE_H__79C43544_7FE6_11D3_9860_006097789FE5__INCLUDED_)
