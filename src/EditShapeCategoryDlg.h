#if !defined(AFX_INPCATDLG_H__2ACE9F19_2A02_11D4_AE6A_00A0C9B72FDD__INCLUDED_)
#define AFX_INPCATDLG_H__2ACE9F19_2A02_11D4_AE6A_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditShapeCategoryDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// EditShapeCategoryDlg ダイアログ

class EditShapeCategoryDlg : public CDialog
{
public:
	EditShapeCategoryDlg(CWnd* pParent = NULL);
	CString	m_strName;

protected:
	//{{AFX_DATA(EditShapeCategoryDlg)
	enum { IDD = IDD_SHPCATDLG };
	CEdit	m_edit;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(EditShapeCategoryDlg)
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(EditShapeCategoryDlg)
	afx_msg void OnChangeEdit1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_INPCATDLG_H__2ACE9F19_2A02_11D4_AE6A_00A0C9B72FDD__INCLUDED_)
