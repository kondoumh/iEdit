// SvgWriter.cpp: SvgWriter クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "iEdit.h"
#include "SvgWriter.h"
#include "math.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

SvgWriter::SvgWriter(iNodes& nodes, iLinks& links, NodeKeyVec& drawOrder) :
	m_nodes(nodes), m_links(links), m_drawOrder(drawOrder),
	m_TextHtmlFileName(_T("")), m_TextHtmlFilePrefix(_T(""))
{
}

SvgWriter::~SvgWriter()
{

}

void SvgWriter::SetTextHtmlFileName(const CString& fileName)
{
	m_TextHtmlFileName = fileName;
}

void SvgWriter::SetTextHtmlFilePrefix(const CString& prefix)
{
	m_TextHtmlFilePrefix = prefix;
}

void SvgWriter::Write(const CString& path, const CPoint& maxPt, bool bEmbed)
{
	CWaitCursor wc;

	// DOM生成
	MSXML2::IXMLDOMDocumentPtr doc(_T("MSXML.DOMDocument"));

	doc->appendChild(
		doc->createProcessingInstruction(
			_T("xml"), _T("version='1.0' encoding='utf-8'")));

	MSXML2::IXMLDOMElementPtr eSvg = doc->createElement(_T("svg"));
	eSvg->setAttribute(_T("xmlns"), _T("http://www.w3.org/2000/svg"));
	eSvg->setAttribute(_T("xmlns:xlink"), _T("http://www.w3.org/1999/xlink"));
	CString sWidth; sWidth.Format(_T("%d"), maxPt.x + 10);
	CString sHeight; sHeight.Format(_T("%d"), maxPt.y + 10);
	eSvg->setAttribute(_T("width"), sWidth.GetBuffer(sWidth.GetLength()));
	eSvg->setAttribute(_T("height"), sHeight.GetBuffer(sHeight.GetLength()));
	doc->appendChild(eSvg);


	// ノードの列挙
	vector<DWORD>::iterator svIt = m_drawOrder.begin();
	for (; svIt != m_drawOrder.end(); svIt++) {
		node_c_iter it = m_nodes.FindRead(*svIt);
		if (!(*it).second.Visible()) continue;
		MSXML2::IXMLDOMElementPtr eGrp = doc->createElement(_T("g"));

		iNode node = (*it).second;
		MSXML2::IXMLDOMElementPtr eNode = CreateNodeElement(node, doc);
		if (eNode != NULL) {
			eGrp->appendChild(eNode);
		}

		if (node.GetTextStyle() == iNode::notext && node.GetShape() != iNode::MetaFile) continue;
		// メタファイルの場合は、notextでもテキストを描画するようにする
		MSXML2::IXMLDOMElementPtr eNText = CreateNodeTextElement(node, doc, bEmbed);
		if (eNText != NULL) {
			eGrp->appendChild(eNText);
		}
		eSvg->appendChild(eGrp);
	}
	// リンクの列挙
	link_c_iter li = m_links.begin();
	for (; li != m_links.end(); li++) {
		if (!(*li).CanDraw()) continue;
		iLink link = (*li);
		if (link.GetArrowStyle() == iLink::other) continue;

		MSXML2::IXMLDOMElementPtr eGrp = doc->createElement(_T("g"));

		MSXML2::IXMLDOMElementPtr eLink = CreateLinkElement(link, doc);
		if (eLink != NULL) {
			eGrp->appendChild(eLink);
		}
		MSXML2::IXMLDOMElementPtr eLText = CreateLinkTextElement(link, doc);
		if (eLText != NULL) {
			eGrp->appendChild(eLText);
		}
		if (link.GetArrowStyle() == iLink::arrow || link.GetArrowStyle() == iLink::arrow2 ||
			link.GetArrowStyle() == iLink::depend || link.GetArrowStyle() == iLink::depend2 ||
			link.GetArrowStyle() == iLink::inherit) {
			MSXML2::IXMLDOMElementPtr eArrow = CreateLinkArrowElement(link, doc);
			eGrp->appendChild(eArrow);
		}
		else if (link.GetArrowStyle() == iLink::aggregat || link.GetArrowStyle() == iLink::composit) {
			MSXML2::IXMLDOMElementPtr eSquare = CreateLinkSquareElement(link, doc);
			eGrp->appendChild(eSquare);
		}
		if (link.GetArrowStyle() == iLink::arrow2 || link.GetArrowStyle() == iLink::depend2) {
			MSXML2::IXMLDOMElementPtr eArrow2 = CreateLinkArrow2Element(link, doc);
			eGrp->appendChild(eArrow2);
		}
		eSvg->appendChild(eGrp);
	}

	CString spath = path;
	doc->save(spath.GetBuffer(spath.GetLength()));
	//	ShellExecute(NULL, "open", spath, NULL, NULL, SW_SHOW);
}

MSXML2::IXMLDOMElementPtr SvgWriter::CreateNodeElement(const iNode &node, MSXML2::IXMLDOMDocumentPtr pDoc)
{
	MSXML2::IXMLDOMElementPtr pNode = NULL;

	int shape = node.GetShape();
	CRect bound = node.GetBound();
	CString sCx; sCx.Format(_T("%d"), bound.left);
	CString sCy; sCy.Format(_T("%d"), bound.top);
	CString sWidth; sWidth.Format(_T("%d"), bound.Width());
	CString sHeight; sHeight.Format(_T("%d"), bound.Height());

	if (shape == iNode::rectangle || shape == iNode::MetaFile ||
		shape == iNode::roundRect) {
		pNode = pDoc->createElement(_T("rect"));

		pNode->setAttribute(_T("x"), sCx.GetBuffer(sCx.GetLength()));
		pNode->setAttribute(_T("y"), sCy.GetBuffer(sCy.GetLength()));
		pNode->setAttribute(_T("width"), sWidth.GetBuffer(sWidth.GetLength()));
		pNode->setAttribute(_T("height"), sHeight.GetBuffer(sHeight.GetLength()));
		if (shape == iNode::roundRect) {
			int r = (bound.Width() < bound.Height()) ? bound.Width() : bound.Height();
			CString sR; sR.Format(_T("%d"), r / 8);
			pNode->setAttribute(_T("rx"), sR.GetBuffer(sR.GetLength()));
			pNode->setAttribute(_T("ry"), sR.GetBuffer(sR.GetLength()));
		}
	}
	else if (shape == iNode::arc) {
		pNode = pDoc->createElement(_T("ellipse"));
		CPoint cPt = bound.CenterPoint();
		CString sCx; sCx.Format(_T("%d"), cPt.x);
		pNode->setAttribute(_T("cx"), sCx.GetBuffer(sCx.GetLength()));
		CString sCy; sCy.Format(_T("%d"), cPt.y);
		pNode->setAttribute(_T("cy"), sCy.GetBuffer(sCy.GetLength()));
		CString sRx; sRx.Format(_T("%d"), bound.Width() / 2);
		pNode->setAttribute(_T("rx"), sRx.GetBuffer(sRx.GetLength()));
		CString sRy; sRy.Format(_T("%d"), bound.Height() / 2);
		pNode->setAttribute(_T("ry"), sRy.GetBuffer(sRy.GetLength()));
	}
	else if (shape == iNode::MindMapNode) {
		pNode = pDoc->createElement(_T("line"));
		CString sX1; sX1.Format(_T("%d"), bound.left);
		CString sY1; sY1.Format(_T("%d"), bound.bottom);
		CString sX2; sX2.Format(_T("%d"), bound.right);
		CString sY2; sY2.Format(_T("%d"), bound.bottom);

		pNode->setAttribute(_T("x1"), sX1.GetBuffer(sX1.GetLength()));
		pNode->setAttribute(_T("y1"), sY1.GetBuffer(sY1.GetLength()));
		pNode->setAttribute(_T("x2"), sX2.GetBuffer(sX2.GetLength()));
		pNode->setAttribute(_T("y2"), sY2.GetBuffer(sY2.GetLength()));
	}

	CString sNodeStyle = CreateNodeStyleAttrib(node);
	pNode->setAttribute(_T("style"), sNodeStyle.GetBuffer(sNodeStyle.GetLength()));

	return pNode;
}

MSXML2::IXMLDOMElementPtr SvgWriter::CreateNodeTextElement(const iNode &node, MSXML2::IXMLDOMDocumentPtr pDoc, bool bEmbed)
{
	MSXML2::IXMLDOMElementPtr pNText = NULL;

	pNText = pDoc->createElement(_T("text"));
	CSize textSize = GetNodeTextSize(node);

	// text alignment
	CString sTop; sTop.Format(_T("%d"), node.GetBound().top + textSize.cy / 2 + 1 + node.GetMarginTop());
	CString sBottom; sBottom.Format(_T("%d"), node.GetBound().bottom - 1 - node.GetMarginBottom());

	CRect bound = node.GetBound();
	int r = (bound.Width() < bound.Height()) ? bound.Width() : bound.Height();
	int left = node.GetBound().left + 1 + node.GetMarginLeft();
	if (node.GetShape() == iNode::roundRect) {
		left += r / 8;
	}
	CString sLeft; sLeft.Format(_T("%d"), left);

	int right = node.GetBound().right - 1 - node.GetMarginRight();
	if (node.GetShape() == iNode::roundRect) {
		right -= r / 8;
	}
	CString sRight; sRight.Format(_T("%d"), right);

	CString sHCenter; sHCenter.Format(_T("%d"), node.GetBound().left + node.GetBound().Width() / 2);
	CString sVCenter; sVCenter.Format(_T("%d"),
		node.GetBound().top + node.GetBound().Height() / 2 + textSize.cy / 3);

	switch (node.GetTextStyle())
	{
	case iNode::s_bc:
		pNText->setAttribute(_T("x"), sHCenter.GetBuffer(sHCenter.GetLength()));
		pNText->setAttribute(_T("y"), sBottom.GetBuffer(sBottom.GetLength()));
		pNText->setAttribute(_T("text-anchor"), _T("middle"));
		break;
	case iNode::s_bl:
		pNText->setAttribute(_T("x"), sLeft.GetBuffer(sLeft.GetLength()));
		pNText->setAttribute(_T("y"), sBottom.GetBuffer(sBottom.GetLength()));
		pNText->setAttribute(_T("text-anchor"), _T("start"));
		break;
	case iNode::s_br:
		pNText->setAttribute(_T("x"), sRight.GetBuffer(sRight.GetLength()));
		pNText->setAttribute(_T("y"), sBottom.GetBuffer(sBottom.GetLength()));
		pNText->setAttribute(_T("text-anchor"), _T("end"));
		break;
	case iNode::s_cc:
		pNText->setAttribute(_T("x"), sHCenter.GetBuffer(sHCenter.GetLength()));
		pNText->setAttribute(_T("y"), sVCenter.GetBuffer(sVCenter.GetLength()));
		pNText->setAttribute(_T("text-anchor"), _T("middle"));
		break;
	case iNode::s_cl:
		pNText->setAttribute(_T("x"), sLeft.GetBuffer(sLeft.GetLength()));
		pNText->setAttribute(_T("y"), sVCenter.GetBuffer(sVCenter.GetLength()));
		pNText->setAttribute(_T("text-anchor"), _T("start"));
		break;
	case iNode::s_cr:
		pNText->setAttribute(_T("x"), sRight.GetBuffer(sRight.GetLength()));
		pNText->setAttribute(_T("y"), sVCenter.GetBuffer(sVCenter.GetLength()));
		pNText->setAttribute(_T("text-anchor"), _T("end"));
		break;
	case iNode::s_tc:
		pNText->setAttribute(_T("x"), sHCenter.GetBuffer(sHCenter.GetLength()));
		pNText->setAttribute(_T("y"), sTop.GetBuffer(sTop.GetLength()));
		pNText->setAttribute(_T("text-anchor"), _T("middle"));
		break;
	case iNode::s_tl:
		pNText->setAttribute(_T("x"), sLeft.GetBuffer(sLeft.GetLength()));
		pNText->setAttribute(_T("y"), sTop.GetBuffer(sTop.GetLength()));
		pNText->setAttribute(_T("text-anchor"), _T("start"));
		break;
	case iNode::s_tr:
		pNText->setAttribute(_T("x"), sRight.GetBuffer(sRight.GetLength()));
		pNText->setAttribute(_T("y"), sTop.GetBuffer(sTop.GetLength()));
		pNText->setAttribute(_T("text-anchor"), _T("end"));
		break;
	case iNode::m_c:
		pNText->setAttribute(_T("x"), sHCenter.GetBuffer(sHCenter.GetLength()));
		pNText->setAttribute(_T("y"), sTop.GetBuffer(sTop.GetLength()));
		pNText->setAttribute(_T("text-anchor"), _T("middle"));
		break;
	case iNode::m_l:
		pNText->setAttribute(_T("x"), sLeft.GetBuffer(sLeft.GetLength()));
		pNText->setAttribute(_T("y"), sTop.GetBuffer(sTop.GetLength()));
		pNText->setAttribute(_T("text-anchor"), _T("start"));
		break;
	case iNode::m_r:
		pNText->setAttribute(_T("x"), sRight.GetBuffer(sRight.GetLength()));
		pNText->setAttribute(_T("y"), sTop.GetBuffer(sTop.GetLength()));
		pNText->setAttribute(_T("text-anchor"), _T("end"));
		break;
	}

	// フォントスタイル設定
	CString sStyle = CreateTextStyle(node.GetFontInfo(), node.GetFontColor());
	pNText->setAttribute(_T("style"), sStyle.GetBuffer(sStyle.GetLength()));

	// ラベル
	CString name = node.GetName();
	int style = node.GetTextStyle();
	if (style == iNode::m_c || style == iNode::m_l || style == iNode::m_r) {
		CString sDx; sDx.Format(_T("%d"), textSize.cy);
		vector<CString> lines = SplitTSpan(node.GetName(),
			textSize.cx,
			node.GetBound().Width() - node.GetMarginLeft() - node.GetMarginRight());
		for (unsigned int i = 0; i < lines.size(); i++) {
			MSXML2::IXMLDOMElementPtr pNtspan = pDoc->createElement(_T("tspan"));
			if (i > 0) {
				pNtspan->setAttribute(_T("x"), pNText->getAttribute(_T("x")));
				pNtspan->setAttribute(_T("dy"), sDx.GetBuffer(sDx.GetLength()));
			}
			pNtspan->Puttext(lines[i].GetBuffer(lines[i].GetLength()));
			pNText->appendChild(pNtspan);
		}
	}
	else {
		pNText->Puttext(name.GetBuffer(name.GetLength()));
	}

	if (bEmbed) {
		CString sKey; sKey.Format(_T("%d"), node.GetKey());
		CString url = _T("text.html#") + sKey;
		if (m_TextHtmlFileName != _T("")) {
			url = m_TextHtmlFileName + _T("#") + sKey;
		}
		else {
			url = _T("text/") + m_TextHtmlFilePrefix + sKey + _T(".html");
		}
		MSXML2::IXMLDOMElementPtr pNodeRef = NULL;
		pNodeRef = pDoc->createElement(_T("a"));
		pNodeRef->setAttribute(_T("xlink:href"), url.GetBuffer(url.GetLength()));
		pNodeRef->setAttribute(_T("target"), _T("text"));
		pNodeRef->appendChild(pNText);
		return pNodeRef;
	}
	else {
		// リンクかテキストの1行目からURLを抽出する)
		CString url;
		link_c_iter li = m_links.begin();
		for (; li != m_links.end(); li++) {
			if ((*li).GetFromNodeKey() == node.GetKey()) {
				if ((*li).GetArrowStyle() == iLink::other) {
					CString name = (*li).GetPath();
					if (name.Find(_T("http://")) != -1 || name.Find(_T("https://")) != -1) {
						url = name;
						break;
					}
				}
			}
		}
		if (url == _T("")) {
			url = FindFirstUrl(node.GetText());
		}
		if (url != _T("")) {
			MSXML2::IXMLDOMElementPtr pNodeRef = NULL;
			pNodeRef = pDoc->createElement(_T("a"));
			pNodeRef->setAttribute(_T("xlink:href"), url.GetBuffer(url.GetLength()));
			pNodeRef->setAttribute(_T("target"), _T("_blank"));
			CString addStyle = _T("fill:rgb(0,0,255); text-decoration:underline;");
			CString sStyle = pNText->getAttribute(_T("style"));
			sStyle += addStyle;
			pNText->setAttribute(_T("style"), sStyle.GetBuffer(sStyle.GetLength()));
			pNodeRef->appendChild(pNText);
			return pNodeRef;
		}
	}
	return pNText;
}

CString SvgWriter::FindFirstUrl(const CString& text)
{
	int pos = 0;
	CString firstline = text.Tokenize(_T("\n"), pos);
	if (firstline != _T("") && firstline.Find(_T("http://"), 0) != -1) {
		return firstline;
	}
	return _T("");
}

vector<CString> SvgWriter::SplitTSpan(const CString& label, const int labelWidth, const int boundWidth)
{
	vector<CString> v;
	int bytePerLine = (boundWidth / (labelWidth / label.GetLength())) - 1;

	int pos = 0;
	CString token = _T(" ");
	while (token != _T("")) {
		token = label.Tokenize(_T("\n"), pos);
		vector<CString> lines = SplitByWidth(token, bytePerLine);
		for (unsigned int i = 0; i < lines.size(); i++) {
			v.push_back(lines[i]);
		}
	}
	return v;
}

vector<CString> SvgWriter::SplitByWidth(const CString& line, const int byte)
{
	vector<CString> v;
	int j = 0;
	CString split;
	for (int i = 0; i < line.GetLength(); i++) {
		split += line.GetAt(i);
		j++;
		if (_ismbblead(line.GetAt(i)) != 0) {
			split += line.GetAt(i + 1);
			i++;
			j++;
		}
		if (j >= byte || i >= line.GetLength() - 1) {
			v.push_back(split);
			j = 0;
			split = _T("");
		}
	}
	return v;
}

// TODO:Utilitiesへ移動
CSize SvgWriter::GetNodeTextSize(const iNode& node)
{
	CWnd wnd;
	CFont font; font.CreateFontIndirect(&node.GetFontInfo());
	CClientDC dc(&wnd);
	CFont* pOldFont = dc.SelectObject(&font);
	CSize sz = dc.GetTabbedTextExtent(node.GetName(), -1, 0, NULL);
	dc.SelectObject(pOldFont);
	return sz;
}

CString SvgWriter::CreateNodeStyleAttrib(const iNode &node)
{
	///// Stroke Style /////
	COLORREF lineColor = node.GetLineColor();
	BYTE lRed GetRValue(lineColor);
	BYTE lGreen GetGValue(lineColor);
	BYTE lBlue GetBValue(lineColor);
	int lineStyle = node.GetLineStyle();
	CString strokeStyle;
	if (lineStyle == PS_NULL) {
		strokeStyle = _T("stroke:none;");
	}
	else if (lineStyle == PS_DOT) {
		strokeStyle.Format(_T("stroke:rgb(%d,%d,%d); stroke-width:1; stroke-dasharray:3,3;"),
			lRed, lGreen, lBlue);
	}
	else if (lineStyle == PS_SOLID) {
		int width = node.GetLineWidth();
		if (width == 0) { width = 1; }
		strokeStyle.Format(_T("stroke:rgb(%d,%d,%d); stroke-width:%d;"), lRed, lGreen, lBlue, width);
	}

	if (node.GetShape() == iNode::MetaFile) {
		strokeStyle.Format(_T("stroke:rgb(0,0,0); stroke-width:%d;"), 1);
	}

	// fillColor
	COLORREF fill = node.GetFillColor();
	BYTE bRed GetRValue(fill);
	BYTE bGreen GetGValue(fill);
	BYTE bBlue GetBValue(fill);

	CString style;
	if (node.Filled()) {
		style.Format(_T("%s fill:rgb(%d,%d,%d)"), strokeStyle, bRed, bGreen, bBlue); // #%02x%02x%02x
	}
	else {
		style.Format(_T("%s fill:none"), strokeStyle);
	}
	return style;
}

MSXML2::IXMLDOMElementPtr SvgWriter::CreateLinkElement(const iLink &link, MSXML2::IXMLDOMDocumentPtr pDoc)
{
	MSXML2::IXMLDOMElementPtr pLink = NULL;

	CPoint ptFrom = link.GetPtFrom();
	CPoint ptTo = link.GetPtTo();
	CPoint ptPath = link.GetPtPath();
	COLORREF lineColor = link.GetLinkColor();
	BYTE lRed GetRValue(lineColor);
	BYTE lGreen GetGValue(lineColor);
	BYTE lBlue GetBValue(lineColor);

	int lineStyle = link.GetLineStyle();
	int width = link.SetLineWidth();
	if (width == 0) { width = 1; }

	if (!link.IsCurved() /* && link.GetFromNodeKey() != link.GetToNodeKey()*/) {
		pLink = pDoc->createElement(_T("line"));

		CString sX1; sX1.Format(_T("%d"), ptFrom.x);
		CString sY1; sY1.Format(_T("%d"), ptFrom.y);
		CString sX2; sX2.Format(_T("%d"), ptTo.x);
		CString sY2; sY2.Format(_T("%d"), ptTo.y);

		pLink->setAttribute(_T("x1"), sX1.GetBuffer(sX1.GetLength()));
		pLink->setAttribute(_T("y1"), sY1.GetBuffer(sY1.GetLength()));
		pLink->setAttribute(_T("x2"), sX2.GetBuffer(sX2.GetLength()));
		pLink->setAttribute(_T("y2"), sY2.GetBuffer(sY2.GetLength()));

		CString strokeStyle;
		if (lineStyle == PS_DOT) {
			strokeStyle.Format(_T("stroke:rgb(%d,%d,%d); stroke-width:1; stroke-dasharray:3,3;"),
				lRed, lGreen, lBlue);
		}
		else if (lineStyle == PS_SOLID) {
			strokeStyle.Format(_T("stroke:rgb(%d,%d,%d); stroke-width:%d;"), lRed, lGreen, lBlue, width);
		}
		pLink->setAttribute(_T("style"), strokeStyle.GetBuffer(strokeStyle.GetLength()));
	}
	else {
		pLink = pDoc->createElement(_T("path"));
		CString sPath;
		if (link.IsAngled()) {
			sPath.Format(_T("M %d %d L %d %d %d %d"), ptFrom.x, ptFrom.y, ptPath.x, ptPath.y, ptTo.x, ptTo.y);
		}
		else {
			sPath.Format(_T("M %d %d C %d %d %d %d %d %d"), ptFrom.x, ptFrom.y, ptPath.x, ptPath.y, ptPath.x, ptPath.y, ptTo.x, ptTo.y);
		}
		pLink->setAttribute(_T("d"), sPath.GetBuffer(sPath.GetLength()));

		CString sColor;
		sColor.Format(_T("rgb(%d,%d,%d)"), lRed, lGreen, lBlue);
		pLink->setAttribute(_T("stroke"), sColor.GetBuffer(sColor.GetLength()));
		pLink->setAttribute(_T("fill"), _T("none"));
		if (lineStyle == PS_DOT) {
			pLink->setAttribute(_T("stroke-width"), _T("1"));
			pLink->setAttribute(_T("stroke-dasharray"), _T("3"));
		}
		else if (lineStyle == PS_SOLID) {
			CString sWidth; sWidth.Format(_T("%d"), width);
			pLink->setAttribute(_T("stroke-width"), sWidth.GetBuffer(sWidth.GetLength()));
		}
	}

	return pLink;
}

MSXML2::IXMLDOMElementPtr SvgWriter::CreateLinkTextElement(const iLink &link, MSXML2::IXMLDOMDocumentPtr pDoc)
{
	MSXML2::IXMLDOMElementPtr pLText = NULL;
	if (link.GetName() == _T("")) return NULL;

	// ラベル表示位置
	CPoint ptOrg = CalcLinkLabelOrg(link);
	CString sCx; sCx.Format(_T("%d"), ptOrg.x);
	CString sCy; sCy.Format(_T("%d"), ptOrg.y);

	pLText = pDoc->createElement(_T("text"));
	pLText->setAttribute(_T("x"), sCx.GetBuffer(sCx.GetLength()));
	pLText->setAttribute(_T("y"), sCy.GetBuffer(sCy.GetLength()));

	// スタイル設定
	CString sStyle = CreateTextStyle(link.GetFontInfo(), link.GetLinkColor());
	pLText->setAttribute(_T("style"), sStyle.GetBuffer(sStyle.GetLength()));

	// ラベル設定
	CString name = link.GetName();
	pLText->Puttext(name.GetBuffer(name.GetLength()));

	return pLText;
}

// リンクラベルの座標計算
CPoint SvgWriter::CalcLinkLabelOrg(const iLink& link)
{
	LOGFONT lf = link.GetFontInfo();
	int size = abs(lf.lfHeight);

	int width = link.GetName().GetLength()*size / 2;
	int height = size;

	CPoint pt;
	if (link.IsCurved()) {
		pt = link.GetPtPath();
	}
	else {
		pt.x = (link.GetPtFrom().x + link.GetPtTo().x) / 2;
		pt.y = (link.GetPtFrom().y + link.GetPtTo().y) / 2;
	}

	pt.x -= width / 2;
	pt.y += height / 2;

	return pt;
}

MSXML2::IXMLDOMElementPtr SvgWriter::CreateLinkArrowElement(const iLink &link, MSXML2::IXMLDOMDocumentPtr pDoc)
{
	CPoint ptTo = link.GetPtTo();
	CPoint ptFrom = link.GetPtFrom();
	CPoint ptPath = link.GetPtPath();
	CRect selfRect = link.GetSelfRect();
	int width = link.SetLineWidth();
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


	if (!link.IsCurved()) {
		RotateArrow(pt, 3, ptFrom, ptTo, ptTo);
	}
	else {
		RotateArrow(pt, 3, ptPath, ptTo, ptTo);
	}

	MSXML2::IXMLDOMElementPtr pArrow = NULL;

	pArrow = pDoc->createElement(_T("path"));
	CString sPath;

	if (link.GetArrowStyle() == iLink::depend || link.GetArrowStyle() == iLink::depend2) {
		sPath.Format(_T("M %d %d L %d %d L %d %d"), pt[1].x, pt[1].y, pt[0].x, pt[0].y, pt[2].x, pt[2].y);
	}
	else {
		sPath.Format(_T("M %d %d L %d %d L %d %d z"), pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[2].x, pt[2].y);
	}
	pArrow->setAttribute(_T("d"), sPath.GetBuffer(sPath.GetLength()));


	CString sStyle;
	COLORREF color = link.GetLinkColor();
	BYTE bRed GetRValue(color);
	BYTE bGreen GetGValue(color);
	BYTE bBlue GetBValue(color);
	if (link.GetArrowStyle() == iLink::arrow || link.GetArrowStyle() == iLink::arrow2) {
		sStyle.Format(_T("fill:rgb(%d,%d,%d); "), bRed, bGreen, bBlue);
	}
	else if (link.GetArrowStyle() == iLink::inherit) {
		sStyle = _T("fill:rgb(255,255,255); ");
	}
	else if (link.GetArrowStyle() == iLink::depend || link.GetArrowStyle() == iLink::depend2) {
		sStyle = _T("fill:none; ");
	}
	CString sStroke;
	sStroke.Format(_T("stroke:rgb(%d,%d,%d); stroke-width:%d;"), bRed, bGreen, bBlue, width);
	sStyle += sStroke;
	pArrow->setAttribute(_T("style"), sStyle.GetBuffer(sStyle.GetLength()));
	return pArrow;
}

MSXML2::IXMLDOMElementPtr SvgWriter::CreateLinkSquareElement(const iLink &link, MSXML2::IXMLDOMDocumentPtr pDoc)
{
	CPoint ptTo = link.GetPtTo();
	CPoint ptFrom = link.GetPtFrom();
	CPoint ptPath = link.GetPtPath();
	CRect selfRect = link.GetSelfRect();
	int width = link.SetLineWidth();
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


	if (!link.IsCurved()) {
		RotateArrow(pt, 4, ptFrom, ptTo, ptTo);
	}
	else {
		RotateArrow(pt, 4, ptPath, ptTo, ptTo);
	}

	MSXML2::IXMLDOMElementPtr pArrow = NULL;

	pArrow = pDoc->createElement(_T("path"));
	CString sPath;

	sPath.Format(_T("M %d %d L %d %d L %d %d L %d %d z"), pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[2].x, pt[2].y, pt[3].x, pt[3].y);
	pArrow->setAttribute(_T("d"), sPath.GetBuffer(sPath.GetLength()));


	CString sStyle;
	COLORREF color = link.GetLinkColor();
	BYTE bRed GetRValue(color);
	BYTE bGreen GetGValue(color);
	BYTE bBlue GetBValue(color);
	if (link.GetArrowStyle() == iLink::composit) {
		sStyle.Format(_T("fill:rgb(%d,%d,%d); "), bRed, bGreen, bBlue);
	}
	else if (link.GetArrowStyle() == iLink::aggregat) {
		sStyle = _T("fill:rgb(255,255,255); ");
	}
	CString sStroke;
	sStroke.Format(_T("stroke:rgb(%d,%d,%d); stroke-width:%d;"), bRed, bGreen, bBlue, width);
	sStyle += sStroke;
	pArrow->setAttribute(_T("style"), sStyle.GetBuffer(sStyle.GetLength()));
	return pArrow;
}

MSXML2::IXMLDOMElementPtr SvgWriter::CreateLinkArrow2Element(const iLink &link, MSXML2::IXMLDOMDocumentPtr pDoc)
{
	DWORD keyFrom = link.GetFromNodeKey();
	DWORD keyTo = link.GetToNodeKey();
	CPoint ptTo = link.GetPtTo();
	CPoint ptFrom = link.GetPtFrom();
	CPoint ptPath = link.GetPtPath();
	CRect selfRect = link.GetSelfRect();
	int width = link.SetLineWidth();
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

	if (!link.IsCurved()) {
		RotateArrow(pt, 3, ptTo, ptFrom, ptFrom);
	}
	else {
		RotateArrow(pt, 3, ptPath, ptFrom, ptFrom);
	}

	MSXML2::IXMLDOMElementPtr pArrow = NULL;

	pArrow = pDoc->createElement(_T("path"));
	CString sPath;
	if (link.GetArrowStyle() == iLink::depend || link.GetArrowStyle() == iLink::depend2) {
		sPath.Format(_T("M %d %d L %d %d L %d %d"), pt[1].x, pt[1].y, pt[0].x, pt[0].y, pt[2].x, pt[2].y);
	}
	else {
		sPath.Format(_T("M %d %d L %d %d L %d %d z"), pt[0].x, pt[0].y, pt[1].x, pt[1].y, pt[2].x, pt[2].y);
	}
	pArrow->setAttribute(_T("d"), sPath.GetBuffer(sPath.GetLength()));

	CString sStyle;
	COLORREF color = link.GetLinkColor();
	BYTE bRed GetRValue(color);
	BYTE bGreen GetGValue(color);
	BYTE bBlue GetBValue(color);
	if (link.GetArrowStyle() == iLink::arrow2) {
		sStyle.Format(_T("fill:rgb(%d,%d,%d); "), bRed, bGreen, bBlue);
	}
	else if (link.GetArrowStyle() == iLink::depend2) {
		sStyle = _T("fill:none; ");
	}
	CString sStroke;
	sStroke.Format(_T("stroke:rgb(%d,%d,%d); stroke-width:%d;"), bRed, bGreen, bBlue, width);
	sStyle += sStroke;
	pArrow->setAttribute(_T("style"), sStyle.GetBuffer(sStyle.GetLength()));

	return pArrow;
}

void SvgWriter::RotateArrow(CPoint *pPoint, int size, CPoint &pFrom, CPoint &pTo, CPoint &ptOrg)
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

CString SvgWriter::CreateTextStyle(const LOGFONT& lf, COLORREF fontColor)
{
	CString sStyle;

	// font size
	CString s; s.Format(_T("font-size:%dpt; "), abs(lf.lfHeight) * 11 / 15);
	sStyle += s;

	// fill color
	BYTE bRed GetRValue(fontColor);
	BYTE bGreen GetGValue(fontColor);
	BYTE bBlue GetBValue(fontColor);
	s.Format(_T("fill:rgb(%d,%d,%d); "), bRed, bGreen, bBlue);
	sStyle += s;

	// font family
	CString sFName = lf.lfFaceName;
	if (sFName == _T("ＭＳ 明朝")) {
		sFName = _T("MS-Mincho");
	}
	else if (sFName == _T("ＭＳ Ｐ明朝")) {
		sFName = _T("MS-PMincho");
	}
	else if (sFName == _T("ＭＳ ゴシック")) {
		sFName = _T("MS-Gothic");
	}
	else if (sFName == _T("ＭＳ Ｐゴシック")) {
		sFName = _T("MS-PGothic");
	}
	else if (sFName == _T("メイリオ")) {
		sFName = _T("Meiryo");
	}
	else {
		sFName = _T("MS-Gothic");
	}
	s.Format(_T("font-family:%s"), sFName.GetBuffer(sFName.GetLength()));
	sStyle += s;

	// font style
	if (lf.lfItalic) {
		sStyle += _T("; font-style:italic");
	}
	if (lf.lfStrikeOut || lf.lfUnderline) {
		sStyle += _T("; text-decoration:");
		if (lf.lfStrikeOut) {
			sStyle += _T("line-through ");
		}
		if (lf.lfUnderline) {
			sStyle += _T("underline");
		}
	}


	// font width
	if (lf.lfWeight == FW_BOLD) {
		sStyle += _T("; font-weight:bold");
	}

	sStyle += _T("; ");

	return sStyle;
}
