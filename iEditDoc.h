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
	int event;
	CString str;
	CRect preRC, curRC;
	DWORD keyTarget;
	DWORD keyParent;
};

struct colorref {
	BYTE r;
	BYTE g;
	BYTE b;
};

typedef vector<iLink> lVec;
class OutlineView;

class Memento {
private:
	int mementoType_;
	iNode node_;
	iLink link_;
public:
	enum {NodeType, LinkType};
	Memento(const iNode& node) : node_(node) {mementoType_ = Memento::NodeType;}
	Memento(const iLink& link) : link_(link) {mementoType_ = Memento::LinkType;}
	int getMementoType() { return mementoType_; }
};

typedef stack<Memento> Mementos;

class iEditDoc : public CDocument
{
	iNodes nodes_;
	iLinks links_;
	nVec nodes_undo;
	lVec links_undo;
	Mementos mements_;
protected: // シリアライズ機能のみから作成します。
	iEditDoc();
	DECLARE_DYNCREATE(iEditDoc)

// アトリビュート
public:

// オペレーション
public:
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
	void applyFormatToSelectedLink();
	void applyFormatToSelectedNode();
	void saveSelectedLinkFormat();
	void saveSelectedNodeFormat();
	void resizeSelectedNodeFont(bool bEnLarge);
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
	void SerializeXML(CArchive& ar);
	bool isCurKeyInBranch() const;
	bool isShowSubBranch() const;
	void resetShowBranch();
	CString getSubBranchRootLabel() const;
	void setShowBranch(DWORD branchRootKey);
	void setVisibleNodes(KeySet& keySet);
	iNode getHitNode(const CPoint& pt, bool bDrwAll);
	void exportSVG(bool bDrwAll, const CString& path);
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
	CString remvCR(const CString& str);
	void backUpUndoNodes2(bool bDrwAll=false);
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
	bool saveXML2(const CString &outPath);
	CString procLF(const CString& str);
	CString procCR(const CString& str);
	CString getKeyNodeText(DWORD key);
	void generateHTM(CStdioFile* f);
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
	int getSelectedNodeShape() const;
	BOOL canCopyNode();
	void makeCopyNode(const CPoint& pt, bool useDefault = true);
	void setSelectedNodeCopyOrg();
	BOOL linkExist(bool drwAll = false) const;
	void addNode2(const label& l, const CSize& mvSz);
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
	void notifySelectLink(const lsItems& ls, int index, bool drwAll=false);
	void selectChild();
	void getSelectedLinkPts(CPoint& start, CPoint& end, bool bDrwAll=false);
	void setSelectedLinkCurve(CPoint pt, bool curve=true, bool bDrwAll=false);
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
	void deleteSelectedLink(bool drwAll=false);
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
	void setSelectedNodeBound(const CRect& r);
	void moveSelectedNode(const CSize& sz);
	bool hitTest(const CPoint& pt, CRect& r, bool bDrwAll=false);
	void drawNodes(CDC* pDC, bool bDrwAll = false);
	void setCurNodeText(CString& s);
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
	CString getSafeFileName(const CString& str) const;
	OutlineView* getOutlineView() const;
	template <class T>
	void outStyleSheetLine(T &f);
	void setConnectPoint3(); // visibleなリンクの再配置
	void setConnectPoint2(); // 芋づるモードのリンク再配置用
	void saveToXML(CArchive& ar);
	bool loadFromXML(const CString& filename);
	CPoint tags2pathPt(MSXML::IXMLDOMNode* pNode);
	COLORREF tags2linkColor(MSXML::IXMLDOMNode* pNode);
	void tags2linkStyle(MSXML::IXMLDOMNode* pNode, int& style, int& width, int& arrow);
	COLORREF tags2nodeLineColor(MSXML::IXMLDOMNode* pNode);
	void tags2nodeLine(MSXML::IXMLDOMNode* pNode, int& style, int& width);
	COLORREF tags2foreColor(MSXML::IXMLDOMNode* pNode);
	void tags2bound(MSXML::IXMLDOMNode* pNode, CRect& rc);
	int tag2Shape(const CString& tag);
	int tag2Align(const CString& tag);
	bool DomTree2Nodes2(MSXML::IXMLDOMElement* node, CStdioFile* f);
	bool DomTree2Nodes3(MSXML::IXMLDOMElement* node);
	bool isKeyInLabels(const Labels& labels, DWORD key);
	double width2Len(int width);
	CString rn2br(const CString& text);
	void addImportData(bool brepRoot);
	DWORD findPairKey(const DWORD first);
	bool DomTree2Nodes(MSXML::IXMLDOMElement* node, CStdioFile* f);
	void calcMaxPt(CPoint& pt);
	void InitDocument();
	void saveOrderByTree(CArchive& ar);
	//{{AFX_MSG(iEditDoc)
	afx_msg void OnFileSaveAs();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	iNode m_nodeForFormat;
	iLink m_linkForFormat;
	CRect m_deleteBound;
	bool m_bSerializeXML;
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
	iNode nodeFind;
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
