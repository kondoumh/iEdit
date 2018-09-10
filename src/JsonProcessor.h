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
	static CRect JsonToRect(json::value v);
	static void FontToJson(const LOGFONT& lf, json::value& v);
	static LOGFONT JsonToFont(json::value v);
	static bool HasValue(json::value v, json::value::value_type type, CString key1, CString key2 = L"");

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
	static const wchar_t* B_LEFT;
	static const wchar_t* B_TOP;
	static const wchar_t* B_RIGHT;
	static const wchar_t* B_BOTTOM;
	static const wchar_t* LINE_STYLE;
	static const wchar_t* FILL_COLOR;
	static const wchar_t* FILL;
	static const wchar_t* LINE_COLOR;
	static const wchar_t* LINE_WIDTH;
	static const wchar_t* FONT_COLOR;
	static const wchar_t* LINK_STYLE;
	static const wchar_t* KEY_FROM;
	static const wchar_t* KEY_TO;
	static const wchar_t* L_NAME;
	static const wchar_t* L_PATH;
	static const wchar_t* L_VIA_PT;
	static const wchar_t* PT_X;
	static const wchar_t* PT_Y;
	static const wchar_t* S_CC;
	static const wchar_t* S_CL;
	static const wchar_t* S_CR;
	static const wchar_t* S_TC;
	static const wchar_t* S_TL;
	static const wchar_t* S_TR;
	static const wchar_t* S_BC;
	static const wchar_t* S_BL;
	static const wchar_t* S_BR;
	static const wchar_t* M_C;
	static const wchar_t* M_L;
	static const wchar_t* M_R;
	static const wchar_t* HIDDEN;
	static const wchar_t* SH_RECT;
	static const wchar_t* SH_R_RECT;
	static const wchar_t* SH_ELIPSE;
	static const wchar_t* SH_META;
	static const wchar_t* SH_MM;
	static const wchar_t* LN_NO_DIR;
	static const wchar_t* LN_UNI_DIR;
	static const wchar_t* LN_BI_DIR;
	static const wchar_t* LN_DEPEND;
	static const wchar_t* LN_INT_DEPEND;
	static const wchar_t* LN_INHERIT;
	static const wchar_t* LN_AGGR;
	static const wchar_t* LN_COMP;
	static const wchar_t* LN_NO_G;
	static const wchar_t* LS_SOLID;
	static const wchar_t* LS_DOT;
	static const wchar_t* LS_NONE;
	static const wchar_t* LW_THIN;
	static const wchar_t* LW_MID_THIN;
	static const wchar_t* LW_MID_THCK;
	static const wchar_t* LW_THCK;
	static const wchar_t* LW_VRY_THCK;
	static const wchar_t* LFONT;
	static const wchar_t* LF_NAME;
	static const wchar_t* LF_POINT;
	static const wchar_t* LF_BOLD;
	static const wchar_t* LF_UL;
	static const wchar_t* LF_SO;
	static const wchar_t* LF_ITL;

public:
	JsonProcessor(node_vec& nodesImport, link_vec& linksImport, DWORD& assignKey, NodeKeyPairs& idcVec);
	~JsonProcessor();
	bool Import(const CString& fileName);
	static bool Save(const CString &outPath, bool bSerialize, iNodes& nodes, iLinks & links, NodePropsVec& ls);
};
