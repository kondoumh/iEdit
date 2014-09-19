#pragma once
#include "afxwin.h"


// SetTextExportDlg ダイアログ

class SetTextExportDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SetTextExportDlg)

public:
	SetTextExportDlg(CWnd* pParent = NULL);   // 標準コンストラクター
	virtual ~SetTextExportDlg();

// ダイアログ データ
	enum { IDD = IDD_SET_TEXT_EXPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

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
public:
	afx_msg void OnBnClickedNodeText();
	afx_msg void OnBnClickedNodeOnly();
	afx_msg void OnBnClickedFileEveryNode();
	afx_msg void OnBnClickedMarkdown();
	afx_msg void OnBnClickedOrgMode();
};
