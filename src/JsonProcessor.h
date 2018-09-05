#pragma once
#include "iNode.h"
#include "iLink.h"
#include <cpprest/json.h>

using namespace web;

class JsonProcessor
{
	node_vec& nodesImport;
	link_vec& linksImport;
	DWORD& assignKey;
	NodeKeyPairs& idcVec;
	static CString ToLabelAlignString(int align);
	static int FromLabelAlignString(const CString sAlign);
	static CString ToLineStyleString(int nodeLine);
	static int FromLineStyleString(const CString slineStyle);
	static CString ToShapeString(int shape);
	static int FromShapeString(const CString sShape);
	static bool NodePropsContainsKey(const iNodes& nodes, const NodePropsVec& props, DWORD key1, DWORD key2);
	static CString ToLinkStyleString(int linkStyle);
	static int FromLinkStyleString(const CString slinkStyle);
	DWORD FindPairKey(const DWORD first);
	static CString ToColorHexString(COLORREF ref);
	static COLORREF FromColoerHexString(const CString sHex);
	static CString ToLineWidthString(int width);
	static int FromLineWidthString(const CString sWidth);
	static void FontToJson(const LOGFONT& lf, json::value& v);
	static LOGFONT JsonToFont(json::value v);
	bool HasValue(json::value v, json::value::value_type type, CString key1, CString key2 = L"");

public:
	JsonProcessor(node_vec& nodesImport, link_vec& linksImport, DWORD& assignKey, NodeKeyPairs& idcVec);
	~JsonProcessor();
	bool Import(const CString& fileName);
	static bool Save(const CString &outPath, bool bSerialize, iNodes& nodes, iLinks & links, NodePropsVec& ls);
};
