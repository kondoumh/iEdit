#pragma once

class StringUtil
{
public:
	StringUtil(void);
	~StringUtil(void);
	static CString GetSafeFileName(const CString& target);
	static CString RemoveCr(const CString &target);
	static std::vector<CString> GetLines(const CString& text);
	static bool IsMachineDependentChar(int nByte);
	static CString RemoveMachineDependentChar(LPCTSTR target);
	static CString ReplaceLfToCrlf(const CString& target);
	static CString ReplaceCrToLf(const CString& target);
	static bool IsUrl(const CString& target);
	static CString TrimLeft(const CString& target);
	static int GetIndent(const CString& target);
	static CString EscapeHtmlSpecialChar(const CString& target);
	static CString EscapeHtmlSpecialCharPre(const CString& target);
};
