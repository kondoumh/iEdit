// OutLineView.h : OutlineView クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTLINEVIEW_H__96DFF9C1_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
#define AFX_OUTLINEVIEW_H__96DFF9C1_1881_11D3_808A_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class nodeSrchDlg;

class OutlineView : public CTreeView
{
protected: // シリアライズ機能のみから作成します。
	OutlineView();
	DECLARE_DYNCREATE(OutlineView)

// アトリビュート
public:
	iEditDoc* GetDocument();

// オペレーション
public:

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(OutlineView)
	public:
	virtual void OnDraw(CDC* pDC);  // このビューを描画する際にオーバーライドされます。
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // 構築後の最初の１度だけ呼び出されます。
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	serialVec getDrawOrder(const bool bShowSubBranch) const;
	void hideModeless();
	void treeToSequence0(Labels& ls);
	void OutputXML(const CString& outPath);
	void OutputHTML(const CString& outPath, const CString& outDir);
	void setViewFont();
	void treeToSequence(Labels& ls);
	virtual ~OutlineView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成されたメッセージ マップ関数
protected:
	void cloneTree(const HTREEITEM& curItem, HTREEITEM targetParent, IdMap& idm);
	int getBranchMode() const;
	void resetShowBranch();
	void doColorSetting();
	void foldUpTree(HTREEITEM hItem, int curLevel, int levelSet);
	void clearUndo();
	afx_msg LRESULT OnListUpNodes(UINT wParam, LONG lParam);
	afx_msg LRESULT OnHideSrchDlg(UINT wParam, LONG lParam);
	afx_msg LRESULT OnSearchNode(UINT wParam, LONG lParam);
	void htmlOutTree(HTREEITEM hItem, CStdioFile* f);
	bool ImportXML(const CString& inPath);
	bool ImportText(const CString& inPath);
	void textOutTree(HTREEITEM hItem, CStdioFile* f, int tab, BOOL bOutText=TRUE);
	void OutputText(const CString& outPath);
	BOOL IsChildNodeOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent);
	void treeAddBranch(const DWORD rootKey);
	void deleteNode();
	void deleteKeyNode(DWORD key, DWORD parentKey);
	void copySubNodes(HTREEITEM hOrg, HTREEITEM hNewParent);
	HTREEITEM findKeyItem(DWORD key, HTREEITEM item);
	CTreeCtrl& tree() const;
	HTREEITEM curItem() const;
	void treeConstruct();
	//{{AFX_MSG(OutlineView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnAddChild();
	afx_msg void OnUpdateAddChild(CCmdUI* pCmdUI);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddSibling();
	afx_msg void OnUpdateAddSibling(CCmdUI* pCmdUI);
	afx_msg void OnEditLabel();
	afx_msg void OnUpdateEditLabel(CCmdUI* pCmdUI);
	afx_msg void OnLebelUp();
	afx_msg void OnUpdateLebelUp(CCmdUI* pCmdUI);
	afx_msg void OnLebelDown();
	afx_msg void OnUpdateLebelDown(CCmdUI* pCmdUI);
	afx_msg void OnOrderUp();
	afx_msg void OnUpdateOrderUp(CCmdUI* pCmdUI);
	afx_msg void OnOrderDown();
	afx_msg void OnUpdateOrderDown(CCmdUI* pCmdUI);
	afx_msg void OnDelete();
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
	afx_msg void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnAddLink();
	afx_msg void OnUpdateAddLink(CCmdUI* pCmdUI);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnAddUrl();
	afx_msg void OnUpdateAddUrl(CCmdUI* pCmdUI);
	afx_msg void OnSelectChild();
	afx_msg void OnUpdateSelectChild(CCmdUI* pCmdUI);
	afx_msg void OnSelectParent();
	afx_msg void OnUpdateSelectParent(CCmdUI* pCmdUI);
	afx_msg void OnSortChildren();
	afx_msg void OnUpdateSortChildren(CCmdUI* pCmdUI);
	afx_msg void OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnExportData();
	afx_msg void OnUpdateExportData(CCmdUI* pCmdUI);
	afx_msg void OnImportData();
	afx_msg void OnUpdateImportData(CCmdUI* pCmdUI);
	afx_msg void OnEditFind();
	afx_msg void OnUpdateEditFind(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnEditReplace();
	afx_msg void OnSetFoldup();
	afx_msg void OnUpdateSetFoldup(CCmdUI* pCmdUI);
	afx_msg void OnShowSelectedBranch();
	afx_msg void OnUpdateShowSelectedBranch(CCmdUI* pCmdUI);
	afx_msg void OnShowSelectedChildren();
	afx_msg void OnUpdateShowSelectedChildren(CCmdUI* pCmdUI);
	afx_msg void OnDropFirstOrder();
	afx_msg void OnUpdateDropFirstOrder(CCmdUI* pCmdUI);
	afx_msg void OnDropLevelUp();
	afx_msg void OnUpdateDropLevelUp(CCmdUI* pCmdUI);
	afx_msg void OnCopyTreeToClipboard();
	afx_msg void OnUpdateCopyTreeToClipboard(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void moveNodes(DWORD keyTarget, DWORD keyMove);
	void catTreeLabel(HTREEITEM hItem, CString& text);
	enum {drop_none, drop_child, drop_sibling};
	int m_nDropStatus;
	HCURSOR m_hCsrCopy;
	HCURSOR m_hCsrMove;
	HTREEITEM m_hItemShowRoot;
	HTREEITEM m_HNew;
	HTREEITEM m_hItemMoved;
	HTREEITEM m_hParentPreMove;
	HTREEITEM m_hSiblingPreMove;
	nodeSrchDlg* m_pSrchDlg;
	int m_opTreeOut;
	bool m_bAddingChild;
	CImageList m_imgList;
	HTREEITEM m_hitemDragPre;
	HTREEITEM m_hitemDrop;
	HTREEITEM m_hitemDrag;
	bool m_bItemDragging;
	CFont m_font;
	HCURSOR m_hLinkCsr;
	bool m_bNodeSel;
	bool m_bAddingLink;
	bool m_bLabelEditting;
	bool m_bAdding;
	bool m_bHitR;
public:
	void setSubNodeLevels();
	void setAllNodeLevels();
	void setNodeLevels(HTREEITEM hItem, int curLevel);
	BOOL isPosterityOF(HTREEITEM hRoot, HTREEITEM hChild) const;
	afx_msg void OnAddChild2();
	afx_msg void OnUpdateAddChild2(CCmdUI *pCmdUI);
public:
//	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnCreateClone();
	afx_msg void OnUpdateCreateClone(CCmdUI *pCmdUI);
};

inline CTreeCtrl& OutlineView::tree() const
{
	return GetTreeCtrl();
}

inline HTREEITEM OutlineView::curItem() const
{
	return GetTreeCtrl().GetSelectedItem();
}

#ifndef _DEBUG  // OutLineView.cpp ファイルがデバッグ環境の時使用されます。
inline iEditDoc* OutlineView::GetDocument()
   { return (iEditDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_OUTLINEVIEW_H__96DFF9C1_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
