#if !defined(AFX_SETFOLDUPDLG_H__9B186546_554F_11D4_AB29_006097789FE5__INCLUDED_)
#define AFX_SETFOLDUPDLG_H__9B186546_554F_11D4_AB29_006097789FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FoldingSettingsDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// FoldingSettingsDlg ダイアログ

class FoldingSettingsDlg : public CDialog
{
public:
	FoldingSettingsDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(FoldingSettingsDlg)
	enum { IDD = IDD_SETFOLDUP };
	CSpinButtonCtrl	m_spin;
	int		m_level;
	//}}AFX_DATA

	//{{AFX_VIRTUAL(FoldingSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(FoldingSettingsDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_SETFOLDUPDLG_H__9B186546_554F_11D4_AB29_006097789FE5__INCLUDED_)
