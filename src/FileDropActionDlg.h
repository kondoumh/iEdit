/////////////////////////////////////////////////////////////////////////////
// FileDropActionDlg ダイアログ

#pragma once

class FileDropActionDlg : public CDialog
{
public:
	FileDropActionDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_SELFILEDROP };
	int		m_nDropProc;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
};
