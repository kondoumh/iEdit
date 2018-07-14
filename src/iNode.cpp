// iNode.cpp: iNode クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "iEdit.h"
#include "iNode.h"
#include "Token.h"
#include "iEdit.h"
#include "StringUtil.h"
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define REGS_NODE _T("Node Properties")

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

iNode::iNode()
{
	Init();
}

iNode::iNode(const CString &name)
{
	Init();
	name_ = name;
	AdjustFont(true);
}

void iNode::Init()
{
	bound_.left = 0;
	bound_.top = 0;
	bound_.right = 10;
	bound_.bottom = 5;

	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	memset(&lf_, 0, sizeof(LOGFONT));
	::lstrcpy(lf_.lfFaceName, pApp->m_rgsNode.lf.lfFaceName);
	lf_.lfHeight = pApp->m_rgsNode.lf.lfHeight;
	lf_.lfWidth = pApp->m_rgsNode.lf.lfWidth;
	lf_.lfItalic = pApp->m_rgsNode.lf.lfItalic;
	lf_.lfUnderline = pApp->m_rgsNode.lf.lfUnderline;
	lf_.lfStrikeOut = pApp->m_rgsNode.lf.lfStrikeOut;
	lf_.lfCharSet = pApp->m_rgsNode.lf.lfCharSet;
	lf_.lfWeight = pApp->m_rgsNode.lf.lfWeight;
	colorFont = pApp->m_rgsNode.colorFont;
	styleText = pApp->m_rgsNode.styleText;

	colorLine = pApp->m_rgsNode.colorLine;
	colorFill = pApp->m_rgsNode.colorFill;

	lineWidth = pApp->m_rgsNode.lineWidth;
	styleLine = pApp->m_rgsNode.styleLine;
	selected_ = false;
	visible = false;
	deleted_ = false;
	bfillcolor = pApp->m_rgsNode.bFillColor;
	shape_ = pApp->m_rgsNode.shape;
	dx = 0.0;
	dy = 0.0;
	fixed_ = FALSE;
	hMF_ = NULL;
	drawOrder_ = 0;
	nLevel_ = 0;
	treeIconId_ = 0;
	margin_l_ = pApp->m_rgsNode.margin_l;
	margin_r_ = pApp->m_rgsNode.margin_r;
	margin_t_ = pApp->m_rgsNode.margin_t;
	margin_b_ = pApp->m_rgsNode.margin_b;
	scrollpos_ = 0;
	dragging_ = false;
}

iNode::~iNode()
{
	if (shape_ == iNode::MetaFile) {
		::DeleteEnhMetaFile(hMF_);
	}
}

iNode::iNode(const iNode & n)
{
	CopyProps(n);
}

iNode& iNode::operator =(const iNode &n)
{
	CopyProps(n);
	return *this;
}

void iNode::CopyProps(const iNode &n)
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
	deleted_ = n.deleted_;
	shape_ = n.shape_;
	dx = n.dx;
	dy = n.dy;
	fixed_ = n.fixed_;
	if (shape_ == iNode::MetaFile) {
		hMF_ = CopyEnhMetaFile(n.hMF_, NULL);
	}
	drawOrder_ = n.drawOrder_;
	nLevel_ = n.nLevel_;
	treeIconId_ = n.treeIconId_;
	margin_l_ = n.margin_l_;
	margin_r_ = n.margin_r_;
	margin_t_ = n.margin_t_;
	margin_b_ = n.margin_b_;
	scrollpos_ = n.scrollpos_;
	dragging_ = n.dragging_;
}

IMPLEMENT_SERIAL(iNode, CObject, 0)

void iNode::Serialize(CArchive &ar)
{
	if (ar.IsStoring()) {
		if (deleted_) return;
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
	}
	else {
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

void iNode::SerializeEx(CArchive& ar, int version)
{
	if (ar.IsStoring()) {
		if (deleted_) return;
		CString fname(lf_.lfFaceName);
		ar << key_ << parent_ << treeState_ << bound_ << styleLine << styleText << shape_ << fixed_
			<< bfillcolor << colorFill << colorFont << colorLine << lineWidth << styleLine << name_ << text_
			<< lf_.lfCharSet << lf_.lfHeight << lf_.lfWidth << lf_.lfItalic << lf_.lfUnderline << lf_.lfStrikeOut << lf_.lfWeight
			<< fname;
		if (version > 1) {
			ar << nLevel_;
		}
		if (version > 3) {
			ar << treeIconId_;
		}
		if (version > 4) {
			ar << margin_l_ << margin_r_ << margin_t_ << margin_b_;
		}
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
	}
	else {
		CString fname;
		ar >> key_ >> parent_ >> treeState_ >> bound_ >> styleLine >> styleText >> shape_ >> fixed_
			>> bfillcolor >> colorFill >> colorFont >> colorLine >> lineWidth >> styleLine >> name_ >> text_
			>> lf_.lfCharSet >> lf_.lfHeight >> lf_.lfWidth >> lf_.lfItalic >> lf_.lfUnderline >> lf_.lfStrikeOut >> lf_.lfWeight
			>> fname;
		::lstrcpy(lf_.lfFaceName, fname);
		if (version > 1) {
			ar >> nLevel_;
		}
		if (version > 3) {
			ar >> treeIconId_;
		}
		if (version > 4) {
			ar >> margin_l_ >> margin_r_ >> margin_t_ >> margin_b_;
		}
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

void iNode::SetFontInfo(const LOGFONT &lf, bool resize)
{
	LONG pre = lf_.lfHeight;
	lf_ = lf;
	::lstrcpy(lf_.lfFaceName, lf.lfFaceName);
	double rate = (double)lf_.lfHeight / (double)pre;
	double cx = ((double)(bound_.Width()))*rate;
	double cy = ((double)(bound_.Height()))*rate;
	AdjustFont();
}

void iNode::SetName(const CString &name)
{
	name_ = name;
	AdjustFont();
}

void iNode::AdjustFont(bool bForceResize)
{
	if (((CiEditApp*)AfxGetApp())->m_rgsNode.bDisableNodeResize && !bForceResize) return;
	if (styleText == iNode::notext) return;
	CSize sz = GetNodeTextSize();
	LONG hmargin = sz.cy * 4 / 7;
	LONG wmargin = sz.cy;
	if (!bfillcolor && styleLine == PS_NULL) {
		hmargin /= 2;
		wmargin /= 2;
	}
	if (styleText != m_l && styleText != m_r && styleText != m_c) {
		int width = sz.cx + wmargin + margin_l_ + margin_r_;
		int height = sz.cy + hmargin + margin_t_ + margin_b_;
		if (bound_.Width() < width) {
			bound_.right = bound_.left + width;
		}
		if (bound_.Height() < height) {
			bound_.bottom = bound_.top + height;
		}
	}
	else {
		int width = sz.cx + margin_l_ + margin_r_;
		int height = sz.cy + margin_t_;
		if (bound_.Width()*bound_.Height() >= width * height) return;
		if (name_.Find(_T("\n")) == -1) {
			ExpandBoundGradually(width*height);
			bound_.bottom += (int)((double)hmargin);
		}
		else {
			ExtendLineOriented(sz);
		}
		bound_.right += wmargin;
	}
}

void iNode::ExpandBoundGradually(int area)
{
	double dw = bound_.Width();
	double dh = bound_.Height();
	int square = (int)(dw*dh);
	for (int i = 0; square <= area; i++) {
		dw *= 1.01;
		dh *= 1.01;
		bound_.right = bound_.left + (int)dw;
		bound_.bottom = bound_.top + (int)dh;
		square = bound_.Height()*bound_.Width();
	}
}

void iNode::ExtendLineOriented(const CSize& sz)
{
	int lineCount, maxLength;
	GetInnerLineInfo(name_, lineCount, maxLength);
	int width = (int)((double)sz.cx*(((double)maxLength)) / ((double)(name_.GetLength())))
		+ sz.cy + margin_l_ + margin_r_; // 1文字文余分にマージンをつけた
	int height = sz.cy*(lineCount - 1) + margin_t_;
	bound_.right = bound_.left + width;
	bound_.bottom = bound_.top + height;
}

void iNode::GetInnerLineInfo(const CString& str, int& lineCount, int& maxLength)
{
	CToken tok(str + _T("\n"));
	tok.SetToken(_T("\n"));
	lineCount = 1;
	maxLength = 0;
	for (; tok.MoreTokens(); lineCount++) {
		CString line = tok.GetNextToken();
		if (maxLength < line.GetLength()) {
			maxLength = line.GetLength();
		}
	}
}

CSize iNode::GetNodeTextSize()
{
	CWnd wnd;
	CFont font; font.CreateFontIndirect(&lf_);
	CClientDC dc(&wnd);
	CFont* pOldFont = dc.SelectObject(&font);
	CSize sz = dc.GetTabbedTextExtent(name_, -1, 0, NULL);
	dc.SelectObject(pOldFont);
	return sz;
}

void iNode::SetTextStyle(int s)
{
	int pre = styleText;
	styleText = s;
	if ((s == iNode::m_c || s == iNode::m_l || s == iNode::m_r) &&
		(pre != iNode::m_c && pre != iNode::m_l && pre != iNode::m_r)) {
		if (bound_.Width() > 600) {
			bound_.right = bound_.left + 10;
			bound_.bottom = bound_.top + 5;
		}
	}
	AdjustFont();
}

bool iNode::operator ==(iNode &n)
{
	return key_ == n.GetKey();
}

bool iNode::operator <(iNode &n)
{
	return key_ < n.key_;
}

void iNode::SetMetaFile(HENHMETAFILE& hMF)
{
	hMF_ = CopyEnhMetaFile(hMF, NULL);
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();

	ENHMETAHEADER h;
	::GetEnhMetaFileHeader(hMF_, /*sz*/sizeof(h), &h);
	RECTL b = h.rclBounds;
	bound_.bottom = bound_.top + abs(b.bottom - b.top);
	bound_.right = bound_.left + abs(b.right - b.left);
}


HENHMETAFILE iNode::GetMetaFile() const
{
	return hMF_;
}

void iNode::BackupFixState()
{
	fixed_back_ = fixed_;
}

void iNode::RestoreFixState()
{
	fixed_ = fixed_back_;
}

void iNode::FitSizeToLabel()
{
	double height = 20.0*((double)bound_.Height()) / ((double)bound_.Width());
	bound_.right = bound_.left + 20;
	bound_.bottom = bound_.top + (int)height;
	AdjustFont();
}

void iNode::SetDragging(bool dragging)
{
	dragging_ = dragging;
}

bool iNode::Dragging() const
{
	return dragging_;
}

// iNodeDrawer クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
void iNodeDrawer::Draw(const iNode &node, CDC *pDC, BOOL bDrawOrderInfo)
{
	int oldBkMode = pDC->SetBkMode(TRANSPARENT); // DCの背景色透明

	if (node.Filled()) {
		FillBound(node, pDC);
	}
	DrawShape(node, pDC);
	AdjustTextArea(node);
	DrawLabel(node, pDC, bDrawOrderInfo);
	if (node.Dragging()) {
		DrawTracker(node, pDC);
	}

	pDC->SetBkMode(oldBkMode); // DCの背景リストア
}

void iNodeDrawer::DrawTracker(const iNode& node, CDC* pDC)
{
	CPen penLine;
	penLine.CreatePen(PS_SOLID, 10, RGB(127, 127, 255)); // ペン作成
	CPen * 	pOldPen = pDC->SelectObject(&penLine); // DCのペン変更

	CRect bound = node.getBound();
	pDC->MoveTo(bound.TopLeft());
	pDC->LineTo(bound.right, bound.top);
	pDC->LineTo(bound.BottomRight());
	pDC->LineTo(bound.left, bound.bottom);
	pDC->LineTo(bound.TopLeft());

	pDC->SelectObject(pOldPen);  // DCのペンリストア
	penLine.DeleteObject();          // ペン開放
}

void iNodeDrawer::AdjustTextArea(const iNode &node)
{
	m_textRect = node.getBound();
	m_textRect.left += node.GetLineWidth() + 1;
	m_textRect.right -= node.GetLineWidth() + 1;
	m_textRect.top += node.GetLineWidth() + 1;
	m_textRect.bottom -= node.GetLineWidth() + 1;
}

void iNodeDrawer::FillBound(const iNode & node, CDC *pDC)
{
	CBrush brush(node.GetFillColor());	// ブラシ作成
	CBrush* brsOld = pDC->SelectObject(&brush);  // DCのブラシ変更

	FillBoundSpecific(node, pDC, &brush); // サブクラスで処理

	pDC->SelectObject(brsOld);             // DCのブラシリストア
	brush.DeleteObject();	                  // ブラシ開放
}

void iNodeDrawer::DrawShape(const iNode &node, CDC *pDC)
{
	CPen penLine;
	penLine.CreatePen(node.GetLineStyle(),
		node.GetLineWidth(), node.GetLineColor()); // ペン作成
	CPen * 	pOldPen = pDC->SelectObject(&penLine); // DCのペン変更

	DrawShapeSpecific(node, pDC, &penLine); // サブクラスで処理 メタファイルはここで再生

	pDC->SelectObject(pOldPen);  // DCのペンリストア
	penLine.DeleteObject();          // ペン開放
}

void iNodeDrawer::DrawLabel(const iNode &node, CDC *pDC, BOOL bDrawOrderInfo)
{
	// フォント作成
	LOGFONT lf = node.GetFontInfo();
	CFont font;
	font.CreateFont(lf.lfHeight, lf.lfWidth, 0, 0, lf.lfWeight, lf.lfItalic, lf.lfUnderline, lf.lfStrikeOut, lf.lfCharSet,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, lf.lfFaceName);


	// デバイスコンテキストのフォント変更
	CFont* pOldFont = pDC->SelectObject(&font);
	COLORREF preColor = pDC->SetTextColor(node.GetFontColor());

	DrawLabelSpecific(node, pDC);

	pDC->SelectObject(pOldFont);
	pDC->SetTextColor(preColor);
	font.DeleteObject();

	if (bDrawOrderInfo) {
		font.CreatePointFont(90, _T("MS Gothic"), pDC);
		pOldFont = pDC->SelectObject(&font);
		preColor = pDC->SetTextColor(RGB(90, 90, 90));
		CString test; test.Format(_T("%d"), node.GetDrawOrder());
		pDC->TextOut(node.getBound().left + 2, node.getBound().top + 2, test);
		pDC->SelectObject(pOldFont);
		pDC->SetTextColor(preColor);
		font.DeleteObject();
	}
}

void iNodeDrawer::DrawLabelSpecific(const iNode &node, CDC *pDC)
{
	int styleText = node.GetTextStyle();
	UINT nFormat;
	switch (styleText) {
	case iNode::s_cc:
		nFormat = DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;
		m_textRect.left += node.GetMarginLeft();
		m_textRect.right -= node.GetMarginRight();
		break;

	case iNode::s_cl:
		nFormat = DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;
		m_textRect.left += node.GetMarginLeft();
		m_textRect.right -= node.GetMarginRight();
		break;

	case iNode::s_cr:
		nFormat = DT_RIGHT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX;
		m_textRect.left += node.GetMarginLeft();
		m_textRect.right -= node.GetMarginRight();
		break;

	case iNode::s_tc:
		nFormat = DT_CENTER | DT_TOP | DT_SINGLELINE | DT_NOPREFIX;
		m_textRect.top += node.GetMarginTop();
		m_textRect.left += node.GetMarginLeft();
		m_textRect.right -= node.GetMarginRight();
		break;

	case iNode::s_tl:
		nFormat = DT_LEFT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX;
		m_textRect.top += node.GetMarginTop();
		m_textRect.left += node.GetMarginLeft();
		m_textRect.right -= node.GetMarginRight();
		break;

	case iNode::s_tr:
		nFormat = DT_RIGHT | DT_TOP | DT_SINGLELINE | DT_NOPREFIX;
		m_textRect.top += node.GetMarginTop();
		m_textRect.left += node.GetMarginLeft();
		m_textRect.right -= node.GetMarginRight();
		break;

	case iNode::s_bc:
		nFormat = DT_CENTER | DT_BOTTOM | DT_SINGLELINE | DT_NOPREFIX;
		m_textRect.bottom -= node.GetMarginBottom();
		m_textRect.left += node.GetMarginLeft();
		m_textRect.right -= node.GetMarginRight();
		break;

	case iNode::s_bl:
		nFormat = DT_LEFT | DT_BOTTOM | DT_SINGLELINE | DT_NOPREFIX;
		m_textRect.bottom -= node.GetMarginBottom();
		m_textRect.left += node.GetMarginLeft();
		m_textRect.right -= node.GetMarginRight();
		break;

	case iNode::s_br:
		nFormat = DT_RIGHT | DT_BOTTOM | DT_SINGLELINE | DT_NOPREFIX;
		m_textRect.bottom -= node.GetMarginBottom();
		m_textRect.left += node.GetMarginLeft();
		m_textRect.right -= node.GetMarginRight();
		break;

	case iNode::m_c:
		nFormat = DT_CENTER | DT_WORDBREAK | DT_NOPREFIX;
		m_textRect.top += node.GetMarginTop();
		m_textRect.left += node.GetMarginLeft();
		m_textRect.right -= node.GetMarginRight();
		break;

	case iNode::m_l:
		nFormat = DT_LEFT | DT_WORDBREAK | DT_NOPREFIX;
		m_textRect.top += node.GetMarginTop();
		m_textRect.left += node.GetMarginLeft();
		m_textRect.right -= node.GetMarginRight();
		break;

	case iNode::m_r:
		nFormat = DT_RIGHT | DT_WORDBREAK | DT_NOPREFIX;
		m_textRect.top += node.GetMarginTop();
		m_textRect.left += node.GetMarginLeft();
		m_textRect.right -= node.GetMarginRight();
		break;
	case iNode::notext:
		break;
	}

	if (styleText != iNode::notext) {
		pDC->DrawText(node.GetName(), &m_textRect, nFormat);
	}
}

void iNodeDrawer::DrawShapeSpecific(const iNode &node, CDC *pDC, const CPen* pen)
{

}

void iNodeDrawer::FillBoundSpecific(const iNode &node, CDC *pDC, CBrush* brush)
{

}

// iNodeRectDrawer クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
void iNodeRectDrawer::FillBoundSpecific(const iNode &node, CDC *pDC, CBrush *brush)
{
	if (node.Filled()) {
		pDC->FillRect(node.getBound(), brush);
	}
}

void iNodeRectDrawer::DrawShapeSpecific(const iNode &node, CDC *pDC, const CPen *pen)
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
void iNodeRoundRectDrawer::DrawShape(const iNode &node, CDC *pDC)
{
	CPen penLine;
	penLine.CreatePen(node.GetLineStyle(),
		node.GetLineWidth(), node.GetLineColor()); // ペン作成
	CPen * 	pOldPen = pDC->SelectObject(&penLine); // DCのペン変更

	CBrush* pBrsOld;
	HGDIOBJ hGdiObj;
	CBrush brs(node.GetFillColor());
	if (node.Filled()) {
		pBrsOld = pDC->SelectObject(&brs);
	}
	else {
		hGdiObj = SelectObject(pDC->m_hDC, GetStockObject(NULL_BRUSH));
	}
	CRect bound = node.getBound();
	m_r = (bound.Width() < bound.Height()) ? bound.Width() : bound.Height();
	pDC->RoundRect(bound, CPoint(m_r / 4, m_r / 4));

	if (node.Filled()) {
		pDC->SelectObject(pBrsOld);
	}
	else {
		SelectObject(pDC->m_hDC, hGdiObj);
	}

	pDC->SelectObject(pOldPen);  // DCのペンリストア
	penLine.DeleteObject();          // ペン開放
}

void iNodeRoundRectDrawer::AdjustTextArea(const iNode &node)
{
	iNodeDrawer::AdjustTextArea(node);
	switch (node.GetTextStyle()) {
	case iNode::s_tl:
		m_textRect.left += m_r / 8;
		break;

	case iNode::s_tr:
		m_textRect.right -= m_r / 8;
		break;

	case iNode::s_bl:
		m_textRect.left += m_r / 8;
		break;

	case iNode::s_br:
		m_textRect.right -= m_r / 8;
		break;
	}
}

// iNodeArcDrawer クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

void iNodeArcDrawer::FillBoundSpecific(const iNode &node, CDC *pDC, CBrush *brush)
{
	CPen penArc;
	penArc.CreatePen(node.GetLineStyle(), node.GetLineWidth(), node.GetFillColor());
	CPen* pOldPen = pDC->SelectObject(&penArc);
	pDC->Ellipse(node.getBound());
	pDC->SelectObject(pOldPen);
	penArc.DeleteObject();
}

void iNodeArcDrawer::DrawShapeSpecific(const iNode &node, CDC *pDC, const CPen *pen)
{
	CRect bound = node.getBound();
	pDC->Arc(&bound, bound.TopLeft(), bound.TopLeft());
}

void iNodeArcDrawer::AdjustTextArea(const iNode &node)
{
	m_textRect = node.getBound();
	m_textRect.left += node.getBound().Width() / 16;
	m_textRect.right -= node.getBound().Width() / 16;
	m_textRect.top += node.getBound().Height() / 16;
	m_textRect.bottom -= node.getBound().Height() / 16;

	switch (node.GetTextStyle()) {
	case iNode::s_tl:
		m_textRect.top += node.getBound().Height() * 3 / 16;
		break;

	case iNode::s_tr:
		m_textRect.top += node.getBound().Height() * 3 / 16;
		break;

	case iNode::s_bl:
		m_textRect.bottom -= node.getBound().Height() * 3 / 16;
		break;

	case iNode::s_br:
		m_textRect.bottom -= node.getBound().Height() * 3 / 16;
		break;
	}
}

// iNodeMetafileDrawer クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////
void iNodeMetafileDrawer::DrawShape(const iNode &node, CDC *pDC)
{
	pDC->PlayMetaFile(node.GetMetaFile(), node.getBound());
}

void iNodeMetafileDrawer::FillBoundSpecific(const iNode &node, CDC *pDC, CBrush *brush)
{
	// Do Nothing
}


// iNodeMMNodeDrawer クラスのインプリメンテーション
void iNodeMMNodeDrawer::DrawShapeSpecific(const iNode &node, CDC* pDC, const CPen *pen)
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
	node_iter it = begin();
	for (; it != end(); it++) {
		if (selKey_ == (*it).second.GetKey()) {
			(*it).second.Select();
			curParent_ = (*it).second.GetParentKey();
		}
		else {
			(*it).second.Select(false);
		}
	}
}

node_iter iNodes::getSelectedNode()
{
	return FindWrite(selKey_);
}

node_c_iter iNodes::GetSelectedConstIter() const
{
	return FindRead(selKey_);
}

void iNodes::initSelection()
{
	if (size() == 1) {
		selKey_ = 0;
		setSelKey(selKey_);
	}
	else {
		node_c_iter it = begin();
		for (; it != end(); it++) {
			if ((*it).second.GetTreeState() & TVIS_SELECTED) {
				// TVIS_EXPANDEDと間違えてた
				selKey_ = (*it).second.GetKey();
				curParent_ = (*it).second.GetParentKey();
				break;
			}
		}
	}
}

void iNodes::drawNodes(CDC *pDC, bool bDrwAll)
{
	vector<iNode*>::iterator it = nodesDraw_.begin();
	for (; it != nodesDraw_.end(); it++) {
		iNodeDrawer* pDrawer = getNodeDrawer(*(*it));
		pDrawer->Draw(*(*it), pDC, m_bDrawOrderInfo);
	}
}

iNode* iNodes::hitTest(const CPoint &pt, bool bTestAll)
{
	node_iter it = begin();
	for (; it != end(); it++) {
		(*it).second.Select(false);
	}

	vector<iNode*>::reverse_iterator vit = nodesDraw_.rbegin();
	CRect preRc(0, 0, 0, 0);
	vector<iNode*>::reverse_iterator vit_inner = nodesDraw_.rend();
	for (; vit != nodesDraw_.rend(); vit++) {
		CRect rc = (*vit)->getBound();
		rc.left -= 1;
		rc.right += 1;
		rc.top -= 1;
		rc.bottom += 1;
		if (rc.PtInRect(pt)) {
			if (!preRc.IsRectNull()) {
				CRect andRc = preRc & rc;
				if (andRc.Width() < preRc.Width() && andRc.Height() < preRc.Height()) {
					vit_inner = vit;
					preRc = rc;
				}
			}
			else {
				vit_inner = vit;
				preRc = rc;
			}
		}
	}
	if (vit_inner != nodesDraw_.rend()) {
		(*vit_inner)->Select();
		selKey_ = (*vit_inner)->GetKey();
		curParent_ = (*vit_inner)->GetParentKey();
	}
	if (vit_inner == nodesDraw_.rend()) return NULL;
	return *vit_inner;
}

iNode* iNodes::hitTest2(const CPoint &pt, bool bTestAll) const
{
	vector<iNode*>::const_reverse_iterator vit = nodesDraw_.rbegin();
	CRect preRc(0, 0, 0, 0);
	vector<iNode*>::const_reverse_iterator vit_inner = nodesDraw_.rend();
	for (; vit != nodesDraw_.rend(); vit++) {
		CRect rc = (*vit)->getBound();
		rc.left -= 1;
		rc.right += 1;
		rc.top -= 1;
		rc.bottom += 1;
		if (rc.PtInRect(pt)) {
			if (!preRc.IsRectNull()) {
				CRect andRc = preRc & rc;
				if (andRc.Width() < preRc.Width() && andRc.Height() < preRc.Height()) {
					vit_inner = vit;
					preRc = rc;
				}
			}
			else {
				vit_inner = vit;
				preRc = rc;
			}
		}
	}
	if (vit_inner == nodesDraw_.rend()) return NULL;
	return *vit_inner;
}

void iNodes::moveSelectedNode(const CSize &sz)
{
	node_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).second.Selected()) {
			(*it).second.MoveBound(sz);
		}
	}
}

void iNodes::setSelectedNodeBound(const CRect &r)
{
	node_iter it = FindWrite(selKey_);
	if (it != end()) {
		(*it).second.SetBound(r);
	}
}

void iNodes::setSelectedNodeFont(const LOGFONT &lf)
{
	node_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).second.Selected()) {
			(*it).second.SetFontInfo(lf);
		}
	}
}

void iNodes::getSelectedNodeFont(LOGFONT& lf)
{
	node_c_iter it = FindRead(selKey_);
	if (it != end()) {
		lf = (*it).second.GetFontInfo();
		::lstrcpy(lf.lfFaceName, (*it).second.GetFontInfo().lfFaceName);
	}
}


COLORREF iNodes::getSelectedNodeFontColor() const
{
	COLORREF c = RGB(0, 0, 0);
	node_c_iter it = FindRead(selKey_);
	if (it != end()) {
		c = (*it).second.GetFontColor();
	}
	return c;
}

void iNodes::setSelectedNodeFontColor(const COLORREF &c)
{
	node_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).second.Selected()) {
			(*it).second.SetFontColor(c);
		}
	}
}

void iNodes::SetSelectedFillColor(const COLORREF &c)
{
	node_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).second.Selected()) {
			(*it).second.SetFillColor(c);
		}
	}
}

void iNodes::ToggleSelectedFill(BOOL noBrush)
{
	node_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).second.Selected()) {
			(*it).second.ToggleFill(noBrush);
		}
	}
}

void iNodes::setSelectedNodeLineColor(const COLORREF &c)
{
	node_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).second.Selected()) {
			(*it).second.SetLineColor(c);
		}
	}
}

COLORREF iNodes::getSelectedNodeLineColor() const
{
	COLORREF c = RGB(0, 0, 0);
	node_c_iter it = FindRead(selKey_);
	if (it != end()) {
		c = (*it).second.GetLineColor();
	}
	return c;
}

COLORREF iNodes::GetSelectedFillColor() const
{
	COLORREF c = RGB(255, 255, 255);
	node_c_iter it = FindRead(selKey_);
	if (it != end()) {
		c = (*it).second.GetFillColor();
	}
	return c;
}

void iNodes::setSelectedNodeLineStyle(int style)
{
	node_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).second.Selected()) {
			(*it).second.SetLineStyle(style);
		}
	}
}

int iNodes::getSelectedNodeLineStyle() const
{
	int s = PS_SOLID;
	node_c_iter it = FindRead(selKey_);
	if (it != end()) {
		s = (*it).second.GetLineStyle();
	}
	return s;
}

void iNodes::setSelectedNodeLineWidth(int w)
{
	node_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).second.Selected()) {
			(*it).second.SetLineWidth(w);
		}
	}
}


int iNodes::getSelectedNodeLineWidth() const
{
	int w = 0;
	node_c_iter it = FindRead(selKey_);
	if (it != end()) {
		w = (*it).second.GetLineWidth();
	}
	return w;
}

void iNodes::setSelectedNodeTextStyle(int style)
{
	node_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).second.Selected()) {
			(*it).second.SetTextStyle(style);
		}
	}
}

void iNodes::setSelectedNodeTreeIconId(int id)
{
	node_iter it = FindWrite(selKey_);
	if (it != end()) {
		(*it).second.SetTreeIconId(id);
	}
}

int iNodes::getSelectedNodeTextStyle() const
{
	int s = iNode::s_cc;
	node_c_iter it = FindRead(selKey_);
	if (it != end()) {
		s = (*it).second.GetTextStyle();
	}
	return s;
}

int iNodes::SelectInBound(const CRect &bound, CRect &selRect)
{
	node_iter it = begin();
	int cnt = 0;
	for (; it != end(); it++) {
		CRect r = (*it).second.getBound();
		if (!(*it).second.Visible()) continue;
		if ((r | bound) == bound) {
			(*it).second.Select();
			selRect = (*it).second.getBound();
			cnt++;
		}
		else {
			(*it).second.Select(false);
		}
	}

	it = begin();
	for (; it != end(); it++) {
		if ((*it).second.Selected()) {
			if ((*it).second.getBound().left < selRect.left) selRect.left = (*it).second.getBound().left;
			if ((*it).second.getBound().right > selRect.right) selRect.right = (*it).second.getBound().right;
			if ((*it).second.getBound().top < selRect.top) selRect.top = (*it).second.getBound().top;
			if ((*it).second.getBound().bottom > selRect.bottom) selRect.bottom = (*it).second.getBound().bottom;
		}
	}
	return cnt;
}

bool orderComp(const iNode* n1, const iNode* n2)
{
	return n1->GetDrawOrder() < n2->GetDrawOrder();
}

CString iNodes::CreateClickableMapString(const CString& fileName, bool singleText)
{
	CString mapString;
	vector<iNode*>::reverse_iterator it = nodesDraw_.rbegin();
	for (; it != nodesDraw_.rend(); it++) {
		iNodeDrawer* pDrawer = getNodeDrawer(*(*it));
		CString coordsValue;
		CPoint ptl = (*it)->getBound().TopLeft();
		CPoint pbr = (*it)->getBound().BottomRight();
		coordsValue.Format(_T("%d,%d,%d,%d"), ptl.x, ptl.y, pbr.x, pbr.y);
		CString href;
		if (singleText) {
			href.Format(fileName + _T("#%d"), (*it)->GetKey());
		}
		else {
			href.Format(_T("text/") + fileName + _T("%d.html"), (*it)->GetKey());
		}
		mapString += _T("<area shape=\"rect\" coords=\"") + coordsValue
			+ _T("\" href=\"") + href + _T("\" target=\"text\" alt=\"") + StringUtil::RemoveCr((*it)->GetName()) + _T("\" />\n");
	}
	return mapString;
}

void iNodes::PrepareVisibles(DWORD key)
{
	nodesDraw_.clear();
	nodesDraw_.resize(0);

	node_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).second.GetParentKey() != curParent_) {
			(*it).second.SetVisible(false);
		}
		else {
			(*it).second.SetVisible();
			unsigned int order = 0;
			vector<DWORD>::iterator ik = std::find(svec_.begin(), svec_.end(), (*it).second.GetKey());
			if (ik != svec_.end()) {
				order = std::distance(svec_.begin(), ik);
			}
			(*it).second.SetDrawOrder(order);
			nodesDraw_.push_back(&((*it).second));
		}
	}
	if (((CiEditApp*)AfxGetApp())->m_rgsNode.bSyncOrder) {
		std::sort(nodesDraw_.begin(), nodesDraw_.end(), orderComp);
	}
}

vector<iNode*> iNodes::GetVisibles() const
{
	return nodesDraw_;
}

void iNodes::PrepareVisibles(NodeKeySet& keySet)
{
	nodesDraw_.clear();
	nodesDraw_.resize(0);

	node_iter it = begin();
	for (; it != end(); it++) {
		(*it).second.SetVisible(false);
	}

	set<DWORD>::iterator itks = keySet.begin();
	node_iter nit;
	for (; itks != keySet.end(); itks++) {
		nit = FindWrite(*itks);
		if (nit != end()) {
			(*nit).second.SetVisible(true);
			unsigned int order = 0;
			vector<DWORD>::iterator ik = std::find(svec_.begin(), svec_.end(), (*nit).second.GetKey());
			if (ik != svec_.end()) {
				order = std::distance(svec_.begin(), ik);
			}
			(*nit).second.SetDrawOrder(order);
			nodesDraw_.push_back(&((*nit).second));
		}
	}
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	if (((CiEditApp*)AfxGetApp())->m_rgsNode.bSyncOrder) {
		std::sort(nodesDraw_.begin(), nodesDraw_.end(), orderComp);
	}
}

BOOL iNodes::SelectedFilled() const
{
	node_c_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).second.Selected()) {
			return ((*it).second.Filled());
		}
	}
	return FALSE;
}

int iNodes::GetSelectedShape() const
{
	int shape = iNode::rectangle;
	node_c_iter it = FindRead(selKey_);
	if (it != end()) {
		shape = (*it).second.GetShape();
	}
	return shape;
}

void iNodes::SetSelectedShape(int shape)
{
	node_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).second.Selected()) {
			(*it).second.SetShape(shape);
			if (shape > 9) {
				CiEditApp* pApp = (CiEditApp*)AfxGetApp();
				(*it).second.SetMetaFile(pApp->m_hMetaFiles[shape - 10]);
			}
		}
	}
}

BOOL iNodes::IsSelectedFixed() const
{
	node_c_iter it = FindRead(selKey_);
	BOOL fix = FALSE;
	if (it != end()) {
		fix = (*it).second.Fixed();
	}
	return fix;
}

void iNodes::FixSelected(BOOL f)
{
	node_iter it = FindWrite(selKey_);
	if (it != end()) {
		(*it).second.Fix(f);
	}
}

node_c_iter iNodes::FindRead(DWORD key) const
{
	return find(key);
}

node_iter iNodes::FindWrite(DWORD key)
{
	return find(key);
}

CSize iNodes::GetNodeSizeMax(bool selection) const
{
	CSize sz(0, 0);

	node_c_iter it = begin();
	for (; it != end(); it++) {
		if (!(*it).second.Visible()) {
			continue;
		}
		if (selection) {
			if (!(*it).second.Selected()) {
				continue;
			}
		}
		CRect rc = (*it).second.getBound();
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
	int shape = node.GetShape();
	if (shape == iNode::rectangle) {
		return m_pNodeRectDrawer;
	}
	else if (shape == iNode::roundRect) {
		return m_pNodeRoundRectDrawer;
	}
	else if (shape == iNode::arc) {
		return m_pNodeArcDrawer;
	}
	else if (shape == iNode::MetaFile) {
		return m_pMetafileDrawer;
	}
	else if (shape == iNode::MindMapNode) {
		return m_pMMNodeDrawer;
	}
	return m_pNodeRectDrawer;
}

void iNodes::DrawSelected(CDC *pDC)
{
	vector<iNode*>::iterator it = nodesDraw_.begin();
	for (; it != nodesDraw_.end(); it++) {
		if ((*(*it)).Selected()) {
			iNodeDrawer* pDrawer = getNodeDrawer(*(*it));
			pDrawer->Draw(*(*it), pDC, m_bDrawOrderInfo);
		}
	}
}

void iNodes::FixReversibly(DWORD keyExcluded)
{
	node_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).second.IsInChain()) {
			(*it).second.BackupFixState();
			if ((*it).second.GetKey() != keyExcluded) {
				(*it).second.Fix();
			}
		}
	}
}

void iNodes::RestoreFixState(DWORD keyExcluded)
{
	node_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).second.IsInChain()) {
			if ((*it).second.GetKey() != keyExcluded) {
				(*it).second.RestoreFixState();
			}
		}
	}
}

void iNodes::resizeSelectedNodeFont(bool bEnlarge)
{
	node_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).second.Selected()) {
			LOGFONT lf = (*it).second.GetFontInfo();
			LONG pre = lf.lfHeight;
			if (bEnlarge) {
				lf.lfHeight -= 2;
			}
			else {
				if (lf.lfHeight < -4) {
					lf.lfHeight += 2;
				}
			}
			(*it).second.SetFontInfo(lf, false);
		}
	}
}

NodeKeyVec iNodes::GetSelectedKeys() const
{
	NodeKeyVec v;
	node_c_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).second.Selected() && (*it).second.Visible()) {
			v.push_back((*it).second.GetKey());
		}
	}
	return v;
}

void iNodes::SetMarginToSelected(int l, int r, int t, int b)
{
	node_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).second.Selected()) {
			(*it).second.SetMarginLeft(l);
			(*it).second.SetMarginRight(r);
			(*it).second.SetMarginTop(t);
			(*it).second.SetMarginBottom(b);
		}
	}
}

void iNodes::DragSelected(bool dragging)
{
	node_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).second.Selected()) {
			(*it).second.SetDragging(dragging);
			return;
		}
	}
}
