#include "stdafx.h"
#include "Utilities.h"
#include <shlwapi.h>

Utilities::Utilities(void)
{
}

Utilities::~Utilities(void)
{
}

CString Utilities::getSafeFileName(const CString& str)
{
	CString rs;
	for (int i = 0; i < str.GetLength(); i++) {
		TCHAR a = str.GetAt(i);
		if ((a == '|' || a == '\\') && i > 0) {
			if (_ismbblead(str.GetAt(i-1))) {
				rs += _T(a);
				continue;
			}
		}
		if (a == '\n' || a == '\r') continue;
		UINT type = ::PathGetCharType(a);
		if (type == GCT_INVALID || type & GCT_WILD || type & GCT_SEPARATOR) continue;
		rs += _T(a);
	}
	return rs;
}
