#pragma once


// ImportTextDlg ダイアログ

class ImportTextDlg : public CDialog
{
	DECLARE_DYNAMIC(ImportTextDlg)

public:
	ImportTextDlg(CWnd* pParent = NULL);
	virtual ~ImportTextDlg();

// ダイアログ データ
	enum { IDD = IDD_IMPORT_TEXT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:
	int m_charSelection;
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	CString m_fileName;
};
