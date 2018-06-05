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
	int m_charSelection;
	CString m_fileName;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
};
