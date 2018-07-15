#pragma once

class StringUtil
{
public:
	StringUtil(void);
	~StringUtil(void);
	static CString GetSafeFileName(const CString& str);
	static CString RemoveCr(const CString &str);
	static vector<CString> GetLines(const CString& text);
	static bool IsMachineDependentChar(int nByte);
	static CString RemoveMachineDependentChar(LPCTSTR moji);
	static CString ReplaceLfToCrlf(const CString& str);
	static CString ReplaceCrToLf(const CString& str);
	static bool IsUrl(const CString& str);
	static CString TrimLeft(const CString& string);
	static int GetIndent(const CString& string);
};
