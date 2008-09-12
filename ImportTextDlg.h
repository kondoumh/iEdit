#pragma once


// ImportTextDlg ダイアログ

class ImportTextDlg : public CDialog
{
	DECLARE_DYNAMIC(ImportTextDlg)

public:
	ImportTextDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~ImportTextDlg();

// ダイアログ データ
	enum { IDD = IDD_IMPORT_TEXT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	int m_charSelection;
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
};
