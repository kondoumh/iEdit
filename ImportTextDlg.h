#pragma once


// ImportTextDlg �_�C�A���O

class ImportTextDlg : public CDialog
{
	DECLARE_DYNAMIC(ImportTextDlg)

public:
	ImportTextDlg(CWnd* pParent = NULL);   // �W���R���X�g���N�^
	virtual ~ImportTextDlg();

// �_�C�A���O �f�[�^
	enum { IDD = IDD_IMPORT_TEXT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
public:
	int m_charSelection;
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
};
