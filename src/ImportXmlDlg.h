#if !defined(AFX_SELIMPORTDLG_H__A62F3CA6_D771_11D3_AE0C_00A0C9B72FDD__INCLUDED_)
#define AFX_SELIMPORTDLG_H__A62F3CA6_D771_11D3_AE0C_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImportXmlDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// ImportXmlDlg ダイアログ

class ImportXmlDlg : public CDialog
{
public:
	ImportXmlDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(ImportXmlDlg)
	enum { IDD = IDD_SELIMPORT };
	int		m_importMode;
	CString m_titleMessage;
	//}}AFX_DATA

protected:
	//{{AFX_VIRTUAL(ImportXmlDlg)
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	virtual BOOL OnInitDialog();

	//{{AFX_MSG(ImportXmlDlg)
	afx_msg void OnRdRep();
	afx_msg void OnRdSub();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_SELIMPORTDLG_H__A62F3CA6_D771_11D3_AE0C_00A0C9B72FDD__INCLUDED_)
