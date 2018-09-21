/////////////////////////////////////////////////////////////////////////////
// FoldingSettingsDlg ダイアログ

#pragma once

class FoldingSettingsDlg : public CDialog
{
public:
	FoldingSettingsDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_SETFOLDUP };
	CSpinButtonCtrl	m_spin;
	int		m_level;

protected:

	virtual void DoDataExchange(CDataExchange* pDX);

	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
