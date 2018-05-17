#pragma once

// FiletypeSettingsDlg ダイアログ

class FiletypeSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(FiletypeSettingsDlg)

public:
	FiletypeSettingsDlg(CWnd* pParent = NULL);
	virtual ~FiletypeSettingsDlg();

	enum { IDD = IDD_SHELL_FILETYPE_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	int m_registFileType;
	BOOL m_bRegistOldtype;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRdNotRegist();
	afx_msg void OnBnClickedRdRegist();
};
