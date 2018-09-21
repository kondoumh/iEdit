// NodeMarginSettingsDlg ダイアログ
#pragma once

#include "afxcmn.h"
#include "afxwin.h"

class NodeMarginSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(NodeMarginSettingsDlg)

public:
	NodeMarginSettingsDlg(CWnd* pParent = NULL);
	virtual ~NodeMarginSettingsDlg();

	enum { IDD = IDD_SETMARGIN };
	CSpinButtonCtrl m_spinLeft;
	CSpinButtonCtrl m_spinRight;
	CSpinButtonCtrl m_spinTop;
	CSpinButtonCtrl m_spinBottom;
	int m_nLeft;
	int m_nRight;
	int m_nTop;
	int m_nBottom;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEdLeft();
	afx_msg void OnEnChangeEdRight();
	afx_msg void OnEnChangeEdTop();
	afx_msg void OnEnChangeEdBottom();
	afx_msg void OnBnClickedOk();
};
