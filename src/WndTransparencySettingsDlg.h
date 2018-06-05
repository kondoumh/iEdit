#if !defined(AFX_SETALPHADLG_H__B956C603_D213_44F3_8CE3_785ECD40795B__INCLUDED_)
#define AFX_SETALPHADLG_H__B956C603_D213_44F3_8CE3_785ECD40795B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WndTransparencySettingsDlg.h : ヘッダー ファイル
//

#define WM_SLIDEALPHA WM_USER + 11

/////////////////////////////////////////////////////////////////////////////
// WndTransparencySettingsDlg ダイアログ

class WndTransparencySettingsDlg : public CDialog
{
	// コンストラクション
public:
	long m_nLevel;
	WndTransparencySettingsDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(WndTransparencySettingsDlg)
	enum { IDD = IDD_ALPHASLIDE };
	CStatic	m_lbLevel;
	CSliderCtrl	m_sldrAlpha;
	//}}AFX_DATA


	//{{AFX_VIRTUAL(WndTransparencySettingsDlg)
public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

protected:

	//{{AFX_MSG(WndTransparencySettingsDlg)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CWnd * m_pParent;
};

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_SETALPHADLG_H__B956C603_D213_44F3_8CE3_785ECD40795B__INCLUDED_)
