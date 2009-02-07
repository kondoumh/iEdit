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
	void exportSVG(const CString& path, const CPoint& maxPt);
	SvgWriter(iNodes& nodes, iLinks& links, bool bDrwAll=false);
	virtual ~SvgWriter();

private:
	iNodes& m_nodes;
	iLinks& m_links;
	bool m_bDrwAll;
protected:
	MSXML2::IXMLDOMElementPtr createLinkArrow2Element(const iLink& link, MSXML2::IXMLDOMDocumentPtr pDoc);
	MSXML2::IXMLDOMElementPtr createLinkArrowElement(const iLink& link, MSXML2::IXMLDOMDocumentPtr pDoc);
	MSXML2::IXMLDOMElementPtr createLinkTextElement(const iLink& link, MSXML2::IXMLDOMDocumentPtr pDoc);
	MSXML2::IXMLDOMElementPtr createNodeTextElement(const iNode& node, MSXML2::IXMLDOMDocumentPtr pDoc);
	MSXML2::IXMLDOMElementPtr createLinkElement(const iLink& link, MSXML2::IXMLDOMDocumentPtr pDoc);
	MSXML2::IXMLDOMElementPtr createNodeElement(const iNode& node, MSXML2::IXMLDOMDocumentPtr pDoc);
	void rotateArrow(CPoint *pPoint, int size, CPoint &pFrom, CPoint &pTo, CPoint &ptOrg);
	CString createNodeStyleAtrb(const iNode& node);
	// ラベルの配置計算用
	CPoint calcNodeLabelOrg(const iNode& node);
	// リンクラベルの座標計算
	CPoint calcLinkLabelOrg(const iLink& link);
	CString createTextStyle(const LOGFONT& lf, COLORREF fontColor);
};

#endif // !defined(AFX_SVGWRITER_H__4589807F_F92A_4D52_B27B_2B685172227C__INCLUDED_)
