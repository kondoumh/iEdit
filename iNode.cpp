// iNode.cpp: iNode クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "iEdit.h"
#include "iNode.h"
#include "Token.h"
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define REGS_NODE _T("Node Properties")

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

iNode::iNode()
{
	bound_.left = 0;
	bound_.top = 0; 
	bound_.right = 10;
	bound_.bottom = 10;
	
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	memset(&lf_, 0, sizeof(LOGFONT));
	::lstrcpy(lf_.lfFaceName, pApp->m_rgsNode.lf.lfFaceName);
	lf_.lfHeight = pApp->m_rgsNode.lf.lfHeight;
	lf_.lfWidth = pApp->m_rgsNode.lf.lfWidth;
	lf_.lfItalic = pApp->m_rgsNode.lf.lfItalic;
	lf_.lfUnderline = pApp->m_rgsNode.lf.lfUnderline;
	lf_.lfStrikeOut = pApp->m_rgsNode.lf.lfStrikeOut;
	lf_.lfCharSet= pApp->m_rgsNode.lf.lfCharSet;
	lf_.lfWeight = pApp->m_rgsNode.lf.lfWeight;
	colorFont = pApp->m_rgsNode.colorFont;
	styleText = pApp->m_rgsNode.styleText;
	
	colorLine = pApp->m_rgsNode.colorLine;
	colorFill = pApp->m_rgsNode.colorFill;
	
	lineWidth = pApp->m_rgsNode.lineWidth;
	styleLine = pApp->m_rgsNode.styleLine;
	selected_ = false;
	visible = false;
	bfillcolor = pApp->m_rgsNode.bFillColor;
	shape_ = pApp->m_rgsNode.shape;
	dx = 0.0;
	dy = 0.0;
	fixed_ = FALSE;
	hMF_ = NULL;
	drawOrder_ = 0;
}

iNode::iNode(const CString &name)
{
	bound_.left = 0;
	bound_.top = 0; 
	bound_.right = 10;
	bound_.bottom = 10;
	
	name_ = name;
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	memset(&lf_, 0, sizeof(LOGFONT));
	::lstrcpy(lf_.lfFaceName, pApp->m_rgsNode.lf.lfFaceName);
	lf_.lfHeight = pApp->m_rgsNode.lf.lfHeight;
	lf_.lfWidth = pApp->m_rgsNode.lf.lfWidth;
	lf_.lfItalic = pApp->m_rgsNode.lf.lfItalic;
	lf_.lfUnderline = pApp->m_rgsNode.lf.lfUnderline;
	lf_.lfStrikeOut = pApp->m_rgsNode.lf.lfStrikeOut;
	lf_.lfCharSet= pApp->m_rgsNode.lf.lfCharSet;
	lf_.lfWeight = pApp->m_rgsNode.lf.lfWeight;
	colorFont = pApp->m_rgsNode.colorFont;
	styleText = pApp->m_rgsNode.styleText;
	
	colorLine = pApp->m_rgsNode.colorLine;
	colorFill = pApp->m_rgsNode.colorFill;
	
	lineWidth = pApp->m_rgsNode.lineWidth;
	styleLine = pApp->m_rgsNode.styleLine;
	selected_ = false;
	visible = false;
	bfillcolor = pApp->m_rgsNode.bFillColor;
	shape_ = pApp->m_rgsNode.shape;
	dx = 0.0;
	dy = 0.0;
	fixed_ = FALSE;
	adjustFont();
	procMultiLine();
	hMF_ = NULL;
	drawOrder_ = 0;
}

iNode::~iNode()
{
	if (shape_ == iNode::MetaFile) {
		::DeleteEnhMetaFile(hMF_);
	}
}

iNode::iNode(const iNode & n)
{
	bound_ = n.bound_;
	key_ = n.key_;
	name_ = n.name_;
	parent_ = n.parent_;
	text_ = n.text_;
	treeState_ = n.treeState_;
	lf_ = n.lf_;
	::lstrcpy(lf_.lfFaceName, n.lf_.lfFaceName);
	colorFont = n.colorFont;
	styleText = n.styleText;
	colorLine = n.colorLine;
	lineWidth = n.lineWidth;
	styleLine = n.styleLine;
	bfillcolor = n.bfillcolor;
	colorFill = n.colorFill;

	selected_ = n.selected_;
	visible = n.visible;
	shape_ = n.shape_;
	dx = n.dx;
	dy = n.dy;
	fixed_ = n.fixed_;
	if (shape_ == iNode::MetaFile) {
		hMF_ = CopyEnhMetaFile(n.hMF_, NULL);
	}
	drawOrder_ = n.drawOrder_;
}

IMPLEMENT_SERIAL(iNode, CObject, 0)

void iNode::Serialize(CArchive &ar)
{
//	CObject::Serialize(ar);
	if (ar.IsStoring()) {
		CString fname(lf_.lfFaceName);
		ar << key_ << parent_ << treeState_ << bound_ << styleLine << styleText << shape_ << fixed_
		   << bfillcolor << colorFill << colorFont << colorLine << lineWidth << styleLine << name_ << text_
		   << lf_.lfCharSet << lf_.lfHeight << lf_.lfWidth << lf_.lfItalic << lf_.lfUnderline << lf_.lfStrikeOut << lf_.lfWeight
		   << fname;
		if (shape_ == iNode::MetaFile) {
			UINT hBits = GetEnhMetaFileBits(hMF_, NULL, NULL);
			BYTE *pData = new BYTE[hBits];
			UINT ret = GetEnhMetaFileBits(hMF_, hBits, pData);
			ar << hBits;
			for (unsigned int i = 0; i < hBits; i++) {
				ar << pData[i];
			}
			delete pData;
		}
	} else {
	//	UINT schema_number = ar.GetObjectSchema();
	//	CString s; s.Format("%d", schema_number);
	//	AfxMessageBox(s);
		
		CString fname;
		ar >> key_ >> parent_ >> treeState_ >> bound_ >> styleLine >> styleText >> shape_ >> fixed_
		   >> bfillcolor >> colorFill >> colorFont >> colorLine >> lineWidth >> styleLine >> name_ >> text_
		   >> lf_.lfCharSet >> lf_.lfHeight >> lf_.lfWidth >> lf_.lfItalic >> lf_.lfUnderline >> lf_.lfStrikeOut >> lf_.lfWeight
		   >> fname;
		::lstrcpy(lf_.lfFaceName, fname);
		if (shape_ == iNode::MetaFile) {
			UINT hBits;
			ar >> hBits;
			BYTE *pData = new BYTE[hBits];
			for (unsigned int i = 0; i < hBits; i++) {
				ar >> pData[i];
			}
			hMF_ = SetEnhMetaFileBits(hBits, pData);
			delete pData;
		}
	}
}

iNode& iNode::operator =(const iNode &n)
{
	bound_ = n.bound_;
	key_ = n.key_;
	name_ = n.name_;
	parent_ = n.parent_;
	text_ = n.text_;
	treeState_ = n.treeState_;
	lf_ = n.lf_;
	::lstrcpy(lf_.lfFaceName, n.lf_.lfFaceName);
	colorFont = n.colorFont;
	styleText = n.styleText;
	colorLine = n.colorLine;
	lineWidth = n.lineWidth;
	styleLine = n.styleLine;
	bfillcolor = n.bfillcolor;
	colorFill = n.colorFill;

	selected_ = n.selected_;
	visible = n.visible;
	shape_ = n.shape_;
	dx = n.dx;
	dy = n.dy;
	fixed_ = n.fixed_;
	if (shape_ == iNode::MetaFile) {
		hMF_ = CopyEnhMetaFile(n.hMF_, NULL);
	}
	return *this;
}

void iNode::setFontInfo(const LOGFONT &lf)
{
	LONG pre = lf_.lfHeight;
	lf_ = lf;
	::lstrcpy(lf_.lfFaceName, lf.lfFaceName);
	double rate = (double)lf_.lfHeight/(double)pre;
	double cx = ((double)(bound_.Width()))*rate;
	double cy = ((double)(bound_.Height()))*rate;
	bound_.right = bound_.left + (LONG)cx;
	bound_.bottom = bound_.top + (LONG)cy;
}

void iNode::setName(const CString &name)
{
	int preLength = name_.GetLength();
	name_ = name;
	adjustFont();
	procMultiLine();
}

void iNode::adjustFont(bool bForceResize)
{
	if (styleText == m_l || styleText == m_r || styleText == m_c) {
		if (name_.GetLength() > 80) return;
	}
	CSize sz = getNodeTextSize();
	LONG hmargin = sz.cy*4/7;
	LONG wmargin = sz.cy;
	if (lstrcmp(lf_.lfFaceName,"メイリオ") == 0) {
		hmargin = sz.cy*2/3;
	}
	if (!bfillcolor && styleLine == PS_NULL) {
		hmargin /= 2;
		wmargin /= 2;
	}
	LONG width = sz.cx + wmargin;
	if (width > bound_.Width() || 
		!bfillcolor && styleLine == PS_NULL ||
		bForceResize) {
		bound_.right = bound_.left + width;
	}
	LONG height = sz.cy + hmargin;
	if (height > bound_.Height() ||
		!bfillcolor && styleLine == PS_NULL ||
		bForceResize) {
		bound_.bottom = bound_.top + height;
	}
}

CSize iNode::getNodeTextSize()
{
	CWnd wnd;
	CFont font; font.CreateFontIndirectA(&lf_);
	CClientDC dc(&wnd);
	CFont* pOldFont = dc.SelectObject(&font);
	CSize sz = dc.GetTabbedTextExtentA(name_, -1, 0, NULL);
	dc.SelectObject(pOldFont);
	return sz;
}

void iNode::procMultiLine()
{
	CSize sz = getNodeTextSize();
	LONG hmargin = sz.cy*4/7;
	LONG wmargin = sz.cy;
	if (lstrcmp(lf_.lfFaceName,"メイリオ") == 0) {
		hmargin = sz.cy*4/5;
	}
	int lineCount, maxLength;
	getInnerLineInfo(name_, lineCount, maxLength);
	if (styleText != m_l && styleText != m_r && styleText != m_c) {
		if (lineCount <= 1) {
			procMultiLineInner(sz, wmargin, hmargin);
		}
	} else {
		if (lineCount <= 1) {
			procMultiLineInner(sz, wmargin, hmargin);
		} else {
			int width = sz.cx*maxLength/name_.GetLength() + wmargin;
			int height = sz.cy*(lineCount - 1) + hmargin;
			bound_.right = bound_.left + width;
			bound_.bottom = bound_.top + height;
		}
	}
}

void iNode::procMultiLineInner(const CSize& sz, int wmargin, int hmargin)
{
	if (name_.GetLength() > 80) {
		int width = sz.cx/name_.GetLength()*24 + wmargin;
		int height = sz.cy*((name_.GetLength()+1)/24) + hmargin;
		if (bound_.Height()*bound_.Width() < height*width || 
			bound_.Width()/bound_.Height() > 8.0) {
			styleText = iNode::m_c;
			bound_.right = bound_.left + width;
			bound_.bottom = bound_.top + height;
		}
	}
}

void iNode::getInnerLineInfo(const CString& str, int& lineCount, int& maxLength)
{
	CToken tok(str);
	tok.SetToken("\n");
	lineCount = 1;
	maxLength = 0;
	for ( ; tok.MoreTokens(); lineCount++) {
		CString line = tok.GetNextToken();
		if (maxLength < line.GetLength()) {
			maxLength = line.GetLength();
		}
	}
}

void iNode::setTextStyle(int s)
{
	int pre = styleText;
	styleText = s;

	if (s != iNode::m_c && s != iNode::m_l && s != iNode::m_r && 
		(pre == iNode::m_c || pre == iNode::m_l || pre == iNode::m_r) ) {
		adjustFont(true);
	} else if ((s == iNode::m_c || s == iNode::m_l || s == iNode::m_r) &&
		pre != iNode::m_c && pre != iNode::m_l && pre != iNode::m_r) {
		procMultiLine();
	}
}

bool iNode::operator ==(iNode &n)
{
	return key_ == n.getKey();
}

bool iNode::operator <(iNode &n)
{
	return key_ < n.key_;
}

void iNode::setMetaFile(HENHMETAFILE& hMF)
{
	hMF_ = CopyEnhMetaFile(hMF, NULL);
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	
	ENHMETAHEADER h;
	::GetEnhMetaFileHeader(hMF_, /*sz*/sizeof(h), &h);
	RECTL b = h.rclBounds;
	bound_.bottom = bound_.top + abs(b.bottom - b.top);
	bound_.right = bound_.left + abs(b.right - b.left);
}


HENHMETAFILE iNode::getMetaFile() const
{
	return hMF_;
}

void iNode::backupState()
{
	fixed_back_ = fixed_;
}

void iNode::restoreState()
{
	fixed_ = fixed_back_;
}


// iNodeDrawer クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
void iNodeDrawer::draw(const iNode &node, CDC *pDC, BOOL bDrawOrderInfo)
{
	int oldBkMode = pDC->SetBkMode(TRANSPARENT); // DCの背景色透明
	
	if (node.isFilled()) {
		fillBound(node, pDC);
	}
	drawShape(node, pDC);
	adjustTextArea(node);
	drawLabel(node, pDC, bDrawOrderInfo);
	
	pDC->SetBkMode(oldBkMode); // DCの背景リストア
}

void iNodeDrawer::adjustTextArea(const iNode &node)
{
	m_textRect = node.getBound();
	m_textRect.left += node.getLineWidth() + 1;
	m_textRect.right -= node.getLineWidth() + 1;
	m_textRect.top += node.getLineWidth() + 1;
	m_textRect.bottom -= node.getLineWidth() + 1;
}

void iNodeDrawer::fillBound(const iNode & node, CDC *pDC)
{
	CBrush brush(node.getBrsColor());	// ブラシ作成
	CBrush* brsOld = pDC->SelectObject(&brush);  // DCのブラシ変更
	
	fillBoundSpecific(node, pDC, &brush); // サブクラスで処理
	
	pDC->SelectObject(brsOld);             // DCのブラシリストア
	brush.DeleteObject();	                  // ブラシ開放
}

void iNodeDrawer::drawShape(const iNode &node, CDC *pDC)
{
	CPen penLine;
	penLine.CreatePen(node.getLineStyle(),
		node.getLineWidth(), node.getLineColor()); // ペン作成
	CPen * 	pOldPen = pDC->SelectObject(&penLine); // DCのペン変更
	
	drawShapeSpecific(node, pDC, &penLine); // サブクラスで処理 メタファイルはここで再生
	
	pDC->SelectObject(pOldPen);  // DCのペンリストア
	penLine.DeleteObject();          // ペン開放
}

void iNodeDrawer::drawLabel(const iNode &node, CDC *pDC, BOOL bDrawOrderInfo)
{
	// フォント作成
	LOGFONT lf = node.getFontInfo();
	CFont font;
	font.CreateFont(lf.lfHeight, lf.lfWidth, 0, 0, lf.lfWeight, lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		             OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, lf.lfFaceName);

	
	// デバイスコンテキストのフォント変更
	CFont* pOldFont = pDC->SelectObject(&font);
	COLORREF preColor = pDC->SetTextColor(node.getFontColor());
	
	drawLabelSpecific(node, pDC);
	
	pDC->SelectObject(pOldFont);
	pDC->SetTextColor(preColor);
	font.DeleteObject();
	
	if (bDrawOrderInfo) {
		font.CreatePointFont(90, "MS Gothic", pDC);
		pOldFont = pDC->SelectObject(&font);
		preColor = pDC->SetTextColor(RGB(90, 90, 90));
		CString test; test.Format("%d", node.getDrawOrder());
		pDC->TextOut(node.getBound().left+ 2, node.getBound().top + 2, test);
		pDC->SelectObject(pOldFont);
		pDC->SetTextColor(preColor);
		font.DeleteObject();
	}
}

void iNodeDrawer::drawLabelSpecific(const iNode &node, CDC *pDC)
{
	int styleText = node.getTextStyle();
	UINT nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
	
	switch (styleText) {
	case iNode::s_cc:
		nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE;
		break;
	
	case iNode::s_cl:
		nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE;
		break;
	
	case iNode::s_cr:
		nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE;
		break;
	
	case iNode::s_tc:
		nFormat =DT_CENTER | DT_TOP | DT_SINGLELINE;
		break;
	
	case iNode::s_tl:
		nFormat = DT_LEFT | DT_TOP | DT_SINGLELINE;
		break;
	
	case iNode::s_tr:
		nFormat =DT_RIGHT | DT_TOP | DT_SINGLELINE;
		break;
	
	case iNode::s_bc:
		nFormat =DT_CENTER | DT_BOTTOM | DT_SINGLELINE;
		break;
	
	case iNode::s_bl:
		nFormat = DT_LEFT | DT_BOTTOM | DT_SINGLELINE;
		break;
	
	case iNode::s_br:
		nFormat = DT_RIGHT | DT_BOTTOM | DT_SINGLELINE;
		break;
	
	case iNode::m_c:
		nFormat = DT_CENTER | DT_WORDBREAK;
		break;
	
	case iNode::m_l:
		nFormat = DT_LEFT | DT_WORDBREAK;
		break;
	
	case iNode::m_r:
		nFormat = DT_RIGHT | DT_WORDBREAK;
		break;
	case iNode::notext:
		break;
	}
	
	if (styleText != iNode::notext) {
		pDC->DrawText(node.getName(), &m_textRect, nFormat);
	}
}

void iNodeDrawer::drawShapeSpecific(const iNode &node, CDC *pDC, const CPen* pen)
{

}

void iNodeDrawer::fillBoundSpecific(const iNode &node, CDC *pDC, CBrush* brush)
{

}

// iNodeRectDrawer クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
void iNodeRectDrawer::fillBoundSpecific(const iNode &node, CDC *pDC, CBrush *brush)
{
	if (node.isFilled()) {
		pDC->FillRect(node.getBound(), brush);
	}
}

void iNodeRectDrawer::drawShapeSpecific(const iNode &node, CDC *pDC, const CPen *pen)
{
	CRect bound = node.getBound();
	pDC->MoveTo(bound.TopLeft());
	pDC->LineTo(bound.right, bound.top);
	pDC->LineTo(bound.BottomRight());
	pDC->LineTo(bound.left, bound.bottom);
	pDC->LineTo(bound.TopLeft());
}

// iNodeRoundRectDrawer クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
void iNodeRoundRectDrawer::drawShape(const iNode &node, CDC *pDC)
{
	CPen penLine;
	penLine.CreatePen(node.getLineStyle(),
		node.getLineWidth(), node.getLineColor()); // ペン作成
	CPen * 	pOldPen = pDC->SelectObject(&penLine); // DCのペン変更
	
	CBrush* pBrsOld;
	HGDIOBJ hGdiObj;
	CBrush brs(node.getBrsColor());
	if (node.isFilled()) {
		pBrsOld = pDC->SelectObject(&brs);
	} else {
		hGdiObj = SelectObject(pDC->m_hDC, GetStockObject(NULL_BRUSH));
	}
	CRect bound = node.getBound();
	m_r = (bound.Width() < bound.Height()) ? bound.Width() : bound.Height();
	pDC->RoundRect(bound, CPoint(m_r/4, m_r/4));
	
	if (node.isFilled()) {
		pDC->SelectObject(pBrsOld);
	} else {
		SelectObject(pDC->m_hDC, hGdiObj);
	}
	
	pDC->SelectObject(pOldPen);  // DCのペンリストア
	penLine.DeleteObject();          // ペン開放
}

void iNodeRoundRectDrawer::adjustTextArea(const iNode &node)
{
	iNodeDrawer::adjustTextArea(node);
	switch (node.getTextStyle()) {
	case iNode::s_tl:
		m_textRect.left += m_r/8;
		break;
	
	case iNode::s_tr:
		m_textRect.right -= m_r/8;
		break;
	
	case iNode::s_bl:
		m_textRect.left += m_r/8;
		break;
	
	case iNode::s_br:
		m_textRect.right -= m_r/8;
		break;
	}
}

// iNodeArcDrawer クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

void iNodeArcDrawer::fillBoundSpecific(const iNode &node, CDC *pDC, CBrush *brush)
{
	CPen penArc;
	penArc.CreatePen(node.getLineStyle(), node.getLineWidth(), node.getBrsColor());
	CPen* pOldPen = pDC->SelectObject(&penArc);
	pDC->Ellipse(node.getBound());
	pDC->SelectObject(pOldPen);
	penArc.DeleteObject();
}

void iNodeArcDrawer::drawShapeSpecific(const iNode &node, CDC *pDC, const CPen *pen)
{
	CRect bound = node.getBound();
	pDC->Arc(&bound, bound.TopLeft(), bound.TopLeft());
}

void iNodeArcDrawer::adjustTextArea(const iNode &node)
{
	m_textRect = node.getBound();
	m_textRect.left += node.getBound().Width()/16;
	m_textRect.right -= node.getBound().Width()/16;
	m_textRect.top += node.getBound().Height()/16;
	m_textRect.bottom -= node.getBound().Height()/16;
	
	switch (node.getTextStyle()) {
	case iNode::s_tl:
		m_textRect.top += node.getBound().Height()*3/16;
		break;
	
	case iNode::s_tr:
		m_textRect.top += node.getBound().Height()*3/16;
		break;
	
	case iNode::s_bl:
		m_textRect.bottom -= node.getBound().Height()*3/16;
		break;
	
	case iNode::s_br:
		m_textRect.bottom -= node.getBound().Height()*3/16;
		break;
	}
}

// iNodeMetafileDrawer クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
void iNodeMetafileDrawer::drawShape(const iNode &node, CDC *pDC)
{
	pDC->PlayMetaFile(node.getMetaFile(), node.getBound());
}

void iNodeMetafileDrawer::fillBoundSpecific(const iNode &node, CDC *pDC, CBrush *brush)
{
	// Do Nothing
}


// iNodeMMNodeDrawer クラスのインプリメンテーション
void iNodeMMNodeDrawer::drawShapeSpecific(const iNode &node, CDC* pDC, const CPen *pen)
{
	pDC->MoveTo(node.getBound().left, node.getBound().bottom);
	pDC->LineTo(node.getBound().right, node.getBound().bottom);
}

// iNodes クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

void iNodes::setSelKey(DWORD key)
{
	selKey_ = key;
	nodeFind.setKey(selKey_);
	niterator it = begin();
	for ( ; it != end(); it++) {
		if (selKey_ == (*it).getKey()) {
			(*it).selectNode();
			curParent_ = (*it).getParent();
		} else {
			(*it).selectNode(false);
		}
	}
}

niterator iNodes::getSelectedNode()
{
	return findNodeW(nodeFind);
}

const_niterator iNodes::getSelectedNodeR() const
{
	return findNode(nodeFind);
}

void iNodes::initSelection()
{
	if (size() == 1) {
		selKey_ = 0;
		nodeFind.setKey(selKey_);
		setSelKey(selKey_);
	} else {
		const_niterator it = begin();
		for ( ; it != end(); it++) {
			if ((*it).getTreeState() & TVIS_SELECTED) {
				// TVIS_EXPANDEDと間違えてた
				selKey_ = (*it).getKey();
				nodeFind.setKey(selKey_);
				curParent_ = (*it).getParent();
				break;
			}
		}
	}
}

void iNodes::drawNodes(CDC *pDC, bool bDrwAll)
{
	vector<iNode*>::iterator it = nodesDraw_.begin();
	for ( ; it != nodesDraw_.end(); it++) {
		iNodeDrawer* pDrawer = getNodeDrawer(*(*it));
		pDrawer->draw(*(*it), pDC, m_bDrawOrderInfo);
	}
}

niterator iNodes::hitTest(const CPoint &pt, bool bTestAll)
{
	niterator it = begin();
	for ( ; it != end(); it++) {
		(*it).selectNode(false);
	}
	
	it = begin();
	CRect preRc(0, 0, 0, 0);
	niterator it_inner = end();
	for ( ; it != end(); it++) {
//		if (!bTestAll) {
			if (!(*it).isVisible()) {
				continue;
			}
//		}
		CRect rc = (*it).getBound();
		rc.left -= 1;
		rc.right += 1;
		rc.top -= 1;
		rc.bottom += 1;
		if (rc.PtInRect(pt)) {
			if (!preRc.IsRectNull()) {
				CRect andRc = preRc & rc;
				if (andRc.Width() < preRc.Width() && andRc.Height() < preRc.Height()) {
					it_inner = it;
					preRc = rc;
				}
			} else {
				it_inner = it;
				preRc = rc;
			}
		}
	}
	if (it_inner != end()) {
		(*it_inner).selectNode();
		selKey_ = (*it_inner).getKey();
		curParent_ = (*it_inner).getParent();
	}
	return it_inner;
}

const_niterator iNodes::hitTest2(const CPoint &pt, bool bTestAll) const
{
	const_niterator it = begin();
	CRect preRc(0, 0, 0, 0);
	const_niterator it_inner = end();
	for ( ; it != end(); it++) {
//		if (!bTestAll) {
			if (!(*it).isVisible()) {
				continue;
			}
//		}
		CRect rc = (*it).getBound();
		rc.left -= 1;
		rc.right += 1;
		rc.top -= 1;
		rc.bottom += 1;
		if (rc.PtInRect(pt)) {
			if (!preRc.IsRectNull()) {
				CRect andRc = preRc & rc;
				if (andRc.Width() < preRc.Width() && andRc.Height() < preRc.Height()) {
					it_inner = it;
					preRc = rc;
				}
			} else {
				it_inner = it;
				preRc = rc;
			}
		}
	}
	
	return it_inner;
}

void iNodes::moveSelectedNode(const CSize &sz)
{
	niterator it = begin();
	for ( ; it != end(); it++) {
		if ((*it).isSelected()) {
			(*it).moveBound(sz);
		}
	}
}

void iNodes::setSelectedNodeBound(const CRect &r)
{
	niterator it = findNodeW(nodeFind);
	if (it != end()) {
		(*it).setBound(r);
	}
}

void iNodes::setSelectedNodeFont(const LOGFONT &lf)
{
	niterator it = begin();
	for ( ; it != end(); it++) {
		if ((*it).isSelected()) {
			(*it).setFontInfo(lf);
		}
	}
}

void iNodes::getSelectedNodeFont(LOGFONT& lf)
{
	const_niterator it = findNode(nodeFind);
	if (it != end()) {
		lf = (*it).getFontInfo();
		::lstrcpy(lf.lfFaceName, (*it).getFontInfo().lfFaceName);
	}
}


COLORREF iNodes::getSelectedNodeFontColor() const
{
	COLORREF c = RGB(0, 0, 0);
	const_niterator it = findNode(nodeFind);
	if (it != end()) {
		c = (*it).getFontColor();
	}
	return c;
}

void iNodes::setSelectedNodeFontColor(const COLORREF &c)
{
	niterator it = begin();
	for ( ; it != end(); it++) {
		if ((*it).isSelected()) {
			(*it).setFontColor(c);
		}
	}
}

void iNodes::setSelectedNodeBrush(const COLORREF &c)
{
	niterator it = begin();
	for ( ; it != end(); it++) {
		if ((*it).isSelected()) {
			(*it).setBrush(c);
		}
	}
}

void iNodes::setSelectedNodeNoBrush(BOOL noBrush)
{
	niterator it = begin();
	for ( ; it != end(); it++) {
		if ((*it).isSelected()) {
			(*it).setNoBrush(noBrush);
		}
	}
}

void iNodes::setSelectedNodeLineColor(const COLORREF &c)
{
	niterator it = begin();
	for ( ; it != end(); it++) {
		if ((*it).isSelected()) {
			(*it).setLineColor(c);
		}
	}
}

COLORREF iNodes::getSelectedNodeLineColor() const
{
	COLORREF c = RGB(0, 0, 0);
	const_niterator it = findNode(nodeFind);
	if (it != end()) {
		c = (*it).getLineColor();
	}
	return c;
}

COLORREF iNodes::getSelectedNodeBrsColor() const
{
	COLORREF c = RGB(255, 255, 255);
	const_niterator it = findNode(nodeFind);
	if (it != end()) {
		c = (*it).getBrsColor();
	}
	return c;
}

void iNodes::setSelectedNodeLineStyle(int style)
{
	niterator it = begin();
	for ( ; it != end(); it++) {
		if ((*it).isSelected()) {
			(*it).setLineStyle(style);
		}
	}
}

int iNodes::getSelectedNodeLineStyle() const
{
	int s = PS_SOLID;
	const_niterator it = findNode(nodeFind);
	if (it != end()) {
		s = (*it).getLineStyle();
	}
	return s;
}

void iNodes::setSelectedNodeLineWidth(int w)
{
	niterator it = begin();
	for ( ; it != end(); it++) {
		if ((*it).isSelected()) {
			(*it).setLineWidth(w);
		}
	}
}


int iNodes::getSelectedNodeLineWidth() const
{
	int w = 0;
	const_niterator it = findNode(nodeFind);
	if (it != end()) {
		w = (*it).getLineWidth();
	}
	return w;
}

void iNodes::setSelectedNodeTextStyle(int style)
{
	niterator it = begin();
	for ( ; it != end(); it++) {
		if ((*it).isSelected()) {
			(*it).setTextStyle(style);
		}
	}
}

int iNodes::getSelectedNodeTextStyle() const
{
	int s = iNode::s_cc;
	const_niterator it = findNode(nodeFind);
	if (it != end()) {
		s = (*it).getTextStyle();
	}
	return s;
}

int iNodes::selectNodesInBound(const CRect &bound, CRect &selRect, bool bDrwAll)
{
	niterator it = begin();
	int cnt=0;
	for ( ; it != end(); it++) {
		CRect r = (*it).getBound();
//		if (!bDrwAll) {
			if (!(*it).isVisible()) continue;
//		}
		if ((r | bound) == bound /*r.IntersectRect(r, bound)*/) {
			(*it).selectNode();
			selRect = (*it).getBound();
			cnt++;
		} else {
			(*it).selectNode(false);
		}
	}
	
	it = begin();
	for ( ; it != end(); it++) {
		if (/*(*it).isVisible() && */(*it).isSelected()) {
			if ((*it).getBound().left < selRect.left) selRect.left = (*it).getBound().left;
			if ((*it).getBound().right > selRect.right) selRect.right = (*it).getBound().right;
			if ((*it).getBound().top < selRect.top) selRect.top = (*it).getBound().top;
			if ((*it).getBound().bottom > selRect.bottom) selRect.bottom = (*it).getBound().bottom;
		}
	}
	return cnt;
}

bool orderComp(const iNode* n1, const iNode* n2)
{
	return n1->getDrawOrder() < n2->getDrawOrder();
}

void iNodes::setVisibleNodes(DWORD key)
{
	nodesDraw_.clear();
	nodesDraw_.resize(0);
	
	niterator it = begin();
	for ( ; it != end(); it++) {
		if ((*it).getParent() != curParent_) {
			(*it).setVisible(false);
		} else {
			(*it).setVisible();
			unsigned int order = 0;
			vector<DWORD>::iterator ik = std::find(svec_.begin(), svec_.end(), (*it).getKey());
			if (ik != svec_.end()) {
				order = std::distance(svec_.begin(), ik);
			}
			(*it).setDrawOrder(order);
			nodesDraw_.push_back(&(*it));
		}
	}
	if (((CiEditApp*)AfxGetApp())->m_rgsNode.bSyncOrder) {
		std::sort(nodesDraw_.begin(), nodesDraw_.end(), orderComp);
	}
}

void iNodes::setVisibleNodes(KeySet& keySet)
{
	nodesDraw_.clear();
	nodesDraw_.resize(0);
	
	niterator it = begin();
	for ( ; it != end(); it++) {
		(*it).setVisible(false);
	}
	
	set<DWORD>::iterator itks = keySet.begin();
	iNode nf;
	niterator nit;
	for ( ; itks != keySet.end(); itks++) {
		nf.setKey(*itks);
		nit = findNodeW(nf);
		if (nit != end()) {
			(*nit).setVisible(true);
			unsigned int order = 0;
			vector<DWORD>::iterator ik = std::find(svec_.begin(), svec_.end(), (*nit).getKey());
			if (ik != svec_.end()) {
				order = std::distance(svec_.begin(), ik);
			}
			(*nit).setDrawOrder(order);
			nodesDraw_.push_back(&(*nit));
		}
	}
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	if (((CiEditApp*)AfxGetApp())->m_rgsNode.bSyncOrder) {
		std::sort(nodesDraw_.begin(), nodesDraw_.end(), orderComp);
	}
}

BOOL iNodes::isSelectedNodeFilled() const
{
	const_niterator it = begin();
	for ( ; it != end(); it++) {
		if ((*it).isSelected()) {
			return ((*it).isFilled());
		}
	}
	return FALSE;
}

int iNodes::getSelectedNodeShape() const
{
	int shape = iNode::rectangle;
	const_niterator it = findNode(nodeFind);
	if (it != end()) {
		shape = (*it).getNodeShape();
	}
	return shape;
}

void iNodes::setSelectedNodeShape(int shape)
{
	niterator it = begin();
	for ( ; it != end(); it++) {
		if ((*it).isSelected()) {
			(*it).setNodeShape(shape);
			if (shape > 9) {
				CiEditApp* pApp = (CiEditApp*)AfxGetApp();
				(*it).setMetaFile(pApp->m_hMetaFiles[shape-10]);
			}
		}
	}
}

BOOL iNodes::isSelectedNodeFixed() const
{
	const_niterator it = findNode(nodeFind);
	BOOL fix = FALSE;
	if (it != end()) {
		fix = (*it).isFixed();
	}
	return fix;
}

void iNodes::setSelectedNodeFixed(BOOL f)
{
	niterator it = findNodeW(nodeFind);
	if (it != end()) {
		(*it).fix(f);
	}
}

const_niterator iNodes::findNode(const iNode &n) const
{
	return find(n);
}

niterator iNodes::findNodeW(const iNode &n)
{
	return find(n);
}

CSize iNodes::getMaxNodeSize(bool selection, bool bDrwAll) const
{
	CSize sz(0, 0);
	
	const_niterator it = begin();
	for ( ; it != end(); it++) {
//		if (!bDrwAll) {
			if (!(*it).isVisible()) {
				continue;
			}
//		}
		if (selection) {
			if (!(*it).isSelected()) {
				continue;
			}
		}
		CRect rc = (*it).getBound();
		if (sz.cx < rc.Width()) {
			sz.cx = rc.Width();
		}
		if (sz.cy < rc.Height()) {
			sz.cy = rc.Height();
		}
	}
	return sz;
}

iNodes::iNodes()
{
	m_pNodeRectDrawer = new iNodeRectDrawer;
	m_pNodeRoundRectDrawer = new iNodeRoundRectDrawer;
	m_pNodeArcDrawer = new iNodeArcDrawer;
	m_pMetafileDrawer = new iNodeMetafileDrawer;
	m_pMMNodeDrawer = new iNodeMMNodeDrawer;
	m_bDrawOrderInfo = FALSE;
}

iNodes::~iNodes()
{
	delete m_pNodeRectDrawer;
	delete m_pNodeRoundRectDrawer;
	delete m_pNodeArcDrawer;
	delete m_pMetafileDrawer;
	delete m_pMMNodeDrawer;
}

iNodeDrawer* iNodes::getNodeDrawer(const iNode &node)
{
	int shape = node.getNodeShape();
	if (shape == iNode::rectangle) {
		return m_pNodeRectDrawer;
	} else if (shape == iNode::roundRect) {
		return m_pNodeRoundRectDrawer;
	} else if (shape == iNode::arc) {
		return m_pNodeArcDrawer;
	} else if (shape == iNode::MetaFile) {
		return m_pMetafileDrawer;
	} else if (shape == iNode::MindMapNode) {
		return m_pMMNodeDrawer;
	}
	return m_pNodeRectDrawer;
}

void iNodes::drawNodesSelected(CDC *pDC)
{
	vector<iNode*>::iterator it = nodesDraw_.begin();
	for ( ; it != nodesDraw_.end(); it++) {
		if ((*(*it)).isSelected()) {
			iNodeDrawer* pDrawer = getNodeDrawer(*(*it));
			pDrawer->draw(*(*it), pDC, m_bDrawOrderInfo);
		}
	}
}

void iNodes::fixNodesReversibly(DWORD keyExcluded)
{
	niterator it = begin();
	for ( ; it != end(); it++) {
		if ((*it).isInChain()) {
			(*it).backupState();
			if ((*it).getKey() != keyExcluded) {
				(*it).fix();
			}
		}
	}
}

void iNodes::restoreNodesFixState(DWORD keyExcluded)
{
	niterator it = begin();
	for ( ; it != end(); it++) {
		if ((*it).isInChain()) {
			if ((*it).getKey() != keyExcluded) {
				(*it).restoreState();
			}
		}
	}
}

void iNodes::resizeSelectedNodeFont(bool bEnlarge)
{
	niterator it = begin();
	for ( ; it != end(); it++) {
		if ((*it).isSelected()) {
			LOGFONT lf = (*it).getFontInfo();
			LONG pre = lf.lfHeight;
			if (bEnlarge) {
				lf.lfHeight -= 2;
			} else {
				if (lf.lfHeight < -4) {
					lf.lfHeight += 2;
				}
			}
			(*it).setFontInfo(lf);
		}
	}
}

serialVec iNodes::getSelectedNodeKeys() const
{
	serialVec v;
	const_niterator it = begin();
	for ( ; it != end(); it++) {
		if ((*it).isSelected() && (*it).isVisible()) {
			v.push_back((*it).getKey());
		}
	}
	return v;
}
