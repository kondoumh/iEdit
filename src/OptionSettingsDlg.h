#if !defined(AFX_PROPERTYDLG_H__79C43541_7FE6_11D3_9860_006097789FE5__INCLUDED_)
#define AFX_PROPERTYDLG_H__79C43541_7FE6_11D3_9860_006097789FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyDlg.h : ヘッダー ファイル
//

#include "TabSeet.h"

/////////////////////////////////////////////////////////////////////////////
// OptionSettingsDlg ダイアログ
class OptionPageForFrame;
class OptionPageForNode;
class OptionPageForLink;
class OptionPageForOther;

class OptionSettingsDlg : public CDialog
{
public:
	OptionSettingsDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(OptionSettingsDlg)
	enum { IDD = IDD_PROPERTIES };
	CTabSeet	m_tabSeet;
	//}}AFX_DATA

protected:

	//{{AFX_VIRTUAL(OptionSettingsDlg)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

	//{{AFX_MSG(OptionSettingsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
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

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_PROPERTYDLG_H__79C43541_7FE6_11D3_9860_006097789FE5__INCLUDED_)
