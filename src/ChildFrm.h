// ChildFrm.h : CChildFrame クラスのインターフェイス

#pragma once

class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();
	void ChangeSelectedLineWidth();
	void ChangeSelectedLinkArrow();
	void ChangeSelectedNodeColor();
	void ChangeSelectedLineColor();
	void ChangeSelectedFontColor();

protected:

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	afx_msg void OnClose();
	afx_msg void OnChangeViewFocus();
	afx_msg void OnUpdateChangeViewFocus(CCmdUI* pCmdUI);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

private:
	CSplitterWnd m_Splitter1;
	CSplitterWnd m_Splitter2;
	CSplitterWnd m_Splitter3;
};
