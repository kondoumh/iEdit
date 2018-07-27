#pragma once
#include "iNode.h"
#include "iLink.h"

class XmlProcessor
{
	node_vec& nodesImport;
	link_vec& linksImport;
	DWORD& assignKey;
	NodeKeyPairs& idcVec;
	void ComvertToImportData(MSXML2::IXMLDOMElement* node);
	bool Dom2Nodes3(MSXML2::IXMLDOMElement* node);

public:
	XmlProcessor(node_vec& nodesImport, link_vec& linksImport, DWORD& assignKey, NodeKeyPairs& idcVec);
	~XmlProcessor();

	bool ImportXml(const CString& fileName);
	bool SerializeFromXml(const CString& filename);

	CPoint Dom2LinkPathPt(MSXML2::IXMLDOMNode* pNode);
	COLORREF Dom2LinkColor(MSXML2::IXMLDOMNode* pNode);
	void Dom2LinkStyle(MSXML2::IXMLDOMNode* pNode, int& style, int& width, int& arrow);
	COLORREF Dom2NodeLineColor(MSXML2::IXMLDOMNode* pNode);
	void Dom2NodeLine(MSXML2::IXMLDOMNode* pNode, int& style, int& width);
	COLORREF Dom2ForeColor(MSXML2::IXMLDOMNode* pNode);
	void Dom2Bound(MSXML2::IXMLDOMNode* pNode, CRect& rc);
	int Dom2Shape(const CString& tag);
	int Dom2TextAlign(const CString& tag);

	DWORD FindPairKey(const DWORD first);

	//bool Dom2Nodes3(MSXML2::IXMLDOMElement* node);
};

