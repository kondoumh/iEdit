#pragma once
#include "iNode.h"
#include "iLink.h"

#import "msxml6.dll" named_guids

class XmlProcessor
{
	node_vec& nodesImport;
	link_vec& linksImport;
	DWORD& assignKey;
	NodeKeyPairs& idcVec;

	void ComvertToImportData(MSXML2::IXMLDOMElement* node);
	bool ConvertToSerializeData(MSXML2::IXMLDOMElement* node);

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

	template <class T>
	static void OutputStyleSheetLine(T &f);
	static bool NodePropsContainsKey(const iNodes& nodes, const NodePropsVec& labels, DWORD key);

public:
	XmlProcessor(node_vec& nodesImport, link_vec& linksImport, DWORD& assignKey, NodeKeyPairs& idcVec);
	~XmlProcessor();

	bool Import(const CString& fileName);
	bool SerializeFromXml(const CString& filename);
	static bool Save(const CString &outPath, bool bSerialize, iNodes& nodes, iLinks & links, NodePropsVec& ls);
};

