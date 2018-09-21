// SvgWriter.h: SvgWriter クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "iNode.h"
#include "iLink.h"

#import "msxml6.dll" named_guids

class SvgWriter
{
public:
	SvgWriter(iNodes& nodes, iLinks& links, NodeKeyVec& drawOrder);
	virtual ~SvgWriter();
	void Write(const CString& path, const CPoint& maxPt, bool bEmbed = false);
	void SetTextHtmlFileName(const CString& fileName);
	void SetTextHtmlFilePrefix(const CString& prefix);

private:
	iNodes& m_nodes;
	iLinks& m_links;
	NodeKeyVec m_drawOrder;
	CString m_TextHtmlFileName;
	CString m_TextHtmlFilePrefix;

	CString FindFirstUrl(const CString& text);
	std::vector<CString> SplitTSpan(const CString& label, const int labelWidth, const int boundWidth);
	std::vector<CString> SplitByWidth(const CString& line, const int byte);
	CSize GetNodeTextSize(const iNode& node);
	MSXML2::IXMLDOMElementPtr CreateLinkSquareElement(const iLink& link, MSXML2::IXMLDOMDocumentPtr pDoc);
	MSXML2::IXMLDOMElementPtr CreateLinkArrow2Element(const iLink& link, MSXML2::IXMLDOMDocumentPtr pDoc);
	MSXML2::IXMLDOMElementPtr CreateLinkArrowElement(const iLink& link, MSXML2::IXMLDOMDocumentPtr pDoc);
	MSXML2::IXMLDOMElementPtr CreateLinkTextElement(const iLink& link, MSXML2::IXMLDOMDocumentPtr pDoc);
	MSXML2::IXMLDOMElementPtr CreateNodeTextElement(const iNode& node, MSXML2::IXMLDOMDocumentPtr pDoc, bool bEmbed);
	MSXML2::IXMLDOMElementPtr CreateLinkElement(const iLink& link, MSXML2::IXMLDOMDocumentPtr pDoc);
	MSXML2::IXMLDOMElementPtr CreateNodeElement(const iNode& node, MSXML2::IXMLDOMDocumentPtr pDoc);
	void RotateArrow(CPoint *pPoint, int size, CPoint &pFrom, CPoint &pTo, CPoint &ptOrg);
	CString CreateNodeStyleAttrib(const iNode& node);
	// リンクラベルの座標計算
	CPoint CalcLinkLabelOrg(const iLink& link);
	CString CreateTextStyle(const LOGFONT& lf, COLORREF fontColor);
};
