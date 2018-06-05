#include "StdAfx.h"
#include "SystemConfiguration.h"

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);

/// 環境チェックを行います。
SystemConfiguration::SystemConfiguration(void)
{
	m_OsVersion = SystemConfiguration::Unknown;
	m_OsProduct = SystemConfiguration::UnknownProduct;
	m_OsProssesorEdition = SystemConfiguration::x32;

	CreateNameDictionaries();
	CheckEnvironment();
}

SystemConfiguration::~SystemConfiguration(void)
{
}

/// OSのバージョンやプロダクト情報を取得します。
void SystemConfiguration::CheckEnvironment()
{
	CString osVersionName;
	OSVERSIONINFO  osVerInfo;
	ZeroMemory(&osVerInfo, sizeof(OSVERSIONINFO));
	osVerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!GetVersionEx((OSVERSIONINFO*)&osVerInfo))
	{
		return;
	}

	if (osVerInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) // 9x系OS
	{
		if (osVerInfo.dwMajorVersion == 4)
		{
			switch (osVerInfo.dwMinorVersion)
			{
			case 0:
				m_OsVersion = SystemConfiguration::Windows95;
				break;
			case 10:
				m_OsVersion = SystemConfiguration::Windows98;
				break;
			case 90:
				m_OsVersion = SystemConfiguration::WindowsMe;
				break;
			}
		}
		return;
	}

	if (osVerInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) // NT系OS
	{
		if (osVerInfo.dwMajorVersion < 5)
		{
			switch (osVerInfo.dwMajorVersion)
			{
			case 3:
				m_OsVersion = SystemConfiguration::WindowsNT35;
				break;
			case 4:
				m_OsVersion = SystemConfiguration::WindowsNT40;
				break;
			}
		}
		if (osVerInfo.dwMajorVersion >= 5)
		{
			OSVERSIONINFOEX osVerInfoEx;
			osVerInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
			ZeroMemory(&osVerInfoEx, sizeof(OSVERSIONINFOEX));
			osVerInfoEx.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
			GetVersionEx((LPOSVERSIONINFO)&osVerInfoEx);
			if (!GetVersionEx((LPOSVERSIONINFO)&osVerInfoEx))
			{
				m_OsVersion = SystemConfiguration::UnKnown2000;
				return;
			}
			if (osVerInfoEx.dwMajorVersion == 5)
			{
				switch (osVerInfoEx.dwMinorVersion)
				{
				case 0:
					m_OsVersion = SystemConfiguration::Windows2000;
					if (osVerInfoEx.wProductType == VER_NT_WORKSTATION) {
						m_OsProduct = SystemConfiguration::Professional;
					}
					else {
						m_OsProduct = SystemConfiguration::Server;
					}
					break;
				case 1:
					m_OsVersion = SystemConfiguration::WindowsXP;
					if (osVerInfoEx.wProductType == VER_NT_WORKSTATION) {
						m_OsProduct = SystemConfiguration::Professional;
					}
					else {
						if (osVerInfoEx.wSuiteMask == VER_SUITE_PERSONAL)
						{
							m_OsProduct = SystemConfiguration::Home;
						}
					}
					break;
				case 2:
					m_OsVersion = SystemConfiguration::Windows2003;
					m_OsProduct = SystemConfiguration::Server;
					PGNSI pGNSI = (PGNSI)GetProcAddress(GetModuleHandle(_T("kernel32.dll")), "GetNativeSystemInfo");
					if (pGNSI != NULL) {
						SYSTEM_INFO si;
						pGNSI(&si);
						if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
							si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
						{
							m_OsVersion = SystemConfiguration::WindowsXP;
							m_OsProssesorEdition = SystemConfiguration::x64;
							m_OsProduct = SystemConfiguration::Professional;
						}
					}
					break;
				}
				if (osVerInfoEx.szCSDVersion != _T("")) {
					m_ServicePackName = CString(osVerInfoEx.szCSDVersion);
				}
			}
			else if (osVerInfoEx.dwMajorVersion == 6)
			{
				if (osVerInfoEx.dwMinorVersion == 0) {
					if (osVerInfoEx.wProductType == VER_NT_WORKSTATION) {
						m_OsVersion = SystemConfiguration::WindowsVista;
					}
					else {
						m_OsVersion = SystemConfiguration::Windows2008;
					}
				}
				else if (osVerInfoEx.dwMinorVersion == 1) {
					if (osVerInfoEx.wProductType == VER_NT_WORKSTATION) {
						m_OsVersion = SystemConfiguration::Windows7;
					}
					else {
						m_OsVersion = SystemConfiguration::Windows2008;
					}
				}
				else if (osVerInfoEx.dwMinorVersion == 2) {
					m_OsVersion = SystemConfiguration::Windows8;
				}
				else if (osVerInfoEx.dwMinorVersion == 3) {
					m_OsVersion = SystemConfiguration::Windows8_1;
				}
				if (osVerInfoEx.szCSDVersion != _T("")) {
					m_ServicePackName = CString(osVerInfoEx.szCSDVersion);
				}
			}
			else if (osVerInfoEx.dwMajorVersion == 10) {

				m_OsVersion = SystemConfiguration::Windows10;
			}
			CString m; m.Format(_T("%d + %d : %s\n"), osVerInfoEx.dwMajorVersion, osVerInfoEx.dwMinorVersion, m_VersionNames[m_OsVersion]);
			OutputDebugString(m);
		}
	}
}

/// OSのバージョン名用ディクショナリーを作成するヘルパーメソッドです。
void SystemConfiguration::CreateNameDictionaries()
{
	m_VersionNames[SystemConfiguration::Unknown] = _T("");
	m_VersionNames[SystemConfiguration::UnKnown2000] = _T("Windows 2000");
	m_VersionNames[SystemConfiguration::Windows95] = _T("Windows 95");
	m_VersionNames[SystemConfiguration::Windows98] = _T("Windows 98");
	m_VersionNames[SystemConfiguration::WindowsMe] = _T("Windows Me");
	m_VersionNames[SystemConfiguration::WindowsNT35] = _T("Windows NT 3.51");
	m_VersionNames[SystemConfiguration::WindowsNT40] = _T("Windows NT 4.0");
	m_VersionNames[SystemConfiguration::Windows2000] = _T("Windows 2000");
	m_VersionNames[SystemConfiguration::WindowsXP] = _T("Windows XP");
	m_VersionNames[SystemConfiguration::Windows2003] = _T("Windows Server 2003");
	m_VersionNames[SystemConfiguration::WindowsVista] = _T("Windows Vista");
	m_VersionNames[SystemConfiguration::Windows2008] = _T("Windows Server 2008");
	m_VersionNames[SystemConfiguration::Windows7] = _T("Windows 7");
	m_VersionNames[SystemConfiguration::Windows8] = _T("Windows 8");
	m_VersionNames[SystemConfiguration::Windows8_1] = _T("Windows 8.1");
	m_VersionNames[SystemConfiguration::Windows10] = _T("Windows 10");

	m_ProductNames[SystemConfiguration::UnknownProduct] = _T("");
	m_ProductNames[SystemConfiguration::Home] = _T("Home Edition");
	m_ProductNames[SystemConfiguration::Server] = _T("Server");
	m_ProductNames[SystemConfiguration::Professional] = _T("Professional Edition");
}

/// システム構成の文字列表現を返します。
CString SystemConfiguration::GetConfigurationName() const
{
	CString name;
	Dictionary::const_iterator it = m_VersionNames.find(m_OsVersion);
	if (it != m_VersionNames.end())
	{
		name += (*it).second;
	}

	if (m_OsVersion != Windows2003 && m_OsVersion != Windows2008) {
		it = m_ProductNames.find(m_OsProduct);
		if (it != m_ProductNames.end())
		{
			name += _T(" ") + (*it).second;
		}
	}

	if (m_ServicePackName != _T(""))
	{
		name += _T(" ") + m_ServicePackName;
	}

	if (m_OsProssesorEdition == SystemConfiguration::x64)
	{
		name += _T(" 64bit版");
	}
	return name;
}

bool SystemConfiguration::isMeiryoEnabled()
{
	return m_OsVersion == SystemConfiguration::WindowsVista ||
		m_OsVersion == SystemConfiguration::Windows2008 ||
		m_OsVersion == SystemConfiguration::Windows7 ||
		m_OsVersion == SystemConfiguration::Windows8 ||
		m_OsVersion == SystemConfiguration::Windows8_1 ||
		m_OsVersion == SystemConfiguration::Windows10;
}

bool SystemConfiguration::isMeiryoUiEnabled()
{
	return m_OsVersion == SystemConfiguration::Windows7 ||
		m_OsVersion == SystemConfiguration::Windows8 ||
		m_OsVersion == SystemConfiguration::Windows8_1 ||
		m_OsVersion == SystemConfiguration::Windows10;
}
