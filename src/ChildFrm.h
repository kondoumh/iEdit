// ChildFrm.h : CChildFrame クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDFRM_H__96DFF9BD_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
#define AFX_CHILDFRM_H__96DFF9BD_1881_11D3_808A_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

	//{{AFX_VIRTUAL(CChildFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CChildFrame)
	afx_msg void OnClose();
	afx_msg void OnChangeViewFocus();
	afx_msg void OnUpdateChangeViewFocus(CCmdUI* pCmdUI);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	CSplitterWnd m_Splitter1;
	CSplitterWnd m_Splitter2;
	CSplitterWnd m_Splitter3;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_CHILDFRM_H__96DFF9BD_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
