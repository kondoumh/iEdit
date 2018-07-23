#pragma once
#include <map>

using namespace std;

typedef map<int, CString> Dictionary;

class SystemConfiguration
{
public:
	SystemConfiguration(void);
	~SystemConfiguration(void);
	static bool MeiryoAvailable();
	static bool MeiryoUiAvailable();
	static const CSize GetMetafileSize();
	static bool WndTransparencyAvailable();
};
