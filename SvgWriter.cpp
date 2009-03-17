// SvgWriter.cpp: SvgWriter クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "iEdit.h"
#include "SvgWriter.h"
#include "math.h"
#include "Token.h"

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

void SvgWriter::exportSVG(const CString& path, const CPoint& maxPt, bool bEmbed)
{
	CWaitCursor wc;
	
	// DOM生成
	MSXML2::IXMLDOMDocumentPtr doc("MSXML.DOMDocument");
	
	doc->appendChild(
		doc->createProcessingInstruction(
		"xml", "version='1.0' encoding='utf-8'"));
	
	MSXML2::IXMLDOMElementPtr eSvg  = doc->createElement("svg");
	eSvg->setAttribute("xmlns", "http://www.w3.org/2000/svg");
	eSvg->setAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
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
		MSXML2::IXMLDOMElementPtr eNText = createNodeTextElement(node, doc, bEmbed);
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
		if (link.getArrowStyle() == iLink::arrow || link.getArrowStyle() == iLink::arrow2 ||
			link.getArrowStyle() == iLink::depend || link.getArrowStyle() == iLink::depend2 ||
			link.getArrowStyle() == iLink::inherit) {
			MSXML2::IXMLDOMElementPtr eArrow = createLinkArrowElement(link, doc);
			eGrp->appendChild(eArrow);
		} else if (link.getArrowStyle() == iLink::aggregat || link.getArrowStyle() == iLink::composit) {
			MSXML2::IXMLDOMElementPtr eSquare = createLinkSquareElement(link, doc);
			eGrp->appendChild(eSquare);
		}
		if (link.getArrowStyle() == iLink::arrow2 || link.getArrowStyle() == iLink::depend2) {
			MSXML2::IXMLDOMElementPtr eArrow2 = createLinkArrow2Element(link, doc);
			eGrp->appendChild(eArrow2);
		}
		eSvg->appendChild(eGrp);
	}
	
	CString spath = path;
	doc->save(spath.GetBuffer(spath.GetLength()));
//	ShellExecute(NULL, "open", spath, NULL, NULL, SW_SHOW);
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
		shape == iNode::roundRect) {
		pNode = pDoc->createElement("rect");
		
		pNode->setAttribute("x", sCx.GetBuffer(sCx.GetLength()));
		pNode->setAttribute("y", sCy.GetBuffer(sCy.GetLength()));
		pNode->setAttribute("width", sWidth.GetBuffer(sWidth.GetLength()));
		pNode->setAttribute("height", sHeight.GetBuffer(sHeight.GetLength()));
		if (shape == iNode::roundRect) {
			int r = (bound.Width() < bound.Height()) ? bound.Width() : bound.Height();
			CString sR; sR.Format("%d", r/8);
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
	} else if (shape == iNode::MindMapNode) {
		pNode = pDoc->createElement("line");
		CString sX1; sX1.Format("%d", bound.left);
		CString sY1; sY1.Format("%d", bound.bottom);
		CString sX2; sX2.Format("%d", bound.right);
		CString sY2; sY2.Format("%d", bound.bottom);
		
		pNode->setAttribute("x1", sX1.GetBuffer(sX1.GetLength()));
		pNode->setAttribute("y1", sY1.GetBuffer(sY1.GetLength()));
		pNode->setAttribute("x2", sX2.GetBuffer(sX2.GetLength()));
		pNode->setAttribute("y2", sY2.GetBuffer(sY2.GetLength()));
	}
	
	CString sNodeStyle = createNodeStyleAtrb(node);
	pNode->setAttribute("style", sNodeStyle.GetBuffer(sNodeStyle.GetLength()));
	
	return pNode;
}

MSXML2::IXMLDOMElementPtr SvgWriter::createNodeTextElement(const iNode &node, MSXML2::IXMLDOMDocumentPtr pDoc, bool bEmbed)
{
	MSXML2::IXMLDOMElementPtr pNText = NULL;
	
	pNText = pDoc->createElement("text");
	CSize textSize = getNodeTextSize(node);
	
	// text alignment
	CString sTop; sTop.Format("%d", node.getBound().top + textSize.cy/2 + 1);
	CString sBottom; sBottom.Format("%d", node.getBound().bottom - 1);
	
	CRect bound = node.getBound();
	int r = (bound.Width() < bound.Height()) ? bound.Width() : bound.Height();
	int left = node.getBound().left + 1;
	if (node.getNodeShape() == iNode::roundRect) {
		left += r/8;
	}
	CString sLeft; sLeft.Format("%d", left);
	
	int right = node.getBound().right - 1;
	if (node.getNodeShape() == iNode::roundRect) {
		right -= r/8;
	}
	CString sRight; sRight.Format("%d", right);
	
	CString sHCenter; sHCenter.Format("%d", node.getBound().left + node.getBound().Width()/2);
	CString sVCenter; sVCenter.Format("%d", 
		node.getBound().top + node.getBound().Height()/2 + textSize.cy/3);
	
	switch (node.getTextStyle())
	{
	case iNode::s_bc:
		pNText->setAttribute("x", sHCenter.GetBuffer(sHCenter.GetLength()));
		pNText->setAttribute("y", sBottom.GetBuffer(sBottom.GetLength()));
		pNText->setAttribute("text-anchor", "middle");
		break;
	case iNode::s_bl:
		pNText->setAttribute("x", sLeft.GetBuffer(sLeft.GetLength()));
		pNText->setAttribute("y", sBottom.GetBuffer(sBottom.GetLength()));
		pNText->setAttribute("text-anchor", "start");
		break;
	case iNode::s_br:
		pNText->setAttribute("x", sRight.GetBuffer(sRight.GetLength()));
		pNText->setAttribute("y", sBottom.GetBuffer(sBottom.GetLength()));
		pNText->setAttribute("text-anchor", "end");
		break;
	case iNode::s_cc:
		pNText->setAttribute("x", sHCenter.GetBuffer(sHCenter.GetLength()));
		pNText->setAttribute("y", sVCenter.GetBuffer(sVCenter.GetLength()));
		pNText->setAttribute("text-anchor", "middle");
		break;
	case iNode::s_cl:
		pNText->setAttribute("x", sLeft.GetBuffer(sLeft.GetLength()));
		pNText->setAttribute("y", sVCenter.GetBuffer(sVCenter.GetLength()));
		pNText->setAttribute("text-anchor", "start");
		break;
	case iNode::s_cr:
		pNText->setAttribute("x", sRight.GetBuffer(sRight.GetLength()));
		pNText->setAttribute("y", sVCenter.GetBuffer(sVCenter.GetLength()));
		pNText->setAttribute("text-anchor", "end");
		break;
	case iNode::s_tc:
		pNText->setAttribute("x", sHCenter.GetBuffer(sHCenter.GetLength()));
		pNText->setAttribute("y", sTop.GetBuffer(sTop.GetLength()));
		pNText->setAttribute("text-anchor", "middle");
		break;
	case iNode::s_tl:
		pNText->setAttribute("x", sLeft.GetBuffer(sLeft.GetLength()));
		pNText->setAttribute("y", sTop.GetBuffer(sTop.GetLength()));
		pNText->setAttribute("text-anchor", "start");
		break;
	case iNode::s_tr:
		pNText->setAttribute("x", sRight.GetBuffer(sRight.GetLength()));
		pNText->setAttribute("y", sTop.GetBuffer(sTop.GetLength()));
		pNText->setAttribute("text-anchor", "end");
		break;
	case iNode::m_c:
		pNText->setAttribute("x", sHCenter.GetBuffer(sHCenter.GetLength()));
		pNText->setAttribute("y", sTop.GetBuffer(sTop.GetLength()));
		pNText->setAttribute("text-anchor", "middle");
		break;
	case iNode::m_l:
		pNText->setAttribute("x", sLeft.GetBuffer(sLeft.GetLength()));
		pNText->setAttribute("y", sTop.GetBuffer(sTop.GetLength()));
		pNText->setAttribute("text-anchor", "start");
		break;
	case iNode::m_r:
		pNText->setAttribute("x", sRight.GetBuffer(sRight.GetLength()));
		pNText->setAttribute("y", sTop.GetBuffer(sTop.GetLength()));
		pNText->setAttribute("text-anchor", "end");
		break;
	}
	
	// フォントスタイル設定
	CString sStyle = createTextStyle(node.getFontInfo(), node.getFontColor());
	pNText->setAttribute("style", sStyle.GetBuffer(sStyle.GetLength()));
	
	// ラベル
	CString name = node.getName();
	int style = node.getTextStyle();
	if (style == iNode::m_c || style == iNode::m_l || style == iNode::m_r) {
		CString sDx; sDx.Format("%d", textSize.cy);
		vector<CString> lines = splitTSpan(node.getName(), textSize.cx, node.getBound().Width());
		for (unsigned int i = 0; i < lines.size(); i++) {
			MSXML2::IXMLDOMElementPtr pNtspan = pDoc->createElement("tspan");
			if (i > 0) {
				pNtspan->setAttribute("x", pNText->getAttribute("x"));
				pNtspan->setAttribute("dy", sDx.GetBuffer(sDx.GetLength()));
			}
			pNtspan->Puttext(lines[i].GetBuffer(lines[i].GetLength()));
			pNText->appendChild(pNtspan);
		}
	} else {
		pNText->Puttext(name.GetBuffer(name.GetLength()));
	}
	
	if (bEmbed) {
		CString sKey; sKey.Format("%d", node.getKey());
		CString url = "text.html#" + sKey;
		MSXML2::IXMLDOMElementPtr pNodeRef = NULL;
		pNodeRef = pDoc->createElement("a");
		pNodeRef->setAttribute("xlink:href", url.GetBuffer(url.GetLength()));
		pNodeRef->setAttribute("target", "text");
		pNodeRef->appendChild(pNText);
		return pNodeRef;
	} else {
		// nodeのテキストの1行目がURLだったらリンクを作る
		// TODO:リンクにURLが含まれている場合はそれの1個目を取ってくる
		CString url = extractfirstURLfromText(node.getText());
		if (url != "") {
			MSXML2::IXMLDOMElementPtr pNodeRef = NULL;
			pNodeRef = pDoc->createElement("a");
			pNodeRef->setAttribute("xlink:href", url.GetBuffer(url.GetLength()));
			pNodeRef->setAttribute("target", "_blank");
			CString addStyle = "fill:rgb(0,0,255); text-decoration:underline;";
			CString sStyle = pNText->getAttribute("style");
			sStyle += addStyle;
			pNText->setAttribute("style", sStyle.GetBuffer(sStyle.GetLength()));
			pNodeRef->appendChild(pNText);
			return pNodeRef;
		}
	}
	return pNText;
}

CString SvgWriter::extractfirstURLfromText(const CString& text)
{
	int pos = 0;
	CString firstline = text.Tokenize("\n", pos);
	if (firstline != "" && firstline.Find("http://", 0) != -1) {
		return firstline;
	}
	return "";
}

vector<CString> SvgWriter::splitTSpan(const CString& label, const int labelWidth, const int boundWidth)
{
	vector<CString> v;
	int bytePerLine = (boundWidth/(labelWidth/label.GetLength())) - 1;
	
	int pos = 0;
	CString token = " ";
	while (token != "") {
		token = label.Tokenize("\n", pos);
		vector<CString> lines = splitByWidth(token, bytePerLine);
		for (unsigned int i = 0; i < lines.size(); i++) {
			v.push_back(lines[i]);
		}
	}
	return v;
}

vector<CString> SvgWriter::splitByWidth(const CString& line, const int byte)
{
	vector<CString> v;
	int j = 0;
	CString split;
	for (int i = 0; i < line.GetLength(); i++) {
		split += line.GetAt(i);
		j++;
		if (_ismbblead(line.GetAt(i)) != 0) {
			split += line.GetAt(i+1);
			i++;
			j++;
		}
		if (j >= byte || i >= line.GetLength() - 1) {
			v.push_back(split);
			j = 0;
			split = "";
		}
	}
	return v;
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
		if (link.isAngled()) {
			sPath.Format("M %d %d L %d %d %d %d", ptFrom.x, ptFrom.y, ptPath.x, ptPath.y, ptTo.x, ptTo.y);
		} else {
			sPath.Format("M %d %d C %d %d %d %d %d %d", ptFrom.x, ptFrom.y, ptPath.x, ptPath.y, ptPath.x, ptPath.y,ptTo.x, ptTo.y);
		}	
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
	CPoint ptTo = link.getPtTo();
	CPoint ptFrom = link.getPtFrom();
	CPoint ptPath = link.getPtPath();
	CRect selfRect = link.getSelfRect();
	int width = link.getLineWidth();
	if (width == 0) width = 1;
	
	int ArrowWidth = 5;
	int ArrowHeight = 12;
	switch (width) {
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
	
	CPoint pt[3];
	pt[0].x = ptTo.x;
	pt[0].y = ptTo.y;
	pt[1].x = pt[0].x + ArrowHeight;
	pt[1].y = pt[0].y + ArrowWidth;
	pt[2].x = pt[0].x + ArrowHeight;
	pt[2].y = pt[0].y - ArrowWidth;
	
	
	if (!link.isCurved()) {
		rotateArrow(pt, 3, ptFrom, ptTo, ptTo);
	} else {
		rotateArrow(pt, 3, ptPath, ptTo, ptTo);		
	}
	
	MSXML2::IXMLDOMElementPtr pArrow = NULL;
	
	pArrow = pDoc->createElement("path");
	CString sPath;
	
	if (link.getArrowStyle() == iLink::depend || link.getArrowStyle() == iLink::depend2) {
		sPath.Format("M %d %d L %d %d L %d %d", pt[1].x, pt[1].y, pt[0].x, pt[0].y, pt[2].x, pt[2].y);
	} else {
		sPath.Format("M %d %d L %d %d L %d %d z", pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[2].x, pt[2].y);
	}
	pArrow->setAttribute("d", sPath.GetBuffer(sPath.GetLength()));

	
	CString sStyle;
		COLORREF color = link.getLinkColor();
	BYTE bRed GetRValue(color);
	BYTE bGreen GetGValue(color);
	BYTE bBlue GetBValue(color);
	if (link.getArrowStyle() == iLink::arrow || link.getArrowStyle() == iLink::arrow2) {
		sStyle.Format("fill:rgb(%d,%d,%d); ", bRed, bGreen, bBlue);
	} else if (link.getArrowStyle() == iLink::inherit) {
		sStyle = "fill:rgb(255,255,255); ";
	} else if (link.getArrowStyle() == iLink::depend || link.getArrowStyle() == iLink::depend2) {
		sStyle = "fill:none; ";
	}
	CString sStroke;
	sStroke.Format("stroke:rgb(%d,%d,%d); stroke-width:%d;", bRed, bGreen, bBlue, width);
	sStyle += sStroke;
	pArrow->setAttribute("style", sStyle.GetBuffer(sStyle.GetLength()));
	return pArrow;
}

MSXML2::IXMLDOMElementPtr SvgWriter::createLinkSquareElement(const iLink &link, MSXML2::IXMLDOMDocumentPtr pDoc)
{
	CPoint ptTo = link.getPtTo();
	CPoint ptFrom = link.getPtFrom();
	CPoint ptPath = link.getPtPath();
	CRect selfRect = link.getSelfRect();
	int width = link.getLineWidth();
	if (width == 0) width = 1;
	
	int ArrowWidth = 5;
	int ArrowHeight = 12;
	switch (width) {
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
	
	CPoint pt[4];
	pt[0].x = ptTo.x;
	pt[0].y = ptTo.y;
	pt[1].x = pt[0].x + ArrowHeight;
	pt[1].y = pt[0].y + ArrowWidth;
	pt[2].x = pt[1].x + ArrowHeight;
	pt[2].y = pt[0].y;
	pt[3].x = pt[1].x;
	pt[3].y = pt[0].y - ArrowWidth;
	
	
	if (!link.isCurved()) {
		rotateArrow(pt, 4, ptFrom, ptTo, ptTo);
	} else {
		rotateArrow(pt, 4, ptPath, ptTo, ptTo);		
	}
	
	MSXML2::IXMLDOMElementPtr pArrow = NULL;
	
	pArrow = pDoc->createElement("path");
	CString sPath;
	
	sPath.Format("M %d %d L %d %d L %d %d L %d %d z", pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[2].x, pt[2].y, pt[3].x, pt[3].y);
	pArrow->setAttribute("d", sPath.GetBuffer(sPath.GetLength()));

	
	CString sStyle;
		COLORREF color = link.getLinkColor();
	BYTE bRed GetRValue(color);
	BYTE bGreen GetGValue(color);
	BYTE bBlue GetBValue(color);
	if (link.getArrowStyle() == iLink::composit) {
		sStyle.Format("fill:rgb(%d,%d,%d); ", bRed, bGreen, bBlue);
	} else if (link.getArrowStyle() == iLink::aggregat) {
		sStyle = "fill:rgb(255,255,255); ";
	}
	CString sStroke;
	sStroke.Format("stroke:rgb(%d,%d,%d); stroke-width:%d;", bRed, bGreen, bBlue, width);
	sStyle += sStroke;
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
	int width = link.getLineWidth();
	if (width == 0) width = 1;
	
	int ArrowWidth = 5;
	int ArrowHeight = 12;
	switch (width) {
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
	
	CPoint pt[3];
	pt[0].x = ptFrom.x;
	pt[0].y = ptFrom.y;
	pt[1].x = pt[0].x + ArrowHeight;
	pt[1].y = pt[0].y + ArrowWidth;
	pt[2].x = pt[0].x + ArrowHeight;
	pt[2].y = pt[0].y - ArrowWidth;
	
	if (!link.isCurved()) {
		rotateArrow(pt, 3, ptTo, ptFrom, ptFrom);
	} else {
		rotateArrow(pt, 3, ptPath, ptFrom, ptFrom);
	}
	
	MSXML2::IXMLDOMElementPtr pArrow = NULL;
	
	pArrow = pDoc->createElement("path");
	CString sPath;
	if (link.getArrowStyle() == iLink::depend || link.getArrowStyle() == iLink::depend2) {
		sPath.Format("M %d %d L %d %d L %d %d", pt[1].x, pt[1].y, pt[0].x, pt[0].y, pt[2].x, pt[2].y);
	} else {
		sPath.Format("M %d %d L %d %d L %d %d z", pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[2].x, pt[2].y);
	}
	pArrow->setAttribute("d", sPath.GetBuffer(sPath.GetLength()));
	
	CString sStyle;
	COLORREF color = link.getLinkColor();
	BYTE bRed GetRValue(color);
	BYTE bGreen GetGValue(color);
	BYTE bBlue GetBValue(color);
	if (link.getArrowStyle() == iLink::arrow2) {
		sStyle.Format("fill:rgb(%d,%d,%d); ", bRed, bGreen, bBlue);
	} else if (link.getArrowStyle() == iLink::depend2) {
		sStyle = "fill:none; ";
	}
	CString sStroke;
	sStroke.Format("stroke:rgb(%d,%d,%d); stroke-width:%d;", bRed, bGreen, bBlue, width);
	sStyle += sStroke;
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
		sStyle += "; font-style:italic";
	}
	if (lf.lfStrikeOut || lf.lfUnderline) {
		sStyle += "; text-decoration:";
		if (lf.lfStrikeOut) {
			sStyle += "line-through ";
		}
		if (lf.lfUnderline) {
			sStyle += "underline";
		}
	}
	
	
	// font width
	if (lf.lfWeight == FW_BOLD) {
		sStyle += "; font-weight:bold";
	}
	
	sStyle += "; ";
	
	return sStyle;
}
