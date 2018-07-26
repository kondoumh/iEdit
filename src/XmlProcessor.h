#pragma once
#include "iNode.h"
#include "iLink.h"

class XmlProcessor
{
public:
	XmlProcessor();
	~XmlProcessor();

	static bool ValidateXmlFile(const CString& fileName);
	static bool Import(node_vec& nodesImport, link_vec& linksImport);

	//CPoint Dom2LinkPathPt(MSXML2::IXMLDOMNode* pNode);
	//COLORREF Dom2LinkColor(MSXML2::IXMLDOMNode* pNode);
	//void Dom2LinkStyle(MSXML2::IXMLDOMNode* pNode, int& style, int& width, int& arrow);
	//COLORREF Dom2NodeLineColor(MSXML2::IXMLDOMNode* pNode);
	//void Dom2NodeLine(MSXML2::IXMLDOMNode* pNode, int& style, int& width);
	//COLORREF Dom2ForeColor(MSXML2::IXMLDOMNode* pNode);
	//void Dom2Bound(MSXML2::IXMLDOMNode* pNode, CRect& rc);
	//int Dom2Shape(const CString& tag);
	//int Dom2TextAlign(const CString& tag);
	//bool Dom2Nodes2(MSXML2::IXMLDOMElement* node, CStdioFile* f);
	//bool Dom2Nodes3(MSXML2::IXMLDOMElement* node);
};

