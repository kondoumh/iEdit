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

// アトリビュート
public:

// オペレーション
public:
	void changeSelectedLinkWidth();
	void changeSelectedLinkArrow();

//オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CChildFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 生成したメッセージ マップ関数
protected:
	//{{AFX_MSG(CChildFrame)
	afx_msg void OnClose();
	afx_msg void OnChangeViewFocus();
	afx_msg void OnUpdateChangeViewFocus(CCmdUI* pCmdUI);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CSplitterWnd m_Splitter3;
	CSplitterWnd m_Splitter2;
	CSplitterWnd m_Splitter1;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_CHILDFRM_H__96DFF9BD_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
