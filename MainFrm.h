// MainFrm.h : CMainFrame クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__96DFF9BB_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
#define AFX_MAINFRM_H__96DFF9BB_1881_11D3_808A_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class SetAlphaDlg;
typedef DWORD (WINAPI *FWINLAYER)(HWND hwnd,DWORD crKey,BYTE bAlpha,DWORD dwFlags);

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// アトリビュート
public:

// オペレーション
public:

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CMainFrame)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	CComboBox* getZoomComboBox();
	void loadFramePosition();
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // コントロール バー用メンバ
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;

// 生成されたメッセージ マップ関数
protected:
	void MakeWindowTransparent();
	afx_msg LRESULT OnSlideAlpha(UINT wParam, LONG lParam);
	void JointCBLine(CFrameWnd *pFrameWnd,CControlBar *pBar,CControlBar *pCB);
	CToolBar m_wndFormPalette;
	void addComboZoom();
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetProperties();
	afx_msg void OnUpdateSetProperties(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnAdjustAlpha();
	afx_msg void OnUpdateAdjustAlpha(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnTransparentMode();
	afx_msg void OnUpdateTransparentMode(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CImageList m_tbPlDwnImage;
	BOOL m_bCanBeTransparent;
	BOOL m_bTransparent;
	long m_nAlphaValue;
	SetAlphaDlg* m_pSetAlphaDlg;
	CComboBox m_comboZoom;
	CFont m_fntComboZoom;
	void saveFramePosition();
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg void OnMnuLsR0();
public:
	afx_msg void OnUpdateMnuLsR0(CCmdUI *pCmdUI);
public:
	afx_msg void OnMnuLsDot();
public:
	afx_msg void OnUpdateMnuLsDot(CCmdUI *pCmdUI);
public:
	afx_msg void OnMnuLsR1();
public:
	afx_msg void OnUpdateMnuLsR1(CCmdUI *pCmdUI);
public:
	afx_msg void OnMnuLsR2();
public:
	afx_msg void OnUpdateMnuLsR2(CCmdUI *pCmdUI);
public:
	afx_msg void OnMnuLsR3();
public:
	afx_msg void OnUpdateMnuLsR3(CCmdUI *pCmdUI);
public:
	afx_msg void OnMnuLsR4();
public:
	afx_msg void OnUpdateMnuLsR4(CCmdUI *pCmdUI);
public:
	afx_msg void OnLinkArrowNone();
public:
	afx_msg void OnUpdateLinkArrowNone(CCmdUI *pCmdUI);
public:
	afx_msg void OnLinkArrowSingle();
public:
	afx_msg void OnUpdateLinkArrowSingle(CCmdUI *pCmdUI);
public:
	afx_msg void OnLinkArrowDouble();
public:
	afx_msg void OnUpdateLinkArrowDouble(CCmdUI *pCmdUI);
public:
	afx_msg void OnLinkDependSingle();
public:
	afx_msg void OnUpdateLinkDependSingle(CCmdUI *pCmdUI);
public:
	afx_msg void OnLinkDependDouble();
public:
	afx_msg void OnUpdateLinkDependDouble(CCmdUI *pCmdUI);
public:
	afx_msg void OnLinkArrowInherit();
public:
	afx_msg void OnUpdateLinkArrowInherit(CCmdUI *pCmdUI);
public:
	afx_msg void OnLinkArrowAgregat();
public:
	afx_msg void OnUpdateLinkArrowAgregat(CCmdUI *pCmdUI);
public:
	afx_msg void OnLinkArrowComposit();
public:
	afx_msg void OnUpdateLinkArrowComposit(CCmdUI *pCmdUI);
public:
	afx_msg void OnViewFormBar();
	afx_msg void OnUpdateViewFormBar(CCmdUI *pCmdUI);
	void createBrushedBitmap(CBitmap* pBitmap, COLORREF rgb);
	afx_msg void OnSelectNodeColor();
	afx_msg void OnSelectLineColor();
	afx_msg void OnSelectFontColor();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_MAINFRM_H__96DFF9BB_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
