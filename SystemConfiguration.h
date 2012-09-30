#pragma once
#include <map>

using namespace std;

typedef map<int, CString> Dictionary;

class SystemConfiguration
{
private:
	CString m_ConfigurationName; // �V�X�e���\���̕�����\����ێ�
	Dictionary m_VersionNames;   // OS�̃o�[�W�����p�f�B�N�V���i���[
	Dictionary m_ProductNames;   // OS�̃v���_�N�g���p�f�B�N�V���i���[
	
public:
	SystemConfiguration(void);
	~SystemConfiguration(void);
	
	// OS�̃o�[�W������enum
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
		Windows8
	};
	// OS�̃v���_�N�g�^�C�v��enum
	enum ProductTypes {UnknownProduct, Home, Professional, Server};
	/// OS�̃v���Z�b�T�[�^�C�v��enum
	enum ProcessorTypes {x32, x64};
	
	int m_OsVersion; //  OS�̃o�[�W������enum�l���i�[
	int m_OsProduct; // OS�̃v���_�N�g�^�C�v��enum�l���i�[
	int m_OsProssesorEdition; // OS�̃v���Z�b�T�[�^�C�v��enum�l���i�[
	CString m_ServicePackName; // OS�̃T�[�r�X�p�b�N�����i�[
	CString GetConfigurationName() const;
	bool isMeiryoEnabled();
	bool isMeiryoUiEnabled();
private:
	void CheckEnvironment();
	void CreateNameDictionaries();
};
