#pragma once
#include "afxwin.h"

// ExportTextDlg ダイアログ

class ExportTextDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ExportTextDlg)

public:
	ExportTextDlg(CWnd* pParent = NULL);
	virtual ~ExportTextDlg();

	enum { IDD = IDD_SET_TEXT_EXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
public:

	int m_rdTreeOption;
	int m_rdFormatOption;
	int m_rdChapterNumberOption;
	BOOL m_excludeLabelFromFileName;
	BOOL m_excludeLabelFromContent;
	virtual BOOL OnInitDialog();
private:
	void EnableContentFileOption(BOOL enable);
	void EnableChapterNumberOption(BOOL enable);
public:
	afx_msg void OnBnClickedNodeText();
	afx_msg void OnBnClickedNodeOnly();
	afx_msg void OnBnClickedFileEveryNode();
	afx_msg void OnBnClickedMarkdown();
	afx_msg void OnBnClickedOrgMode();
};
