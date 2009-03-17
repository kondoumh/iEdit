#pragma once

class Utilities
{
public:
	Utilities(void);
	~Utilities(void);
	static CString getSafeFileName(const CString& str);
	static CString removeCR(const CString &str);
};
