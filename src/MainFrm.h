// MainFrm.h : CMainFrame クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__96DFF9BB_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
#define AFX_MAINFRM_H__96DFF9BB_1881_11D3_808A_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class WndTransparencySettingsDlg;
typedef DWORD(WINAPI *FWINLAYER)(HWND hwnd, DWORD crKey, BYTE bAlpha, DWORD dwFlags);

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	CComboBox* getZoomComboBox();
	void loadFramePosition();
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMainFrame)
	afx_msg LRESULT OnSlideAlpha(UINT wParam, LONG lParam);
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
	afx_msg void OnMnuLsR0();
	afx_msg void OnUpdateMnuLsR0(CCmdUI *pCmdUI);
	afx_msg void OnMnuLsDot();
	afx_msg void OnUpdateMnuLsDot(CCmdUI *pCmdUI);
	afx_msg void OnMnuLsR1();
	afx_msg void OnUpdateMnuLsR1(CCmdUI *pCmdUI);
	afx_msg void OnMnuLsR2();
	afx_msg void OnUpdateMnuLsR2(CCmdUI *pCmdUI);
	afx_msg void OnMnuLsR3();
	afx_msg void OnUpdateMnuLsR3(CCmdUI *pCmdUI);
	afx_msg void OnMnuLsR4();
	afx_msg void OnUpdateMnuLsR4(CCmdUI *pCmdUI);
	afx_msg void OnLinkArrowNone();
	afx_msg void OnUpdateLinkArrowNone(CCmdUI *pCmdUI);
	afx_msg void OnLinkArrowSingle();
	afx_msg void OnUpdateLinkArrowSingle(CCmdUI *pCmdUI);
	afx_msg void OnLinkArrowDouble();
	afx_msg void OnUpdateLinkArrowDouble(CCmdUI *pCmdUI);
	afx_msg void OnLinkDependSingle();
	afx_msg void OnUpdateLinkDependSingle(CCmdUI *pCmdUI);
	afx_msg void OnLinkDependDouble();
	afx_msg void OnUpdateLinkDependDouble(CCmdUI *pCmdUI);
	afx_msg void OnLinkArrowInherit();
	afx_msg void OnUpdateLinkArrowInherit(CCmdUI *pCmdUI);
	afx_msg void OnLinkArrowAgregat();
	afx_msg void OnUpdateLinkArrowAgregat(CCmdUI *pCmdUI);
	afx_msg void OnLinkArrowComposit();
	afx_msg void OnUpdateLinkArrowComposit(CCmdUI *pCmdUI);
	afx_msg void OnViewFormBar();
	afx_msg void OnUpdateViewFormBar(CCmdUI *pCmdUI);
	afx_msg void OnSelectNodeColor();
	afx_msg void OnSelectLineColor();
	afx_msg void OnSelectFontColor();
	afx_msg void OnAppHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CStatusBar  m_wndStatusBar;
	CToolBar    m_wndToolBar;
	CToolBar m_wndFormPalette;
	CImageList m_tbPlDwnImage;
	BOOL m_bCanBeTransparent;
	BOOL m_bTransparent;
	long m_nAlphaValue;
	WndTransparencySettingsDlg* m_pSetAlphaDlg;
	CComboBox m_comboZoom;
	CFont m_fntComboZoom;
	void saveFramePosition();
	void createBrushedBitmap(CBitmap* pBitmap, COLORREF rgb);
	void MakeWindowTransparent();
	void JointCBLine(CFrameWnd *pFrameWnd, CControlBar *pBar, CControlBar *pCB);
	void addComboZoom();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_MAINFRM_H__96DFF9BB_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
