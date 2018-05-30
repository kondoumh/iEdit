#if !defined(AFX_SELEXPORTDLG_H__52A28553_FB9A_11D3_AE3F_00A0C9B72FDD__INCLUDED_)
#define AFX_SELEXPORTDLG_H__52A28553_FB9A_11D3_AE3F_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ExportXmlDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// ExportXmlDlg ダイアログ

class ExportXmlDlg : public CDialog
{
public:
	ExportXmlDlg(CWnd* pParent = NULL);
	
	//{{AFX_DATA(ExportXmlDlg)
	enum { IDD = IDD_SELEXPORT };
	int		m_nTreeOp;
	//}}AFX_DATA

protected:
	//{{AFX_VIRTUAL(ExportXmlDlg)
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(ExportXmlDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_SELEXPORTDLG_H__52A28553_FB9A_11D3_AE3F_00A0C9B72FDD__INCLUDED_)
