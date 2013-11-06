// CG: このファイルはスプラッシュ スクリーン コンポーネントによって追加されました。

#ifndef _SPLASH_SCRN_
#define _SPLASH_SCRN_

// Splash.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
//   スプラッシュ スクリーン クラス

class CSplashWnd : public CWnd
{
// 構築
protected:
	CSplashWnd();

// アトリビュート:
public:
	CBitmap m_bitmap;

// オペレーション
public:
	static void EnableSplashScreen(BOOL bEnable = TRUE);
	static void ShowSplashScreen(CWnd* pParentWnd = NULL);
	static BOOL PreTranslateAppMessage(MSG* pMsg);

// オーバーライド
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashWnd)
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	~CSplashWnd();
	virtual void PostNcDestroy();

protected:
	BOOL Create(CWnd* pParentWnd = NULL);
	void HideSplashScreen();
	static BOOL c_bShowSplashWnd;
	static CSplashWnd* c_pSplashWnd;

// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG(CSplashWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
