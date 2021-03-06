﻿// iLink.cpp: iLink クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "iEdit.h"
#include "iLink.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

iLink::iLink()
{
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	memset(&lf_, 0, sizeof(LOGFONT));
	::lstrcpy(lf_.lfFaceName, pApp->m_rgsLink.lf.lfFaceName);
	lf_.lfHeight = pApp->m_rgsLink.lf.lfHeight;
	lf_.lfWidth = pApp->m_rgsLink.lf.lfWidth;
	lf_.lfItalic = pApp->m_rgsLink.lf.lfItalic;
	lf_.lfUnderline = pApp->m_rgsLink.lf.lfUnderline;
	lf_.lfStrikeOut = pApp->m_rgsLink.lf.lfStrikeOut;
	lf_.lfCharSet = pApp->m_rgsLink.lf.lfCharSet;
	lf_.lfWeight = pApp->m_rgsLink.lf.lfWeight;

	drwFlag = false;
	deleted_ = false;
	colorLine = pApp->m_rgsLink.colorLine;
	styleArrow = iLink::line;
	styleLine = pApp->m_rgsLink.styleLine;
	lineWidth = pApp->m_rgsLink.lineWidth;
	selected_ = false;
	curved_ = false;
	angled_ = false;
	len_ = -1.0;
	dropTarget_ = false;
}

iLink::~iLink()
{

}

iLink::iLink(const iLink &l)
{
	CopyProps(l);
}

iLink& iLink::operator =(const iLink &l)
{
	CopyProps(l);
	return *this;
}

void iLink::CopyProps(const iLink& l)
{
	key_ = l.key_;
	keyFrom = l.keyFrom;
	keyTo = l.keyTo;
	rcFrom = l.rcFrom;
	rcTo = l.rcTo;
	ptFrom = l.ptFrom;
	ptTo = l.ptTo;
	ptPath = l.ptPath;
	selfRect = l.selfRect;
	drwFlag = l.drwFlag;
	deleted_ = l.deleted_;
	colorLine = l.colorLine;
	selected_ = l.selected_;
	name_ = l.name_;
	lf_ = l.lf_;
	::lstrcpy(lf_.lfFaceName, l.lf_.lfFaceName);
	styleArrow = l.styleArrow;
	styleLine = l.styleLine;
	lineWidth = l.lineWidth;
	path_ = l.path_;
	curved_ = l.curved_;
	angled_ = l.angled_;
	len_ = l.len_;
	dropTarget_ = l.dropTarget_;
}

IMPLEMENT_SERIAL(iLink, CObject, 0)

void iLink::DrawComment(CDC *pDC, bool clipbrd)
{
	CRect rc = getCommentRect();

	font_.CreateFont(lf_.lfHeight, lf_.lfWidth, 0, 0, lf_.lfWeight, lf_.lfItalic, lf_.lfUnderline, lf_.lfStrikeOut, lf_.lfCharSet,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, lf_.lfFaceName);


	CFont* pOldFont = pDC->SelectObject(&font_);
	COLORREF preColor = pDC->SetTextColor(colorLine);

	pDC->TextOut(rc.left, rc.top, name_);

	bool renderHandle = this->curved_ && this->name_.GetLength() == 0 && !this->angled_;

	if (!pDC->IsPrinting() && !clipbrd && renderHandle) {
		penCommentRect.CreatePen(PS_DOT, 0, RGB(127, 127, 127));
		CPen* poldPen = pDC->SelectObject(&penCommentRect);
		pDC->MoveTo(rc.TopLeft());
		pDC->LineTo(rc.right, rc.top);
		pDC->LineTo(rc.BottomRight());
		pDC->LineTo(rc.left, rc.bottom);
		pDC->LineTo(rc.TopLeft());

		pDC->SelectObject(poldPen);
		penCommentRect.DeleteObject();
	}
	pDC->SetTextColor(preColor);
	pDC->SelectObject(pOldFont);
	font_.DeleteObject();
}

void iLink::DrawLine(CDC *pDC)
{
	penLine.CreatePen(styleLine, lineWidth, colorLine);
	CPen* pOldPen = pDC->SelectObject(&penLine);
	if (keyFrom != keyTo) {
		if (!curved_) {
			pDC->MoveTo(ptFrom);
			pDC->LineTo(ptTo);
		}
		else {
			if (!angled_) {
				CPoint pt[4];
				pt[0] = ptFrom;
				pt[1] = ptPath;
				pt[2] = ptPath;
				pt[3] = ptTo;
				pDC->PolyBezier(pt, 4);
			}
			else {
				pDC->MoveTo(ptFrom);
				pDC->LineTo(ptPath);
				pDC->LineTo(ptTo);
			}
		}
	}
	else {
		if (!angled_) {
			CPoint pt[4];
			pt[0] = ptFrom;
			pt[1] = ptPath;
			pt[2] = ptPath;
			pt[3] = ptTo;
			pDC->PolyBezier(pt, 4);
		}
		else {
			pDC->MoveTo(ptFrom);
			pDC->LineTo(ptPath);
			pDC->LineTo(ptTo);
		}
	}

	if (dropTarget_) {
		DrawAsDropTarget(pDC);
	}

	pDC->SelectObject(pOldPen);
	penLine.DeleteObject();
}

void iLink::DrawAsDropTarget(CDC* pDC)
{
	penLine.CreatePen(PS_SOLID, 10, RGB(127, 127, 255));
	CPen* pOldPen = pDC->SelectObject(&penLine);
	if (keyFrom != keyTo) {
		if (!curved_) {
			pDC->MoveTo(ptFrom);
			pDC->LineTo(ptTo);
		}
		else {
			if (!angled_) {
				CPoint pt[4];
				pt[0] = ptFrom;
				pt[1] = ptPath;
				pt[2] = ptPath;
				pt[3] = ptTo;
				pDC->PolyBezier(pt, 4);
			}
			else {
				pDC->MoveTo(ptFrom);
				pDC->LineTo(ptPath);
				pDC->LineTo(ptTo);
			}
		}
	}
	else {
		if (!angled_) {
			CPoint pt[4];
			pt[0] = ptFrom;
			pt[1] = ptPath;
			pt[2] = ptPath;
			pt[3] = ptTo;
			pDC->PolyBezier(pt, 4);
		}
		else {
			pDC->MoveTo(ptFrom);
			pDC->LineTo(ptPath);
			pDC->LineTo(ptTo);
		}
	}

	pDC->SelectObject(pOldPen);
	penLine.DeleteObject();
}

void iLink::DrawArrow(CDC *pDC)
{
	if (!this->IsRectTermLink()) {
		DrawTriangleArrows(pDC);
	}
	else {
		DrawRectArraws(pDC);
	}
}

const bool iLink::IsRectTermLink() const
{
	if (styleArrow == iLink::aggregat || styleArrow == iLink::composit) {
		return true;
	}
	return false;
}

void iLink::DrawRectArraws(CDC* pDC)
{

	CBrush* pOldBrs;
	CBrush brs(colorLine);
	if (styleArrow == iLink::aggregat) {
		pOldBrs = (CBrush*)pDC->SelectStockObject(WHITE_BRUSH);
	}
	else {
		pOldBrs = pDC->SelectObject(&brs);
	}

	int ArrowWidth = 5;
	int ArrowHeight = 12;

	CPoint pt[4];
	pt[0].x = ptTo.x;
	pt[0].y = ptTo.y;
	pt[1].x = pt[0].x + ArrowHeight;
	pt[1].y = pt[0].y + ArrowWidth;
	pt[2].x = pt[1].x + ArrowHeight;
	pt[2].y = pt[0].y;
	pt[3].x = pt[1].x;
	pt[3].y = pt[0].y - ArrowWidth;

	if (!curved_) {
		RotateArrow(pt, 4, ptFrom, ptTo, ptTo);
	}
	else {
		RotateArrow(pt, 4, ptPath, ptTo, ptTo);
	}

	pDC->Polygon(pt, 4);
	pDC->SelectObject(pOldBrs);

	penLine.CreatePen(PS_SOLID, lineWidth, colorLine);
	CPen* oldpen = pDC->SelectObject(&penLine);
	pDC->MoveTo(pt[0]);
	pDC->LineTo(pt[1]);
	pDC->LineTo(pt[2]);
	pDC->LineTo(pt[3]);
	pDC->LineTo(pt[0]);
	pDC->SelectObject(oldpen);
	penLine.DeleteObject();
}

void iLink::DrawTriangleArrows(CDC* pDC)
{
	CBrush* pOldBrs;
	CBrush brs(colorLine);
	if (styleArrow == iLink::inherit) {
		pOldBrs = (CBrush*)pDC->SelectStockObject(WHITE_BRUSH);
	}
	else {
		pOldBrs = pDC->SelectObject(&brs);
	}

	penLine.CreatePen(PS_SOLID, lineWidth, colorLine);
	CPen* oldpen = pDC->SelectObject(&penLine);

	int ArrowWidth = 5;
	int ArrowHeight = 12;
	switch (this->lineWidth) {
	case 0:
		ArrowWidth = 5;
		ArrowHeight = 12;
		break;
	case 2:
		ArrowWidth = 6;
		ArrowHeight = 13;
		break;
	case 3:
		ArrowWidth = 7;
		ArrowHeight = 16;
		break;
	case 4:
		ArrowWidth = 7;
		ArrowHeight = 18;
		break;
	case 5:
		ArrowWidth = 8;
		ArrowHeight = 19;
		break;
	}

	if (IsOneWay() || IsTwoWay()) {
		CPoint pt[3];
		pt[0].x = ptTo.x;
		pt[0].y = ptTo.y;
		pt[1].x = pt[0].x + ArrowHeight;
		pt[1].y = pt[0].y + ArrowWidth;
		pt[2].x = pt[0].x + ArrowHeight;
		pt[2].y = pt[0].y - ArrowWidth;

		if (keyFrom != keyTo) {
			if (!curved_) {
				RotateArrow(pt, 3, ptFrom, ptTo, ptTo);
			}
			else {
				RotateArrow(pt, 3, ptPath, ptTo, ptTo);
			}
		}
		else {
			RotateArrow(pt, 3, ptPath, ptTo, ptTo);
		}
		if (styleArrow == iLink::arrow || styleArrow == iLink::arrow2 ||
			styleArrow == iLink::inherit) {
			pDC->Polygon(pt, 3);
		}
		else if (styleArrow == iLink::depend || styleArrow == iLink::depend2) {
			pDC->MoveTo(pt[0]);
			pDC->LineTo(pt[1]);
			pDC->MoveTo(pt[0]);
			pDC->LineTo(pt[2]);
		}
	}
	if (IsTwoWay()) {
		CPoint pt[3];
		pt[0].x = ptFrom.x;
		pt[0].y = ptFrom.y;
		pt[1].x = pt[0].x + ArrowHeight;
		pt[1].y = pt[0].y + ArrowWidth;
		pt[2].x = pt[0].x + ArrowHeight;
		pt[2].y = pt[0].y - ArrowWidth;

		if (keyFrom != keyTo) {
			if (!curved_) {
				RotateArrow(pt, 3, ptTo, ptFrom, ptFrom);
			}
			else {
				RotateArrow(pt, 3, ptPath, ptFrom, ptFrom);
			}
		}
		else {
			RotateArrow(pt, 3, ptPath, ptFrom, ptFrom);
		}
		if (styleArrow == iLink::arrow2) {
			pDC->Polygon(pt, 3);
		}
		else if (styleArrow == iLink::depend2) {
			pDC->MoveTo(pt[0]);
			pDC->LineTo(pt[1]);
			pDC->MoveTo(pt[0]);
			pDC->LineTo(pt[2]);
		}
	}
	brs.DeleteObject();
	pDC->SelectObject(pOldBrs);
	pDC->SelectObject(oldpen);
	penLine.DeleteObject();
}

void iLink::RotateArrow(CPoint *pPoint, int size, CPoint &pFrom, CPoint &pTo, CPoint &ptOrg)
{
	double c, s;
	double r = sqrt(pow((double)(pTo.x - pFrom.x), 2) + pow((double)(pTo.y - pFrom.y), 2));
	c = ((double)(pFrom.x - pTo.x)) / r;
	s = ((double)(pFrom.y - pTo.y)) / r;

	for (int i = 0; i < size; i++) { // 頂点の数だけ繰り返し
		// m_ptTo を原点になるように座標の並行移動
		pPoint[i].x = pPoint[i].x - pTo.x;
		pPoint[i].y = pPoint[i].y - pTo.y;
		// 回転処理
		int rotatedX = (int)(pPoint[i].x*c - pPoint[i].y*s);
		int rotatedY = (int)(pPoint[i].y*c + pPoint[i].x*s);
		// 座標を並行移動
		pPoint[i].x = rotatedX + ptOrg.x;
		pPoint[i].y = rotatedY + ptOrg.y;
	}
}

const bool iLink::IsTwoWay() const
{
	return (styleArrow == iLink::arrow2 || styleArrow == iLink::depend2);
}

const bool iLink::IsOneWay() const
{
	return (styleArrow == iLink::arrow || styleArrow == iLink::depend ||
		styleArrow == iLink::inherit || styleArrow == iLink::aggregat ||
		styleArrow == iLink::composit);
}

void iLink::SetConnectionPoint()
{
	CPoint gFrom = rcFrom.CenterPoint();
	CPoint gTo = rcTo.CenterPoint();

	if (keyFrom != keyTo) {
		if (!curved_) {
			ptFrom = GetIntersection(rcFrom, gTo);
			ptTo = GetIntersection(rcTo, gFrom);
		}
		else {
			ptFrom = GetIntersection(rcFrom, ptPath);
			ptTo = GetIntersection(rcTo, ptPath);
		}
	}
	else {
		selfRect.left = gFrom.x; selfRect.bottom = gFrom.y;
		selfRect.right = ptPath.x; selfRect.top = ptPath.y;
		selfRect.NormalizeRect();

		if (ptPath.x >= gFrom.x && ptPath.y <= gFrom.y) {
			ptFrom = CPoint(rcFrom.right, gFrom.y);
			ptTo = CPoint(gFrom.x, rcFrom.top);
		}
		else if (ptPath.x >= gFrom.x && ptPath.y > gFrom.y) {
			ptFrom = CPoint(gFrom.x, rcFrom.bottom);
			ptTo = CPoint(rcFrom.right, gFrom.y);
		}
		else if (ptPath.x < gFrom.x && ptPath.y > gFrom.y) {
			ptFrom = CPoint(rcFrom.left, gFrom.y);
			ptTo = CPoint(gFrom.x, rcFrom.bottom);
		}
		else if (ptPath.x < gFrom.x && ptPath.y <= gFrom.y) {
			ptFrom = CPoint(gFrom.x, rcFrom.top);
			ptTo = CPoint(rcFrom.left, gFrom.y);
		}

		curved_ = true;
	}
}

void iLink::Serialize(CArchive &ar)
{
	if (ar.IsStoring()) {
		if (deleted_) return;
		CString fname(lf_.lfFaceName);

		// Visual C++ 6.0 と 7.0(.NET)の互換性のためのシリアライズコード
		// 7.0ではbool値をそのままシリアル化可能となっているが、過去のファイル
		// との整合性のため1と0に変換してシリアル化することとする
		int curve;
		if (curved_ == true) {
			curve = 1;
		}
		else {
			curve = 0;
		}

		ar << keyFrom << keyTo << rcFrom << rcTo << ptFrom << ptTo << ptPath << curve
			<< lineWidth << styleArrow << styleLine << colorLine << name_ << path_
			<< lf_.lfHeight << lf_.lfWidth << lf_.lfItalic << lf_.lfUnderline << lf_.lfStrikeOut << lf_.lfWeight
			<< fname;
	}
	else {
		CString fname;
		int curve;
		ar >> keyFrom >> keyTo >> rcFrom >> rcTo >> ptFrom >> ptTo >> ptPath >> curve
			>> lineWidth >> styleArrow >> styleLine >> colorLine >> name_ >> path_
			>> lf_.lfHeight >> lf_.lfWidth >> lf_.lfItalic >> lf_.lfUnderline >> lf_.lfStrikeOut >> lf_.lfWeight
			>> fname;
		::lstrcpy(lf_.lfFaceName, fname);
		if (curve > 0) {
			curved_ = true;
		}
		else {
			curved_ = false;
		}
		if (keyFrom == keyTo) {
			SetConnectionPoint();
		}
	}
}

void iLink::SerializeEx(CArchive& ar, int version)
{
	if (ar.IsStoring()) {
		if (deleted_) return;
		CString fname(lf_.lfFaceName);

		int curve = curved_ ? 1 : 0;

		ar << keyFrom << keyTo << rcFrom << rcTo << ptFrom << ptTo << ptPath << curve
			<< lineWidth << styleArrow << styleLine << colorLine << name_ << path_
			<< lf_.lfHeight << lf_.lfWidth << lf_.lfItalic << lf_.lfUnderline << lf_.lfStrikeOut << lf_.lfWeight
			<< fname;
		if (version > 2) {
			int angled = angled_ ? 1 : 0;
			ar << angled;
		}
	}
	else {
		CString fname;
		int curve;
		ar >> keyFrom >> keyTo >> rcFrom >> rcTo >> ptFrom >> ptTo >> ptPath >> curve
			>> lineWidth >> styleArrow >> styleLine >> colorLine >> name_ >> path_
			>> lf_.lfHeight >> lf_.lfWidth >> lf_.lfItalic >> lf_.lfUnderline >> lf_.lfStrikeOut >> lf_.lfWeight
			>> fname;
		::lstrcpy(lf_.lfFaceName, fname);
		if (version > 2) {
			int angled;
			ar >> angled;
			angled_ = (angled > 0);
		}
		curved_ = (curve > 0);
		if (keyFrom == keyTo) {
			SetConnectionPoint();
		}
	}
}

CPoint iLink::GetIntersection(const CRect &target, const CPoint &start)
{
	CPoint g((target.left + target.right) / 2, (target.top + target.bottom) / 2);

	double t;
	CPoint cpt;
	for (t = 0.01; t < 1.0; t += 0.01) {
		cpt.x = start.x + (int)(t*(g.x - start.x));
		cpt.y = start.y + (int)(t*(g.y - start.y));
		if (target.PtInRect(cpt)) break;
	}

	return cpt;
}

bool iLink::HitTest(const CPoint &pt)
{
	if (rcFrom.PtInRect(pt) || rcTo.PtInRect(pt)) return false;
	if (HitTestConnectionPtFrom(pt) || HitTestConnectionPtTo(pt)) return false;
	if (!curved_) {
		CPoint pts[4];
		const int mrgn = 4;
		if (abs(ptFrom.x - ptTo.x) > abs(ptFrom.y - ptTo.y)) {
			pts[0].x = ptFrom.x;
			pts[0].y = ptFrom.y - mrgn;
			pts[1].x = ptFrom.x;
			pts[1].y = ptFrom.y + mrgn;
			pts[2].x = ptTo.x;
			pts[2].y = ptTo.y + mrgn;
			pts[3].x = ptTo.x;
			pts[3].y = ptTo.y - mrgn;
		}
		else {
			pts[0].x = ptFrom.x - mrgn;
			pts[0].y = ptFrom.y;
			pts[1].x = ptFrom.x + mrgn;
			pts[1].y = ptFrom.y;
			pts[2].x = ptTo.x + mrgn;
			pts[2].y = ptTo.y;
			pts[3].x = ptTo.x - mrgn;
			pts[3].y = ptTo.y;
		}
		CRgn* r = new CRgn;
		r->CreatePolygonRgn(pts, 4, WINDING);
		if (r->PtInRegion(pt)) {
			selected_ = true;
		}
		else {
			selected_ = false;
		}
		CRect cr = getCommentRect();
		if (cr.PtInRect(pt)) {
			selected_ = true;
		}
		delete r;
	}
	else {
		CPoint pts[6];
		const int mrgn = 4;
		pts[0].x = ptFrom.x;
		pts[0].y = ptFrom.y - mrgn;
		pts[1].x = ptPath.x;
		pts[1].y = ptPath.y - mrgn;
		pts[2].x = ptTo.x;
		pts[2].y = ptTo.y - mrgn;
		pts[3].x = ptTo.x;
		pts[3].y = ptTo.y + mrgn;
		pts[4].x = ptPath.x;
		pts[4].y = ptPath.y + mrgn;
		pts[5].x = ptFrom.x;
		pts[5].y = ptFrom.y + mrgn;
		CRgn* r = new CRgn;
		r->CreatePolygonRgn(pts, 6, WINDING);
		if (r->PtInRegion(pt)) {
			selected_ = true;
		}
		else {
			selected_ = false;
		}
		delete r;
		CRect cr = getCommentRect();
		if (cr.PtInRect(pt)) {
			selected_ = true;
		}
	}

	return selected_;
}

bool iLink::HitTestConnectionPtFrom(const CPoint &pt) const
{
	if (rcFrom.PtInRect(pt)) return false;
	CPoint pts[4];
	const int mrgn = 10;
	pts[0].x = ptFrom.x - mrgn;
	pts[0].y = ptFrom.y - mrgn;
	pts[1].x = ptFrom.x + mrgn;
	pts[1].y = ptFrom.y - mrgn;
	pts[2].x = ptFrom.x + mrgn;
	pts[2].y = ptFrom.y + mrgn;
	pts[3].x = ptFrom.x - mrgn;
	pts[3].y = ptFrom.y + mrgn;
	CRgn* r = new CRgn;
	r->CreatePolygonRgn(pts, 4, WINDING);
	BOOL bIn = r->PtInRegion(pt);
	delete r;
	return bIn == TRUE;
}

bool iLink::HitTestConnectionPtTo(const CPoint &pt) const
{
	if (rcTo.PtInRect(pt)) return false;
	CPoint pts[4];
	const int mrgn = 10;
	pts[0].x = ptTo.x - mrgn;
	pts[0].y = ptTo.y - mrgn;
	pts[1].x = ptTo.x + mrgn;
	pts[1].y = ptTo.y - mrgn;
	pts[2].x = ptTo.x + mrgn;
	pts[2].y = ptTo.y + mrgn;
	pts[3].x = ptTo.x - mrgn;
	pts[3].y = ptTo.y + mrgn;
	CRgn* r = new CRgn;
	r->CreatePolygonRgn(pts, 4, WINDING);
	BOOL bIn = r->PtInRegion(pt);
	delete r;
	return bIn == TRUE;
}


bool iLink::HitTestExcludePathPt(const CPoint &pt)
{
	if (HitTestConnectionPtFrom(pt) || HitTestConnectionPtTo(pt)) {
		return false;
	}
	CPoint pts[4];
	const int mrgn = 10;
	if (abs(ptFrom.x - ptTo.x) > abs(ptFrom.y - ptTo.y)) {
		pts[0].x = ptFrom.x;
		pts[0].y = ptFrom.y - mrgn;
		pts[1].x = ptFrom.x;
		pts[1].y = ptFrom.y + mrgn;
		pts[2].x = ptTo.x;
		pts[2].y = ptTo.y + mrgn;
		pts[3].x = ptTo.x;
		pts[3].y = ptTo.y - mrgn;
	}
	else {
		pts[0].x = ptFrom.x - mrgn;
		pts[0].y = ptFrom.y;
		pts[1].x = ptFrom.x + mrgn;
		pts[1].y = ptFrom.y;
		pts[2].x = ptTo.x + mrgn;
		pts[2].y = ptTo.y;
		pts[3].x = ptTo.x - mrgn;
		pts[3].y = ptTo.y;
	}
	CRgn* r = new CRgn;
	r->CreatePolygonRgn(pts, 4, WINDING);
	if (r->PtInRegion(pt)) {
		selected_ = true;
	}
	else {
		selected_ = false;
	}
	delete r;
	return selected_;
}

void iLink::DrawSelection(CDC *pDC)
{
	if (selected_) {
		CBrush Brs(BLACK_BRUSH);
		const int mrgn = 3;
		CRect FromRc(ptFrom.x - mrgn, ptFrom.y - mrgn, ptFrom.x + mrgn, ptFrom.y + mrgn);
		CRect ToRc(ptTo.x - mrgn, ptTo.y - mrgn, ptTo.x + mrgn, ptTo.y + mrgn);
		pDC->FillRect(ToRc, &Brs);
		pDC->FillRect(FromRc, &Brs);
		if (curved_) {
			CRect pathRc(ptPath.x - mrgn, ptPath.y - mrgn, ptPath.x + mrgn, ptPath.y + mrgn);
			pDC->FillRect(pathRc, &Brs);
		}
		pDC->MoveTo(ptFrom);
		if (curved_) {
			pDC->LineTo(ptPath);
		}
		pDC->LineTo(ptTo);
	}
}

void iLink::DrawSelectionFrom(CDC *pDC)
{
	const int mrgn = 5;
	CPoint topleft(ptFrom.x - mrgn, ptFrom.y - mrgn);
	CPoint bottomright(ptFrom.x + mrgn, ptFrom.y + mrgn);
	CRect rc(topleft, bottomright);
	pDC->Arc(rc, topleft, topleft);
}

void iLink::DrawSelectionTo(CDC *pDC)
{
	const int mrgn = 5;
	CPoint topleft(ptTo.x - mrgn, ptTo.y - mrgn);
	CPoint bottomright(ptTo.x + mrgn, ptTo.y + mrgn);
	CRect rc(topleft, bottomright);
	pDC->Arc(rc, topleft, topleft);
}

CRect iLink::getCommentRect() const
{
	CPoint cpt;
	if (!curved_) {
		cpt.x = (ptFrom.x + ptTo.x) / 2; cpt.y = (ptFrom.y + ptTo.y) / 2;
	}
	else {
		cpt = ptPath;
	}
	int width = name_.GetLength()*abs(lf_.lfHeight) / 2;
	int height = abs(lf_.lfHeight);
	if (name_.GetLength() == 0) {
		width = 5; height = 5;
	}

	cpt.x -= width / 2;
	cpt.y -= height / 2;
	CRect rc(cpt.x, cpt.y, cpt.x + width, cpt.y + height);
	return rc;
}

CRect iLink::getBound() const
{
	CRect rc;
	if (keyFrom != keyTo) {
		rc.TopLeft() = ptFrom;
		rc.BottomRight() = ptTo;
		rc.NormalizeRect();
		// ノード同士が正対してるとRectができないため調整
		rc.right += 5;
		rc.bottom += 5;
		if (curved_) {
			CRect r1(ptFrom, ptPath); r1.NormalizeRect();
			rc |= r1;
		}
	}
	else {
		rc = selfRect;
	}
	rc |= getCommentRect();
	rc.left -= 5;
	rc.right += 5;
	rc.top -= 5;
	rc.bottom += 5;
	return rc;
}

void iLink::ReverseDirection()
{
	DWORD kfrom = keyFrom;
	DWORD kto = keyTo;
	CRect rfrom = rcFrom;
	CRect rto = rcTo;
	keyFrom = kto;
	keyTo = kfrom;
	rcFrom = rto;
	rcTo = rfrom;
	SetConnectionPoint();
}

// iLinks : iLinks クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

iLinks::iLinks()
{
	dividedLinkKey_ = -1;
}

void iLinks::DrawComments(CDC *pDC, bool clipbrd)
{
	link_iter it = begin();
	for (; it != end(); it++) {
		if (!(*it).CanDraw()) {
			continue;
		}
		if ((*it).GetArrowStyle() != iLink::other) {
			(*it).DrawComment(pDC, clipbrd);
		}
	}
}

void iLinks::DrawLines(CDC *pDC)
{
	link_iter it = begin();
	for (; it != end(); it++) {
		if (!(*it).CanDraw()) {
			continue;
		}
		if ((*it).GetArrowStyle() != iLink::other) {
			(*it).DrawLine(pDC);
		}
	}
}

void iLinks::DrawArrows(CDC *pDC)
{
	link_iter it = begin();
	for (; it != end(); it++) {
		if (!(*it).CanDraw()) {
			continue;
		}
		if ((*it).GetArrowStyle() != iLink::other) {
			(*it).DrawArrow(pDC);
		}
	}
}

bool iLinks::HitTest(const CPoint &pt, DWORD& key, CString& path)
{
	link_iter it = begin();
	bool hit = false;
	for (; it != end(); it++) {
		if (!(*it).CanDraw()) {
			continue;
		}
		if ((*it).HitTest(pt)) {
			(*it).Select();
			key = (*it).GetFromNodeKey();
			path = (*it).GetPath();
			hit = true;
		}
		else {
			(*it).Select(false);
		}
	}
	return hit;
}

DWORD iLinks::HitTestDropTarget(const CPoint &pt, const DWORD selectedNodeKey)
{
	DWORD hitKey = -1;
	bool hit = false;
	link_iter it = begin();
	for (; it != end(); it++) {
		if (!(*it).CanDraw()) {
			continue;
		}
		if ((*it).GetFromNodeKey() == selectedNodeKey || (*it).GetToNodeKey() == selectedNodeKey) {
			continue;
		}
		if ((*it).HitTest(pt)) {
			if (!hit) {
				(*it).SetAsDropTarget();
				hitKey = (*it).GetKey();
				hit = true;
			}
		}
		else {
			(*it).SetAsDropTarget(false);
		}
	}
	return hitKey;
}

bool iLinks::HitTestFrom(const CPoint &pt, DWORD &key, CString &path)
{
	link_iter it = begin();
	bool hit = false;
	for (; it != end(); it++) {
		if (!(*it).CanDraw()) {
			continue;
		}
		if ((*it).HitTestConnectionPtFrom(pt)) {
			(*it).Select();
			key = (*it).GetFromNodeKey();
			path = (*it).GetPath();
			hit = true;
		}
		else {
			(*it).Select(false);
		}
	}
	return hit;
}

bool iLinks::HitTestTo(const CPoint &pt, DWORD &key, CString &path)
{
	link_iter it = begin();
	bool hit = false;
	for (; it != end(); it++) {
		if (!(*it).CanDraw()) {
			continue;
		}
		if ((*it).HitTestConnectionPtTo(pt)) {
			(*it).Select();
			key = (*it).GetFromNodeKey();
			path = (*it).GetPath();
			hit = true;
		}
		else {
			(*it).Select(false);
		}
	}
	return hit;
}

void iLinks::DrawSelection(CDC *pDC)
{
	link_iter it = begin();
	for (; it != end(); it++) {
		if (!(*it).CanDraw()) {
			continue;
		}
		if ((*it).IsSelected()) {
			(*it).DrawSelection(pDC);
		}
	}
}

void iLinks::DrawSelectionFrom(CDC *pDC)
{
	link_iter it = begin();
	for (; it != end(); it++) {
		if (!(*it).CanDraw()) {
			continue;
		}
		if ((*it).IsSelected()) {
			(*it).DrawSelectionFrom(pDC);
		}
	}
}

void iLinks::DrawSelectionTo(CDC *pDC)
{
	link_iter it = begin();
	for (; it != end(); it++) {
		if (!(*it).CanDraw()) {
			continue;
		}
		if ((*it).IsSelected()) {
			(*it).DrawSelectionTo(pDC);
		}
	}
}

link_c_iter iLinks::GetSelectedRead() const
{
	link_c_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).GetArrowStyle() != iLink::other) {
			if (!(*it).CanDraw()) {
				continue;
			}
			if ((*it).IsSelected()) {
				return it;
			}
		}
	}
	return end();
}

link_c_iter iLinks::GetSelectedRead2() const
{
	link_c_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).IsSelected()) {
			return it;
		}
	}
	return end();
}

link_iter iLinks::GetSelectedWrite()
{
	link_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).GetArrowStyle() != iLink::other) {
			if (!(*it).CanDraw()) {
				continue;
			}
			if ((*it).IsSelected()) {
				return it;
			}
		}
	}
	return end();
}

link_iter iLinks::GetSelectedWrite2()
{
	link_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).IsSelected()) {
			return it;
		}
	}
	return end();
}

void iLinks::SelectInBound(const CRect &r)
{
	link_iter it = begin();
	for (; it != end(); it++) {
		if (!(*it).CanDraw()) {
			continue;
		}
		if (!(*it).IsCurved() && (*it).GetFromNodeKey() != (*it).GetToNodeKey() && (*it).GetArrowStyle() != iLink::other) {
			if (r.PtInRect((*it).GetPtFrom()) && r.PtInRect((*it).GetPtTo())) {
				(*it).Select();
			}
			else {
				(*it).Select(false);
			}
		}
		else if ((*it).IsCurved() && (*it).GetFromNodeKey() != (*it).GetToNodeKey() && (*it).GetArrowStyle() != iLink::other) {
			if (r.PtInRect((*it).GetPtFrom()) && r.PtInRect((*it).GetPtTo()) && r.PtInRect((*it).GetPtPath())) {
				(*it).Select();
			}
			else {
				(*it).Select(false);
			}
		}
		else if ((*it).GetFromNodeKey() == (*it).GetToNodeKey() && (*it).GetArrowStyle() != iLink::other) {
			if (r.PtInRect((*it).GetSelfRect().TopLeft()) && r.PtInRect((*it).GetSelfRect().BottomRight())) {
				(*it).Select();
			}
			else {
				(*it).Select(false);
			}
		}
		else {
			(*it).Select(false);
		}
	}
}


void iLinks::GetSelectedFont(LOGFONT &lf)
{
	link_iter it = begin();
	for (; it != end(); it++) {
		if (!(*it).CanDraw()) {
			continue;
		}
		if ((*it).IsSelected()) {
			lf = (*it).GetFontInfo();
			::lstrcpy(lf.lfFaceName, (*it).GetFontInfo().lfFaceName);
		}
	}
}

void iLinks::SetSelectedFont(const LOGFONT &lf)
{
	link_iter it = begin();
	for (; it != end(); it++) {
		if (!(*it).CanDraw()) {
			continue;
		}
		if ((*it).IsSelected()) {
			(*it).SetFontInfo(lf);
		}
	}
}

void iLinks::SetSelectedLineColor(const COLORREF &c)
{
	link_iter it = begin();
	for (; it != end(); it++) {
		if (!(*it).CanDraw()) {
			continue;
		}
		if ((*it).IsSelected()) {
			(*it).SetLinkColor(c);
		}
	}
}

COLORREF iLinks::GetSelectedLineColor() const
{
	COLORREF c = RGB(0, 0, 0);
	link_c_iter it = GetSelectedRead();
	if (it != end()) {
		c = (*it).GetLinkColor();
	}
	return c;
}

void iLinks::SetSelectedWidth(int w)
{
	link_iter it = begin();
	for (; it != end(); it++) {
		if (!(*it).CanDraw()) {
			continue;
		}
		if ((*it).IsSelected()) {
			(*it).SetLineWidth(w);
		}
	}
}

int iLinks::GetSelectedWidth() const
{
	int w = 0;
	link_c_iter it = GetSelectedRead();
	if (it != end()) {
		w = (*it).GetLineWidth();
	}
	return w;
}

void iLinks::SetSelectedLineStyle(int style)
{
	link_iter it = begin();
	for (; it != end(); it++) {
		if (!(*it).CanDraw()) {
			continue;
		}
		if ((*it).IsSelected()) {
			(*it).SetLineStyle(style);
		}
	}
}

int iLinks::GetSelectedLineStyle() const
{
	int s = PS_SOLID;
	link_c_iter it = GetSelectedRead();
	if (it != end()) {
		s = (*it).GetLineStyle();
	}
	return s;
}

bool iLinks::IsNodeIsolated(DWORD nodeKey) const
{
	link_c_iter it = begin();
	for (; it != end(); it++) {
		if (!(*it).CanDraw()) {
			continue;
		}
		if ((*it).GetFromNodeKey() == nodeKey || (*it).GetToNodeKey() == nodeKey) {
			return false;
		}
	}
	return true;
}

CRect iLinks::GetSelectedBound() const
{
	link_c_iter it = GetSelectedRead();
	CRect rc(0, 0, 0, 0);
	if (it == end()) return rc;
	rc = (*it).getBound();
	it = begin();
	for (; it != end(); it++) {
		if ((*it).CanDraw()) {
			continue;
		}
		if ((*it).IsSelected()) {
			rc |= (*it).getBound();
			rc |= (*it).GetFromNodeRect();
			rc |= (*it).GetToNodeRect();
		}
	}
	return rc;
}

void iLinks::SetSelectedLinkNodeFrom(DWORD key, const CRect &bound)
{
	link_iter it = GetSelectedWrite();
	if (it != end()) {
		if (key != (*it).GetFromNodeKey()) {
			(*it).SetFromNodeKey(key);
			(*it).SetFromNodeRect(bound);
		}
	}
}

void iLinks::SetSelectedLinkNodeTo(DWORD key, const CRect& bound)
{
	link_iter it = GetSelectedWrite();
	if (it != end()) {
		if (key != (*it).GetToNodeKey()) {
			(*it).SetToNodeKey(key);
			(*it).SetToNodeRect(bound);
		}
	}
}

void iLinks::ReversetSelected()
{
	link_iter it = GetSelectedWrite();
	if (it != end()) {
		(*it).ReverseDirection();
	}
}

DWORD iLinks::GetFirstVisiblePair(DWORD key) const
{
	link_c_iter it = begin();
	for (; it != end(); it++) {
		if (!(*it).CanDraw()) continue;
		if ((*it).GetFromNodeKey() == (*it).GetToNodeKey()) continue;
		if ((*it).GetArrowStyle() == iLink::other) continue;
		if ((*it).IsTerminalNodeKey(key)) {
			if ((*it).GetFromNodeKey() == key) {
				return (*it).GetToNodeKey();
			}
			else {
				return (*it).GetFromNodeKey();
			}
		}
	}
	return -1;
}

void iLinks::ResizeSelectedFont(bool bEnlarge)
{
	link_iter it = begin();
	for (; it != end(); it++) {
		if ((*it).IsSelected()) {
			LOGFONT lf = (*it).GetFontInfo();
			LONG pre = lf.lfHeight;
			if (bEnlarge) {
				lf.lfHeight -= 2;
			}
			else {
				if (lf.lfHeight < -4) {
					lf.lfHeight += 2;
				}
			}
			(*it).SetFontInfo(lf);
		}
	}
}

link_c_iter iLinks::findByKey(DWORD key) const
{
	link_c_iter li = begin();
	for (; li != end(); li++) {
		if ((*li).GetKey() == key) return li;
	}

	return end();
}

DWORD iLinks::GetDividedKey() const
{
	if (dividedLinkKey_ != -1) {
		if (findByKey(dividedLinkKey_) != end()) {
			return dividedLinkKey_;
		}
	}
	return -1;
}

void iLinks::ClearDividedKey()
{
	dividedLinkKey_ = -1;
}

void iLinks::DivideTargetLinks(DWORD dropNodeKey, DWORD newLinkKey)
{
	link_iter li = begin();
	for (; li != end(); li++) {
		if ((*li).IsDropTarget()) {
			(*li).Curve(false);
			iLink l((*li));
			DWORD orgKeyTo = (*li).GetToNodeKey();
			(*li).SetToNodeKey(dropNodeKey);
			(*li).SetAsDropTarget(false);
			l.SetKey(newLinkKey);
			l.SetToNodeKey(orgKeyTo);
			l.SetFromNodeKey(dropNodeKey);
			push_back(l);
			dividedLinkKey_ = newLinkKey;
			break;
		}
	}
}
