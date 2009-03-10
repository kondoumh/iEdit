#pragma once


// SetHtmlExportDlg ダイアログ

class SetHtmlExportDlg : public CDialog
{
	DECLARE_DYNAMIC(SetHtmlExportDlg)

public:
	SetHtmlExportDlg(CWnd* pParent = NULL);   // 標準コンストラクタ
	virtual ~SetHtmlExportDlg();

// ダイアログ データ
	enum { IDD = IDD_SET_HTML_EXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
public:
	CString m_sDocTitle;
	int m_xvRdNav;
	int m_xvRdImg;
	int m_xvRdText;
	CString m_xvEdCssToc;
	CString m_xvEdCssText;
	CString m_xvEdPrfIndex;
	CString m_xvEdPrfToc;
	CString m_xvEdPrfTextSingle;
	CString m_xvEdPrfTextEverynode;
	afx_msg void OnBnClickedRdNavOutline();
private:
	void SetRdImageEnable(BOOL bEnable);
	void SetEdPrfTextSingleEnable(BOOL bEnable);
	void SetEdPrfTextEverynodeEnable(BOOL bEnable);
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRdNavNetwork();
	afx_msg void OnBnClickedRdNavBoth();
	afx_msg void OnBnClickedRdTextEverynode();
	afx_msg void OnBnClickedRdTextSingle();
	CString m_xvEdPrfNet;
	afx_msg void OnBnClickedBtnSetPrfByFname();
	afx_msg void OnEnChangeEdPrfToc();
	afx_msg void OnEnChangeEdPrfIndex();
	afx_msg void OnEnChangeEdPrfNet();
	afx_msg void OnEnChangeEdPrfTextSingle();
};
