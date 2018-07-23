#include "StdAfx.h"
#include "SystemConfiguration.h"
#include <VersionHelpers.h>

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);

SystemConfiguration::SystemConfiguration(void)
{
}

SystemConfiguration::~SystemConfiguration(void)
{
}

bool SystemConfiguration::MeiryoAvailable()
{
	return IsWindowsVistaOrGreater();
}

bool SystemConfiguration::MeiryoUiAvailable()
{
	return IsWindows7OrGreater();
}

const CSize SystemConfiguration::GetMetafileSize()
{
	int width = 353;
	int height = 353;
	
	if (!IsWindowsXPOrGreater()) {
		width = 253;
		height = 238;
	}
	else if (IsWindowsXPOrGreater() && !IsWindowsVistaOrGreater()) {
		width = 260;
		height = 255;
	}
	return CSize(width, height);
}

bool SystemConfiguration::WndTransparencyAvailable()
{
	return IsWindowsXPOrGreater();
}