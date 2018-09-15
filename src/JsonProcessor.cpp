#include "stdafx.h"
#include "iEdit.h"
#include "JsonProcessor.h"
#include <locale>
#include "FileUtil.h"

const wchar_t* JsonProcessor::IEDITDOC(L"ieditDoc");
const wchar_t* JsonProcessor::NODES(L"nodes");
const wchar_t* JsonProcessor::LINKS(L"links");
const wchar_t* JsonProcessor::LABEL(L"label");
const wchar_t* JsonProcessor::KEY(L"key");
const wchar_t* JsonProcessor::PARENT(L"parent");
const wchar_t* JsonProcessor::LEVEL(L"level");
const wchar_t* JsonProcessor::N_TEXT(L"text");
const wchar_t* JsonProcessor::ALIGN(L"labelAlign");
const wchar_t* JsonProcessor::SHAPE(L"shape");
const wchar_t* JsonProcessor::BOUND(L"bound");
const wchar_t* JsonProcessor::B_LEFT(L"left");
const wchar_t* JsonProcessor::B_TOP(L"top");
const wchar_t* JsonProcessor::B_RIGHT(L"right");
const wchar_t* JsonProcessor::B_BOTTOM(L"bottom");
const wchar_t* JsonProcessor::LINE_STYLE(L"lineStyle");
const wchar_t* JsonProcessor::FILL_COLOR(L"fillColor");
const wchar_t* JsonProcessor::FILL(L"fill");
const wchar_t* JsonProcessor::LINE_COLOR(L"lineColor");
const wchar_t* JsonProcessor::LINE_WIDTH(L"lineWidth");
const wchar_t* JsonProcessor::FONT_COLOR(L"fontColor");
const wchar_t* JsonProcessor::LINK_STYLE(L"style");
const wchar_t* JsonProcessor::KEY_FROM(L"from");
const wchar_t* JsonProcessor::KEY_TO(L"to");
const wchar_t* JsonProcessor::L_NAME(L"caption");
const wchar_t* JsonProcessor::L_PATH(L"path");
const wchar_t* JsonProcessor::L_VIA_PT(L"viaPoint");
const wchar_t* JsonProcessor::PT_X(L"x");
const wchar_t* JsonProcessor::PT_Y(L"y");
const wchar_t* JsonProcessor::S_CC(L"single-middle-center");
const wchar_t* JsonProcessor::S_CL(L"single-middle-left");
const wchar_t* JsonProcessor::S_CR(L"single-midele-right");
const wchar_t* JsonProcessor::S_TC(L"single-top-center");
const wchar_t* JsonProcessor::S_TL(L"single-top-left");
const wchar_t* JsonProcessor::S_TR(L"single-top-right");
const wchar_t* JsonProcessor::S_BC(L"single-bottom-center");
const wchar_t* JsonProcessor::S_BL(L"single-bottom-left");
const wchar_t* JsonProcessor::S_BR(L"single-bottom-right");
const wchar_t* JsonProcessor::M_C(L"multi-center");
const wchar_t* JsonProcessor::M_L(L"multi-left");
const wchar_t* JsonProcessor::M_R(L"multi-right");
const wchar_t* JsonProcessor::HIDDEN(L"hidden");
const wchar_t* JsonProcessor::SH_RECT(L"rectangle");
const wchar_t* JsonProcessor::SH_R_RECT(L"rounded-rectangle");
const wchar_t* JsonProcessor::SH_ELIPSE(L"elipse");
const wchar_t* JsonProcessor::SH_META(L"metafile");
const wchar_t* JsonProcessor::SH_MM(L"mindmap-node");
const wchar_t* JsonProcessor::LN_NO_DIR(L"no-directional");
const wchar_t* JsonProcessor::LN_UNI_DIR(L"uni-directional");
const wchar_t* JsonProcessor::LN_BI_DIR(L"bi-directional");
const wchar_t* JsonProcessor::LN_DEPEND(L"dependency");
const wchar_t* JsonProcessor::LN_INT_DEPEND(L"inter-dependency");
const wchar_t* JsonProcessor::LN_INHERIT(L"inheritance");
const wchar_t* JsonProcessor::LN_AGGR(L"aggregation");
const wchar_t* JsonProcessor::LN_COMP(L"composition");
const wchar_t* JsonProcessor::LN_NO_G(L"no-graphical");
const wchar_t* JsonProcessor::LS_SOLID(L"solid-line");
const wchar_t* JsonProcessor::LS_DOT(L"doted-line");
const wchar_t* JsonProcessor::LS_NONE(L"no-line");
const wchar_t* JsonProcessor::LW_THIN(L"thin");
const wchar_t* JsonProcessor::LW_MID_THIN(L"middle-thin");
const wchar_t* JsonProcessor::LW_MID_THCK(L"middle-thick");
const wchar_t* JsonProcessor::LW_THCK(L"thick");
const wchar_t* JsonProcessor::LW_VRY_THCK(L"very-thick");
const wchar_t* JsonProcessor::LFONT(L"font");
const wchar_t* JsonProcessor::LF_NAME(L"name");
const wchar_t* JsonProcessor::LF_POINT(L"point");
const wchar_t* JsonProcessor::LF_BOLD(L"bold");
const wchar_t* JsonProcessor::LF_UL(L"underLine");
const wchar_t* JsonProcessor::LF_SO(L"strikeOut");
const wchar_t* JsonProcessor::LF_ITL(L"italic");

JsonProcessor::JsonProcessor(node_vec& nodesImport, link_vec& linksImport, DWORD& assignKey, NodeKeyPairs& idcVec) :
	nodesImport(nodesImport), linksImport(linksImport), assignKey(assignKey), idcVec(idcVec)
{
}

JsonProcessor::~JsonProcessor()
{
}

bool JsonProcessor::Import(const CString &fileName)
{
	FILE* fp;
	if ((fp = FileUtil::OpenStdioFile(fileName)) == NULL) return false;
	CStdioFile f(fp);
	_wsetlocale(LC_ALL, L"jpn");
	CString target, in;
	while (f.ReadString(in)) {
		target += in;
	}
	f.Close();
	_wsetlocale(LC_ALL, L"");
	std::error_code error;
	json::value json = json::value::parse(target.GetBuffer(), error);
	if (error.value() > 0) {
		AfxMessageBox(L"JSON の解析に失敗しました。");
		return false;
	}

	if (json[IEDITDOC].is_null()) {
		AfxMessageBox(L"iEdit で利用可能な JSON 形式ではありません。");
		return false;
	}

	if (!HasValue(json, json::value::Array, IEDITDOC, NODES)) {
		AfxMessageBox(L"インポート対象の要素がありません。");
		return false;
	}
	json::array values = json[IEDITDOC][NODES].as_array();
	json::array::const_iterator it = values.cbegin();
	DWORD keyDefault = 0;
	for (; it != values.cend(); it++) {
		json::value v = *it;

		CString label = HasValue(v, json::value::String, LABEL) ? v[LABEL].as_string().c_str() : L"未設定";
		iNode node(label);
		node.SetKey(++assignKey);

		DWORD key = HasValue(v, json::value::Number, KEY) ? v[KEY].as_integer() : ++keyDefault;
		NodeKeyPair keyPair;
		keyPair.first = key;
		keyPair.second = node.GetKey();
		idcVec.push_back(keyPair);

		DWORD parent = v[PARENT].as_integer();
		node.SetParentKey(parent);

		if (HasValue(v, json::value::Number, LEVEL)) {
			//node.SetLevel(v[LEVEL].as_integer());
		}

		CString text = HasValue(v, json::value::String, N_TEXT) ? v[N_TEXT].as_string().c_str() : L"";
		node.SetText(text);

		CString sAlign = HasValue(v, json::value::String, ALIGN) ? v[ALIGN].as_string().c_str() : S_CC;
		node.SetTextStyle(FromLabelAlignString(sAlign));

		CString sShape = HasValue(v, json::value::String, SHAPE) ? v[SHAPE].as_string().c_str() : SH_RECT;
		node.SetShape(FromShapeString(sShape));

		CString sLineStyle = HasValue(v, json::value::String, LINE_STYLE) ? v[LINE_STYLE].as_string().c_str() : LS_SOLID;
		node.SetLineStyle(FromLineStyleString(sLineStyle));

		CString sFillColor = HasValue(v, json::value::String, FILL_COLOR) ? v[FILL_COLOR].as_string().c_str() : L"#FFFFFF";
		node.SetFillColor(FromColoerHexString(sFillColor));

		CString sLineColor = HasValue(v, json::value::String, LINE_COLOR) ? v[LINE_COLOR].as_string().c_str() : L"#000000";
		node.SetLineColor(FromColoerHexString(sLineColor));

		bool bFill = HasValue(v, json::value::Boolean, FILL) ? v[FILL].as_bool() : true;
		node.ToggleFill(bFill);

		CString sLineWidth = HasValue(v, json::value::String, LINE_WIDTH) ? v[LINE_WIDTH].as_string().c_str() : LW_THIN;
		node.SetLineWidth(FromLineWidthString(sLineWidth));

		CString sFontColor = HasValue(v, json::value::String, FONT_COLOR) ? v[FONT_COLOR].as_string().c_str() : L"#000000";
		node.SetFontColor(FromColoerHexString(sFontColor));

		node.SetFontInfo(JsonToFont(v), false);

		CRect rc = JsonToRect(v);
		node.SetBound(JsonToRect(v));
		if (rc.Height() <= 10 && rc.Width() <= 10) { // Bound がない場合の補正
			bool resizeDisabled = ((CiEditApp*)AfxGetApp())->m_rgsNode.bDisableNodeResize;
			((CiEditApp*)AfxGetApp())->m_rgsNode.bDisableNodeResize = false;
			node.SetName(node.GetName()); // AdjustFont 呼び出し
			((CiEditApp*)AfxGetApp())->m_rgsNode.bDisableNodeResize = resizeDisabled;
		}

		nodesImport.push_back(node);
	}

	if (!HasValue(json, json::value::Array, IEDITDOC, LINKS)) return true; // リンクはなくても正常終了
	json::array linkValues = json[IEDITDOC][LINKS].as_array();
	json::array::const_iterator li = linkValues.cbegin();
	for (; li != linkValues.cend(); li++) {
		json::value v = *li;
		if (!HasValue(v, json::value::Number, KEY_FROM)) continue;

		iLink l;
		DWORD keyFrom = FindPairKey(v[KEY_FROM].as_integer());
		if (keyFrom == -1) continue;
		l.SetFromNodeKey(keyFrom);
		
		CString caption = HasValue(v, json::value::String, L_NAME) ? v[L_NAME].as_string().c_str() : L"";
		l.SetName(caption);

		CString sStyle = HasValue(v, json::value::String, LINK_STYLE) ? v[LINK_STYLE].as_string().c_str() : LN_NO_DIR;
		l.SetArrowStyle(FromLinkStyleString(sStyle));

		if (l.GetArrowStyle() != iLink::other) {
			if (!HasValue(v, json::value::Number, KEY_TO)) continue;
			l.SetToNodeKey(FindPairKey(v[KEY_TO].as_integer()));

			CPoint viaPt = JsonToViaPt(v);
			if (viaPt.x != -1 && viaPt.y != -1) {
				l.SetPathPt(viaPt);
			}

			CString sLineStyle = HasValue(v, json::value::String, LINE_STYLE) ? v[LINE_STYLE].as_string().c_str() : LS_SOLID;
			l.SetLineStyle(FromLineStyleString(sLineStyle));

			CString sLineColor = HasValue(v, json::value::String, LINE_COLOR) ? v[LINE_COLOR].as_string().c_str() : L"#000000";
			l.SetLinkColor(FromColoerHexString(sLineColor));

			CString sLineWidth = HasValue(v, json::value::String, LINE_WIDTH) ? v[LINE_WIDTH].as_string().c_str() : LW_THIN;
			l.SetLineWidth(FromLineWidthString(sLineWidth));

			l.SetFontInfo(JsonToFont(v));
		}
		else {
			l.SetToNodeKey(l.GetFromNodeKey());

			CString path = HasValue(v, json::value::String, L_PATH) ? v[L_PATH].as_string().c_str() : L"";
			l.SetPath(path);
		}
		linksImport.push_back(l);
	}

	return true;
}

DWORD JsonProcessor::FindPairKey(const DWORD first)
{
	for (unsigned int i = 0; i < idcVec.size(); i++) {
		if (idcVec[i].first == first) {
			return idcVec[i].second;
		}
	}
	return -1;
}

bool JsonProcessor::HasValue(json::value v, json::value::value_type type, CString key1, CString key2)
{
	json::value target = v[key1.GetBuffer()];
	if (target.is_null()) return false;

	if (key2 != L"") {
		target = v[key1.GetBuffer()][key2.GetBuffer()];
		if (target.is_null()) {
			return false;
		}
	}
	switch (type) {
	case json::value::String: return target.is_string();
	case json::value::Number: return target.is_number();
	case json::value::Array: return target.is_array();
	case json::value::Boolean: return target.is_boolean();
	}
	return false;
}

bool JsonProcessor::Save(const CString &outPath, bool bSerialize, iNodes& nodes, iLinks & links, NodePropsVec& ls)
{
	std::vector<json::value> nodeValues;

	for (unsigned int i = 0; i < ls.size(); i++) {
		node_c_iter it = nodes.FindRead(ls[i].key);

		DWORD key, parent;
		key = (*it).second.GetKey(); parent = (*it).second.GetParentKey();
		if (i == 0 && key != parent) {
			parent = key;
		}
		json::value v;
		v[KEY] = json::value::number((uint64_t)key);
		v[PARENT] = json::value::number((uint64_t)parent);
		v[LABEL] = json::value::string(ls[i].name.GetBuffer());
		v[LEVEL] = json::value::number((*it).second.GetLevel());
		CString text = (*it).second.GetText();
		v[N_TEXT] = json::value::string(text.GetBuffer());
		v[ALIGN] = json::value::string(ToLabelAlignString((*it).second.GetTextStyle()).GetBuffer());
		v[SHAPE] = json::value::string(ToShapeString((*it).second.GetShape()).GetBuffer());
		CRect r = (*it).second.GetBound();
		v[BOUND][B_LEFT] = json::value::number(r.left);
		v[BOUND][B_TOP] = json::value::number(r.top);
		v[BOUND][B_RIGHT] = json::value::number(r.right);
		v[BOUND][B_BOTTOM] = json::value::number(r.bottom);
		v[LINE_STYLE] = json::value::string(ToLineStyleString((*it).second.GetLineStyle()).GetBuffer());
		CString fillColor = ToColorHexString((*it).second.GetFillColor());
		v[FILL_COLOR] = json::value::string(fillColor.GetBuffer());
		CString lineColor = ToColorHexString((*it).second.GetLineColor());
		v[FILL] = json::value::boolean((*it).second.Filled());
		v[LINE_COLOR] = json::value::string(lineColor.GetBuffer());
		v[LINE_WIDTH] = json::value::string(ToLineWidthString((*it).second.GetLineWidth()).GetBuffer());
		CString fontColor = ToColorHexString((*it).second.GetFontColor());
		v[FONT_COLOR] = json::value::string(fontColor.GetBuffer());
		FontToJson((*it).second.GetFontInfo(), v);
		nodeValues.push_back(v);
	}
	json::value root;
	root[IEDITDOC][NODES] = json::value::array(nodeValues);

	std::vector<json::value> linkValues;
	link_c_iter li = links.cbegin();
	for (; li != links.cend(); li++) {
		if (!NodePropsContainsKey(nodes, ls, (*li).GetFromNodeKey(), (*li).GetToNodeKey())) continue;
		json::value v;
		int style = (*li).GetArrowStyle();
		v[LINK_STYLE] = json::value::string(ToLinkStyleString(style).GetBuffer());
		v[KEY_FROM] = json::value::number((uint64_t)(*li).GetFromNodeKey());
		if (style != iLink::other) {
			v[KEY_TO] = json::value::number((uint64_t)(*li).GetToNodeKey());
		}
		CString caption = (*li).GetName();
		v[L_NAME] = json::value::string(caption.GetBuffer());
		if (style == iLink::other) {
			if ((*li).GetPath() != L"") {
				CString path = (*li).GetPath();
				v[L_PATH] = json::value::string(path.GetBuffer());
			}
		}
		else {
			if ((*li).IsCurved()) {
				CPoint pt = (*li).GetPtPath();
				v[L_VIA_PT][PT_X] = json::value::number(pt.x);
				v[L_VIA_PT][PT_Y] = json::value::number(pt.y);
			}
			v[LINE_STYLE] = json::value::string(ToLineStyleString((*li).GetLineStyle()).GetBuffer());
			CString lineColor = ToColorHexString((*li).GetLinkColor());
			v[LINE_COLOR] = json::value::string(lineColor.GetBuffer());
			v[LINE_WIDTH] = json::value::string(ToLineWidthString((*li).GetLineWidth()).GetBuffer());
			FontToJson((*li).GetFontInfo(), v);
		}
		linkValues.push_back(v);
	}

	root[IEDITDOC][LINKS] = json::value::array(linkValues);
	CString result(root.serialize().c_str());

	FILE* fp;
	if ((fp = FileUtil::CreateStdioFile(outPath)) == NULL) return false;
	CStdioFile f(fp);
	_wsetlocale(LC_ALL, L"jpn");
	f.WriteString(result);
	f.Flush();
	f.Close();
	_wsetlocale(LC_ALL, L"");

	return true;
}

CString JsonProcessor::ToLabelAlignString(int align)
{
	CString result;
	switch (align) {
	case iNode::s_cc: result = S_CC; break;
	case iNode::s_cl: result = S_CL; break;
	case iNode::s_cr: result = S_CR; break;
	case iNode::s_tc: result = S_TC; break;
	case iNode::s_tl: result = S_TL; break;
	case iNode::s_tr: result = S_TR; break;
	case iNode::s_bc: result = S_BC; break;
	case iNode::s_bl: result = S_BL; break;
	case iNode::s_br: result = S_BR; break;
	case iNode::m_c: result = M_C; break;
	case iNode::m_l: result = M_L; break;
	case iNode::m_r: result = M_R; break;
	case iNode::notext: result = HIDDEN; break;
	}
	return result;
}

int JsonProcessor::FromLabelAlignString(const CString sAlign)
{
	if (sAlign == S_CC) {
		return iNode::s_cc;
	}
	else if (sAlign == S_CL) {
		return iNode::s_cl;
	}
	else if (sAlign == S_CR) {
		return iNode::s_cr;
	}
	else if (sAlign == S_TC) {
		return iNode::s_tc;
	}
	else if (sAlign == S_TL) {
		return iNode::s_tl;
	}
	else if (sAlign == S_TR) {
		return iNode::s_tr;
	}
	else if (sAlign == S_BC) {
		return iNode::s_bc;
	}
	else if (sAlign == S_BL) {
		return iNode::s_bl;
	}
	else if (sAlign == S_BR) {
		return iNode::s_br;
	}
	else if (sAlign == M_C) {
		return iNode::m_c;
	}
	else if (sAlign == M_L) {
		return iNode::m_l;
	}
	else if (sAlign == M_R) {
		return iNode::m_r;
	}
	else if (sAlign == HIDDEN) {
		return iNode::notext;
	}
	return iNode::s_cc;
}

CString JsonProcessor::ToShapeString(int shape)
{
	switch (shape) {
	case iNode::rectangle: return SH_RECT;
	case iNode::roundRect: return SH_R_RECT;
	case iNode::arc: return SH_ELIPSE;
	case iNode::MetaFile: return SH_META;
	case iNode::MindMapNode: return SH_MM;
	}
	return SH_RECT;
}

int JsonProcessor::FromShapeString(const CString sShape)
{
	if (sShape == SH_RECT) {
		return iNode::rectangle;
	}
	else if (sShape == SH_R_RECT) {
		return iNode::roundRect;
	}
	else if (sShape == SH_ELIPSE) {
		return iNode::arc;
	}
	else if (sShape == SH_META) {
		return iNode::MetaFile;
	}
	else if (sShape == SH_MM) {
		return iNode::MindMapNode;
	}
	return iNode::rectangle;
}

bool JsonProcessor::NodePropsContainsKey(const iNodes& nodes, const NodePropsVec& props, DWORD key1, DWORD key2) {
	for (unsigned int i = 0; i < props.size(); i++) {
		if (props[i].key == key1 || props[i].key == key2) {
			return true;
		}
	}
	return false;
}

CString JsonProcessor::ToLinkStyleString(int linkStyle)
{
	switch (linkStyle) {
	case iLink::line: return LN_NO_DIR;
	case iLink::arrow: return LN_UNI_DIR;
	case iLink::arrow2: return LN_BI_DIR;
	case iLink::depend: return LN_DEPEND;
	case iLink::depend2: return LN_INT_DEPEND;
	case iLink::inherit: return LN_INHERIT;
	case iLink::aggregat: return LN_AGGR;
	case iLink::composit: return LN_COMP;
	case iLink::other: return LN_NO_G;
	}
	return LN_NO_DIR;
}

int JsonProcessor::FromLinkStyleString(const CString slinkStyle)
{
	if (slinkStyle == LN_NO_DIR) {
		return iLink::line;
	}
	else if (slinkStyle == LN_UNI_DIR) {
		return iLink::arrow;
	}
	else if (slinkStyle == LN_BI_DIR) {
		return iLink::arrow2;
	}
	else if (slinkStyle == LN_DEPEND) {
		return iLink::depend;
	}
	else if (slinkStyle == LN_INT_DEPEND) {
		return iLink::depend2;
	}
	else if (slinkStyle == LN_INHERIT) {
		return iLink::inherit;
	}
	else if (slinkStyle == LN_AGGR) {
		return iLink::aggregat;
	}
	else if (slinkStyle == LN_COMP) {
		return iLink::composit;
	}
	else if (slinkStyle == LN_NO_G) {
		return iLink::other;
	}

	return iLink::line;
}

CString JsonProcessor::ToLineStyleString(int lineStyle)
{
	switch (lineStyle) {
	case PS_SOLID: return LS_SOLID;
	case PS_DOT: return LS_DOT;
	case PS_NULL: return LS_NONE;
	}
	return LS_SOLID;
}

int JsonProcessor::FromLineStyleString(const CString slineStyle)
{
	if (slineStyle == LS_SOLID) {
		return PS_SOLID;
	}
	else if (slineStyle == LS_DOT) {
		return PS_DOT;
	}
	else if (slineStyle == LS_NONE) {
		return PS_NULL;
	}
	return PS_SOLID;
}

CString JsonProcessor::ToColorHexString(COLORREF ref)
{
	DWORD r = GetRValue(ref);
	DWORD g = GetGValue(ref);
	DWORD b = GetBValue(ref);
	CString result;
	result.Format(L"#%02X%02X%02X", r, g, b);
	return result;
}

COLORREF JsonProcessor::FromColoerHexString(const CString sHex)
{
	CString buf(sHex);
	DWORD r, g, b;
	swscanf_s(buf.GetBuffer(), L"#%02X%02X%02X", &r, &g, &b);
	return RGB(r, g, b);
}

CString JsonProcessor::ToLineWidthString(int width)
{
	switch (width) {
	case 0: return LW_THIN;
	case 1: return LW_MID_THIN;
	case 2: return LW_MID_THCK;
	case 3: return LW_THCK;
	case 4: return LW_VRY_THCK;
	}
	return LW_THIN;
}

int JsonProcessor::FromLineWidthString(const CString sWidth)
{
	if (sWidth == LW_THIN) {
		return 0;
	}
	else if (sWidth == LW_MID_THIN) {
		return 1;
	}
	else if (sWidth == LW_MID_THCK) {
		return 2;
	}
	else if (sWidth == LW_THCK) {
		return 3;
	}
	else if (sWidth == LW_VRY_THCK) {
		return 4;
	}
	return 0;
}

CRect JsonProcessor::JsonToRect(json::value v)
{
	CRect r(0, 0, 10, 10);
	SYSTEMTIME st;
	GetSystemTime(&st);
	srand((unsigned)st.wMilliseconds);
	r.MoveToXY(rand() % 800, rand() % 600);

	if (v[BOUND].is_null()) return r;

	if (!HasValue(v, json::value::Number, BOUND, B_LEFT) || !HasValue(v, json::value::Number, BOUND, B_TOP) ||
		!HasValue(v, json::value::Number, BOUND, B_RIGHT) || !HasValue(v, json::value::Number, BOUND, B_BOTTOM)) {
		return r;
	}

	r.left = v[BOUND][B_LEFT].as_integer();
	r.top = v[BOUND][B_TOP].as_integer();
	r.right = v[BOUND][B_RIGHT].as_integer();
	r.bottom = v[BOUND][B_BOTTOM].as_integer();

	return r;
}

CPoint JsonProcessor::JsonToViaPt(json::value v)
{
	CPoint pt(-1, -1);
	if (!HasValue(v, json::value::Number, L_VIA_PT, PT_X) || !HasValue(v, json::value::Number, L_VIA_PT, PT_Y)) {
		return pt;
	}
	pt.x = v[L_VIA_PT][PT_X].as_integer();
	pt.y = v[L_VIA_PT][PT_Y].as_integer();
	return pt;
}

void JsonProcessor::FontToJson(const LOGFONT& lf, json::value& v)
{
	v[LFONT][LF_NAME] = json::value::string(lf.lfFaceName);
	v[LFONT][LF_POINT] = json::value::number(MulDiv(-lf.lfHeight, 72, 96));
	v[LFONT][LF_BOLD] = json::value::boolean(lf.lfWeight >= 600);
	v[LFONT][LF_UL] = json::value::boolean(lf.lfUnderline);
	v[LFONT][LF_SO] = json::value::boolean(lf.lfStrikeOut);
	v[LFONT][LF_ITL] = json::value::boolean(lf.lfItalic);
}

LOGFONT JsonProcessor::JsonToFont(json::value v)
{
	LOGFONT lf;
	lstrcpy(lf.lfFaceName, L"メイリオ");
	lf.lfHeight = -15;
	lf.lfWeight = 400;
	lf.lfUnderline = false;
	lf.lfStrikeOut = false;
	lf.lfItalic = false;
	lf.lfCharSet = SHIFTJIS_CHARSET;

	if (v[LFONT].is_null()) return lf;

	if (HasValue(v, json::value::String, LFONT, LF_NAME)) {
		lstrcpy(lf.lfFaceName, v[LFONT][LF_NAME].as_string().c_str());
	}
	if (HasValue(v, json::value::Number, LFONT, LF_POINT)) {
		int point = v[LFONT][LF_POINT].as_integer();
		lf.lfHeight = -MulDiv(point, 96, 72);
	}
	if (HasValue(v, json::value::Boolean, LFONT, LF_BOLD)) {
		bool bold = v[LFONT][LF_BOLD].as_bool();
		lf.lfWeight = bold ? 700 : 400;
	}
	if (HasValue(v, json::value::Boolean, LFONT, LF_UL)) {
		lf.lfUnderline = v[LFONT][LF_UL].as_bool();
	}
	if (HasValue(v, json::value::Boolean, LFONT, LF_SO)) {
		lf.lfStrikeOut = v[LFONT][LF_SO].as_bool();
	}
	if (HasValue(v, json::value::Boolean, LFONT, LF_ITL)) {
		lf.lfItalic = v[LFONT][LF_ITL].as_bool();
	}
	return lf;
}
