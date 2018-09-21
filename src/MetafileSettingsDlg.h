/////////////////////////////////////////////////////////////////////////////
// MetafileSettingsDlg ダイアログ

#pragma once

class MetafileSettingsDlg : public CDialog
{
public:
	MetafileSettingsDlg(CWnd* pParent = NULL);
	double m_ry;
	double m_rx;

	enum { IDD = IDD_MFSIZE };
	CStatic	m_stInfo;
	CStatic	m_stMF;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	virtual BOOL OnInitDialog();
	afx_msg void OnBtnLeft();
	afx_msg void OnBtnRight();
	afx_msg void OnBtnDown();
	afx_msg void OnBtnUp();
	afx_msg void OnBnClickedBtnReset();
	DECLARE_MESSAGE_MAP()

private:
	void showSizeInfo();
	HENHMETAFILE createMF();
};
