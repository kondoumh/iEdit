#pragma once


// FiletypeSettingDlg ダイアログ

class FiletypeSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(FiletypeSettingDlg)

public:
	FiletypeSettingDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~FiletypeSettingDlg();

// ダイアログ データ
	enum { IDD = IDD_SHELL_FILETYPE_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	int m_registFileType;
	BOOL m_bRegistOldtype;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRdNotRegist();
	afx_msg void OnBnClickedRdRegist();
};
