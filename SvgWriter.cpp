// SvgWriter.cpp: SvgWriter クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "iEdit.h"
#include "SvgWriter.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

SvgWriter::SvgWriter(iNodes& nodes, iLinks& links, bool bDrwAll) :
   m_nodes(nodes), m_links(links), m_bDrwAll(bDrwAll)
{

}

SvgWriter::~SvgWriter()
{

}

void SvgWriter::exportSVG(const CString& path, const CPoint& maxPt)
{
	CWaitCursor wc;
	
	// DOM生成
	MSXML2::IXMLDOMDocumentPtr doc("MSXML.DOMDocument");
	
	doc->appendChild(
		doc->createProcessingInstruction(
		"xml", "version='1.0' encoding='utf-8'"));
	
	MSXML2::IXMLDOMElementPtr eSvg  = doc->createElement("svg");
	eSvg->setAttribute("xmlns", "http://www.w3.org/2000/svg");
	CString sWidth; sWidth.Format("%d", maxPt.x + 10);
	CString sHeight; sHeight.Format("%d", maxPt.y + 10);
	eSvg->setAttribute("width", sWidth.GetBuffer(sWidth.GetLength()));
	eSvg->setAttribute("height", sHeight.GetBuffer(sHeight.GetLength()));
	doc->appendChild(eSvg);
	
	
	// ノードの列挙
	const_niterator it = m_nodes.begin();
	for ( ; it != m_nodes.end(); it++) {
		if (!m_bDrwAll && !(*it).isVisible()) continue;
		MSXML2::IXMLDOMElementPtr eGrp  = doc->createElement("g");
		
		iNode node = (*it);
		MSXML2::IXMLDOMElementPtr eNode = createNodeElement(node, doc);
		if (eNode != NULL) {
			eGrp->appendChild(eNode);
		}
		
		if (node.getTextStyle() == iNode::notext && node.getNodeShape() != iNode::MetaFile) continue;
		// メタファイルの場合は、notextでもテキストを描画するようにする
		MSXML2::IXMLDOMElementPtr eNText = createNodeTextElement(node, doc);
		if (eNText != NULL) {
			eGrp->appendChild(eNText);
		}
		eSvg->appendChild(eGrp);
	}
	// リンクの列挙
	const_literator li = m_links.begin();
	for ( ; li != m_links.end(); li++) {
		if (!m_bDrwAll && !(*li).canDraw()) continue;
		iLink link = (*li);
		if (link.getArrowStyle() == iLink::other) continue;
		
		MSXML2::IXMLDOMElementPtr eGrp  = doc->createElement("g");
		
		MSXML2::IXMLDOMElementPtr eLink = createLinkElement(link, doc);
		if (eLink != NULL) {
			eGrp->appendChild(eLink);
		}
		MSXML2::IXMLDOMElementPtr eLText = createLinkTextElement(link, doc);
		if (eLText != NULL) {
			eGrp->appendChild(eLText);
		}
		if (link.getArrowStyle() != iLink::line && link.getArrowStyle() != iLink::other) {
			MSXML2::IXMLDOMElementPtr eArrow = createLinkArrowElement(link, doc);
			eGrp->appendChild(eArrow);
		}
		if (link.getArrowStyle() == iLink::arrow2 || link.getArrowStyle() == iLink::depend2) {
			MSXML2::IXMLDOMElementPtr eArrow2 = createLinkArrow2Element(link, doc);
			eGrp->appendChild(eArrow2);
		}
		eSvg->appendChild(eGrp);
	}
	
	CString spath = path;
	doc->save(spath.GetBuffer(spath.GetLength()));
	ShellExecute(NULL, "open", spath, NULL, NULL, SW_SHOW);
}

MSXML2::IXMLDOMElementPtr SvgWriter::createNodeElement(const iNode &node, MSXML2::IXMLDOMDocumentPtr pDoc)
{
	MSXML2::IXMLDOMElementPtr pNode = NULL;
	
	int shape = node.getNodeShape();
	CRect bound = node.getBound();
	CString sCx; sCx.Format("%d", bound.left);
	CString sCy; sCy.Format("%d", bound.top);
	CString sWidth; sWidth.Format("%d", bound.Width());
	CString sHeight; sHeight.Format("%d", bound.Height());
	
	if (shape == iNode::rectangle || shape == iNode::MetaFile ||
		shape == iNode::roundRect || shape == iNode::MindMapNode) {
		pNode = pDoc->createElement("rect");
		
		pNode->setAttribute("x", sCx.GetBuffer(sCx.GetLength()));
		pNode->setAttribute("y", sCy.GetBuffer(sCy.GetLength()));
		pNode->setAttribute("width", sWidth.GetBuffer(sWidth.GetLength()));
		pNode->setAttribute("height", sHeight.GetBuffer(sHeight.GetLength()));
		if (shape == iNode::roundRect) {
			int r = (bound.Width() < bound.Height()) ? bound.Width() : bound.Height();
			CString sR; sR.Format("%d", r/4);
			pNode->setAttribute("rx", sR.GetBuffer(sR.GetLength()));
			pNode->setAttribute("ry", sR.GetBuffer(sR.GetLength()));
		}
	} else if (shape == iNode::arc) {
		pNode = pDoc->createElement("ellipse");
		CPoint cPt = bound.CenterPoint();
		CString sCx; sCx.Format("%d", cPt.x);
		pNode->setAttribute("cx", sCx.GetBuffer(sCx.GetLength()));
		CString sCy; sCy.Format("%d", cPt.y);
		pNode->setAttribute("cy", sCy.GetBuffer(sCy.GetLength()));
		CString sRx; sRx.Format("%d", bound.Width()/2);
		pNode->setAttribute("rx", sRx.GetBuffer(sRx.GetLength()));
		CString sRy; sRy.Format("%d", bound.Height()/2);
		pNode->setAttribute("ry", sRy.GetBuffer(sRy.GetLength()));
	}
	
	CString sNodeStyle = createNodeStyleAtrb(node);
	pNode->setAttribute("style", sNodeStyle.GetBuffer(sNodeStyle.GetLength()));
	
	return pNode;
}

MSXML2::IXMLDOMElementPtr SvgWriter::createNodeTextElement(const iNode &node, MSXML2::IXMLDOMDocumentPtr pDoc)
{
	MSXML2::IXMLDOMElementPtr pNText = NULL;
	
	pNText = pDoc->createElement("text");
	
	// text alignment
	CPoint labelOrg = calcNodeLabelOrg(node);
	CString sCx; sCx.Format("%d", labelOrg.x);
	CString sCy; sCy.Format("%d", labelOrg.y);
	pNText->setAttribute("x", sCx.GetBuffer(sCx.GetLength()));
	pNText->setAttribute("y", sCy.GetBuffer(sCy.GetLength()));
	
	// フォントスタイル設定
	CString sStyle = createTextStyle(node.getFontInfo(), node.getFontColor());
	pNText->setAttribute("style", sStyle.GetBuffer(sStyle.GetLength()));
	
	// ラベル
	CString name = node.getName();
	pNText->Puttext(name.GetBuffer(name.GetLength()));
	
	
	/* URL貼り込みテスト
	MSXML2::IXMLDOMElementPtr pNodeRef = NULL;
	pNodeRef = pDoc->createElement("a");
	pNodeRef->setAttribute("xlink:href", "http://member.nifty.ne.jp/kondoumh/");
	pNodeRef->setAttribute("target", "_blank");
	pNodeRef->appendChild(pNText);
	return pNodeRef;
	*/
	
	return pNText;
}

// ラベルの配置計算用
CPoint SvgWriter::calcNodeLabelOrg(const iNode& node)
{
	LOGFONT lf = node.getFontInfo();
	int size = abs(lf.lfHeight);
	
	int width = node.getName().GetLength()*size/2;
	int height = size;
	
	CPoint pt = node.getBound().CenterPoint();
	pt.x -= width/2;
	pt.y += height*3/7;
	return pt;
}

CSize SvgWriter::getNodeTextSize(const iNode& node)
{
	CWnd wnd;
	CFont font; font.CreateFontIndirectA(&node.getFontInfo());
	CClientDC dc(&wnd);
	CFont* pOldFont = dc.SelectObject(&font);
	CSize sz = dc.GetTabbedTextExtentA(node.getName(), -1, 0, NULL);
	dc.SelectObject(pOldFont);
	return sz;
}

CString SvgWriter::createNodeStyleAtrb(const iNode &node)
{
	///// Stroke Style /////
	COLORREF lineColor = node.getLineColor();
	BYTE lRed GetRValue(lineColor);
	BYTE lGreen GetGValue(lineColor);
	BYTE lBlue GetBValue(lineColor);
	int lineStyle = node.getLineStyle();
	CString strokeStyle;
	if (lineStyle == PS_NULL) {
		strokeStyle = "stroke:none;";
	} else if (lineStyle == PS_DOT) {
		strokeStyle.Format("stroke:rgb(%d,%d,%d); stroke-width:1; stroke-dasharray:3,3;",
		  lRed, lGreen, lBlue);
	} else if (lineStyle == PS_SOLID) {
		int width = node.getLineWidth();
		if (width == 0) { width = 1; }
		strokeStyle.Format("stroke:rgb(%d,%d,%d); stroke-width:%d;", lRed, lGreen, lBlue, width);
	}

	if (node.getNodeShape() == iNode::MetaFile) {
		strokeStyle.Format("stroke:rgb(0,0,0); stroke-width:%d;", 1);
	}
	
	// fillColor
	COLORREF fill = node.getBrsColor();
	BYTE bRed GetRValue(fill);
	BYTE bGreen GetGValue(fill);
	BYTE bBlue GetBValue(fill);
	
	CString style;
	if (node.isFilled()) {
		style.Format("%s fill:rgb(%d,%d,%d)", strokeStyle, bRed, bGreen, bBlue); // #%02x%02x%02x
	} else {
		style.Format("%s fill:none", strokeStyle);
	}
	return style;
}

MSXML2::IXMLDOMElementPtr SvgWriter::createLinkElement(const iLink &link, MSXML2::IXMLDOMDocumentPtr pDoc)
{
	MSXML2::IXMLDOMElementPtr pLink = NULL;
	
	CPoint ptFrom = link.getPtFrom();
	CPoint ptTo = link.getPtTo();
	CPoint ptPath = link.getPtPath();
	COLORREF lineColor = link.getLinkColor();
	BYTE lRed GetRValue(lineColor);
	BYTE lGreen GetGValue(lineColor);
	BYTE lBlue GetBValue(lineColor);
	
	int lineStyle = link.getLineStyle();
	int width = link.getLineWidth();
	if (width == 0) { width = 1; }
	
	if (!link.isCurved() /* && link.getKeyFrom() != link.getKeyTo()*/) {
		pLink = pDoc->createElement("line");
		
		CString sX1; sX1.Format("%d", ptFrom.x);
		CString sY1; sY1.Format("%d", ptFrom.y);
		CString sX2; sX2.Format("%d", ptTo.x);
		CString sY2; sY2.Format("%d", ptTo.y);
		
		pLink->setAttribute("x1", sX1.GetBuffer(sX1.GetLength()));
		pLink->setAttribute("y1", sY1.GetBuffer(sY1.GetLength()));
		pLink->setAttribute("x2", sX2.GetBuffer(sX2.GetLength()));
		pLink->setAttribute("y2", sY2.GetBuffer(sY2.GetLength()));
		
		CString strokeStyle;
		if (lineStyle == PS_DOT) {
			strokeStyle.Format("stroke:rgb(%d,%d,%d); stroke-width:1; stroke-dasharray:3,3;",
			lRed, lGreen, lBlue);
		} else if (lineStyle == PS_SOLID) {
			strokeStyle.Format("stroke:rgb(%d,%d,%d); stroke-width:%d;", lRed, lGreen, lBlue, width);
		}
		pLink->setAttribute("style", strokeStyle.GetBuffer(strokeStyle.GetLength()));
	} else {
		pLink = pDoc->createElement("path");
		CString sPath;
		sPath.Format("M %d %d S %d %d %d %d", ptFrom.x, ptFrom.y, ptPath.x, ptPath.y, ptTo.x, ptTo.y);
		pLink->setAttribute("d", sPath.GetBuffer(sPath.GetLength()));
		
		CString sColor;
		sColor.Format("rgb(%d,%d,%d)", lRed, lGreen, lBlue);
		pLink->setAttribute("stroke", sColor.GetBuffer(sColor.GetLength()));
		pLink->setAttribute("fill", "none");
		if (lineStyle == PS_DOT) {
			pLink->setAttribute("stroke-width", "1");
			pLink->setAttribute("stroke-dasharray", "3");
		} else if (lineStyle == PS_SOLID) {
			CString sWidth; sWidth.Format("%d", width);
			pLink->setAttribute("stroke-width", sWidth.GetBuffer(sWidth.GetLength()));
		}
	}
	
	return pLink;
}

MSXML2::IXMLDOMElementPtr SvgWriter::createLinkTextElement(const iLink &link, MSXML2::IXMLDOMDocumentPtr pDoc)
{
	MSXML2::IXMLDOMElementPtr pLText = NULL;
	if (link.getName() == "") return NULL;
	
	// ラベル表示位置
	CPoint ptOrg = calcLinkLabelOrg(link);
	CString sCx; sCx.Format("%d", ptOrg.x);
	CString sCy; sCy.Format("%d", ptOrg.y);
	
	pLText = pDoc->createElement("text");
	pLText->setAttribute("x", sCx.GetBuffer(sCx.GetLength()));
	pLText->setAttribute("y", sCy.GetBuffer(sCy.GetLength()));
	
	// スタイル設定
	CString sStyle = createTextStyle(link.getFontInfo(), link.getLinkColor());
	pLText->setAttribute("style", sStyle.GetBuffer(sStyle.GetLength()));
	
	// ラベル設定
	CString name = link.getName();
	pLText->Puttext(name.GetBuffer(name.GetLength()));
	
	return pLText;
}

// リンクラベルの座標計算
CPoint SvgWriter::calcLinkLabelOrg(const iLink& link)
{
	LOGFONT lf = link.getFontInfo();
	int size = abs(lf.lfHeight);
	
	int width = link.getName().GetLength()*size/2;
	int height = size;
	
	CPoint pt;
	if (link.isCurved()) {
		pt = link.getPtPath();
	} else {
		pt.x = (link.getPtFrom().x + link.getPtTo().x)/2;
		pt.y = (link.getPtFrom().y + link.getPtTo().y)/2;
	}
	
	pt.x -= width/2;
	pt.y += height/2;
	
	return pt;
}

MSXML2::IXMLDOMElementPtr SvgWriter::createLinkArrowElement(const iLink &link, MSXML2::IXMLDOMDocumentPtr pDoc)
{
	DWORD keyFrom = link.getKeyFrom();
	DWORD keyTo = link.getKeyTo();
	CPoint ptTo = link.getPtTo();
	CPoint ptFrom = link.getPtFrom();
	CPoint ptPath = link.getPtPath();
	CRect selfRect = link.getSelfRect();
	
	CPoint pt[3];
	pt[0].x = ptTo.x;
	pt[0].y = ptTo.y;
	pt[1].x = pt[0].x + 10;
	pt[1].y = pt[0].y + 4;
	pt[2].x = pt[0].x + 10;
	pt[2].y = pt[0].y - 4;
	
	if (keyFrom != keyTo) {
		if (!link.isCurved()) {
			rotateArrow(pt, 3, ptFrom, ptTo, ptTo);
		} else {
			rotateArrow(pt, 3, ptPath, ptTo, ptTo);		
		}
	} else {
		CRect rcFrom = link.getRectFrom();
		CPoint gFrom = rcFrom.CenterPoint();
		CPoint from;
		if (ptPath.x >= gFrom.x && ptPath.y <= gFrom.y) {
			from.x = gFrom.x + selfRect.Width()/8; from.y = rcFrom.top - selfRect.Height();
		} else if (ptPath.x >= gFrom.x && ptPath.y > gFrom.y) {
			from.x = rcFrom.right+selfRect.Height(); from.y = gFrom.y + selfRect.Height()/8;
		} else if (ptPath.x < gFrom.x && ptPath.y > gFrom.y) {
			from.x = gFrom.x - selfRect.Width()/8, from.y = rcFrom.bottom + selfRect.Height();
		} else if (ptPath.x < gFrom.x && ptPath.y <= gFrom.y) {
			from.x = rcFrom.left - selfRect.Width(); from.y = gFrom.y - selfRect.Height()/8;
		}
		rotateArrow(pt, 3, from, ptTo, ptTo);
	}
	
	MSXML2::IXMLDOMElementPtr pArrow = NULL;
	
	pArrow = pDoc->createElement("path");
	CString sPath;
	sPath.Format("M %d %d L %d %d L %d %d z", pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[2].x, pt[2].y);
	pArrow->setAttribute("d", sPath.GetBuffer(sPath.GetLength()));
	
	COLORREF color = link.getLinkColor();
	BYTE bRed GetRValue(color);
	BYTE bGreen GetGValue(color);
	BYTE bBlue GetBValue(color);
	CString sStyle;
	sStyle.Format("fill:rgb(%d,%d,%d); ", bRed, bGreen, bBlue);
	
	pArrow->setAttribute("style", sStyle.GetBuffer(sStyle.GetLength()));
	
	return pArrow;
}

MSXML2::IXMLDOMElementPtr SvgWriter::createLinkArrow2Element(const iLink &link, MSXML2::IXMLDOMDocumentPtr pDoc)
{
	DWORD keyFrom = link.getKeyFrom();
	DWORD keyTo = link.getKeyTo();
	CPoint ptTo = link.getPtTo();
	CPoint ptFrom = link.getPtFrom();
	CPoint ptPath = link.getPtPath();
	CRect selfRect = link.getSelfRect();

	CPoint pt[3];
	pt[0].x = ptFrom.x;
	pt[0].y = ptFrom.y;
	pt[1].x = pt[0].x + 10;
	pt[1].y = pt[0].y + 4;
	pt[2].x = pt[0].x + 10;
	pt[2].y = pt[0].y - 4;
	
	if (keyFrom != keyTo) {
		if (!link.isCurved()) {
			rotateArrow(pt, 3, ptTo, ptFrom, ptFrom);
		} else {
			rotateArrow(pt, 3, ptPath, ptFrom, ptFrom);
		}
	} else {
		CRect rcFrom = link.getRectFrom();
		CPoint gFrom = rcFrom.CenterPoint();
		CPoint from;
		if (ptPath.x >= gFrom.x && ptPath.y <= gFrom.y) {
			from.x = rcFrom.right+selfRect.Height(); from.y = gFrom.y - selfRect.Height()/8;
		} else if (ptPath.x >= gFrom.x && ptPath.y > gFrom.y) {
			from.x = gFrom.x + selfRect.Width()/8, from.y = rcFrom.bottom + selfRect.Height();
		} else if (ptPath.x < gFrom.x && ptPath.y > gFrom.y) {
			from.x = rcFrom.left - selfRect.Width(); from.y = gFrom.y + selfRect.Height()/8;
		} else if (ptPath.x < gFrom.x && ptPath.y <= gFrom.y) {
			from.x = gFrom.x - selfRect.Width()/8; from.y = rcFrom.top - selfRect.Height();
		}
		rotateArrow(pt, 3, from, ptFrom, ptFrom);
	}
	
	MSXML2::IXMLDOMElementPtr pArrow = NULL;
	
	pArrow = pDoc->createElement("path");
	CString sPath;
	sPath.Format("M %d %d L %d %d L %d %d z", pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[2].x, pt[2].y);
	pArrow->setAttribute("d", sPath.GetBuffer(sPath.GetLength()));
	
	COLORREF color = link.getLinkColor();
	BYTE bRed GetRValue(color);
	BYTE bGreen GetGValue(color);
	BYTE bBlue GetBValue(color);
	CString sStyle;
	sStyle.Format("fill:rgb(%d,%d,%d); ", bRed, bGreen, bBlue);
	
	pArrow->setAttribute("style", sStyle.GetBuffer(sStyle.GetLength()));
	
	return pArrow;
}

void SvgWriter::rotateArrow(CPoint *pPoint, int size, CPoint &pFrom, CPoint &pTo, CPoint &ptOrg)
{
	double c,s;
	double r = sqrt(pow((double)(pTo.x - pFrom.x), 2) + pow((double)(pTo.y - pFrom.y), 2));
	c = ((double)(pFrom.x - pTo.x))/r;
	s = ((double)(pFrom.y - pTo.y))/r;
	
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

CString SvgWriter::createTextStyle(const LOGFONT& lf, COLORREF fontColor)
{
	CString sStyle;
	
	// font size
	CString s; s.Format("font-size:%dpt; ", abs(lf.lfHeight)*11/15);
	sStyle += s;
	
	// fill color
	BYTE bRed GetRValue(fontColor);
	BYTE bGreen GetGValue(fontColor);
	BYTE bBlue GetBValue(fontColor);
	s.Format("fill:rgb(%d,%d,%d); ", bRed, bGreen, bBlue);
	sStyle += s;
	
	// font family
	CString sFName = lf.lfFaceName;
	if (sFName == "ＭＳ 明朝") {
		sFName = "MS-Mincho";
	} else if (sFName == "ＭＳ Ｐ明朝") {
		sFName = "MS-PMincho";
	} else if (sFName == "ＭＳ ゴシック") {
		sFName = "MS-Gothic";
	} else if (sFName == "ＭＳ Ｐゴシック") {
		sFName = "MS-PGothic";
	} else if (sFName == "メイリオ") {
		sFName = "Meiryo";
	} else {
		sFName = "MS-Gothic";
	}
	s.Format("font-family:%s", sFName.GetBuffer(sFName.GetLength()));
	sStyle += s;
	
	// font style
	if (lf.lfItalic) {
		sStyle += "; font-style:italic; ";
	}	
	if (lf.lfStrikeOut) {
		sStyle += "; text-decoration:underline";
	}
	if (lf.lfUnderline) {
		sStyle += "; text-decoration:line-through";
	}
	
	// font width
	if (lf.lfWeight == FW_BOLD) {
		sStyle += "; font-weight:bold";
	}
	return sStyle;
}
