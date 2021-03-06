﻿/////////////////////////////////////////////////////////////////////////////
// LinkView ビュー
#pragma once

class iEditDoc;

typedef std::stack<DWORD> keyStack;

class LinkView : public CListView
{
protected:
	LinkView();
	DECLARE_DYNCREATE(LinkView)

public:
	iEditDoc* GetDocument();

	virtual void OnInitialUpdate();
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);

protected:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual ~LinkView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

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
	afx_msg void OnLinkMoveUp();
	afx_msg void OnUpdateLinkMoveUp(CCmdUI *pCmdUI);
	afx_msg void OnLinkMoveDown();
	afx_msg void OnUpdateLinkMoveDown(CCmdUI *pCmdUI);
	DECLARE_MESSAGE_MAP()

private:
	int m_preWidth;
	COleDropTarget m_oleDropTarget;
	CFont m_font;
	int curSel;
	DWORD m_preKey;
	keyStack kstack;
	LinkPropsVec items_;
	CImageList m_imageList;

	void SetViewFont();
	void SelectRow(int index);
	void ApplyColorSetting();
	CString GetLocationFromUrlFile(LPCTSTR path);
	void ConstructLinkLIst();
	void AddLinkInfo();
	void NotifySelected();
	void GoBack();
	void GoForward();
	void Reflesh();
};
