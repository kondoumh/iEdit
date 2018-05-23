#if !defined(AFX_SETMFSIZEDLG_H__4DDA3CFA_849A_4875_87EE_0CACC6A6E93F__INCLUDED_)
#define AFX_SETMFSIZEDLG_H__4DDA3CFA_849A_4875_87EE_0CACC6A6E93F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MetafileSettingsDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// MetafileSettingsDlg ダイアログ

class MetafileSettingsDlg : public CDialog
{
public:
	MetafileSettingsDlg(CWnd* pParent = NULL);
	double m_ry;
	double m_rx;

	//{{AFX_DATA(MetafileSettingsDlg)
	enum { IDD = IDD_MFSIZE };
	CStatic	m_stInfo;
	CStatic	m_stMF;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(MetafileSettingsDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:
	void showSizeInfo();
	HENHMETAFILE createMF();

	//{{AFX_MSG(MetafileSettingsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnLeft();
	afx_msg void OnBtnRight();
	afx_msg void OnBtnDown();
	afx_msg void OnBtnUp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnReset();
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_SETMFSIZEDLG_H__4DDA3CFA_849A_4875_87EE_0CACC6A6E93F__INCLUDED_)
