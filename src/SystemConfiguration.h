#pragma once
#include <map>

using namespace std;

typedef map<int, CString> Dictionary;

class SystemConfiguration
{
public:
	SystemConfiguration(void);
	~SystemConfiguration(void);
	bool IsMeiryoAvailable() const;
	bool IsMeiryoUiAvailable() const;
	const CSize GetMetafileSize() const;
	bool WndTransparencyAvailable() const;
};
