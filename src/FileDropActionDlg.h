#if !defined(AFX_SELFILEDROPDLG_H__B17C7D51_E20B_11D5_9ACE_00105A6E744E__INCLUDED_)
#define AFX_SELFILEDROPDLG_H__B17C7D51_E20B_11D5_9ACE_00105A6E744E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FileDropActionDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// FileDropActionDlg ダイアログ

class FileDropActionDlg : public CDialog
{
public:
	FileDropActionDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(FileDropActionDlg)
	enum { IDD = IDD_SELFILEDROP };
	int		m_nDropProc;
	//}}AFX_DATA

protected:
	//{{AFX_VIRTUAL(FileDropActionDlg)
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(FileDropActionDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_SELFILEDROPDLG_H__B17C7D51_E20B_11D5_9ACE_00105A6E744E__INCLUDED_)
