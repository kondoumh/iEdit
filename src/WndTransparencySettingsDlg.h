/////////////////////////////////////////////////////////////////////////////
// WndTransparencySettingsDlg ダイアログ

#pragma once

#define WM_SLIDEALPHA WM_USER + 11

class WndTransparencySettingsDlg : public CDialog
{
	// コンストラクション
public:
	long m_nLevel;
	WndTransparencySettingsDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_ALPHASLIDE };
	CStatic	m_lbLevel;
	CSliderCtrl	m_sldrAlpha;

public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	DECLARE_MESSAGE_MAP()

private:
	CWnd * m_pParent;
};
