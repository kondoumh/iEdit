#if !defined(AFX_LINKVIEW_H__96DFF9CC_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
#define AFX_LINKVIEW_H__96DFF9CC_1881_11D3_808A_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LinkView.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// LinkView ビュー
class iEditDoc;

typedef stack<DWORD> keyStack;

class LinkView : public CListView
{
protected:
	LinkView();
	DECLARE_DYNCREATE(LinkView)

public:
	int m_preWidth;
	void setViewFont();
	void jumpBack();
	iEditDoc* GetDocument();

public:
	//{{AFX_VIRTUAL(LinkView)
	virtual void OnInitialUpdate();
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
protected:
	virtual void OnDraw(CDC* pDC);      // このビューを描画するためにオーバーライドしました。
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

protected:
	virtual ~LinkView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	void reflesh();
	void setSelection(int index);
	void doColorSetting();
	bool isURLStr(const CString& str) const;
	CString getLocationFromURLFile(LPCTSTR path);
	void listConstruct();
	void setLinkInfo();
	void notifySelLink();
	void jumpTo();
	//{{AFX_MSG(LinkView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnJumpTo();
	afx_msg void OnUpdateJumpTo(CCmdUI* pCmdUI);
	afx_msg void OnJumpBack();
	afx_msg void OnUpdateJumpBack(CCmdUI* pCmdUI);
	afx_msg void OnDelete();
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
	afx_msg void OnSetLinkInfo();
	afx_msg void OnUpdateSetLinkInfo(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnReturn(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditLabel();
	afx_msg void OnEditReplace();
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditLabel(CCmdUI* pCmdUI);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	COleDropTarget m_oleDropTarget;
	CFont m_font;
	int curSel;
	DWORD m_preKey;
	keyStack kstack;
	LinkPropsVec items_;
	CImageList m_imageList;
public:
	afx_msg void OnLinkMoveUp();
	afx_msg void OnUpdateLinkMoveUp(CCmdUI *pCmdUI);
	afx_msg void OnLinkMoveDown();
	afx_msg void OnUpdateLinkMoveDown(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_LINKVIEW_H__96DFF9CC_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
