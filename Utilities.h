#pragma once

class Utilities
{
public:
	Utilities(void);
	~Utilities(void);
	static CString getSafeFileName(const CString& str);
	static CString removeCR(const CString &str);
	static vector<CString> getLines(const CString& text);
	static bool isDependChar(int nByte);
	static CString removeDependChar(LPCTSTR moji);
};
