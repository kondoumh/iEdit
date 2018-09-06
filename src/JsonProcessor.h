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

	static const wchar_t* IEDITDOC;
	static const wchar_t* NODES;
	static const wchar_t* LINKS;
	static const wchar_t* NAME;
	static const wchar_t* KEY;
	static const wchar_t* PARENT;
	static const wchar_t* LEVEL;
	static const wchar_t* N_TEXT;
	static const wchar_t* ALIGN;
	static const wchar_t* SHAPE;
	static const wchar_t* BOUND;
	static const wchar_t* LINE_STYLE;
	static const wchar_t* FILL_COLOR;
	static const wchar_t* FILL;
	static const wchar_t* LINE_COLOR;
	static const wchar_t* LINE_WIDTH;
	static const wchar_t* FONT_COLOR;

public:
	JsonProcessor(node_vec& nodesImport, link_vec& linksImport, DWORD& assignKey, NodeKeyPairs& idcVec);
	~JsonProcessor();
	bool Import(const CString& fileName);
	static bool Save(const CString &outPath, bool bSerialize, iNodes& nodes, iLinks & links, NodePropsVec& ls);
};
