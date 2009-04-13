// SvgWriter.h: SvgWriter クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SVGWRITER_H__4589807F_F92A_4D52_B27B_2B685172227C__INCLUDED_)
#define AFX_SVGWRITER_H__4589807F_F92A_4D52_B27B_2B685172227C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "iNode.h"
#include "iLink.h"

class SvgWriter  
{
public:
	void exportSVG(const CString& path, const CPoint& maxPt, bool bEmbed=false);
	SvgWriter(iNodes& nodes, iLinks& links, serialVec& drawOrder, bool bDrwAll=false);
	virtual ~SvgWriter();

private:
	iNodes& m_nodes;
	iLinks& m_links;
	bool m_bDrwAll;
	serialVec m_drawOrder;
	CString m_TextHtmlFileName;
	CString m_TextHtmlFilePrefix;
protected:
	CString extractfirstURLfromText(const CString& text);
	vector<CString> splitTSpan(const CString& label, const int labelWidth, const int boundWidth);
	vector<CString> splitByWidth(const CString& line, const int byte);
	CSize getNodeTextSize(const iNode& node);
	MSXML2::IXMLDOMElementPtr createLinkSquareElement(const iLink& link, MSXML2::IXMLDOMDocumentPtr pDoc);
	MSXML2::IXMLDOMElementPtr createLinkArrow2Element(const iLink& link, MSXML2::IXMLDOMDocumentPtr pDoc);
	MSXML2::IXMLDOMElementPtr createLinkArrowElement(const iLink& link, MSXML2::IXMLDOMDocumentPtr pDoc);
	MSXML2::IXMLDOMElementPtr createLinkTextElement(const iLink& link, MSXML2::IXMLDOMDocumentPtr pDoc);
	MSXML2::IXMLDOMElementPtr createNodeTextElement(const iNode& node, MSXML2::IXMLDOMDocumentPtr pDoc, bool bEmbed);
	MSXML2::IXMLDOMElementPtr createLinkElement(const iLink& link, MSXML2::IXMLDOMDocumentPtr pDoc);
	MSXML2::IXMLDOMElementPtr createNodeElement(const iNode& node, MSXML2::IXMLDOMDocumentPtr pDoc);
	void rotateArrow(CPoint *pPoint, int size, CPoint &pFrom, CPoint &pTo, CPoint &ptOrg);
	CString createNodeStyleAtrb(const iNode& node);
	// リンクラベルの座標計算
	CPoint calcLinkLabelOrg(const iLink& link);
	CString createTextStyle(const LOGFONT& lf, COLORREF fontColor);
};

#endif // !defined(AFX_SVGWRITER_H__4589807F_F92A_4D52_B27B_2B685172227C__INCLUDED_)
