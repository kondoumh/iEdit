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
#include "StringUtil.h"
#include <atlimage.h>
#include <regex>
#include <locale>
#include "StringUtil.h"

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

/// iLink のキーが同一かを判定
class iLink_eqkey : public unary_function<iLink, bool>
{
	DWORD key_;  // 削除されるnodeのキー
public:
	explicit iLink_eqkey(DWORD key) : key_(key) { }
	bool operator()(const iLink& l) const {
		return (l.getKey() == key_);
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
	m_bShowBranch = false;
	m_initialBranchMode = 0;
	m_bOldBinary = false;
	m_serialVersion = 0;
	m_bLoadXmlSucceeded = false;
}

iEditDoc::~iEditDoc()
{
}

BOOL iEditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

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
		if (!m_bSerializeXML) {
			if (m_bOldBinary) {
				saveOrderByTree(ar); // ノードの保存
				// リンクの保存
				ar << links_.size();
				literator li = links_.begin();
				for (; li != links_.end(); li++) {
					(*li).Serialize(ar);
				}
			}
			else {
				const int SERIAL_VERSION = 5; // シリアル化バージョン番号
				ar << SERIAL_VERSION;
				saveOrderByTreeEx(ar, SERIAL_VERSION); // ノードの保存
				// リンクの保存
				ar << links_.size();
				literator li = links_.begin();
				for (; li != links_.end(); li++) {
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
			loadFromXML(ar.GetFile()->GetFilePath());
		}
		else {
			if (m_bOldBinary) {
				// ノードの読み込み
				ar >> lastKey;
				unsigned int count;
				ar >> count;
				for (unsigned int i = 0; i < count; i++) {
					iNode n;
					n.Serialize(ar);
					nodes_[n.getKey()] = n;
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
			}
			else {
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
					nodes_[n.getKey()] = n;
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

void iEditDoc::saveOrderByTree(CArchive& ar)
{
	OutlineView* pView = getOutlineView();
	NodePropsVec ls;
	pView->treeToSequence0(ls);  // シリアライズ専用シーケンス取得
	ar << lastKey;
	ar << ls.size();
	for (unsigned int i = 0; i < ls.size(); i++) {
		niterator it = nodes_.findNodeW(ls[i].key);
		if (it != nodes_.end()) {
			(*it).second.setTreeState(ls[i].state);
			(*it).second.Serialize(ar);
		}
	}
}

void iEditDoc::saveOrderByTreeEx(CArchive &ar, int version)
{
	OutlineView* pView = getOutlineView();
	NodePropsVec ls;
	pView->treeToSequence0(ls);  // シリアライズ専用シーケンス取得
	ar << lastKey;
	ar << ls.size();
	for (unsigned int i = 0; i < ls.size(); i++) {
		niterator it = nodes_.findNodeW(ls[i].key);
		if (it != nodes_.end()) {
			(*it).second.setTreeState(ls[i].state);
			(*it).second.SerializeEx(ar, version);
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

DWORD iEditDoc::GetBranchRootKey() const
{
	return m_dwBranchRootKey;
}

int iEditDoc::GetInitialBranchMode() const
{
	return m_initialBranchMode;
}

void iEditDoc::copyNodeLabels(NodePropsVec &v)
{
	for (unsigned int i = 0; i < sv.size(); i++) {
		const_niterator it = nodes_.findNode(sv[i]);
		NodeProps l;
		l.name = (*it).second.getName();
		l.key = (*it).second.getKey();
		l.parent = (*it).second.getParent();
		l.state = (*it).second.getTreeState();
		l.level = (*it).second.getLevel();
		l.treeIconId = (*it).second.getTreeIconId();
		v.push_back(l);
	}
	sv.clear();
	sv.resize(0);
}

// OutlineViewでのノード追加用メソッド
void iEditDoc::addNode(const NodeProps &l, DWORD inheritKey, bool bInherit)
{
	const_niterator it = nodes_.findNode(inheritKey);
	iNode n(l.name);
	n.setKey(l.key);
	n.setParent(l.parent);

	if (ShowSubBranch()) {
		m_visibleKeys.insert(l.key);
	}

	if (bInherit && it != nodes_.end()) {
		n.setLineColor((*it).second.getLineColor());
		n.setLineWidth((*it).second.getLineWidth());
		n.setLineStyle((*it).second.getLineStyle());
		n.setBrush((*it).second.getBrsColor());
		n.setNoBrush((*it).second.isFilled());
		n.setNodeShape((*it).second.getNodeShape());
		n.setTextStyle((*it).second.getTextStyle());
		n.setFontColor((*it).second.getFontColor());
		n.setFontInfo((*it).second.getFontInfo());
	}

	if (it != nodes_.end()) {
		CRect rc = (*it).second.getBound();
		n.moveBound(CSize(rc.left + 20, rc.top + 30));
	}

	nodes_[n.getKey()] = n;
	selChanged(l.key, true, ShowSubBranch());
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeAdd;
	UpdateAllViews(NULL, (LPARAM)l.key, &hint);
}

// OutlineViewのインポートテキストファイル専用に作ってる
void iEditDoc::addNode2(const iNode &n)
{
	nodes_[n.getKey()] = n;
	sv.push_back(n.getKey());
	SetModifiedFlag();
}

DWORD iEditDoc::getUniqKey()
{
	return ++lastKey;
}

bool iEditDoc::setKeyNodeName(DWORD key, const CString &name)
{
	niterator it = nodes_.findNodeW(key);
	if (it == nodes_.end()) {
		return false;
	}
	(*it).second.setName(name);
	SetModifiedFlag();
	UpdateAllViews(NULL);
	return true;
}

void iEditDoc::setKeyNodeParent(DWORD key, DWORD parent)
{
	niterator it = nodes_.findNodeW(key);
	if (it != nodes_.end()) {
		/*const_cast<iNode&>*/(*it).second.setParent(parent);
	}
	SetModifiedFlag();
	UpdateAllViews(NULL);
}

CString iEditDoc::getKeyNodeChapterNumber(DWORD key) {
	const_niterator it = nodes_.findNode(key);
	if (it != nodes_.end()) {
		return (*it).second.getChapterNumber();
	}
	return _T("");
}

void iEditDoc::setKeyNodeChapterNumber(DWORD key, const CString& chapterNumber) {
	niterator it = nodes_.findNodeW(key);
	if (it != nodes_.end()) {
		(*it).second.setChapterNumber(chapterNumber);
	}
	// transient 属性なので、DirtyFlag 立てたりビュー再描画したりは不要。
}

void iEditDoc::deleteKeyItem(DWORD key)
{
	DWORD parent = nodes_.getCurParent();
	// 以下の行は、メタファイルの再描画不具合のため廃止iNode_eqの中でiNodeの参照を使うのが原因？
//	nodes_.erase(remove_if(nodes_.begin(), nodes_.end(), iNode_eq(key)), nodes_.end());

	niterator it = nodes_.findNodeW(key);
	if (it != nodes_.end()) {
		nodes_.erase(it);
	}
	links_.erase(remove_if(links_.begin(), links_.end(), iLink_eq(key)), links_.end());
	if (m_bShowBranch) {
		nodes_.setVisibleNodes(m_visibleKeys);
	}
	else {
		nodes_.setVisibleNodes(this->nodes_.getSelKey());
	}
	SetModifiedFlag();
	iHint h; h.event = iHint::nodeDeleteByKey;
	UpdateAllViews(NULL, key, &h);
}

BOOL iEditDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	m_openFilePath = lpszPathName;
	WCHAR drive[_MAX_DRIVE];
	WCHAR dir[_MAX_DIR];
	WCHAR fileName[_MAX_FNAME];
	WCHAR ext[_MAX_EXT];
	ZeroMemory(drive, _MAX_DRIVE);
	ZeroMemory(dir, _MAX_DIR);
	ZeroMemory(fileName, _MAX_FNAME);
	ZeroMemory(ext, _MAX_EXT);
	_wsplitpath_s((const wchar_t *)m_openFilePath, drive, _MAX_DRIVE, dir, _MAX_DIR, fileName, _MAX_FNAME, ext, _MAX_EXT);
	CString extent = ext;
	extent.MakeLower();
	if (extent != _T(".iedx") && extent != _T(".ied") && extent != _T(".xml")) {
		AfxMessageBox(_T("iEditファイルではありません"));
		return FALSE;
	}
	m_bSerializeXML = false;
	if (extent == _T(".iedx")) {
		m_bSerializeXML = false;
		m_bOldBinary = false;
	}
	else if (extent == _T(".ied")) {
		m_bSerializeXML = false;
		m_bOldBinary = true;
	}
	else if (extent == _T(".xml")) {
		m_bSerializeXML = true;
		m_bLoadXmlSucceeded = false;
		m_bOldBinary = false;
	}

	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	if (m_bSerializeXML && !m_bLoadXmlSucceeded) {
		return FALSE;
	}
	InitDocument();
	return TRUE;
}

BOOL iEditDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	WCHAR drive[_MAX_DRIVE];
	WCHAR dir[_MAX_DIR];
	WCHAR fileName[_MAX_FNAME];
	WCHAR ext[_MAX_EXT];
	ZeroMemory(drive, _MAX_DRIVE);
	ZeroMemory(dir, _MAX_DIR);
	ZeroMemory(fileName, _MAX_FNAME);
	ZeroMemory(ext, _MAX_EXT);
	_wsplitpath_s((const wchar_t *)lpszPathName, drive, _MAX_DRIVE, dir, _MAX_DIR, fileName, _MAX_FNAME, ext, _MAX_EXT);
	CString extent = ext;
	if (extent != _T(".iedx") && extent != _T(".ied") && extent != _T(".xml")) {
		extent = _T(".iedx");
	}
	if (extent == _T(".iedx")) {
		m_bSerializeXML = false;
		m_bOldBinary = false;
	}
	else if (extent == _T(".ied")) {
		m_bSerializeXML = false;
		m_bOldBinary = true;
	}
	else if (extent == _T(".xml")) {
		m_bSerializeXML = true;
		m_bOldBinary = false;
		SetModifiedFlag(FALSE);
		SaveXml(lpszPathName, true);
		return TRUE;
	}
	return CDocument::OnSaveDocument(lpszPathName);
}


void iEditDoc::InitDocument()
{
	if (nodes_.size() == 0) {
		lastKey = 0;
		iNode i(_T("主題")); i.setKey(0); i.setParent(0);
		i.moveBound(CSize(10, 10));
		CString title = GetFileNameFromOpenPath();
		if (GetFileNameFromOpenPath() != _T("")) {
			i.setName(title);
		}
		nodes_[i.getKey()] = i;
		sv.push_back(i.getKey());
	}

	nodes_.initSelection();
	curParent = nodes_.getCurParent();
	nodes_.setVisibleNodes(nodes_.getSelKey());
	calcMaxPt(m_maxPt);
	canCpyLink = FALSE;
}

CString iEditDoc::GetFileNameFromOpenPath()
{
	WCHAR drive[_MAX_DRIVE];
	WCHAR dir[_MAX_DIR];
	WCHAR fileName[_MAX_FNAME];
	WCHAR ext[_MAX_EXT];
	ZeroMemory(drive, _MAX_DRIVE);
	ZeroMemory(dir, _MAX_DIR);
	ZeroMemory(fileName, _MAX_FNAME);
	ZeroMemory(ext, _MAX_EXT);
	_wsplitpath_s((const wchar_t *)m_openFilePath, drive, _MAX_DRIVE, dir, _MAX_DIR, fileName, _MAX_FNAME, ext, _MAX_EXT);
	CString extent = ext;
	extent.MakeLower();
	if (extent == _T(".iedx") || extent == _T(".ied") || extent == _T(".xml")) {
		return fileName;
	}
	return _T("");
}

CString iEditDoc::getSelectedNodeText()
{
	niterator it = nodes_.getSelectedNode();
	if (it != nodes_.end()) {
		return (*it).second.getText();
	}
	return _T("");
}

void iEditDoc::selChanged(DWORD key, bool reflesh, bool bShowSubBranch)
{
	DWORD parentOld = nodes_.getCurParent();
	nodes_.setSelKey(key);

	NodeKeyVec svec = getOutlineView()->getDrawOrder(bShowSubBranch);
	if (((CiEditApp*)AfxGetApp())->m_rgsNode.orderDirection == 1) {
		// 降順での描画オプションの場合は反転する
		std::reverse(svec.begin(), svec.end());
	}
	nodes_.setDrawOrder(svec);

	if (!bShowSubBranch) {
		nodes_.setVisibleNodes(key);
	}
	else {
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
	}
	else {
		iHint h; h.event = iHint::nodeSel;
		UpdateAllViews(NULL, (LPARAM)key, &h);
	}
}

void iEditDoc::setCurNodeText(CString &s, int scrollPos)
{
	niterator it = nodes_.getSelectedNode();
	if (it != nodes_.end()) {
		(*it).second.setText(s);
		(*it).second.setScrollPos(scrollPos);
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
	SetConnectionPoint();
	calcMaxPt(m_maxPt);
	SetModifiedFlag();
}

void iEditDoc::MoveNodesInBound(const CRect& bound, const CSize move)
{
	niterator itSelected = nodes_.getSelectedNode();
	if (itSelected == nodes_.end()) return;
	niterator it = nodes_.begin();
	bool moved = false;
	NodeKeySet keySet;
	for (; it != nodes_.end(); it++) {
		if (!(*it).second.isVisible()) continue;
		BOOL bInBound = bound.PtInRect((*it).second.getBound().TopLeft()) &&
			bound.PtInRect((*it).second.getBound().BottomRight());
		if (bInBound) {
			if ((*it).second.getDrawOrder() > (*itSelected).second.getDrawOrder()) {
				(*it).second.moveBound(move);
				keySet.insert((*it).second.getKey());
				moved = true;
			}
		}
	}

	if (moved) {
		literator li = links_.begin();
		for (; li != links_.end(); li++) {
			if (keySet.find((*li).getKeyFrom()) != keySet.end() &&
				keySet.find((*li).getKeyTo()) != keySet.end()) {
				(*li).movePts(move);
			}
		}
		SetConnectionPoint();

		iHint hint; hint.event = iHint::groupMoved;
		UpdateAllViews(NULL, nodes_.getSelKey(), &hint);
	}
}

void iEditDoc::moveSelectedLink(const CSize &sz)
{
	literator li = links_.begin();
	for (; li != links_.end(); li++) {
		niterator itFrom = nodes_.findNodeW((*li).getKeyFrom());
		niterator itTo = nodes_.findNodeW((*li).getKeyTo());
		if (itFrom == nodes_.end() || itTo == nodes_.end()) continue;
		if ((*itFrom).second.isSelected() && (*itTo).second.isSelected()) {
			(*li).movePts(sz);
			SetModifiedFlag();
			calcMaxPt(m_maxPt);
		}
	}
}


void iEditDoc::setSelectedNodeBound(const CRect &r, bool withLink, bool noBackup)
{
	if (!noBackup) {
		BackupNodesForUndo();
	}
	nodes_.setSelectedNodeBound(r);
	if (withLink) {
		SetConnectionPoint();
	}
	calcMaxPt(m_maxPt);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.getSelKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::SetConnectionPoint()
{
	literator li = links_.begin();
	for (; li != links_.end(); li++) {
		niterator itFrom = nodes_.findNodeW((*li).getKeyFrom());
		niterator itTo = nodes_.findNodeW((*li).getKeyTo());
		if (itFrom == nodes_.end() || itTo == nodes_.end()) continue;
		if ((*itFrom).second.isSelected() && (*itTo).second.isSelected()) {
			if (itFrom != itTo) continue;
		}

		if ((*li).getKeyFrom() == (*itFrom).second.getKey()) {
			(*li).setRFrom((*itFrom).second.getBound());
		}
		if ((*li).getKeyTo() == (*itTo).second.getKey()) {
			(*li).setRTo((*itTo).second.getBound());
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
	NodeKeySet ks;
	pt = CPoint(0, 0);
	const_niterator it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if (!(*it).second.isVisible()/* && !m_bShowAll*/) {
			continue;
		} // この位置でフィルタをかけるべきだよね。これが、選択領域のバグの原因？
		ks.insert((*it).second.getKey());

		if ((*it).second.getBound().BottomRight().x > pt.x) {
			pt.x = (*it).second.getBound().BottomRight().x;
		}
		if ((*it).second.getBound().BottomRight().y > pt.y) {
			pt.y = (*it).second.getBound().BottomRight().y;
		}
	}
	literator li = links_.begin();
	for (; li != links_.end(); li++) {
		if (ks.find((*li).getKeyFrom()) != ks.end() && ks.find((*li).getKeyTo()) != ks.end() && (*li).getArrowStyle() != iLink::other) {
			(*li).setDrawFlag();
			if ((*li).getBound().BottomRight().x > pt.x) {
				pt.x = (*li).getBound().BottomRight().x;
			}
			if ((*li).getBound().BottomRight().y > pt.y) {
				pt.y = (*li).getBound().BottomRight().y;
			}
		}
		else {
			(*li).setDrawFlag(false);
		}
	}
}

void iEditDoc::drawLinks(CDC *pDC, bool bDrwAll, bool clipbrd)
{
	links_.drawLines(pDC);
	links_.drawArrows(pDC);
	links_.drawComments(pDC, clipbrd);
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
		}
		else {
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

bool iEditDoc::SwitchLinkStartNodeAt(const CPoint &pt, bool bDrwAll)
{
	iNode* pNode = nodes_.hitTest(pt, bDrwAll); // リンク元を再選択
	if (pNode != NULL) {
		BackupLinksForUndo();
		links_.setSelectedNodeLinkFrom(pNode->getKey(), pNode->getBound());
		SetModifiedFlag();
		iHint h; h.event = iHint::linkModified;
		UpdateAllViews(NULL, (LPARAM)getSelectedNodeKey(), &h);
		return true;
	}
	return false;
}

bool iEditDoc::SwitchLinkEndNodeAt(const CPoint &pt, bool bDrwAll)
{
	iNode* pNode = nodes_.hitTest2(pt, bDrwAll); // 再選択なし
	if (pNode != NULL) {
		BackupLinksForUndo();
		links_.setSelectedNodeLinkTo(pNode->getKey(), pNode->getBound());
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
	SetConnectionPoint();
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
	}
	else {
		style = iNode::s_cc;
	}
	BackupNodesForUndo();
	BackupLinksForUndo();
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
		rc = (*it).second.getBound();
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
	for (; it != nodes_.end(); it++) {
		if (!(*it).second.isVisible()) continue;
		if ((*it).second.getKey() > pastkey && (*it).second.getKey() < curkey) {
			pastkey = (*it).second.getKey();
		}
	}
	it = nodes_.findNode(pastkey);
	if (it != nodes_.end()) {
		rc = (*it).second.getBound();
	}
	return rc;
}

// クリップボートからのノード一括作成のためにこのメソッドだけシグネチャを変えました。
void iEditDoc::addNodeRect(const CString &name, const CPoint &pt, bool bSetMultiLineProcess, bool bNoBound)
{
	AddNodeInternal(name, pt, iNode::rectangle, bSetMultiLineProcess, bNoBound);
}

void iEditDoc::addNodeArc(const CString &name, const CPoint &pt)
{
	AddNodeInternal(name, pt, iNode::arc, true);
}

void iEditDoc::AddNodeRoundedRect(const CString &name, const CPoint &pt)
{
	AddNodeInternal(name, pt, iNode::roundRect, true);
}

void iEditDoc::AddNodeInternal(const CString &name, const CPoint &pt, int nodeType, bool bEnableMultiLineProcess, bool bNoBound)
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
	nodes_[n.getKey()] = n;

	if (ShowSubBranch()) {
		m_visibleKeys.insert(n.getKey());
	}
	calcMaxPt(m_maxPt);
	selChanged(n.getKey(), true, ShowSubBranch());
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

void iEditDoc::AddShapeNode(const CString &name, const CPoint &pt, int mfIndex, HENHMETAFILE& mh)
{
	iNode n(name);
	n.setKey(getUniqKey());
	n.setParent(nodes_.getCurParent());
	n.setNodeShape(iNode::MetaFile);
	n.moveBound(CSize(pt.x, pt.y));
	n.setMetaFile(mh);
	n.setTextStyle(iNode::notext);
	nodes_[n.getKey()] = n;

	if (ShowSubBranch()) {
		m_visibleKeys.insert(n.getKey());
	}

	selChanged(n.getKey(), true, ShowSubBranch());
	SetModifiedFlag();
	iHint hint;
	hint.event = iHint::rectAdd;
	hint.str = name;
	UpdateAllViews(NULL, (LPARAM)n.getKey(), &hint);
}

bool iEditDoc::hitTestLinks(const CPoint &pt, bool drwAll)
{
	DWORD key; CString path;
	bool hit = links_.hitTest(pt, key, path);
	if (hit) {
		iHint h; h.event = iHint::linkSel; h.str = path;
		UpdateAllViews(NULL, (LPARAM)key, &h);
	}
	return hit;
}

DWORD iEditDoc::hitTestDropTarget(const CPoint& pt, const DWORD selectedNodeKey)
{
	return links_.hitTestDropTarget(pt, selectedNodeKey);
}

bool iEditDoc::SelectLinkStartIfHit(const CPoint &pt, bool drwAll)
{
	DWORD key; CString path;
	bool hit = links_.hitTestFrom(pt, key, path);
	if (hit) {
		iHint h; h.event = iHint::linkSel; h.str = path;
		UpdateAllViews(NULL, (LPARAM)key, &h);
	}
	return hit;
}

bool iEditDoc::SelectLinkEndIfHit(const CPoint &pt, bool drwAll)
{
	DWORD key; CString path;
	bool hit = links_.hitTestTo(pt, key, path);
	if (hit) {
		iHint h; h.event = iHint::linkSel; h.str = path;
		UpdateAllViews(NULL, (LPARAM)key, &h);
	}
	return hit;
}

void iEditDoc::drawLinkSelection(CDC *pDC, bool bDrwAll)
{
	links_.drawSelection(pDC);
}

void iEditDoc::DrawLinkSelectionFrom(CDC *pDC, bool bDrwAll)
{
	links_.drawSelectionFrom(pDC);
}

void iEditDoc::DrawLinkSelectionTo(CDC *pDC, bool bDrwAll)
{
	links_.drawSelectionTo(pDC);
}

void iEditDoc::setNewLinkInfo(DWORD keyFrom, DWORD keyTo, const CString &comment, int styleArrow)
{
	niterator itFrom = nodes_.findNodeW(keyFrom);
	niterator itTo = nodes_.findNodeW(keyTo);
	if (itFrom == nodes_.end() || itTo == nodes_.end()) return;
	iLink l;
	l.setArrowStyle(styleArrow);
	l.setName(comment);
	l.setNodes((*itFrom).second.getBound(), (*itTo).second.getBound(), keyFrom, keyTo);
	if ((*itFrom).second.isVisible() && (*itTo).second.isVisible()) {
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
	const_literator li = links_.getSelectedLink();
	if (li != links_.end()) {
		sComment = (*li).getName();
		arrowType = (*li).getArrowStyle();
		const_niterator itFrom = nodes_.findNode((*li).getKeyFrom());
		const_niterator itTo = nodes_.findNode((*li).getKeyTo());
		if (itFrom != nodes_.end()) sFrom = (*itFrom).second.getName();
		if (itTo != nodes_.end()) sTo = (*itTo).second.getName();
	}
}

void iEditDoc::setSelectedLinkInfo(const CString &sComment, int arrowType, bool bDrwAll)
{
	literator li = links_.getSelectedLinkW();
	if (li != links_.end()) {
		BackupLinksForUndo();
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
	links_.selectLinksInBound(r);
}

int iEditDoc::getSelectedLinkWidth(bool drwAll) const
{
	// links や nodesはdocのprivateメンバなのだから要素に関する演算は
	// docでやってもかまわないとこの関数を書いていて気づいた。
	// 方針変更しようと思ったが、inlineで同じメソッド名を使用している
	// とリンカがうまくいかないのでやはり...
	return links_.getSelectedLinkWidth();
}

void iEditDoc::setSelectedLinkWidth(int w, bool drwAll)
{
	links_.setSelectedLinkWidth(w);
	SetModifiedFlag();
	iHint h; h.event = iHint::linkModified;
	UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
}

void iEditDoc::setSelectedLinkLineStyle(int style, bool drwAll)
{
	links_.setSelectedLinkLineStyle(style);
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
	return links_.getSelectedLinkLineColor();
}

void iEditDoc::setSelectedLinkLineColor(const COLORREF &c, bool drwAll)
{
	links_.setSelectedLinkLineColor(c);
	SetModifiedFlag();
	iHint h; h.event = iHint::linkModified;
	UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
}

void iEditDoc::setSelectedLinkFont(const LOGFONT &lf, bool drwAll)
{
	links_.setSelectedLinkFont(lf);
	SetModifiedFlag();
	iHint h; h.event = iHint::linkModified;
	UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
}

void iEditDoc::getSelectedLinkFont(LOGFONT &lf, bool drwAll)
{
	links_.getSelectedLinkFont(lf);
}

BOOL iEditDoc::RouteCmdToAllViews(CView *pView, UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo)
{
	POSITION pos = GetFirstViewPosition();
	while (pos != NULL) {
		CView* pNextView = GetNextView(pos);
		if (((CCmdTarget*)pNextView)->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) {
			return TRUE;
		}
	}
	return FALSE;
}

void iEditDoc::deleteSelectedLink()
{
	literator it = links_.getSelectedLinkW();
	if (it != links_.end()) {
		m_deleteBound = (*it).getBound();
		links_.erase(it);
		SetModifiedFlag();
		iHint h; h.event = iHint::linkDelete;
		UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
	}
}

void iEditDoc::deleteSelectedLink2()
{
	literator it = links_.getSelectedLinkW2();
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
	if (ShowSubBranch()) {
		parentKey = m_dwBranchRootKey;
	}
	NodeKeyVec v = nodes_.getSelectedNodeKeys();
	vector<DWORD>::iterator it = v.begin();
	for (; it != v.end(); it++) {
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
		return (*it).second.getName();
	}
	return _T("");
}

CString iEditDoc::getSelectedLinkLabel(bool drawAll)
{
	CString label(_T(""));
	const_literator it = links_.getSelectedLink();
	if (it != links_.end()) {
		label = (*it).getName();
	}
	return label;
}


bool iEditDoc::canDeleteNode() const
{
	return nodes_.getSelKey() != 0;
}


void iEditDoc::getLinkInfoList(LinkPropsVec &ls, bool drwAll)
{
	DWORD curKey = nodes_.getSelKey();
	literator it = links_.begin();
	for (; it != links_.end(); it++) {
		if ((*it).getKeyFrom() != curKey && (*it).getKeyTo() != curKey) continue;
		LinkProps i;
		i.comment = (*it).getName();
		if ((*it).getKeyFrom() == curKey) {
			i.keyTo = (*it).getKeyTo();
		}
		else if ((*it).getKeyTo() == curKey) {
			i.keyTo = (*it).getKeyFrom();
		}

		i.path = (*it).getPath();

		DWORD searchKey = 0;
		if ((*it).getKeyFrom() == curKey) {
			searchKey = (*it).getKeyTo();
		}
		else if ((*it).getKeyTo() == curKey) {
			searchKey = (*it).getKeyFrom();
		}

		const_niterator ni = nodes_.findNode(searchKey);
		i.sTo = (*ni).second.getName();

		if ((*it).getArrowStyle() != iLink::other) {
			if ((*it).canDraw()) {
				if ((*it).getKeyFrom() == curKey) {
					i.linkType = LinkProps::linkSL;
				}
				else if ((*it).getKeyTo() == curKey) {
					i.linkType = LinkProps::linkSL2;
				}
			}
			else {
				if ((*it).getKeyFrom() == curKey) {
					i.linkType = LinkProps::linkDL;
				}
				else if ((*it).getKeyTo() == curKey) {
					i.linkType = LinkProps::linkDL2;
				}
			}
		}
		else {
			CString url = (*it).getPath();
			if (url.Find(_T("http://")) != -1 || url.Find(_T("https://")) != -1 || url.Find(_T("ftp://")) != -1) {
				i.linkType = LinkProps::WebURL;
			}
			else {
				if (PathIsDirectory(url)) {
					i.linkType = LinkProps::linkFolder;
				}
				else {
					if (url.Right(5) == _T(".iedx") || url.Right(4) == _T(".ied")) {
						i.linkType = LinkProps::iedFile;
					}
					else {
						i.linkType = LinkProps::FileName;
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

void iEditDoc::notifySelectLink(const LinkPropsVec &ls, int index, bool drwAll)
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
		}
		else {
			(*it).selectLink(false);
		}
	}

	if (selected) {
		if (ls[index].linkType == LinkProps::linkSL) {
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

void iEditDoc::deleteSpecifidLink(const LinkProps &i)
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

void iEditDoc::setSpecifiedLinkInfo(const LinkProps &iOld, const LinkProps &iNew)
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

void iEditDoc::SetNodeRelax(CRelaxThrd *r, bool bDrwAll)
{
	////////////////////////////////////////////////
	// 自動レイアウトアルゴリズム用のedgeデータ設定
	////////////////////////////////////////////////
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();

	r->bounds.clear();
	r->edges.clear();
	r->edges.resize(0);
	literator li = links_.begin();
	for (; li != links_.end(); li++) {
		if ((*li).getArrowStyle() != iLink::other) {
			if ((*li).getKeyFrom() == (*li).getKeyTo()) continue;
			if (!(*li).canDraw() && !bDrwAll) continue;
			iEdge e;
			e.from = (*li).getKeyFrom();
			e.to = (*li).getKeyTo();

			niterator itFrom = nodes_.findNodeW((*li).getKeyFrom());
			niterator itTo = nodes_.findNodeW((*li).getKeyTo());

			CRect rFrom = (*itFrom).second.getBound();
			CRect rTo = (*itTo).second.getBound();

			CSize sz;
			sz.cx = (rFrom.Width() + rTo.Width()) / 2;
			sz.cy = (rFrom.Height() + rTo.Height()) / 2;

			////////////////////////
			// アイコン間の距離設定
			////////////////////////
			double rate;
			if (pApp->m_rgsLink.bSetStrength) {
				rate = width2Len((*li).getLineWidth());
				if ((*li).getLineStyle() == PS_DOT) {
					rate *= 1.5;
				}
			}
			else {
				rate = width2Len(0);
			}
			rate *= (((double)pApp->m_rgsLink.strength) / 10.0);
			e.len = sqrt((double)(sz.cx*sz.cx + sz.cy*sz.cy)) * 5 / 4 * rate;

			///////////////////
			// edges への登録
			///////////////////
			bool already = false; // 登録されているedgeとの重複をチェック
			for (unsigned int i = 0; i < r->edges.size(); i++) {
				if (r->edges[i].from == e.from && r->edges[i].to == e.to ||
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
			b.label = (*itFrom).second.getName();
			b.fixed = (*itFrom).second.isFixed();
			b.oldBound = (*itFrom).second.getBound();
			b.newBound = b.oldBound;
			r->bounds.insert(b);

			iBound b2;
			b2.key = e.to;
			b2.label = (*itTo).second.getName();
			b2.fixed = (*itTo).second.isFixed();
			b2.oldBound = (*itTo).second.getBound();
			b2.newBound = b2.oldBound;
			r->bounds.insert(b2);
		}
	}
}

double iEditDoc::width2Len(int width)
{
	double l;
	switch (width) {
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
	for (; it != nodes_.end(); it++) {
		if ((*it).second.isVisible() && (*it).second.isSelected()) {
			rc = (*it).second.getBound();
			break;
		}
	}

	it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if ((*it).second.isVisible() && (*it).second.isSelected()) {
			rc |= (*it).second.getBound();
			const_literator li = links_.begin();
			for (; li != links_.end(); li++) {
				if (!(*li).canDraw()/* && !drwAll*/) {
					continue;
				}
				if ((*it).second.getKey() == (*li).getKeyFrom() || (*it).second.getKey() == (*li).getKeyTo()) {
					rc |= (*li).getBound();
				}
			}
		}
	}

	return rc;
}

CRect iEditDoc::GetRelatedBoundAnd(bool drwAll)
{
	CRect rc(CRect(0, 0, 0, 0));

	const_niterator it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if ((*it).second.isVisible() && (*it).second.isSelected()) {
			rc = (*it).second.getBound();
			break;
		}
	}

	it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if (/*!drwAll && */ (*it).second.isVisible() && (*it).second.isSelected() /*|| drwAll && (*it).isSelected()*/) {
			rc |= (*it).second.getBound();
		}
	}

	const_literator li = links_.begin();
	for (; li != links_.end(); li++) {
		if (!(*li).canDraw()) {
			continue;
		}
		niterator itFrom = nodes_.findNodeW((*li).getKeyFrom());
		niterator itTo = nodes_.findNodeW((*li).getKeyTo());

		if ((*itFrom).second.isSelected() && (*itTo).second.isSelected()) {
			rc |= (*li).getBound();
		}
	}

	return rc;
}

void iEditDoc::setSelectedLinkCurve(CPoint pt, bool curve, bool bDrwAll)
{
	literator li = links_.getSelectedLinkW();
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
		if ((*li).hitTest2(pt) && (*li).getName() == _T("")) {
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
	BackupLinksForUndo();
	literator li = links_.getSelectedLinkW();
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
	const_literator li = links_.getSelectedLink();
	if (li != links_.end()) {
		const_niterator itstart = nodes_.findNode((*li).getKeyFrom());
		const_niterator itend = nodes_.findNode((*li).getKeyTo());
		if (itstart == nodes_.end() || itend == nodes_.end()) {
			start = CPoint(0, 0); end = CPoint(0, 0);
			return;
		}
		start.x = ((*itstart).second.getBound().left + (*itstart).second.getBound().right) / 2;
		start.y = ((*itstart).second.getBound().top + (*itstart).second.getBound().bottom) / 2;
		end.x = ((*itend).second.getBound().left + (*itend).second.getBound().right) / 2;
		end.y = ((*itend).second.getBound().top + (*itend).second.getBound().bottom) / 2;
	}
}

void iEditDoc::selectChild()
{
	iHint h; h.event = iHint::selectChild;
	UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
}

BOOL iEditDoc::isSelectedLinkCurved(bool bDrwAll) const
{
	const_literator li = links_.getSelectedLink();
	if (li != links_.end() && (*li).isCurved()) {
		return TRUE;
	}
	return FALSE;
}

BOOL iEditDoc::isSelectedLinkSelf() const
{
	const_literator li = links_.getSelectedLink();

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

	const_niterator itFrom = nodes_.findNode(m_cpLinkOrg.getKeyFrom());
	const_niterator itTo = nodes_.findNode(m_cpLinkOrg.getKeyTo());

	if (itFrom != nodes_.end() && itTo != nodes_.end()) {
		m_cpLinkOrg.setNodes((*itFrom).second.getBound(), (*itTo).second.getBound(),
			(*itFrom).second.getKey(), (*itTo).second.getKey());
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

BOOL iEditDoc::LinksExist() const
{
	const_literator li = links_.begin();
	for (; li != links_.end(); li++) {
		if ((*li).canDraw()) {
			return TRUE;
		}
	}
	return FALSE;
}

void iEditDoc::AddSelectedNodesToCopyOrg()
{
	copyOrg.clear(); copyOrg.resize(0);
	niterator It = nodes_.getSelectedNode();
	ptSelectMin = (*It).second.getBound().TopLeft();
	const_niterator it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if ((*it).second.isSelected()) {
			copyOrg.push_back((*it).second.getKey());
			if (ptSelectMin.x > (*it).second.getBound().TopLeft().x) {
				ptSelectMin.x = (*it).second.getBound().TopLeft().x;
			}
			if (ptSelectMin.y > (*it).second.getBound().TopLeft().y) {
				ptSelectMin.y = (*it).second.getBound().TopLeft().y;
			}
		}
	}
}

void iEditDoc::DuplicateNodes(const CPoint& pt, bool useDefault)
{
	for (unsigned int i = 0; i < copyOrg.size(); i++) {
		niterator it = nodes_.findNodeW(copyOrg[i]);
		iNode n((*it).second);
		n.setKey(getUniqKey());
		n.setParent(nodes_.getCurParent());

		if (!useDefault) {
			CPoint ptNew = pt;
			int szx = (*it).second.getBound().TopLeft().x - ptSelectMin.x;
			int szy = (*it).second.getBound().TopLeft().y - ptSelectMin.y;
			ptNew.x += szx; ptNew.y += szy;
			n.moveTo(ptNew);
		}
		else {
			n.moveBound(CSize(20, 30));
		}

		nodes_[n.getKey()] = n;

		if (ShowSubBranch()) {
			m_visibleKeys.insert(n.getKey());
		}

		selChanged(n.getKey(), true, ShowSubBranch());
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

BOOL iEditDoc::CanDuplicateNodes()
{
	if (copyOrg.size() != 0) return TRUE;
	return FALSE;
}

int iEditDoc::GetSelectedNodeShape() const
{
	return nodes_.getSelectedNodeShape();
}

void iEditDoc::SetSelectedNodeShape(int shape, int mfIndex)
{
	DisableUndo();
	BackupNodesForUndo();
	BackupLinksForUndo();
	nodes_.setSelectedNodeShape(shape);
	SetModifiedFlag();
	if (shape == iNode::MetaFile) {
		niterator it = nodes_.getSelectedNode();
		if (it != nodes_.end()) {
			CiEditApp* pApp = (CiEditApp*)AfxGetApp();
			(*it).second.setMetaFile(pApp->m_hMetaFiles[mfIndex]);
			SetConnectionPoint();
		}
	}
	iHint h; h.event = iHint::nodeStyleChanged;
	UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
}

void iEditDoc::SetSelectedNodeMetaFile(HENHMETAFILE metafile)
{
	niterator it = nodes_.getSelectedNode();
	if (it != nodes_.end()) {
		DisableUndo();
		BackupNodesForUndo();
		BackupLinksForUndo();
		(*it).second.setNodeShape(iNode::MetaFile);
		(*it).second.setMetaFile(metafile);
		SetConnectionPoint();
		SetModifiedFlag();
		iHint h; h.event = iHint::nodeStyleChanged;
		UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
	}
}

void iEditDoc::SetSelectedNodeLabel(const CString &label)
{
	niterator it = nodes_.getSelectedNode();
	if (it != nodes_.end()) {
		(*it).second.setName(label);
		SetModifiedFlag();
		iHint hint; hint.event = iHint::nodeLabelChanged;
		hint.str = label;
		DWORD key = nodes_.getSelKey();
		UpdateAllViews(NULL, (LPARAM)key, &hint);
	}
}


int iEditDoc::GetSelectedNodeTextStyle() const
{
	return nodes_.getSelectedNodeTextStyle();
}

void iEditDoc::SetSelectedNodeTextStyle(int style)
{
	nodes_.setSelectedNodeTextStyle(style);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.getSelKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::SetSelectedNodeTreeIconId(int id)
{
	nodes_.setSelectedNodeTreeIconId(id);
	SetModifiedFlag();
}

void iEditDoc::SetSelectedNodeFixed(BOOL f)
{
	nodes_.setSelectedNodeFixed(f);
	SetModifiedFlag();
}

BOOL iEditDoc::IsSelectedNodeFixed() const
{
	return nodes_.isSelectedNodeFixed();
}

void iEditDoc::SetResultRelax(Bounds &bounds)
{
	Bounds::iterator it = bounds.begin();
	for (; it != bounds.end(); it++) {
		niterator nit = nodes_.findNodeW((*it).key);
		(*nit).second.setBound((*it).newBound);
	}
	calcMaxPt(m_maxPt);
	SetConnectionPoint();
	SetModifiedFlag();
}

// このloadメソッドはインポート用
bool iEditDoc::LoadXml(const CString &filename, bool replace)
{
	MSXML2::IXMLDOMDocument		*pDoc = NULL;
	MSXML2::IXMLDOMParseError	*pParsingErr = NULL;
	MSXML2::IXMLDOMElement		*element = NULL;
	MSXML2::IXMLDOMNodeList		*childs = NULL;
	MSXML2::IXMLDOMNode			*node = NULL;

	BSTR	bstr = NULL;
	HRESULT hr;
	int     rc = 0;

	hr = CoInitialize(NULL);
	if (!SUCCEEDED(hr))
		return false;

	hr = CoCreateInstance(MSXML2::CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
		MSXML2::IID_IXMLDOMDocument, (LPVOID *)&pDoc);
	if (!pDoc) {
		AfxMessageBox(_T("XML ドキュメントをパースできません。"));
		return false;
	}
	pDoc->put_async(VARIANT_FALSE);
	bstr = filename.AllocSysString();
	hr = pDoc->load(bstr);
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
	}
	else {
		idcVec.clear(); idcVec.resize(0);
		nodesImport.clear(); nodesImport.resize(0);
		nodeImport.setKey(-1); linkImport.setKeyFrom(-1);

		CWaitCursor wc;
		pDoc->get_documentElement(&element);

		BSTR s = NULL;
		element->get_nodeTypeString(&s);

		if (!wcscmp(s, L"element")) {
			element->get_nodeName(&s);
			CString elems(s);
			if (elems != _T("iEditDoc")) {
				AfxMessageBox(_T("これはiEdit用のXMLファイルではありません"));
				return false;
			}
		}
		nodeImport.setBound(CRect(-1, -1, 0, 0));
		nodeImport.setName(_T(""));
		nodeImport.setText(_T(""));
		nodeImport.setTreeState(TVIS_EXPANDED);
		linkImport.setName(_T(""));
		linkImport.setPath(_T(""));
		linkImport.setArrowStyle(iLink::line);
		linkImport.setLineWidth(0);
		linkImport.setLinkColor(RGB(0, 0, 0));

		CStdioFile f;
		CFileStatus status;
		CFileException e;

		if (!f.Open(_T("import.log"), CFile::typeText | CFile::modeCreate | CFile::modeWrite, &e)) {
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
	MSXML2::IXMLDOMDocument		*pDoc = NULL;
	MSXML2::IXMLDOMParseError	*pParsingErr = NULL;
	MSXML2::IXMLDOMElement		*element = NULL;
	MSXML2::IXMLDOMNodeList		*childs = NULL;
	MSXML2::IXMLDOMNode			*node = NULL;

	BSTR	bstr = NULL;
	HRESULT hr;
	int     rc = 0;

	hr = CoInitialize(NULL);
	if (!SUCCEEDED(hr))
		return false;

	hr = CoCreateInstance(MSXML2::CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
		MSXML2::IID_IXMLDOMDocument, (LPVOID *)&pDoc);
	if (!pDoc) {
		AfxMessageBox(_T("XML ドキュメントをパースできません。"));
		return false;
	}
	pDoc->put_async(VARIANT_FALSE);
	bstr = filename.AllocSysString();
	hr = pDoc->load(bstr);
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
	}
	else {
		idcVec.clear(); idcVec.resize(0);
		nodesImport.clear(); nodesImport.resize(0);
		nodeImport.setKey(-1); linkImport.setKeyFrom(-1);

		CWaitCursor wc;
		pDoc->get_documentElement(&element);

		BSTR s = NULL;
		element->get_nodeTypeString(&s);

		if (!wcscmp(s, L"element")) {
			element->get_nodeName(&s);
			CString elems(s);
			if (elems != _T("iEditDoc")) {
				AfxMessageBox(_T("これはiEdit用のXMLファイルではありません"));
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

		if (nodesImport.size() > 0 && nodesImport[0].getKey() != 0) {
			CString mes = _T("部分的にエクスポートしたデータを直接開くことはできません。\n";
			mes += _T("インポート機能を使用して取り込んでください。");
			AfxMessageBox(mes));
			return false;
		}

		if (ret) {
			unsigned int i;
			for (i = 0; i < nodesImport.size(); i++) {
				nodes_[nodesImport[i].getKey()] = nodesImport[i];
				sv.push_back(nodesImport[i].getKey());
				if (nodesImport[i].getKey() > lastKey) {
					lastKey = nodesImport[i].getKey();
				}
			}
			lastLinkKey = 0;
			const_niterator it;
			for (i = 0; i < linksImport.size(); i++) {
				linksImport[i].setKey(lastLinkKey++);
				it = nodes_.findNode(linksImport[i].getKeyFrom());
				if (it != nodes_.end()) {
					linksImport[i].setRFrom((*it).second.getBound());
				}
				it = nodes_.findNode(linksImport[i].getKeyTo());
				if (it != nodes_.end()) {
					linksImport[i].setRTo((*it).second.getBound());
				}
				links_.push_back(linksImport[i]);
			}
		}
		m_bLoadXmlSucceeded = true;
		return ret;
	}
	return false;
}


// インポート時
bool iEditDoc::DomTree2Nodes2(MSXML2::IXMLDOMElement *node, CStdioFile* f)
{
	MSXML2::IXMLDOMNodeList	*childs = NULL;
	MSXML2::IXMLDOMNodeList	*childs2 = NULL;
	MSXML2::IXMLDOMNodeList	*childs3 = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	MSXML2::IXMLDOMNode		*childnode2 = NULL;
	node->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i, j;
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		if (ename == _T("inode")) {
			iNode n(_T("add")); n.setKey(getUniqKey());
			n.setNoBrush(FALSE);
			n.setBound(CRect(-1, -1, 0, 0));
			nodesImport.push_back(n);
			childnode->get_childNodes(&childs2);
			for (j = 0; j < childs2->Getlength(); j++) {
				childs2->get_item(j, &childnode2);
				BSTR name2 = childnode2->nodeName;
				CString ename2(name2);
				if (ename2 == _T("id")) {
					childnode2->firstChild->get_text(&s);
					CString ids(s);
					int id;
					swscanf_s((const wchar_t*)ids.GetBuffer(), _T("%d"), &id);
					NodeKeyPair idc;
					idc.first = (DWORD)id;
					idc.second = nodesImport[nodesImport.size() - 1].getKey();
					idcVec.push_back(idc);

					ids += ' '; f->WriteString(ids); // log

				}
				else if (ename2 == _T("pid")) {
					childnode2->firstChild->get_text(&s);
					CString pids(s);
					int pid;
					swscanf_s((const wchar_t*)pids.GetBuffer(), _T("%d"), &pid);
					nodesImport[nodesImport.size() - 1].setParent((DWORD)pid);
				}
				else if (ename2 == _T("label")) {
					childnode2->firstChild->get_text(&s);
					CString name(s);
					nodesImport[nodesImport.size() - 1].setName(name);

					name += '\n'; f->WriteString(name); // log

				}
				else if (ename2 == _T("text")) {
					childnode2->firstChild->get_text(&s);
					CString text(s);
					text = StringUtil::ReplaceLfToCrlf(text);
					nodesImport[nodesImport.size() - 1].setText(text);
				}
				else if (ename2 == _T("labelAlign")) {
					childnode2->firstChild->get_text(&s);
					CString align(s);
					nodesImport[nodesImport.size() - 1].setTextStyle(tag2Align(align));
				}
				else if (ename2 == _T("shape")) {
					childnode2->firstChild->get_text(&s);
					CString shape(s);
					nodesImport[nodesImport.size() - 1].setNodeShape(tag2Shape(shape));
				}
				else if (ename2 == _T("bound")) {
					CRect rc = nodesImport[nodesImport.size() - 1].getBound();
					tags2bound(childnode2, rc);
					rc.NormalizeRect();
					nodesImport[nodesImport.size() - 1].setBound(rc);
				}
				else if (ename2 == _T("ForColor")) {
					COLORREF cr = tags2foreColor(childnode2);
					nodesImport[nodesImport.size() - 1].setBrush(cr);
				}
				else if (ename2 == _T("nodeLine")) {
					int lineStyle(PS_SOLID), lineWidth(0);
					tags2nodeLine(childnode2, lineStyle, lineWidth);
					nodesImport[nodesImport.size() - 1].setLineStyle(lineStyle);
					nodesImport[nodesImport.size() - 1].setLineWidth(lineWidth);
				}
				else if (ename2 == _T("nodeLineColor")) {
					COLORREF cr = tags2nodeLineColor(childnode2);
					nodesImport[nodesImport.size() - 1].setLineColor(cr);
				}
			}
		}
		else if (ename == _T("ilink")) {
			iLink l;
			linksImport.push_back(l);
			childnode->get_childNodes(&childs2);
			for (j = 0; j < childs2->Getlength(); j++) {
				childs2->get_item(j, &childnode2);
				BSTR name2 = childnode2->nodeName;
				CString ename2(name2);
				if (ename2 == _T("from")) {
					childnode2->firstChild->get_text(&s);
					CString from(s); int idfrom; swscanf_s((const wchar_t*)from.GetBuffer(), _T("%d"), &idfrom);
					linksImport[linksImport.size() - 1].setKeyFrom(findPairKey((DWORD)idfrom));
				}
				else if (ename2 == _T("to")) {
					childnode2->firstChild->get_text(&s);
					CString to(s); int idto; swscanf_s((const wchar_t*)to.GetBuffer(), _T("%d"), &idto);
					linksImport[linksImport.size() - 1].setKeyTo(findPairKey((DWORD)idto));
				}
				else if (ename2 == _T("caption")) {
					childnode2->firstChild->get_text(&s);
					linksImport[linksImport.size() - 1].setName(CString(s));
				}
				else if (ename2 == _T("linkLine")) {
					int style(PS_SOLID); int lineWidth(0); int arrow(iLink::line);
					tags2linkStyle(childnode2, style, lineWidth, arrow);
					linksImport[linksImport.size() - 1].setLineStyle(style);
					linksImport[linksImport.size() - 1].setLineWidth(lineWidth);
					linksImport[linksImport.size() - 1].setArrowStyle(arrow);
				}
				else if (ename2 == _T("linkLineColor")) {
					COLORREF rc = tags2linkColor(childnode2);
					linksImport[linksImport.size() - 1].setLinkColor(rc);
				}
				else if (ename2 == _T("pathPt")) {
					CPoint pt = tags2pathPt(childnode2);
					linksImport[linksImport.size() - 1].setPathPt(pt);
				}
				else if (ename2 == _T("locate")) {
					childnode2->firstChild->get_text(&s);
					CString path(s);
					linksImport[linksImport.size() - 1].setPath(path);
					linksImport[linksImport.size() - 1].setArrowStyle(iLink::other);
				}
			}
		}
	}
	return true;
}

// シリアライズ用
bool iEditDoc::DomTree2Nodes3(MSXML2::IXMLDOMElement *node)
{
	MSXML2::IXMLDOMNodeList	*childs = NULL;
	MSXML2::IXMLDOMNodeList	*childs2 = NULL;
	MSXML2::IXMLDOMNodeList	*childs3 = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	MSXML2::IXMLDOMNode		*childnode2 = NULL;
	node->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i, j;
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		if (ename == _T("inode")) {
			iNode n(_T("add")); n.setKey(0);
			n.setNoBrush(FALSE);
			n.setBound(CRect(-1, -1, 0, 0));
			nodesImport.push_back(n);
			childnode->get_childNodes(&childs2);
			for (j = 0; j < childs2->Getlength(); j++) {
				childs2->get_item(j, &childnode2);
				BSTR name2 = childnode2->nodeName;
				CString ename2(name2);
				if (ename2 == _T("id")) {
					childnode2->firstChild->get_text(&s);
					CString ids(s);
					int id;
					swscanf_s((const wchar_t*)ids.GetBuffer(), _T("%d"), &id);

					//	ids += ' '; f->WriteString(ids); // log
					nodesImport[nodesImport.size() - 1].setKey((DWORD)id);
					if (id == 0) {
						nodesImport[nodesImport.size() - 1].setTreeState(TVIS_EXPANDED | TVIS_SELECTED);
					}
					else {
						nodesImport[nodesImport.size() - 1].setTreeState(0);
					}
				}
				else if (ename2 == _T("pid")) {
					childnode2->firstChild->get_text(&s);
					CString pids(s);
					int pid;
					swscanf_s((const wchar_t *)pids.GetBuffer(), _T("%d"), &pid);
					nodesImport[nodesImport.size() - 1].setParent((DWORD)pid);
					/*	} else if (ename2 == "level") {
							childnode2->firstChild->get_text(&s);
							CString sLevel(s);
							int nLevel;
							sscanf(sLevel, "%d", &nLevel);
							nodesImport[nodesImport.size()-1].setLevel(nLevel); */
				}
				else if (ename2 == _T("label")) {
					childnode2->firstChild->get_text(&s);
					CString name(s);
					nodesImport[nodesImport.size() - 1].setName(name);

					//	name += '\n'; f->WriteString(name); // log

				}
				else if (ename2 == _T("text")) {
					childnode2->firstChild->get_text(&s);
					CString text(s);
					text = StringUtil::ReplaceLfToCrlf(text);
					nodesImport[nodesImport.size() - 1].setText(text);
				}
				else if (ename2 == _T("labelAlign")) {
					childnode2->firstChild->get_text(&s);
					CString align(s);
					nodesImport[nodesImport.size() - 1].setTextStyle(tag2Align(align));
				}
				else if (ename2 == _T("shape")) {
					childnode2->firstChild->get_text(&s);
					CString shape(s);
					nodesImport[nodesImport.size() - 1].setNodeShape(tag2Shape(shape));
				}
				else if (ename2 == _T("bound")) {
					CRect rc = nodesImport[nodesImport.size() - 1].getBound();
					tags2bound(childnode2, rc);
					rc.NormalizeRect();
					nodesImport[nodesImport.size() - 1].setBound(rc);
				}
				else if (ename2 == _T("ForColor")) {
					COLORREF cr = tags2foreColor(childnode2);
					nodesImport[nodesImport.size() - 1].setBrush(cr);
				}
				else if (ename2 == _T("nodeLine")) {
					int lineStyle(PS_SOLID), lineWidth(0);
					tags2nodeLine(childnode2, lineStyle, lineWidth);
					nodesImport[nodesImport.size() - 1].setLineStyle(lineStyle);
					nodesImport[nodesImport.size() - 1].setLineWidth(lineWidth);
				}
				else if (ename2 == _T("nodeLineColor")) {
					COLORREF cr = tags2nodeLineColor(childnode2);
					nodesImport[nodesImport.size() - 1].setLineColor(cr);
				}
			}
		}
		else if (ename == _T("ilink")) {
			iLink l;
			linksImport.push_back(l);
			childnode->get_childNodes(&childs2);
			for (j = 0; j < childs2->Getlength(); j++) {
				childs2->get_item(j, &childnode2);
				BSTR name2 = childnode2->nodeName;
				CString ename2(name2);
				if (ename2 == _T("from")) {
					childnode2->firstChild->get_text(&s);
					CString from(s); int idfrom; swscanf_s((const wchar_t*)from.GetBuffer(), _T("%d"), &idfrom);
					linksImport[linksImport.size() - 1].setKeyFrom(((DWORD)idfrom));
				}
				else if (ename2 == _T("to")) {
					childnode2->firstChild->get_text(&s);
					CString to(s); int idto; swscanf_s((const wchar_t*)to.GetBuffer(), _T("%d"), &idto);
					linksImport[linksImport.size() - 1].setKeyTo(((DWORD)idto));
				}
				else if (ename2 == _T("caption")) {
					childnode2->firstChild->get_text(&s);
					linksImport[linksImport.size() - 1].setName(CString(s));
				}
				else if (ename2 == _T("linkLine")) {
					int style(PS_SOLID); int lineWidth(0); int arrow(iLink::line);
					tags2linkStyle(childnode2, style, lineWidth, arrow);
					linksImport[linksImport.size() - 1].setLineStyle(style);
					linksImport[linksImport.size() - 1].setLineWidth(lineWidth);
					linksImport[linksImport.size() - 1].setArrowStyle(arrow);
				}
				else if (ename2 == _T("linkLineColor")) {
					COLORREF rc = tags2linkColor(childnode2);
					linksImport[linksImport.size() - 1].setLinkColor(rc);
				}
				else if (ename2 == _T("pathPt")) {
					CPoint pt = tags2pathPt(childnode2);
					linksImport[linksImport.size() - 1].setPathPt(pt);
				}
				else if (ename2 == _T("locate")) {
					childnode2->firstChild->get_text(&s);
					CString path(s);
					linksImport[linksImport.size() - 1].setPath(path);
					linksImport[linksImport.size() - 1].setArrowStyle(iLink::other);
				}
			}
		}
	}
	return true;
}

int iEditDoc::tag2Align(const CString &tag)
{
	if (tag == _T("single-middle-center")) {
		return iNode::s_cc;
	}
	else if (tag == _T("single-middle-left")) {
		return iNode::s_cl;
	}
	else if (tag == _T("single-midele-right")) {
		return iNode::s_cr;
	}
	else if (tag == _T("single-top-center")) {
		return iNode::s_tc;
	}
	else if (tag == _T("single-top-left")) {
		return iNode::s_tl;
	}
	else if (tag == _T("single-top-right")) {
		return iNode::s_tr;
	}
	else if (tag == _T("single-bottom-center")) {
		return iNode::s_bc;
	}
	else if (tag == _T("single-bottom-left")) {
		return iNode::s_bl;
	}
	else if (tag == _T("single-bottom-right")) {
		return iNode::s_br;
	}
	else if (tag == _T("multi-center")) {
		return iNode::m_c;
	}
	else if (tag == _T("multi-left")) {
		return iNode::m_l;
	}
	else if (tag == _T("multi-right")) {
		return iNode::m_r;
	}
	else if (tag == _T("hidden")) {
		return iNode::notext;
	}
	return iNode::s_cc;
}


int iEditDoc::tag2Shape(const CString &tag)
{
	if (tag == _T("Rect")) {
		return iNode::rectangle;
	}
	else if (tag == _T("Oval")) {
		return iNode::arc;
	}
	else if (tag == _T("RoundRect")) {
		return iNode::roundRect;
	}
	else if (tag == _T("MetaFile")) {
		return iNode::MetaFile;
	}
	else if (tag == _T("MMNode")) {
		return (iNode::MindMapNode);
	}
	return iNode::rectangle;
}

void iEditDoc::tags2bound(MSXML2::IXMLDOMNode *pNode, CRect &rc)
{
	MSXML2::IXMLDOMNodeList	*childs = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	pNode->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i;
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		childnode->firstChild->get_text(&s);
		if (ename == _T("left")) {
			CString left(s);
			swscanf_s((const wchar_t*)left.GetBuffer(), _T("%d"), &rc.left);
		}
		else if (ename == _T("right")) {
			CString right(s);
			swscanf_s((const wchar_t*)right.GetBuffer(), _T("%d"), &rc.right);
		}
		else if (ename == _T("top")) {
			CString top(s);
			swscanf_s((const wchar_t*)top.GetBuffer(), _T("%d"), &rc.top);
		}
		else if (ename == _T("bottom")) {
			childnode->firstChild->get_text(&s);
			CString bottom(s);
			swscanf_s((const wchar_t*)bottom.GetBuffer(), _T("%d"), &rc.bottom);
		}
	}
}

COLORREF iEditDoc::tags2foreColor(MSXML2::IXMLDOMNode *pNode)
{
	MSXML2::IXMLDOMNodeList	*childs = NULL;
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
		if (ename == _T("f_red")) {
			CString red(s);
			swscanf_s((const wchar_t*)red.GetBuffer(), _T("%d"), &r);
		}
		else if (ename == _T("f_green")) {
			CString green(s);
			swscanf_s((const wchar_t*)green.GetBuffer(), _T("%d"), &g);
		}
		else if (ename == _T("f_blue")) {
			CString blue(s);
			swscanf_s((const wchar_t*)blue.GetBuffer(), _T("%d"), &b);
		}
	}
	return RGB(r, g, b);
}

void iEditDoc::tags2nodeLine(MSXML2::IXMLDOMNode *pNode, int &style, int &width)
{
	MSXML2::IXMLDOMNodeList	*childs = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	pNode->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i;
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		childnode->firstChild->get_text(&s);
		if (ename == _T("nodeLineStyle")) {
			CString lstyle(s);
			if (lstyle == _T("solidLine")) {
				style = PS_SOLID;
			}
			else if (lstyle == _T("dotedLine")) {
				style = PS_DOT;
			}
			else if (lstyle == _T("noLine")) {
				style = PS_NULL;
			}
		}
		else if (ename == _T("nodeLineWidth")) {
			CString lwidth(s); int w; swscanf_s((const wchar_t*)lwidth.GetBuffer(), _T("%d"), &w);
			if (w == 1) w = 0;
			width = w;
		}
	}
}


COLORREF iEditDoc::tags2nodeLineColor(MSXML2::IXMLDOMNode *pNode)
{
	MSXML2::IXMLDOMNodeList	*childs = NULL;
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
		if (ename == _T("l_red")) {
			CString red(s);
			swscanf_s((const wchar_t*)red.GetBuffer(), _T("%d"), &r);
		}
		else if (ename == _T("l_green")) {
			CString green(s);
			swscanf_s((const wchar_t*)green.GetBuffer(), _T("%d"), &g);
		}
		else if (ename == _T("l_blue")) {
			CString blue(s);
			swscanf_s((const wchar_t*)blue.GetBuffer(), _T("%d"), &b);
		}
	}
	return RGB(r, g, b);
}

void iEditDoc::tags2linkStyle(MSXML2::IXMLDOMNode *pNode, int &style, int &width, int &arrow)
{
	MSXML2::IXMLDOMNodeList	*childs = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	pNode->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i;
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		childnode->firstChild->get_text(&s);
		if (ename == _T("linkLineStyle")) {
			CString lstyle(s);
			if (lstyle == _T("solidLine")) {
				style = PS_SOLID;
			}
			else if (lstyle == _T("dotedLine")) {
				style = PS_DOT;
			}
		}
		else if (ename == _T("linkLineWidth")) {
			CString lwidth(s); int w; swscanf_s((const wchar_t*)lwidth.GetBuffer(), _T("%d"), &w);
			if (w == 1) w = 0;
			width = w;
		}
		else if (ename == _T("arrow")) {
			CString astyle(s);
			if (astyle == _T("a_none")) {
				arrow = iLink::line;
			}
			else if (astyle == _T("a_single")) {
				arrow = iLink::arrow;
			}
			else if (astyle == _T("a_double")) {
				arrow = iLink::arrow2;
			}
			else if (astyle == _T("a_depend")) {
				arrow = iLink::depend;
			}
			else if (astyle == _T("a_depend_double")) {
				arrow = iLink::depend2;
			}
			else if (astyle == _T("a_inherit")) {
				arrow = iLink::inherit;
			}
			else if (astyle == _T("a_aggregat")) {
				arrow = iLink::aggregat;
			}
			else if (astyle == _T("a_composit")) {
				arrow = iLink::composit;
			}
		}
	}
}

COLORREF iEditDoc::tags2linkColor(MSXML2::IXMLDOMNode *pNode)
{
	MSXML2::IXMLDOMNodeList	*childs = NULL;
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
		if (ename == _T("n_red")) {
			CString red(s);
			swscanf_s((const wchar_t*)red.GetBuffer(), _T("%d"), &r);
		}
		else if (ename == _T("n_green")) {
			CString green(s);
			swscanf_s((const wchar_t*)green.GetBuffer(), _T("%d"), &g);
		}
		else if (ename == _T("n_blue")) {
			CString blue(s);
			swscanf_s((const wchar_t*)blue.GetBuffer(), _T("%d"), &b);
		}
	}
	return RGB(r, g, b);
}


CPoint iEditDoc::tags2pathPt(MSXML2::IXMLDOMNode *pNode)
{
	CPoint pt(0, 0);
	MSXML2::IXMLDOMNodeList	*childs = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	pNode->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i;
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		childnode->firstChild->get_text(&s);
		if (ename == _T("path_x")) {
			CString x(s);
			swscanf_s((const wchar_t*)x.GetBuffer(), _T("%d"), &pt.x);
		}
		else if (ename == _T("path_y")) {
			CString y(s);
			swscanf_s((const wchar_t*)y.GetBuffer(), _T("%d"), &pt.y);
		}

	}
	return pt;
}

// エクスポート時のXML出力関数
bool iEditDoc::SaveXml(const CString &outPath, bool bSerialize)
{
	FILE* fp;
	if (_tfopen_s(&fp, outPath, _T("w, ccs=UTF-8")) != 0) {
		AfxMessageBox(_T("coud not open file. ") + outPath);
		return false;
	}
	CStdioFile f(fp);

	OutlineView* pView = getOutlineView();

	NodePropsVec ls;
	if (bSerialize) {
		pView->treeToSequence0(ls);
	}
	else {
		pView->treeToSequence(ls);
	}

	_wsetlocale(LC_ALL, _T("jpn"));
	// Header of XML file
	f.WriteString(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
	outStyleSheetLine(f);
	f.WriteString(_T("<iEditDoc>\n"));

	// iNodes -->iNode Data
	for (unsigned int i = 0; i < ls.size(); i++) {
		const_niterator it = nodes_.findNode(ls[i].key);

		f.WriteString(_T("\t<inode>\n"));

		CString ids;
		DWORD key, parent;
		key = (*it).second.getKey(); parent = (*it).second.getParent();
		if (i == 0 && key != parent) {
			parent = key;
		}
		ids.Format(_T("\t\t<id>%d</id>\n\t\t<pid>%d</pid>\n"), key, parent);
		f.WriteString(ids);

		CString sLevel;
		sLevel.Format(_T("\t\t<level>%d</level>\n"), (*it).second.getLevel());
		f.WriteString(sLevel);

		f.WriteString(_T("\t\t<label>"));
		CString title = _T("<![CDATA[") + (*it).second.getName() + _T("]]>");
		if ((*it).second.getTextStyle() >= iNode::m_c) {
			f.WriteString(StringUtil::ReplaceCrToLf(title));
		}
		else {
			f.WriteString(title);
		}
		f.WriteString(_T("</label>\n"));

		f.WriteString(_T("\t\t<text>"));
		CString text = _T("<![CDATA[") + (*it).second.getText() + _T("]]>");
		f.WriteString(StringUtil::ReplaceCrToLf(text));
		f.WriteString(_T("\n\t\t</text>\n"));

		// ラベルのアライメント
		f.WriteString(_T("\t\t<labelAlign>"));
		int align = (*it).second.getTextStyle(); CString salign;
		switch (align) {
		case iNode::s_cc: salign = _T("single-middle-center"); break;
		case iNode::s_cl: salign = _T("single-middle-left"); break;
		case iNode::s_cr: salign = _T("single-midele-right"); break;
		case iNode::s_tc: salign = _T("single-top-center"); break;
		case iNode::s_tl: salign = _T("single-top-left"); break;
		case iNode::s_tr: salign = _T("single-top-right"); break;
		case iNode::s_bc: salign = _T("single-bottom-center"); break;
		case iNode::s_bl: salign = _T("single-bottom-left"); break;
		case iNode::s_br: salign = _T("single-bottom-right"); break;
		case iNode::m_c: salign = _T("multi-center"); break;
		case iNode::m_l: salign = _T("multi-left"); break;
		case iNode::m_r: salign = _T("multi-right"); break;
		case iNode::notext: salign = _T("hidden"); break;
		}
		f.WriteString(salign);
		f.WriteString(_T("</labelAlign>\n"));

		// 形
		f.WriteString(_T("\t\t<shape>"));
		int shape = (*it).second.getNodeShape();
		if (shape == iNode::rectangle) {
			f.WriteString(_T("Rect"));
		}
		else if (shape == iNode::arc) {
			f.WriteString(_T("Oval"));
		}
		else if (shape == iNode::roundRect) {
			f.WriteString(_T("RoundRect"));
		}
		else if (shape == iNode::MetaFile) {
			f.WriteString(_T("MetaFile"));
		}
		else if (shape == iNode::MindMapNode) {
			f.WriteString(_T("MMNode"));
		}
		else {
			f.WriteString(_T("Rect"));
		}
		f.WriteString(_T("</shape>\n"));

		// 位置
		CString spt;
		CRect bound = (*it).second.getBound();

		f.WriteString(_T("\t\t<bound>\n"));
		f.WriteString(_T("\t\t\t<left>"));
		spt.Format(_T("%d"), bound.left);
		f.WriteString(spt);
		f.WriteString(_T("</left>\n"));

		f.WriteString(_T("\t\t\t<right>"));
		spt.Format(_T("%d"), bound.right);
		f.WriteString(spt);
		f.WriteString(_T("</right>\n"));

		f.WriteString(_T("\t\t\t<top>"));
		spt.Format(_T("%d"), bound.top);
		f.WriteString(spt);
		f.WriteString(_T("</top>\n"));

		f.WriteString(_T("\t\t\t<bottom>"));
		spt.Format(_T("%d"), bound.bottom);
		f.WriteString(spt);
		f.WriteString(_T("</bottom>\n"));
		f.WriteString(_T("\t\t</bound>\n"));


		CString sc;
		// 色(塗りつぶし)
		if ((*it).second.isFilled()) {
			f.WriteString(_T("\t\t<ForColor>\n"));
			COLORREF fc = (*it).second.getBrsColor();
			BYTE fred = GetRValue(fc);
			BYTE fgrn = GetGValue(fc);
			BYTE fblu = GetBValue(fc);

			f.WriteString(_T("\t\t\t<f_red>"));
			sc.Format(_T("%d"), fred);
			f.WriteString(sc);
			f.WriteString(_T("</f_red>\n"));

			f.WriteString(_T("\t\t\t<f_green>"));
			sc.Format(_T("%d"), fgrn);
			f.WriteString(sc);
			f.WriteString(_T("</f_green>\n"));

			f.WriteString(_T("\t\t\t<f_blue>"));
			sc.Format(_T("%d"), fblu);
			f.WriteString(sc);
			f.WriteString(_T("</f_blue>\n"));
			f.WriteString(_T("\t\t</ForColor>\n"));
		}

		// 線のスタイル
		f.WriteString(_T("\t\t<nodeLine>\n"));
		f.WriteString(_T("\t\t\t<nodeLineStyle>"));
		if ((*it).second.getLineStyle() == PS_NULL) {
			f.WriteString(_T("noLine"));
		}
		else if ((*it).second.getLineStyle() == PS_SOLID) {
			f.WriteString(_T("solidLine"));
		}
		else if ((*it).second.getLineStyle() == PS_DOT) {
			f.WriteString(_T("dotedLine"));
		}
		f.WriteString(_T("</nodeLineStyle>\n"));

		if ((*it).second.getLineStyle() == PS_SOLID) {
			f.WriteString(_T("\t\t\t<nodeLineWidth>"));
			int width = (*it).second.getLineWidth();
			if (width == 0) {
				width = 1;
			}
			CString sl; sl.Format(_T("%d"), width);
			f.WriteString(sl);
			f.WriteString(_T("</nodeLineWidth>\n"));
		}
		f.WriteString(_T("\t\t</nodeLine>\n"));

		// 色(線)
		if ((*it).second.getLineStyle() != PS_NULL) {
			f.WriteString(_T("\t\t<nodeLineColor>\n"));
			COLORREF lc = (*it).second.getLineColor();
			BYTE lred = GetRValue(lc);
			BYTE lgrn = GetGValue(lc);
			BYTE lblu = GetBValue(lc);

			f.WriteString(_T("\t\t\t<l_red>"));
			sc.Format(_T("%d"), lred);
			f.WriteString(sc);
			f.WriteString(_T("</l_red>\n"));

			f.WriteString(_T("\t\t\t<l_green>"));
			sc.Format(_T("%d"), lgrn);
			f.WriteString(sc);
			f.WriteString(_T("</l_green>\n"));

			f.WriteString(_T("\t\t\t<l_blue>"));
			sc.Format(_T("%d"), lblu);
			f.WriteString(sc);
			f.WriteString(_T("</l_blue>\n"));
			f.WriteString(_T("\t\t</nodeLineColor>\n"));
		}

		// end of inode tag
		f.WriteString(_T("\t</inode>\n"));
	}

	// iLinks --> iLink Data
	const_literator li = links_.begin();
	for (; li != links_.end(); li++) {
		if (!isKeyInLabels(ls, (*li).getKeyFrom()) || !isKeyInLabels(ls, (*li).getKeyTo())) {
			continue;
		}

		f.WriteString(_T("\t<ilink>\n"));
		CString links;
		if ((*li).getArrowStyle() != iLink::other) {
			links.Format(_T("\t\t<from>%d</from>\n\t\t<to>%d</to>\n"), (*li).getKeyFrom(), (*li).getKeyTo());
		}
		else {
			links.Format(_T("\t\t<from>%d</from>\n\t\t<to>%d</to>\n"), (*li).getKeyFrom(), (*li).getKeyFrom());
		}
		f.WriteString(links);
		CString caption = _T("<![CDATA[") + (*li).getName() + "]]>";
		f.WriteString(_T("\t\t<caption>"));
		f.WriteString(caption);
		f.WriteString(_T("</caption>\n"));

		int astyle = (*li).getArrowStyle();

		if (astyle == iLink::other && (*li).getPath() != "") {
			f.WriteString(_T("\t\t<locate>"));
			CString path = _T("<![CDATA[") + (*li).getPath() + _T("]]>");
			f.WriteString(path);
			f.WriteString(_T("</locate>\n"));
		}
		else {
			f.WriteString(_T("\t\t<linkLine>\n"));

			f.WriteString(_T("\t\t\t<linkLineStyle>"));
			if ((*li).getLineStyle() == PS_SOLID) {
				f.WriteString(_T("solidLine"));
			}
			else if ((*li).getLineStyle() == PS_DOT) {
				f.WriteString(_T("dotedLine"));
			}
			f.WriteString(_T("</linkLineStyle>\n"));

			int width = (*li).getLineWidth();
			if (width == 0) {
				width = 1;
			}
			CString w; w.Format(_T("\t\t\t<linkLineWidth>%d</linkLineWidth>\n"), width);
			f.WriteString(w);

			f.WriteString(_T("\t\t\t<arrow>"));
			if (astyle == iLink::line) {
				f.WriteString(_T("a_none"));
			}
			else if (astyle == iLink::arrow) {
				f.WriteString(_T("a_single"));
			}
			else if (astyle == iLink::arrow2) {
				f.WriteString(_T("a_double"));
			}
			else if (astyle == iLink::depend) {
				f.WriteString(_T("a_depend"));
			}
			else if (astyle == iLink::depend2) {
				f.WriteString(_T("a_depend_double"));
			}
			else if (astyle == iLink::inherit) {
				f.WriteString(_T("a_inherit"));
			}
			else if (astyle == iLink::aggregat) {
				f.WriteString(_T("a_aggregat"));
			}
			else if (astyle == iLink::composit) {
				f.WriteString(_T("a_composit"));
			}
			f.WriteString(_T("</arrow>\n"));

			f.WriteString(_T("\t\t</linkLine>\n"));


			f.WriteString(_T("\t\t<linkLineColor>\n"));
			CString sc;
			COLORREF nc = (*li).getLinkColor();
			BYTE nred = GetRValue(nc);
			BYTE ngrn = GetGValue(nc);
			BYTE nblu = GetBValue(nc);

			f.WriteString(_T("\t\t\t<n_red>"));
			sc.Format(_T("%d"), nred);
			f.WriteString(sc);
			f.WriteString(_T("</n_red>\n"));

			f.WriteString(_T("\t\t\t<n_green>"));
			sc.Format(_T("%d"), ngrn);
			f.WriteString(sc);
			f.WriteString(_T("</n_green>\n"));

			f.WriteString(_T("\t\t\t<n_blue>"));
			sc.Format(_T("%d"), nblu);
			f.WriteString(sc);
			f.WriteString(_T("</n_blue>\n"));

			f.WriteString(_T("\t\t</linkLineColor>\n"));


			if ((*li).isCurved()) {
				f.WriteString(_T("\t\t<pathPt>\n"));
				CString sp; sp.Format(_T("\t\t\t<path_x>%d</path_x>\n\t\t\t<path_y>%d</path_y>\n"),
					(*li).getPtPath().x, (*li).getPtPath().y);
				f.WriteString(sp);
				f.WriteString(_T("\t\t</pathPt>\n"));
			}
		}
		f.WriteString(_T("\t</ilink>\n"));
	}

	f.WriteString(_T("</iEditDoc>\n"));
	f.Flush();
	f.Close();
	_wsetlocale(LC_ALL, _T(""));
	return true;
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
		(*it).second.setName(nodesImport[0].getName());
		(*it).second.setText(nodesImport[0].getText());
		(*it).second.setBound(nodesImport[0].getBound());
	}

	// nodeの格納
	sv.clear();
	sv.resize(0);
	for (int k = 0; sv.size() < nodesImport.size() && k < 100; k++) { // 100ループまで
		if (brepRoot) {
			i = 1;
		}
		else {
			i = 0;
		}
		for (; i < nodesImport.size(); i++) {
			niterator itp = nodes_.find(nodesImport[i].getParent());
			niterator it = nodes_.find(nodesImport[i].getKey());
			if (itp != nodes_.end() && it == nodes_.end()) {
				CRect rc = nodesImport[i].getBound();
				if (rc.left < 0 && rc.top < 0) {
					nodesImport[i].moveTo(CPoint(0, 0));
					nodesImport[i].moveBound(CSize(rand() % 800, rand() % 600));
				}
				nodes_[nodesImport[i].getKey()] = nodesImport[i];
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
	SetConnectionPoint();
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

void iEditDoc::RandomizeNodesPos(const CSize &area)
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	srand((unsigned)st.wMilliseconds);
	niterator it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if (!(*it).second.isVisible()) continue;
		(*it).second.moveTo(CPoint(0, 0));
		(*it).second.moveBound(CSize(rand() % area.cx, rand() % area.cy));
	}
	SetConnectionPoint();
	calcMaxPt(m_maxPt);
	SetModifiedFlag();
}

void iEditDoc::WriteKeyNodeToHtml(DWORD key, CStdioFile* f, bool textIsolated, const CString& textPrefix)
{
	const_niterator it = nodes_.findNode(key);

	CString nameStr = StringUtil::RemoveCr((*it).second.getName());
	// リンクタグの生成
	f->WriteString(_T("<a id="));
	f->WriteString(_T("\""));
	CString keystr;
	keystr.Format(_T("%d"), (*it).second.getKey());
	f->WriteString(keystr);
	f->WriteString(_T("\" />\n"));

	// 内容書き込み
	f->WriteString(_T("<h1>") + nameStr + _T("</h1>\n"));
	f->WriteString(_T("<div class=\"text\">\n"));
	f->WriteString(procWikiNotation((*it).second.getText()));
	f->WriteString(_T("</div>\n"));

	// リンクの書き込み
	f->WriteString(_T("<div class=\"links\">\n"));
	const_literator li = links_.begin();
	CString sLink(_T("<ul>\n"));
	int cnt = 0;
	for (; li != links_.end(); li++) {
		if ((*li).getKeyFrom() != (*it).second.getKey() &&
			(*li).getKeyTo() != (*it).second.getKey()) {
			continue;
		}
		if ((*li).getArrowStyle() != iLink::other) {
			if ((*it).second.getKey() == (*li).getKeyFrom()) {
				const_niterator itTo = nodes_.findNode((*li).getKeyTo());
				if (itTo != nodes_.end()) {
					CString keystr;
					keystr.Format(_T("%d"), (*li).getKeyTo());
					sLink += _T("<li><a href=");
					if (!textIsolated) {
						sLink += _T("\"#");
						sLink += keystr;
					}
					else {
						sLink += _T("\"") + textPrefix + keystr + _T(".html\"");
					}
					sLink += _T("\">");
					sLink += _T("▲") + StringUtil::RemoveCr((*itTo).second.getName());
					if ((*li).getName() != _T("")) {
						sLink += _T("(") + (*li).getName() + _T(")");
					}
					sLink += _T("</a></li>\n");
					cnt++;
				}
			}
			else if ((*it).second.getKey() == (*li).getKeyTo()) {
				const_niterator itFrom = nodes_.findNode((*li).getKeyFrom());
				if (itFrom != nodes_.end()) {
					CString keystr; \
						keystr.Format(_T("%d"), (*li).getKeyFrom());
					sLink += _T("<li><a href=");
					if (!textIsolated) {
						sLink += _T("\"#");
						sLink += keystr;
					}
					else {
						sLink += _T("\"") + textPrefix + keystr + _T(".html\"");
					}
					sLink += _T("\">");
					sLink += _T("▽") + StringUtil::RemoveCr((*itFrom).second.getName());
					if ((*li).getName() != "") {
						sLink += _T("(") + (*li).getName() + ")";
					}
					sLink += _T("</a></li>\n");
					cnt++;
				}
			}
		}
		else {
			CString url = (*li).getPath();
			if (url.Find(_T("http://")) == -1 && url.Find(_T("https://")) == -1 && url.Find(_T("ftp://")) == -1) {
				if (!((CiEditApp*)AfxGetApp())->m_rgsOther.bOutputFileLinksOnExport) continue;
				WCHAR drive[_MAX_DRIVE];
				WCHAR dir[_MAX_DIR];
				WCHAR fileName[_MAX_FNAME];
				WCHAR ext[_MAX_EXT];
				ZeroMemory(drive, _MAX_DRIVE);
				ZeroMemory(dir, _MAX_DIR);
				ZeroMemory(fileName, _MAX_FNAME);
				ZeroMemory(ext, _MAX_EXT);
				_wsplitpath_s((const wchar_t *)url, drive, _MAX_DRIVE, dir, _MAX_DIR, fileName, _MAX_FNAME, ext, _MAX_EXT);
				CString sDrive(drive);
				if (sDrive != "") { // フルパスの時だけ"file:///" つけとけばいいらしい
					url = _T("file:///") + url;
				}
			}
			sLink += _T("<li><a href=\"");
			sLink += url;
			sLink += _T("\" target=\"_top\">");
			if ((*li).getName() != _T("")) {
				sLink += (*li).getName();
			}
			else {
				sLink += url;
			}
			sLink += _T("</a></li>\n");
			cnt++;
		}
	}
	sLink += _T("</ul>\n");
	if (cnt > 0) {
		f->WriteString(sLink);
	}
	f->WriteString(_T("</div>\n"));
}

CString iEditDoc::procWikiNotation(const CString &text)
{
	const std::wregex h2(_T("^\\*\\s([^\\*].*)$")); //"^-.*$" "^[0-9].*$" "^\\*.*$"
	const std::wregex h3(_T("^\\*\\*\\s([^\\*].*)$"));
	const std::wregex h4(_T("^\\*\\*\\*\\s([^\\*].*)$"));
	const std::wregex l1(_T("^-\\s([^-].*)$"));
	const std::wregex l2(_T("^--\\s([^-].*)"));
	const std::wregex l3(_T("^---\\s([^-].*)"));
	const std::wregex uri(_T("^.*(https?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+).*$"));
	std::match_results<std::wstring::const_iterator> result;
	// TODO:wstring使うようにしてみたが、処理が動くか検証
	vector<CString> lines = StringUtil::GetLines(text);
	int level = 0;
	int prevLevel = 0;
	CString rtnStr;
	bool pre = false;
	for (unsigned int i = 0; i < lines.size(); i++) {
		std::wstring line = static_cast<LPCTSTR>(lines[i]);
		if (line.find(_T("<pre>")) != -1) {
			pre = true;
		}
		else if (line.find(_T("</pre>")) != -1) {
			pre = false;
		}
		if (pre) {
			rtnStr += lines[i] + _T("\n");
			continue;
		}
		if (std::regex_match(line, result, h2)) {
			endUL(rtnStr, level);
			rtnStr += _T("<h2>");
			rtnStr += makeInlineUrlLink(CString(result[1].str().c_str()));
			rtnStr += _T("</h2>\n");
		}
		else if (std::regex_match(line, result, h3)) {
			endUL(rtnStr, level);
			rtnStr += _T("<h3>");
			rtnStr += makeInlineUrlLink(CString(result[1].str().c_str()));
			rtnStr += _T("</h3>\n");
		}
		else if (std::regex_match(line, result, h4)) {
			endUL(rtnStr, level);
			rtnStr += _T("<h4>");
			rtnStr += makeInlineUrlLink(CString(result[1].str().c_str()));
			rtnStr += _T("</h4>\n");
		}
		else if (std::regex_match(line, result, l1)) {
			prevLevel = level;
			level = 1;
			beginUL(rtnStr, level, prevLevel);
			rtnStr += _T("<li>");
			rtnStr += makeInlineUrlLink(CString(result[1].str().c_str()));
			rtnStr += _T("</li>\n");
		}
		else if (std::regex_match(line, result, l2)) {
			prevLevel = level;
			level = 2;
			beginUL(rtnStr, level, prevLevel);
			rtnStr += _T("<li>");
			rtnStr += makeInlineUrlLink(CString(result[1].str().c_str()));
			rtnStr += _T("</li>\n");
		}
		else if (std::regex_match(line, result, l3)) {
			prevLevel = level;
			level = 3;
			beginUL(rtnStr, level, prevLevel);
			rtnStr += _T("<li>");
			rtnStr += makeInlineUrlLink(CString(result[1].str().c_str()));
			rtnStr += _T("</li>\n");
		}
		else {
			endUL(rtnStr, level);
			rtnStr += makeInlineUrlLink(CString(line.c_str()));
			rtnStr += _T("<br />\n");
		}
	}
	return rtnStr;
}

// インラインのURLを検出する 今のところ最初の1個のみ
CString iEditDoc::makeInlineUrlLink(const CString &line)
{
	const std::wregex uri(_T("^(.*)(https?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+)(.*)$"));
	const std::wregex wikiLink(_T("^(.*)\\[\\[(.+):(https?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+)\\]\\](.*)$"));
	std::wstring sLine = static_cast<LPCTSTR>(line);
	std::match_results<std::wstring::const_iterator> result;
	if (std::regex_match(sLine, result, wikiLink)) {
		CString rtnStr = result[1].str().c_str();
		rtnStr += _T("<a href=\"");
		rtnStr += result[3].str().c_str();
		rtnStr += _T("\" target=\"_blank\">");
		rtnStr += result[2].str().c_str();
		rtnStr += _T("</a>");
		rtnStr += result[4].str().c_str();
		return rtnStr;
	}
	else if (std::regex_match(sLine, result, uri)) {
		CString rtnStr = result[1].str().c_str();
		rtnStr += _T("<a href=\"");
		rtnStr += result[2].str().c_str();
		rtnStr += _T("\" target=\"_blank\">");
		rtnStr += result[2].str().c_str();
		rtnStr += _T("</a>");
		rtnStr += result[3].str().c_str();
		return rtnStr;
	}
	return line;
}

void iEditDoc::beginUL(CString& str, int& level, int& prevLevel)
{
	if (prevLevel == level - 1) {
		str += _T("<ul>\n");
	}
	else if (prevLevel == level - 2) {
		str += _T("<ul>\n<ul>\n");
	}
	else if (prevLevel == level + 1) {
		str += _T("</ul>\n");
	}
	else if (prevLevel == level + 2) {
		str += _T("</ul>\n</ul>\n");
	}
}

void iEditDoc::endUL(CString & str, int& level)
{
	if (level == 1) {
		str += _T("</ul>\n");
	}
	else if (level == 2) {
		str += _T("</ul>\n</ul>\n");
	}
	else if (level == 3) {
		str += _T("</ul>\n</ul>\n</ul>\n");
	}
	if (level > 0) level = 0;
}

CString iEditDoc::GetKeyNodeText(DWORD key)
{
	const_niterator it = nodes_.findNode(key);
	if (it != nodes_.end()) {
		return (*it).second.getText();
	}
	return _T("");
}

CString iEditDoc::GetKeyNodeLabel(DWORD key)
{
	const_niterator it = nodes_.findNode(key);
	if (it != nodes_.end()) {
		return (*it).second.getName();
	}
	return _T("");
}

bool iEditDoc::isKeyInLabels(const NodePropsVec &labels, DWORD key)
{
	if (labels.size() == nodes_.size()) return true;
	for (unsigned int i = 0; i < labels.size(); i++) {
		if (labels[i].key == key) {
			return true;
		}
	}
	return false;
}

void iEditDoc::ListUpNodes(const CString &sfind, NodePropsVec &labels, BOOL bLabel, BOOL bText, BOOL bLinks, BOOL bUpper)
{
	CString sf = sfind;
	if (bUpper) {
		sf.MakeUpper();
	}
	niterator it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		bool done = false;
		CString name = (*it).second.getName();
		CString text = (*it).second.getText();
		if (bUpper) {
			name.MakeUpper();
			text.MakeUpper();
		}
		if (bLabel && name.Find(sf) != -1) {
			NodeProps l;
			l.name = (*it).second.getName();
			l.key = (*it).second.getKey();
			l.state = 0;
			labels.push_back(l);
			done = true;
		}
		if (bText && text.Find(sf) != -1) {
			NodeProps l;
			l.name = (*it).second.getName();
			l.key = (*it).second.getKey();
			l.state = 1;
			labels.push_back(l);
		}
	}

	literator li = links_.begin();
	for (; li != links_.end(); li++) {
		CString name = (*li).getName();
		CString path = (*li).getPath();
		if (bUpper) {
			name.MakeUpper();
			path.MakeUpper();
		}
		if (bLinks && name.Find(sf) != -1) {
			NodeProps l;
			l.key = (*li).getKeyFrom();
			l.name = (*li).getName();
			l.state = 2;
			labels.push_back(l);
		}
		if (bLinks && path.Find(sf) != -1) {
			NodeProps l;
			l.key = (*li).getKeyFrom();
			l.name = (*li).getPath();
			l.state = 3;
			labels.push_back(l);
		}
	}
}

HENHMETAFILE iEditDoc::GetSelectedNodeMetaFile()
{
	niterator it = nodes_.getSelectedNode();
	if (it != nodes_.end()) {
		return (*it).second.getMetaFile();
	}
	return NULL;
}

void iEditDoc::DrawNodesSelected(CDC *pDC, bool bDrwAll)
{
	nodes_.drawNodesSelected(pDC);
}

void iEditDoc::DrawLinksSelected(CDC *pDC, bool bDrwAll, bool clipbrd)
{
	literator li = links_.begin();
	for (; li != links_.end(); li++) {
		niterator itFrom = nodes_.findNodeW((*li).getKeyFrom());
		niterator itTo = nodes_.findNodeW((*li).getKeyTo());
		if ((*li).getArrowStyle() != iLink::other && (*itFrom).second.isSelected() && (*itTo).second.isSelected()) {
			(*li).drawArrow(pDC);
			(*li).drawLine(pDC);
			(*li).drawComment(pDC, clipbrd);
		}
	}
}

void iEditDoc::BackupNodesForUndo()
{
	nodes_undo.clear();
	nodes_undo.resize(0);
	niterator it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if ((*it).second.isVisible()) {
			iNode n((*it).second);
			nodes_undo.push_back(n);
		}
	}
}

void iEditDoc::RestoreNodesForUndo()
{
	for (unsigned int i = 0; i < nodes_undo.size(); i++) {
		niterator it = nodes_.findNodeW(nodes_undo[i].getKey());
		if (it != nodes_.end()) {
			(*it).second = nodes_undo[i];
		}
	}
	//	SetConnectionPoint();
	calcMaxPt(m_maxPt);
	nodes_undo.clear();
	nodes_undo.resize(0);
	SetModifiedFlag();
}

bool iEditDoc::CanUndo()
{
	if (nodes_undo.size() == 0 && links_undo.size() == 0) {
		return false;
	}
	return true;
}

void iEditDoc::DisableUndo()
{
	nodes_undo.clear();
	nodes_undo.resize(0);
	links_undo.clear();
	links_undo.resize(0);
}

void iEditDoc::BackupLinksForUndo()
{
	links_undo.clear();
	links_undo.resize(0);
	literator li = links_.begin();
	for (; li != links_.end(); li++) {
		if ((*li).canDraw()) {
			iLink l = (*li);
			links_undo.push_back(l);
		}
	}
}

void iEditDoc::RestoreLinksForUndo()
{
	for (unsigned int i = 0; i < links_undo.size(); i++) {
		literator li = links_.begin();
		for (; li != links_.end(); li++) {
			if (links_undo[i].getKey() == (*li).getKey()) {
				(*li) = links_undo[i];
				break;
			}
		}
	}
	if (links_.getDividedLinkKey() != -1) {
		links_.erase(remove_if(links_.begin(), links_.end(), iLink_eqkey(links_.getDividedLinkKey())), links_.end());
		links_.clearDividedLinkKey();
	}
	calcMaxPt(m_maxPt);
	links_undo.clear();
	links_undo.resize(0);
	SetModifiedFlag();
}

void iEditDoc::AlignNodesInBoundTo(const CString& side, const CRect& rect, bool bDrwAll)
{
	niterator it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		//	if (!bDrwAll) {
		if (!(*it).second.isVisible()) continue;
		//	}
		if (!(*it).second.isSelected()) continue;
		CPoint pt;
		if (side == _T("left")) {
			pt.x = rect.left;
			pt.y = (*it).second.getBound().top;
		}
		else if (side == _T("right")) {
			pt.x = (*it).second.getBound().left + rect.right - (*it).second.getBound().right;
			pt.y = (*it).second.getBound().top;
		}
		else if (side == _T("top")) {
			pt.x = (*it).second.getBound().left;
			pt.y = rect.top;
		}
		else if (side == _T("bottom")) {
			pt.x = (*it).second.getBound().left;
			pt.y = (*it).second.getBound().top + rect.bottom - (*it).second.getBound().bottom;
		}

		(*it).second.moveTo(pt);
	}
	SetConnectionPoint();
	calcMaxPt(m_maxPt);
	SetModifiedFlag();
}

void iEditDoc::AlignSelectedNodesToSameSize(const CString &strSize, bool bDrwAll)
{
	CSize maxSz = nodes_.getMaxNodeSize(true, bDrwAll);
	niterator it = nodes_.begin();
	CRect rc;
	for (; it != nodes_.end(); it++) {
		//		if (!bDrwAll) {
		if (!(*it).second.isVisible()) continue;
		//		}
		if (!(*it).second.isSelected()) continue;
		rc = (*it).second.getBound();
		if (strSize == _T("height")) {
			rc.bottom = rc.top + maxSz.cy;
		}
		else if (strSize == _T("width")) {
			rc.right = rc.left + maxSz.cx;
		}
		else if (strSize == _T("rect")) {
			rc.bottom = rc.top + maxSz.cy;
			rc.right = rc.left + maxSz.cx;
		}
		(*it).second.setBound(rc);
	}
	SetConnectionPoint();
	calcMaxPt(m_maxPt);
	SetModifiedFlag();
}

const iLink* iEditDoc::GetSelectedLink(bool bDrawAll) const
{
	const_literator li = links_.getSelectedLink();
	if (li == links_.end()) {
		return NULL;
	}
	return &(*li);
}

void iEditDoc::ReverseSelectedLinkDirection(bool bDrwAll)
{
	const iLink* pl = GetSelectedLink(bDrwAll);
	if (pl == NULL) return;
	DWORD keyTo = pl->getKeyTo();
	links_.setSelectedLinkReverse();
	SetModifiedFlag();
	selChanged(keyTo, true, ShowSubBranch());
	iHint h; h.event = iHint::linkModified;
	UpdateAllViews(NULL, (LPARAM)getSelectedNodeKey(), &h);
}

void iEditDoc::ViewSettingChanged()
{
	iHint hint; hint.event = iHint::viewSettingChanged;
	DWORD key = nodes_.getSelKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::ExportSvg(bool bDrwAll, const CString &path, bool bEmbed,
	const CString& textFileName, bool textSingle)
{
	NodeKeyVec vec = getOutlineView()->getDrawOrder(ShowSubBranch());
	SvgWriter writer(nodes_, links_, vec, bDrwAll);
	if (textSingle) {
		writer.setTextHtmlFileName(textFileName);
	}
	else {
		writer.setTextHtmlFilePrefix(textFileName);
	}
	writer.exportSVG(path, getMaxPt(), bEmbed);
}

iNode iEditDoc::GetHitNode(const CPoint &pt, bool bDrwAll)
{
	iNode* pNode = nodes_.hitTest2(pt, bDrwAll);
	if (pNode != NULL) {
		iNode node = *pNode;
		return node;
	}
	else {
		iNode nil;
		return nil;
	}
}

void iEditDoc::SetVisibleNodes(NodeKeySet& keySet)
{
	m_visibleKeys = keySet;
	nodes_.setVisibleNodes(keySet);
}

void iEditDoc::SetShowBranch(DWORD branchRootKey)
{
	m_dwBranchRootKey = branchRootKey;
	m_bShowBranch = true;
	iHint hint; hint.event = iHint::showSubBranch;
	DWORD key = nodes_.getSelKey();
	calcMaxPt(m_maxPt);
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::ResetShowBranch()
{
	m_bShowBranch = false;
	nodes_.setVisibleNodes(nodes_.getCurParent());
	iHint hint; hint.event = iHint::resetShowSubBranch;
	DWORD key = nodes_.getSelKey();
	calcMaxPt(m_maxPt);
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

CString iEditDoc::GetSubBranchRootLabel() const
{
	const_niterator n = nodes_.findNode(m_dwBranchRootKey);
	if (n != nodes_.end()) {
		return (*n).second.getName();
	}
	return _T("");
}

bool iEditDoc::ShowSubBranch() const
{
	return m_bShowBranch;
}

bool iEditDoc::IsOldBinary() const
{
	return m_bOldBinary;
}

bool iEditDoc::CurKeyInBranch() const
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

	WCHAR drive[_MAX_DRIVE];
	WCHAR dir[_MAX_DIR];
	WCHAR fname[_MAX_FNAME];
	WCHAR ext[_MAX_EXT];
	ZeroMemory(drive, _MAX_DRIVE);
	ZeroMemory(dir, _MAX_DIR);
	ZeroMemory(fname, _MAX_FNAME);
	ZeroMemory(ext, _MAX_EXT);
	_wsplitpath_s((const wchar_t *)fullPath, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	fileName = fname;
	CString driveName = drive;
	if (driveName == _T("")) {
		const_niterator it = nodes_.find(0);
		CString rootLabel = (*it).second.getName();
		if (rootLabel != _T("主題")) {
			// TODO:ファイル名として不正な文字の除去
			CString safeFileName = StringUtil::GetSafeFileName(rootLabel);
			if (safeFileName == "") {
				fileName = GetTitle();
			}
			else {
				fileName = safeFileName;
			}
		}
		else {
			fileName = GetTitle();
		}
	}

	CString szFilter = _T("iEditファイル(*.iedx)|*.iedx|iEditファイル(旧)(*.ied)|*.ied|XMLファイル(*.xml)|*xml||");
	if (!((CiEditApp*)AfxGetApp())->m_rgsOptions.registOldFiletype) {
		szFilter = _T("iEditファイル(*.iedx)|*.iedx|XMLファイル(*.xml)|*xml||");
	}
	CFileDialog cfDlg(FALSE, NULL, fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |
		OFN_FILEMUSTEXIST | OFN_EXPLORER, szFilter, AfxGetMainWnd());
	if (cfDlg.DoModal() == IDOK) {

		DWORD index = cfDlg.m_ofn.nFilterIndex;
		CString pathName = cfDlg.GetPathName();
		CString ext = cfDlg.GetFileExt();
		ext.MakeLower();

		CString extension;
		switch (index) {
		case 1: // iedx
			extension = _T(".iedx");
			break;
		case 2: // ied
			extension = _T(".ied");
			break;
		case 3: // xml
			extension = _T(".xml");
			SetModifiedFlag(FALSE);
			break;
		}
		if (ext != _T("iedx") && ext != _T("ied") && ext != _T("xml")) {
			OnSaveDocument(pathName + extension);
			SetPathName(cfDlg.GetPathName() + extension);
		}
		else if (ext == _T("iedx") || ext == _T("ied") || ext == _T("xml")) {
			OnSaveDocument(pathName);
			SetPathName(cfDlg.GetPathName());
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

	WCHAR drive[_MAX_DRIVE];
	WCHAR dir[_MAX_DIR];
	WCHAR fname[_MAX_FNAME];
	WCHAR ext[_MAX_EXT];
	ZeroMemory(drive, _MAX_DRIVE);
	ZeroMemory(dir, _MAX_DIR);
	ZeroMemory(fname, _MAX_FNAME);
	ZeroMemory(ext, _MAX_EXT);
	_wsplitpath_s((const wchar_t *)fullPath, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

	driveName = drive;
	dirName = dir;
	fileName = fname;
	extName = ext;

	if (driveName == _T("")) {
		OnFileSaveAs();
	}
	else {
		OnSaveDocument(fullPath);
	}
}

void iEditDoc::OnUpdateFileSave(CCmdUI *pCmdUI)
{
	// TODO: ここにコマンド更新 UI ハンドラ コードを追加します。
}

CRect iEditDoc::RestoreDeleteBound() const
{
	return m_deleteBound;
}

void iEditDoc::BackupDeleteBound()
{
	m_deleteBound = getRelatedBound(false);
}

void iEditDoc::SetNodeLevel(const DWORD key, const int nLevel)
{
	niterator it = nodes_.findNodeW(key);
	if (it == nodes_.end()) {
		return;
	}
	(*it).second.setLevel(nLevel);
}


// 芋蔓に必要なノード間距離を計算する
// iNodeのBoundPreも初期化する多機能メソッド←ダメ
void iEditDoc::CalcEdges()
{
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();

	literator li = links_.begin();
	for (; li != links_.end(); li++) {
		if ((*li).getArrowStyle() != iLink::other) {
			// 距離計算
			if ((*li).getKeyFrom() == (*li).getKeyTo()) continue;
			if (!(*li).canDraw() || !(*li).isInChain()) {
				(*li).setLen(-1.0);
			}
			else {
				CSize sz;
				sz.cx = ((*li).getRectFrom().Width() + (*li).getRectTo().Width()) / 2;
				sz.cy = ((*li).getRectFrom().Height() + (*li).getRectTo().Height()) / 2;
				double rate;
				if (pApp->m_rgsLink.bSetStrength) {
					rate = width2Len((*li).getLineWidth());
					if ((*li).getLineStyle() == PS_DOT) {
						rate *= 1.5;
					}
				}
				else {
					rate = width2Len(0);
				}
				rate *= (((double)pApp->m_rgsLink.strength) / 10.0);
				(*li).setLen(sqrt((double)(sz.cx*sz.cx + sz.cy*sz.cy)) * 5 / 4 * rate);
			}
			// preBoundの値を初期化
			if (!(*li).isInChain()) continue;
			niterator itFrom = nodes_.findNodeW((*li).getKeyFrom());
			(*itFrom).second.setBoundPre((*itFrom).second.getBound());
			(*itFrom).second.dx = 0.0;
			(*itFrom).second.dy = 0.0;

			niterator itTo = nodes_.findNodeW((*li).getKeyTo());
			(*itTo).second.setBoundPre((*itTo).second.getBound());
			(*itTo).second.dx = 0.0;
			(*itTo).second.dy = 0.0;
		}
	}
}

void iEditDoc::RelaxSingleStep(const CPoint &point, const CPoint& dragOffset)
{
	// ドラッグ中のノードの位置を変更
	niterator ni = nodes_.begin();
	for (; ni != nodes_.end(); ni++) {
		if ((*ni).second.isSelected()) {
			CRect rc = (*ni).second.getBound();
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

			(*ni).second.setBoundPre((*ni).second.getBoundPre());
			(*ni).second.setBound(rc);
			break;
		}
	}

	////// ばねモデル処理
	literator li = links_.begin();
	for (; li != links_.end(); li++) {
		if ((*li).getArrowStyle() == iLink::other ||
			(*li).getKeyFrom() == (*li).getKeyTo() ||
			!(*li).canDraw()) {
			continue;
		}
		if (!(*li).isInChain()) continue;

		niterator itFrom = nodes_.findNodeW((*li).getKeyFrom());
		niterator itTo = nodes_.findNodeW((*li).getKeyTo());

		double gxto = (*itTo).second.getBoundPre().CenterPoint().x;
		double gyto = (*itTo).second.getBoundPre().CenterPoint().y;
		double gxfrom = (*itFrom).second.getBoundPre().CenterPoint().x;
		double gyfrom = (*itFrom).second.getBoundPre().CenterPoint().y;

		double vx = gxto - gxfrom;
		double vy = gyto - gyfrom;
		double len = sqrt(vx*vx + vy * vy);
		len = (len == 0) ? .0001 : len;
		double f = ((*li).getLen() - len) / (len * 3);
		double dx = f * vx;
		double dy = f * vy;
		/*const_cast<iNode&>*/(*itTo).second.dx += dx;
		/*const_cast<iNode&>*/(*itTo).second.dy += dy;
		/*const_cast<iNode&>*/(*itFrom).second.dx -= dx;
		/*const_cast<iNode&>*/(*itFrom).second.dy -= dy;
	}

	srand((unsigned)time(NULL));
	niterator nit1 = nodes_.begin();
	for (; nit1 != nodes_.end(); nit1++) {
		if (!(*nit1).second.isVisible()) continue;
		if (!(*nit1).second.isInChain()) continue;

		double dx = 0; double dy = 0;
		niterator nit2 = nodes_.begin();
		for (; nit2 != nodes_.end(); nit2++) {
			if (nit1 == nit2) { continue; }
			if (!(*nit2).second.isVisible()) continue;
			if (!(*nit2).second.isInChain()) continue;

			double gx1 = (*nit1).second.getBoundPre().CenterPoint().x;
			double gy1 = (*nit1).second.getBoundPre().CenterPoint().y;
			double gx2 = (*nit2).second.getBoundPre().CenterPoint().x;
			double gy2 = (*nit2).second.getBoundPre().CenterPoint().y;

			double vx = gx1 - gx2;
			double vy = gy1 - gy2;
			double len = vx * vx + vy * vy;
			if (len == 0) {
				dx += ((double)rand()) / (double)RAND_MAX;
				dy += ((double)rand()) / (double)RAND_MAX;
			}
			else /*if (len < 100*100)*/ {
				dx += vx / len;
				dy += vy / len;
			}
		}
		double dlen = dx * dx + dy * dy;
		if (dlen > 0) {
			dlen = sqrt(dlen) / 2;
			/*const_cast<iNode&>*/(*nit1).second.dx += dx / dlen;
			/*const_cast<iNode&>*/(*nit1).second.dy += dy / dlen;
		}
	}

	niterator nit = nodes_.begin();
	for (; nit != nodes_.end(); nit++) {
		if (!(*nit).second.isVisible()) continue;
		if (!(*nit).second.isInChain()) continue;
		if ((*nit).second.isSelected()) continue;
		double x = max(-5, min(5, (*nit).second.dx));
		double y = max(-5, min(5, (*nit).second.dy));
		CRect rc = (*nit).second.getBoundPre();
		rc.OffsetRect((int)x, (int)y);
		// 領域のチェック
		if (rc.left < 0) {
			rc.right = (*nit).second.getBoundPre().Width();
			rc.left = 0;
		}
		if (rc.top < 0) {
			rc.bottom = (*nit).second.getBoundPre().Height();
			rc.top = 0;
		}
		if (!(*nit).second.isFixed() && !(*nit).second.isSelected()) {
			(*nit).second.setBound(rc);
		}
		(*nit).second.dx /= 2;
		(*nit).second.dy /= 2;
	}

	niterator it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if (!(*it).second.isVisible()) continue;
		if (!(*it).second.isInChain()) continue;
		(*it).second.setBoundPre((*it).second.getBound());
	}
	setConnectPoint2();
}

/// 芋づる式に関連ノード・リンクにフラグを立てる
void iEditDoc::ListupChainNodes(bool bResetLinkCurve)
{
	// 直前までのフラグをクリア
	niterator nit = nodes_.begin();
	for (; nit != nodes_.end(); nit++) {
		(*nit).second.setInChain(false);
	}
	literator linit = links_.begin();
	for (; linit != links_.end(); linit++) {
		(*linit).setInChain(false);
	}

	// links_から芋づる検索
	NodeKeySet nodeChain; // 新しい芋用
	NodeKeySet nodeChainChecked; // 掘った芋用
	nodeChain.insert(getSelectedNodeKey()); // selectされている芋
	unsigned int sizePre = nodeChain.size();
	for (; ; ) {
		NodeKeySet::iterator ki = nodeChain.begin();
		for (; ki != nodeChain.end(); ki++) {
			if (nodeChainChecked.find(*ki) != nodeChainChecked.end()) {
				continue;
			}
			literator li = links_.begin();
			for (; li != links_.end(); li++) {
				if ((*li).isTerminalNodeKey(*ki)) {
					if (!(*li).canDraw()) continue;
					if ((*li).getArrowStyle() == iLink::other) continue;
					if ((*li).getKeyFrom() == (*li).getKeyTo()) continue;
					DWORD pairKey;
					if ((*li).getKeyFrom() == (*ki)) {
						pairKey = (*li).getKeyTo();
					}
					else if ((*li).getKeyTo() == (*ki)) {
						pairKey = (*li).getKeyFrom();
					}
					nodeChain.insert(pairKey);
					(*li).setInChain();
					if (bResetLinkCurve) {
						(*li).curve(false);
					}
					iNode nodeFind;
					niterator nf = nodes_.find(pairKey);
					if (nf != nodes_.end()) {
						(*nf).second.setInChain();
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

CRect iEditDoc::GetChaindNodesBound() const
{
	CRect rc = getSelectedNodeRect();
	const_niterator it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if ((*it).second.isVisible() && (*it).second.isInChain()) {
			rc |= (*it).second.getBound();
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
	for (; li != links_.end(); li++) {
		if (!(*li).isInChain()) continue;

		niterator itFrom = nodes_.findNodeW((*li).getKeyFrom());
		niterator itTo = nodes_.findNodeW((*li).getKeyTo());
		if (itFrom == nodes_.end() || itTo == nodes_.end()) continue;

		if ((*li).getKeyFrom() == (*itFrom).second.getKey()) {
			(*li).setRFrom((*itFrom).second.getBound());
		}
		if ((*li).getKeyTo() == (*itTo).second.getKey()) {
			(*li).setRTo((*itTo).second.getBound());
		}
	}
}

void iEditDoc::RecalcArea()
{
	calcMaxPt(m_maxPt);
}

template <class T>
void iEditDoc::outStyleSheetLine(T &f)
{
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	if (!pApp->m_rgsOther.bSetStylesheet) return;
	CString strStylesheetFile = pApp->m_rgsOther.strStyleSheetFile;
	if (strStylesheetFile == _T("")) {
		strStylesheetFile = _T("iedit.xsl");
	}
	CString s = _T("<?xml-stylesheet type=\"text/xsl\" ");
	s += _T("href=\"") + strStylesheetFile + _T("\"") + _T(" ?>\n");
	//	f.WriteString("<?xml-stylesheet type=\"text/xsl\" href=\"iedit.xsl\" ?>\n");
	f.WriteString(s);
}

const CRect iEditDoc::AddNodeWithLink(int nodeType, DWORD keyRoot, DWORD prevSibling, const CPoint & pt, bool bMindmap)
{
	niterator it = nodes_.find(keyRoot);
	if (it == nodes_.end()) return CRect(0, 0, 0, 0);

	iNode nwNode;
	if (bMindmap) {
		CPoint ptTarget = (*it).second.getBound().CenterPoint() + CPoint(100, -100);
		nwNode = InsertNode(nodeType, _T("ノード"), ptTarget);
	}
	else {
		nwNode = InsertNode(nodeType, _T("ノード"), pt);
	}

	DWORD newKey = nwNode.getKey();

	selChanged(nwNode.getKey(), true, ShowSubBranch());
	SetModifiedFlag();

	iHint hint;
	hint.event = iHint::rectAdd;
	hint.str = nwNode.getName();
	UpdateAllViews(NULL, (LPARAM)nwNode.getKey(), &hint);

	iLink l;
	l.setNodes((*it).second.getBound(), nwNode.getBound(), (*it).second.getKey(), nwNode.getKey());
	l.setDrawFlag();
	l.setKey(lastLinkKey++);
	desideLinkLineStyle(l);
	desideLinkArrow(l);
	links_.push_back(l);


	if (bMindmap) {
		ListupChainNodes(false);
		CalcEdges();
		nodes_.fixNodesReversibly(newKey); // 新しいノード以外を固定
		for (int i = 0; i < 100; i++) {
			RelaxSingleStep2();
		}
		nodes_.restoreNodesFixState(newKey); // Fix状態をリストア
	}

	selChanged(nwNode.getKey(), true, ShowSubBranch());

	niterator nit = nodes_.find(nwNode.getKey());
	return (*nit).second.getBound();
}

const CRect iEditDoc::AddNodeWithLink2(int nodeType, DWORD keyPrevSibling)
{
	if (links_.isIsolated(nodes_.getSelKey(), false)) return CRect(0, 0, 0, 0);
	DWORD pairKey = links_.getFirstVisiblePair(nodes_.getSelKey());
	if (pairKey == -1) return CRect(0, 0, 0, 0);

	niterator itRoot = nodes_.find(pairKey);
	if (itRoot == nodes_.end()) return CRect(0, 0, 0, 0);

	niterator itSibling = nodes_.find(keyPrevSibling);
	if (itSibling == nodes_.end()) return CRect(0, 0, 0, 0);

	CPoint ptRoot = (*itRoot).second.getBound().CenterPoint();
	CPoint ptSibling = (*itSibling).second.getBound().CenterPoint();
	CPoint ptTarget = ptSibling;
	CPoint ptOffset = ptSibling - ptRoot;

	// rootとsiblingの位置関係によって、初期の出現位置を変える
	if (ptOffset.x >= 0 && ptOffset.y >= 0) {
		ptTarget += CPoint(-50, 50);
	}
	else if (ptOffset.x < 0 && ptOffset.y >= 0) {
		ptTarget += CPoint(-50, -50);
	}
	else if (ptOffset.x < 0 && ptOffset.y < 0) {
		ptTarget += CPoint(50, -50);
	}
	else if (ptOffset.x >= 0 && ptOffset.y < 0) {
		ptTarget += CPoint(50, 50);
	}

	iNode nwNode = InsertNode(nodeType, _T("ノード"), ptTarget);
	DWORD newKey = nwNode.getKey();

	selChanged(nwNode.getKey(), true, ShowSubBranch());
	SetModifiedFlag();

	iHint hint;
	hint.event = iHint::rectAdd;
	hint.str = nwNode.getName();
	UpdateAllViews(NULL, (LPARAM)nwNode.getKey(), &hint);

	iLink l;
	l.setNodes((*itRoot).second.getBound(), nwNode.getBound(), (*itRoot).second.getKey(), nwNode.getKey());
	l.setDrawFlag();
	l.setKey(lastLinkKey++);
	desideLinkLineStyle(l);
	desideLinkArrow(l);

	links_.push_back(l);

	ListupChainNodes(false);
	CalcEdges();

	nodes_.fixNodesReversibly(newKey);
	for (int i = 0; i < 100; i++) {
		RelaxSingleStep2();
	}
	nodes_.restoreNodesFixState(newKey);
	selChanged(nwNode.getKey(), true, ShowSubBranch());

	niterator nit = nodes_.find(nwNode.getKey());
	return (*nit).second.getBound();
}

// ACTION : addNodeXXを一元化するメソッド(今後リファクタリング)
// Notifyをオプショナルにするか？？
const iNode& iEditDoc::InsertNode(const int nodeType, const CString &name, const CPoint &pt)
{
	iNode n(name);
	n.setKey(getUniqKey());
	n.setParent(nodes_.getCurParent());
	n.setNodeShape(nodeType);
	n.moveBound(CSize(pt.x, pt.y));
	n.setVisible();
	nodes_[n.getKey()] = n;

	if (ShowSubBranch()) {
		m_visibleKeys.insert(n.getKey());
	}
	const_niterator nit = nodes_.find(n.getKey());
	return ((*nit).second);
}

void iEditDoc::RelaxSingleStep2()
{
	////// ばねモデル処理
	literator li = links_.begin();
	for (; li != links_.end(); li++) {
		if ((*li).getArrowStyle() == iLink::other ||
			(*li).getKeyFrom() == (*li).getKeyTo() ||
			!(*li).canDraw()) {
			continue;
		}
		if (!(*li).isInChain()) continue;

		niterator itFrom = nodes_.findNodeW((*li).getKeyFrom());
		niterator itTo = nodes_.findNodeW((*li).getKeyTo());

		double gxto = (*itTo).second.getBoundPre().CenterPoint().x;
		double gyto = (*itTo).second.getBoundPre().CenterPoint().y;
		double gxfrom = (*itFrom).second.getBoundPre().CenterPoint().x;
		double gyfrom = (*itFrom).second.getBoundPre().CenterPoint().y;

		double vx = gxto - gxfrom;
		double vy = gyto - gyfrom;
		double len = sqrt(vx*vx + vy * vy);
		len = (len == 0) ? .0001 : len;
		double f = ((*li).getLen() - len) / (len * 3);
		double dx = f * vx;
		double dy = f * vy;
		(*itTo).second.dx += dx;
		(*itTo).second.dy += dy;
		(*itFrom).second.dx -= dx;
		(*itFrom).second.dy -= dy;
	}

	srand((unsigned)time(NULL));
	niterator nit1 = nodes_.begin();
	for (; nit1 != nodes_.end(); nit1++) {
		if (!(*nit1).second.isVisible()) continue;
		if (!(*nit1).second.isInChain()) continue;

		double dx = 0; double dy = 0;
		niterator nit2 = nodes_.begin();
		for (; nit2 != nodes_.end(); nit2++) {
			if (nit1 == nit2) { continue; }
			if (!(*nit2).second.isVisible()) continue;
			if (!(*nit2).second.isInChain()) continue;

			double gx1 = (*nit1).second.getBoundPre().CenterPoint().x;
			double gy1 = (*nit1).second.getBoundPre().CenterPoint().y;
			double gx2 = (*nit2).second.getBoundPre().CenterPoint().x;
			double gy2 = (*nit2).second.getBoundPre().CenterPoint().y;

			double vx = gx1 - gx2;
			double vy = gy1 - gy2;
			double len = vx * vx + vy * vy;
			if (len == 0) {
				dx += ((double)rand()) / (double)RAND_MAX;
				dy += ((double)rand()) / (double)RAND_MAX;
			}
			else /*if (len < 100*100)*/ {
				dx += vx / len;
				dy += vy / len;
			}
		}
		double dlen = dx * dx + dy * dy;
		if (dlen > 0) {
			dlen = sqrt(dlen) / 2;
			/*const_cast<iNode&>*/(*nit1).second.dx += dx / dlen;
			/*const_cast<iNode&>*/(*nit1).second.dy += dy / dlen;
		}
	}

	niterator nit = nodes_.begin();
	for (; nit != nodes_.end(); nit++) {
		if (!(*nit).second.isVisible()) continue;
		if (!(*nit).second.isInChain()) continue;
		double x = max(-5, min(5, (*nit).second.dx));
		double y = max(-5, min(5, (*nit).second.dy));
		CRect rc = (*nit).second.getBoundPre();
		rc.OffsetRect((int)x, (int)y);
		// 領域のチェック
		if (rc.left < 0) {
			rc.right = (*nit).second.getBoundPre().Width();
			rc.left = 0;
		}
		if (rc.top < 0) {
			rc.bottom = (*nit).second.getBoundPre().Height();
			rc.top = 0;
		}
		if (!(*nit).second.isFixed()) {
			/*const_cast<iNode&>*/(*nit).second.setBound(rc);
		}
		/*const_cast<iNode&>*/(*nit).second.dx /= 2;
		/*const_cast<iNode&>*/(*nit).second.dy /= 2;
	}

	niterator it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if (!(*it).second.isVisible()) continue;
		if (!(*it).second.isInChain()) continue;
		/*const_cast<iNode&>*/(*it).second.setBoundPre((*it).second.getBound());
	}
	setConnectPoint2();
}

int iEditDoc::GetKeyNodeLevelNumber(DWORD key)
{
	const_niterator it = nodes_.findNode(key);
	if (it != nodes_.end()) {
		return (*it).second.getLevel();
	}
	return -1;
}

// サイレントなHitTest
bool iEditDoc::HitTest2(const CPoint& pt)
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
	switch (lineStyle) {
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
	l.setArrowStyle(GetAppLinkArrow());
}

int iEditDoc::GetAppLinkArrow() const
{
	int linkArrow = ((CiEditApp*)AfxGetApp())->m_curLinkArrow;
	switch (linkArrow) {
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

int iEditDoc::GetAppLinkWidth() const
{
	int linkStyle = ((CiEditApp*)AfxGetApp())->m_curLinkLineStyle;
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


void iEditDoc::ResizeSelectedNodeFont(bool bEnLarge)
{
	BackupNodesForUndo();
	BackupLinksForUndo();
	nodes_.resizeSelectedNodeFont(bEnLarge);
	setConnectPoint3();
	calcMaxPt(m_maxPt);
	SetModifiedFlag();
	iHint hint;
	hint.event = iHint::nodeFontResize;
	UpdateAllViews(NULL, (LPARAM)(nodes_.getSelKey()), &hint);
}

void iEditDoc::ResizeSelectedLinkFont(bool bEnLarge)
{
	BackupNodesForUndo();
	BackupLinksForUndo();
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
	for (; li != links_.end(); li++) {
		if (!(*li).canDraw()) continue;
		niterator itFrom = nodes_.findNodeW((*li).getKeyFrom());
		niterator itTo = nodes_.findNodeW((*li).getKeyTo());
		if (itFrom == nodes_.end() || itTo == nodes_.end()) continue;

		if ((*li).getKeyFrom() == (*itFrom).second.getKey()) {
			(*li).setRFrom((*itFrom).second.getBound());
		}
		if ((*li).getKeyTo() == (*itTo).second.getKey()) {
			(*li).setRTo((*itTo).second.getBound());
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

void iEditDoc::SetDrawOrderInfo(bool bSetDrawOrderInfo)
{
	nodes_.m_bDrawOrderInfo = !nodes_.m_bDrawOrderInfo;
}

BOOL iEditDoc::DrawOrderInfo() const
{
	return nodes_.m_bDrawOrderInfo;
}

void iEditDoc::MigrateGroup()
{
	niterator it = nodes_.getSelectedNode();
	CRect r = (*it).second.getBound();
	niterator itr = nodes_.begin();
	int drawOrder = 0;
	DWORD key = -1;
	for (; itr != nodes_.end(); itr++) {
		if (!(*itr).second.isVisible()) continue;
		CRect bound = (*itr).second.getBound();
		BOOL bInBound = bound.PtInRect(r.TopLeft()) &&
			bound.PtInRect(r.BottomRight());
		if (bInBound) {
			if (drawOrder < (*itr).second.getDrawOrder()) {
				key = (*itr).second.getKey();
				drawOrder = (*itr).second.getDrawOrder();
			}
		}
	}
	iHint hint; hint.event = iHint::groupMigrate;
	hint.keyTarget = key;
	UpdateAllViews(NULL, (LPARAM)(*it).second.getKey(), &hint);
}

void iEditDoc::SaveSelectedNodeFormat()
{
	niterator n = nodes_.getSelectedNode();
	m_nodeForFormat = iNode((*n).second);
}

void iEditDoc::ApplyFormatToSelectedNode()
{
	BackupNodesForUndo();
	niterator n = nodes_.getSelectedNode();
	(*n).second.setLineColor(m_nodeForFormat.getLineColor());
	(*n).second.setLineColor(m_nodeForFormat.getLineColor());
	(*n).second.setLineStyle(m_nodeForFormat.getLineStyle());
	(*n).second.setBrush(m_nodeForFormat.getBrsColor());
	(*n).second.setNoBrush(m_nodeForFormat.isFilled());
	(*n).second.setTextStyle(m_nodeForFormat.getTextStyle());
	(*n).second.setFontColor(m_nodeForFormat.getFontColor());
	(*n).second.setFontInfo(m_nodeForFormat.getFontInfo());
	if (m_nodeForFormat.getNodeShape() == iNode::MetaFile) {
		HENHMETAFILE hMetaFile = m_nodeForFormat.getMetaFile();
		(*n).second.setMetaFile(hMetaFile);
	}
	(*n).second.setNodeShape(m_nodeForFormat.getNodeShape());
	(*n).second.setMarginL(m_nodeForFormat.getMarginL());
	(*n).second.setMarginR(m_nodeForFormat.getMarginR());
	(*n).second.setMarginT(m_nodeForFormat.getMarginT());
	(*n).second.setMarginB(m_nodeForFormat.getMarginB());
	SetConnectionPoint();
	calcMaxPt(m_maxPt);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &hint);
}

void iEditDoc::SaveSelectedLinkFormat()
{
	const_literator l = links_.getSelectedLink();
	m_linkForFormat.setArrowStyle((*l).getArrowStyle());
	m_linkForFormat.setLineStyle((*l).getLineStyle());
	m_linkForFormat.setLineWidth((*l).getLineWidth());
	m_linkForFormat.setLinkColor((*l).getLinkColor());
}

void iEditDoc::ApplyFormatToSelectedLink()
{
	BackupLinksForUndo();
	literator l = links_.getSelectedLinkW();
	(*l).setArrowStyle(m_linkForFormat.getArrowStyle());
	(*l).setLineStyle(m_linkForFormat.getLineStyle());
	(*l).setLineWidth(m_linkForFormat.getLineWidth());
	(*l).setLinkColor(m_linkForFormat.getLinkColor());
}

void iEditDoc::DeleteLinksInBound(const CRect& bound)
{
	links_.erase(remove_if(links_.begin(), links_.end(),
		iLink_inBound(bound)), links_.end());
	SetModifiedFlag();
	iHint h; h.event = iHint::linkDeleteMulti;
	UpdateAllViews(NULL, (LPARAM)nodes_.getSelKey(), &h);
}

DWORD iEditDoc::DuplicateKeyNode(DWORD key)
{
	const_niterator it = nodes_.findNode(key);
	iNode n = (*it).second;
	n.setKey(getUniqKey());
	nodes_[n.getKey()] = n;
	return n.getKey();
}

void iEditDoc::DuplicateLinks(const NodeKeyMap& idm)
{
	NodeKeyMap::const_iterator it = idm.begin();
	for (; it != idm.end(); it++) {
		literator li = links_.begin();
		for (; li != links_.end(); li++) {
			if ((*it).first == (*li).getKeyFrom()) {
				NodeKeyMap::const_iterator pr = idm.find((*li).getKeyTo());
				if (pr != idm.end()) {
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

int iEditDoc::GetSerialVersion() const
{
	return m_serialVersion;
}

void iEditDoc::GetSelectedNodeMargin(int& l, int & r, int& t, int& b) const
{
	const_niterator nit = nodes_.getSelectedNodeR();
	if (nit != nodes_.end()) {
		l = (*nit).second.getMarginL();
		r = (*nit).second.getMarginR();
		t = (*nit).second.getMarginT();
		b = (*nit).second.getMarginB();
	}
}

void iEditDoc::SwapLinkOrder(DWORD key1, DWORD key2)
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

void iEditDoc::SetSelectedNodeMargin(int l, int r, int t, int b)
{
	BackupNodesForUndo();
	nodes_.setSelectedNodeMargin(l, r, t, b);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.getSelKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

CString iEditDoc::GetFileNameFromPath() const
{
	CString fullPath = GetPathName();
	WCHAR drive[_MAX_DRIVE];
	WCHAR dir[_MAX_DIR];
	WCHAR fileName[_MAX_FNAME];
	WCHAR ext[_MAX_EXT];
	ZeroMemory(drive, _MAX_DRIVE);
	ZeroMemory(dir, _MAX_DIR);
	ZeroMemory(fileName, _MAX_FNAME);
	ZeroMemory(ext, _MAX_EXT);
	_wsplitpath_s((const wchar_t *)fullPath, drive, _MAX_DRIVE, dir, _MAX_DIR, fileName, _MAX_FNAME, ext, _MAX_EXT);
	return CString(fileName);
}

bool iEditDoc::SaveCurrentImage(const CString& pngPath)
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
	image.Save(pngPath, Gdiplus::ImageFormatPNG);
	return true;
}

bool iEditDoc::WriteClickableMap(CStdioFile& f, const CString& textFileName, bool singleText)
{
	f.WriteString(nodes_.createClickableMapString(textFileName, singleText));
	return true;
}

void iEditDoc::FitSelectedNodeSize()
{
	BackupNodesForUndo();
	BackupLinksForUndo();
	niterator it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if ((*it).second.isSelected()) {
			(*it).second.fitSize();
		}
	}
	SetModifiedFlag();
	SetConnectionPoint();
	calcMaxPt(m_maxPt);
	iHint hint; hint.event = iHint::reflesh;
	DWORD key = nodes_.getSelKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

int iEditDoc::GetSelectedNodeScrollPos() const
{
	const_niterator it = nodes_.getSelectedNodeR();
	if (it != nodes_.end()) {
		return (*it).second.getScrollPos();
	}
	return 0;
}

void iEditDoc::SetSelectedNodeScrollPos(int pos)
{
	niterator it = nodes_.getSelectedNode();
	if (it != nodes_.end()) {
		(*it).second.setScrollPos(pos);
	}
}

void iEditDoc::DivideTargetLink(DWORD key)
{
	links_.divideTargetLinks(key, lastLinkKey++);
	SetConnectionPoint();
	calcMaxPt(m_maxPt);
	SetModifiedFlag();
}

void iEditDoc::SetSelectedNodeDragging(bool dragging)
{
	nodes_.setSelectedLinkDragging(dragging);
}