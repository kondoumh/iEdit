#pragma once
#include <map>

using namespace std;

typedef map<int, CString> Dictionary;

class SystemConfiguration
{
public:
	SystemConfiguration(void);
	~SystemConfiguration(void);
	static bool IsMeiryoAvailable();
	static bool IsMeiryoUiAvailable();
	static const CSize GetMetafileSize();
	static bool WndTransparencyAvailable();
};
