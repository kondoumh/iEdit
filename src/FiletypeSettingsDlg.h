// FiletypeSettingsDlg ダイアログ
#pragma once

class FiletypeSettingsDlg : public CDialog
{
	DECLARE_DYNAMIC(FiletypeSettingsDlg)

public:
	FiletypeSettingsDlg(CWnd* pParent = NULL);
	virtual ~FiletypeSettingsDlg();

	enum { IDD = IDD_SHELL_FILETYPE_SETTING };
	int m_registFileType;
	BOOL m_bRegistOldtype;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedRdNotRegist();
	afx_msg void OnBnClickedRdRegist();
};
