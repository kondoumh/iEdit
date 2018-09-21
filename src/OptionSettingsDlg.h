/////////////////////////////////////////////////////////////////////////////
// OptionSettingsDlg ダイアログ

#pragma once

#include "TabSeet.h"

class OptionPageForFrame;
class OptionPageForNode;
class OptionPageForLink;
class OptionPageForOther;

class OptionSettingsDlg : public CDialog
{
public:
	OptionSettingsDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_PROPERTIES };
	CTabSeet	m_tabSeet;

protected:

	virtual void DoDataExchange(CDataExchange* pDX);

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	DECLARE_MESSAGE_MAP()

private:
	OptionPageForNode * pNode;
	OptionPageForLink* pLink;
	OptionPageForFrame* pFrame;
	OptionPageForOther* pOther;

	void InitPageFrame();
	void InitPageNode();
	void InitPageLink();
	void InitPageOther();
	void WriteFrameSettings();
	void WriteNodeSettings();
	void WriteLinkSettings();
	void WriteOtherSettings();
};
