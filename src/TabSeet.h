#if !defined(AFX_TABSEET_H__A07C8081_FCC0_11D2_B43A_00A0C9B72F81__INCLUDED_)
#define AFX_TABSEET_H__A07C8081_FCC0_11D2_B43A_00A0C9B72F81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabSeet.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// CTabSeet ウィンドウ

class CTabSeet : public CTabCtrl
{
	typedef std::pair<CDialog*, UINT> page;
	std::vector<page> page_;
	int curr_;
	CRect rect_;

public:
	CTabSeet();
	template<class Dialog>
	void AddPage(Dialog* dialog) {
		page_.push_back(page(dialog, Dialog::IDD));
	}
	void EndService(bool deletePage);
	void BeginService(int start = 0);
	bool SetPage(int page);
	bool SetPage();
	virtual ~CTabSeet();

protected:

	//{{AFX_VIRTUAL(CTabSeet)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CTabSeet)
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

inline bool CTabSeet::SetPage() {
	return SetPage(GetCurSel());
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_TABSEET_H__A07C8081_FCC0_11D2_B43A_00A0C9B72F81__INCLUDED_)
