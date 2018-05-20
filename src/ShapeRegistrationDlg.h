#if !defined(AFX_SHAPEINSDLG_H__598871A3_2A0E_11D4_AE6A_00A0C9B72FDD__INCLUDED_)
#define AFX_SHAPEINSDLG_H__598871A3_2A0E_11D4_AE6A_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShapeRegistrationDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// ShapeRegistrationDlg ダイアログ

class ShapeRegistrationDlg : public CDialog
{
public:
	ShapeRegistrationDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(ShapeRegistrationDlg)
	enum { IDD = IDD_SHAPEINS };
	CEdit	m_edPath;
	CStatic	m_iShape;
	CButton	m_BtnBrowse;
	CString	m_strPath;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(ShapeRegistrationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(ShapeRegistrationDlg)
	afx_msg void OnBtnBrowse();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_SHAPEINSDLG_H__598871A3_2A0E_11D4_AE6A_00A0C9B72FDD__INCLUDED_)
