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
#include "StringUtil.h"
#include "MarkdownParser.h"
#include "XmlProcessor.h"
#include "HtmlWriter.h"
#include "FileUtil.h"
#include "JsonProcessor.h"

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
	bool operator()(const iNode& n) const { return n.GetKey() == key_; }
};


/// iLink のPredicate リンクの道連れ削除判定
class iLink_eq : public unary_function<iLink, bool>
{
	DWORD key_;  // 削除されるnodeのキー
public:
	explicit iLink_eq(DWORD key) : key_(key) { }
	bool operator()(const iLink& l) const {
		return (l.GetFromNodeKey() == key_ || l.GetToNodeKey() == key_);
	}
};

/// iLink のキーが同一かを判定
class iLink_eqkey : public unary_function<iLink, bool>
{
	DWORD key_;  // 削除されるnodeのキー
public:
	explicit iLink_eqkey(DWORD key) : key_(key) { }
	bool operator()(const iLink& l) const {
		return (l.GetKey() == key_);
	}
};

/// iLink のPredicate リンクが矩形内にあるかどうかを判定
class iLink_inBound : public unary_function<iLink, CRect>
{
	CRect bound_;
public:
	explicit iLink_inBound(CRect bound) : bound_(bound) {}
	bool operator()(const iLink& l) const {
		return l.CanDraw() &&
			bound_.PtInRect(l.GetPtFrom()) &&
			bound_.PtInRect(l.GetPtTo());
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
				SaveOrderByTree(ar); // ノードの保存
				// リンクの保存
				ar << links_.size();
				link_iter li = links_.begin();
				for (; li != links_.end(); li++) {
					(*li).Serialize(ar);
				}
			}
			else {
				const int SERIAL_VERSION = 5; // シリアル化バージョン番号
				ar << SERIAL_VERSION;
				SaveOrderByTreeEx(ar, SERIAL_VERSION); // ノードの保存
				// リンクの保存
				ar << links_.size();
				link_iter li = links_.begin();
				for (; li != links_.end(); li++) {
					(*li).SerializeEx(ar, SERIAL_VERSION);
				}
				// OutlineView状態の書き込み
				SaveTreeState(ar, SERIAL_VERSION);
			}
		}
	}
	else
	{
		// TODO: この位置に読み込み用のコードを追加してください。
		if (m_bSerializeXML) {
			SerializeFromXml(ar.GetFile()->GetFilePath());
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
					nodes_[n.GetKey()] = n;
					sv.push_back(n.GetKey());
				}
				// リンクの読み込み
				ar >> count;
				lastLinkKey = 0;
				for (unsigned int i = 0; i < count; i++) {
					iLink l;
					l.Serialize(ar);
					l.SetKey(lastLinkKey++);
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
					nodes_[n.GetKey()] = n;
					sv.push_back(n.GetKey());
				}
				// リンクの読み込み
				ar >> count;
				lastLinkKey = 0;
				for (unsigned int i = 0; i < count; i++) {
					iLink l;
					l.SerializeEx(ar, version);
					l.SetKey(lastLinkKey++);
					links_.push_back(l);
				}
				// OutlineView状態の読み込み
				LoadTreeState(ar, version);
			}
		}
	}
}

void iEditDoc::SaveOrderByTree(CArchive& ar)
{
	OutlineView* pView = GetOutlineView();
	NodePropsVec ls;
	pView->SerializeTree0(ls);  // シリアライズ専用シーケンス取得
	ar << lastKey;
	ar << ls.size();
	for (unsigned int i = 0; i < ls.size(); i++) {
		node_iter it = nodes_.FindWrite(ls[i].key);
		if (it != nodes_.end()) {
			(*it).second.SetTreeState(ls[i].state);
			(*it).second.Serialize(ar);
		}
	}
}

void iEditDoc::SaveOrderByTreeEx(CArchive &ar, int version)
{
	OutlineView* pView = GetOutlineView();
	NodePropsVec ls;
	pView->SerializeTree0(ls);  // シリアライズ専用シーケンス取得
	ar << lastKey;
	ar << ls.size();
	for (unsigned int i = 0; i < ls.size(); i++) {
		node_iter it = nodes_.FindWrite(ls[i].key);
		if (it != nodes_.end()) {
			(*it).second.SetTreeState(ls[i].state);
			(*it).second.SerializeEx(ar, version);
		}
	}
}

void iEditDoc::SaveTreeState(CArchive &ar, int version)
{
	OutlineView* pView = GetOutlineView();
	ar << pView->GetBranchMode();
	ar << m_dwBranchRootKey;
}

void iEditDoc::LoadTreeState(CArchive &ar, int version)
{
	ar >> m_initialBranchMode;
	if (m_initialBranchMode == 1 || m_initialBranchMode == 2) {
		m_bShowBranch = true;
	}
	DWORD branchRootKey;
	ar >> branchRootKey;

	m_dwBranchRootKey = branchRootKey;
	CalcMaxPt(m_maxPt);
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

void iEditDoc::CopyNodeLabels(NodePropsVec &v)
{
	for (unsigned int i = 0; i < sv.size(); i++) {
		node_c_iter it = nodes_.FindRead(sv[i]);
		NodeProps l;
		l.name = (*it).second.GetName();
		l.key = (*it).second.GetKey();
		l.parent = (*it).second.GetParentKey();
		l.state = (*it).second.GetTreeState();
		l.level = (*it).second.GetLevel();
		l.treeIconId = (*it).second.GetTreeIconId();
		v.push_back(l);
	}
	sv.clear();
	sv.resize(0);
}

// OutlineViewでのノード追加用メソッド
void iEditDoc::AddNode(const NodeProps &l, DWORD inheritKey, bool bInherit)
{
	node_c_iter it = nodes_.FindRead(inheritKey);
	iNode n(l.name);
	n.SetKey(l.key);
	n.SetParentKey(l.parent);

	if (ShowSubBranch()) {
		m_visibleKeys.insert(l.key);
	}

	if (bInherit && it != nodes_.end()) {
		n.SetLineColor((*it).second.GetLineColor());
		n.SetLineWidth((*it).second.GetLineWidth());
		n.SetLineStyle((*it).second.GetLineStyle());
		n.SetFillColor((*it).second.GetFillColor());
		n.ToggleFill((*it).second.Filled());
		n.SetShape((*it).second.GetShape());
		n.SetTextStyle((*it).second.GetTextStyle());
		n.SetFontColor((*it).second.GetFontColor());
		n.SetFontInfo((*it).second.GetFontInfo());
	}

	if (it != nodes_.end()) {
		CRect rc = (*it).second.GetBound();
		n.MoveBound(CSize(rc.left + 20, rc.top + 30));
	}

	nodes_[n.GetKey()] = n;
	SelectionChanged(l.key, true, ShowSubBranch());
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeAdd;
	UpdateAllViews(NULL, (LPARAM)l.key, &hint);
}

// OutlineViewのインポートテキストファイル専用に作ってる
void iEditDoc::addNode2(const iNode &n)
{
	nodes_[n.GetKey()] = n;
	sv.push_back(n.GetKey());
	SetModifiedFlag();
}

DWORD iEditDoc::AssignNewKey()
{
	return ++lastKey;
}

bool iEditDoc::SetKeyNodeName(DWORD key, const CString &name)
{
	node_iter it = nodes_.FindWrite(key);
	if (it == nodes_.end()) {
		return false;
	}
	(*it).second.SetName(name);
	SetModifiedFlag();
	UpdateAllViews(NULL);
	return true;
}

void iEditDoc::SetKeyNodeParent(DWORD key, DWORD parent)
{
	node_iter it = nodes_.FindWrite(key);
	if (it != nodes_.end()) {
		/*const_cast<iNode&>*/(*it).second.SetParentKey(parent);
	}
	SetModifiedFlag();
	UpdateAllViews(NULL);
}

CString iEditDoc::GetKeyNodeChapterNumber(DWORD key) {
	node_c_iter it = nodes_.FindRead(key);
	if (it != nodes_.end()) {
		return (*it).second.GetChapterNumber();
	}
	return _T("");
}

void iEditDoc::SetKeyNodeChapterNumber(DWORD key, const CString& chapterNumber) {
	node_iter it = nodes_.FindWrite(key);
	if (it != nodes_.end()) {
		(*it).second.SetChapterNumber(chapterNumber);
	}
	// transient 属性なので、DirtyFlag 立てたりビュー再描画したりは不要。
}

void iEditDoc::DeleteKeyItem(DWORD key)
{
	DWORD parent = nodes_.GetCurrentParent();
	// 以下の行は、メタファイルの再描画不具合のため廃止iNode_eqの中でiNodeの参照を使うのが原因？
//	nodes_.erase(remove_if(nodes_.begin(), nodes_.end(), iNode_eq(key)), nodes_.end());

	node_iter it = nodes_.FindWrite(key);
	if (it != nodes_.end()) {
		nodes_.erase(it);
	}
	links_.erase(remove_if(links_.begin(), links_.end(), iLink_eq(key)), links_.end());
	if (m_bShowBranch) {
		nodes_.PrepareVisibles(m_visibleKeys);
	}
	else {
		nodes_.PrepareVisibles(this->nodes_.GetSelectedKey());
	}
	SetModifiedFlag();
	iHint h; h.event = iHint::nodeDeleteByKey;
	UpdateAllViews(NULL, key, &h);
}

BOOL iEditDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	m_openFilePath = lpszPathName;
	CString drive, dir, file, extent;
	FileUtil::SplitPath(m_openFilePath, drive, dir, file, extent);
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
	CString drive, dir, file, extent;
	FileUtil::SplitPath(lpszPathName, drive, dir, file, extent);
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
		iNode i(_T("主題")); i.SetKey(0); i.SetParentKey(0);
		i.MoveBound(CSize(10, 10));
		CString title = GetFileNameFromOpenPath();
		if (GetFileNameFromOpenPath() != _T("")) {
			i.SetName(title);
		}
		nodes_[i.GetKey()] = i;
		sv.push_back(i.GetKey());
	}

	nodes_.InitSelection();
	curParent = nodes_.GetCurrentParent();
	nodes_.PrepareVisibles(nodes_.GetSelectedKey());
	CalcMaxPt(m_maxPt);
	canCpyLink = FALSE;
}

CString iEditDoc::GetFileNameFromOpenPath()
{
	CString drive, dir, fileName, extent;
	FileUtil::SplitPath(m_openFilePath, drive, dir, fileName, extent);
	extent.MakeLower();
	if (extent == _T(".iedx") || extent == _T(".ied") || extent == _T(".xml")) {
		return fileName;
	}
	return _T("");
}

CString iEditDoc::GetSelectedNodeText()
{
	node_iter it = nodes_.GetSelectedIter();
	if (it != nodes_.end()) {
		return (*it).second.GetText();
	}
	return _T("");
}

void iEditDoc::SelectionChanged(DWORD key, bool reflesh, bool bShowSubBranch)
{
	DWORD parentOld = nodes_.GetCurrentParent();
	nodes_.Select(key);

	NodeKeyVec svec = GetOutlineView()->GetDrawOrder(bShowSubBranch);
	if (((CiEditApp*)AfxGetApp())->m_rgsNode.orderDirection == 1) {
		// 降順での描画オプションの場合は反転する
		std::reverse(svec.begin(), svec.end());
	}
	nodes_.SetDrawOrder(svec);

	if (!bShowSubBranch) {
		nodes_.PrepareVisibles(key);
	}
	else {
		nodes_.PrepareVisibles(m_visibleKeys);
	}

	DWORD parentNew = nodes_.GetCurrentParent();

	if (parentOld != parentNew || parentNew == 0) {
		CalcMaxPt(m_maxPt);
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

void iEditDoc::SetCurrentNodeText(CString &s, int scrollPos)
{
	node_iter it = nodes_.GetSelectedIter();
	if (it != nodes_.end()) {
		(*it).second.SetText(s);
		(*it).second.SetTextTopPos(scrollPos);
	}
	// Undo 処理のためのメッセージを飛ばす方が良い。
}

void iEditDoc::DrawNodes(CDC *pDC)
{
	nodes_.Draw(pDC);
}

bool iEditDoc::HitTest(const CPoint& pt, CRect &r)
{
	iNode* pNode = nodes_.HitTest(pt);
	if (pNode != NULL) {
		r = pNode->GetBound();
		iHint hint;
		hint.event = iHint::nodeSel;
		UpdateAllViews(NULL, (LPARAM)pNode->GetKey(), &hint);

		return true;
	}
	return false;
}

void iEditDoc::MoveSelectedNode(const CSize &sz)
{
	nodes_.MoveSelected(sz);
	SetConnectionPoint();
	CalcMaxPt(m_maxPt);
	SetModifiedFlag();
}

void iEditDoc::MoveNodesInBound(const CRect& bound, const CSize move)
{
	node_iter itSelected = nodes_.GetSelectedIter();
	if (itSelected == nodes_.end()) return;
	node_iter it = nodes_.begin();
	bool moved = false;
	NodeKeySet keySet;
	for (; it != nodes_.end(); it++) {
		if (!(*it).second.Visible()) continue;
		BOOL bInBound = bound.PtInRect((*it).second.GetBound().TopLeft()) &&
			bound.PtInRect((*it).second.GetBound().BottomRight());
		if (bInBound) {
			if ((*it).second.GetDrawOrder() > (*itSelected).second.GetDrawOrder()) {
				(*it).second.MoveBound(move);
				keySet.insert((*it).second.GetKey());
				moved = true;
			}
		}
	}

	if (moved) {
		link_iter li = links_.begin();
		for (; li != links_.end(); li++) {
			if (keySet.find((*li).GetFromNodeKey()) != keySet.end() &&
				keySet.find((*li).GetToNodeKey()) != keySet.end()) {
				(*li).MovePoints(move);
			}
		}
		SetConnectionPoint();

		iHint hint; hint.event = iHint::groupMoved;
		UpdateAllViews(NULL, nodes_.GetSelectedKey(), &hint);
	}
}

void iEditDoc::MoveSelectedLink(const CSize &sz)
{
	link_iter li = links_.begin();
	for (; li != links_.end(); li++) {
		node_iter itFrom = nodes_.FindWrite((*li).GetFromNodeKey());
		node_iter itTo = nodes_.FindWrite((*li).GetToNodeKey());
		if (itFrom == nodes_.end() || itTo == nodes_.end()) continue;
		if ((*itFrom).second.Selected() && (*itTo).second.Selected()) {
			(*li).MovePoints(sz);
			SetModifiedFlag();
			CalcMaxPt(m_maxPt);
		}
	}
}


void iEditDoc::SetSelectedNodeBound(const CRect &r, bool withLink, bool noBackup)
{
	if (!noBackup) {
		BackupNodesForUndo();
	}
	nodes_.SetSelectedBound(r);
	if (withLink) {
		SetConnectionPoint();
	}
	CalcMaxPt(m_maxPt);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.GetSelectedKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::SetConnectionPoint()
{
	link_iter li = links_.begin();
	for (; li != links_.end(); li++) {
		node_iter itFrom = nodes_.FindWrite((*li).GetFromNodeKey());
		node_iter itTo = nodes_.FindWrite((*li).GetToNodeKey());
		if (itFrom == nodes_.end() || itTo == nodes_.end()) continue;
		if ((*itFrom).second.Selected() && (*itTo).second.Selected()) {
			if (itFrom != itTo) continue;
		}

		if ((*li).GetFromNodeKey() == (*itFrom).second.GetKey()) {
			(*li).SetFromNodeRect((*itFrom).second.GetBound());
		}
		if ((*li).GetToNodeKey() == (*itTo).second.GetKey()) {
			(*li).SetToNodeRect((*itTo).second.GetBound());
		}
	}
}

const CPoint& iEditDoc::GetMaxPt() const
{
	return m_maxPt;
}

// TODO:このメソッドはパブリックのメソッドが呼ばれたときに暗黙のうちに呼ばれてると
// 思われがちだが、実際は適切なタイミングで呼び出されないといけない。呼び出し忘れ
// があると再描画領域の計算がおかしくなる。
void iEditDoc::CalcMaxPt(CPoint &pt)
{
	NodeKeySet ks;
	pt = CPoint(0, 0);
	node_c_iter it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if (!(*it).second.Visible()/* && !m_bShowAll*/) {
			continue;
		} // この位置でフィルタをかけるべきだよね。これが、選択領域のバグの原因？
		ks.insert((*it).second.GetKey());

		if ((*it).second.GetBound().BottomRight().x > pt.x) {
			pt.x = (*it).second.GetBound().BottomRight().x;
		}
		if ((*it).second.GetBound().BottomRight().y > pt.y) {
			pt.y = (*it).second.GetBound().BottomRight().y;
		}
	}
	link_iter li = links_.begin();
	for (; li != links_.end(); li++) {
		if (ks.find((*li).GetFromNodeKey()) != ks.end() && ks.find((*li).GetToNodeKey()) != ks.end() && (*li).GetArrowStyle() != iLink::other) {
			(*li).SetDrawable();
			if ((*li).getBound().BottomRight().x > pt.x) {
				pt.x = (*li).getBound().BottomRight().x;
			}
			if ((*li).getBound().BottomRight().y > pt.y) {
				pt.y = (*li).getBound().BottomRight().y;
			}
		}
		else {
			(*li).SetDrawable(false);
		}
	}
}

void iEditDoc::DrawLinks(CDC *pDC, bool clipbrd)
{
	links_.DrawLines(pDC);
	links_.DrawArrows(pDC);
	links_.DrawComments(pDC, clipbrd);
}

bool iEditDoc::SetStartLink(const CPoint& pt)
{
	iNode* pNode = nodes_.HitTest(pt);
	if (pNode != NULL) {
		rcLinkFrom = pNode->GetBound();
		keyLinkFrom = pNode->GetKey();
		keyParentLinkFrom = pNode->GetParentKey();
		return true;
	}
	return false;
}

bool iEditDoc::SetEndLink(const CPoint &pt, int ArrowType, bool bArrowSpecification)
{
	iNode* pNode = nodes_.HitTestSilently(pt);
	if (pNode != NULL) {
		rcLinkTo = pNode->GetBound();
		keyLinkTo = pNode->GetKey();
		iLink l;
		l.SetNodes(rcLinkFrom, rcLinkTo, keyLinkFrom, keyLinkTo);
		l.SetDrawable();
		l.SetKey(lastLinkKey++);
		ResolveLinkLineStyle(l);
		if (bArrowSpecification) {
			l.SetArrowStyle(ArrowType);
		}
		else {
			ResolveLinkArrowStyle(l);
		}
		links_.push_back(l);
		SetModifiedFlag();
		iHint h; h.event = iHint::linkAdd;
		UpdateAllViews(NULL, (LPARAM)keyLinkFrom, &h);
		return true;
	}
	return false;
}

bool iEditDoc::SwitchLinkStartNodeAt(const CPoint &pt)
{
	iNode* pNode = nodes_.HitTest(pt); // リンク元を再選択
	if (pNode != NULL) {
		BackupLinksForUndo();
		links_.SetSelectedLinkNodeFrom(pNode->GetKey(), pNode->GetBound());
		SetModifiedFlag();
		iHint h; h.event = iHint::linkModified;
		UpdateAllViews(NULL, (LPARAM)GetSelectedNodeKey(), &h);
		return true;
	}
	return false;
}

bool iEditDoc::SwitchLinkEndNodeAt(const CPoint &pt)
{
	iNode* pNode = nodes_.HitTestSilently(pt);
	if (pNode != NULL) {
		BackupLinksForUndo();
		links_.SetSelectedLinkNodeTo(pNode->GetKey(), pNode->GetBound());
		SetModifiedFlag();
		UpdateAllViews(NULL);
		iHint h; h.event = iHint::linkModified;
		UpdateAllViews(NULL, (LPARAM)GetSelectedNodeKey(), &h);
		return true;
	}
	return false;
}

void iEditDoc::SetSelectedNodeFont(const LOGFONT &lf)
{
	nodes_.SetSelectedFont(lf);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.GetSelectedKey();
	SetConnectionPoint();
	CalcMaxPt(m_maxPt);
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::GetSelectedNodeFont(LOGFONT& lf)
{
	nodes_.GetSelectedFont(lf);
}

COLORREF iEditDoc::GetSelectedNodeFontColor() const
{
	return nodes_.GetSelectedFontColor();
}

void iEditDoc::SetSelectedNodeFontColor(const COLORREF &c)
{
	nodes_.SetSelectedFontColor(c);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.GetSelectedKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::SetSelectedNodeLineColor(const COLORREF &c)
{
	nodes_.SetSelectedLineColor(c);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.GetSelectedKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::SetSelectedNodeBrush(const COLORREF &c)
{
	nodes_.SetSelectedFillColor(c);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.GetSelectedKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::SetSelectedNodeNoBrush(BOOL noBrush)
{
	nodes_.ToggleSelectedFill(noBrush);
	SetConnectionPointVisibleLinks();
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.GetSelectedKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

COLORREF iEditDoc::GetSelectedNodeLineColor() const
{
	return nodes_.GetSelectedLineColor();
}

COLORREF iEditDoc::GetSelectedNodeBrsColor() const
{
	return nodes_.GetSelectedFillColor();
}

void iEditDoc::SetSelectedNodeLineStyle(int style)
{
	nodes_.SetSelectedLineStyle(style);
	SetModifiedFlag();
	SetConnectionPointVisibleLinks();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.GetSelectedKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

int iEditDoc::GetSelectedNodeLineStyle() const
{
	return nodes_.GetSelectedLineStyle();
}

void iEditDoc::SetSelectedNodeLineWidth(int w)
{
	nodes_.SetSelectedLineWidth(w);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.GetSelectedKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

int iEditDoc::GetSelectedNodeLineWidth() const
{
	return nodes_.GetSelectedLineWidth();
}


bool iEditDoc::IsSelectedNodeMultiLine() const
{
	int style = nodes_.GetSelectedTextStyle();
	if (style == iNode::m_c || style == iNode::m_l || style == iNode::m_r) {
		return true;
	}
	return false;
}

void iEditDoc::SetSelectedNodeMultiLine(bool set)
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
	nodes_.SetSelectedTextStyle(style);
	SetConnectionPointVisibleLinks();
	CalcMaxPt(m_maxPt);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.GetSelectedKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

int iEditDoc::SelectNodesInBound(const CRect &r, CRect& selRect)
{
	int cnt = nodes_.SelectInBound(r, selRect);
	if (cnt == 1) {
		CRect rc;
		HitTest(selRect.CenterPoint(), rc);
	}
	return cnt;
}

CRect iEditDoc::GetSelectedNodeRect() const
{
	CRect rc(CRect(0, 0, 0, 0));
	node_c_iter it = nodes_.GetSelectedConstIter();
	if (it != nodes_.end()) {
		rc = (*it).second.GetBound();
	}
	return rc;
}

// クリップボートからのノード一括作成のためにこのメソッドだけシグネチャを変えました。
void iEditDoc::AddNodeRect(const CString &name, const CPoint &pt, bool bSetMultiLineProcess, bool bNoBound)
{
	AddNodeInternal(name, pt, iNode::rectangle, bSetMultiLineProcess, bNoBound);
}

void iEditDoc::AddNodeArc(const CString &name, const CPoint &pt)
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
	n.SetKey(AssignNewKey());
	n.SetParentKey(nodes_.GetCurrentParent());
	n.SetShape(nodeType);
	n.MoveBound(CSize(pt.x, pt.y));
	n.SetVisible();

	if (!bEnableMultiLineProcess) {
		n.SetTextStyle(iNode::s_cc);
	}
	if (bNoBound) {
		n.ToggleFill(FALSE);
		n.SetLineStyle(PS_NULL);
	}
	nodes_[n.GetKey()] = n;

	if (ShowSubBranch()) {
		m_visibleKeys.insert(n.GetKey());
	}
	CalcMaxPt(m_maxPt);
	SelectionChanged(n.GetKey(), true, ShowSubBranch());
	SetModifiedFlag();
	iHint hint;

	// OutlineViewでのイベント処理用に飛ばしてる arcとrectを分ける意味はないようだ。
	hint.event = iHint::rectAdd;
	if (nodeType == iNode::arc) {
		hint.event = iHint::arcAdd;
	}

	hint.str = name;
	UpdateAllViews(NULL, (LPARAM)n.GetKey(), &hint);
}

void iEditDoc::AddShapeNode(const CString &name, const CPoint &pt, int mfIndex, HENHMETAFILE& mh)
{
	iNode n(name);
	n.SetKey(AssignNewKey());
	n.SetParentKey(nodes_.GetCurrentParent());
	n.SetShape(iNode::MetaFile);
	n.MoveBound(CSize(pt.x, pt.y));
	n.SetMetaFile(mh);
	n.SetTextStyle(iNode::notext);
	nodes_[n.GetKey()] = n;

	if (ShowSubBranch()) {
		m_visibleKeys.insert(n.GetKey());
	}

	SelectionChanged(n.GetKey(), true, ShowSubBranch());
	SetModifiedFlag();
	iHint hint;
	hint.event = iHint::rectAdd;
	hint.str = name;
	UpdateAllViews(NULL, (LPARAM)n.GetKey(), &hint);
}

bool iEditDoc::HitTestLinks(const CPoint &pt)
{
	DWORD key; CString path;
	bool hit = links_.HitTest(pt, key, path);
	if (hit) {
		iHint h; h.event = iHint::linkSel; h.str = path;
		UpdateAllViews(NULL, (LPARAM)key, &h);
	}
	return hit;
}

DWORD iEditDoc::HitTestDropTarget(const CPoint& pt, const DWORD selectedNodeKey)
{
	return links_.HitTestDropTarget(pt, selectedNodeKey);
}

bool iEditDoc::SelectLinkStartIfHit(const CPoint &pt, bool drwAll)
{
	DWORD key; CString path;
	bool hit = links_.HitTestFrom(pt, key, path);
	if (hit) {
		iHint h; h.event = iHint::linkSel; h.str = path;
		UpdateAllViews(NULL, (LPARAM)key, &h);
	}
	return hit;
}

bool iEditDoc::SelectLinkEndIfHit(const CPoint &pt, bool drwAll)
{
	DWORD key; CString path;
	bool hit = links_.HitTestTo(pt, key, path);
	if (hit) {
		iHint h; h.event = iHint::linkSel; h.str = path;
		UpdateAllViews(NULL, (LPARAM)key, &h);
	}
	return hit;
}

void iEditDoc::DrawLinkSelection(CDC *pDC)
{
	links_.DrawSelection(pDC);
}

void iEditDoc::DrawLinkSelectionFrom(CDC *pDC)
{
	links_.DrawSelectionFrom(pDC);
}

void iEditDoc::DrawLinkSelectionTo(CDC *pDC)
{
	links_.DrawSelectionTo(pDC);
}

void iEditDoc::SetNewLinkInfo(DWORD keyFrom, DWORD keyTo, const CString &comment, int styleArrow)
{
	node_iter itFrom = nodes_.FindWrite(keyFrom);
	node_iter itTo = nodes_.FindWrite(keyTo);
	if (itFrom == nodes_.end() || itTo == nodes_.end()) return;
	iLink l;
	l.SetArrowStyle(styleArrow);
	l.SetName(comment);
	l.SetNodes((*itFrom).second.GetBound(), (*itTo).second.GetBound(), keyFrom, keyTo);
	if ((*itFrom).second.Visible() && (*itTo).second.Visible()) {
		l.SetDrawable();
	}
	l.SetKey(lastLinkKey++);
	links_.push_back(l);
	SetModifiedFlag();
	iHint h; h.event = iHint::linkAdd;
	UpdateAllViews(NULL, (LPARAM)keyFrom, &h);
}

void iEditDoc::GetSelectedLinkInfo(CString &sFrom, CString &sTo, CString &sComment, int &arrowType)
{
	link_c_iter li = links_.GetSelectedRead();
	if (li != links_.end()) {
		sComment = (*li).GetName();
		arrowType = (*li).GetArrowStyle();
		node_c_iter itFrom = nodes_.FindRead((*li).GetFromNodeKey());
		node_c_iter itTo = nodes_.FindRead((*li).GetToNodeKey());
		if (itFrom != nodes_.end()) sFrom = (*itFrom).second.GetName();
		if (itTo != nodes_.end()) sTo = (*itTo).second.GetName();
	}
}

void iEditDoc::SetSelectedLinkInfo(const CString &sComment, int arrowType)
{
	link_iter li = links_.GetSelectedWrite();
	if (li != links_.end()) {
		BackupLinksForUndo();
		(*li).SetName(sComment);
		(*li).SetArrowStyle(arrowType);
		SetModifiedFlag();
		iHint h; h.event = iHint::linkModified;
		UpdateAllViews(NULL, (LPARAM)nodes_.GetSelectedKey(), &h);
	}
}

void iEditDoc::SelectLinksInBound(const CRect &r)
{
	// HINT: ノードが選択されてるかの判断が必要なので、iEditDocで実装すべき
	// 今のところCanDrawがtrueだと選択する
	links_.SelectInBound(r);
}

int iEditDoc::GetSelectedLinkWidth() const
{
	// links や nodesはdocのprivateメンバなのだから要素に関する演算は
	// docでやってもかまわないとこの関数を書いていて気づいた。
	// 方針変更しようと思ったが、inlineで同じメソッド名を使用している
	// とリンカがうまくいかないのでやはり...
	return links_.GetSelectedWidth();
}

void iEditDoc::SetSelectedLinkWidth(int w)
{
	links_.SetSelectedWidth(w);
	SetModifiedFlag();
	iHint h; h.event = iHint::linkModified;
	UpdateAllViews(NULL, (LPARAM)nodes_.GetSelectedKey(), &h);
}

void iEditDoc::SetSelectedLinkLineStyle(int style)
{
	links_.SetSelectedLineStyle(style);
	SetModifiedFlag();
	iHint h; h.event = iHint::linkModified;
	UpdateAllViews(NULL, (LPARAM)nodes_.GetSelectedKey(), &h);
}

int iEditDoc::GetSelectedLinkLineStyle()
{
	return links_.GetSelectedLineStyle();
}

COLORREF iEditDoc::GetSelectedLinkLineColor() const
{
	return links_.GetSelectedLineColor();
}

void iEditDoc::SetSelectedLinkLineColor(const COLORREF &c)
{
	links_.SetSelectedLineColor(c);
	SetModifiedFlag();
	iHint h; h.event = iHint::linkModified;
	UpdateAllViews(NULL, (LPARAM)nodes_.GetSelectedKey(), &h);
}

void iEditDoc::SetSelectedLinkFont(const LOGFONT &lf)
{
	links_.SetSelectedFont(lf);
	SetModifiedFlag();
	iHint h; h.event = iHint::linkModified;
	UpdateAllViews(NULL, (LPARAM)nodes_.GetSelectedKey(), &h);
}

void iEditDoc::GetSelectedLinkFont(LOGFONT &lf)
{
	links_.GetSelectedFont(lf);
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

void iEditDoc::DeleteSelectedLink()
{
	link_iter it = links_.GetSelectedWrite();
	if (it != links_.end()) {
		m_deleteBound = (*it).getBound();
		links_.erase(it);
		SetModifiedFlag();
		iHint h; h.event = iHint::linkDelete;
		UpdateAllViews(NULL, (LPARAM)nodes_.GetSelectedKey(), &h);
	}
}

void iEditDoc::DeleteSelectedLink2()
{
	link_iter it = links_.GetSelectedWrite2();
	if (it != links_.end()) {
		m_deleteBound = (*it).getBound();
		links_.erase(it);
		SetModifiedFlag();
		iHint h; h.event = iHint::linkDelete;
		UpdateAllViews(NULL, (LPARAM)nodes_.GetSelectedKey(), &h);
	}
}

void iEditDoc::DeleteSelectedNode()
{
	DWORD delKey = nodes_.GetSelectedKey();
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeDelete;
	UpdateAllViews(NULL, (LPARAM)delKey, &hint);
}

void iEditDoc::DeleteSelectedNodes()
{
	SetModifiedFlag();
	DWORD parentKey = nodes_.GetCurrentParent();
	if (ShowSubBranch()) {
		parentKey = m_dwBranchRootKey;
	}
	NodeKeyVec v = nodes_.GetSelectedKeys();
	vector<DWORD>::iterator it = v.begin();
	for (; it != v.end(); it++) {
		DWORD delKey = (*it);
		iHint hint; hint.event = iHint::nodeDeleteMulti;
		hint.keyParent = parentKey;
		UpdateAllViews(NULL, (LPARAM)delKey, &hint);
	}
}

CString iEditDoc::GetSelectedNodeLabel()
{
	node_iter it = nodes_.GetSelectedIter();
	if (it != nodes_.end()) {
		return (*it).second.GetName();
	}
	return _T("");
}

CString iEditDoc::GetSelectedLinkLabel(bool drawAll)
{
	CString label(_T(""));
	link_c_iter it = links_.GetSelectedRead();
	if (it != links_.end()) {
		label = (*it).GetName();
	}
	return label;
}


bool iEditDoc::CanDeleteNode() const
{
	return nodes_.GetSelectedKey() != 0;
}


void iEditDoc::CollectLinkProps(LinkPropsVec &ls)
{
	DWORD curKey = nodes_.GetSelectedKey();
	link_iter it = links_.begin();
	for (; it != links_.end(); it++) {
		if ((*it).GetFromNodeKey() != curKey && (*it).GetToNodeKey() != curKey) continue;
		LinkProps i;
		i.comment = (*it).GetName();
		if ((*it).GetFromNodeKey() == curKey) {
			i.keyTo = (*it).GetToNodeKey();
		}
		else if ((*it).GetToNodeKey() == curKey) {
			i.keyTo = (*it).GetFromNodeKey();
		}

		i.path = (*it).GetPath();

		DWORD searchKey = 0;
		if ((*it).GetFromNodeKey() == curKey) {
			searchKey = (*it).GetToNodeKey();
		}
		else if ((*it).GetToNodeKey() == curKey) {
			searchKey = (*it).GetFromNodeKey();
		}

		node_c_iter ni = nodes_.FindRead(searchKey);
		i.sTo = (*ni).second.GetName();

		if ((*it).GetArrowStyle() != iLink::other) {
			if ((*it).CanDraw()) {
				if ((*it).GetFromNodeKey() == curKey) {
					i.linkType = LinkProps::linkSL;
				}
				else if ((*it).GetToNodeKey() == curKey) {
					i.linkType = LinkProps::linkSL2;
				}
			}
			else {
				if ((*it).GetFromNodeKey() == curKey) {
					i.linkType = LinkProps::linkDL;
				}
				else if ((*it).GetToNodeKey() == curKey) {
					i.linkType = LinkProps::linkDL2;
				}
			}
		}
		else {
			CString url = (*it).GetPath();
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
		i.key = (*it).GetKey();
		i._arrowStyle = (*it).GetArrowStyle();
		i._keyFrom = (*it).GetFromNodeKey();
		i.selected = (*it).IsSelected();
		i.linkColor_ = (*it).GetLinkColor();
		i.linkWidth_ = (*it).SetLineWidth();
		i.styleLine_ = (*it).GetLineStyle();
		i.lf_ = (*it).GetFontInfo();
		::lstrcpy(i.lf_.lfFaceName, (*it).GetFontInfo().lfFaceName);
		ls.push_back(i);
	}
}

void iEditDoc::NotifyLinkSelected(const LinkPropsVec &ls, int index)
{
	DWORD curKey = nodes_.GetSelectedKey();
	link_iter it = links_.begin();

	bool selected = false;
	for (; it != links_.end(); it++) {
		if ((*it).GetFromNodeKey() == ls[index]._keyFrom &&
			(*it).GetToNodeKey() == ls[index].keyTo &&
			(*it).GetName() == ls[index].comment &&
			(*it).GetPath() == ls[index].path &&
			(*it).GetArrowStyle() == ls[index]._arrowStyle) {
			(*it).Select();
			selected = true;
		}
		else {
			(*it).Select(false);
		}
	}

	if (selected) {
		if (ls[index].linkType == LinkProps::linkSL) {
			iHint h; h.event = iHint::linkListSel;
			UpdateAllViews(NULL, LPARAM(curKey), &h);
		}
	}
}

void iEditDoc::AddUrlLink(const CString &url, const CString& comment)
{
	DWORD curKey = nodes_.GetSelectedKey();
	iLink l;
	l.SetDrawable(false);
	CRect r(CRect(0, 0, 0, 0));
	l.SetNodes(r, r, curKey, curKey);
	l.SetName(comment);
	l.SetPath(url);
	l.SetArrowStyle(iLink::other);
	l.SetKey(lastLinkKey++);
	links_.push_back(l);
	SetModifiedFlag();
	iHint h; h.event = iHint::linkAdd;
	UpdateAllViews(NULL, LPARAM(curKey), &h);
}

DWORD iEditDoc::GetSelectedNodeKey() const
{
	return nodes_.GetSelectedKey();
}

void iEditDoc::DeleteSpecifidLink(const LinkProps &i)
{
	link_iter it = links_.begin();
	for (; it != links_.end(); it++) {
		if ((*it).GetFromNodeKey() == i._keyFrom &&
			(*it).GetToNodeKey() == i.keyTo &&
			(*it).GetName() == i.comment &&
			(*it).GetPath() == i.path &&
			(*it).GetArrowStyle() == i._arrowStyle) {
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

void iEditDoc::SetSpecifiedLinkProps(const LinkProps &iOld, const LinkProps &iNew)
{
	link_iter it = links_.begin();
	for (; it != links_.end(); it++) {
		if ((*it).GetFromNodeKey() == iOld._keyFrom &&
			(*it).GetToNodeKey() == iOld.keyTo &&
			(*it).GetName() == iOld.comment &&
			(*it).GetPath() == iOld.path &&
			(*it).GetArrowStyle() == iOld._arrowStyle &&
			(*it).GetLinkColor() == iOld.linkColor_ &&
			(*it).SetLineWidth() == iOld.linkWidth_ &&
			(*it).GetLineStyle() == iOld.styleLine_) {
			break;
		}
	}
	if (it != links_.end()) {
		(*it).SetName(iNew.comment);
		(*it).SetArrowStyle(iNew._arrowStyle);
		(*it).SetPath(iNew.path);
		(*it).SetLinkColor(iNew.linkColor_);
		(*it).SetLineWidth(iNew.linkWidth_);
		(*it).SetLineStyle(iNew.styleLine_);
		(*it).SetFontInfo(iNew.lf_);
		(*it).Select();
		SetModifiedFlag();
		iHint h; h.event = iHint::linkModified;
		UpdateAllViews(NULL, iNew._keyFrom, &h);
	}
}

double iEditDoc::LinkWidth2BondStrength(int width)
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

CRect iEditDoc::GetSelectedLinkBound() const
{
	CRect rc(CRect(0, 0, 0, 0));
	rc = links_.GetSelectedBound();
	return rc;
}

CRect iEditDoc::GetRelatedBound() const
{
	// TODO:branchモードの時の処理
	CRect rc(CRect(0, 0, 0, 0));

	node_c_iter it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if ((*it).second.Visible() && (*it).second.Selected()) {
			rc = (*it).second.GetBound();
			break;
		}
	}

	it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if ((*it).second.Visible() && (*it).second.Selected()) {
			rc |= (*it).second.GetBound();
			link_c_iter li = links_.begin();
			for (; li != links_.end(); li++) {
				if (!(*li).CanDraw()/* && !drwAll*/) {
					continue;
				}
				if ((*it).second.GetKey() == (*li).GetFromNodeKey() || (*it).second.GetKey() == (*li).GetToNodeKey()) {
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

	node_c_iter it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if ((*it).second.Visible() && (*it).second.Selected()) {
			rc = (*it).second.GetBound();
			break;
		}
	}

	it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if (/*!drwAll && */ (*it).second.Visible() && (*it).second.Selected() /*|| drwAll && (*it).IsSelected()*/) {
			rc |= (*it).second.GetBound();
		}
	}

	link_c_iter li = links_.begin();
	for (; li != links_.end(); li++) {
		if (!(*li).CanDraw()) {
			continue;
		}
		node_iter itFrom = nodes_.FindWrite((*li).GetFromNodeKey());
		node_iter itTo = nodes_.FindWrite((*li).GetToNodeKey());

		if ((*itFrom).second.Selected() && (*itTo).second.Selected()) {
			rc |= (*li).getBound();
		}
	}

	return rc;
}

void iEditDoc::CurveSelectedLink(CPoint pt, bool curve)
{
	link_iter li = links_.GetSelectedWrite();
	if (li != links_.end()) {
		if (!curve) {
			(*li).Curve(false);
			SetModifiedFlag();
			iHint h; h.event = iHint::linkStraight;
			UpdateAllViews(NULL, (LPARAM)nodes_.GetSelectedKey(), &h);
			return;
		}

		if (!(*li).IsCurved() && (*li).HitTest(pt)) {
			return;
		}
		(*li).SetPathPt(pt);
		(*li).Curve();
		if ((*li).HitTestExcludePathPt(pt) && (*li).GetName() == _T("")) {
			(*li).Curve(false);
		}
		SetModifiedFlag();
		iHint h; h.event = iHint::linkCurved;
		CalcMaxPt(m_maxPt);
		UpdateAllViews(NULL, (LPARAM)nodes_.GetSelectedKey(), &h);
	}
}

void iEditDoc::AngleSelectedLink(bool angled)
{
	BackupLinksForUndo();
	link_iter li = links_.GetSelectedWrite();
	if (li == links_.end()) return;
	if (!(*li).IsCurved()) return;
	(*li).Angle(angled);
	SetModifiedFlag();
	iHint h; h.event = iHint::linkModified;
	CalcMaxPt(m_maxPt);
	UpdateAllViews(NULL, (LPARAM)nodes_.GetSelectedKey(), &h);
}

void iEditDoc::GetSelectedLinkEndPoints(CPoint &start, CPoint &end)
{
	link_c_iter li = links_.GetSelectedRead();
	if (li != links_.end()) {
		node_c_iter itstart = nodes_.FindRead((*li).GetFromNodeKey());
		node_c_iter itend = nodes_.FindRead((*li).GetToNodeKey());
		if (itstart == nodes_.end() || itend == nodes_.end()) {
			start = CPoint(0, 0); end = CPoint(0, 0);
			return;
		}
		start.x = ((*itstart).second.GetBound().left + (*itstart).second.GetBound().right) / 2;
		start.y = ((*itstart).second.GetBound().top + (*itstart).second.GetBound().bottom) / 2;
		end.x = ((*itend).second.GetBound().left + (*itend).second.GetBound().right) / 2;
		end.y = ((*itend).second.GetBound().top + (*itend).second.GetBound().bottom) / 2;
	}
}

BOOL iEditDoc::IsSelectedLinkCurved() const
{
	link_c_iter li = links_.GetSelectedRead();
	if (li != links_.end() && (*li).IsCurved()) {
		return TRUE;
	}
	return FALSE;
}

BOOL iEditDoc::IsSelectedLinkSelfReferential() const
{
	link_c_iter li = links_.GetSelectedRead();

	if (li == links_.end()) return FALSE;
	if ((*li).GetArrowStyle() == iLink::other) return FALSE;
	if ((*li).GetFromNodeKey() == (*li).GetToNodeKey()) return TRUE;
	return FALSE;
}

void iEditDoc::CopyLinkForPaste()
{
	link_c_iter li = links_.GetSelectedRead2();
	if (li != links_.end()) {
		m_cpLinkOrg = (*li);
		canCpyLink = TRUE;
	}
}

void iEditDoc::PasteCopiedLink()
{
	DWORD curKey = nodes_.GetSelectedKey();
	m_cpLinkOrg.SetKeyFrom(curKey);
	if (m_cpLinkOrg.GetArrowStyle() == iLink::other) {
		m_cpLinkOrg.SetKeyTo(curKey);
	}

	if (m_cpLinkOrg.GetFromNodeKey() != m_cpLinkOrg.GetToNodeKey()) {
		m_cpLinkOrg.Curve(false);
	}

	m_cpLinkOrg.SetKey(lastLinkKey++);
	m_cpLinkOrg.Select();

	node_c_iter itFrom = nodes_.FindRead(m_cpLinkOrg.GetFromNodeKey());
	node_c_iter itTo = nodes_.FindRead(m_cpLinkOrg.GetToNodeKey());

	if (itFrom != nodes_.end() && itTo != nodes_.end()) {
		m_cpLinkOrg.SetNodes((*itFrom).second.GetBound(), (*itTo).second.GetBound(),
			(*itFrom).second.GetKey(), (*itTo).second.GetKey());
	}

	links_.push_back(m_cpLinkOrg);
	SetModifiedFlag();
	iHint h; h.event = iHint::linkAdd;
	UpdateAllViews(NULL, (LPARAM)curKey, &h);
}

BOOL iEditDoc::CanCopyLink()
{
	return canCpyLink;
}

BOOL iEditDoc::IsSelectedNodeFilled() const
{
	return nodes_.SelectedFilled();
}

BOOL iEditDoc::LinksExist() const
{
	link_c_iter li = links_.begin();
	for (; li != links_.end(); li++) {
		if ((*li).CanDraw()) {
			return TRUE;
		}
	}
	return FALSE;
}

void iEditDoc::AddSelectedNodesToCopyOrg()
{
	copyOrg.clear(); copyOrg.resize(0);
	node_iter It = nodes_.GetSelectedIter();
	ptSelectMin = (*It).second.GetBound().TopLeft();
	node_c_iter it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if ((*it).second.Selected()) {
			copyOrg.push_back((*it).second.GetKey());
			if (ptSelectMin.x > (*it).second.GetBound().TopLeft().x) {
				ptSelectMin.x = (*it).second.GetBound().TopLeft().x;
			}
			if (ptSelectMin.y > (*it).second.GetBound().TopLeft().y) {
				ptSelectMin.y = (*it).second.GetBound().TopLeft().y;
			}
		}
	}
}

void iEditDoc::DuplicateNodes(const CPoint& pt, bool useDefault)
{
	for (unsigned int i = 0; i < copyOrg.size(); i++) {
		node_iter it = nodes_.FindWrite(copyOrg[i]);
		iNode n((*it).second);
		n.SetKey(AssignNewKey());
		n.SetParentKey(nodes_.GetCurrentParent());

		if (!useDefault) {
			CPoint ptNew = pt;
			int szx = (*it).second.GetBound().TopLeft().x - ptSelectMin.x;
			int szy = (*it).second.GetBound().TopLeft().y - ptSelectMin.y;
			ptNew.x += szx; ptNew.y += szy;
			n.moveTo(ptNew);
		}
		else {
			n.MoveBound(CSize(20, 30));
		}

		nodes_[n.GetKey()] = n;

		if (ShowSubBranch()) {
			m_visibleKeys.insert(n.GetKey());
		}

		SelectionChanged(n.GetKey(), true, ShowSubBranch());
		CalcMaxPt(m_maxPt);
		SetModifiedFlag();
		iHint hint; hint.event = iHint::rectAdd;
		hint.str = n.GetName();
		hint.treeIconId = n.GetTreeIconId();
		UpdateAllViews(NULL, (LPARAM)n.GetKey(), &hint);
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
	return nodes_.GetSelectedShape();
}

void iEditDoc::SetSelectedNodeShape(int shape, int mfIndex)
{
	DisableUndo();
	BackupNodesForUndo();
	BackupLinksForUndo();
	nodes_.SetSelectedShape(shape);
	SetModifiedFlag();
	if (shape == iNode::MetaFile) {
		node_iter it = nodes_.GetSelectedIter();
		if (it != nodes_.end()) {
			CiEditApp* pApp = (CiEditApp*)AfxGetApp();
			(*it).second.SetMetaFile(pApp->m_hMetaFiles[mfIndex]);
			SetConnectionPoint();
		}
	}
	iHint h; h.event = iHint::nodeStyleChanged;
	UpdateAllViews(NULL, (LPARAM)nodes_.GetSelectedKey(), &h);
}

void iEditDoc::SetSelectedNodeMetaFile(HENHMETAFILE metafile)
{
	node_iter it = nodes_.GetSelectedIter();
	if (it != nodes_.end()) {
		DisableUndo();
		BackupNodesForUndo();
		BackupLinksForUndo();
		(*it).second.SetShape(iNode::MetaFile);
		(*it).second.SetMetaFile(metafile);
		SetConnectionPoint();
		SetModifiedFlag();
		iHint h; h.event = iHint::nodeStyleChanged;
		UpdateAllViews(NULL, (LPARAM)nodes_.GetSelectedKey(), &h);
	}
}

void iEditDoc::SetSelectedNodeLabel(const CString &label)
{
	node_iter it = nodes_.GetSelectedIter();
	if (it != nodes_.end()) {
		(*it).second.SetName(label);
		SetModifiedFlag();
		iHint hint; hint.event = iHint::nodeLabelChanged;
		hint.str = label;
		DWORD key = nodes_.GetSelectedKey();
		UpdateAllViews(NULL, (LPARAM)key, &hint);
	}
}


int iEditDoc::GetSelectedNodeTextStyle() const
{
	return nodes_.GetSelectedTextStyle();
}

void iEditDoc::SetSelectedNodeTextStyle(int style)
{
	nodes_.SetSelectedTextStyle(style);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.GetSelectedKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::SetSelectedNodeTreeIconId(int id)
{
	nodes_.SetSelectedTreeIcon(id);
	SetModifiedFlag();
}

void iEditDoc::SetSelectedNodeFixed(BOOL f)
{
	nodes_.FixSelected(f);
	SetModifiedFlag();
}

BOOL iEditDoc::IsSelectedNodeFixed() const
{
	return nodes_.IsSelectedFixed();
}

bool iEditDoc::ImportXml(const CString &filename, bool replace)
{
	prepareImport();
	CWaitCursor wc;
	XmlProcessor processor(nodesImport, linksImport, lastKey, idcVec);
	if (processor.Import(filename)) {
		AddImportedData(replace);
		return true;
	}
	return false;
}

void iEditDoc::prepareImport()
{
	idcVec.clear(); idcVec.resize(0);
	nodesImport.clear(); nodesImport.resize(0);
	linksImport.clear(); linksImport.resize(0);
	nodeImport.SetKey(-1); linkImport.SetKeyFrom(-1);
	nodeImport.SetBound(CRect(-1, -1, 0, 0));
	nodeImport.SetName(_T(""));
	nodeImport.SetText(_T(""));
	nodeImport.SetTreeState(TVIS_EXPANDED);
	linkImport.SetName(_T(""));
	linkImport.SetPath(_T(""));
	linkImport.SetArrowStyle(iLink::line);
	linkImport.SetLineWidth(0);
	linkImport.SetLinkColor(RGB(0, 0, 0));
}

bool iEditDoc::SerializeFromXml(const CString &filename)
{
	prepareImport();
	CWaitCursor wc;
	XmlProcessor processor(nodesImport, linksImport, lastKey, idcVec);

	if (processor.SerializeFromXml(filename)) {
		unsigned int i;
		for (i = 0; i < nodesImport.size(); i++) {
			nodes_[nodesImport[i].GetKey()] = nodesImport[i];
			sv.push_back(nodesImport[i].GetKey());
			if (nodesImport[i].GetKey() > lastKey) {
				lastKey = nodesImport[i].GetKey();
			}
		}
		lastLinkKey = 0;
		node_c_iter it;
		for (i = 0; i < linksImport.size(); i++) {
			linksImport[i].SetKey(lastLinkKey++);
			it = nodes_.FindRead(linksImport[i].GetFromNodeKey());
			if (it != nodes_.end()) {
				linksImport[i].SetFromNodeRect((*it).second.GetBound());
			}
			it = nodes_.FindRead(linksImport[i].GetToNodeKey());
			if (it != nodes_.end()) {
				linksImport[i].SetToNodeRect((*it).second.GetBound());
			}
			links_.push_back(linksImport[i]);
		}
		m_bLoadXmlSucceeded = true;
		return true;
	}
	return false;
}

bool iEditDoc::SaveXml(const CString &outPath, bool bSerialize)
{
	OutlineView* pView = GetOutlineView();

	NodePropsVec ls;
	if (bSerialize) {
		pView->SerializeTree0(ls);
	}
	else {
		pView->SerializeTree(ls);
	}

	return XmlProcessor::Save(outPath, bSerialize, nodes_, links_, ls);
}

DWORD iEditDoc::FindPairKey(const DWORD first)
{
	for (unsigned int i = 0; i < idcVec.size(); i++) {
		if (idcVec[i].first == first) {
			return idcVec[i].second;
		}
	}
	return -1;
}

void iEditDoc::AddImportedData(bool brepRoot)
{
	//	sort(nodesImport.begin(), nodesImport.end());
	unsigned int i;
	for (i = 0; i < nodesImport.size(); i++) {
		nodesImport[i].SetParentKey(FindPairKey(nodesImport[i].GetParentKey()));
		if (nodesImport[i].GetKey() == nodesImport[i].GetParentKey()) {
			nodesImport[i].SetParentKey(nodes_.GetSelectedKey());
		}
	}

	// ノードを置き換える処理をここに書く
	if (brepRoot) {
		DWORD sel = nodes_.GetSelectedKey();
		DWORD start = nodesImport[0].GetKey();
		nodesImport[0].SetKey(sel);
		for (i = 0; i < nodesImport.size(); i++) {
			if (nodesImport[i].GetParentKey() == start) {
				nodesImport[i].SetParentKey(sel);
			}
		}
		for (i = 0; i < linksImport.size(); i++) {
			if (linksImport[i].GetFromNodeKey() == start) {
				linksImport[i].SetKeyFrom(sel);
			}
			if (linksImport[i].GetToNodeKey() == start) {
				linksImport[i].SetKeyTo(sel);
			}
		}
		node_iter it = nodes_.GetSelectedIter();
		(*it).second.SetName(nodesImport[0].GetName());
		(*it).second.SetText(nodesImport[0].GetText());
		(*it).second.SetBound(nodesImport[0].GetBound());
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
			node_iter itp = nodes_.find(nodesImport[i].GetParentKey());
			node_iter it = nodes_.find(nodesImport[i].GetKey());
			if (itp != nodes_.end() && it == nodes_.end()) {
				CRect rc = nodesImport[i].GetBound();
				if (rc.left < 0 && rc.top < 0) {
					nodesImport[i].moveTo(CPoint(0, 0));
					nodesImport[i].MoveBound(CSize(rand() % 800, rand() % 600));
				}
				nodes_[nodesImport[i].GetKey()] = nodesImport[i];
				sv.push_back(nodesImport[i].GetKey());
			}
		}
	}

	// linkの格納
	for (i = 0; i < linksImport.size(); i++) {
		if (linksImport[i].GetFromNodeKey() != -1 && linksImport[i].GetToNodeKey() != -1) {
			linksImport[i].SetKey(lastLinkKey++);
			links_.push_back(linksImport[i]);
		}
	}
	SetConnectionPoint();
	SetModifiedFlag();

	if (brepRoot) {
		iHint hint; hint.event = iHint::nodeLabelChanged;
		hint.str = nodesImport[0].GetName();
		DWORD key = nodes_.GetSelectedKey();
		UpdateAllViews(NULL, (LPARAM)key, &hint);
	}

	iHint hint; hint.event = iHint::parentSel;
	DWORD key = nodes_.GetSelectedKey();
	CalcMaxPt(m_maxPt);
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::RandomizeNodesPos(const CSize &area)
{
	SYSTEMTIME st;
	GetSystemTime(&st);
	srand((unsigned)st.wMilliseconds);
	node_iter it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if (!(*it).second.Visible()) continue;
		(*it).second.moveTo(CPoint(0, 0));
		(*it).second.MoveBound(CSize(rand() % area.cx, rand() % area.cy));
	}
	SetConnectionPoint();
	CalcMaxPt(m_maxPt);
	SetModifiedFlag();
}

void iEditDoc::WriteKeyNodeToHtml(DWORD key, CStdioFile& f, bool textIsolated, const CString& textPrefix)
{
	node_c_iter it = nodes_.FindRead(key);

	CString keystr; keystr.Format(_T("%d"), (*it).second.GetKey());
	HtmlWriter::WriteText(f, keystr, (*it).second.GetName(), (*it).second.GetText());

	link_c_iter li = links_.begin();
	CString sLink;
	for (; li != links_.end(); li++) {
		if ((*li).GetFromNodeKey() != (*it).second.GetKey() &&
			(*li).GetToNodeKey() != (*it).second.GetKey()) {
			continue;
		}
		if ((*li).GetArrowStyle() != iLink::other) {
			if ((*it).second.GetKey() == (*li).GetFromNodeKey()) {
				node_c_iter itTo = nodes_.FindRead((*li).GetToNodeKey());
				if (itTo != nodes_.end()) {
					CString keystr;
					keystr.Format(_T("%d"), (*li).GetToNodeKey());
					HtmlWriter::BuildLinkTo(sLink, keystr, textIsolated, (*itTo).second.GetName(), (*li).GetName(), textPrefix);
				}
			}
			else if ((*it).second.GetKey() == (*li).GetToNodeKey()) {
				node_c_iter itFrom = nodes_.FindRead((*li).GetFromNodeKey());
				if (itFrom != nodes_.end()) {
					CString keystr;
					keystr.Format(_T("%d"), (*li).GetFromNodeKey());
					HtmlWriter::BuildLinkFrom(sLink, keystr, textIsolated, (*itFrom).second.GetName(), (*li).GetName(), textPrefix);
				}
			}
		}
		else {
			CString url = (*li).GetPath();
			if (url.Find(_T("http://")) == -1 && url.Find(_T("https://")) == -1 && url.Find(_T("ftp://")) == -1) {
				if (!((CiEditApp*)AfxGetApp())->m_rgsOther.bOutputFileLinksOnExport) continue;
				CString sDrive, dir, file, ext;
				FileUtil::SplitPath(url, sDrive, dir, file, ext);
				if (sDrive != "") { // フルパスの時だけ"file:///" つけとけばいいらしい
					url = _T("file:///") + url;
				}
			}
			HtmlWriter::BuildUrlLink(sLink, url, (*li).GetName());
		}
	}
	HtmlWriter::WriteLinks(f, sLink);
}

CString iEditDoc::GetKeyNodeText(DWORD key)
{
	node_c_iter it = nodes_.FindRead(key);
	if (it != nodes_.end()) {
		return (*it).second.GetText();
	}
	return _T("");
}

CString iEditDoc::GetKeyNodeLabel(DWORD key)
{
	node_c_iter it = nodes_.FindRead(key);
	if (it != nodes_.end()) {
		return (*it).second.GetName();
	}
	return _T("");
}

void iEditDoc::ListUpNodes(const CString &sfind, NodePropsVec &labels, BOOL bLabel, BOOL bText, BOOL bLinks, BOOL bUpper)
{
	CString sf = sfind;
	if (bUpper) {
		sf.MakeUpper();
	}
	node_iter it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		bool done = false;
		CString name = (*it).second.GetName();
		CString text = (*it).second.GetText();
		if (bUpper) {
			name.MakeUpper();
			text.MakeUpper();
		}
		if (bLabel && name.Find(sf) != -1) {
			NodeProps l;
			l.name = (*it).second.GetName();
			l.key = (*it).second.GetKey();
			l.state = 0;
			labels.push_back(l);
			done = true;
		}
		if (bText && text.Find(sf) != -1) {
			NodeProps l;
			l.name = (*it).second.GetName();
			l.key = (*it).second.GetKey();
			l.state = 1;
			labels.push_back(l);
		}
	}

	link_iter li = links_.begin();
	for (; li != links_.end(); li++) {
		CString name = (*li).GetName();
		CString path = (*li).GetPath();
		if (bUpper) {
			name.MakeUpper();
			path.MakeUpper();
		}
		if (bLinks && name.Find(sf) != -1) {
			NodeProps l;
			l.key = (*li).GetFromNodeKey();
			l.name = (*li).GetName();
			l.state = 2;
			labels.push_back(l);
		}
		if (bLinks && path.Find(sf) != -1) {
			NodeProps l;
			l.key = (*li).GetFromNodeKey();
			l.name = (*li).GetPath();
			l.state = 3;
			labels.push_back(l);
		}
	}
}

HENHMETAFILE iEditDoc::GetSelectedNodeMetaFile()
{
	node_iter it = nodes_.GetSelectedIter();
	if (it != nodes_.end()) {
		return (*it).second.GetMetaFile();
	}
	return NULL;
}

void iEditDoc::DrawNodesSelected(CDC *pDC)
{
	nodes_.DrawSelected(pDC);
}

void iEditDoc::DrawLinksSelected(CDC *pDC, bool clipbrd)
{
	link_iter li = links_.begin();
	for (; li != links_.end(); li++) {
		node_iter itFrom = nodes_.FindWrite((*li).GetFromNodeKey());
		node_iter itTo = nodes_.FindWrite((*li).GetToNodeKey());
		if ((*li).GetArrowStyle() != iLink::other && (*itFrom).second.Selected() && (*itTo).second.Selected()) {
			(*li).DrawArrow(pDC);
			(*li).DrawLine(pDC);
			(*li).DrawComment(pDC, clipbrd);
		}
	}
}

void iEditDoc::BackupNodesForUndo()
{
	nodes_undo.clear();
	nodes_undo.resize(0);
	node_iter it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if ((*it).second.Visible()) {
			iNode n((*it).second);
			nodes_undo.push_back(n);
		}
	}
}

void iEditDoc::RestoreNodesForUndo()
{
	for (unsigned int i = 0; i < nodes_undo.size(); i++) {
		node_iter it = nodes_.FindWrite(nodes_undo[i].GetKey());
		if (it != nodes_.end()) {
			(*it).second = nodes_undo[i];
		}
	}
	//	SetConnectionPoint();
	CalcMaxPt(m_maxPt);
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
	link_iter li = links_.begin();
	for (; li != links_.end(); li++) {
		if ((*li).CanDraw()) {
			iLink l = (*li);
			links_undo.push_back(l);
		}
	}
}

void iEditDoc::RestoreLinksForUndo()
{
	for (unsigned int i = 0; i < links_undo.size(); i++) {
		link_iter li = links_.begin();
		for (; li != links_.end(); li++) {
			if (links_undo[i].GetKey() == (*li).GetKey()) {
				(*li) = links_undo[i];
				break;
			}
		}
	}
	if (links_.GetDividedKey() != -1) {
		links_.erase(remove_if(links_.begin(), links_.end(), iLink_eqkey(links_.GetDividedKey())), links_.end());
		links_.ClearDividedKey();
	}
	CalcMaxPt(m_maxPt);
	links_undo.clear();
	links_undo.resize(0);
	SetModifiedFlag();
}

void iEditDoc::AlignNodesInBoundTo(const CString& side, const CRect& rect)
{
	node_iter it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if (!(*it).second.Visible()) continue;
		if (!(*it).second.Selected()) continue;
		CPoint pt;
		if (side == _T("left")) {
			pt.x = rect.left;
			pt.y = (*it).second.GetBound().top;
		}
		else if (side == _T("right")) {
			pt.x = (*it).second.GetBound().left + rect.right - (*it).second.GetBound().right;
			pt.y = (*it).second.GetBound().top;
		}
		else if (side == _T("top")) {
			pt.x = (*it).second.GetBound().left;
			pt.y = rect.top;
		}
		else if (side == _T("bottom")) {
			pt.x = (*it).second.GetBound().left;
			pt.y = (*it).second.GetBound().top + rect.bottom - (*it).second.GetBound().bottom;
		}

		(*it).second.moveTo(pt);
	}
	SetConnectionPoint();
	CalcMaxPt(m_maxPt);
	SetModifiedFlag();
}

void iEditDoc::AlignSelectedNodesToSameSize(const CString &strSize)
{
	CSize maxSz = nodes_.GetNodeSizeMax(true);
	node_iter it = nodes_.begin();
	CRect rc;
	for (; it != nodes_.end(); it++) {
		if (!(*it).second.Visible()) continue;
		if (!(*it).second.Selected()) continue;
		rc = (*it).second.GetBound();
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
		(*it).second.SetBound(rc);
	}
	SetConnectionPoint();
	CalcMaxPt(m_maxPt);
	SetModifiedFlag();
}

const iLink* iEditDoc::GetSelectedLink(bool bDrawAll) const
{
	link_c_iter li = links_.GetSelectedRead();
	if (li == links_.end()) {
		return NULL;
	}
	return &(*li);
}

void iEditDoc::ReverseSelectedLinkDirection()
{
	const iLink* pl = GetSelectedLink(false);
	if (pl == NULL) return;
	DWORD keyTo = pl->GetToNodeKey();
	links_.ReversetSelected();
	SetModifiedFlag();
	SelectionChanged(keyTo, true, ShowSubBranch());
	iHint h; h.event = iHint::linkModified;
	UpdateAllViews(NULL, (LPARAM)GetSelectedNodeKey(), &h);
}

void iEditDoc::ViewSettingChanged()
{
	iHint hint; hint.event = iHint::viewSettingChanged;
	DWORD key = nodes_.GetSelectedKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::ExportSvg(const CString &path, bool bEmbed,
	const CString& textFileName, bool textSingle)
{
	NodeKeyVec vec = GetOutlineView()->GetDrawOrder(ShowSubBranch());
	SvgWriter writer(nodes_, links_, vec);
	if (textSingle) {
		writer.SetTextHtmlFileName(textFileName);
	}
	else {
		writer.SetTextHtmlFilePrefix(textFileName);
	}
	writer.Write(path, GetMaxPt(), bEmbed);
}

iNode iEditDoc::GetHitNode(const CPoint &pt)
{
	iNode* pNode = nodes_.HitTestSilently(pt);
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
	nodes_.PrepareVisibles(keySet);
}

void iEditDoc::SetShowBranch(DWORD branchRootKey)
{
	m_dwBranchRootKey = branchRootKey;
	m_bShowBranch = true;
	iHint hint; hint.event = iHint::showSubBranch;
	DWORD key = nodes_.GetSelectedKey();
	CalcMaxPt(m_maxPt);
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

void iEditDoc::ResetShowBranch()
{
	m_bShowBranch = false;
	nodes_.PrepareVisibles(nodes_.GetCurrentParent());
	iHint hint; hint.event = iHint::resetShowSubBranch;
	DWORD key = nodes_.GetSelectedKey();
	CalcMaxPt(m_maxPt);
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

CString iEditDoc::GetSubBranchRootLabel() const
{
	node_c_iter n = nodes_.FindRead(m_dwBranchRootKey);
	if (n != nodes_.end()) {
		return (*n).second.GetName();
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
	DWORD key = GetSelectedNodeKey();
	set<DWORD>::const_iterator it = m_visibleKeys.find(key);
	if (it == m_visibleKeys.end()) {
		return false;
	}
	return true;
}

void iEditDoc::OnFileSaveAs()
{
	CString fullPath = GetPathName();
	CString driveName, dir, fileName, ext;
	FileUtil::SplitPath(fullPath, driveName, dir, fileName, ext);
	if (driveName == _T("")) {
		node_c_iter it = nodes_.find(0);
		CString rootLabel = (*it).second.GetName();
		if (rootLabel != _T("主題")) {
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
			if (((CiEditApp*)AfxGetApp())->m_rgsOptions.registOldFiletype) {
				extension = _T(".ied");
			}
			else {
				extension = _T(".xml");
			}
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
	CString drive, dir, fname, ext;
	FileUtil::SplitPath(GetPathName(), drive, dir, fname, ext);

	if (drive == _T("")) {
		OnFileSaveAs();
	}
	else {
		OnSaveDocument(GetPathName());
	}
}

void iEditDoc::OnUpdateFileSave(CCmdUI *pCmdUI)
{
}

CRect iEditDoc::RestoreDeleteBound() const
{
	return m_deleteBound;
}

void iEditDoc::BackupDeleteBound()
{
	m_deleteBound = GetRelatedBound();
}

void iEditDoc::SetNodeLevel(const DWORD key, const int nLevel)
{
	node_iter it = nodes_.FindWrite(key);
	if (it == nodes_.end()) {
		return;
	}
	(*it).second.SetLevel(nLevel);
}


// 芋蔓に必要なノード間距離を計算する
// iNodeのBoundPreも初期化する多機能メソッド←ダメ
void iEditDoc::CalcEdges()
{
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();

	link_iter li = links_.begin();
	for (; li != links_.end(); li++) {
		if ((*li).GetArrowStyle() != iLink::other) {
			// 距離計算
			if ((*li).GetFromNodeKey() == (*li).GetToNodeKey()) continue;
			if (!(*li).CanDraw() || !(*li).IsInChain()) {
				(*li).SetBoundStrength(-1.0);
			}
			else {
				CSize sz;
				sz.cx = ((*li).GetFromNodeRect().Width() + (*li).GetToNodeRect().Width()) / 2;
				sz.cy = ((*li).GetFromNodeRect().Height() + (*li).GetToNodeRect().Height()) / 2;
				double rate;
				if (pApp->m_rgsLink.bSetStrength) {
					rate = LinkWidth2BondStrength((*li).SetLineWidth());
					if ((*li).GetLineStyle() == PS_DOT) {
						rate *= 1.5;
					}
				}
				else {
					rate = LinkWidth2BondStrength(0);
				}
				rate *= (((double)pApp->m_rgsLink.strength) / 10.0);
				(*li).SetBoundStrength(sqrt((double)(sz.cx*sz.cx + sz.cy*sz.cy)) * 5 / 4 * rate);
			}
			// preBoundの値を初期化
			if (!(*li).IsInChain()) continue;
			node_iter itFrom = nodes_.FindWrite((*li).GetFromNodeKey());
			(*itFrom).second.SetPrevBound((*itFrom).second.GetBound());
			(*itFrom).second.dx = 0.0;
			(*itFrom).second.dy = 0.0;

			node_iter itTo = nodes_.FindWrite((*li).GetToNodeKey());
			(*itTo).second.SetPrevBound((*itTo).second.GetBound());
			(*itTo).second.dx = 0.0;
			(*itTo).second.dy = 0.0;
		}
	}
}

void iEditDoc::RelaxSingleStep(const CPoint &point, const CPoint& dragOffset)
{
	// ドラッグ中のノードの位置を変更
	node_iter ni = nodes_.begin();
	for (; ni != nodes_.end(); ni++) {
		if ((*ni).second.Selected()) {
			CRect rc = (*ni).second.GetBound();
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

			(*ni).second.SetPrevBound((*ni).second.GetPrevBound());
			(*ni).second.SetBound(rc);
			break;
		}
	}

	////// ばねモデル処理
	link_iter li = links_.begin();
	for (; li != links_.end(); li++) {
		if ((*li).GetArrowStyle() == iLink::other ||
			(*li).GetFromNodeKey() == (*li).GetToNodeKey() ||
			!(*li).CanDraw()) {
			continue;
		}
		if (!(*li).IsInChain()) continue;

		node_iter itFrom = nodes_.FindWrite((*li).GetFromNodeKey());
		node_iter itTo = nodes_.FindWrite((*li).GetToNodeKey());

		double gxto = (*itTo).second.GetPrevBound().CenterPoint().x;
		double gyto = (*itTo).second.GetPrevBound().CenterPoint().y;
		double gxfrom = (*itFrom).second.GetPrevBound().CenterPoint().x;
		double gyfrom = (*itFrom).second.GetPrevBound().CenterPoint().y;

		double vx = gxto - gxfrom;
		double vy = gyto - gyfrom;
		double len = sqrt(vx*vx + vy * vy);
		len = (len == 0) ? .0001 : len;
		double f = ((*li).GetBoundStrength() - len) / (len * 3);
		double dx = f * vx;
		double dy = f * vy;
		/*const_cast<iNode&>*/(*itTo).second.dx += dx;
		/*const_cast<iNode&>*/(*itTo).second.dy += dy;
		/*const_cast<iNode&>*/(*itFrom).second.dx -= dx;
		/*const_cast<iNode&>*/(*itFrom).second.dy -= dy;
	}

	srand((unsigned)time(NULL));
	node_iter nit1 = nodes_.begin();
	for (; nit1 != nodes_.end(); nit1++) {
		if (!(*nit1).second.Visible()) continue;
		if (!(*nit1).second.IsInChain()) continue;

		double dx = 0; double dy = 0;
		node_iter nit2 = nodes_.begin();
		for (; nit2 != nodes_.end(); nit2++) {
			if (nit1 == nit2) { continue; }
			if (!(*nit2).second.Visible()) continue;
			if (!(*nit2).second.IsInChain()) continue;

			double gx1 = (*nit1).second.GetPrevBound().CenterPoint().x;
			double gy1 = (*nit1).second.GetPrevBound().CenterPoint().y;
			double gx2 = (*nit2).second.GetPrevBound().CenterPoint().x;
			double gy2 = (*nit2).second.GetPrevBound().CenterPoint().y;

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

	node_iter nit = nodes_.begin();
	for (; nit != nodes_.end(); nit++) {
		if (!(*nit).second.Visible()) continue;
		if (!(*nit).second.IsInChain()) continue;
		if ((*nit).second.Selected()) continue;
		double x = max(-5, min(5, (*nit).second.dx));
		double y = max(-5, min(5, (*nit).second.dy));
		CRect rc = (*nit).second.GetPrevBound();
		rc.OffsetRect((int)x, (int)y);
		// 領域のチェック
		if (rc.left < 0) {
			rc.right = (*nit).second.GetPrevBound().Width();
			rc.left = 0;
		}
		if (rc.top < 0) {
			rc.bottom = (*nit).second.GetPrevBound().Height();
			rc.top = 0;
		}
		if (!(*nit).second.Fixed() && !(*nit).second.Selected()) {
			(*nit).second.SetBound(rc);
		}
		(*nit).second.dx /= 2;
		(*nit).second.dy /= 2;
	}

	node_iter it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if (!(*it).second.Visible()) continue;
		if (!(*it).second.IsInChain()) continue;
		(*it).second.SetPrevBound((*it).second.GetBound());
	}
	SetConnectionPointForLayout();
}

/// 芋づる式に関連ノード・リンクにフラグを立てる
void iEditDoc::ListupChainNodes(bool bResetLinkCurve)
{
	// 直前までのフラグをクリア
	node_iter nit = nodes_.begin();
	for (; nit != nodes_.end(); nit++) {
		(*nit).second.SetInChain(false);
	}
	link_iter linit = links_.begin();
	for (; linit != links_.end(); linit++) {
		(*linit).SetInChain(false);
	}

	// links_から芋づる検索
	NodeKeySet nodeChain; // 新しい芋用
	NodeKeySet nodeChainChecked; // 掘った芋用
	nodeChain.insert(GetSelectedNodeKey()); // selectされている芋
	unsigned int sizePre = nodeChain.size();
	for (; ; ) {
		NodeKeySet::iterator ki = nodeChain.begin();
		for (; ki != nodeChain.end(); ki++) {
			if (nodeChainChecked.find(*ki) != nodeChainChecked.end()) {
				continue;
			}
			link_iter li = links_.begin();
			for (; li != links_.end(); li++) {
				if ((*li).IsTerminalNodeKey(*ki)) {
					if (!(*li).CanDraw()) continue;
					if ((*li).GetArrowStyle() == iLink::other) continue;
					if ((*li).GetFromNodeKey() == (*li).GetToNodeKey()) continue;
					DWORD pairKey;
					if ((*li).GetFromNodeKey() == (*ki)) {
						pairKey = (*li).GetToNodeKey();
					}
					else if ((*li).GetToNodeKey() == (*ki)) {
						pairKey = (*li).GetFromNodeKey();
					}
					nodeChain.insert(pairKey);
					(*li).SetInChain();
					if (bResetLinkCurve) {
						(*li).Curve(false);
					}
					iNode nodeFind;
					node_iter nf = nodes_.find(pairKey);
					if (nf != nodes_.end()) {
						(*nf).second.SetInChain();
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
	CRect rc = GetSelectedNodeRect();
	node_c_iter it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if ((*it).second.Visible() && (*it).second.IsInChain()) {
			rc |= (*it).second.GetBound();
		}
	}
	int length = rc.Width() < rc.Height() ? rc.Width() : rc.Height();
	int margin = (int)(length * 0.5);
	rc.InflateRect(margin, margin);
	return rc;
}

// 芋づるモード用
void iEditDoc::SetConnectionPointForLayout()
{
	link_iter li = links_.begin();
	for (; li != links_.end(); li++) {
		if (!(*li).IsInChain()) continue;

		node_iter itFrom = nodes_.FindWrite((*li).GetFromNodeKey());
		node_iter itTo = nodes_.FindWrite((*li).GetToNodeKey());
		if (itFrom == nodes_.end() || itTo == nodes_.end()) continue;

		if ((*li).GetFromNodeKey() == (*itFrom).second.GetKey()) {
			(*li).SetFromNodeRect((*itFrom).second.GetBound());
		}
		if ((*li).GetToNodeKey() == (*itTo).second.GetKey()) {
			(*li).SetToNodeRect((*itTo).second.GetBound());
		}
	}
}

void iEditDoc::RecalcArea()
{
	CalcMaxPt(m_maxPt);
}

const CRect iEditDoc::AddNodeWithLink(int nodeType, DWORD keyRoot, DWORD prevSibling, const CPoint & pt, bool bMindmap)
{
	node_iter it = nodes_.find(keyRoot);
	if (it == nodes_.end()) return CRect(0, 0, 0, 0);

	iNode nwNode;
	if (bMindmap) {
		CPoint ptTarget = (*it).second.GetBound().CenterPoint() + CPoint(100, -100);
		nwNode = InsertNode(nodeType, _T("ノード"), ptTarget);
	}
	else {
		nwNode = InsertNode(nodeType, _T("ノード"), pt);
	}

	DWORD newKey = nwNode.GetKey();

	SelectionChanged(nwNode.GetKey(), true, ShowSubBranch());
	SetModifiedFlag();

	iHint hint;
	hint.event = iHint::rectAdd;
	hint.str = nwNode.GetName();
	UpdateAllViews(NULL, (LPARAM)nwNode.GetKey(), &hint);

	iLink l;
	l.SetNodes((*it).second.GetBound(), nwNode.GetBound(), (*it).second.GetKey(), nwNode.GetKey());
	l.SetDrawable();
	l.SetKey(lastLinkKey++);
	ResolveLinkLineStyle(l);
	ResolveLinkArrowStyle(l);
	links_.push_back(l);


	if (bMindmap) {
		ListupChainNodes(false);
		CalcEdges();
		nodes_.FixReversibly(newKey); // 新しいノード以外を固定
		for (int i = 0; i < 100; i++) {
			RelaxSingleStep2();
		}
		nodes_.RestoreFixState(newKey); // Fix状態をリストア
	}

	SelectionChanged(nwNode.GetKey(), true, ShowSubBranch());

	node_iter nit = nodes_.find(nwNode.GetKey());
	return (*nit).second.GetBound();
}

const CRect iEditDoc::AddNodeWithLink2(int nodeType, DWORD keyPrevSibling)
{
	if (links_.IsNodeIsolated(nodes_.GetSelectedKey())) return CRect(0, 0, 0, 0);
	DWORD pairKey = links_.GetFirstVisiblePair(nodes_.GetSelectedKey());
	if (pairKey == -1) return CRect(0, 0, 0, 0);

	node_iter itRoot = nodes_.find(pairKey);
	if (itRoot == nodes_.end()) return CRect(0, 0, 0, 0);

	node_iter itSibling = nodes_.find(keyPrevSibling);
	if (itSibling == nodes_.end()) return CRect(0, 0, 0, 0);

	CPoint ptRoot = (*itRoot).second.GetBound().CenterPoint();
	CPoint ptSibling = (*itSibling).second.GetBound().CenterPoint();
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
	DWORD newKey = nwNode.GetKey();

	SelectionChanged(nwNode.GetKey(), true, ShowSubBranch());
	SetModifiedFlag();

	iHint hint;
	hint.event = iHint::rectAdd;
	hint.str = nwNode.GetName();
	UpdateAllViews(NULL, (LPARAM)nwNode.GetKey(), &hint);

	iLink l;
	l.SetNodes((*itRoot).second.GetBound(), nwNode.GetBound(), (*itRoot).second.GetKey(), nwNode.GetKey());
	l.SetDrawable();
	l.SetKey(lastLinkKey++);
	ResolveLinkLineStyle(l);
	ResolveLinkArrowStyle(l);

	links_.push_back(l);

	ListupChainNodes(false);
	CalcEdges();

	nodes_.FixReversibly(newKey);
	for (int i = 0; i < 100; i++) {
		RelaxSingleStep2();
	}
	nodes_.RestoreFixState(newKey);
	SelectionChanged(nwNode.GetKey(), true, ShowSubBranch());

	node_iter nit = nodes_.find(nwNode.GetKey());
	return (*nit).second.GetBound();
}

// ACTION : addNodeXXを一元化するメソッド(今後リファクタリング)
// Notifyをオプショナルにするか？？
const iNode& iEditDoc::InsertNode(const int nodeType, const CString &name, const CPoint &pt)
{
	iNode n(name);
	n.SetKey(AssignNewKey());
	n.SetParentKey(nodes_.GetCurrentParent());
	n.SetShape(nodeType);
	n.MoveBound(CSize(pt.x, pt.y));
	n.SetVisible();
	nodes_[n.GetKey()] = n;

	if (ShowSubBranch()) {
		m_visibleKeys.insert(n.GetKey());
	}
	node_c_iter nit = nodes_.find(n.GetKey());
	return ((*nit).second);
}

void iEditDoc::RelaxSingleStep2()
{
	////// ばねモデル処理
	link_iter li = links_.begin();
	for (; li != links_.end(); li++) {
		if ((*li).GetArrowStyle() == iLink::other ||
			(*li).GetFromNodeKey() == (*li).GetToNodeKey() ||
			!(*li).CanDraw()) {
			continue;
		}
		if (!(*li).IsInChain()) continue;

		node_iter itFrom = nodes_.FindWrite((*li).GetFromNodeKey());
		node_iter itTo = nodes_.FindWrite((*li).GetToNodeKey());

		double gxto = (*itTo).second.GetPrevBound().CenterPoint().x;
		double gyto = (*itTo).second.GetPrevBound().CenterPoint().y;
		double gxfrom = (*itFrom).second.GetPrevBound().CenterPoint().x;
		double gyfrom = (*itFrom).second.GetPrevBound().CenterPoint().y;

		double vx = gxto - gxfrom;
		double vy = gyto - gyfrom;
		double len = sqrt(vx*vx + vy * vy);
		len = (len == 0) ? .0001 : len;
		double f = ((*li).GetBoundStrength() - len) / (len * 3);
		double dx = f * vx;
		double dy = f * vy;
		(*itTo).second.dx += dx;
		(*itTo).second.dy += dy;
		(*itFrom).second.dx -= dx;
		(*itFrom).second.dy -= dy;
	}

	srand((unsigned)time(NULL));
	node_iter nit1 = nodes_.begin();
	for (; nit1 != nodes_.end(); nit1++) {
		if (!(*nit1).second.Visible()) continue;
		if (!(*nit1).second.IsInChain()) continue;

		double dx = 0; double dy = 0;
		node_iter nit2 = nodes_.begin();
		for (; nit2 != nodes_.end(); nit2++) {
			if (nit1 == nit2) { continue; }
			if (!(*nit2).second.Visible()) continue;
			if (!(*nit2).second.IsInChain()) continue;

			double gx1 = (*nit1).second.GetPrevBound().CenterPoint().x;
			double gy1 = (*nit1).second.GetPrevBound().CenterPoint().y;
			double gx2 = (*nit2).second.GetPrevBound().CenterPoint().x;
			double gy2 = (*nit2).second.GetPrevBound().CenterPoint().y;

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

	node_iter nit = nodes_.begin();
	for (; nit != nodes_.end(); nit++) {
		if (!(*nit).second.Visible()) continue;
		if (!(*nit).second.IsInChain()) continue;
		double x = max(-5, min(5, (*nit).second.dx));
		double y = max(-5, min(5, (*nit).second.dy));
		CRect rc = (*nit).second.GetPrevBound();
		rc.OffsetRect((int)x, (int)y);
		// 領域のチェック
		if (rc.left < 0) {
			rc.right = (*nit).second.GetPrevBound().Width();
			rc.left = 0;
		}
		if (rc.top < 0) {
			rc.bottom = (*nit).second.GetPrevBound().Height();
			rc.top = 0;
		}
		if (!(*nit).second.Fixed()) {
			/*const_cast<iNode&>*/(*nit).second.SetBound(rc);
		}
		/*const_cast<iNode&>*/(*nit).second.dx /= 2;
		/*const_cast<iNode&>*/(*nit).second.dy /= 2;
	}

	node_iter it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if (!(*it).second.Visible()) continue;
		if (!(*it).second.IsInChain()) continue;
		/*const_cast<iNode&>*/(*it).second.SetPrevBound((*it).second.GetBound());
	}
	SetConnectionPointForLayout();
}

int iEditDoc::GetKeyNodeLevelNumber(DWORD key)
{
	node_c_iter it = nodes_.FindRead(key);
	if (it != nodes_.end()) {
		return (*it).second.GetLevel();
	}
	return -1;
}

bool iEditDoc::HitTestSilently(const CPoint& pt)
{
	iNode* pNode = nodes_.HitTestSilently(pt);
	if (pNode != NULL) {
		return true;
	}
	return false;
}

void iEditDoc::ResolveLinkLineStyle(iLink& l)
{
	int lineStyle = ((CiEditApp*)AfxGetApp())->m_curLinkLineStyle;
	l.SetLineStyle(PS_SOLID);
	l.SetLineWidth(0);
	switch (lineStyle) {
	case CiEditApp::LS_R0:
		break;
	case CiEditApp::LS_DOT:
		l.SetLineStyle(PS_DOT);
		break;
	case CiEditApp::LS_R1:
		l.SetLineWidth(2);
		break;
	case CiEditApp::LS_R2:
		l.SetLineWidth(3);
		break;
	case CiEditApp::LS_R3:
		l.SetLineWidth(4);
		break;
	case CiEditApp::LS_R4:
		l.SetLineWidth(5);
		break;
	}
}

void iEditDoc::ResolveLinkArrowStyle(iLink& l)
{
	l.SetArrowStyle(GetAppLinkArrow());
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
	nodes_.ResizeSelectedFont(bEnLarge);
	SetConnectionPointVisibleLinks();
	CalcMaxPt(m_maxPt);
	SetModifiedFlag();
	iHint hint;
	hint.event = iHint::nodeFontResize;
	UpdateAllViews(NULL, (LPARAM)(nodes_.GetSelectedKey()), &hint);
}

void iEditDoc::ResizeSelectedLinkFont(bool bEnLarge)
{
	BackupNodesForUndo();
	BackupLinksForUndo();
	links_.ResizeSelectedFont(bEnLarge);
	SetConnectionPointVisibleLinks();
	CalcMaxPt(m_maxPt);
	SetModifiedFlag();
	iHint hint;
	hint.event = iHint::linkModified;
	UpdateAllViews(NULL, (LPARAM)(nodes_.GetSelectedKey()), &hint);
}

// スタイル変更の時に呼び出しているが・・？
void iEditDoc::SetConnectionPointVisibleLinks()
{
	link_iter li = links_.begin();
	for (; li != links_.end(); li++) {
		if (!(*li).CanDraw()) continue;
		node_iter itFrom = nodes_.FindWrite((*li).GetFromNodeKey());
		node_iter itTo = nodes_.FindWrite((*li).GetToNodeKey());
		if (itFrom == nodes_.end() || itTo == nodes_.end()) continue;

		if ((*li).GetFromNodeKey() == (*itFrom).second.GetKey()) {
			(*li).SetFromNodeRect((*itFrom).second.GetBound());
		}
		if ((*li).GetToNodeKey() == (*itTo).second.GetKey()) {
			(*li).SetToNodeRect((*itTo).second.GetBound());
		}
	}
}

OutlineView* iEditDoc::GetOutlineView() const
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
	node_iter it = nodes_.GetSelectedIter();
	CRect r = (*it).second.GetBound();
	node_iter itr = nodes_.begin();
	int drawOrder = 0;
	DWORD key = -1;
	for (; itr != nodes_.end(); itr++) {
		if (!(*itr).second.Visible()) continue;
		CRect bound = (*itr).second.GetBound();
		BOOL bInBound = bound.PtInRect(r.TopLeft()) &&
			bound.PtInRect(r.BottomRight());
		if (bInBound) {
			if (drawOrder < (*itr).second.GetDrawOrder()) {
				key = (*itr).second.GetKey();
				drawOrder = (*itr).second.GetDrawOrder();
			}
		}
	}
	iHint hint; hint.event = iHint::groupMigrate;
	hint.keyTarget = key;
	UpdateAllViews(NULL, (LPARAM)(*it).second.GetKey(), &hint);
}

void iEditDoc::SaveSelectedNodeFormat()
{
	node_iter n = nodes_.GetSelectedIter();
	m_nodeForFormat = iNode((*n).second);
}

void iEditDoc::ApplyFormatToSelectedNode()
{
	BackupNodesForUndo();
	node_iter n = nodes_.GetSelectedIter();
	(*n).second.SetLineColor(m_nodeForFormat.GetLineColor());
	(*n).second.SetLineColor(m_nodeForFormat.GetLineColor());
	(*n).second.SetLineStyle(m_nodeForFormat.GetLineStyle());
	(*n).second.SetFillColor(m_nodeForFormat.GetFillColor());
	(*n).second.ToggleFill(m_nodeForFormat.Filled());
	(*n).second.SetTextStyle(m_nodeForFormat.GetTextStyle());
	(*n).second.SetFontColor(m_nodeForFormat.GetFontColor());
	(*n).second.SetFontInfo(m_nodeForFormat.GetFontInfo());
	if (m_nodeForFormat.GetShape() == iNode::MetaFile) {
		HENHMETAFILE hMetaFile = m_nodeForFormat.GetMetaFile();
		(*n).second.SetMetaFile(hMetaFile);
	}
	(*n).second.SetShape(m_nodeForFormat.GetShape());
	(*n).second.SetMarginLeft(m_nodeForFormat.GetMarginLeft());
	(*n).second.SetMarginRight(m_nodeForFormat.GetMarginRight());
	(*n).second.SetMarginTop(m_nodeForFormat.GetMarginTop());
	(*n).second.SetMarginBottom(m_nodeForFormat.GetMarginBottom());
	SetConnectionPoint();
	CalcMaxPt(m_maxPt);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	UpdateAllViews(NULL, (LPARAM)nodes_.GetSelectedKey(), &hint);
}

void iEditDoc::SaveSelectedLinkFormat()
{
	link_c_iter l = links_.GetSelectedRead();
	m_linkForFormat.SetArrowStyle((*l).GetArrowStyle());
	m_linkForFormat.SetLineStyle((*l).GetLineStyle());
	m_linkForFormat.SetLineWidth((*l).SetLineWidth());
	m_linkForFormat.SetLinkColor((*l).GetLinkColor());
}

void iEditDoc::ApplyFormatToSelectedLink()
{
	BackupLinksForUndo();
	link_iter l = links_.GetSelectedWrite();
	(*l).SetArrowStyle(m_linkForFormat.GetArrowStyle());
	(*l).SetLineStyle(m_linkForFormat.GetLineStyle());
	(*l).SetLineWidth(m_linkForFormat.SetLineWidth());
	(*l).SetLinkColor(m_linkForFormat.GetLinkColor());
}

void iEditDoc::DeleteLinksInBound(const CRect& bound)
{
	links_.erase(remove_if(links_.begin(), links_.end(),
		iLink_inBound(bound)), links_.end());
	SetModifiedFlag();
	iHint h; h.event = iHint::linkDeleteMulti;
	UpdateAllViews(NULL, (LPARAM)nodes_.GetSelectedKey(), &h);
}

DWORD iEditDoc::DuplicateKeyNode(DWORD key)
{
	node_c_iter it = nodes_.FindRead(key);
	iNode n = (*it).second;
	n.SetKey(AssignNewKey());
	nodes_[n.GetKey()] = n;
	return n.GetKey();
}

void iEditDoc::DuplicateLinks(const NodeKeyMap& idm)
{
	NodeKeyMap::const_iterator it = idm.begin();
	for (; it != idm.end(); it++) {
		link_iter li = links_.begin();
		for (; li != links_.end(); li++) {
			if ((*it).first == (*li).GetFromNodeKey()) {
				NodeKeyMap::const_iterator pr = idm.find((*li).GetToNodeKey());
				if (pr != idm.end()) {
					iLink l = (*li);
					l.SetKeyFrom((*it).second);
					l.SetKeyTo((*pr).second);
					l.SetKey(lastLinkKey++);
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
	node_c_iter nit = nodes_.GetSelectedConstIter();
	if (nit != nodes_.end()) {
		l = (*nit).second.GetMarginLeft();
		r = (*nit).second.GetMarginRight();
		t = (*nit).second.GetMarginTop();
		b = (*nit).second.GetMarginBottom();
	}
}

void iEditDoc::SwapLinkOrder(DWORD key1, DWORD key2)
{
	link_c_iter li1 = links_.findByKey(key1);
	link_c_iter li2 = links_.findByKey(key2);
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
	nodes_.SetMarginToSelected(l, r, t, b);
	SetModifiedFlag();
	iHint hint; hint.event = iHint::nodeStyleChanged;
	DWORD key = nodes_.GetSelectedKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

CString iEditDoc::GetFileNameFromPath() const
{
	CString fullPath = GetPathName();
	CString drive, dir, fileName, ext;
	FileUtil::SplitPath(fullPath, drive, dir, fileName, ext);
	return CString(fileName);
}

bool iEditDoc::SaveCurrentImage(const CString& pngPath)
{
	CPoint p1(0, 0);
	CPoint p2 = GetMaxPt();
	p2 += CSize(10, 10);
	CRect rc(p1, p2);
	CImage image;
	image.Create(rc.Width(), rc.Height(), 32);

	CDC* pDC = CDC::FromHandle(image.GetDC());
	CBrush brush(RGB(255, 255, 255));
	pDC->FillRect(rc, &brush);

	DrawNodes(pDC);
	DrawLinks(pDC, true);
	image.ReleaseDC();
	image.Save(pngPath, Gdiplus::ImageFormatPNG);
	return true;
}

bool iEditDoc::WriteClickableMap(CStdioFile& f, const CString& textFileName, bool singleText)
{
	f.WriteString(nodes_.CreateClickableMapString(textFileName, singleText));
	return true;
}

void iEditDoc::FitSelectedNodeSize()
{
	BackupNodesForUndo();
	BackupLinksForUndo();
	node_iter it = nodes_.begin();
	for (; it != nodes_.end(); it++) {
		if ((*it).second.Selected()) {
			(*it).second.FitSizeToLabel();
		}
	}
	SetModifiedFlag();
	SetConnectionPoint();
	CalcMaxPt(m_maxPt);
	iHint hint; hint.event = iHint::reflesh;
	DWORD key = nodes_.GetSelectedKey();
	UpdateAllViews(NULL, (LPARAM)key, &hint);
}

int iEditDoc::GetSelectedNodeScrollPos() const
{
	node_c_iter it = nodes_.GetSelectedConstIter();
	if (it != nodes_.end()) {
		return (*it).second.GetTextTopPos();
	}
	return 0;
}

void iEditDoc::SetSelectedNodeScrollPos(int pos)
{
	node_iter it = nodes_.GetSelectedIter();
	if (it != nodes_.end()) {
		(*it).second.SetTextTopPos(pos);
	}
}

void iEditDoc::DivideTargetLink(DWORD key)
{
	links_.DivideTargetLinks(key, lastLinkKey++);
	SetConnectionPoint();
	CalcMaxPt(m_maxPt);
	SetModifiedFlag();
}

void iEditDoc::SetSelectedNodeDragging(bool dragging)
{
	nodes_.DragSelected(dragging);
}

bool iEditDoc::SaveJson(const CString& outPath)
{
	OutlineView* pView = GetOutlineView();

	NodePropsVec ls;
	pView->SerializeTree(ls);
	return JsonProcessor::Save(outPath, false, nodes_, links_, ls);
}

bool iEditDoc::ImportJson(const CString &filename, bool replace)
{
	prepareImport();
	CWaitCursor wc;
	JsonProcessor processor(nodesImport, linksImport, lastKey, idcVec);
	if (processor.Import(filename)) {
		// AddImportedData(replace);
		return true;
	}
	return false;
}
