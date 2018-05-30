#pragma once
// ExportHtmlDlg ダイアログ

class ExportHtmlDlg : public CDialog
{
	DECLARE_DYNAMIC(ExportHtmlDlg)

public:
	ExportHtmlDlg(CWnd* pParent = NULL);
	virtual ~ExportHtmlDlg();

	enum { IDD = IDD_SET_HTML_EXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

public:
	CString m_NameOfRoot;
	CString m_NameOfVisibleRoot;
	CString m_pathIndex;
	CString m_pathTextSingle;
	CString m_pathOutline;
	CString m_pathNetwork;
	CString m_pathSvg;
	CString m_pathPng;
	CString m_sDocTitle;
	int m_xvRdNav;
	int m_xvRdImg;
	int m_xvRdText;
	CString m_xvEdPrfIndex;
	CString m_xvEdPrfToc;
	CString m_xvEdPrfTextSingle;
	CString m_xvEdPrfTextEverynode;
	CString m_xvEdPrfNet;
	BOOL m_xvRdTree;

private:
	void setPathIndex();
	void setPathTextSingle();
	void setPathOutline();
	void setPathNetwork();
	void SetRdImageEnable(BOOL bEnable);
	void SetEdPrfTextSingleEnable(BOOL bEnable);
	void SetEdPrfTextEverynodeEnable(BOOL bEnable);

protected:
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedRdNavOutline();
	afx_msg void OnBnClickedRdNavNetwork();
	afx_msg void OnBnClickedRdNavBoth();
	afx_msg void OnBnClickedRdTextEverynode();
	afx_msg void OnBnClickedRdTextSingle();
	afx_msg void OnBnClickedBtnSetPrfByFname();
	afx_msg void OnEnChangeEdPrfToc();
	afx_msg void OnEnChangeEdPrfIndex();
	afx_msg void OnEnChangeEdPrfNet();
	afx_msg void OnEnChangeEdPrfTextSingle();
	afx_msg void OnEnChangeEdPrfTextEverynode();
	afx_msg void OnBnClickedBtnSetPrfByRoot();
	afx_msg void OnBnClickedBtnSetPrfByVisibleRoot();
};
