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
#include "StdAfx.h"
#include <stack>

typedef pair<DWORD, DWORD> NodeKeyPair;
typedef vector<NodeKeyPair> NodeKeyPairs;

// View への更新Hintオブジェクト
//////////////////////////////////
class iHint : public CObject {
public:
	enum {
		nodeSel, parentSel, nodeDelete, nodeDeleteByKey, nodeAdd, selectChild, selectParent, nodeLabelChanged,
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

struct ColorRef {
	BYTE r;
	BYTE g;
	BYTE b;
};

typedef vector<iLink> iLinkVec;
class OutlineView;

class iEditDoc : public CDocument
{
	iNodes nodes_;
	iLinks links_;
	nVec nodes_undo;
	iLinkVec links_undo;
	int m_serialVersion;

protected:
	iEditDoc();
	DECLARE_DYNCREATE(iEditDoc)

public:
	void FitSelectedNodeSize();
	bool WriteClickableMap(CStdioFile& f, const CString& textFileName, bool singleText = true);
	bool SaveCurrentImage(const CString& pngPath);
	CString GetFileNameFromPath() const;
	void SwapLinkOrder(DWORD key1, DWORD key2);
	int GetSerialVersion() const;
	int GetInitialBranchMode() const;
	DWORD GetBranchRootKey() const;
	void DuplicateLinks(const NodeKeyMap& idm);
	DWORD DuplicateKeyNode(DWORD key);
	void DeleteLinksInBound(const CRect& bound);
	void MigrateGroup();
	BOOL DrawOrderInfo() const;
	void SetDrawOrderInfo(bool bSetDrawOrderInfo);
	void MoveNodesInBound(const CRect& bound, const CSize move);
	int GetAppLinkWidth() const;
	int GetAppLinkArrow() const;

	//{{AFX_VIRTUAL(iEditDoc)
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

	void SetSelectedNodeDragging(bool dragging = true);
	void DivideTargetLink(DWORD key);
	int GetSelectedNodeScrollPos() const;
	void SetSelectedNodeScrollPos(int pos);
	void SetSelectedNodeMargin(int l, int r, int t, int b);
	void GetSelectedNodeMargin(int &l, int& r, int& t, int& b) const;
	void SetSelectedNodeTreeIconId(int id);
	bool IsOldBinary() const;
	void ApplyFormatToSelectedLink();
	void ApplyFormatToSelectedNode();
	void SaveSelectedLinkFormat();
	void SaveSelectedNodeFormat();
	void ResizeSelectedNodeFont(bool bEnLarge);
	void ResizeSelectedLinkFont(bool bEnLarge);
	bool HitTest2(const CPoint& pt);
	int GetKeyNodeLevelNumber(DWORD key);
	const CRect AddNodeWithLink2(int nodeType, DWORD keyPrevSibling);
	const CRect AddNodeWithLink(int nodeType, DWORD keyRoot, DWORD keyPrevSibling = -1, const CPoint& pt = CPoint(0, 0), bool bMindMap = true);
	void RecalcArea();
	CRect GetChaindNodesBound() const;
	void ListupChainNodes(bool bResetLinkCurve = true);
	void RelaxSingleStep(const CPoint& point, const CPoint& dragOffset);
	void CalcEdges();
	void SetNodeLevel(const DWORD key, const int nLevel);
	void BackupDeleteBound();
	CRect RestoreDeleteBound() const;
	bool CurKeyInBranch() const;
	bool ShowSubBranch() const;
	void ResetShowBranch();
	CString GetSubBranchRootLabel() const;
	void SetShowBranch(DWORD branchRootKey);
	void SetVisibleNodes(NodeKeySet& keySet);
	iNode GetHitNode(const CPoint& pt);
	void ExportSvg(const CString& path, bool bEmbed = false,
		const CString& textFileName = _T(""), bool textSingle = true);
	void ViewSettingChanged();
	void DrawLinkSelectionTo(CDC* pDC);
	void DrawLinkSelectionFrom(CDC* pDC);
	void ReverseSelectedLinkDirection();
	bool SwitchLinkEndNodeAt(const CPoint& pt);
	bool SwitchLinkStartNodeAt(const CPoint& pt);
	const iLink* GetSelectedLink(bool bDrawAll = false) const;
	bool SelectLinkEndIfHit(const CPoint &pt, bool drwAll);
	bool SelectLinkStartIfHit(const CPoint& pt, bool drwAll);
	void AlignSelectedNodesToSameSize(const CString& strSize);
	void AlignNodesInBoundTo(const CString& side, const CRect& rect);
	CString GetKeyNodeLabel(DWORD key);
	void BackupLinksForUndo();
	void BackupNodesForUndo();
	void RestoreLinksForUndo();
	void RestoreNodesForUndo();
	void DisableUndo();
	bool CanUndo();
	CRect GetRelatedBoundAnd(bool drwAll);
	void DrawLinksSelected(CDC* pDC, bool clipbrd);
	void DrawNodesSelected(CDC* pDC);
	HENHMETAFILE GetSelectedNodeMetaFile();
	void ListUpNodes(const CString& sfind, NodePropsVec& labels, BOOL bLabel = TRUE, BOOL bText = FALSE, BOOL bLinks = FALSE, BOOL bUpper = FALSE);
	void AddShapeNode(const CString& name, const CPoint& pt, int mfIndex, HENHMETAFILE& mh);
	void SetConnectionPoint();
	CString GetKeyNodeText(DWORD key);
	void WriteKeyNodeToHtml(DWORD key, CStdioFile* f, bool textIsolated = false, const CString& textPrefix = _T(""));
	void RandomizeNodesPos(const CSize& area);
	bool SaveXml(const CString& outPath, bool bSerialize = false);
	bool LoadXml(const CString& filename, bool replace = false);
	void AddNodeRoundedRect(const CString& name, const CPoint& pt);
	void SetResultRelax(Bounds& bounds);
	void SetNodeRelax(CRelaxThrd* r);
	BOOL IsSelectedNodeFixed() const;
	void SetSelectedNodeFixed(BOOL f = TRUE);
	void SetSelectedNodeTextStyle(int style);
	int GetSelectedNodeTextStyle() const;
	void SetSelectedNodeLabel(const CString& label);
	void SetSelectedNodeShape(int shape, int mfIndex = 0);
	void SetSelectedNodeMetaFile(HENHMETAFILE metafile);
	int GetSelectedNodeShape() const;
	BOOL CanDuplicateNodes();
	void DuplicateNodes(const CPoint& pt, bool useDefault = true);
	void AddSelectedNodesToCopyOrg();
	BOOL LinksExist() const;
	void addNode2(const iNode& n);
	void SetSelectedNodeNoBrush(BOOL noBrush = TRUE);
	COLORREF GetSelectedNodeBrsColor() const;
	BOOL IsSelectedNodeFilled() const;
	void SetSelectedNodeBrush(const COLORREF &c);
	void MoveSelectedLink(const CSize &sz);
	BOOL canCopyLink();
	void addSetLinkOrg();
	void setCpLinkOrg();
	BOOL IsSelectedLinkCurved() const;
	BOOL IsSelectedLinkSelfReferential() const;
	void NotifyLinkSelected(const LinkPropsVec& ls, int index);
	void GetSelectedLinkEndPoints(CPoint& start, CPoint& end);
	void CurveSelectedLink(CPoint pt, bool curve = true);
	void AngleSelectedLink(bool angled = true);
	CRect GetRelatedBound() const;
	CRect GetSelectedLinkBound() const;
	void SetSpecifiedLinkProps(const LinkProps& iOld, const LinkProps& iNew);
	void DeleteSpecifidLink(const LinkProps& i);
	DWORD GetSelectedNodeKey() const;
	void AddUrlLink(const CString& url, const CString& comment);
	void CollectLinkProps(LinkPropsVec& ls, bool drwAll = false);
	bool CanDeleteNode() const;
	CString GetSelectedLinkLabel(bool drawAll = false);
	CString GetSelectedNodeLabel();
	void DeleteSelectedNode();
	void DeleteSelectedNodes();
	void DeleteSelectedLink();
	void DeleteSelectedLink2();
	BOOL RouteCmdToAllViews(CView * pView, UINT nID, int nCode, void * pExtra, AFX_CMDHANDLERINFO * pHandlerInfo);
	void GetSelectedLinkFont(LOGFONT& lf, bool drwAll = false);
	void SetSelectedLinkFont(const LOGFONT& lf, bool drwAll = false);
	void SetSelectedLinkLineColor(const COLORREF& c, bool drwAll = false);
	COLORREF GetSelectedLinkLineColor(bool drwAll = false) const;
	int GetSelectedLinkLineStyle(bool drwAll = false);
	void SetSelectedLinkLineStyle(int style, bool drwAll = false);
	void SetSelectedLinkWidth(int w, bool drwAll = false);
	int GetSelectedLinkWidth(bool drwAll = false) const;
	void SelectLinksInBound(const CRect& r, bool drwAll = false);
	void SetSelectedLinkInfo(const CString& sComment, int arrowType);
	void GetSelectedLinkInfo(CString& sFrom, CString& sTo, CString& sComment, int& arrowType);
	void SetNewLinkInfo(DWORD keyFrom, DWORD keyTo, const CString& comment, int styleArrow);
	void DrawLinkSelection(CDC* pDC);
	bool HitTestLinks(const CPoint& pt, bool drwAll = false);
	DWORD HitTestDropTarget(const CPoint& pt, const DWORD selectedNodeKey);
	void AddNodeArc(const CString& name, const CPoint& pt);
	void AddNodeRect(const CString& name, const CPoint& pt, bool bSetMultiLineProcess = true, bool bNoBound = false);
	CRect GetSelectedNodeRect() const;
	int SelectNodesInBound(const CRect& r, CRect& selRect, bool drwAll = false);
	void SetSelectedNodeMultiLine(bool set = true);
	bool IsSelectedNodeMultiLine() const;
	int GetSelectedNodeLineWidth() const;
	void SetSelectedNodeLineWidth(int w);
	int GetSelectedNodeLineStyle() const;
	void SetSelectedNodeLineStyle(int style);
	COLORREF GetSelectedNodeLineColor() const;
	void SetSelectedNodeLineColor(const COLORREF& c);
	void SetSelectedNodeFontColor(const COLORREF& c);
	COLORREF GetSelectedNodeFontColor() const;
	void GetSelectedNodeFont(LOGFONT& lf);
	void SetSelectedNodeFont(const LOGFONT& lf);
	const CPoint& GetMaxPt() const;
	bool SetEndLink(const CPoint& pt, int arrowType = iLink::line, bool bArrowSpecification = false);
	bool SetStartLink(const CPoint& pt);
	void DrawLinks(CDC* pDC, bool clipbrd = false);
	void SetSelectedNodeBound(const CRect& r, bool widthLink = true, bool noBackup = false);
	void MoveSelectedNode(const CSize& sz);
	bool HitTest(const CPoint& pt, CRect& r);
	void DrawNodes(CDC* pDC);
	void SetCurrentNodeText(CString& s, int scrollPos);
	void SelectionChanged(DWORD key, bool reflesh = true, bool bShowSubBranch = false);
	CString GetSelectedNodeText();
	void DeleteKeyItem(DWORD key);
	void SetKeyNodeParent(DWORD key, DWORD parent);
	bool SetKeyNodeName(DWORD key, const CString& name);
	CString GetKeyNodeChapterNumber(DWORD key);
	void SetKeyNodeChapterNumber(DWORD key, const CString& chapterNumber);
	DWORD AssignNewKey();
	void addNode(const NodeProps& l, DWORD inheritKey = 0, bool bInherit = false);
	void copyNodeLabels(NodePropsVec& v);
	virtual ~iEditDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

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
	bool isKeyInLabels(const NodePropsVec& labels, DWORD key);
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
	afx_msg void OnFileSave();
	afx_msg void OnUpdateFileSave(CCmdUI *pCmdUI);
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
	NodeKeySet m_visibleKeys;
	DWORD m_dwBranchRootKey;
	CPoint m_pathPtImport;
	ColorRef m_fcolorImport, m_lcolorImport, m_ncolorImport;
	CRect m_boundImport;
	bool m_bShowBranch;
	iLinkVec linksImport;
	nVec nodesImport;
	iLink linkImport;
	iNode nodeImport;
	NodeKeyPairs idcVec;
	CPoint ptSelectMin;
	NodeKeyVec sv;
	NodeKeyVec copyOrg;
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

	const iNode& InsertNode(const int nodeType, const CString& name, const CPoint& pt);
	void RelaxSingleStep2();
	void desideLinkLineStyle(iLink& l);
	void desideLinkArrow(iLink& l);
	void AddNodeInternal(const CString& name, const CPoint& pt, int nodeType, bool bEnableMultiLineProcess, bool bNoBound = false);
	CString GetFileNameFromOpenPath();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_IEDITDOC_H__96DFF9BF_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
