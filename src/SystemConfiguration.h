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

private:
	// OSのバージョンのenum
	enum Versions {
		Unknown,
		UnKnown2000,
		Windows95,
		Windows98,
		WindowsMe,
		WindowsNT35,
		WindowsNT40,
		Windows2000,
		WindowsXP,
		Windows2003,
		WindowsVista,
		Windows2008,
		Windows7,
		Windows8,
		Windows8_1,
		Windows10
	};
	// OSのプロダクトタイプのenum
	enum ProductTypes { UnknownProduct, Home, Professional, Server };
	/// OSのプロセッサータイプのenum
	enum ProcessorTypes { x32, x64 };
	CString m_ConfigurationName; // システム構成の文字列表現を保持
	Dictionary m_VersionNames;   // OSのバージョン用ディクショナリー
	Dictionary m_ProductNames;   // OSのプロダクト名用ディクショナリー
	int m_OsVersion; //  OSのバージョンのenum値を格納
	int m_OsProduct; // OSのプロダクトタイプのenum値を格納
	int m_OsProssesorEdition; // OSのプロセッサータイプのenum値を格納
	CString m_ServicePackName; // OSのサービスパック名を格納

	CString GetConfigurationName() const;
	void CheckEnvironment();
	void CreateNameDictionaries();
};
