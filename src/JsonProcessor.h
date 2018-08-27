#pragma once
#include "iNode.h"
#include "iLink.h"

class JsonProcessor
{
	node_vec& nodesImport;
	link_vec& linksImport;
	DWORD& assignKey;
	NodeKeyPairs& idcVec;
	static CString ToLabelAlignString(int align);
	static int FromLabelAlignString(const CString sAlign);
	static CString ToShapeString(int shape);
	static int FromShapeString(const CString sShape);

public:
	JsonProcessor(node_vec& nodesImport, link_vec& linksImport, DWORD& assignKey, NodeKeyPairs& idcVec);
	~JsonProcessor();
	bool Import(const CString& fileName);
	static bool Save(const CString &outPath, bool bSerialize, iNodes& nodes, iLinks & links, NodePropsVec& ls);
};

