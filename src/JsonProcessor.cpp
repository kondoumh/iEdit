#include "stdafx.h"
#include "JsonProcessor.h"
#include <cpprest/json.h>
#include <locale>
#include "FileUtil.h"

using namespace web;

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

	if (json[L"ieditDoc"].is_null()) {
		AfxMessageBox(L"iEdit で利用可能な JSON 形式ではありません。");
		return false;
	}

	json::array values = json[L"ieditDoc"][L"nodes"].as_array();
	json::array::const_iterator it = values.cbegin();
	for (; it != values.cend(); it++) {
		json::value v = *it;
		CString name(v[L"name"].as_string().c_str());
		iNode node(name);node.SetKey(++assignKey);

		DWORD key = v[L"key"].as_integer();
		NodeKeyPair keyPair;
		keyPair.first = key;
		keyPair.second = node.GetKey();
		idcVec.push_back(keyPair);

		DWORD parent = v[L"parent"].as_integer();
		node.SetParentKey(parent);

		int level = v[L"level"].as_integer();
		CString text(v[L"text"].as_string().c_str());
		node.SetText(text);

		int align = FromLabelAlignString(v[L"labelAlign"].as_string().c_str());
		node.SetTextStyle(align);

		int shape = FromShapeString(v[L"shape"].as_string().c_str());
		node.SetShape(shape);

		CRect bound;
		json::array arr = v[L"bound"].as_array();
		bound.left = arr[0].as_integer();
		bound.top = arr[1].as_integer();
		bound.right = arr[2].as_integer();
		bound.bottom = arr[3].as_integer();
		node.SetBound(bound);
		nodesImport.push_back(node);
	}

	json::array linkValues = json[L"ieditDoc"][L"links"].as_array();
	json::array::const_iterator li = values.cbegin();
	for (; li != linkValues.cend(); li++) {
	}

	return true;
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
		v[L"key"] = json::value::number((uint64_t)key);
		v[L"parent"] = json::value::number((uint64_t)parent);
		v[L"name"] = json::value::string(ls[i].name.GetBuffer());
		v[L"level"] = json::value::number((*it).second.GetLevel());
		CString text = (*it).second.GetText();
		v[L"text"] = json::value::string(text.GetBuffer());
		v[L"labelAlign"] = json::value::string(ToLabelAlignString((*it).second.GetTextStyle()).GetBuffer());
		v[L"shape"] = json::value::string(ToShapeString((*it).second.GetShape()).GetBuffer());
		CRect r = (*it).second.GetBound();
		std::vector<json::value> vec;
		vec.push_back(r.left);
		vec.push_back(r.top);
		vec.push_back(r.right);
		vec.push_back(r.bottom);
		v[L"bound"] = json::value::array(vec);
		nodeValues.push_back(v);
	}
	json::value root;
	root[L"ieditDoc"][L"nodes"] = json::value::array(nodeValues);

	std::vector<json::value> linkValues;
	link_c_iter li = links.cbegin();
	for (; li != links.cend(); li++) {
		if (!NodePropsContainsKey(nodes, ls, (*li).GetFromNodeKey(), (*li).GetToNodeKey())) continue;
		json::value v;
		v[L"from"] = json::value::number((uint64_t)(*li).GetFromNodeKey());
		v[L"to"] = json::value::number((uint64_t)(*li).GetToNodeKey());
		CString caption = (*li).GetName();
		v[L"caption"] = json::value::string(caption.GetBuffer());
		int style = (*li).GetArrowStyle();
		v[L"style"] = json::value::string(ToLinkStyleString(style).GetBuffer());
		if (style == iLink::other && (*li).GetPath() != L"") {
			CString path = (*li).GetPath();
			v[L"location"] == json::value::string(path.GetBuffer());
		}
		linkValues.push_back(v);
	}

	root[L"ieditDoc"][L"links"] = json::value::array(linkValues);
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
	case iNode::s_cc: result = L"single-middle-center"; break;
	case iNode::s_cl: result = L"single-middle-left"; break;
	case iNode::s_cr: result = L"single-midele-right"; break;
	case iNode::s_tc: result = L"single-top-center"; break;
	case iNode::s_tl: result = L"single-top-left"; break;
	case iNode::s_tr: result = L"single-top-right"; break;
	case iNode::s_bc: result = L"single-bottom-center"; break;
	case iNode::s_bl: result = L"single-bottom-left"; break;
	case iNode::s_br: result = L"single-bottom-right"; break;
	case iNode::m_c: result = L"multi-center"; break;
	case iNode::m_l: result = L"multi-left"; break;
	case iNode::m_r: result = L"multi-right"; break;
	case iNode::notext: result = L"hidden"; break;
	}
	return result;
}

int JsonProcessor::FromLabelAlignString(const CString sAlign)
{
	if (sAlign == L"single-middle-center") {
		return iNode::s_cc;
	}
	else if (sAlign == L"single-middle-left") {
		return iNode::s_cl;
	}
	else if (sAlign == L"single-midele-right") {
		return iNode::s_cr;
	}
	else if (sAlign == L"single-top-center") {
		return iNode::s_tc;
	}
	else if (sAlign == L"single-top-left") {
		return iNode::s_tl;
	}
	else if (sAlign == L"single-top-right") {
		return iNode::s_tr;
	}
	else if (sAlign == L"single-bottom-center") {
		return iNode::s_bc;
	}
	else if (sAlign == L"single-bottom-left") {
		return iNode::s_bl;
	}
	else if (sAlign == L"single-bottom-right") {
		return iNode::s_br;
	}
	else if (sAlign == L"multi-center") {
		return iNode::m_c;
	}
	else if (sAlign == L"multi-left") {
		return iNode::m_l;
	}
	else if (sAlign == L"multi-right") {
		return iNode::m_r;
	}
	else if (sAlign == L"hidden") {
		return iNode::notext;
	}
	return iNode::s_cc;
}

CString JsonProcessor::ToShapeString(int shape)
{
	switch (shape) {
	case iNode::rectangle: return L"rectangle";
	case iNode::roundRect: return L"rounded-rectangle";
	case iNode::arc: return L"elipse";
	case iNode::MetaFile: return L"metafile";
	case iNode::MindMapNode: return L"mindmap-node";
	}
	return L"rectangle";
}

int JsonProcessor::FromShapeString(const CString sShape)
{
	if (sShape == L"rectangle") {
		return iNode::rectangle;
	}
	else if (sShape == L"rounded-rectangle") {
		return iNode::roundRect;
	}
	else if (sShape == L"elipse") {
		return iNode::arc;
	}
	else if (sShape == L"metafile") {
		return iNode::MetaFile;
	}
	else if (sShape == L"mindmap-node") {
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
	case iLink::line: return L"no-directional";
	case iLink::arrow: return L"uni-directional";
	case iLink::arrow2: return L"bi-directional";
	case iLink::depend: return L"dependency";
	case iLink::depend2: return L"inter-dependency";
	case iLink::inherit: return L"inheritance";
	case iLink::aggregat: return L"aggregation";
	case iLink::composit: return L"composition";
	case iLink::other: return L"no-graphical";
	}
	return L"";
}

int JsonProcessor::FromLinkStyleString(const CString slinkStyle)
{
	return 0;
}