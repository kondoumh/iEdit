#include "stdafx.h"
#include "JsonProcessor.h"
#include <cpprest/json.h>
#include <locale>
#include "FileUtil.h"

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
	web::json::value json = web::json::value::parse(target.GetBuffer(), error);
	if (error.value() > 0) {
		AfxMessageBox(_T("JSON �̉�͂Ɏ��s���܂����B"));
		return false;
	}

	if (json[L"ieditDoc"].is_null()) {
		AfxMessageBox(_T("iEdit �ŗ��p�\�� JSON �`���ł͂���܂���B"));
		return false;
	}

	web::json::array nodes = json[L"ieditDoc"][L"nodes"].as_array();
	web::json::array::const_iterator it = nodes.cbegin();
	for (; it != nodes.cend(); it++) {
		web::json::value node = *it;
		DWORD key = node[L"key"].as_integer();
		DWORD parent = node[L"parent"].as_integer();
		CString name(node[L"name"].as_string().c_str());
		int level = node[L"level"].as_integer();
	}
	return true;
}

bool JsonProcessor::Save(const CString &outPath, bool bSerialize, iNodes& nodes, iLinks & links, NodePropsVec& ls)
{
	std::vector<web::json::value> values;

	for (unsigned int i = 0; i < ls.size(); i++) {
		node_c_iter it = nodes.FindRead(ls[i].key);

		DWORD key, parent;
		key = (*it).second.GetKey(); parent = (*it).second.GetParentKey();
		if (i == 0 && key != parent) {
			parent = key;
		}
		web::json::value v;
		v[L"key"] = web::json::value::number((uint64_t)key);
		v[L"parent"] = web::json::value::number((uint64_t)parent);
		v[L"name"] = web::json::value::string(ls[i].name.GetBuffer());
		v[L"level"] = web::json::value::number((*it).second.GetLevel());
		values.push_back(v);
	}
	web::json::value root;
	root[L"ieditDoc"][L"nodes"] = web::json::value::array(values);
	root[L"ieditDoc"][L"links"] = web::json::value::array();
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