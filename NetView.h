#if !defined(AFX_NETVIEW_H__96DFF9CA_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
#define AFX_NETVIEW_H__96DFF9CA_1881_11D3_808A_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NetView.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// NetView ビュー
class iEditDoc;
class CRelaxThrd;
class shapesDlg;

typedef stack<CRelaxThrd*> RelaxStack;

class NetView : public CScrollView
{
protected:
	NetView();           // 動的生成に使用されるプロテクト コンストラクタ
	DECLARE_DYNCREATE(NetView)

public:
	void changeSelectedFontColor();
	void changeSelectedLineColor();
	void changeSelectedNodeColor();
	void changeSelectedLineWidth();
	void changeSelectedLinkArrow();
	void ViewDPtoLP(CRect& rect);
	void ViewLPtoDP(CRect& rect);
	void ViewLPtoDP(LPPOINT lpPoints, int nCount = 1);
	void ViewDPtoLP(LPPOINT lpPoints, int nCount = 1);
	CPoint GetLogicalCenterPoint();
	void ZoomOut(CPoint *point = NULL, double delta = 0.01);
	void ZoomIn(CPoint *point = NULL, double delta = 0.01);
	double GetZoomLevel();
	void SetScrollSizes(int nMapMode, SIZE sizeTotal, const SIZE& sizePage = sizeDefault, const SIZE& sizeLine = sizeDefault);
	void setNodeProp();
	void hideModeless();
	iEditDoc* GetDocument();
	enum {none, single, multi, link, linkTermFrom, linkTermTo};
	enum {normal, rect, rRect, arc, link0, link1, link2, label};

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(NetView)
	public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void OnDraw(CDC* pDC);      // このビューを描画するためにオーバーライドしました。
	virtual void OnInitialUpdate();     // 構築後、最初の処理。
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	void setEMF2Clpbrd(HENHMETAFILE emf);
	void cancelDragRelax();
	void prepareDragRelax();
	void setMFSize();
	void doColorSetting();
	void CenterOnLogicalPoint(CPoint ptCenter);
	void doPostSelection(const CPoint& logPt, BOOL shiftPressed);
	void doUpdateSelection(const CPoint& logPt);
	bool isAddingLink() const;
	bool isAddingNode() const;
	void preparePastePoint(const CPoint& point);
	void stopLayouting();
	bool isScaleChanged() const;
	void UpdateViewport(CPoint * ptNewCenter);
	void ReCalcBars();
	virtual void SetZoomLevel(double fNewScale);
	void drawAlterLinkTo(CDC* pDC);
	void drawAlterLinkFrom(CDC* pDC);
	void startAlterTo(const CPoint& pt);
	void startAlterFrom(const CPoint& pt);
	void sameNodesSize(const CString& strSize);
	void adjustNodesEnd(const CString& side);
	void selectAll();
	void copyMFtoClpbrd();
	afx_msg LRESULT OnRegNodeMetaFile(UINT wParam, LONG lParam);
	afx_msg LRESULT OnAddMetaFileID(UINT wParam, LONG lParam);
	afx_msg LRESULT OnHideShapesDlg(UINT wParam, LONG lParam);
	afx_msg void OnTipDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	void cursorMove(int dx, int dy);
	void adjustScrollArea();
	void KillRelaxThrd();
	void setLinkInfo();
	CRect getCurveBound(const CPoint& pt);
	void drawCurve(CDC* pDC);
	void adjustRedrawBound(CRect& rc);
	void addNode(const CPoint& logPt, const CPoint& screenPt, const CString& s="");
	void trackMulti(CPoint& logPt, CPoint& point, CDC* pDC);
	void trackSingle(CPoint& logPt, CPoint& point, CDC* pDC, BOOL keepRatio = FALSE);
	void drawAddLink(CDC* pDC);
	void startLink(const CPoint pt);
	void drawSelection(CDC* pDC);
	virtual ~NetView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(NetView)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnAddLink0();
	afx_msg void OnUpdateAddLink0(CCmdUI* pCmdUI);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSetNodeFont();
	afx_msg void OnUpdateSetNodeFont(CCmdUI* pCmdUI);
	afx_msg void OnSetNodeLineColor();
	afx_msg void OnUpdateSetNodeLineColor(CCmdUI* pCmdUI);
	afx_msg void OnSetLineSolid0();
	afx_msg void OnUpdateSetLineSolid0(CCmdUI* pCmdUI);
	afx_msg void OnSetLineSolid1();
	afx_msg void OnUpdateSetLineSolid1(CCmdUI* pCmdUI);
	afx_msg void OnSetLineSolid2();
	afx_msg void OnUpdateSetLineSolid2(CCmdUI* pCmdUI);
	afx_msg void OnSetLineSolid3();
	afx_msg void OnUpdateSetLineSolid3(CCmdUI* pCmdUI);
	afx_msg void OnSetLineSolid4();
	afx_msg void OnUpdateSetLineSolid4(CCmdUI* pCmdUI);
	afx_msg void OnSetLineDot();
	afx_msg void OnUpdateSetLineDot(CCmdUI* pCmdUI);
	afx_msg void OnSetMultiLine();
	afx_msg void OnUpdateSetMultiLine(CCmdUI* pCmdUI);
	afx_msg void OnSetSingleLine();
	afx_msg void OnUpdateSetSingleLine(CCmdUI* pCmdUI);
	afx_msg void OnAddRect();
	afx_msg void OnUpdateAddRect(CCmdUI* pCmdUI);
	afx_msg void OnAddArc();
	afx_msg void OnUpdateAddArc(CCmdUI* pCmdUI);
	afx_msg void OnAddLink1();
	afx_msg void OnUpdateAddLink1(CCmdUI* pCmdUI);
	afx_msg void OnAddLink2();
	afx_msg void OnUpdateAddLink2(CCmdUI* pCmdUI);
	afx_msg void OnSetLineNull();
	afx_msg void OnUpdateSetLineNull(CCmdUI* pCmdUI);
	afx_msg void OnSetLinkInfo();
	afx_msg void OnUpdateSetLinkInfo(CCmdUI* pCmdUI);
	afx_msg void OnDelete();
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
	afx_msg void OnAutoLayout();
	afx_msg void OnUpdateAutoLayout(CCmdUI* pCmdUI);
	afx_msg void OnSetNodeProp();
	afx_msg void OnUpdateSetNodeProp(CCmdUI* pCmdUI);
	afx_msg void OnGraspMode();
	afx_msg void OnUpdateGraspMode(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLinkStraight();
	afx_msg void OnUpdateLinkStraight(CCmdUI* pCmdUI);
	afx_msg void OnNoBrush();
	afx_msg void OnUpdateNoBrush(CCmdUI* pCmdUI);
	afx_msg void OnSetNodeBrush();
	afx_msg void OnUpdateSetNodeBrush(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnSetNodeRect();
	afx_msg void OnUpdateSetNodeRect(CCmdUI* pCmdUI);
	afx_msg void OnSetNodeArc();
	afx_msg void OnUpdateSetNodeArc(CCmdUI* pCmdUI);
	afx_msg void OnFixNode();
	afx_msg void OnUpdateFixNode(CCmdUI* pCmdUI);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNormalCursor();
	afx_msg void OnUpdateNormalCursor(CCmdUI* pCmdUI);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg void OnAddRoundRect();
	afx_msg void OnUpdateAddRoundRect(CCmdUI* pCmdUI);
	afx_msg void OnSetNodeRoundRect();
	afx_msg void OnUpdateSetNodeRoundRect(CCmdUI* pCmdUI);
	afx_msg void OnRandomize();
	afx_msg void OnUpdateRandomize(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectAll();
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnRefreshNetView();
	afx_msg void OnUpdateRefreshNetView(CCmdUI* pCmdUI);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnShowNodeShapes();
	afx_msg void OnUpdateShowNodeShapes(CCmdUI* pCmdUI);
	afx_msg void OnEditReplace();
	afx_msg void OnCopyToClipbrd();
	afx_msg void OnUpdateCopyToClipbrd(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnAdjustTop();
	afx_msg void OnUpdateAdjustTop(CCmdUI* pCmdUI);
	afx_msg void OnAdjustBottom();
	afx_msg void OnUpdateAdjustBottom(CCmdUI* pCmdUI);
	afx_msg void OnAdjustLeft();
	afx_msg void OnUpdateAdjustLeft(CCmdUI* pCmdUI);
	afx_msg void OnAdjustRight();
	afx_msg void OnUpdateAdjustRight(CCmdUI* pCmdUI);
	afx_msg void OnSameHeight();
	afx_msg void OnUpdateSameHeight(CCmdUI* pCmdUI);
	afx_msg void OnSameRect();
	afx_msg void OnUpdateSameRect(CCmdUI* pCmdUI);
	afx_msg void OnSameWidth();
	afx_msg void OnUpdateSameWidth(CCmdUI* pCmdUI);
	afx_msg void OnReverseLinkDirection();
	afx_msg void OnUpdateReverseLinkDirection(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnUpdateZoomMode(CCmdUI* pCmdUI);
	afx_msg void OnZoomMode();
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSelchangeDropdown();
	afx_msg void OnExportSvg();
	afx_msg void OnUpdateExportSvg(CCmdUI* pCmdUI);
	afx_msg void OnInsertChild();
	afx_msg void OnUpdateInsertChild(CCmdUI* pCmdUI);
	afx_msg void OnInsertSibling();
	afx_msg void OnUpdateInsertSibling(CCmdUI* pCmdUI);
	afx_msg void OnAddNodesFromCfText();
	afx_msg void OnUpdateAddNodesFromCfText(CCmdUI* pCmdUI);
	afx_msg void OnSetLinkArrowNone();
	afx_msg void OnUpdateSetLinkArrowNone(CCmdUI* pCmdUI);
	afx_msg void OnSetLinkArrowSingle();
	afx_msg void OnUpdateSetLinkArrowSingle(CCmdUI* pCmdUI);
	afx_msg void OnSetLinkArrowDouble();
	afx_msg void OnUpdateSetLinkArrowDouble(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CPoint m_curveStartPt;
	CPoint m_alterLinkStartPt;
	void deleteSelectedNodes();
	BOOL m_bGrpOlCoupled;
	BOOL m_bFormCopied;
	bool m_bAplyForm;
	void aplyFormat(CPoint& pt);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	CPoint m_dragOffset;
	bool m_bDragRelax;
	CRect m_testRect;
	CSize m_szOrigPageDev;
	CSize m_szOrigLineDev;
	CSize m_szOrigTotalDev;
	double m_fZoomScalePrev;
	double m_fZoomScale;
	BOOL m_bZooming;
	bool m_bAlteringLinkTo;
	bool m_bAlteringLinkFrom;
	COLORREF m_bkColor;
	BOOL m_bAccel;
	int m_cntUp, m_cntDown, m_cntRight, m_cntLeft;
	shapesDlg* m_pShapesDlg;
	CClientDC* m_pDC;
	RelaxStack m_relaxStack;
	CPoint m_ptNew;
	CPoint m_ptPaste;
	CPen m_linkPen;
	CPoint m_ptPrePos;
	bool m_bDragView;
	bool m_bGrasp;
	double m_mfHeight;
	double m_mfWidth;
	CPoint m_linkStart, m_linkEnd, m_linkPath;
	bool m_bLinkCurving;
	CRect m_preRedrawBound;
	bool m_bLayouting;
	CPoint m_ptLinkTo;
	CPoint m_ptLinkFrom;
	CPoint m_ptAlterLinkTo;
	CPoint m_ptAlterLinkFrom;
	CPoint m_ptAlteringAnker;
	CPoint m_ptScreen;
	bool m_bStartAdd;
	HCURSOR m_hAplyForm;
	HCURSOR m_hLabelCsr;
	HCURSOR m_hLinkCsr;
	HCURSOR m_hRectCsr;
	HCURSOR m_hRRectCsr;
	HCURSOR m_hArcCsr;
	HCURSOR m_hHandOpen;
	HCURSOR m_hHandClose;
	HCURSOR m_hLayout;
	HCURSOR m_hZoomCsr;
	int m_addMode;
	int m_selectStatus;
	CRect m_selectRect;
	// ツールチップ表示用
	CToolTipCtrl m_toolTip;
	CString m_strTip;
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnAddlinkednodeArc();
	afx_msg void OnUpdateAddlinkednodeArc(CCmdUI *pCmdUI);
	afx_msg void OnAddlinkednodeRect();
	afx_msg void OnUpdateAddlinkednodeRect(CCmdUI *pCmdUI);
	afx_msg void OnAddlinkednodeRndrect();
	afx_msg void OnUpdateAddlinkednodeRndrect(CCmdUI *pCmdUI);
	void procRenameDialog(const CRect& nodeBound);
	afx_msg void OnAddLabelOnly();
	afx_msg void OnUpdateAddLabelOnly(CCmdUI *pCmdUI);
	afx_msg void OnFontEnlarge();
	afx_msg void OnFontEnsmall();
	afx_msg void OnUpdateFontEnlarge(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFontEnsmall(CCmdUI *pCmdUI);
	afx_msg void OnSetLinkDependSingle();
	afx_msg void OnUpdateSetLinkDependSingle(CCmdUI *pCmdUI);
	afx_msg void OnSetLinkDependDouble();
	afx_msg void OnUpdateSetLinkDependDouble(CCmdUI *pCmdUI);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSetLinkInherit();
	afx_msg void OnUpdateSetLinkInherit(CCmdUI *pCmdUI);
	afx_msg void OnSetLinkAgregat();
	afx_msg void OnUpdateSetLinkAgregat(CCmdUI *pCmdUI);
	afx_msg void OnSetLinkComposit();
	afx_msg void OnUpdateSetLinkComposit(CCmdUI *pCmdUI);
	afx_msg void OnDrawOrderInfo();
	afx_msg void OnUpdateDrawOrderInfo(CCmdUI *pCmdUI);
public:
	afx_msg void OnSetNodeMm();
	afx_msg void OnUpdateSetNodeMm(CCmdUI *pCmdUI);
	afx_msg void OnValignTop();
	afx_msg void OnUpdateValignTop(CCmdUI *pCmdUI);
	afx_msg void OnAlignBottom();
	afx_msg void OnUpdateAlignBottom(CCmdUI *pCmdUI);
	afx_msg void OnHalignRight();
	afx_msg void OnUpdateHalignRight(CCmdUI *pCmdUI);
	afx_msg void OnHalignLeft();
	afx_msg void OnUpdateHalignLeft(CCmdUI *pCmdUI);
	afx_msg void OnValignCenter();
	afx_msg void OnUpdateValignCenter(CCmdUI *pCmdUI);
	afx_msg void OnHalignCenter();
	afx_msg void OnUpdateHalignCenter(CCmdUI *pCmdUI);
	afx_msg void OnBtnNodeFillColor();
	afx_msg void OnUpdateBtnNodeFillColor(CCmdUI *pCmdUI);
	afx_msg void OnBtnLineColor();
	afx_msg void OnUpdateBtnLineColor(CCmdUI *pCmdUI);
	afx_msg void OnBtnTextColor();
	afx_msg void OnUpdateBtnTextColor(CCmdUI *pCmdUI);
	afx_msg void OnBtnLinkArrow();
	afx_msg void OnUpdateBtnLinkArrow(CCmdUI *pCmdUI);
	afx_msg void OnBtnLinkLineStyle();
	afx_msg void OnUpdateBtnLinkLineStyle(CCmdUI *pCmdUI);
	afx_msg void OnSaveFormat();
	afx_msg void OnUpdateSaveFormat(CCmdUI *pCmdUI);
	afx_msg void OnAplyFormat();
	afx_msg void OnUpdateAplyFormat(CCmdUI *pCmdUI);
	afx_msg void OnGrpOlCoupled();
	afx_msg void OnUpdateGrpOlCoupled(CCmdUI *pCmdUI);
	afx_msg void OnDeleteSelectedNodes();
	afx_msg void OnUpdateDeleteSelectedNodes(CCmdUI *pCmdUI);
	afx_msg void OnDeleteSelectedLinks();
	afx_msg void OnUpdateDeleteSelectedLinks(CCmdUI *pCmdUI);
	afx_msg void OnSetLinkAngled();
	afx_msg void OnUpdateSetLinkAngled(CCmdUI *pCmdUI);
	afx_msg void OnExportEmf();
	afx_msg void OnUpdateExportEmf(CCmdUI *pCmdUI);
	afx_msg void OnExportPng();
	afx_msg void OnUpdateExportPng(CCmdUI *pCmdUI);
	afx_msg void OnSetMargin();
	afx_msg void OnUpdateSetMargin(CCmdUI *pCmdUI);
	afx_msg void OnResizeTofit();
	afx_msg void OnUpdateResizeTofit(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_NETVIEW_H__96DFF9CA_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
