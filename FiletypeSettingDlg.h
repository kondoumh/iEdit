#pragma once


// FiletypeSettingDlg �_�C�A���O

class FiletypeSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(FiletypeSettingDlg)

public:
	FiletypeSettingDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^�[
	virtual ~FiletypeSettingDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_SHELL_FILETYPE_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	int m_registFileType;
	BOOL m_bRegistOldtype;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRdNotRegist();
	afx_msg void OnBnClickedRdRegist();
};
