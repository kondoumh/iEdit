#pragma once
#include "afxwin.h"


// DebugPrintDlg ダイアログ

class DebugPrintDlg : public CDialog
{
	DECLARE_DYNAMIC(DebugPrintDlg)
	
	CWnd* m_pParent;

public:
	DebugPrintDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~DebugPrintDlg();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, 
		DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
		UINT nID, CCreateContext* pContext = NULL);
	
	virtual void OnOK();
	virtual void OnCancel();

// ダイアログ データ
	enum { IDD = IDD_DEBUG_WINDOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edConsole;
	afx_msg void OnBnClickedBtnClear();
	afx_msg void OnBnClickedBtnCopy();
};
