// OutLineView.h : OutlineView クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_OUTLINEVIEW_H__96DFF9C1_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
#define AFX_OUTLINEVIEW_H__96DFF9C1_1881_11D3_808A_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class NodeSearchDlg;

class OutlineView : public CTreeView
{
protected:
	OutlineView();
	DECLARE_DYNCREATE(OutlineView)

public:
	iEditDoc * GetDocument();
	enum { blue, blueRoot1, blueRoot2, red, yellow, check, cancel, question, warning, face, idea };

	//{{AFX_VIRTUAL(OutlineView)
public:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	virtual void OnInitialUpdate();
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

public:
	int GetBranchMode() const;
	NodeKeyVec GetDrawOrder(const bool bShowSubBranch) const;
	void HideChildWindow();
	void SerializeTree(NodePropsVec& ls);
	void SerializeTree0(NodePropsVec& ls);
	virtual ~OutlineView();

protected:

	//{{AFX_MSG(OutlineView)
	afx_msg LRESULT OnListUpNodes(UINT wParam, LONG lParam);
	afx_msg LRESULT OnHideSrchDlg(UINT wParam, LONG lParam);
	afx_msg LRESULT OnSearchNode(UINT wParam, LONG lParam);
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
	afx_msg void OnAddChild2();
	afx_msg void OnUpdateAddChild2(CCmdUI *pCmdUI);
	afx_msg void OnCreateClone();
	afx_msg void OnUpdateCreateClone(CCmdUI *pCmdUI);
	afx_msg void OnResetShowSubbranch();
	afx_msg void OnUpdateResetShowSubbranch(CCmdUI *pCmdUI);
	afx_msg void OnTreeImageChcek();
	afx_msg void OnUpdateTreeImageChcek(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTreeImageBlue(CCmdUI *pCmdUI);
	afx_msg void OnTreeImageBlue();
	afx_msg void OnTreeImageRed();
	afx_msg void OnUpdateTreeImageRed(CCmdUI *pCmdUI);
	afx_msg void OnTreeImageYealow();
	afx_msg void OnUpdateTreeImageYealow(CCmdUI *pCmdUI);
	afx_msg void OnTreeImageCancel();
	afx_msg void OnUpdateTreeImageCancel(CCmdUI *pCmdUI);
	afx_msg void OnTreeImageQuestion();
	afx_msg void OnUpdateTreeImageQuestion(CCmdUI *pCmdUI);
	afx_msg void OnTreeImageWarning();
	afx_msg void OnUpdateTreeImageWarning(CCmdUI *pCmdUI);
	afx_msg void OnTreeImageFace();
	afx_msg void OnUpdateTreeImageFace(CCmdUI *pCmdUI);
	afx_msg void OnTreeImageIdea();
	afx_msg void OnUpdateTreeImageIdea(CCmdUI *pCmdUI);
	afx_msg void OnPasteTreeFromClipboard();
	afx_msg void OnUpdatePasteTreeFromClipboard(CCmdUI *pCmdUI);
	afx_msg void OnExportToHtml();
	afx_msg void OnUpdateExportToHtml(CCmdUI *pCmdUI);
	afx_msg void OnExportToText();
	afx_msg void OnUpdateExportToText(CCmdUI *pCmdUI);
	afx_msg void OnExportToXml();
	afx_msg void OnUpdateExportToXml(CCmdUI *pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	ExportOptions m_exportOption;

	struct TextExportOption {
		int treeOption;
		int formatOption;
		int chapterNumberOption;
		BOOL excludeLabelFromFileName;
		BOOL excludeLabelFromContent;
		CString outDir;
	} m_textExportOption;

	enum { drop_none, drop_child, drop_sibling };
	int m_nDropStatus;
	HCURSOR m_hCsrCopy;
	HCURSOR m_hCsrMove;
	HTREEITEM m_hItemShowRoot;
	HTREEITEM m_HNew;
	HTREEITEM m_hItemMoved;
	HTREEITEM m_hParentPreMove;
	HTREEITEM m_hSiblingPreMove;
	NodeSearchDlg* m_pSrchDlg;
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

	void OutputHtml();
	bool InputExportOptions();
	bool InputHtmlExportFolder();
	void SetViewFont();
	void CloneTree(const HTREEITEM& curItem, HTREEITEM targetParent, NodeKeyMap& idm);
	void ResetBranchMode();
	void ApplyColorSetting();
	void FoldupTree(HTREEITEM hItem, int curLevel, int levelSet);
	void DisableUndo();
	void OutputOutlineHtml(HTREEITEM hRoot, HTREEITEM hItem, CStdioFile& foutline, CStdioFile& ftext);
	void WriteHtmlHeader(CStdioFile& f);
	void WriteTextStyle(CStdioFile& f, bool single = true);
	bool ImportXML(const CString& inPath);
	bool ImportText(const CString& inPath, node_vec& addNodes, const char LevelChar);
	bool AddTreeAcordingToLevel(const vector<CString>& lines, node_vec& addNodes, const char levelChar = '.');
	int GetIndentCount(const CString& line, const char levelChar) const;
	void OutputOutlineText(HTREEITEM hItem, CStdioFile* f, int tab);
	void OutputOutlineTextByNode(HTREEITEM hItem);
	BOOL IsChildNodeOf(HTREEITEM hitemChild, HTREEITEM hitemSuspectedParent);
	void AddBranch(const DWORD rootKey);
	void AddBranch2(const DWORD rootKey, node_vec& addNodes);
	void DeleteNode();
	void DeleteKeyNode(DWORD key, DWORD parentKey);
	void CopySubNodes(HTREEITEM hOrg, HTREEITEM hNewParent);
	void AssignChapterNumbers();
	void AssignChapterNumber(vector<int>& numbers, const char separator, HTREEITEM hItem);
	HTREEITEM FindKeyItem(DWORD key, HTREEITEM item);
	CTreeCtrl& Tree() const;
	HTREEITEM Selected() const;
	void ConstructTree();
	void ConstructTree2();
	void CreateNodeTextFile(const CString& title, const CString& text);
	void MoveNodes(DWORD keyTarget, DWORD keyMove);
	void TreeToText(HTREEITEM hItem, CString& text);
	void RecalcSubNodeLevels();
	void RecalcAllNodeLevels();
	void RecalcNodeLevels(HTREEITEM hItem, int curLevel);
	BOOL IsPosterityOF(HTREEITEM hRoot, HTREEITEM hChild) const;
};

inline CTreeCtrl& OutlineView::Tree() const
{
	return GetTreeCtrl();
}

inline HTREEITEM OutlineView::Selected() const
{
	return GetTreeCtrl().GetSelectedItem();
}

#ifndef _DEBUG
inline iEditDoc* OutlineView::GetDocument()
{
	return (iEditDoc*)m_pDocument;
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_OUTLINEVIEW_H__96DFF9C1_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
