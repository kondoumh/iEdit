// iEditDoc.cpp : iEditDoc クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "iEdit.h"

#include "iEditDoc.h"
#include "OutlineView.h"
#include <algorithm>
#include <complex>

#include "SvgWriter.h"
#include <shlwapi.h>
#include "Utilities.h"
#include <atlimage.h>
#include <regex>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CHECKHR(x) {hr = x; if (FAILED(hr)) goto CleanUp;}
#define SAFERELEASE(p) {if (p) {(p)->Release(); p = NULL;}}

/// iNode のPredicate(キーと等しいかどうか)
class iNode_eq : public unary_function<iNode, bool>
{
	DWORD key_;
public:
	explicit iNode_eq(DWORD key) : key_(key) { }
	bool operator()(const iNode& n) const { return n.getKey() == key_; }
};

/// iLink のPredicate リンクの道連れ削除判定
class iLink_eq : public unary_function<iLink, bool>
{
	DWORD key_;  // 削除されるnodeのキー
public:
	explicit iLink_eq(DWORD key) : key_(key) { }
	bool operator()(const iLink& l) const {
		return (l.getKeyFrom() == key_ || l.getKeyTo() == key_);
	}
};

/// iLink のPredicate リンクが矩形内にあるかどうかを判定
class iLink_inBound : public unary_function<iLink, CRect>
{
	CRect bound_;
public:
	explicit iLink_inBound(CRect bound) : bound_(bound) {}
	bool operator()(const iLink& l) const {
		return l.canDraw() &&
			bound_.PtInRect(l.getPtFrom()) && 
			bound_.PtInRect(l.getPtTo());
	}
};

/////////////////////////////////////////////////////////////////////////////
// iEditDoc

IMPLEMENT_DYNCREATE(iEditDoc, CDocument)

BEGIN_MESSAGE_MAP(iEditDoc, CDocument)
	//{{AFX_MSG_MAP(iEditDoc)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_SAVE, &iEditDoc::OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, &iEditDoc::OnUpdateFileSave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// iEditDoc クラスの構築/消滅

iEditDoc::iEditDoc()
{
	// TODO: この位置に１度だけ呼ばれる構築用のコードを追加してください。
	m_bShowBranch = false;
	m_initialBranchMode = 0;
	m_bOldBinary = false;
	m_serialVersion = 0;
}

iEditDoc::~iEditDoc()
{
}

BOOL iEditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: この位置に再初期化処理を追加してください。
	// (SDI ドキュメントはこのドキュメントを再利用します。)
	InitDocument();
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// iEditDoc シリアライゼーション

void iEditDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
 	{
 		// TODO: この位置に保存用のコードを追加してください。
		if (m_bSerializeXML) {
			SerializeXML(ar);
		} else {
			if (m_bOldBinary) {
				saveOrderByTree(ar); // ノードの保存
				// リンクの保存
				ar << links_.size();
				literator li = links_.begin();
				for ( ; li != links_.end(); li++) {
					(*li).Serialize(ar);
				}
			} else {
			 	const int SERIAL_VERSION = 5; // シリアル化バージョン番号
				ar << SERIAL_VERSION;
				saveOrderByTreeEx(ar, SERIAL_VERSION); // ノードの保存
				// リンクの保存
				ar << links_.size(); 
				literator li = links_.begin();
				for ( ; li != links_.end(); li++) {
					(*li).SerializeEx(ar, SERIAL_VERSION);
				}
				// OutlineView状態の書き込み
				saveTreeState(ar, SERIAL_VERSION);
			}
		}
 	}
 	else
 	{
 		// TODO: この位置に読み込み用のコードを追加してください。
		
		
		if (m_bSerializeXML) {
			SerializeXML(ar);
		} else {
			if (m_bOldBinary) {
				// ノードの読み込み
				ar >> lastKey;
 				unsigned int count;
 				ar >> count;
	 			for (unsigned int i = 0; i < count; i++) {
 					iNode n;
 					n.Serialize(ar);
 					nodes_.insert(n);
					sv.push_back(n.getKey());
 				}
				// リンクの読み込み
				ar >> count;
				lastLinkKey = 0;
				for (unsigned int i = 0; i < count; i++) {
					iLink l;
					l.Serialize(ar);
					l.setKey(lastLinkKey++);
					links_.push_back(l);
				}
			} else {
				// ノードの読み込み
				int version;
				ar >> version;
				m_serialVersion = version;
				ar >> lastKey;
 				unsigned int count;
 				ar >> count;
	 			for (unsigned int i = 0; i < count; i++) {
 					iNode n;
 					n.SerializeEx(ar, version);
 					nodes_.insert(n);
					sv.push_back(n.getKey());
 				}
				// リンクの読み込み
				ar >> count;
				lastLinkKey = 0;
				for (unsigned int i = 0; i < count; i++) {
					iLink l;
					l.SerializeEx(ar, version);
					l.setKey(lastLinkKey++);
					links_.push_back(l);
				}
				// OutlineView状態の読み込み
				loadTreeState(ar, version);
			}
		}
 	}
}

void iEditDoc::SerializeXML(CArchive &ar)
{
	if (ar.IsStoring())
	{
		saveToXML(ar);
	}
	else
	{
		loadFromXML(ar.GetFile()->GetFilePath());
	}
}


void iEditDoc::saveOrderByTree(CArchive& ar)
{
	OutlineView* pView = getOutlineView();
	Labels ls;
	pView->treeToSequence0(ls);  // シリアライズ専用シーケンス取得
	ar << lastKey;
	ar << ls.size();
	for (unsigned int i = 0; i < ls.size(); i++) {
		nodeFind.setKey(ls[i].key);
		niterator it = nodes_.findNodeW(nodeFind);
		if (it != nodes_.end()) {
			(*it).setTreeState(ls[i].state);
			(*it).Serialize(ar);
		}
	}
}

void iEditDoc::saveOrderByTreeEx(CArchive &ar, int version)
{
	OutlineView* pView = getOutlineView();
	Labels ls;
	pView->treeToSequence0(ls);  // シリアライズ専用シーケンス取得
	ar << lastKey;
	ar << ls.size();
	for (unsigned int i = 0; i < ls.size(); i++) {
		nodeFind.setKey(ls[i].key);
		niterator it = nodes_.findNodeW(nodeFind);
		if (it != nodes_.end()) {
			(*it).setTreeState(ls[i].state);
			(*it).SerializeEx(ar, version);
		}
	}
}

void iEditDoc::saveTreeState(CArchive &ar, int version)
{
	OutlineView* pView = getOutlineView();
	ar << pView->getBranchMode();
	ar << m_dwBranchRootKey;
}

void iEditDoc::loadTreeState(CArchive &ar, int version)
{
	ar >> m_initialBranchMode;
	if (m_initialBranchMode == 1 || m_initialBranchMode == 2) {
		m_bShowBranch = true;
	}
	DWORD branchRootKey;
	ar >> branchRootKey;

	m_dwBranchRootKey = branchRootKey;
	calcMaxPt(m_maxPt);
}


/////////////////////////////////////////////////////////////////////////////
// iEditDoc クラスの診断

#ifdef _DEBUG
void iEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void iEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// iEditDoc コマンド

DWORD iEditDoc::getBranchRootKey() const
{
	return m_dwBranchRootKey;
}

int iEditDoc::getInitialBranchMode() const
{
	return m_initialBranchMode;
}

void iEditDoc::copyNodeLabels(Labels &v)
{
	for(unsigned int i = 0; i < sv.size(); i++) {
		nodeFind.setKey(sv[i]);
		const_niterator it = nodes_.findNode(nodeFind);
		label l;
		l.name = (*it).getName();
		l.key = (*it).getKey();
		l.parent = (*it).getParent();
		l.state = (*it).getTreeState();
		l.level = (*it).getLevel();
		l.treeIconId = (*it).getTreeIconId();
		v.push_back(l);
	}
	sv.clear();
	sv.resize(0);
}

// OutlineViewでのノード追加用メソッド
void iEditDoc::addNode(const label &l, DWORD inheritKey, bool bInherit)
{
	nodeFind.setKey(inheritKey);
	const_niterator it = nodes_.findNode(nodeFind);
	iNode n(l.name);
	n.setKey(l.key);
	n.setParent(l.parent);
	
	if (isShowSubBranch()) {
		m_visibleKeys.insert(l.key);
	}
	
	if (bInherit && it != nodes_.end()) {
		n.setLineColor((*it).getLineColor());
		n.setLineWidth((*it).getLineWidth());
		n.setLineStyle((*it).getLineStyle());
		n.setBrush((*it).getBrsColor());
		n.setNoBrush((*it).isFilled());
		n.setNodeShape((*it).getNodeShape());
		n.setTextStyle((*it).getTextStyle());
		n.setFontColor((*it).getFontColor());
		n.setFontInfo((*it).getFontInfo());
	}
	
	if (it != nodes_.end()) {
		CRect rc = (*it).getBound();
		n.moveBound(CSize(rc.left+20, rc.top+30));
	}
	
	nodes_.insert(n);
	selChanged(l.key, true, isShowSubBranch());
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeAdd;
	UpdateAllViews(NULL, (LPARAM)l.key, &hint);
}

// OutlineViewのインポートテキストファイル専用に作ってる
void iEditDoc::addNode2(const iNode &n)
{
	nodes_.insert(n);
	sv.push_back(n.getKey());
	SetModifiedFlag();
}

DWORD iEditDoc::getUniqKey()
{
	return ++lastKey;
}

bool iEditDoc::setKeyNodeName(DWORD key, const CString &name)
{
	nodeFind.setKey(key);
	niterator it = nodes_.findNodeW(nodeFind);
	if (it == nodes_.end()) {
		return false;
	}
	(*it).setName(name);
	SetModifiedFlag();
	UpdateAllViews(NULL);
	return true;
}

void iEditDoc::setKeyNodeParent(DWORD key, DWORD parent)
{
	nodeFind.setKey(key);
	niterator it = nodes_.findNodeW(nodeFind);
	if (it != nodes_.end()) {
		(*it).setParent(parent);
	}
	SetModifiedFlag();
	UpdateAllViews(NULL);
}

void iEditDoc::deleteKeyItem(DWORD key)
{
	DWORD parent = nodes_.getCurParent();
	// 以下の行は、メタファイルの再描画不具合のため廃止iNode_eqの中でiNodeの参照を使うのが原因？
//	nodes_.erase(remove_if(nodes_.begin(), nodes_.end(), iNode_eq(key)), nodes_.end());
	
	nodeFind.setKey(key);
	niterator it = nodes_.findNodeW(nodeFind);
	if (it != nodes_.end()) {
		nodes_.erase(it);
	}
	links_.erase(remove_if(links_.begin(), links_.end(), iLink_eq(key)), links_.end());
	if (m_bShowBranch) {
		nodes_.setVisibleNodes(m_visibleKeys);
	} else {
		nodes_.setVisibleNodes(this->nodes_.getSelKey());
	}
	SetModifiedFlag();
	iHint h; h.event = iHint::nodeDeleteByKey;
	UpdateAllViews(NULL, key, &h);
}

BOOL iEditDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath_s(lpszPathName, drive, dir, fname, ext );
	CString extent = ext;
	extent.MakeLower();
	if (extent != ".iedx" && extent != ".ied" && extent != ".xml") {
		AfxMessageBox("iEditファイルではありません");
		return FALSE;
	}
	m_bSerializeXML = false;
	if (extent == ".iedx") {
		m_bSerializeXML = false;
		m_bOldBinary = false;
	} else if (extent == ".ied") {
		m_bSerializeXML = false;
		m_bOldBinary = true;
	} else if (extent == ".xml") {
		m_bSerializeXML = true;
		m_bOldBinary = false;
	}
	
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	// TODO: この位置に固有の作成用コードを追加してください
	InitDocument();
	return TRUE;
}

BOOL iEditDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath_s(lpszPathName, drive, dir, fname, ext );
	CString extent = ext;
	if (extent != ".iedx" && extent != ".ied" &&extent != ".xml") {
		extent = ".iedx";
	}
	m_bSerializeXML = false;
	
	if (extent == ".iedx") {
		m_bSerializeXML = false;
		m_bOldBinary = false;
	} else if (extent == ".ied") {
		m_bSerializeXML = false;
		m_bOldBinary = true;
	} else if (extent == ".xml") {
		m_bSerializeXML = true;
		m_bOldBinary = false;
	}
	return CDocument::OnSaveDocument(lpszPathName);
}


void iEditDoc::InitDocument()
{
	if (nodes_.size() == 0) {
		lastKey = 0;
		iNode i("主題"); i.setKey(0); i.setParent(0);
		i.moveBound(CSize(10, 10));
		nodes_.insert(i);
		sv.push_back(i.getKey());
	}
	
	nodes_.initSelection();
	curParent = nodes_.getCurParent();
	nodes_.setVisibleNodes(nodes_.getSelKey());
	calcMaxPt(m_maxPt);
	canCpyLink = FALSE;
}

CString iEditDoc::getSelectedNodeText()
{
	niterator it = nodes_.getSelectedNode();
	if (it != nodes_.end()) {
		return (*it).getText();
	}
	return "";
}

void iEditDoc::selChanged(DWORD key, bool reflesh, bool bShowSubBranch)
{
	DWORD parentOld = nodes_.getCurParent();
	nodes_.setSelKey(key);
	
	serialVec svec = getOutlineView()->getDrawOrder(bShowSubBranch);
	if (((CiEditApp*)AfxGetApp())->m_rgsNode.orderDirection == 1) {
		// 降順での描画オプションの場合は反転する
		std::reverse(svec.begin(), svec.end());
	}
	nodes_.setDrawOrder(svec);
	
	if (!bShowSubBranch) {
		nodes_.setVisibleNodes(key);
	} else {
		nodes_.setVisibleNodes(m_visibleKeys);
	}
	
	DWORD parentNew = nodes_.getCurParent();
	
	if (parentOld != parentNew || parentNew == 0) {
		calcMaxPt(m_maxPt);
	}
	
	if (!reflesh) {
		iHint h;
		h.event = iHint::linkSelRet;
		UpdateAllViews(NULL, (LPARAM)key, &h);
		return;
	}
	if (parentOld != parentNew && !bShowSubBranch) {
		iHint h;
		h.event = iHint::parentSel;
		UpdateAllViews(NULL, (LPARAM)key, &h);
	} else {
		iHint h; h.event = iHint::nodeSel;
		UpdateAllViews(NULL, (LPARAM)key, &h);
	}
}

void iEditDoc::setCurNodeText(CString &s)
{
	niterator it = nodes_.getSelectedNode();
	if (it != nodes_.end()) {
		(*it).setText(s);
	}
	// Undo 処理のためのメッセージを飛ばす方が良い。
}

void iEditDoc::drawNodes(CDC *pDC, bool bDrwAll)
{
	nodes_.drawNodes(pDC, bDrwAll);
}

bool iEditDoc::hitTest(const CPoint& pt, CRect &r, bool bDrwAll)
{
	iNode* pNode = nodes_.hitTest(pt, bDrwAll);
	if (pNode != NULL) {
		r = pNode->getBound();
		iHint hint;
		hint.event = iHint::nodeSel;
		UpdateAllViews(NULL, (LPARAM)pNode->getKey(), &hint);
		
		return true;
	}
	return false;
}

void iEditDoc::moveSelectedNode(const CSize &sz)
{
	nodes_.moveSelectedNode(sz);
	setConnectPoint();
	calcMaxPt(m_maxPt);
	SetModifiedFlag();
	// Undo 処理のために別のメッセージを送った方が良い
//	iHint hint; hint.event = iHint::nodeStyleChanged;
//	DWORD key = nodes_.getSelKey();
//	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::moveNodesInBound(const CRect& bound,	const CSize move)
{
	niterator itSelected = nodes_.getSelectedNode();
	if (itSelected == nodes_.end()) return;
	niterator it = nodes_.begin();
	bool moved = false;
	KeySet keySet;
	for ( ; it != nodes_.end(); it++) {
		if (!(*it).isVisible()) continue;
		BOOL bInBound = bound.PtInRect((*it).getBound().TopLeft()) &&
			            bound.PtInRect((*it).getBound().BottomRight());
		if (bInBound) {
			if ((*it).getDrawOrder() > (*itSelected).getDrawOrder()) {
				(*it).moveBound(move);
				keySet.insert((*it).getKey());
				moved = true;
			}
		}
	}
	
	if (moved) {
		literator li = links_.begin();
		for ( ; li != links_.end(); li++) {
			if (keySet.find((*li).getKeyFrom()) != keySet.end() &&
				keySet.find((*li).getKeyTo()) != keySet.end()) {
				(*li).movePts(move);
			}
		}
		setConnectPoint();
		
		iHint hint; hint.event = iHint::groupMoved;
		UpdateAllViews(NULL, nodes_.getSelKey(), &hint);
	}
}

void iEditDoc::moveSelectedLink(const CSize &sz)
{
	literator li = links_.begin();
	for ( ; li != links_.end(); li++) {
		nodeFind.setKey((*li).getKeyFrom());
		niterator itFrom = nodes_.findNodeW(nodeFind);
		nodeFind.setKey((*li).getKeyTo());
		niterator itTo   = nodes_.findNodeW(nodeFind);
		if (itFrom == nodes_.end() || itTo == nodes_.end()) continue;
		if ((*itFrom).isSelected() && (*itTo).isSelected()) {
			(*li).movePts(sz);
			SetModifiedFlag();
			calcMaxPt(m_maxPt);
		}
	}
}


void iEditDoc::setSelectedNodeBound(const CRect &r)
{
	backUpUndoNodes();
	nodes_.setSelectedNodeBound(r);
	setConnectPoint();
	calcMaxPt(m_maxPt);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.getSelKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::setConnectPoint()
{
	literator li = links_.begin();
	for ( ; li != links_.end(); li++) {
		nodeFind.setKey((*li).getKeyFrom());
		niterator itFrom = nodes_.findNodeW(nodeFind);
		nodeFind.setKey((*li).getKeyTo());
		niterator itTo   = nodes_.findNodeW(nodeFind);
		if (itFrom == nodes_.end() || itTo == nodes_.end()) continue;
		if ((*itFrom).isSelected() && (*itTo).isSelected()) {
			if (itFrom != itTo) continue;
		}
		
		if ((*li).getKeyFrom() == (*itFrom).getKey()) {
			(*li).setRFrom((*itFrom).getBound());
		}
		if ((*li).getKeyTo() == (*itTo).getKey()) {
			(*li).setRTo((*itTo).getBound());
		}
	}
}

const CPoint& iEditDoc::getMaxPt() const
{
	return m_maxPt;
}

// TODO:このメソッドはパブリックのメソッドが呼ばれたときに暗黙のうちに呼ばれてると
// 思われがちだが、実際は適切なタイミングで呼び出されないといけない。呼び出し忘れ
// があると再描画領域の計算がおかしくなる。
void iEditDoc::calcMaxPt(CPoint &pt)
{
	KeySet ks;
	pt = CPoint(0, 0);
	const_niterator it = nodes_.begin();
	for ( ; it != nodes_.end(); it++) {
		if (!(*it).isVisible()/* && !m_bShowAll*/) {
			continue;
		} // この位置でフィルタをかけるべきだよね。これが、選択領域のバグの原因？
		ks.insert((*it).getKey());
		
		if ((*it).getBound().BottomRight().x > pt.x) {
			pt.x = (*it).getBound().BottomRight().x;
		}
		if ((*it).getBound().BottomRight().y > pt.y) {
			pt.y = (*it).getBound().BottomRight().y;
		}
	}
	literator li = links_.begin();
	for ( ; li != links_.end(); li++) {
		if (ks.find((*li).getKeyFrom()) != ks.end() && ks.find((*li).getKeyTo()) != ks.end() && (*li).getArrowStyle() != iLink::other) {
			(*li).setDrawFlag();
			if ((*li).getBound().BottomRight().x > pt.x) {
				pt.x = (*li).getBound().BottomRight().x;
			}
			if ((*li).getBound().BottomRight().y > pt.y) {
				pt.y = (*li).getBound().BottomRight().y;
			}
		} else {
			(*li).setDrawFlag(false);
		}
	}
}

void iEditDoc::drawLinks(CDC *pDC, bool bDrwAll, bool clipbrd)
{
	links_.drawLines(pDC, bDrwAll);
	links_.drawArrows(pDC, bDrwAll);
	links_.drawComments(pDC, bDrwAll, clipbrd);
}

bool iEditDoc::setStartLink(const CPoint& pt)
{
	iNode* pNode = nodes_.hitTest(pt);
	if (pNode != NULL) {
		rcLinkFrom = pNode->getBound();
		keyLinkFrom = pNode->getKey();
		keyParentLinkFrom = pNode->getParent();
		return true;
	}
	return false;
}

bool iEditDoc::setEndLink(const CPoint &pt, int ArrowType, bool bDrwAll, bool bArrowSpecification)
{
	iNode* pNode = nodes_.hitTest2(pt, bDrwAll);
	if (pNode != NULL) {
		rcLinkTo = pNode->getBound();
		keyLinkTo = pNode->getKey();
		iLink l;
		l.setNodes(rcLinkFrom, rcLinkTo, keyLinkFrom, keyLinkTo);
		l.setDrawFlag();
		l.setKey(lastLinkKey++);
		desideLinkLineStyle(l);
		if (bArrowSpecification) {
			l.setArrowStyle(ArrowType);
		} else {
			desideLinkArrow(l);
		}
		links_.push_back(l);
		SetModifiedFlag();
		iHint h; h.event = iHint::linkAdd;
		UpdateAllViews(NULL, (LPARAM)keyLinkFrom, &h);
		return true;
	}
	return false;
}

bool iEditDoc::setAlterLinkFrom(const CPoint &pt, bool bDrwAll)
{
	iNode* pNode = nodes_.hitTest(pt, bDrwAll); // リンク元を再選択
	if (pNode != NULL) {
		backUpUndoLinks();
		links_.setSelectedNodeLinkFrom(pNode->getKey(), pNode->getBound(), bDrwAll);
		SetModifiedFlag();
		iHint h; h.event = iHint::linkModified;
		UpdateAllViews(NULL, (LPARAM)getSelectedNodeKey(), &h);
		return true;
	}
	return false;
}

bool iEditDoc::setAlterLinkTo(const CPoint &pt, bool bDrwAll)
{
	iNode* pNode = nodes_.hitTest2(pt, bDrwAll); // 再選択なし
	if (pNode != NULL) {
		backUpUndoLinks();
		links_.setSelectedNodeLinkTo(pNode->getKey(), pNode->getBound(), bDrwAll);
		SetModifiedFlag();
		UpdateAllViews(NULL);
		iHint h; h.event = iHint::linkModified;
		UpdateAllViews(NULL, (LPARAM)getSelectedNodeKey(), &h);
		return true;
	}
	return false;
}

void iEditDoc::setSelectedNodeFont(const LOGFONT &lf)
{
	nodes_.setSelectedNodeFont(lf);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.getSelKey();
	setConnectPoint();
	calcMaxPt(m_maxPt);
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::getSelectedNodeFont(LOGFONT& lf)
{
	nodes_.getSelectedNodeFont(lf);
}

COLORREF iEditDoc::getSelectedNodeFontColor() const
{
	return nodes_.getSelectedNodeFontColor();
}

void iEditDoc::setSelectedNodeFontColor(const COLORREF &c)
{
	nodes_.setSelectedNodeFontColor(c);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.getSelKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::setSelectedNodeLineColor(const COLORREF &c)
{
	nodes_.setSelectedNodeLineColor(c);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.getSelKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::setSelectedNodeBrush(const COLORREF &c)
{
	nodes_.setSelectedNodeBrush(c);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.getSelKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::setSelectedNodeNoBrush(BOOL noBrush)
{
	nodes_.setSelectedNodeNoBrush(noBrush);
	setConnectPoint3();
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.getSelKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

COLORREF iEditDoc::getSelectedNodeLineColor() const
{
	return nodes_.getSelectedNodeLineColor();
}

COLORREF iEditDoc::getSelectedNodeBrsColor() const
{
	return nodes_.getSelectedNodeBrsColor();
}

void iEditDoc::setSelectedNodeLineStyle(int style)
{
	nodes_.setSelectedNodeLineStyle(style);
	SetModifiedFlag();
	setConnectPoint3();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.getSelKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

int iEditDoc::getSelectedNodeLineStyle() const
{
	return nodes_.getSelectedNodeLineStyle();
}

void iEditDoc::setSelectedNodeLineWidth(int w)
{
	nodes_.setSelectedNodeLineWidth(w);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.getSelKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

int iEditDoc::getSelectedNodeLineWidth() const
{
	return nodes_.getSelectedNodeLineWidth();
}


bool iEditDoc::isSelectedNodeMultiLine() const
{
	int style = nodes_.getSelectedNodeTextStyle();
	if (style == iNode::m_c || style == iNode::m_l || style == iNode::m_r) {
		return true;
	}
	return false;
}

void iEditDoc::setSelectedNodeMultiLine(bool set)
{
	int style;
	if (set) {
		style = iNode::m_c;
	} else {
		style = iNode::s_cc;
	}
	backUpUndoNodes();
	backUpUndoLinks();
	nodes_.setSelectedNodeTextStyle(style);
	setConnectPoint3();
	calcMaxPt(m_maxPt);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.getSelKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

int iEditDoc::selectNodesInBound(const CRect &r, CRect& selRect, bool drwAll)
{
	int cnt = nodes_.selectNodesInBound(r, selRect, drwAll);
	if (cnt == 1) {
		CRect rc;
		hitTest(selRect.CenterPoint(), rc, drwAll);
	}
	return cnt;
}

CRect iEditDoc::getSelectedNodeRect() const
{
	CRect rc(CRect(0, 0, 0, 0));
	const_niterator it = nodes_.getSelectedNodeR();
	if (it != nodes_.end()) {
		rc = (*it).getBound();
	}
	return rc;
}

////////////////////////////////////////////////////////////////////////////
// treeview で追加されたノードに対し、適当な座標値を与えるための専用関数
////////////////////////////////////////////////////////////////////////////
CRect iEditDoc::getRecentNodeRect()
{
	CRect rc(CRect(0, 0, 50, 30));
	DWORD curkey = nodes_.getSelKey();
	const_niterator it = nodes_.begin();
	DWORD pastkey = 0;
	for ( ; it != nodes_.end(); it++) {
		if (!(*it).isVisible()) continue;
		if ((*it).getKey() > pastkey && (*it).getKey() < curkey) {
			pastkey = (*it).getKey();
		}
	}
	nodeFind.setKey(pastkey);
	it = nodes_.findNode(nodeFind);
	if (it != nodes_.end()) {
		rc = (*it).getBound();
	}
	return rc;
}

// クリップボートからのノード一括作成のためにこのメソッドだけシグネチャを変えました。
void iEditDoc::addNodeRect(const CString &name, const CPoint &pt, bool bSetMultiLineProcess, bool bNoBound)
{
	addNodeInternal(name, pt, iNode::rectangle, bSetMultiLineProcess, bNoBound);
}

void iEditDoc::addNodeArc(const CString &name, const CPoint &pt)
{
	addNodeInternal(name, pt, iNode::arc, true);
}

void iEditDoc::addNodeRoundRect(const CString &name, const CPoint &pt)
{
	addNodeInternal(name, pt, iNode::roundRect, true);
}

void iEditDoc::addNodeInternal(const CString &name, const CPoint &pt, int nodeType, bool bEnableMultiLineProcess, bool bNoBound)
{
	iNode n(name);
	n.setKey(getUniqKey());
	n.setParent(nodes_.getCurParent());
	n.setNodeShape(nodeType);
	n.moveBound(CSize(pt.x, pt.y));
	n.setVisible();
	
	if (!bEnableMultiLineProcess) {
		n.setTextStyle(iNode::s_cc);
	}
	if (bNoBound) {
		n.setNoBrush(FALSE);
		n.setLineStyle(PS_NULL);
	}
	nodes_.insert(n);
	
	if (isShowSubBranch()) {
		m_visibleKeys.insert(n.getKey());
	}
	calcMaxPt(m_maxPt);
	selChanged(n.getKey(), true, isShowSubBranch());
	SetModifiedFlag();
	iHint hint;
	
	// OutlineViewでのイベント処理用に飛ばしてる arcとrectを分ける意味はないようだ。
	hint.event = iHint::rectAdd;
	if (nodeType == iNode::arc) {
		hint.event = iHint::arcAdd;
	}
	
	hint.str = name;
	UpdateAllViews(NULL, (LPARAM)n.getKey(), &hint);
}

void iEditDoc::addNodeMF(const CString &name, const CPoint &pt, int mfIndex, HENHMETAFILE& mh)
{
	iNode n(name);
	n.setKey(getUniqKey());
	n.setParent(nodes_.getCurParent());
	n.setNodeShape(iNode::MetaFile);
	n.moveBound(CSize(pt.x, pt.y));
	n.setMetaFile(mh);
	n.setTextStyle(iNode::notext);
	nodes_.insert(n);
	
	if (isShowSubBranch()) {
		m_visibleKeys.insert(n.getKey());
	}
	
	selChanged(n.getKey(), true, isShowSubBranch());
	SetModifiedFlag();
	iHint hint;
	hint.event = iHint::rectAdd;
	hint.str = name;
	UpdateAllViews(NULL, (LPARAM)n.getKey(), &hint);
}

bool iEditDoc::hitTestLinks(const CPoint &pt, bool drwAll)
{
	DWORD key; CString path;
	bool hit = links_.hitTest(pt, key, path, drwAll);
	if (hit) {
		iHint h; h.event = iHint::linkSel; h.str = path;
		UpdateAllViews(NULL, (LPARAM)key, &h);
	}
	return hit;
}

bool iEditDoc::hitTestLinksFrom(const CPoint &pt, bool drwAll)
{
	DWORD key; CString path;
	bool hit = links_.hitTestFrom(pt, key, path, drwAll);
	if (hit) {
		iHint h; h.event = iHint::linkSel; h.str = path;
		UpdateAllViews(NULL, (LPARAM)key, &h);
	}
	return hit;
}

bool iEditDoc::hitTestLinksTo(const CPoint &pt, bool drwAll)
{
	DWORD key; CString path;
	bool hit = links_.hitTestTo(pt, key, path, drwAll);
	if (hit) {
		iHint h; h.event = iHint::linkSel; h.str = path;
		UpdateAllViews(NULL, (LPARAM)key, &h);
	}
	return hit;
}

void iEditDoc::drawLinkSelection(CDC *pDC, bool bDrwAll)
{
	links_.drawSelection(pDC, bDrwAll);
}

void iEditDoc::drawLinkSelectionFrom(CDC *pDC, bool bDrwAll)
{
	links_.drawSelectionFrom(pDC, bDrwAll);
}

void iEditDoc::drawLinkSelectionTo(CDC *pDC, bool bDrwAll)
{
	links_.drawSelectionTo(pDC, bDrwAll);
}

void iEditDoc::setNewLinkInfo(DWORD keyFrom, DWORD keyTo, const CString &comment, int styleArrow)
{
	nodeFind.setKey(keyFrom);
	niterator itFrom = nodes_.findNodeW(nodeFind);
	nodeFind.setKey(keyTo);
	niterator itTo = nodes_.findNodeW(nodeFind);
	if (itFrom == nodes_.end() || itTo == nodes_.end()) return;
	iLink l;
	l.setArrowStyle(styleArrow);
	l.setName(comment);
	l.setNodes((*itFrom).getBound(), (*itTo).getBound(), keyFrom, keyTo);
	if ((*itFrom).isVisible() && (*itTo).isVisible()) {
		l.setDrawFlag();
	}
	l.setKey(lastLinkKey++);
	links_.push_back(l);
	SetModifiedFlag();
	iHint h; h.event = iHint::linkAdd;
	UpdateAllViews(NULL, (LPARAM)keyFrom, &h);
}

void iEditDoc::getSelectedLinkInfo(CString &sFrom, CString &sTo, CString &sComment, int &arrowType, bool bDrwAll)
{
	const_literator li = links_.getSelectedLink(bDrwAll);
	if (li != links_.end()) {
		sComment = (*li).getName();
		arrowType = (*li).getArrowStyle();
		nodeFind.setKey((*li).getKeyFrom());
		const_niterator itFrom = nodes_.findNode(nodeFind);
		nodeFind.setKey((*li).getKeyTo());
		const_niterator itTo = nodes_.findNode(nodeFind);
		if (itFrom != nodes_.end()) sFrom = (*itFrom).getName();
		if (itTo != nodes_.end()) sTo = (*itTo).getName();
	}
}

void iEditDoc::setSelectedLinkInfo(const CString &sComment, int arrowType, bool bDrwAll)
{
	literator li = links_.getSelectedLinkW(bDrwAll);
	if (li != links_.end()) {
		backUpUndoLinks();
		(*li).setName(sComment);
		(*li).setArrowStyle(arrowType);
		SetModifiedFlag();
		iHint h; h.event = iHint::linkModified;
		UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
	}
}

void iEditDoc::selectLinksInBound(const CRect &r, bool drwAll)
{
	// HINT: ノードが選択されてるかの判断が必要なので、iEditDocで実装すべき
	// 今のところcanDrawがtrueだと選択する
	links_.selectLinksInBound(r, drwAll);
}

int iEditDoc::getSelectedLinkWidth(bool drwAll) const
{
	// links や nodesはdocのprivateメンバなのだから要素に関する演算は
	// docでやってもかまわないとこの関数を書いていて気づいた。
	// 方針変更しようと思ったが、inlineで同じメソッド名を使用している
	// とリンカがうまくいかないのでやはり...
	return links_.getSelectedLinkWidth(drwAll);
}

void iEditDoc::setSelectedLinkWidth(int w, bool drwAll)
{
	links_.setSelectedLinkWidth(w, drwAll);
	SetModifiedFlag();
	iHint h; h.event = iHint::linkModified;
	UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
}

void iEditDoc::setSelectedLinkLineStyle(int style, bool drwAll)
{
	links_.setSelectedLinkLineStyle(style, drwAll);
	SetModifiedFlag();
	iHint h; h.event = iHint::linkModified;
	UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
}

int iEditDoc::getSelectedLinkLineStyle(bool drwAll)
{
	return links_.getSelectedLinkLineStyle(drwAll);
}

COLORREF iEditDoc::getSelectedLinkLineColor(bool drwAll) const
{
	return links_.getSelectedLinkLineColor(drwAll);
}

void iEditDoc::setSelectedLinkLineColor(const COLORREF &c, bool drwAll)
{
	links_.setSelectedLinkLineColor(c, drwAll);
	SetModifiedFlag();
	iHint h; h.event = iHint::linkModified;
	UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
}

void iEditDoc::setSelectedLinkFont(const LOGFONT &lf, bool drwAll)
{
	links_.setSelectedLinkFont(lf, drwAll);
	SetModifiedFlag();
	iHint h; h.event = iHint::linkModified;
	UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
}

void iEditDoc::getSelectedLinkFont(LOGFONT &lf, bool drwAll)
{
	links_.getSelectedLinkFont(lf, drwAll);
}

BOOL iEditDoc::RouteCmdToAllViews(CView *pView, UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo)
{
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL) {
		CView* pNextView = GetNextView(pos);
		if(((CCmdTarget*)pNextView)->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) {
			return TRUE;
		}
	}
	return FALSE;
}

void iEditDoc::deleteSelectedLink(bool drwAll)
{
	literator it = links_.getSelectedLinkW(drwAll);
	if (it != links_.end()) {
		m_deleteBound = (*it).getBound();
		links_.erase(it);
		SetModifiedFlag();
		iHint h; h.event = iHint::linkDelete;
		UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
	}
}

void iEditDoc::deleteSelectedNode()
{
	DWORD delKey = nodes_.getSelKey(); 
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeDelete;
	UpdateAllViews(NULL, (LPARAM)delKey, &hint);
}

void iEditDoc::deleteSelectedNodes()
{
	SetModifiedFlag();
	DWORD parentKey = nodes_.getCurParent();
	if (isShowSubBranch()) {
		parentKey = m_dwBranchRootKey;
	}
	serialVec v = nodes_.getSelectedNodeKeys();	
	vector<DWORD>::iterator it = v.begin();
	for ( ; it != v.end(); it++) {
		DWORD delKey = (*it);
		iHint hint; hint.event = iHint::nodeDeleteMulti;
		hint.keyParent = parentKey;
		UpdateAllViews(NULL, (LPARAM)delKey, &hint);
	}
}

CString iEditDoc::getSelectedNodeLabel()
{
	niterator it = nodes_.getSelectedNode();
	if (it != nodes_.end()) {
		return (*it).getName();
	}
	return "";
}

CString iEditDoc::getSelectedLinkLabel(bool drawAll)
{
	CString label("");
	const_literator it = links_.getSelectedLink(drawAll);
	if (it != links_.end()) {
		label = (*it).getName();
	}
	return label;
}


bool iEditDoc::canDeleteNode() const
{
	return nodes_.getSelKey() != 0;
}


void iEditDoc::getLinkInfoList(lsItems &ls, bool drwAll)
{
	DWORD curKey = nodes_.getSelKey();
	literator it = links_.begin();
	for (; it != links_.end(); it++) {
		if ((*it).getKeyFrom() != curKey && (*it).getKeyTo() != curKey) continue;
		listitem i;
		i.comment = (*it).getName();
		if ((*it).getKeyFrom() == curKey) {
			i.keyTo = (*it).getKeyTo();
		} else if ((*it).getKeyTo() == curKey) {
			i.keyTo = (*it).getKeyFrom();
		}
		
		i.path = (*it).getPath();
		
		if ((*it).getKeyFrom() == curKey) {
			nodeFind.setKey((*it).getKeyTo());
		} else if ((*it).getKeyTo() == curKey) {
			nodeFind.setKey((*it).getKeyFrom());
		}
		
		const_niterator ni = nodes_.findNode(nodeFind);
		i.sTo = (*ni).getName();
		
		if ((*it).getArrowStyle() != iLink::other) {
			if ((*it).canDraw()) {
				if ((*it).getKeyFrom() == curKey) {
					i.linkType = listitem::linkSL;
				} else if ((*it).getKeyTo() == curKey) {
					i.linkType = listitem::linkSL2;
				}
			} else {
				if ((*it).getKeyFrom() == curKey) {
					i.linkType = listitem::linkDL;
				} else if ((*it).getKeyTo() == curKey) {
					i.linkType = listitem::linkDL2;
				}
			}
		} else {
			CString url = (*it).getPath();
			if (url.Find("http://") != -1 || url.Find("https://") != -1 || url.Find("ftp://") != -1) {
				i.linkType = listitem::WebURL;
			} else {
				CFileFind finder;
				finder.FindFile(url);
				finder.FindNextFile();
				if (finder.IsDirectory()) {
					i.linkType = listitem::linkFolder;
				} else {
					if (url.Right(5) == ".iedx" || url.Right(4) == ".ied") {
						i.linkType = listitem::iedFile;
					} else {
						i.linkType = listitem::FileName;
					}
				}
			}
		}
		i.key = (*it).getKey();
		i._arrowStyle = (*it).getArrowStyle();
		i._keyFrom = (*it).getKeyFrom();
		i.selected = (*it).isSelected();
		i.linkColor_ = (*it).getLinkColor();
		i.linkWidth_ = (*it).getLineWidth();
		i.styleLine_ = (*it).getLineStyle();
		i.lf_ = (*it).getFontInfo();
		::lstrcpy(i.lf_.lfFaceName, (*it).getFontInfo().lfFaceName);
		ls.push_back(i);
	}
}

void iEditDoc::notifySelectLink(const lsItems &ls, int index, bool drwAll)
{
	DWORD curKey = nodes_.getSelKey();
	literator it = links_.begin();
	
	bool selected = false;
	for (; it != links_.end(); it++) {
		if ((*it).getKeyFrom() == ls[index]._keyFrom &&
			(*it).getKeyTo() == ls[index].keyTo &&
			(*it).getName() == ls[index].comment &&
			(*it).getPath() == ls[index].path &&
			(*it).getArrowStyle() == ls[index]._arrowStyle) {
			(*it).selectLink();
			selected = true;
		} else {
			(*it).selectLink(false);
		}
	}
	
	if (selected) {
		if (ls[index].linkType == listitem::linkSL) {
			iHint h; h.event = iHint::linkListSel;
			UpdateAllViews(NULL, LPARAM(curKey), &h);
		}
	}
}

void iEditDoc::addURLLink(const CString &url, const CString& comment)
{
	DWORD curKey = nodes_.getSelKey();
	iLink l;
	l.setDrawFlag(false);
	CRect r(CRect(0, 0, 0, 0));
	l.setNodes(r, r, curKey, curKey);
	l.setName(comment);
	l.setPath(url);
	l.setArrowStyle(iLink::other);
	l.setKey(lastLinkKey++);
	links_.push_back(l);
	SetModifiedFlag();
	iHint h; h.event = iHint::linkAdd;
	UpdateAllViews(NULL, LPARAM(curKey), &h);
}

DWORD iEditDoc::getSelectedNodeKey() const
{
	return nodes_.getSelKey();
}

void iEditDoc::deleteSpecifidLink(const listitem &i)
{
	literator it = links_.begin();
	for (; it != links_.end(); it++) {
		if ((*it).getKeyFrom() == i._keyFrom &&
			(*it).getKeyTo() == i.keyTo &&
			(*it).getName() == i.comment &&
			(*it).getPath() == i.path &&
			(*it).getArrowStyle() == i._arrowStyle) {
			break;
		}
	}
	if (it != links_.end()) {
		m_deleteBound = (*it).getBound();
		links_.erase(it);
		SetModifiedFlag();
		iHint h; h.event = iHint::linkDelete;
		UpdateAllViews(NULL, (LPARAM)i._keyFrom, &h);
	}
}

void iEditDoc::setSpecifiedLinkInfo(const listitem &iOld, const listitem &iNew)
{
	literator it = links_.begin();
	for (; it != links_.end(); it++) {
		if ((*it).getKeyFrom() == iOld._keyFrom &&
			(*it).getKeyTo() == iOld.keyTo &&
			(*it).getName() == iOld.comment &&
			(*it).getPath() == iOld.path &&
			(*it).getArrowStyle() == iOld._arrowStyle &&
			(*it).getLinkColor() == iOld.linkColor_ &&
			(*it).getLineWidth() == iOld.linkWidth_ &&
			(*it).getLineStyle() == iOld.styleLine_) {
			break;
		}
	}
	if (it != links_.end()) {
		(*it).setName(iNew.comment);
		(*it).setArrowStyle(iNew._arrowStyle);
		(*it).setPath(iNew.path);
		(*it).setLinkColor(iNew.linkColor_);
		(*it).setLineWidth(iNew.linkWidth_);
		(*it).setLineStyle(iNew.styleLine_);
		(*it).setFontInfo(iNew.lf_);
		(*it).selectLink();
		SetModifiedFlag();
		iHint h; h.event = iHint::linkModified;
		UpdateAllViews(NULL, iNew._keyFrom, &h);
	}
}

void iEditDoc::setNodeRelax(CRelaxThrd *r, bool bDrwAll)
{
	////////////////////////////////////////////////
	// 自動レイアウトアルゴリズム用のedgeデータ設定
	////////////////////////////////////////////////
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();

	r->bounds.clear();
	r->edges.clear();
	r->edges.resize(0);
	literator li = links_.begin();
	for ( ; li != links_.end(); li++) {
		if ((*li).getArrowStyle() != iLink::other) {
			if ((*li).getKeyFrom() == (*li).getKeyTo()) continue;
			if (!(*li).canDraw() && !bDrwAll) continue;
			iEdge e;
			e.from = (*li).getKeyFrom();
			e.to = (*li).getKeyTo();
			
			nodeFind.setKey((*li).getKeyFrom());
			niterator itFrom = nodes_.findNodeW(nodeFind);
			nodeFind.setKey((*li).getKeyTo());
			niterator itTo = nodes_.findNodeW(nodeFind);
			
			CRect rFrom = (*itFrom).getBound();
			CRect rTo = (*itTo).getBound();
			
			CSize sz;
			sz.cx = (rFrom.Width() + rTo.Width())/2;
			sz.cy = (rFrom.Height() + rTo.Height())/2;
			
			////////////////////////
			// アイコン間の距離設定
			////////////////////////
			double rate;
			if (pApp->m_rgsLink.bSetStrength) {
				rate = width2Len((*li).getLineWidth());
				if ((*li).getLineStyle() == PS_DOT) {
					rate *= 1.5;
				}
			} else {
				rate = width2Len(0);
			}
			rate *= (((double)pApp->m_rgsLink.strength)/10.0);
			e.len = sqrt((double)(sz.cx*sz.cx + sz.cy*sz.cy))*5/4*rate; 
			
			///////////////////
			// edges への登録
			///////////////////
			bool already = false; // 登録されているedgeとの重複をチェック
			for (unsigned int i = 0; i < r->edges.size(); i++) {
				if (r->edges[i].from == e.from && r->edges[i].to == e.to  ||
					r->edges[i].from == e.to && r->edges[i].to == e.from) {
					already = true;
					break;
				}
			}
			if (!already) {
				r->edges.push_back(e);
			}
			//////////////////
			// bounds への登録
			//////////////////
			iBound b;
			b.key = e.from;
			b.label = (*itFrom).getName();
			b.fixed = (*itFrom).isFixed();
			b.oldBound = (*itFrom).getBound();
			b.newBound = b.oldBound;
			r->bounds.insert(b);

			iBound b2;
			b2.key = e.to;
			b2.label = (*itTo).getName();
			b2.fixed = (*itTo).isFixed();
			b2.oldBound = (*itTo).getBound();
			b2.newBound = b2.oldBound;
			r->bounds.insert(b2);
		}
	}
}

double iEditDoc::width2Len(int width)
{
	double l;
	switch (width){
	case 0:
		l = 1.0;
		break;
	case 2:
		l = 0.8;
		break;
	case 3:
		l = 0.6;
		break;
	case 4:
		l = 0.5;
		break;
	case 5:
		l = 0.3;
		break;
	default:
		l = 1.0;
		break;
	}
	
	return l;
}

CRect iEditDoc::getSelectedLinkBound(bool drwAll) const
{
	CRect rc(CRect(0, 0, 0, 0));
	rc = links_.getSelectedLinkBound(drwAll);
	return rc;
}

CRect iEditDoc::getRelatedBound(bool drwAll) const
{
	// TODO:branchモードの時の処理
	CRect rc(CRect(0, 0, 0, 0));
	
	const_niterator it = nodes_.begin();
	for ( ; it != nodes_.end(); it++) {
		if ((*it).isVisible() && (*it).isSelected()) {
			rc = (*it).getBound();
			break;
		}
	}
	
	it = nodes_.begin();
	for ( ; it != nodes_.end(); it++) {
		if ((*it).isVisible() && (*it).isSelected()) {
			rc |= (*it).getBound();
			const_literator li = links_.begin();
			for ( ; li != links_.end(); li++) {
				if (!(*li).canDraw()/* && !drwAll*/) {
					continue;
				}
				if ((*it).getKey() == (*li).getKeyFrom() || (*it).getKey() == (*li).getKeyTo()) {
					rc |= (*li).getBound();
				}
			}
		}
	}
	
	return rc;
}

CRect iEditDoc::getRelatedBoundAnd(bool drwAll)
{
	CRect rc(CRect(0, 0, 0, 0));
	
	const_niterator it = nodes_.begin();
	for ( ; it != nodes_.end(); it++) {
		if ((*it).isVisible() && (*it).isSelected()) {
			rc = (*it).getBound();
			break;
		}
	}
	
	it = nodes_.begin();
	for ( ; it != nodes_.end(); it++) {
		if (/*!drwAll && */ (*it).isVisible() && (*it).isSelected() /*|| drwAll && (*it).isSelected()*/) {
			rc |= (*it).getBound();
		}
	}
	
	const_literator li = links_.begin();
	for ( ; li != links_.end(); li++) {
		if (!(*li).canDraw()) {
			continue;
		}
		nodeFind.setKey((*li).getKeyFrom());
		niterator itFrom = nodes_.findNodeW(nodeFind);
		nodeFind.setKey((*li).getKeyTo());
		niterator itTo = nodes_.findNodeW(nodeFind);
		
		if ((*itFrom).isSelected() && (*itTo).isSelected()) {
			rc |= (*li).getBound();
		}
	}
	
	return rc;
}

void iEditDoc::setSelectedLinkCurve(CPoint pt, bool curve, bool bDrwAll)
{
	literator li = links_.getSelectedLinkW(bDrwAll);
	if (li != links_.end()) {
		if (!curve) {
			(*li).curve(false);
			SetModifiedFlag();
			iHint h; h.event = iHint::linkStraight;
			UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
			return;
		}
		
		if (!(*li).isCurved() && (*li).hitTest(pt)) {
			return;
		}
		(*li).setPathPt(pt);
		(*li).curve();
		if ((*li).hitTest2(pt) && (*li).getName() == "") {
			(*li).curve(false);
		}
		SetModifiedFlag();
		iHint h; h.event = iHint::linkCurved;
		calcMaxPt(m_maxPt);
		UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
	}
}

void iEditDoc::setSelectedLinkAngled(bool angled)
{
	backUpUndoLinks();
	literator li = links_.getSelectedLinkW(false);
	if (li == links_.end()) return;
	if (!(*li).isCurved()) return;
	(*li).angle(angled);
	SetModifiedFlag();
	iHint h; h.event = iHint::linkModified;
	calcMaxPt(m_maxPt);
	UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
}

void iEditDoc::getSelectedLinkPts(CPoint &start, CPoint &end, bool bDrwAll)
{
	const_literator li = links_.getSelectedLink(bDrwAll);
	if (li != links_.end()) {
		nodeFind.setKey((*li).getKeyFrom());
		const_niterator itstart = nodes_.findNode(nodeFind);
		nodeFind.setKey((*li).getKeyTo());
		const_niterator itend = nodes_.findNode(nodeFind);
		if (itstart == nodes_.end() || itend == nodes_.end()) {
			start = CPoint(0, 0); end = CPoint(0, 0);
			return;
		}
		start.x = ((*itstart).getBound().left + (*itstart).getBound().right)/2;
		start.y = ((*itstart).getBound().top + (*itstart).getBound().bottom)/2;
		end.x = ((*itend).getBound().left + (*itend).getBound().right)/2;
		end.y = ((*itend).getBound().top + (*itend).getBound().bottom)/2;
	}
}

void iEditDoc::selectChild()
{
	iHint h; h.event = iHint::selectChild;
	UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
}

BOOL iEditDoc::isSelectedLinkCurved(bool bDrwAll) const
{
	const_literator li = links_.getSelectedLink(bDrwAll);
	if (li != links_.end() && (*li).isCurved()) {
		return TRUE;
	}
	return FALSE;
}

BOOL iEditDoc::isSelectedLinkSelf() const
{
	const_literator li = links_.getSelectedLink(false);

	if (li == links_.end()) return FALSE;
	if ((*li).getArrowStyle() == iLink::other) return FALSE;
	if ((*li).getKeyFrom() == (*li).getKeyTo()) return TRUE;
	return FALSE;
}

void iEditDoc::setCpLinkOrg()
{
	const_literator li = links_.getSelectedLink2();
	if (li != links_.end()) {
		m_cpLinkOrg = (*li);
		canCpyLink = TRUE;
	}
}

void iEditDoc::addSetLinkOrg()
{
	DWORD curKey = nodes_.getSelKey();
	m_cpLinkOrg.setKeyFrom(curKey);
	if (m_cpLinkOrg.getArrowStyle() == iLink::other) {
		m_cpLinkOrg.setKeyTo(curKey);
	}
	
	if (m_cpLinkOrg.getKeyFrom() != m_cpLinkOrg.getKeyTo()) {
		m_cpLinkOrg.curve(false);
	}
	
	m_cpLinkOrg.setKey(lastLinkKey++);
	m_cpLinkOrg.selectLink();
	
	iNode nodeFind; nodeFind.setKey(m_cpLinkOrg.getKeyFrom());
	const_niterator itFrom = nodes_.findNode(nodeFind);
	nodeFind.setKey(m_cpLinkOrg.getKeyTo());
	const_niterator itTo = nodes_.findNode(nodeFind);
	
	if (itFrom != nodes_.end() && itTo != nodes_.end()) {
		m_cpLinkOrg.setNodes((*itFrom).getBound(), (*itTo).getBound(), (*itFrom).getKey(), (*itTo).getKey());
	}
	
	links_.push_back(m_cpLinkOrg);
	SetModifiedFlag();
	iHint h; h.event = iHint::linkAdd;
	UpdateAllViews(NULL, (LPARAM)curKey, &h);
}

BOOL iEditDoc::canCopyLink()
{
	return canCpyLink;
}

BOOL iEditDoc::isSelectedNodeFilled() const
{
	return nodes_.isSelectedNodeFilled();	
}

int iEditDoc::getDataSize() const
{
	return nodes_.size();
}

BOOL iEditDoc::linkExist(bool drwAll) const
{
	const_literator li = links_.begin();
	for ( ; li != links_.end(); li++) {
//		if (!drwAll) {
			if ((*li).canDraw()) {
				return TRUE;
			}
//		} else {
//			if ((*li).getKeyFrom() != (*li).getKeyTo()) {
//				return TRUE;
//			}
//		}
	}
	return FALSE;
}

void iEditDoc::setSelectedNodeCopyOrg()
{
	copyOrg.clear(); copyOrg.resize(0);
	niterator It = nodes_.getSelectedNode();
	ptSelectMin = (*It).getBound().TopLeft();
	const_niterator it = nodes_.begin();
	for ( ; it != nodes_.end(); it++) {
		if ((*it).isSelected()) {
			copyOrg.push_back((*it).getKey());
			if (ptSelectMin.x > (*it).getBound().TopLeft().x) {
				ptSelectMin.x = (*it).getBound().TopLeft().x;
			}
			if (ptSelectMin.y > (*it).getBound().TopLeft().y) {
				ptSelectMin.y = (*it).getBound().TopLeft().y;
			}
		}
	}
}

void iEditDoc::makeCopyNode(const CPoint& pt, bool useDefault)
{
	for (unsigned int i = 0; i < copyOrg.size(); i++) {
		nodeFind.setKey(copyOrg[i]);
		niterator it = nodes_.findNodeW(nodeFind);
		iNode n(*it);
		n.setKey(getUniqKey());
		n.setParent(nodes_.getCurParent());
		
		if (!useDefault) {
			CPoint ptNew = pt;
			int szx = (*it).getBound().TopLeft().x - ptSelectMin.x;
			int szy = (*it).getBound().TopLeft().y - ptSelectMin.y;
			ptNew.x += szx; ptNew.y += szy;
			n.moveTo(ptNew);
		} else {
			n.moveBound(CSize(20, 30));
		}
		
		nodes_.insert(n);
		
		if (isShowSubBranch()) {
			m_visibleKeys.insert(n.getKey());
		}
		
		selChanged(n.getKey(), true, isShowSubBranch());
		calcMaxPt(m_maxPt);
		SetModifiedFlag();
		iHint hint; hint.event = iHint::rectAdd;
		hint.str = n.getName();
		hint.treeIconId = n.getTreeIconId();
		UpdateAllViews(NULL, (LPARAM)n.getKey(), &hint);
	}
	copyOrg.clear();
	copyOrg.resize(0);
}

BOOL iEditDoc::canCopyNode()
{
	if (copyOrg.size() != 0) return TRUE;
	return FALSE;
}

int iEditDoc::getSelectedNodeShape() const
{
	return nodes_.getSelectedNodeShape();
}

void iEditDoc::setSelectedNodeShape(int shape, int mfIndex)
{
	disableUndo();
	backUpUndoNodes();
	backUpUndoLinks();
	nodes_.setSelectedNodeShape(shape);
	SetModifiedFlag();
	if (shape == iNode::MetaFile) {
		niterator it = nodes_.getSelectedNode();
		if (it != nodes_.end()) {
			CiEditApp* pApp = (CiEditApp*)AfxGetApp();
			(*it).setMetaFile(pApp->m_hMetaFiles[mfIndex]);
			setConnectPoint();
		}
	}
	iHint h; h.event = iHint::nodeStyleChanged;
	UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
}

void iEditDoc::setSelectedNodeMetaFile(HENHMETAFILE metafile)
{
	niterator it = nodes_.getSelectedNode();
	if (it != nodes_.end()) {
		disableUndo();
		backUpUndoNodes();
		backUpUndoLinks();
		(*it).setNodeShape(iNode::MetaFile);
		(*it).setMetaFile(metafile);
		setConnectPoint();
		SetModifiedFlag();
		iHint h; h.event = iHint::nodeStyleChanged;
		UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
	}
}

void iEditDoc::setSelectedNodeLabel(const CString &label)
{
	niterator it = nodes_.getSelectedNode();
	if (it != nodes_.end()) {
		(*it).setName(label);
		SetModifiedFlag();
		iHint hint; hint.event = iHint::nodeLabelChanged;
		hint.str = label;
		DWORD key = nodes_.getSelKey();
		UpdateAllViews(NULL, (LPARAM)key, &hint);
	}
}


int iEditDoc::getSelectedNodeTextStyle() const
{
	return nodes_.getSelectedNodeTextStyle();
}

void iEditDoc::setSelectedNodeTextStyle(int style)
{
	nodes_.setSelectedNodeTextStyle(style);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.getSelKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::setSelectedNodeTreeIconId(int id)
{
	nodes_.setSelectedNodeTreeIconId(id);
	SetModifiedFlag();
}

void iEditDoc::setSelectedNodeFixed(BOOL f)
{
	nodes_.setSelectedNodeFixed(f);
	SetModifiedFlag();
}

BOOL iEditDoc::isSelectedNodeFixed() const
{
	return nodes_.isSelectedNodeFixed();
}

void iEditDoc::setResultRelax(Bounds &bounds)
{
	Bounds::iterator it = bounds.begin();
	for ( ; it != bounds.end(); it++) {
		nodeFind.setKey((*it).key);
		niterator nit = nodes_.findNodeW(nodeFind);
		(*nit).setBound((*it).newBound);
	}
	calcMaxPt(m_maxPt);
	setConnectPoint();
	SetModifiedFlag();
}

// このloadメソッドはインポート用
bool iEditDoc::loadXML(const CString &filename, bool replace)
{
	MSXML2::IXMLDOMDocument		*pDoc        = NULL;
	MSXML2::IXMLDOMParseError	*pParsingErr = NULL;
	MSXML2::IXMLDOMElement		*element     = NULL;
	MSXML2::IXMLDOMNodeList		*childs      = NULL; 
	MSXML2::IXMLDOMNode			*node        = NULL;
	
	BSTR	bstr = NULL;
	HRESULT hr;
    int     rc = 0;
	
	hr = CoInitialize(NULL);
	if(!SUCCEEDED(hr))
		return false;

	hr = CoCreateInstance (MSXML2::CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, 
							MSXML2::IID_IXMLDOMDocument, (LPVOID *)&pDoc);
	if(!pDoc) {
		AfxMessageBox("この機能を利用するには Microsoft Internet Explorer 5が必要になります");
		return false;
	}
    pDoc->put_async(VARIANT_FALSE);
	bstr = filename.AllocSysString();
	hr = pDoc->load (bstr);
    SysFreeString(bstr);
	
	if (!hr) {
		long line, linePos;
		BSTR reason = NULL;
		
		pDoc->get_parseError(&pParsingErr);
		
		pParsingErr->get_line(&line);
		pParsingErr->get_linepos(&linePos);
		pParsingErr->get_reason(&reason);
		pParsingErr->get_errorCode(&hr);
		
		SysFreeString(reason);
		return false;
	} else {
		idcVec.clear(); idcVec.resize(0);
		nodesImport.clear(); nodesImport.resize(0);
		nodeImport.setKey(-1); linkImport.setKeyFrom(-1);
		
		CWaitCursor wc;
		pDoc->get_documentElement(&element);
		
		BSTR s = NULL;
		element->get_nodeTypeString(&s);
		
		if(!wcscmp(s,L"element")) {
			element->get_nodeName(&s);
			CString elems(s);
			if (elems != "iEditDoc") {
				AfxMessageBox("これはiEdit用のXMLファイルではありません");
				return false;
			}
		}
		nodeImport.setBound(CRect(-1, -1, 0, 0));
		nodeImport.setName("");
		nodeImport.setText("");
		nodeImport.setTreeState(TVIS_EXPANDED);
		linkImport.setName("");
		linkImport.setPath("");
		linkImport.setArrowStyle(iLink::line);
		linkImport.setLineWidth(0);
		linkImport.setLinkColor(RGB(0, 0, 0));
		
		CStdioFile f;
		CFileStatus status;
		CFileException e;
	
		if (!f.Open("import.log", CFile::typeText | CFile::modeCreate | CFile::modeWrite, &e)) {
			return false;
		}
		bool ret = DomTree2Nodes2(element, &f);
		
		if (ret) {
		//	nodesImport.push_back(nodeImport);
		//	linksImport.push_back(linkImport);
			
			addImportData(replace);
		}
		return ret;
	}
	return false;
}

// このLoadメソッドはシリアライズ用
bool iEditDoc::loadFromXML(const CString &filename)
{
	MSXML2::IXMLDOMDocument		*pDoc        = NULL;
	MSXML2::IXMLDOMParseError	*pParsingErr = NULL;
	MSXML2::IXMLDOMElement		*element     = NULL;
	MSXML2::IXMLDOMNodeList		*childs      = NULL; 
	MSXML2::IXMLDOMNode			*node        = NULL;
	
	BSTR	bstr = NULL;
	HRESULT hr;
    int     rc = 0;
	
	hr = CoInitialize(NULL);
	if(!SUCCEEDED(hr))
		return false;

	hr = CoCreateInstance (MSXML2::CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, 
							MSXML2::IID_IXMLDOMDocument, (LPVOID *)&pDoc);
	if(!pDoc) {
		AfxMessageBox("この機能を利用するには Microsoft Internet Explorer 5が必要になります");
		return false;
	}
    pDoc->put_async(VARIANT_FALSE);
	bstr = filename.AllocSysString();
	hr = pDoc->load (bstr);
    SysFreeString(bstr);
	
	if (!hr) {
		long line, linePos;
		BSTR reason = NULL;
		
		pDoc->get_parseError(&pParsingErr);
		
		pParsingErr->get_line(&line);
		pParsingErr->get_linepos(&linePos);
		pParsingErr->get_reason(&reason);
		pParsingErr->get_errorCode(&hr);
		
		SysFreeString(reason);
		return false;
	} else {
		idcVec.clear(); idcVec.resize(0);
		nodesImport.clear(); nodesImport.resize(0);
		nodeImport.setKey(-1); linkImport.setKeyFrom(-1);
		
		CWaitCursor wc;
		pDoc->get_documentElement(&element);
		
		BSTR s = NULL;
		element->get_nodeTypeString(&s);
		
		if(!wcscmp(s,L"element")) {
			element->get_nodeName(&s);
			CString elems(s);
			if (elems != "iEditDoc") {
				AfxMessageBox("これはiEdit用のXMLファイルではありません");
				return false;
			}
		}
		nodeImport.setBound(CRect(-1, -1, 0, 0));
		nodeImport.setName("");
		nodeImport.setText("");
		nodeImport.setTreeState(TVIS_EXPANDED);
		linkImport.setName("");
		linkImport.setPath("");
		linkImport.setArrowStyle(iLink::line);
		linkImport.setLineWidth(0);
		linkImport.setLinkColor(RGB(0, 0, 0));
		
		bool ret = DomTree2Nodes3(element);
		
		if (ret) {
			unsigned int i;
			for (i = 0; i < nodesImport.size(); i++) {
				nodes_.insert(nodesImport[i]);
				sv.push_back(nodesImport[i].getKey());
				if (nodesImport[i].getKey() > lastKey) {
					lastKey = nodesImport[i].getKey();
				}
			}
			lastLinkKey = 0;
			const_niterator it;
			for (i = 0; i < linksImport.size(); i++) {
				linksImport[i].setKey(lastLinkKey++);
				nodeFind.setKey(linksImport[i].getKeyFrom());
				it = nodes_.findNode(nodeFind);
				if (it != nodes_.end()) {
					linksImport[i].setRFrom((*it).getBound());
				}
				nodeFind.setKey(linksImport[i].getKeyTo());
				it = nodes_.findNode(nodeFind);
				if (it != nodes_.end()) {
					linksImport[i].setRTo((*it).getBound());
				}
				links_.push_back(linksImport[i]);
			}
		}
		return ret;
	}
	return false;
}


// インポート時
bool iEditDoc::DomTree2Nodes2(MSXML2::IXMLDOMElement *node, CStdioFile* f)
{
	MSXML2::IXMLDOMNodeList	*childs    = NULL;
	MSXML2::IXMLDOMNodeList	*childs2    = NULL;
	MSXML2::IXMLDOMNodeList	*childs3    = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	MSXML2::IXMLDOMNode		*childnode2 = NULL;
	node->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i, j;
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		if (ename == "inode") {
			iNode n("add"); n.setKey(getUniqKey());
			n.setNoBrush(FALSE);
			n.setBound(CRect(-1, -1, 0, 0));
			nodesImport.push_back(n);
			childnode->get_childNodes(&childs2);
			for (j = 0; j < childs2->Getlength(); j++) {
				childs2->get_item(j, &childnode2);
				BSTR name2 = childnode2->nodeName;
				CString ename2(name2);
				if (ename2 == "id") {
					childnode2->firstChild->get_text(&s);
					CString ids(s);
					int id;
					sscanf_s(ids, "%d", &id);
					idConv idc;
					idc.first = (DWORD)id;
					idc.second = nodesImport[nodesImport.size()-1].getKey();
					idcVec.push_back(idc);
					
					ids += ' '; f->WriteString(ids); // log
					
				} else if (ename2 == "pid") {
					childnode2->firstChild->get_text(&s);
					CString pids(s);
					int pid;
					sscanf_s(pids, "%d", &pid);
					nodesImport[nodesImport.size()-1].setParent((DWORD)pid);
				} else if (ename2 == "label") {
					childnode2->firstChild->get_text(&s);
					CString name(s);
					nodesImport[nodesImport.size()-1].setName(name);
					
					name += '\n'; f->WriteString(name); // log
					
				} else if (ename2 == "text") {
					childnode2->firstChild->get_text(&s);
					CString text(s);
					text = procLF(text);
					nodesImport[nodesImport.size()-1].setText(text);
				} else if (ename2 == "labelAlign") {
					childnode2->firstChild->get_text(&s);
					CString align(s);
					nodesImport[nodesImport.size()-1].setTextStyle(tag2Align(align));
				} else if (ename2 == "shape") {
					childnode2->firstChild->get_text(&s);
					CString shape(s);
					nodesImport[nodesImport.size()-1].setNodeShape(tag2Shape(shape));
				} else if (ename2 == "bound") {
					CRect rc = nodesImport[nodesImport.size()-1].getBound();
					tags2bound(childnode2, rc);
					rc.NormalizeRect();
					nodesImport[nodesImport.size()-1].setBound(rc);
				} else if (ename2 == "ForColor") {
					COLORREF cr = tags2foreColor(childnode2);
					nodesImport[nodesImport.size()-1].setBrush(cr);
				} else if (ename2 == "nodeLine") {
					int lineStyle(PS_SOLID), lineWidth(0);
					tags2nodeLine(childnode2, lineStyle, lineWidth);
					nodesImport[nodesImport.size()-1].setLineStyle(lineStyle);
					nodesImport[nodesImport.size()-1].setLineWidth(lineWidth);
				} else if (ename2 == "nodeLineColor") {
					COLORREF cr = tags2nodeLineColor(childnode2);
					nodesImport[nodesImport.size()-1].setLineColor(cr);
				}
			}
		} else if (ename == "ilink") {
			iLink l;
			linksImport.push_back(l);
			childnode->get_childNodes(&childs2);
			for (j = 0; j < childs2->Getlength(); j++) {
				childs2->get_item(j, &childnode2);
				BSTR name2 = childnode2->nodeName;
				CString ename2(name2);
				if (ename2 == "from") {
					childnode2->firstChild->get_text(&s);
					CString from(s); int idfrom; sscanf_s(from, "%d", &idfrom);
					linksImport[linksImport.size()-1].setKeyFrom(findPairKey((DWORD)idfrom));
				} else if (ename2 == "to") {
					childnode2->firstChild->get_text(&s);
					CString to(s); int idto; sscanf_s(to, "%d", &idto);
					linksImport[linksImport.size()-1].setKeyTo(findPairKey((DWORD)idto));
				} else if (ename2 == "caption") {
					childnode2->firstChild->get_text(&s);
					linksImport[linksImport.size()-1].setName(CString(s));
				} else if (ename2 == "linkLine") {
					int style(PS_SOLID); int lineWidth(0); int arrow(iLink::line);
					tags2linkStyle(childnode2, style, lineWidth, arrow);
					linksImport[linksImport.size()-1].setLineStyle(style);
					linksImport[linksImport.size()-1].setLineWidth(lineWidth);
					linksImport[linksImport.size()-1].setArrowStyle(arrow);
				} else if (ename2 == "linkLineColor") {
					COLORREF rc = tags2linkColor(childnode2);
					linksImport[linksImport.size()-1].setLinkColor(rc);
				} else if (ename2 == "pathPt") {
					CPoint pt = tags2pathPt(childnode2);
					linksImport[linksImport.size()-1].setPathPt(pt);
				} else if (ename2 == "locate") {
					childnode2->firstChild->get_text(&s);
					CString path(s);
					linksImport[linksImport.size()-1].setPath(path);
					linksImport[linksImport.size()-1].setArrowStyle(iLink::other);
				}
			}
		}
	}
	return true;
}

// シリアライズ用
bool iEditDoc::DomTree2Nodes3(MSXML2::IXMLDOMElement *node)
{
	MSXML2::IXMLDOMNodeList	*childs    = NULL;
	MSXML2::IXMLDOMNodeList	*childs2    = NULL;
	MSXML2::IXMLDOMNodeList	*childs3    = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	MSXML2::IXMLDOMNode		*childnode2 = NULL;
	node->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i, j;
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		if (ename == "inode") {
			iNode n("add"); n.setKey(0);
			n.setNoBrush(FALSE);
			n.setBound(CRect(-1, -1, 0, 0));
			nodesImport.push_back(n);
			childnode->get_childNodes(&childs2);
			for (j = 0; j < childs2->Getlength(); j++) {
				childs2->get_item(j, &childnode2);
				BSTR name2 = childnode2->nodeName;
				CString ename2(name2);
				if (ename2 == "id") {
					childnode2->firstChild->get_text(&s);
					CString ids(s);
					int id;
					sscanf_s(ids, "%d", &id);
					
				//	ids += ' '; f->WriteString(ids); // log
					nodesImport[nodesImport.size()-1].setKey((DWORD)id);
					if (id == 0) {
						nodesImport[nodesImport.size()-1].setTreeState(TVIS_EXPANDED | TVIS_SELECTED);
					} else {
						nodesImport[nodesImport.size()-1].setTreeState(0);
					}
				} else if (ename2 == "pid") {
					childnode2->firstChild->get_text(&s);
					CString pids(s);
					int pid;
					sscanf_s(pids, "%d", &pid);
					nodesImport[nodesImport.size()-1].setParent((DWORD)pid);
			/*	} else if (ename2 == "level") {
					childnode2->firstChild->get_text(&s);
					CString sLevel(s);
					int nLevel;
					sscanf(sLevel, "%d", &nLevel);
					nodesImport[nodesImport.size()-1].setLevel(nLevel); */
				} else if (ename2 == "label") {
					childnode2->firstChild->get_text(&s);
					CString name(s);
					nodesImport[nodesImport.size()-1].setName(name);
					
				//	name += '\n'; f->WriteString(name); // log
					
				} else if (ename2 == "text") {
					childnode2->firstChild->get_text(&s);
					CString text(s);
					text = procLF(text);
					nodesImport[nodesImport.size()-1].setText(text);
				} else if (ename2 == "labelAlign") {
					childnode2->firstChild->get_text(&s);
					CString align(s);
					nodesImport[nodesImport.size()-1].setTextStyle(tag2Align(align));
				} else if (ename2 == "shape") {
					childnode2->firstChild->get_text(&s);
					CString shape(s);
					nodesImport[nodesImport.size()-1].setNodeShape(tag2Shape(shape));
				} else if (ename2 == "bound") {
					CRect rc = nodesImport[nodesImport.size()-1].getBound();
					tags2bound(childnode2, rc);
					rc.NormalizeRect();
					nodesImport[nodesImport.size()-1].setBound(rc);
				} else if (ename2 == "ForColor") {
					COLORREF cr = tags2foreColor(childnode2);
					nodesImport[nodesImport.size()-1].setBrush(cr);
				} else if (ename2 == "nodeLine") {
					int lineStyle(PS_SOLID), lineWidth(0);
					tags2nodeLine(childnode2, lineStyle, lineWidth);
					nodesImport[nodesImport.size()-1].setLineStyle(lineStyle);
					nodesImport[nodesImport.size()-1].setLineWidth(lineWidth);
				} else if (ename2 == "nodeLineColor") {
					COLORREF cr = tags2nodeLineColor(childnode2);
					nodesImport[nodesImport.size()-1].setLineColor(cr);
				}
			}
		} else if (ename == "ilink") {
			iLink l;
			linksImport.push_back(l);
			childnode->get_childNodes(&childs2);
			for (j = 0; j < childs2->Getlength(); j++) {
				childs2->get_item(j, &childnode2);
				BSTR name2 = childnode2->nodeName;
				CString ename2(name2);
				if (ename2 == "from") {
					childnode2->firstChild->get_text(&s);
					CString from(s); int idfrom; sscanf_s(from, "%d", &idfrom);
					linksImport[linksImport.size()-1].setKeyFrom(((DWORD)idfrom));
				} else if (ename2 == "to") {
					childnode2->firstChild->get_text(&s);
					CString to(s); int idto; sscanf_s(to, "%d", &idto);
					linksImport[linksImport.size()-1].setKeyTo(((DWORD)idto));
				} else if (ename2 == "caption") {
					childnode2->firstChild->get_text(&s);
					linksImport[linksImport.size()-1].setName(CString(s));
				} else if (ename2 == "linkLine") {
					int style(PS_SOLID); int lineWidth(0); int arrow(iLink::line);
					tags2linkStyle(childnode2, style, lineWidth, arrow);
					linksImport[linksImport.size()-1].setLineStyle(style);
					linksImport[linksImport.size()-1].setLineWidth(lineWidth);
					linksImport[linksImport.size()-1].setArrowStyle(arrow);
				} else if (ename2 == "linkLineColor") {
					COLORREF rc = tags2linkColor(childnode2);
					linksImport[linksImport.size()-1].setLinkColor(rc);
				} else if (ename2 == "pathPt") {
					CPoint pt = tags2pathPt(childnode2);
					linksImport[linksImport.size()-1].setPathPt(pt);
				} else if (ename2 == "locate") {
					childnode2->firstChild->get_text(&s);
					CString path(s);
					linksImport[linksImport.size()-1].setPath(path);
					linksImport[linksImport.size()-1].setArrowStyle(iLink::other);
				}
			}
		}
	}
	return true;
}

int iEditDoc::tag2Align(const CString &tag)
{
	if (tag == "single-middle-center") {
		return iNode::s_cc;
	} else if (tag == "single-middle-left") {
		return iNode::s_cl;
	} else if (tag == "single-midele-right") {
		return iNode::s_cr;
	} else if (tag == "single-top-center") {
		return iNode::s_tc;
	} else if (tag == "single-top-left") {
		return iNode::s_tl;
	} else if (tag == "single-top-right") {
		return iNode::s_tr;
	} else if (tag == "single-bottom-center") {
		return iNode::s_bc;
	} else if (tag == "single-bottom-left") {
		return iNode::s_bl;
	} else if (tag == "single-bottom-right") {
		return iNode::s_br;
	} else if (tag == "multi-center") {
		return iNode::m_c;
	} else if (tag == "multi-left") {
		return iNode::m_l;
	} else if (tag == "multi-right") {
		return iNode::m_r;
	} else if (tag == "hidden") {
		return iNode::notext;
	}
	return iNode::s_cc;
}


int iEditDoc::tag2Shape(const CString &tag)
{
	if (tag == "Rect") {
		return iNode::rectangle;
	} else if (tag == "Oval") {
		return iNode::arc;
	} else if (tag == "RoundRect") {
		return iNode::roundRect;
	} else if (tag == "MetaFile") {
		return iNode::MetaFile;
	} else if (tag == "MMNode") {
		return (iNode::MindMapNode);
	}
	return iNode::rectangle;
}

void iEditDoc::tags2bound(MSXML2::IXMLDOMNode *pNode, CRect &rc)
{
	MSXML2::IXMLDOMNodeList	*childs    = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	pNode->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i;
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		childnode->firstChild->get_text(&s);
		if (ename == "left") {
			CString left(s);
			sscanf_s(left, "%d", &rc.left);
		} else if (ename == "right") {
			CString right(s);
			sscanf_s(right, "%d", &rc.right);
		} else if (ename == "top") {
			CString top(s);
			sscanf_s(top, "%d", &rc.top);
		} else if (ename == "bottom") {
			childnode->firstChild->get_text(&s);
			CString bottom(s);
			sscanf_s(bottom, "%d", &rc.bottom);
		}
	}
}

COLORREF iEditDoc::tags2foreColor(MSXML2::IXMLDOMNode *pNode)
{
	MSXML2::IXMLDOMNodeList	*childs    = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	pNode->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i;
	int r(255), g(255), b(255);
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		childnode->firstChild->get_text(&s);
		if (ename == "f_red") {
			CString red(s);
			sscanf_s(red, "%d", &r);
		} else if (ename == "f_green") {
			CString green(s);
			sscanf_s(green, "%d", &g);
		} else if (ename == "f_blue") {
			CString blue(s);
			sscanf_s(blue, "%d", &b);
		}
	}
	return RGB(r, g, b);
}

void iEditDoc::tags2nodeLine(MSXML2::IXMLDOMNode *pNode, int &style, int &width)
{
	MSXML2::IXMLDOMNodeList	*childs    = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	pNode->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i;
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		childnode->firstChild->get_text(&s);
		if (ename == "nodeLineStyle") {
			CString lstyle(s);
			if (lstyle == "solidLine") {
				style = PS_SOLID;
			} else if (lstyle == "dotedLine") {
				style = PS_DOT;
			} else if (lstyle == "noLine") {
				style = PS_NULL;
			}
		} else if (ename == "nodeLineWidth") {
			CString lwidth(s); int w; sscanf_s(lwidth, "%d", &w);
			if (w == 1) w = 0;
			width = w;
		}
	}
}


COLORREF iEditDoc::tags2nodeLineColor(MSXML2::IXMLDOMNode *pNode)
{
	MSXML2::IXMLDOMNodeList	*childs    = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	pNode->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i;
	int r(255), g(255), b(255);
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		childnode->firstChild->get_text(&s);
		if (ename == "l_red") {
			CString red(s);
			sscanf_s(red, "%d", &r);
		} else if (ename == "l_green") {
			CString green(s);
			sscanf_s(green, "%d", &g);
		} else if (ename == "l_blue") {
			CString blue(s);
			sscanf_s(blue, "%d", &b);
		}
	}
	return RGB(r, g, b);
}

void iEditDoc::tags2linkStyle(MSXML2::IXMLDOMNode *pNode, int &style, int &width, int &arrow)
{
	MSXML2::IXMLDOMNodeList	*childs    = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	pNode->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i;
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		childnode->firstChild->get_text(&s);
		if (ename == "linkLineStyle") {
			CString lstyle(s);
			if (lstyle == "solidLine") {
				style = PS_SOLID;
			} else if (lstyle == "dotedLine") {
				style = PS_DOT;
			}
		} else if (ename == "linkLineWidth") {
			CString lwidth(s); int w; sscanf_s(lwidth, "%d", &w);
			if (w == 1) w = 0;
			width = w;
		} else if (ename == "arrow") {
			CString astyle(s);
			if (astyle == "a_none") {
				arrow = iLink::line;
			} else if (astyle == "a_single") {
				arrow = iLink::arrow;
			} else if (astyle == "a_double") {
				arrow = iLink::arrow2;
			} else if (astyle == "a_depend") {
				arrow = iLink::depend;
			} else if (astyle == "a_depend_double") {
				arrow = iLink::depend2;
			} else if (astyle == "a_inherit") {
				arrow = iLink::inherit;
			} else if (astyle == "a_aggregat") {
				arrow = iLink::aggregat;
			} else if (astyle == "a_composit") {
				arrow = iLink::composit;
			}
		}
	}
}

COLORREF iEditDoc::tags2linkColor(MSXML2::IXMLDOMNode *pNode)
{
	MSXML2::IXMLDOMNodeList	*childs    = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	pNode->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i;
	int r(255), g(255), b(255);
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		childnode->firstChild->get_text(&s);
		if (ename == "n_red") {
			CString red(s);
			sscanf_s(red, "%d", &r);
		} else if (ename == "n_green") {
			CString green(s);
			sscanf_s(green, "%d", &g);
		} else if (ename == "n_blue") {
			CString blue(s);
			sscanf_s(blue, "%d", &b);
		}
	}
	return RGB(r, g, b);
}


CPoint iEditDoc::tags2pathPt(MSXML2::IXMLDOMNode *pNode)
{
	CPoint pt(0, 0);
	MSXML2::IXMLDOMNodeList	*childs    = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	pNode->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i;
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		childnode->firstChild->get_text(&s);
		if (ename == "path_x") {
			CString x(s);
			sscanf_s(x, "%d", &pt.x);
		} else if (ename == "path_y") {
			CString y(s);
			sscanf_s(y, "%d", &pt.y);
		}
	
	}	
	return pt;
}

bool iEditDoc::DomTree2Nodes(MSXML2::IXMLDOMElement *node, CStdioFile* f)
{
	MSXML2::IXMLDOMNodeList	*childs    = NULL; 
	MSXML2::IXMLDOMNode		*childnode = NULL;
	
	BSTR        s = NULL;
	long        i;
	
	node->get_nodeTypeString(&s);
	
	if(!wcscmp(s,L"element")) {
		node->get_nodeName(&s);
		CString elems(s);
		if (elems == "inode") {
			if (nodeImport.getKey() != -1) {
				nodesImport.push_back(nodeImport);
				nodeImport.setBound(CRect(-1, -1, 0, 0));
				nodeImport.setName("");
				nodeImport.setText("");
				nodeImport.setTreeState(TVIS_EXPANDED);
			}
		} else if (elems == "id") {
			node->firstChild->get_text(&s);
			CString ids(s);
			int id;
			sscanf_s(ids, "%d", &id);
			nodeImport.setKey(getUniqKey());
			idConv idc;
			idc.first = (DWORD)id;
			idc.second = nodeImport.getKey();
			idcVec.push_back(idc);
		} else if (elems == "pid") {
			node->firstChild->get_text(&s);
			CString pids(s);
			int pid;
			sscanf_s(pids, "%d", &pid);
			nodeImport.setParent((DWORD)pid);
		} else if (elems == "label") {
			node->firstChild->get_text(&s);
			CString label(s);
			label += '\n';
			f->WriteString(label);
			
			nodeImport.setName(CString(s));
		} else if (elems == "text") {
			node->firstChild->get_text(&s);
			CString text = CString(s);
			text = procLF(text);
			nodeImport.setText(text);
		} else if (elems == "ilink") {
			if (linkImport.getKeyFrom() != -1 && linkImport.getKeyTo() != -1) {
				linksImport.push_back(linkImport);
				linkImport.setName("");
				linkImport.setPath("");
				linkImport.setArrowStyle(iLink::line);
				linkImport.setLineWidth(0);
			}
		} else if (elems == "from") {
			node->firstChild->get_text(&s);
			CString fromids(s);
			int fromid;
			sscanf_s(fromids, "%d", &fromid);
			linkImport.setKeyFrom(findPairKey((DWORD)fromid));
		} else if (elems == "to") {
			node->firstChild->get_text(&s);
			CString toids(s);
			int toid;
			sscanf_s(toids, "%d", &toid);
			linkImport.setKeyTo(findPairKey((DWORD)toid));
		} else if (elems == "linkLineWidth") {
			node->firstChild->get_text(&s);
			CString wids(s);
			int width; sscanf_s(wids, "%d", &width);
			if (width == 1) {
				width = 0;
			}
			linkImport.setLineWidth(width);
		} else if (elems == "locate") {
			node->firstChild->get_text(&s);
			CString path(s);
			linkImport.setPath(path);
			linkImport.setArrowStyle(iLink::other);
		} else if (elems == "caption") {
			node->firstChild->get_text(&s);
			linkImport.setName(CString(s));
		}
	}
	
	if(node->hasChildNodes()) {
		node->get_childNodes(&childs);
		for(i=0;i < childs->Getlength(); i++) {
			childs->get_item(i,&childnode);
			DomTree2Nodes((MSXML2::IXMLDOMElement *)childnode, f);
		}
	}
	return true;
}

bool iEditDoc::saveXML2(const CString &outPath)
{
	OutlineView* pView = this->getOutlineView();
	Labels ls;
	pView->treeToSequence(ls);
	
	MSXML2::IXMLDOMDocument		*pDoc        = NULL;
	BSTR	bstr = NULL;
	HRESULT hr = S_OK;
    BSTR pBURL = NULL;
    BSTR pBOutputName = NULL;
	
	hr = CoInitialize(NULL);
	if(!SUCCEEDED(hr))
		return false;
	
	hr = CoCreateInstance (MSXML2::CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, 
							MSXML2::IID_IXMLDOMDocument, (LPVOID *)&pDoc);
	
	if(!pDoc) {
		AfxMessageBox("この機能を利用するには Microsoft Internet Explorer 5が必要になります");
		return false;
	}
	
	
	MSXML2::IXMLDOMNode *root = NULL;
	MSXML2::IXMLDOMElement *rootElem = NULL;
	MSXML2::IXMLDOMNode *node = NULL, *p = NULL, *p1 = NULL;
	
	
	// save
	VARIANT vName;
    vName.vt = VT_BSTR;
    V_BSTR(&vName) = outPath.AllocSysString();
    pDoc->save(vName);
	return true;
}

// エクスポート時のXML出力関数
bool iEditDoc::saveXML(const CString &outPath, bool bSerialize)
{
	CStdioFile f;
	CFileStatus status;
	CFileException e;
	CWaitCursor wc;
	if (!f.Open(outPath, CFile::typeText | CFile::modeCreate | CFile::modeWrite, &e)) {
		return false;
	}
	
	OutlineView* pView = getOutlineView();
	
	Labels ls;
	if (bSerialize) {
		pView->treeToSequence0(ls);
	} else {
		pView->treeToSequence(ls);
	}
	
	// Header of XML file
	f.WriteString("<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\n");
	outStyleSheetLine(f);
	f.WriteString("<iEditDoc>\n");
	
	// iNodes -->iNode Data
	for (unsigned int i = 0; i < ls.size(); i++) {
		nodeFind.setKey(ls[i].key);
		const_niterator it = nodes_.findNode(nodeFind);
		
		f.WriteString("\t<inode>\n");
		
		CString ids;
		DWORD key, parent;
		key = (*it).getKey(); parent = (*it).getParent();
		if (i == 0 && key != parent) {
			parent = key;
		}
		ids.Format("\t\t<id>%d</id>\n\t\t<pid>%d</pid>\n", key, parent);
		f.WriteString(ids);
		
		CString sLevel;
		sLevel.Format("\t\t<level>%d</level>\n", (*it).getLevel());
		f.WriteString(sLevel);
		
		f.WriteString("\t\t<label>");
		CString title = "<![CDATA[" + (*it).getName() + "]]>";
		if ((*it).getTextStyle() >= iNode::m_c) {
			f.WriteString(procCR(title));
		} else {
			f.WriteString(title);
		}
		f.WriteString("</label>\n");
		
		f.WriteString("\t\t<text>");
		CString text = "<![CDATA[" + (*it).getText() + "]]>";
		f.WriteString(procCR(text));
		f.WriteString("\n\t\t</text>\n");
		
		// ラベルのアライメント
		f.WriteString("\t\t<labelAlign>");
		int align = (*it).getTextStyle(); CString salign;
		switch (align) {
		case iNode::s_cc: salign = "single-middle-center"; break;
		case iNode::s_cl: salign = "single-middle-left"; break;
		case iNode::s_cr: salign = "single-midele-right"; break;
		case iNode::s_tc: salign = "single-top-center"; break;
		case iNode::s_tl: salign = "single-top-left"; break;
		case iNode::s_tr: salign = "single-top-right"; break;
		case iNode::s_bc: salign = "single-bottom-center"; break;
		case iNode::s_bl: salign = "single-bottom-left"; break;
		case iNode::s_br: salign = "single-bottom-right"; break;
		case iNode::m_c: salign = "multi-center"; break;
		case iNode::m_l: salign = "multi-left"; break;
		case iNode::m_r: salign = "multi-right"; break;
		case iNode::notext: salign = "hidden"; break;
		}
		f.WriteString(salign);
		f.WriteString("</labelAlign>\n");
		
		// 形
		f.WriteString("\t\t<shape>");
		int shape = (*it).getNodeShape();
		if (shape == iNode::rectangle) {
			f.WriteString("Rect");
		} else if (shape == iNode::arc) {
			f.WriteString("Oval");
		} else if (shape == iNode::roundRect) {
			f.WriteString("RoundRect");
		} else if (shape == iNode::MetaFile) {
			f.WriteString("MetaFile");
		} else if (shape == iNode::MindMapNode) {
			f.WriteString("MMNode");
		} else {
			f.WriteString("Rect");
		}
		f.WriteString("</shape>\n");
		
		// 位置
		CString spt;
		CRect bound = (*it).getBound();
		
		f.WriteString("\t\t<bound>\n");
		f.WriteString("\t\t\t<left>");
		spt.Format("%d", bound.left);
		f.WriteString(spt);
		f.WriteString("</left>\n");
		
		f.WriteString("\t\t\t<right>");
		spt.Format("%d", bound.right);
		f.WriteString(spt);
		f.WriteString("</right>\n");
		
		f.WriteString("\t\t\t<top>");
		spt.Format("%d", bound.top);
		f.WriteString(spt);
		f.WriteString("</top>\n");
		
		f.WriteString("\t\t\t<bottom>");
		spt.Format("%d", bound.bottom);
		f.WriteString(spt);
		f.WriteString("</bottom>\n");
		f.WriteString("\t\t</bound>\n");
		
		
		CString sc;
		// 色(塗りつぶし)
		if ((*it).isFilled()) {
			f.WriteString("\t\t<ForColor>\n");
			COLORREF fc = (*it).getBrsColor();
			BYTE fred = GetRValue(fc);
			BYTE fgrn = GetGValue(fc);
			BYTE fblu = GetBValue(fc);
			
			f.WriteString("\t\t\t<f_red>");
			sc.Format("%d", fred);
			f.WriteString(sc);
			f.WriteString("</f_red>\n");
			
			f.WriteString("\t\t\t<f_green>");
			sc.Format("%d", fgrn);
			f.WriteString(sc);
			f.WriteString("</f_green>\n");
			
			f.WriteString("\t\t\t<f_blue>");
			sc.Format("%d", fblu);
			f.WriteString(sc);
			f.WriteString("</f_blue>\n");
			f.WriteString("\t\t</ForColor>\n");
		}
		
		// 線のスタイル
		f.WriteString("\t\t<nodeLine>\n");
		f.WriteString("\t\t\t<nodeLineStyle>");
		if ((*it).getLineStyle() == PS_NULL) {
			f.WriteString("noLine");
		} else if ((*it).getLineStyle() == PS_SOLID) {
			f.WriteString("solidLine");
		} else if ((*it).getLineStyle() == PS_DOT) {
			f.WriteString("dotedLine");
		}
		f.WriteString("</nodeLineStyle>\n");
		
		if ((*it).getLineStyle() == PS_SOLID) {
			f.WriteString("\t\t\t<nodeLineWidth>");
			int width = (*it).getLineWidth();
			if (width == 0) {
				width = 1;
			}
			CString sl; sl.Format("%d", width);
			f.WriteString(sl);
			f.WriteString("</nodeLineWidth>\n");
		}
		f.WriteString("\t\t</nodeLine>\n");
		
		// 色(線)
		if ((*it).getLineStyle() != PS_NULL) {
			f.WriteString("\t\t<nodeLineColor>\n");
			COLORREF lc = (*it).getLineColor();
			BYTE lred = GetRValue(lc);
			BYTE lgrn = GetGValue(lc);
			BYTE lblu = GetBValue(lc);

			f.WriteString("\t\t\t<l_red>");
			sc.Format("%d", lred);
			f.WriteString(sc);
			f.WriteString("</l_red>\n");
			
			f.WriteString("\t\t\t<l_green>");
			sc.Format("%d", lgrn);
			f.WriteString(sc);
			f.WriteString("</l_green>\n");
			
			f.WriteString("\t\t\t<l_blue>");
			sc.Format("%d", lblu);
			f.WriteString(sc);
			f.WriteString("</l_blue>\n");
			f.WriteString("\t\t</nodeLineColor>\n");
		}
		
		// end of inode tag
		f.WriteString("\t</inode>\n");
	}
	
	// iLinks --> iLink Data
	const_literator li = links_.begin();
	for ( ; li != links_.end(); li++) {
		if (!isKeyInLabels(ls, (*li).getKeyFrom()) || !isKeyInLabels(ls, (*li).getKeyTo())) {
			continue;
		}
		
		f.WriteString("\t<ilink>\n");
		CString links;
		if ((*li).getArrowStyle() != iLink::other) {
			links.Format("\t\t<from>%d</from>\n\t\t<to>%d</to>\n", (*li).getKeyFrom(), (*li).getKeyTo());
		} else {
			links.Format("\t\t<from>%d</from>\n\t\t<to>%d</to>\n", (*li).getKeyFrom(), (*li).getKeyFrom());
		}
		f.WriteString(links);
		
		CString caption = "<![CDATA[" + (*li).getName() + "]]>";
		f.WriteString("\t\t<caption>");
		f.WriteString(caption);
		f.WriteString("</caption>\n");
		
		int astyle = (*li).getArrowStyle();
		
		if (astyle == iLink::other && (*li).getPath() != "") {
			f.WriteString("\t\t<locate>");
			CString path = "<![CDATA[" + (*li).getPath() + "]]>";
			f.WriteString(path);
			f.WriteString("</locate>\n");
		} else {
			f.WriteString("\t\t<linkLine>\n");
			
			f.WriteString("\t\t\t<linkLineStyle>");
			if ((*li).getLineStyle() == PS_SOLID) {
				f.WriteString("solidLine");
			} else if ((*li).getLineStyle() == PS_DOT) {
				f.WriteString("dotedLine");
			}
			f.WriteString("</linkLineStyle>\n");
			
			int width = (*li).getLineWidth();
			if (width == 0) {
				width=1;
			}
			CString w; w.Format("\t\t\t<linkLineWidth>%d</linkLineWidth>\n", width);
			f.WriteString(w);
			
			f.WriteString("\t\t\t<arrow>");
			if (astyle == iLink::line) {
				f.WriteString("a_none");
			} else if (astyle == iLink::arrow) {
				f.WriteString("a_single");
			} else if (astyle == iLink::arrow2) {
				f.WriteString("a_double");
			} else if (astyle == iLink::depend) {
				f.WriteString("a_depend");
			} else if (astyle == iLink::depend2) {
				f.WriteString("a_depend_double");
			} else if (astyle == iLink::inherit) {
				f.WriteString("a_inherit");
			} else if (astyle == iLink::aggregat) {
				f.WriteString("a_aggregat");
			} else if (astyle == iLink::composit) {
				f.WriteString("a_composit");
			}
			f.WriteString("</arrow>\n");
			
			f.WriteString("\t\t</linkLine>\n");
			
			
			f.WriteString("\t\t<linkLineColor>\n");
			CString sc;
			COLORREF nc = (*li).getLinkColor();
			BYTE nred = GetRValue(nc);
			BYTE ngrn = GetGValue(nc);
			BYTE nblu = GetBValue(nc);
			
			f.WriteString("\t\t\t<n_red>");
			sc.Format("%d", nred);
			f.WriteString(sc);
			f.WriteString("</n_red>\n");
			
			f.WriteString("\t\t\t<n_green>");
			sc.Format("%d", ngrn);
			f.WriteString(sc);
			f.WriteString("</n_green>\n");
			
			f.WriteString("\t\t\t<n_blue>");
			sc.Format("%d", nblu);
			f.WriteString(sc);
			f.WriteString("</n_blue>\n");
			
			f.WriteString("\t\t</linkLineColor>\n");
			
			
			if ((*li).isCurved()) {
				f.WriteString("\t\t<pathPt>\n");
				CString sp; sp.Format("\t\t\t<path_x>%d</path_x>\n\t\t\t<path_y>%d</path_y>\n", 
					(*li).getPtPath().x, (*li).getPtPath().y);
				f.WriteString(sp);
				f.WriteString("\t\t</pathPt>\n");
			}
		}
		f.WriteString("\t</ilink>\n");
	}
	
	f.WriteString("</iEditDoc>\n");
	f.Flush();
	f.Close();
	return true;
}

void iEditDoc::saveToXML(CArchive &ar)
{
	ar.Flush();
	OutlineView* pView = getOutlineView();
	
	Labels ls;
	pView->treeToSequence0(ls);
	
	// Header of XML file
	ar.WriteString("<?xml version=\"1.0\" encoding=\"Shift_JIS\"?>\n");
	outStyleSheetLine(ar);
	ar.WriteString("<iEditDoc>\n");
	
	// iNodes -->iNode Data
	for (unsigned int i = 0; i < ls.size(); i++) {
		nodeFind.setKey(ls[i].key);
		const_niterator it = nodes_.findNode(nodeFind);
		
		ar.WriteString("\t<inode>\n");
		
		CString ids;
		DWORD key, parent;
		key = (*it).getKey(); parent = (*it).getParent();
		if (i == 0 && key != parent) {
			parent = key;
		}
		ids.Format("\t\t<id>%d</id>\n\t\t<pid>%d</pid>\n", key, parent);
		ar.WriteString(ids);
		
		CString sLevel;
		sLevel.Format("\t\t<level>%d</level>\n", (*it).getLevel());
		ar.WriteString(sLevel);
		
		ar.WriteString("\t\t<label>");
		CString title = "<![CDATA[" + (*it).getName() + "]]>";
		if ((*it).getTextStyle() >= iNode::m_c) {
			ar.WriteString(procCR(title));
		} else {
			ar.WriteString(title);
		}
		ar.WriteString("</label>\n");
		
		ar.WriteString("\t\t<text>");
		CString text = "<![CDATA[" + (*it).getText() + "]]>";
		ar.WriteString(procCR(text));
		ar.WriteString("\n\t\t</text>\n");
		
		// ラベルのアライメント
		ar.WriteString("\t\t<labelAlign>");
		int align = (*it).getTextStyle(); CString salign;
		switch (align) {
		case iNode::s_cc: salign = "single-middle-center"; break;
		case iNode::s_cl: salign = "single-middle-left"; break;
		case iNode::s_cr: salign = "single-midele-right"; break;
		case iNode::s_tc: salign = "single-top-center"; break;
		case iNode::s_tl: salign = "single-top-left"; break;
		case iNode::s_tr: salign = "single-top-right"; break;
		case iNode::s_bc: salign = "single-bottom-center"; break;
		case iNode::s_bl: salign = "single-bottom-left"; break;
		case iNode::s_br: salign = "single-bottom-right"; break;
		case iNode::m_c: salign = "multi-center"; break;
		case iNode::m_l: salign = "multi-left"; break;
		case iNode::m_r: salign = "multi-right"; break;
		case iNode::notext: salign = "hidden"; break;
		}
		ar.WriteString(salign);
		ar.WriteString("</labelAlign>\n");
		
		// 形
		ar.WriteString("\t\t<shape>");
		int shape = (*it).getNodeShape();
		if (shape == iNode::rectangle) {
			ar.WriteString("Rect");
		} else if (shape == iNode::arc) {
			ar.WriteString("Oval");
		} else if (shape == iNode::roundRect) {
			ar.WriteString("RoundRect");
		} else if (shape == iNode::MetaFile) {
			ar.WriteString("MetaFile");
		} else if (shape == iNode::MindMapNode) {
			ar.WriteString("MMNode");
		} else {
			ar.WriteString("Rect");
		}
		ar.WriteString("</shape>\n");
		
		// 位置
		CString spt;
		CRect bound = (*it).getBound();
		
		ar.WriteString("\t\t<bound>\n");
		ar.WriteString("\t\t\t<left>");
		spt.Format("%d", bound.left);
		ar.WriteString(spt);
		ar.WriteString("</left>\n");
		
		ar.WriteString("\t\t\t<right>");
		spt.Format("%d", bound.right);
		ar.WriteString(spt);
		ar.WriteString("</right>\n");
		
		ar.WriteString("\t\t\t<top>");
		spt.Format("%d", bound.top);
		ar.WriteString(spt);
		ar.WriteString("</top>\n");
		
		ar.WriteString("\t\t\t<bottom>");
		spt.Format("%d", bound.bottom);
		ar.WriteString(spt);
		ar.WriteString("</bottom>\n");
		ar.WriteString("\t\t</bound>\n");
		
		
		CString sc;
		// 色(塗りつぶし)
		if ((*it).isFilled()) {
			ar.WriteString("\t\t<ForColor>\n");
			COLORREF fc = (*it).getBrsColor();
			BYTE fred = GetRValue(fc);
			BYTE fgrn = GetGValue(fc);
			BYTE fblu = GetBValue(fc);
			
			ar.WriteString("\t\t\t<f_red>");
			sc.Format("%d", fred);
			ar.WriteString(sc);
			ar.WriteString("</f_red>\n");
			
			ar.WriteString("\t\t\t<f_green>");
			sc.Format("%d", fgrn);
			ar.WriteString(sc);
			ar.WriteString("</f_green>\n");
			
			ar.WriteString("\t\t\t<f_blue>");
			sc.Format("%d", fblu);
			ar.WriteString(sc);
			ar.WriteString("</f_blue>\n");
			ar.WriteString("\t\t</ForColor>\n");
		}
		
		// 線のスタイル
		ar.WriteString("\t\t<nodeLine>\n");
		ar.WriteString("\t\t\t<nodeLineStyle>");
		if ((*it).getLineStyle() == PS_NULL) {
			ar.WriteString("noLine");
		} else if ((*it).getLineStyle() == PS_SOLID) {
			ar.WriteString("solidLine");
		} else if ((*it).getLineStyle() == PS_DOT) {
			ar.WriteString("dotedLine");
		}
		ar.WriteString("</nodeLineStyle>\n");
		
		if ((*it).getLineStyle() == PS_SOLID) {
			ar.WriteString("\t\t\t<nodeLineWidth>");
			int width = (*it).getLineWidth();
			if (width == 0) {
				width = 1;
			}
			CString sl; sl.Format("%d", width);
			ar.WriteString(sl);
			ar.WriteString("</nodeLineWidth>\n");
		}
		ar.WriteString("\t\t</nodeLine>\n");
		
		// 色(線)
		if ((*it).getLineStyle() != PS_NULL) {
			ar.WriteString("\t\t<nodeLineColor>\n");
			COLORREF lc = (*it).getLineColor();
			BYTE lred = GetRValue(lc);
			BYTE lgrn = GetGValue(lc);
			BYTE lblu = GetBValue(lc);

			ar.WriteString("\t\t\t<l_red>");
			sc.Format("%d", lred);
			ar.WriteString(sc);
			ar.WriteString("</l_red>\n");
			
			ar.WriteString("\t\t\t<l_green>");
			sc.Format("%d", lgrn);
			ar.WriteString(sc);
			ar.WriteString("</l_green>\n");
			
			ar.WriteString("\t\t\t<l_blue>");
			sc.Format("%d", lblu);
			ar.WriteString(sc);
			ar.WriteString("</l_blue>\n");
			ar.WriteString("\t\t</nodeLineColor>\n");
		}
		
		// end of inode tag
		ar.WriteString("\t</inode>\n");
	}
	
	// iLinks --> iLink Data
	const_literator li = links_.begin();
	for ( ; li != links_.end(); li++) {
		if (!isKeyInLabels(ls, (*li).getKeyFrom()) || !isKeyInLabels(ls, (*li).getKeyTo())) {
			continue;
		}
		
		ar.WriteString("\t<ilink>\n");
		CString links;
		if ((*li).getArrowStyle() != iLink::other) {
			links.Format("\t\t<from>%d</from>\n\t\t<to>%d</to>\n", (*li).getKeyFrom(), (*li).getKeyTo());
		} else {
			links.Format("\t\t<from>%d</from>\n\t\t<to>%d</to>\n", (*li).getKeyFrom(), (*li).getKeyFrom());
		}
		ar.WriteString(links);
		
		CString caption = "<![CDATA[" + (*li).getName() + "]]>";
		ar.WriteString("\t\t<caption>");
		ar.WriteString(caption);
		ar.WriteString("</caption>\n");
		
		int astyle = (*li).getArrowStyle();
		
		if (astyle == iLink::other && (*li).getPath() != "") {
			ar.WriteString("\t\t<locate>");
			CString path = "<![CDATA[" + (*li).getPath() + "]]>";
			ar.WriteString(path);
			ar.WriteString("</locate>\n");
		} else {
			ar.WriteString("\t\t<linkLine>\n");
			
			ar.WriteString("\t\t\t<linkLineStyle>");
			if ((*li).getLineStyle() == PS_SOLID) {
				ar.WriteString("solidLine");
			} else if ((*li).getLineStyle() == PS_DOT) {
				ar.WriteString("dotedLine");
			}
			ar.WriteString("</linkLineStyle>\n");
			
			int width = (*li).getLineWidth();
			if (width == 0) {
				width=1;
			}
			CString w; w.Format("\t\t\t<linkLineWidth>%d</linkLineWidth>\n", width);
			ar.WriteString(w);
			
			ar.WriteString("\t\t\t<arrow>");
			if (astyle == iLink::line) {
				ar.WriteString("a_none");
			} else if (astyle == iLink::arrow) {
				ar.WriteString("a_single");
			} else if (astyle == iLink::arrow2) {
				ar.WriteString("a_double");
			} else if (astyle == iLink::depend) {
				ar.WriteString("a_depend");
			} else if (astyle == iLink::depend2) {
				ar.WriteString("a_depend_double");
			} else if (astyle == iLink::inherit) {
				ar.WriteString("a_inherit");
			} else if (astyle == iLink::aggregat) {
				ar.WriteString("a_aggregat");
			} else if (astyle == iLink::composit) {
				ar.WriteString("a_composit");
			}
			ar.WriteString("</arrow>\n");
			
			ar.WriteString("\t\t</linkLine>\n");
			
			
			ar.WriteString("\t\t<linkLineColor>\n");
			CString sc;
			COLORREF nc = (*li).getLinkColor();
			BYTE nred = GetRValue(nc);
			BYTE ngrn = GetGValue(nc);
			BYTE nblu = GetBValue(nc);
			
			ar.WriteString("\t\t\t<n_red>");
			sc.Format("%d", nred);
			ar.WriteString(sc);
			ar.WriteString("</n_red>\n");
			
			ar.WriteString("\t\t\t<n_green>");
			sc.Format("%d", ngrn);
			ar.WriteString(sc);
			ar.WriteString("</n_green>\n");
			
			ar.WriteString("\t\t\t<n_blue>");
			sc.Format("%d", nblu);
			ar.WriteString(sc);
			ar.WriteString("</n_blue>\n");
			
			ar.WriteString("\t\t</linkLineColor>\n");
			
			
			if ((*li).isCurved()) {
				ar.WriteString("\t\t<pathPt>\n");
				CString sp; sp.Format("\t\t\t<path_x>%d</path_x>\n\t\t\t<path_y>%d</path_y>\n", 
					(*li).getPtPath().x, (*li).getPtPath().y);
				ar.WriteString(sp);
				ar.WriteString("\t\t</pathPt>\n");
			}
		}
		ar.WriteString("\t</ilink>\n");
	}
	
	ar.WriteString("</iEditDoc>\n");
	ar.Flush();
}

DWORD iEditDoc::findPairKey(const DWORD first)
{
	for (unsigned int i = 0; i < idcVec.size(); i++) {
		if (idcVec[i].first == first) {
			return idcVec[i].second;
		}
	}
	return -1;
}

void iEditDoc::addImportData(bool brepRoot)
{
//	sort(nodesImport.begin(), nodesImport.end());
	unsigned int i;
	for (i = 0; i < nodesImport.size(); i++) {
		nodesImport[i].setParent(findPairKey(nodesImport[i].getParent()));
		if (nodesImport[i].getKey() == nodesImport[i].getParent()) {
			nodesImport[i].setParent(nodes_.getSelKey());
		}
	}
	
	// ノードを置き換える処理をここに書く
	if (brepRoot) {
		DWORD sel = nodes_.getSelKey();
		DWORD start = nodesImport[0].getKey();
		nodesImport[0].setKey(sel);
		for (i = 0; i < nodesImport.size(); i++) {
			if (nodesImport[i].getParent() == start) {
				nodesImport[i].setParent(sel);
			}
		}
		for (i = 0; i < linksImport.size(); i++) {
			if (linksImport[i].getKeyFrom() == start) {
				linksImport[i].setKeyFrom(sel);
			}
			if (linksImport[i].getKeyTo() == start) {
				linksImport[i].setKeyTo(sel);
			}
		}
		niterator it = nodes_.getSelectedNode();
		(*it).setName(nodesImport[0].getName());
		(*it).setText(nodesImport[0].getText());
		(*it).setBound(nodesImport[0].getBound());
	}
	
	// nodeの格納
	sv.clear();
	sv.resize(0);
	for (int k = 0; sv.size() < nodesImport.size() && k < 100; k++) { // 100ループまで
		if (brepRoot) {
			i = 1;
		} else {
			i = 0;
		}
		for ( ; i < nodesImport.size(); i++) {
			nodeFind.setKey(nodesImport[i].getParent());
			niterator itp = nodes_.find(nodeFind);
			nodeFind.setKey(nodesImport[i].getKey());
			niterator it = nodes_.find(nodeFind);
			if (itp != nodes_.end() && it == nodes_.end()) {
				CRect rc = nodesImport[i].getBound();
				if (rc.left < 0 && rc.top < 0) {
					nodesImport[i].moveTo(CPoint(0, 0));
					nodesImport[i].moveBound(CSize(rand()%800, rand()%600));
				}
				nodes_.insert(nodesImport[i]);
				sv.push_back(nodesImport[i].getKey());
			}
		}
	}
	
	// linkの格納
	for (i = 0; i < linksImport.size(); i++) {
		if (linksImport[i].getKeyFrom() != -1 && linksImport[i].getKeyTo() != -1) {
			linksImport[i].setKey(lastLinkKey++);
			links_.push_back(linksImport[i]);
		}
	}
	setConnectPoint();
	SetModifiedFlag();

	if (brepRoot) {
		iHint hint; hint.event = iHint::nodeLabelChanged;
		hint.str = nodesImport[0].getName();
		DWORD key = nodes_.getSelKey();
		UpdateAllViews(NULL, (LPARAM)key, &hint);
	}
	
	iHint hint; hint.event = iHint::parentSel;
	DWORD key = nodes_.getSelKey();
	calcMaxPt(m_maxPt);
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::randomNodesPos(const CSize &area, bool bDrwAll)
{
	niterator it = nodes_.begin();
	for ( ; it != nodes_.end(); it++) {
//		if (!bDrwAll) {
			if (!(*it).isVisible()) continue;
//		}
		(*it).moveTo(CPoint(0, 0));
		(*it).moveBound(CSize(rand()%area.cx, rand()%area.cy));
	}
	setConnectPoint();
	calcMaxPt(m_maxPt);
	SetModifiedFlag();
}

void iEditDoc::writeTextHtml(DWORD key, CStdioFile* f, bool textIsolated, const CString& textPrefix)
{
	nodeFind.setKey(key);
	const_niterator it = nodes_.findNode(nodeFind);
	
	CString nameStr = Utilities::removeCR((*it).getName());
	// リンクタグの生成
	f->WriteString("<a id=");
	f->WriteString("\"");
	CString keystr;
	keystr.Format("%d", (*it).getKey());
	f->WriteString(keystr);
	f->WriteString("\" />\n");
	
	// 内容書き込み
	f->WriteString("<h1>" + nameStr + "</h1>\n");
	f->WriteString("<div class=\"text\">\n");
	f->WriteString(procWikiNotation((*it).getText()));
	f->WriteString("</div>\n");
	
	// リンクの書き込み
	f->WriteString("<div class=\"links\">\n");
	const_literator li = links_.begin();
	CString sLink("<ul>\n");
	int cnt=0;
	for (; li != links_.end(); li++) {
		if ((*li).getKeyFrom() != (*it).getKey() &&
			(*li).getKeyTo() != (*it).getKey()) {
			continue;
		}
		if ((*li).getArrowStyle() != iLink::other) {
			if ((*it).getKey() == (*li).getKeyFrom()) {
				nodeFind.setKey((*li).getKeyTo());
				const_niterator itTo = nodes_.findNode(nodeFind);
				if (itTo != nodes_.end()) {
					CString keystr;
					keystr.Format("%d", (*li).getKeyTo());
					sLink += "<li><a href=";
					if (!textIsolated) {
						sLink += "\"#";
						sLink += keystr;
					} else {
						sLink += "\"" + textPrefix + keystr + ".html\"";
					}
					sLink += "\">";
					sLink += "▲" + Utilities::removeCR((*itTo).getName());
					if ((*li).getName() != "") {
						sLink += "(" + (*li).getName() + ")";
					}
					sLink += "</a></li>\n";
					cnt++;
				}
			} else if ((*it).getKey() == (*li).getKeyTo()) {
				nodeFind.setKey((*li).getKeyFrom());
				const_niterator itFrom = nodes_.findNode(nodeFind);
				if (itFrom != nodes_.end()) {
					CString keystr;
					keystr.Format("%d", (*li).getKeyFrom());
					sLink += "<li><a href=";
					if (!textIsolated) {
						sLink += "\"#";
						sLink += keystr;
					} else {
						sLink += "\"" + textPrefix + keystr + ".html\"";
					}
					sLink += "\">";
					sLink += "▽" + Utilities::removeCR((*itFrom).getName());
					if ((*li).getName() != "") {
						sLink += "(" + (*li).getName() + ")";
					}
					sLink += "</a></li>\n";
					cnt++;
				}
			}
		} else {
			CString url = (*li).getPath();
			if (url.Find("http://") != -1 || url.Find("https://") != -1 || url.Find("ftp://") != -1) {
				sLink += "<li><a href=";
				sLink += url;
				sLink += " target=\"_top\">";
				if ((*li).getName() != "") {
					sLink += (*li).getName();
				} else {
					sLink += url;
				}
				sLink += "</a></li>\n";
				cnt++;
			}
		}
	}
	sLink += "</ul>\n";
	if (cnt > 0) {
		f->WriteString(sLink);
	}
	f->WriteString("</div>\n");
}

CString iEditDoc::procWikiNotation(const CString &text)
{
	const std::tr1::regex h2("^\\*\\s([^\\*].*)$"); //"^-.*$" "^[0-9].*$" "^\\*.*$"
	const std::tr1::regex h3("^\\*\\*\\s([^\\*].*)$");
	const std::tr1::regex h4("^\\*\\*\\*\\s([^\\*].*)$");
	const std::tr1::regex l1("^-\\s([^-].*)$");
	const std::tr1::regex l2("^--\\s([^-].*)");
	const std::tr1::regex l3("^---\\s([^-].*)");
	const std::tr1::regex uri("^.*(https?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+).*$");
	std::tr1::match_results<std::string::const_iterator> result;
	
	vector<CString> lines = Utilities::getLines(text);
	int level = 0;
	int prevLevel = 0;
	CString rtnStr;
	bool pre = false;
	for (unsigned int i = 0; i < lines.size(); i++) {
		std::string line = lines[i];
		if (line.find("<pre>") != -1) {
			pre = true;
		} else if (line.find("</pre>") != -1) {
			pre = false;
		}
		if (pre) {
			rtnStr += lines[i] + "\n";
			continue;
		}
		if (std::tr1::regex_match(line, result, h2)) {
			endUL(rtnStr, level);
			rtnStr += "<h2>";
			rtnStr += makeInlineUrlLink(CString(result[1].str().c_str()));
			rtnStr += "</h2>\n";
		} else if (std::tr1::regex_match(line, result, h3)) {
			endUL(rtnStr, level);
			rtnStr += "<h3>";
			rtnStr += makeInlineUrlLink(CString(result[1].str().c_str()));
			rtnStr += "</h3>\n";
		} else if (std::tr1::regex_match(line, result, h4)) {
			endUL(rtnStr, level);
			rtnStr += "<h4>";
			rtnStr += makeInlineUrlLink(CString(result[1].str().c_str()));
			rtnStr += "</h4>\n";
		} else if (std::tr1::regex_match(line, result, l1)) {
			prevLevel = level;
			level = 1;
			beginUL(rtnStr, level, prevLevel);
			rtnStr += "<li>";
			rtnStr += makeInlineUrlLink(CString(result[1].str().c_str()));
			rtnStr += "</li>\n";
		} else if (std::tr1::regex_match(line, result, l2)) {
			prevLevel = level;
			level = 2;
			beginUL(rtnStr, level, prevLevel);
			rtnStr += "<li>";
			rtnStr += makeInlineUrlLink(CString(result[1].str().c_str()));
			rtnStr += "</li>\n";
		} else if (std::tr1::regex_match(line, result, l3)) {
			prevLevel = level;
			level = 3;
			beginUL(rtnStr, level, prevLevel);
			rtnStr += "<li>";
			rtnStr += makeInlineUrlLink(CString(result[1].str().c_str()));
			rtnStr += "</li>\n";
		} else {
			endUL(rtnStr, level);
			rtnStr += makeInlineUrlLink(CString(line.c_str()));
			rtnStr += "<br />\n";
		}
	}
	return rtnStr;
}

// インラインのURLを検出する 今のところ最初の1個のみ
CString iEditDoc::makeInlineUrlLink(const CString &line)
{
	const std::tr1::regex uri("^(.*)(https?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+)(.*)$");
	const std::tr1::regex wikiLink("^(.*)\\[\\[(.+):(https?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+)\\]\\](.*)$");
	std::string sLine = line;
	std::tr1::match_results<std::string::const_iterator> result;
	if (std::tr1::regex_match(sLine, result, wikiLink)) {
		CString rtnStr = result[1].str().c_str();
		rtnStr += "<a href=\"";
		rtnStr += result[3].str().c_str();
		rtnStr += "\" target=\"_blank\">";
		rtnStr += result[2].str().c_str();
		rtnStr += "</a>";
		rtnStr += result[4].str().c_str();
		return rtnStr;
	} else if (std::tr1::regex_match(sLine, result, uri)) {
		CString rtnStr = result[1].str().c_str();
		rtnStr += "<a href=\"";
		rtnStr += result[2].str().c_str();
		rtnStr += "\" target=\"_blank\">";
		rtnStr += result[2].str().c_str();
		rtnStr += "</a>";
		rtnStr += result[3].str().c_str();
		return rtnStr;
	} 
	return line;
}

void iEditDoc::beginUL(CString& str, int& level, int& prevLevel)
{
	if (prevLevel == level - 1) {
		str += "<ul>\n";
	} else if (prevLevel == level -2) {
		str += "<ul>\n<ul>\n";
	} else if (prevLevel == level + 1) {
		str += "</ul>\n";
	} else if (prevLevel == level + 2) {
		str += "</ul>\n</ul>\n";
	}
}

void iEditDoc::endUL(CString & str, int& level)
{
	if (level == 1) {
		str += "</ul>\n";
	} else if (level == 2) {
		str += "</ul>\n</ul>\n";
	} else if (level == 3) {
		str += "</ul>\n</ul>\n</ul>\n";
	}
	if (level > 0) level = 0;
}

CString iEditDoc::getKeyNodeText(DWORD key)
{
	nodeFind.setKey(key);
	const_niterator it = nodes_.findNode(nodeFind);
	if (it != nodes_.end()) {
		return (*it).getText();
	}
	return "";
}

CString iEditDoc::getKeyNodeLabel(DWORD key)
{
	nodeFind.setKey(key);
	const_niterator it = nodes_.findNode(nodeFind);
	if (it != nodes_.end()) {
		return (*it).getName();
	}
	return "";
}

CString iEditDoc::procCR(const CString &str)
{
	CString toStr;
	for (int i = 0; i < str.GetLength(); i++) {
		if (str[i] == '\n') {
			;
		} else if (str[i] == '\r') {
			toStr += "\n";
		} else {
			toStr += str[i];
		}
	}
	return toStr;
}

CString iEditDoc::procLF(const CString &str)
{
	CString toStr;
	for (int i = 0; i < str.GetLength(); i++) {
		if (str[i] == '\r') {
			;
		} else if (str[i] == '\n') {
			toStr += "\r\n";
		} else {
			toStr += str[i];
		}
	}
	return toStr;
}

bool iEditDoc::isKeyInLabels(const Labels &labels, DWORD key)
{
	if (labels.size() == nodes_.size()) return true;
	for (unsigned int i = 0; i < labels.size(); i++) {
		if (labels[i].key == key) {
			return true;
		}
	}
	return false;
}

void iEditDoc::listUpNodes(const CString &sfind, Labels &labels, BOOL bLabel, BOOL bText, BOOL bLinks, BOOL bUpper)
{
	CString sf = sfind;
	if (bUpper) {
		sf.MakeUpper();
	}
	niterator it = nodes_.begin();
	for ( ; it != nodes_.end(); it++) {
		bool done = false;
		CString name = (*it).getName();
		CString text = (*it).getText();
		if (bUpper) {
			name.MakeUpper();
			text.MakeUpper();
		}
		if (bLabel && name.Find(sf) != -1) {
			label l;
			l.name = (*it).getName();
			l.key = (*it).getKey();
			l.state = 0;
			labels.push_back(l);
			done = true;
		}
		if (bText && text.Find(sf) != -1) {
			label l;
			l.name = (*it).getName();
			l.key = (*it).getKey();
			l.state = 1;
			labels.push_back(l);
		}
	}
	
	literator li = links_.begin();
	for ( ; li != links_.end(); li++) {
		CString name = (*li).getName();
		CString path = (*li).getPath();
		if (bUpper) {
			name.MakeUpper();
			path.MakeUpper();
		}
		if (bLinks && name.Find(sf) != -1) {
			label l;
			l.key = (*li).getKeyFrom();
			l.name = (*li).getName();
			l.state = 2;
			labels.push_back(l);
		}
		if (bLinks && path.Find(sf) != -1) {
			label l;
			l.key = (*li).getKeyFrom();
			l.name = (*li).getPath();
			l.state = 3;
			labels.push_back(l);
		}
	}
}

HENHMETAFILE iEditDoc::getSelectedNodeMetaFile()
{
	niterator it = nodes_.getSelectedNode();
	if (it != nodes_.end()) {
		return (*it).getMetaFile();
	}
	return NULL;
}

void iEditDoc::drawNodesSelected(CDC *pDC, bool bDrwAll)
{
	nodes_.drawNodesSelected(pDC);
}

void iEditDoc::drawLinksSelected(CDC *pDC, bool bDrwAll, bool clipbrd)
{
	literator li = links_.begin();
	for ( ; li != links_.end(); li++) {
		nodeFind.setKey((*li).getKeyFrom());
		niterator itFrom = nodes_.findNodeW(nodeFind);
		nodeFind.setKey((*li).getKeyTo());
		niterator itTo = nodes_.findNodeW(nodeFind);
		if ((*li).getArrowStyle() != iLink::other && (*itFrom).isSelected() && (*itTo).isSelected()) {
			(*li).drawArrow(pDC);
			(*li).drawLine(pDC);
			(*li).drawComment(pDC, clipbrd);
		}
	}
}

void iEditDoc::backUpUndoNodes()
{
	nodes_undo.clear();
	nodes_undo.resize(0);
	niterator it = nodes_.begin();
	for ( ; it != nodes_.end(); it++) {
		if ((*it).isVisible()) {
			iNode n(*it);
			nodes_undo.push_back(n);
		}
	}
}

void iEditDoc::resumeUndoNodes()
{
	for (unsigned int i = 0; i < nodes_undo.size(); i++) {
		nodeFind.setKey(nodes_undo[i].getKey());
		niterator it = nodes_.findNodeW(nodeFind);
		if (it != nodes_.end()) {
			(*it) = nodes_undo[i];
		}
	}
//	setConnectPoint();
	calcMaxPt(m_maxPt);
	nodes_undo.clear();
	nodes_undo.resize(0);
	SetModifiedFlag();
}

bool iEditDoc::canResumeUndo()
{
	if (nodes_undo.size() == 0 && links_undo.size() == 0) {
		return false;
	}
	return true;
}

void iEditDoc::disableUndo()
{
	nodes_undo.clear();
	nodes_undo.resize(0);
	links_undo.clear();
	links_undo.resize(0);
}

void iEditDoc::backUpUndoLinks()
{
	links_undo.clear();
	links_undo.resize(0);
	literator li = links_.begin();
	for ( ; li != links_.end(); li++) {
		if ((*li).canDraw()) {
			iLink l = (*li);
			links_undo.push_back(l);
		}
	}
}

void iEditDoc::resumeUndoLinks()
{
	for (unsigned int i = 0; i < links_undo.size(); i++) {
		literator li = links_.begin();
		for ( ; li != links_.end(); li++) {
			if (links_undo[i].getKey() == (*li).getKey()) {
				(*li) = links_undo[i];
				break;
			}
		}
	}
	calcMaxPt(m_maxPt);
	links_undo.clear();
	links_undo.resize(0);
	SetModifiedFlag();
}

void iEditDoc::adjustNodesEnd(const CString& side, const CRect& rect, bool bDrwAll)
{
	niterator it = nodes_.begin();
	for ( ; it != nodes_.end(); it++) {
	//	if (!bDrwAll) {
			if (!(*it).isVisible()) continue;
	//	}
		if (!(*it).isSelected()) continue;
		CPoint pt;
		if (side == "left") {
			pt.x = rect.left;
			pt.y = (*it).getBound().top;
		} else if (side == "right") {
			pt.x = (*it).getBound().left + rect.right - (*it).getBound().right;
			pt.y = (*it).getBound().top;
		} else if (side == "top") {
			pt.x = (*it).getBound().left;
			pt.y = rect.top;
		} else if (side == "bottom") {
			pt.x = (*it).getBound().left;
			pt.y = (*it).getBound().top + rect.bottom - (*it).getBound().bottom;
		}
		
		(*it).moveTo(pt);
	}
	setConnectPoint();
	calcMaxPt(m_maxPt);
	SetModifiedFlag();
}

void iEditDoc::sameNodesSize(const CString &strSize, bool bDrwAll)
{
	CSize maxSz = nodes_.getMaxNodeSize(true, bDrwAll);
	niterator it = nodes_.begin();
	CRect rc;
	for ( ; it != nodes_.end(); it++) {
//		if (!bDrwAll) {
			if (!(*it).isVisible()) continue;
//		}
		if (!(*it).isSelected()) continue;
		rc = (*it).getBound();
		if (strSize == "height") {
			rc.bottom = rc.top + maxSz.cy;
		} else if (strSize == "width") {
			rc.right = rc.left + maxSz.cx;
		} else if (strSize == "rect") {
			rc.bottom = rc.top + maxSz.cy;
			rc.right = rc.left + maxSz.cx;
		}
		(*it).setBound(rc);
	}
	setConnectPoint();
	calcMaxPt(m_maxPt);
	SetModifiedFlag();
}

const iLink* iEditDoc::getSelectedLink(bool bDrawAll) const
{
	const_literator li = links_.getSelectedLink(bDrawAll);
	if (li == links_.end()) {
		return NULL;
	}
	return &(*li);
}

void iEditDoc::setSelectedLinkReverse(bool bDrwAll)
{
	const iLink* pl = getSelectedLink(bDrwAll);
	if (pl == NULL) return;
	DWORD keyTo = pl->getKeyTo();
	links_.setSelectedLinkReverse(bDrwAll);
	SetModifiedFlag();
	selChanged(keyTo, true, isShowSubBranch());
	iHint h; h.event = iHint::linkModified;
	UpdateAllViews(NULL, (LPARAM)getSelectedNodeKey(), &h);
}

void iEditDoc::viewSettingChanged()
{
	iHint hint; hint.event = iHint::viewSettingChanged;
	DWORD key = nodes_.getSelKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::exportSVG(bool bDrwAll, const CString &path, bool bEmbed,
						 const CString& textFileName, bool textSingle)
{
	serialVec vec = getOutlineView()->getDrawOrder(isShowSubBranch());
	SvgWriter writer(nodes_, links_, vec, bDrwAll);
	if (textSingle) {
		writer.setTextHtmlFileName(textFileName);
	} else {
		writer.setTextHtmlFilePrefix(textFileName);
	}
	writer.exportSVG(path, getMaxPt(), bEmbed);
}

iNode iEditDoc::getHitNode(const CPoint &pt, bool bDrwAll)
{
	iNode* pNode = nodes_.hitTest2(pt, bDrwAll);
	if (pNode != NULL) {
		iNode node = *pNode;
		return node;
	} else {
		iNode nil;
        return nil;
	}
}

void iEditDoc::setVisibleNodes(KeySet& keySet)
{
	m_visibleKeys = keySet;
	nodes_.setVisibleNodes(keySet);
}

void iEditDoc::setShowBranch(DWORD branchRootKey)
{
	m_dwBranchRootKey = branchRootKey;
	m_bShowBranch = true;
	iHint hint; hint.event = iHint::showSubBranch;
	DWORD key = nodes_.getSelKey();
	calcMaxPt(m_maxPt);
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::resetShowBranch()
{
	m_bShowBranch = false;
	nodes_.setVisibleNodes(nodes_.getCurParent());
	iHint hint; hint.event = iHint::resetShowSubBranch;
	DWORD key = nodes_.getSelKey();
	calcMaxPt(m_maxPt);
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

CString iEditDoc::getSubBranchRootLabel() const
{
	iNode nodef;
	nodef.setKey(m_dwBranchRootKey);
	const_niterator n = nodes_.findNode(nodef);
	if (n != nodes_.end()) {
		return (*n).getName();
	}
	return "";
}

bool iEditDoc::isShowSubBranch() const
{
	return m_bShowBranch;
}

bool iEditDoc::isOldBinary() const
{
	return m_bOldBinary;
}

bool iEditDoc::isCurKeyInBranch() const
{
	DWORD key = getSelectedNodeKey();
	set<DWORD>::const_iterator it = m_visibleKeys.find(key);
	if (it == m_visibleKeys.end()) {
		return false;
	}
	return true;
}

void iEditDoc::OnFileSaveAs() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	CString fullPath = GetPathName();
	CString fileName;
	
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath_s(fullPath, drive, dir, fname, ext );
	
	fileName = fname;
	CString driveName = drive;
	if (driveName == "") {
		iNode nf; nf.setKey(0);
		const_niterator it = nodes_.find(nf);
		CString rootLabel = (*it).getName();
		if (rootLabel != "主題") {
			// TODO:ファイル名として不正な文字の除去
			CString safeFileName = Utilities::getSafeFileName(rootLabel);
			if (safeFileName == "") {
				fileName = GetTitle();
			} else {
				fileName = safeFileName;
			}
		} else {
			fileName = GetTitle();
		}
	}
	
	CString szFilter = "iEditファイル(*.iedx)|*.iedx|iEditファイル(旧)(*.ied)|*.ied|XMLファイル(*.xml)|*.xml||";
	CFileDialog cfDlg(FALSE, NULL, fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |
		OFN_FILEMUSTEXIST | OFN_EXPLORER, szFilter, AfxGetMainWnd());
	if (cfDlg.DoModal() == IDOK) {
		
		DWORD index = cfDlg.m_ofn.nFilterIndex;
		CString pathName = cfDlg.GetPathName();
		CString ext = cfDlg.GetFileExt();
		ext.MakeLower();
		
		switch (index) {
		case 1: // iedx
			if (ext != "iedx" && ext != "ied" && ext != "xml") {
				OnSaveDocument(pathName + ".iedx");
				SetPathName(cfDlg.GetPathName() + ".iedx");
			} else if (ext == "iedx" || ext == "ied"|| ext == "xml") {
				OnSaveDocument(pathName);
				SetPathName(cfDlg.GetPathName());
			}
			break;
		case 2: // ied
			if (ext != "iedx" && ext != "ied" && ext != "xml") {
				OnSaveDocument(pathName + ".ied");
				SetPathName(cfDlg.GetPathName() + ".ied");
			} else if (ext == "iedx" || ext == "ied"|| ext == "xml") {
				OnSaveDocument(pathName);
				SetPathName(cfDlg.GetPathName());
			}
			break;
		case 3: // xml
			if (ext != "iedx" && ext != "xml" && ext != "ied") {
				OnSaveDocument(cfDlg.GetPathName() + ".xml");
				SetPathName(cfDlg.GetPathName() + ".xml");
			} else if (ext == "iedx" || ext == "ied"|| ext == "xml") {
				OnSaveDocument(pathName);
				SetPathName(cfDlg.GetPathName());
			}
			break;
		}
	}
}

void iEditDoc::OnFileSave()
{
	// TODO: ここにコマンド ハンドラ コードを追加します。
	CString fullPath = GetPathName();
	CString driveName;
	CString dirName;
	CString fileName = GetTitle();
	CString extName;
	
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath_s(fullPath, drive, dir, fname, ext );
	driveName = drive;
	dirName = dir;
	fileName = fname;
	extName = ext;
	
	if (driveName == "") {
		OnFileSaveAs();
	} else {
		OnSaveDocument(fullPath);
	}
}

void iEditDoc::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
}

CRect iEditDoc::restoreDeleteBound() const
{
	return m_deleteBound;
}

void iEditDoc::backupDeleteBound()
{
	m_deleteBound = getRelatedBound(false);
}

void iEditDoc::setNodeLevel(const DWORD key, const int nLevel)
{
	nodeFind.setKey(key);
	niterator it = nodes_.findNodeW(nodeFind);
	if (it == nodes_.end()) {
		return;
	}
	(*it).setLevel(nLevel);
}


// 芋蔓に必要なノード間距離を計算する
// iNodeのBoundPreも初期化する多機能メソッド←ダメ
void iEditDoc::calcEdges()
{
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	
	literator li = links_.begin();
	for ( ; li != links_.end(); li++) {
		if ((*li).getArrowStyle() != iLink::other) {
			// 距離計算
			if ((*li).getKeyFrom() == (*li).getKeyTo()) continue;
			if (!(*li).canDraw() || !(*li).isInChain()) {
				(*li).setLen(-1.0);
			} else {
				CSize sz;
				sz.cx = ((*li).getRectFrom().Width() + (*li).getRectTo().Width())/2;
				sz.cy = ((*li).getRectFrom().Height() + (*li).getRectTo().Height())/2;
				double rate;
				if (pApp->m_rgsLink.bSetStrength) {
					rate = width2Len((*li).getLineWidth());
					if ((*li).getLineStyle() == PS_DOT) {
						rate *= 1.5;
					}
				} else {
					rate = width2Len(0);
				}
				rate *= (((double)pApp->m_rgsLink.strength)/10.0);
				(*li).setLen(sqrt((double)(sz.cx*sz.cx + sz.cy*sz.cy))*5/4*rate);
			}
			// preBoundの値を初期化
			if (!(*li).isInChain()) continue;
			nodeFind.setKey((*li).getKeyFrom());
			niterator itFrom = nodes_.findNodeW(nodeFind);
			(*itFrom).setBoundPre((*itFrom).getBound());
			(*itFrom).dx = 0.0;
			(*itFrom).dy = 0.0;
			
			nodeFind.setKey((*li).getKeyTo());
			niterator itTo = nodes_.findNodeW(nodeFind);
			(*itTo).setBoundPre((*itTo).getBound());
			(*itTo).dx = 0.0;
			(*itTo).dy = 0.0;
		}
	}
}

void iEditDoc::relaxSingleStep(const CPoint &point, const CPoint& dragOffset)
{
	// ドラッグ中のノードの位置を変更
	niterator ni = nodes_.begin();
	for ( ; ni != nodes_.end(); ni++) {
		if ((*ni).isSelected()) {
			CRect rc = (*ni).getBound();
			int height = rc.Height();
			int width = rc.Width();
			rc.left = point.x;
			rc.top = point.y;
			rc.right = rc.left + width;
			rc.bottom = rc.top + height;
			rc.OffsetRect(dragOffset);
			
			if (rc.left < 0) {
				rc.left = 0;
				rc.right = width;
			}
			if (rc.top < 0) {
				rc.top = 0;
				rc.bottom = height;
			}
			
			(*ni).setBoundPre((*ni).getBoundPre());
			(*ni).setBound(rc);
			break;
		}
	}
	
	////// ばねモデル処理
	literator li = links_.begin();
	for ( ; li != links_.end(); li++) {
		if ((*li).getArrowStyle() == iLink::other || 
			(*li).getKeyFrom() == (*li).getKeyTo() || 
			!(*li).canDraw()) {
			continue;
		}
		if (!(*li).isInChain()) continue;
		
		nodeFind.setKey((*li).getKeyFrom());
		niterator itFrom = nodes_.findNodeW(nodeFind);
		nodeFind.setKey((*li).getKeyTo());
		niterator itTo = nodes_.findNodeW(nodeFind);
		
		double gxto = (*itTo).getBoundPre().CenterPoint().x;
		double gyto = (*itTo).getBoundPre().CenterPoint().y;
		double gxfrom = (*itFrom).getBoundPre().CenterPoint().x;
		double gyfrom = (*itFrom).getBoundPre().CenterPoint().y;
		
		double vx = gxto - gxfrom;
		double vy = gyto - gyfrom;
		double len = sqrt(vx*vx + vy*vy);
		len = (len == 0) ? .0001 : len;
		double f = ((*li).getLen() - len)/(len*3);
		double dx = f*vx;
		double dy = f*vy;
		(*itTo).dx += dx;
		(*itTo).dy += dy;
		(*itFrom).dx -= dx;
		(*itFrom).dy -= dy;
	}
	
	srand((unsigned)time(NULL));
	niterator nit1 = nodes_.begin();
	for ( ; nit1 != nodes_.end(); nit1++) {
		if (!(*nit1).isVisible()) continue;
		if (!(*nit1).isInChain()) continue;
		
		double dx = 0; double dy = 0;
		niterator nit2 = nodes_.begin();
		for (; nit2 != nodes_.end(); nit2++) {
			if (nit1 == nit2) { continue; }
			if (!(*nit2).isVisible()) continue;
			if (!(*nit2).isInChain()) continue;
			
			double gx1 = (*nit1).getBoundPre().CenterPoint().x;
			double gy1 = (*nit1).getBoundPre().CenterPoint().y;
			double gx2 = (*nit2).getBoundPre().CenterPoint().x;
			double gy2 = (*nit2).getBoundPre().CenterPoint().y;
			
			double vx = gx1 - gx2;
			double vy = gy1 - gy2;
			double len = vx*vx + vy*vy;
			if (len == 0) {
				dx += ((double)rand())/(double)RAND_MAX;
				dy += ((double)rand())/(double)RAND_MAX;
			} else /*if (len < 100*100)*/ {
				dx += vx / len;
				dy += vy / len;
			}
		}
		double dlen = dx*dx + dy*dy;
		if (dlen > 0) {
			dlen = sqrt(dlen)/2;
			(*nit1).dx += dx/dlen;
			(*nit1).dy += dy/dlen;
		}
	}
	
	niterator nit = nodes_.begin();
	for ( ; nit != nodes_.end(); nit++) {
		if (!(*nit).isVisible()) continue;
		if (!(*nit).isInChain()) continue;
		if ((*nit).isSelected()) continue;
		double x = max(-5, min(5, (*nit).dx));
		double y = max(-5, min(5, (*nit).dy));
		CRect rc = (*nit).getBoundPre();
		rc.OffsetRect((int)x, (int)y);
		// 領域のチェック
		if (rc.left < 0) {
			rc.right = (*nit).getBoundPre().Width();
			rc.left = 0;
		}
		if (rc.top < 0) {
			rc.bottom = (*nit).getBoundPre().Height();
			rc.top = 0;
		}
		if (!(*nit).isFixed() && !(*nit).isSelected()) {
			(*nit).setBound(rc);
		}
		(*nit).dx /= 2;
		(*nit).dy /= 2;
	}

	niterator it = nodes_.begin();
	for ( ; it != nodes_.end(); it++) {
		if (!(*it).isVisible()) continue;
		if (!(*it).isInChain()) continue;
		(*it).setBoundPre((*it).getBound());
	}
	setConnectPoint2();
}

/// 芋づる式に関連ノード・リンクにフラグを立てる
void iEditDoc::listupChainNodes(bool bResetLinkCurve)
{
	// 直前までのフラグをクリア
	niterator nit = nodes_.begin();
	for ( ; nit != nodes_.end(); nit++) {
		(*nit).setInChain(false);
	}
	literator linit = links_.begin();
	for ( ; linit != links_.end(); linit++) {
		(*linit).setInChain(false);
	}
	
	// links_から芋づる検索
	KeySet nodeChain; // 新しい芋用
	KeySet nodeChainChecked; // 掘った芋用
	nodeChain.insert(getSelectedNodeKey()); // selectされている芋
	unsigned int sizePre = nodeChain.size();
	for ( ; ; ) {
		KeySet::iterator ki = nodeChain.begin();
		for ( ; ki != nodeChain.end(); ki++) {
			if (nodeChainChecked.find(*ki) != nodeChainChecked.end()) {
				continue;
			}
			literator li = links_.begin();
			for ( ; li != links_.end(); li++) {
				if ((*li).isTerminalNodeKey(*ki)) {
					if (!(*li).canDraw()) continue;
					if ((*li).getArrowStyle() == iLink::other) continue;
					if ((*li).getKeyFrom() == (*li).getKeyTo()) continue;
					DWORD pairKey;
					if ((*li).getKeyFrom() == (*ki)) {
						pairKey = (*li).getKeyTo();
					} else if ((*li).getKeyTo() == (*ki)) {
						pairKey = (*li).getKeyFrom();
					}
					nodeChain.insert(pairKey);
					(*li).setInChain();
					if (bResetLinkCurve) {
						(*li).curve(false);
					}
					iNode nodeFind;
					nodeFind.setKey(pairKey);
					niterator nf = nodes_.find(nodeFind);
					if (nf != nodes_.end()) {
						(*nf).setInChain();
					}
				}
			}
		}
		if (sizePre >= nodeChain.size()) {
			break;
		}
		sizePre = nodeChain.size();
	}
}

CRect iEditDoc::getChaindNodesBound() const
{
	CRect rc = getSelectedNodeRect();
	const_niterator it = nodes_.begin();
	for ( ; it != nodes_.end(); it++) {
		if ((*it).isVisible() && (*it).isInChain()) {
			rc |= (*it).getBound();
		}
	}
	int length = rc.Width() < rc.Height() ? rc.Width() : rc.Height();
	int margin = (int)(length * 0.5);
	rc.InflateRect(margin, margin);
	return rc;
}

// 芋づるモード用
void iEditDoc::setConnectPoint2()
{
	literator li = links_.begin();
	for ( ; li != links_.end(); li++) {
		if (!(*li).isInChain()) continue;
		
		nodeFind.setKey((*li).getKeyFrom());
		niterator itFrom = nodes_.findNodeW(nodeFind);
		nodeFind.setKey((*li).getKeyTo());
		niterator itTo   = nodes_.findNodeW(nodeFind);
		if (itFrom == nodes_.end() || itTo == nodes_.end()) continue;
		
		if ((*li).getKeyFrom() == (*itFrom).getKey()) {
			(*li).setRFrom((*itFrom).getBound());
		}
		if ((*li).getKeyTo() == (*itTo).getKey()) {
			(*li).setRTo((*itTo).getBound());
		}
	}
}

void iEditDoc::recalcArea()
{
	calcMaxPt(m_maxPt);
}

template <class T>
void iEditDoc::outStyleSheetLine(T &f)
{
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	if (!pApp->m_rgsOther.bSetStylesheet) return;
	CString strStylesheetFile = pApp->m_rgsOther.strStyleSheetFile;
	if (strStylesheetFile == "") {
		strStylesheetFile = "iedit.xsl";
	}
	CString s = "<?xml-stylesheet type=\"text/xsl\" ";
	s +=  "href=\"" + strStylesheetFile + "\"" + " ?>\n";
//	f.WriteString("<?xml-stylesheet type=\"text/xsl\" href=\"iedit.xsl\" ?>\n");
	f.WriteString(s);
}

const CRect iEditDoc::addNodeWithLink(int nodeType, DWORD keyRoot, DWORD prevSibling, const CPoint & pt, bool bMindmap)
{
	iNode nodeFind;
	nodeFind.setKey(keyRoot);
	niterator it = nodes_.find(nodeFind);
	if (it == nodes_.end()) return CRect(0, 0, 0, 0);
	
	iNode nwNode;
	if (bMindmap) {
	CPoint ptTarget = (*it).getBound().CenterPoint() + CPoint(100, -100);
		nwNode = insertNode(nodeType, "ノード", ptTarget);
	} else {
		nwNode = insertNode(nodeType, "ノード", pt);
	}
	
	DWORD newKey = nwNode.getKey();
	
	selChanged(nwNode.getKey(), true, isShowSubBranch());
	SetModifiedFlag();
	
	iHint hint;
	hint.event = iHint::rectAdd;
	hint.str = nwNode.getName();
	UpdateAllViews(NULL, (LPARAM)nwNode.getKey(), &hint);
	
	iLink l;
	l.setNodes((*it).getBound(), nwNode.getBound(), (*it).getKey(), nwNode.getKey());
	l.setDrawFlag();
	l.setKey(lastLinkKey++);
	desideLinkLineStyle(l);
	desideLinkArrow(l);
	links_.push_back(l);
	
	
	if (bMindmap) {
		listupChainNodes(false);
		calcEdges();
		nodes_.fixNodesReversibly(newKey); // 新しいノード以外を固定
		for (int i = 0; i < 100; i++) {
			relaxSingleStep2();
		}
		nodes_.restoreNodesFixState(newKey); // Fix状態をリストア
	}
	
	selChanged(nwNode.getKey(), true, isShowSubBranch());
	
	nodeFind.setKey(nwNode.getKey());
	niterator nit = nodes_.find(nodeFind);
	return (*nit).getBound();
}

const CRect iEditDoc::addNodeWithLink2(int nodeType, DWORD keyPrevSibling)
{
	if (links_.isIsolated(nodes_.getSelKey(), false)) return CRect(0, 0, 0, 0);
	DWORD pairKey = links_.getFirstVisiblePair(nodes_.getSelKey()); 
	if (pairKey == -1) return CRect(0, 0, 0, 0);
	
	iNode nodeFind;
	nodeFind.setKey(pairKey);
	niterator itRoot = nodes_.find(nodeFind);
	if (itRoot == nodes_.end()) return CRect(0, 0, 0, 0);
	
	nodeFind.setKey(keyPrevSibling);
	niterator itSibling = nodes_.find(nodeFind);
	if (itSibling == nodes_.end()) return CRect(0, 0, 0, 0);
	
	CPoint ptRoot = (*itRoot).getBound().CenterPoint();
	CPoint ptSibling = (*itSibling).getBound().CenterPoint();
	CPoint ptTarget = ptSibling;
	CPoint ptOffset = ptSibling - ptRoot;
	
	// rootとsiblingの位置関係によって、初期の出現位置を変える
	if (ptOffset.x >= 0 && ptOffset.y >= 0) {
		ptTarget += CPoint(-50, 50);
	} else if (ptOffset.x < 0 && ptOffset.y >= 0) {
		ptTarget += CPoint(-50, -50);
	} else if (ptOffset.x < 0 && ptOffset.y < 0) {
		ptTarget += CPoint(50, -50);
	} else if (ptOffset.x >= 0 && ptOffset.y < 0) {
		ptTarget += CPoint(50, 50);
	}
	
	iNode nwNode = insertNode(nodeType, "ノード", ptTarget);
	DWORD newKey = nwNode.getKey();
	
	selChanged(nwNode.getKey(), true, isShowSubBranch());
	SetModifiedFlag();
	
	iHint hint;
	hint.event = iHint::rectAdd;
	hint.str = nwNode.getName();
	UpdateAllViews(NULL, (LPARAM)nwNode.getKey(), &hint);
	
	iLink l;
	l.setNodes((*itRoot).getBound(), nwNode.getBound(), (*itRoot).getKey(), nwNode.getKey());
	l.setDrawFlag();
	l.setKey(lastLinkKey++);
	desideLinkLineStyle(l);
	desideLinkArrow(l);

	links_.push_back(l);
	
	listupChainNodes(false);
	calcEdges();
	
	nodes_.fixNodesReversibly(newKey);
	for (int i = 0; i < 100; i++) {
		relaxSingleStep2();
	}
	nodes_.restoreNodesFixState(newKey);
	selChanged(nwNode.getKey(), true, isShowSubBranch());
	
	nodeFind.setKey(nwNode.getKey());
	niterator nit = nodes_.find(nodeFind);
	return (*nit).getBound();
}

// ACTION : addNodeXXを一元化するメソッド(今後リファクタリング)
// Notifyをオプショナルにするか？？
const iNode& iEditDoc::insertNode(const int nodeType, const CString &name, const CPoint &pt)
{
	iNode n(name);
	n.setKey(getUniqKey());
	n.setParent(nodes_.getCurParent());
	n.setNodeShape(nodeType);
	n.moveBound(CSize(pt.x, pt.y));
	n.setVisible();
	nodes_.insert(n);

	if (isShowSubBranch()) {
		m_visibleKeys.insert(n.getKey());
	}
	iNode nodeFind;
	nodeFind.setKey(n.getKey());
	const_niterator nit = nodes_.find(nodeFind);
	return (*nit);
}

void iEditDoc::relaxSingleStep2()
{
	////// ばねモデル処理
	literator li = links_.begin();
	for ( ; li != links_.end(); li++) {
		if ((*li).getArrowStyle() == iLink::other || 
			(*li).getKeyFrom() == (*li).getKeyTo() || 
			!(*li).canDraw()) {
			continue;
		}
		if (!(*li).isInChain()) continue;
		
		nodeFind.setKey((*li).getKeyFrom());
		niterator itFrom = nodes_.findNodeW(nodeFind);
		nodeFind.setKey((*li).getKeyTo());
		niterator itTo = nodes_.findNodeW(nodeFind);
		
		double gxto = (*itTo).getBoundPre().CenterPoint().x;
		double gyto = (*itTo).getBoundPre().CenterPoint().y;
		double gxfrom = (*itFrom).getBoundPre().CenterPoint().x;
		double gyfrom = (*itFrom).getBoundPre().CenterPoint().y;
		
		double vx = gxto - gxfrom;
		double vy = gyto - gyfrom;
		double len = sqrt(vx*vx + vy*vy);
		len = (len == 0) ? .0001 : len;
		double f = ((*li).getLen() - len)/(len*3);
		double dx = f*vx;
		double dy = f*vy;
		(*itTo).dx += dx;
		(*itTo).dy += dy;
		(*itFrom).dx -= dx;
		(*itFrom).dy -= dy;
	}
	
	srand((unsigned)time(NULL));
	niterator nit1 = nodes_.begin();
	for ( ; nit1 != nodes_.end(); nit1++) {
		if (!(*nit1).isVisible()) continue;
		if (!(*nit1).isInChain()) continue;
		
		double dx = 0; double dy = 0;
		niterator nit2 = nodes_.begin();
		for (; nit2 != nodes_.end(); nit2++) {
			if (nit1 == nit2) { continue; }
			if (!(*nit2).isVisible()) continue;
			if (!(*nit2).isInChain()) continue;
			
			double gx1 = (*nit1).getBoundPre().CenterPoint().x;
			double gy1 = (*nit1).getBoundPre().CenterPoint().y;
			double gx2 = (*nit2).getBoundPre().CenterPoint().x;
			double gy2 = (*nit2).getBoundPre().CenterPoint().y;
			
			double vx = gx1 - gx2;
			double vy = gy1 - gy2;
			double len = vx*vx + vy*vy;
			if (len == 0) {
				dx += ((double)rand())/(double)RAND_MAX;
				dy += ((double)rand())/(double)RAND_MAX;
			} else /*if (len < 100*100)*/ {
				dx += vx / len;
				dy += vy / len;
			}
		}
		double dlen = dx*dx + dy*dy;
		if (dlen > 0) {
			dlen = sqrt(dlen)/2;
			(*nit1).dx += dx/dlen;
			(*nit1).dy += dy/dlen;
		}
	}
	
	niterator nit = nodes_.begin();
	for ( ; nit != nodes_.end(); nit++) {
		if (!(*nit).isVisible()) continue;
		if (!(*nit).isInChain()) continue;
		double x = max(-5, min(5, (*nit).dx));
		double y = max(-5, min(5, (*nit).dy));
		CRect rc = (*nit).getBoundPre();
		rc.OffsetRect((int)x, (int)y);
		// 領域のチェック
		if (rc.left < 0) {
			rc.right = (*nit).getBoundPre().Width();
			rc.left = 0;
		}
		if (rc.top < 0) {
			rc.bottom = (*nit).getBoundPre().Height();
			rc.top = 0;
		}
		if (!(*nit).isFixed()) {
			(*nit).setBound(rc);
		}
		(*nit).dx /= 2;
		(*nit).dy /= 2;
	}

	niterator it = nodes_.begin();
	for ( ; it != nodes_.end(); it++) {
		if (!(*it).isVisible()) continue;
		if (!(*it).isInChain()) continue;
		(*it).setBoundPre((*it).getBound());
	}
	setConnectPoint2();
}

int iEditDoc::getKeyNodeLevelNumber(DWORD key)
{
	nodeFind.setKey(key);
	const_niterator it = nodes_.findNode(nodeFind);
	if (it != nodes_.end()) {
		return (*it).getLevel();
	}
	return -1;
}

// サイレントなHitTest
bool iEditDoc::hitTest2(const CPoint& pt)
{
	iNode* pNode = nodes_.hitTest2(pt, false);
	if (pNode != NULL) {
		return true;
	}
	return false;
}

void iEditDoc::desideLinkLineStyle(iLink& l)
{
	int lineStyle = ((CiEditApp*)AfxGetApp())->m_curLinkLineStyle;
	l.setLineStyle(PS_SOLID);
	l.setLineWidth(0);
	switch(lineStyle) {
	case CiEditApp::LS_R0:
		break;
	case CiEditApp::LS_DOT:
		l.setLineStyle(PS_DOT);
		break;
	case CiEditApp::LS_R1:
		l.setLineWidth(2);
		break;
	case CiEditApp::LS_R2:
		l.setLineWidth(3);
		break;
	case CiEditApp::LS_R3:
		l.setLineWidth(4);
		break;
	case CiEditApp::LS_R4:
		l.setLineWidth(5);
		break;
	}		
}

void iEditDoc::desideLinkArrow(iLink& l)
{
	l.setArrowStyle(getAppLinkArrow());
}

int iEditDoc::getAppLinkArrow() const
{
	int linkArrow = ((CiEditApp*)AfxGetApp())->m_curLinkArrow;
	switch(linkArrow) {
	case CiEditApp::LA_NONE:
		return iLink::line;
	case CiEditApp::LA_SINGLE:
		return iLink::arrow;
	case CiEditApp::LA_DOUBLE:
		return iLink::arrow2;
	case CiEditApp::LA_DEPEND_SINGLE:
		return iLink::depend;
	case CiEditApp::LA_DEPEND_DOUBLE:
		return iLink::depend2;
	case CiEditApp::LA_INHERIT:
		return iLink::inherit;
	case CiEditApp::LA_AGGREGAT:
		return iLink::aggregat;
	case CiEditApp::LA_COMPOSIT:
		return iLink::composit;
	}
	return iLink::line;
}

int iEditDoc::getAppLinkWidth() const
{
	int linkStyle= ((CiEditApp*)AfxGetApp())->m_curLinkLineStyle;
	switch (linkStyle) {
	case CiEditApp::LS_DOT:
		return -1; // PS_DOT
	case CiEditApp::LS_R0:
		return 0;
	case CiEditApp::LS_R1:
		return 2;
	case CiEditApp::LS_R2:
		return 3;
	case CiEditApp::LS_R3:
		return 4;
	case CiEditApp::LS_R4:
		return 5;
	}
	return 0;
}


void iEditDoc::resizeSelectedNodeFont(bool bEnLarge)
{
	backUpUndoNodes();
	backUpUndoLinks();
	nodes_.resizeSelectedNodeFont(bEnLarge);
	setConnectPoint3();
	calcMaxPt(m_maxPt);
	SetModifiedFlag();
	iHint hint;
	hint.event = iHint::nodeFontResize;
	UpdateAllViews(NULL, (LPARAM)(nodes_.getSelKey()), &hint);
}

void iEditDoc::resizeSelectedLinkFont(bool bEnLarge)
{
	backUpUndoNodes();
	backUpUndoLinks();
	links_.resizeSelectedLinkFont(bEnLarge);
	setConnectPoint3();
	calcMaxPt(m_maxPt);
	SetModifiedFlag();
	iHint hint;
	hint.event = iHint::linkModified;
	UpdateAllViews(NULL, (LPARAM)(nodes_.getSelKey()), &hint);
}

// スタイル変更の時に呼び出しているが・・？
void iEditDoc::setConnectPoint3()
{
	literator li = links_.begin();
	for ( ; li != links_.end(); li++) {
		if (!(*li).canDraw()) continue;
		nodeFind.setKey((*li).getKeyFrom());
		niterator itFrom = nodes_.findNodeW(nodeFind);
		nodeFind.setKey((*li).getKeyTo());
		niterator itTo   = nodes_.findNodeW(nodeFind);
		if (itFrom == nodes_.end() || itTo == nodes_.end()) continue;
		
		if ((*li).getKeyFrom() == (*itFrom).getKey()) {
			(*li).setRFrom((*itFrom).getBound());
		}
		if ((*li).getKeyTo() == (*itTo).getKey()) {
			(*li).setRTo((*itTo).getBound());
		}
	}
}

OutlineView* iEditDoc::getOutlineView() const
{
	POSITION pos = GetFirstViewPosition();
	OutlineView* pView = (OutlineView*)GetNextView(pos);
	
	ASSERT(pView->IsKindOf(RUNTIME_CLASS(OutlineView)));
	return pView;
}

void iEditDoc::setDrawOrderInfo(bool bSetDrawOrderInfo)
{
	nodes_.m_bDrawOrderInfo = !nodes_.m_bDrawOrderInfo;
}

BOOL iEditDoc::isDrawOrderInfo() const
{
	return nodes_.m_bDrawOrderInfo;
}

void iEditDoc::migrateGroup()
{
	niterator it = nodes_.getSelectedNode();
	CRect r = (*it).getBound();
	niterator itr = nodes_.begin();
	int drawOrder = 0;
	DWORD key = -1;
	for ( ; itr != nodes_.end(); itr++) {
		if (!(*itr).isVisible()) continue;
		CRect bound = (*itr).getBound();
		BOOL bInBound = bound.PtInRect(r.TopLeft()) &&
			            bound.PtInRect(r.BottomRight());
		if (bInBound) {
			if (drawOrder < (*itr).getDrawOrder()) {
				key = (*itr).getKey();
				drawOrder = (*itr).getDrawOrder();
			}
		}
	}
	iHint hint; hint.event = iHint::groupMigrate;
	hint.keyTarget = key;
	UpdateAllViews(NULL, (LPARAM)(*it).getKey(), &hint);
}

void iEditDoc::saveSelectedNodeFormat()
{
	niterator n = nodes_.getSelectedNode();
	m_nodeForFormat = iNode((*n));
}

void iEditDoc::applyFormatToSelectedNode()
{
	backUpUndoNodes();
	niterator n = nodes_.getSelectedNode();
	(*n).setLineColor(m_nodeForFormat.getLineColor());
	(*n).setLineColor(m_nodeForFormat.getLineColor());
	(*n).setLineStyle(m_nodeForFormat.getLineStyle());
	(*n).setBrush(m_nodeForFormat.getBrsColor());
	(*n).setNoBrush(m_nodeForFormat.isFilled());
	(*n).setTextStyle(m_nodeForFormat.getTextStyle());
	(*n).setFontColor(m_nodeForFormat.getFontColor());
	(*n).setFontInfo(m_nodeForFormat.getFontInfo());
	if (m_nodeForFormat.getNodeShape() == iNode::MetaFile) {
		HENHMETAFILE hMetaFile = m_nodeForFormat.getMetaFile();
		(*n).setMetaFile(hMetaFile);
	}
	(*n).setNodeShape(m_nodeForFormat.getNodeShape());
	(*n).setMarginL(m_nodeForFormat.getMarginL());
	(*n).setMarginR(m_nodeForFormat.getMarginR());
	(*n).setMarginT(m_nodeForFormat.getMarginT());
	(*n).setMarginB(m_nodeForFormat.getMarginB());
	setConnectPoint();
	calcMaxPt(m_maxPt);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &hint);
}

void iEditDoc::saveSelectedLinkFormat()
{
	const_literator l = links_.getSelectedLink(false);
	m_linkForFormat.setArrowStyle((*l).getArrowStyle());
	m_linkForFormat.setLineStyle((*l).getLineStyle());
	m_linkForFormat.setLineWidth((*l).getLineWidth());
	m_linkForFormat.setLinkColor((*l).getLinkColor());
}

void iEditDoc::applyFormatToSelectedLink()
{
	backUpUndoLinks();
	literator l = links_.getSelectedLinkW(false);
	(*l).setArrowStyle(m_linkForFormat.getArrowStyle());
	(*l).setLineStyle(m_linkForFormat.getLineStyle());
	(*l).setLineWidth(m_linkForFormat.getLineWidth());
	(*l).setLinkColor(m_linkForFormat.getLinkColor());
}

void iEditDoc::deleteLinksInBound(const CRect& bound)
{
	links_.erase(remove_if(links_.begin(), links_.end(), 
		iLink_inBound(bound)), links_.end());
	SetModifiedFlag();
	iHint h; h.event = iHint::linkDeleteMulti;
	UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
}

DWORD iEditDoc::duplicateKeyNode(DWORD key)
{
	nodeFind.setKey(key);
	const_niterator it = nodes_.findNode(nodeFind);
	iNode n = *it;
	n.setKey(getUniqKey());
	nodes_.insert(n);
	return n.getKey();
}

void iEditDoc::duplicateLinks(const IdMap& idm)
{
	IdMap::const_iterator it = idm.begin();
	for ( ; it != idm.end(); it++) {
		literator li = links_.begin();
		for ( ; li != links_.end(); li++) {
			if ((*it).first == (*li).getKeyFrom()) {
				IdMap::const_iterator pr = idm.find((*li).getKeyTo());
				if(pr != idm.end()) {
					iLink l = (*li);
					l.setKeyFrom((*it).second);
					l.setKeyTo((*pr).second);
					l.setKey(lastLinkKey++);
					links_.push_back(l);
				}
			}
		}
	}
}

int iEditDoc::getSerialVersion() const
{
	return m_serialVersion;
}

void iEditDoc::getSelectedNodeMargin(int& l, int & r, int& t, int& b) const
{
	const_niterator nit = nodes_.getSelectedNodeR();
	if (nit != nodes_.end()) {
		l = (*nit).getMarginL();
		r = (*nit).getMarginR();
		t = (*nit).getMarginT();
		b = (*nit).getMarginB();
	}
}

void iEditDoc::swapLinkOrder(DWORD key1, DWORD key2)
{
	const_literator li1 = links_.findByKey(key1);
	const_literator li2 = links_.findByKey(key2);
	if (li1 != links_.end() && li2 != links_.end()) {
		iLink link(*li2);
		links_.insert(li1, link);
		links_.erase(li2);
		SetModifiedFlag();
	}
}

void iEditDoc::setSelectedNodeMargin(int l, int r, int t, int b)
{
	backUpUndoNodes();
	nodes_.setSelectedNodeMargin(l, r, t, b);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.getSelKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

CString iEditDoc::getTitleFromPath() const
{
	CString fullPath = GetPathName();
	CString fileName;
	
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath_s(fullPath, drive, dir, fname, ext );
	return CString(fname);
}

bool iEditDoc::saveCurrentImage(const CString& pngPath)
{
	CPoint p1(0, 0);
	CPoint p2 = getMaxPt();
	p2 += CSize(10, 10);
	CRect rc(p1, p2);	
	CImage image;
	image.Create(rc.Width(), rc.Height(), 32);
	
	CDC* pDC = CDC::FromHandle(image.GetDC());
	CBrush brush(RGB(255, 255, 255)); 
	pDC->FillRect(rc, &brush);
	
	drawNodes(pDC, false);
	drawLinks(pDC, false, true);
	image.ReleaseDC();
	image.Save(TEXT(pngPath), Gdiplus::ImageFormatPNG);
	return true;
}

bool iEditDoc::writeClickableMap(CStdioFile& f, const CString& textFileName, bool singleText)
{
	f.WriteString(nodes_.createClickableMapString(textFileName, singleText));
	return true;
}

void iEditDoc::fitSetlectedNodeSize()
{
	backUpUndoNodes();
	backUpUndoLinks();
	niterator it = nodes_.begin();
	for ( ; it != nodes_.end(); it++) {
		if ((*it).isSelected()) {
			(*it).fitSize();
		}
	}
	SetModifiedFlag();
	setConnectPoint();
	calcMaxPt(m_maxPt);
	iHint hint; hint.event = iHint::reflesh;
	DWORD key = nodes_.getSelKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

