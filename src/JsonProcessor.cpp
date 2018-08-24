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
	_wsetlocale(LC_ALL, _T("jpn"));
	CString target, in;
	while (f.ReadString(in)) {
		target += in;
	}
	f.Close();
	_wsetlocale(LC_ALL, _T(""));
	std::error_code error;
	json::value json = json::value::parse(target.GetBuffer(), error);
	if (error.value() > 0) {
		AfxMessageBox(_T("JSON の解析に失敗しました。"));
		return false;
	}

	if (json[L"ieditDoc"].is_null()) {
		AfxMessageBox(_T("iEdit で利用可能な JSON 形式ではありません。"));
		return false;
	}

	json::array values = json[L"ieditDoc"][L"nodes"].as_array();
	json::array::const_iterator it = values.cbegin();
	for (; it != values.cend(); it++) {
		json::value v = *it;
		DWORD key = v[L"key"].as_integer();
		DWORD parent = v[L"parent"].as_integer();
		CString name(v[L"name"].as_string().c_str());
		int level = v[L"level"].as_integer();
		CString s; s.Format(_T("%d %d %s %d\n"), key, parent, name, level);
		OutputDebugString(s);
	}
	return true;
}

bool JsonProcessor::Save(const CString &outPath, bool bSerialize, iNodes& nodes, iLinks & links, NodePropsVec& ls)
{
	std::vector<json::value> values;

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
		values.push_back(v);
	}
	json::value root;
	root[L"ieditDoc"][L"nodes"] = json::value::array(values);
	root[L"ieditDoc"][L"links"] = json::value::array();
	CString result(root.serialize().c_str());

	FILE* fp;
	if ((fp = FileUtil::CreateStdioFile(outPath)) == NULL) return false;
	CStdioFile f(fp);
	_wsetlocale(LC_ALL, _T("jpn"));
	f.WriteString(result);
	f.Flush();
	f.Close();
	_wsetlocale(LC_ALL, _T(""));

	return true;
}
