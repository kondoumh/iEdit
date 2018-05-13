#pragma once

class StringUtil
{
public:
	StringUtil(void);
	~StringUtil(void);
	static CString getSafeFileName(const CString& str);
	static CString removeCR(const CString &str);
	static vector<CString> getLines(const CString& text);
	static bool isDependChar(int nByte);
	static CString removeDependChar(LPCTSTR moji);
};
