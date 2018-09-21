/////////////////////////////////////////////////////////////////////////////
// ExportXmlDlg ダイアログ

#pragma once

class ExportXmlDlg : public CDialog
{
public:
	ExportXmlDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_SELEXPORT };
	int		m_nTreeOp;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
};
