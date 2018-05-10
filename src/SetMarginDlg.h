#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// SetMarginDlg ダイアログ

class SetMarginDlg : public CDialog
{
	DECLARE_DYNAMIC(SetMarginDlg)

public:
	SetMarginDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~SetMarginDlg();

// ダイアログ データ
	enum { IDD = IDD_SETMARGIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	CSpinButtonCtrl m_spinLeft;
	CSpinButtonCtrl m_spinRight;
	CSpinButtonCtrl m_spinTop;
	CSpinButtonCtrl m_spinBottom;
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEdLeft();
	afx_msg void OnEnChangeEdRight();
	afx_msg void OnEnChangeEdTop();
	afx_msg void OnEnChangeEdBottom();
	int m_nLeft;
	int m_nRight;
	afx_msg void OnBnClickedOk();
	int m_nTop;
	int m_nBottom;
};
