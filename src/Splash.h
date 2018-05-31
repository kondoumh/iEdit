#ifndef _SPLASH_SCRN_
#define _SPLASH_SCRN_

// Splash.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
//   スプラッシュ スクリーン クラス

class CSplashWnd : public CWnd
{
protected:
	CSplashWnd();

public:
	CBitmap m_bitmap;
	static void EnableSplashScreen(BOOL bEnable = TRUE);
	static void ShowSplashScreen(CWnd* pParentWnd = NULL);
	static BOOL PreTranslateAppMessage(MSG* pMsg);


public:
	~CSplashWnd();

	//{{AFX_VIRTUAL(CSplashWnd)
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

protected:
	BOOL Create(CWnd* pParentWnd = NULL);
	void HideSplashScreen();
	static BOOL c_bShowSplashWnd;
	static CSplashWnd* c_pSplashWnd;

	//{{AFX_MSG(CSplashWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif
