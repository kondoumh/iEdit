// iEditDoc.h : iEditDoc クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_IEDITDOC_H__96DFF9BF_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
#define AFX_IEDITDOC_H__96DFF9BF_1881_11D3_808A_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "iNode.h"
#include "iLink.h"
#include "RelaxThrd.h"
#include "StdAfx.h"	// ClassView によって追加されました。
#include <stack>

// View への更新Hintオブジェクト
//////////////////////////////////
class iHint : public CObject {
public:
	enum {nodeSel, parentSel, nodeDelete, nodeDeleteByKey, nodeAdd, selectChild, selectParent, nodeLabelChanged,
		  rectAdd, arcAdd, nodeStyleChanged,
		  linkAdd, linkDelete, linkModified, lelax, linkCurved, linkStraight, linkSel, linkSelRet, linkListSel,
	      nextNode, nextNodeSibling, reflesh, viewSettingChanged, showSubBranch, resetShowSubBranch, nodeFontResize,
	      groupMigrate, groupMoved,
		  nodeDeleteMulti,
		  linkDeleteMulti
	};
	iHint() : treeIconId(0) {}
	int event;
	CString str;
	CRect preRC, curRC;
	DWORD keyTarget;
	DWORD keyParent;
	int treeIconId;
};

struct colorref {
	BYTE r;
	BYTE g;
	BYTE b;
};

typedef vector<iLink> lVec;
class OutlineView;

class iEditDoc : public CDocument
{
	iNodes nodes_;
	iLinks links_;
	nVec nodes_undo;
	lVec links_undo;
	int m_serialVersion;
protected: // シリアライズ機能のみから作成します。
	iEditDoc();
	DECLARE_DYNCREATE(iEditDoc)

// アトリビュート
public:

// オペレーション
public:
	void fitSetlectedNodeSize();
	bool writeClickableMap(CStdioFile& f, const CString& textFileName, bool singleText=true);
	bool saveCurrentImage(const CString& pngPath);
	CString getTitleFromPath() const;
	void swapLinkOrder(DWORD key1, DWORD key2);
	int getSerialVersion() const;
	int getInitialBranchMode() const;
	DWORD getBranchRootKey() const;
	void duplicateLinks(const IdMap& idm);
	DWORD duplicateKeyNode(DWORD key);
	void deleteLinksInBound(const CRect& bound);
	void migrateGroup();
	BOOL isDrawOrderInfo() const;
	void setDrawOrderInfo(bool bSetDrawOrderInfo);
	void moveNodesInBound(const CRect& bound, const CSize move);
	int getAppLinkWidth() const;
	int getAppLinkArrow() const;

//オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(iEditDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	void setSelectedNodeDragging(bool dragging = true);
	void divideTargetLink(DWORD key);
	CString getTitleFromOpenPath();
	int getSelectedNodeScrollPos() const;
	void setSelectedNodeScrollPos(int pos);
	void setSelectedNodeMargin(int l, int r, int t, int b);
	void getSelectedNodeMargin(int &l, int& r, int& t, int& b) const;
	void setSelectedNodeTreeIconId(int id);
	bool isOldBinary() const;
	void applyFormatToSelectedLink();
	void applyFormatToSelectedNode();
	void saveSelectedLinkFormat();
	void saveSelectedNodeFormat();
	void resizeSelectedNodeFont(bool bEnLarge);
	void resizeSelectedLinkFont(bool bEnLarge);
	bool hitTest2(const CPoint& pt);
	int getKeyNodeLevelNumber(DWORD key);
	void addNodeInternal(const CString& name, const CPoint& pt, int nodeType, bool bEnableMultiLineProcess, bool bNoBound = false);
	const CRect addNodeWithLink2(int nodeType, DWORD keyPrevSibling);
	void relaxSingleStep2();
	const iNode& insertNode(const int nodeType, const CString& name, const CPoint& pt);
	const CRect addNodeWithLink(int nodeType, DWORD keyRoot, DWORD keyPrevSibling = -1, const CPoint& pt = CPoint(0, 0), bool bMindMap = true);
	void recalcArea();
	CRect getChaindNodesBound() const;
	void listupChainNodes(bool bResetLinkCurve=true);
	void relaxSingleStep(const CPoint& point, const CPoint& dragOffset);
	void calcEdges();
	void setNodeLevel(const DWORD key, const int nLevel);
	void backupDeleteBound();
	CRect restoreDeleteBound() const;
	bool isCurKeyInBranch() const;
	bool isShowSubBranch() const;
	void resetShowBranch();
	CString getSubBranchRootLabel() const;
	void setShowBranch(DWORD branchRootKey);
	void setVisibleNodes(KeySet& keySet);
	iNode getHitNode(const CPoint& pt, bool bDrwAll);
	void exportSVG(bool bDrwAll, const CString& path, bool bEmbed = false,
		const CString& textFileName = _T(""), bool textSingle = true);
	void viewSettingChanged();
	void drawLinkSelectionTo(CDC* pDC, bool bDrwAll);
	void drawLinkSelectionFrom(CDC* pDC, bool bDrwAll=false);
	void setSelectedLinkReverse(bool bDrwAll = false);
	bool setAlterLinkTo(const CPoint& pt, bool bDrwAll);
	bool setAlterLinkFrom(const CPoint& pt, bool bDrwAll);
	const iLink* getSelectedLink(bool bDrawAll=false) const;
	bool hitTestLinksTo(const CPoint &pt, bool drwAll);
	bool hitTestLinksFrom(const CPoint& pt, bool drwAll);
	void sameNodesSize(const CString& strSize, bool bDrwAll = false);
	void adjustNodesEnd(const CString& side, const CRect& rect, bool bDrwAll = false);
	CString getKeyNodeLabel(DWORD key);
	void backUpUndoLinks();
	void backUpUndoNodes();
	void resumeUndoLinks();
	void disableUndo();
	bool canResumeUndo();
	void resumeUndoNodes();
	CRect getRelatedBoundAnd(bool drwAll);
	void drawLinksSelected(CDC* pDC, bool bDrwAll, bool clipbrd);
	void drawNodesSelected(CDC* pDC, bool bDrwAll);
	HENHMETAFILE getSelectedNodeMetaFile();
	void listUpNodes(const CString& sfind, Labels& labels, BOOL bLabel = TRUE, BOOL bText = FALSE, BOOL bLinks = FALSE, BOOL bUpper = FALSE);
	void addNodeMF(const CString& name, const CPoint& pt, int mfIndex, HENHMETAFILE& mh);
	void setConnectPoint();
	CString procLF(const CString& str);
	CString procCR(const CString& str);
	CString getKeyNodeText(DWORD key);
	void writeTextHtml(DWORD key, CStdioFile* f, bool textIsolated = false, const CString& textPrefix = _T(""));
	void randomNodesPos(const CSize& area, bool bDrwAll=false);
	bool saveXML(const CString& outPath, bool bSerialize = false);
	bool loadXML(const CString& filename, bool replace=false);
	void addNodeRoundRect(const CString& name, const CPoint& pt);
	void setResultRelax(Bounds& bounds);
	void setNodeRelax(CRelaxThrd* r, bool bDrwAll=false);
	BOOL isSelectedNodeFixed() const;
	void setSelectedNodeFixed(BOOL f=TRUE);
	void setSelectedNodeTextStyle(int style);
	int getSelectedNodeTextStyle() const;
	void setSelectedNodeLabel(const CString& label);
	void setSelectedNodeShape(int shape, int mfIndex=0);
	void setSelectedNodeMetaFile(HENHMETAFILE metafile);
	int getSelectedNodeShape() const;
	BOOL canCopyNode();
	void makeCopyNode(const CPoint& pt, bool useDefault = true);
	void setSelectedNodeCopyOrg();
	BOOL linkExist(bool drwAll = false) const;
	void addNode2(const iNode& n);
	int getDataSize() const;
	void setSelectedNodeNoBrush(BOOL noBrush=TRUE);
	COLORREF getSelectedNodeBrsColor() const;
	BOOL isSelectedNodeFilled() const;
	void setSelectedNodeBrush(const COLORREF &c);
	void moveSelectedLink(const CSize &sz);
	BOOL canCopyLink();
	void addSetLinkOrg();
	void setCpLinkOrg();
	BOOL isSelectedLinkCurved(bool bDrwAll=false) const;
	BOOL isSelectedLinkSelf() const;
	void notifySelectLink(const lsItems& ls, int index, bool drwAll=false);
	void selectChild();
	void getSelectedLinkPts(CPoint& start, CPoint& end, bool bDrwAll=false);
	void setSelectedLinkCurve(CPoint pt, bool curve=true, bool bDrwAll=false);
	void setSelectedLinkAngled(bool angled = true);
	CRect getRelatedBound(bool drwAll = false) const;
	CRect getSelectedLinkBound(bool drwAll=false) const;
	void setSpecifiedLinkInfo(const listitem& iOld, const listitem& iNew);
	void deleteSpecifidLink(const listitem& i);
	DWORD getSelectedNodeKey() const;
	void addURLLink(const CString& url, const CString& comment);
	void getLinkInfoList(lsItems& ls, bool drwAll=false);
	bool canDeleteNode() const;
	CString getSelectedLinkLabel(bool drawAll=false);
	CString getSelectedNodeLabel();
	void deleteSelectedNode();
	void deleteSelectedNodes();
	void deleteSelectedLink();
	void deleteSelectedLink2();
	BOOL RouteCmdToAllViews(CView * pView, UINT nID, int nCode, void * pExtra, AFX_CMDHANDLERINFO * pHandlerInfo);
	void getSelectedLinkFont(LOGFONT& lf, bool drwAll=false);
	void setSelectedLinkFont(const LOGFONT& lf, bool drwAll=false);
	void setSelectedLinkLineColor(const COLORREF& c, bool drwAll=false);
	COLORREF getSelectedLinkLineColor(bool drwAll=false) const;
	int getSelectedLinkLineStyle(bool drwAll=false);
	void setSelectedLinkLineStyle(int style, bool drwAll=false);
	void setSelectedLinkWidth(int w, bool drwAll=false);
	int getSelectedLinkWidth(bool drwAll = false) const;
	void selectLinksInBound(const CRect& r, bool drwAll = false);
	void setSelectedLinkInfo(const CString& sComment, int arrowType, bool bDrwAll=false);
	void getSelectedLinkInfo(CString& sFrom, CString& sTo, CString& sComment, int& arrowType, bool bDrwAll=false);
	void setNewLinkInfo(DWORD keyFrom, DWORD keyTo, const CString& comment, int styleArrow);
	void drawLinkSelection(CDC* pDC, bool bDrwAll = false);
	bool hitTestLinks(const CPoint& pt, bool drwAll = false);
	DWORD hitTestDropTarget(const CPoint& pt, const DWORD selectedNodeKey);
	void addNodeArc(const CString& name, const CPoint& pt);
	void addNodeRect(const CString& name, const CPoint& pt, bool bSetMultiLineProcess = true, bool bNoBound = false);
	CRect getRecentNodeRect();
	CRect getSelectedNodeRect() const;
	int selectNodesInBound(const CRect& r, CRect& selRect, bool drwAll=false);
	void setSelectedNodeMultiLine(bool set=true);
	bool isSelectedNodeMultiLine() const;
	int getSelectedNodeLineWidth() const;
	void setSelectedNodeLineWidth(int w);
	int getSelectedNodeLineStyle() const;
	void setSelectedNodeLineStyle(int style);
	COLORREF getSelectedNodeLineColor() const;
	void setSelectedNodeLineColor(const COLORREF& c);
	void setSelectedNodeFontColor(const COLORREF& c);
	COLORREF getSelectedNodeFontColor() const;
	void getSelectedNodeFont(LOGFONT& lf);
	void setSelectedNodeFont(const LOGFONT& lf);
	const CPoint& getMaxPt() const;
	bool setEndLink(const CPoint& pt, int arrowType=iLink::line, bool bDrwAll=false, bool bArrowSpecification=false);
	bool setStartLink(const CPoint& pt);
	void drawLinks(CDC* pDC, bool bDrwAll=false, bool clipbrd=false);
	void setSelectedNodeBound(const CRect& r, bool widthLink = true, bool noBackup = false);
	void moveSelectedNode(const CSize& sz);
	bool hitTest(const CPoint& pt, CRect& r, bool bDrwAll=false);
	void drawNodes(CDC* pDC, bool bDrwAll = false);
	void setCurNodeText(CString& s, int scrollPos);
	void selChanged(DWORD key, bool reflesh=true, bool bShowSubBranch = false);
	CString getSelectedNodeText();
	void deleteKeyItem(DWORD key);
	void setKeyNodeParent(DWORD key, DWORD parent);
	bool setKeyNodeName(DWORD key, const CString& name);
	DWORD getUniqKey();
	void addNode(const label& l, DWORD inheritKey = 0, bool bInherit = false);
	void copyNodeLabels(Labels& v);
	virtual ~iEditDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成されたメッセージ マップ関数
protected:
	CString procWikiNotation(const CString& text);
	void beginUL(CString& str, int& level, int& prevLevel);
	void endUL(CString& str, int& level);
	CString makeInlineUrlLink(const CString& line);
	OutlineView* getOutlineView() const;
	template <class T>
	void outStyleSheetLine(T &f);
	void setConnectPoint2(); // 芋づるモードのリンク再配置用
	void setConnectPoint3(); // visibleなリンクの再配置
	bool loadFromXML(const CString& filename);
	CPoint tags2pathPt(MSXML2::IXMLDOMNode* pNode);
	COLORREF tags2linkColor(MSXML2::IXMLDOMNode* pNode);
	void tags2linkStyle(MSXML2::IXMLDOMNode* pNode, int& style, int& width, int& arrow);
	COLORREF tags2nodeLineColor(MSXML2::IXMLDOMNode* pNode);
	void tags2nodeLine(MSXML2::IXMLDOMNode* pNode, int& style, int& width);
	COLORREF tags2foreColor(MSXML2::IXMLDOMNode* pNode);
	void tags2bound(MSXML2::IXMLDOMNode* pNode, CRect& rc);
	int tag2Shape(const CString& tag);
	int tag2Align(const CString& tag);
	bool DomTree2Nodes2(MSXML2::IXMLDOMElement* node, CStdioFile* f);
	bool DomTree2Nodes3(MSXML2::IXMLDOMElement* node);
	bool isKeyInLabels(const Labels& labels, DWORD key);
	double width2Len(int width);
	void addImportData(bool brepRoot);
	DWORD findPairKey(const DWORD first);
	void calcMaxPt(CPoint& pt);
	void InitDocument();
	void saveOrderByTree(CArchive& ar);
	void saveOrderByTreeEx(CArchive& ar, int version);
	void saveTreeState(CArchive& ar, int version);
	void loadTreeState(CArchive& ar, int version);
	//{{AFX_MSG(iEditDoc)
	afx_msg void OnFileSaveAs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_openFilePath;
	int m_initialBranchMode;
	iNode m_nodeForFormat;
	iLink m_linkForFormat;
	CRect m_deleteBound;
	bool m_bSerializeXML;
	bool m_bLoadXmlSucceeded;
	bool m_bOldBinary;
	KeySet m_visibleKeys;
	DWORD m_dwBranchRootKey;
	CPoint m_pathPtImport;
	colorref m_fcolorImport, m_lcolorImport, m_ncolorImport;
	CRect m_boundImport;
	bool m_bShowBranch;
	lVec linksImport;
	nVec nodesImport;
	iLink linkImport;
	iNode nodeImport;
	idCVec idcVec;
	CPoint ptSelectMin;
	serialVec sv;
	serialVec copyOrg;
	BOOL canCpyLink;
	iLink m_cpLinkOrg;
	CPoint m_maxPt;
	DWORD curParent;
	DWORD keyLinkTo;
	CRect rcLinkTo;
	DWORD keyLinkFrom;
	DWORD keyParentLinkFrom;
	CRect rcLinkFrom;
	DWORD lastKey, lastLinkKey;
	void desideLinkLineStyle(iLink& l);
	void desideLinkArrow(iLink& l);
public:
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_IEDITDOC_H__96DFF9BF_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
