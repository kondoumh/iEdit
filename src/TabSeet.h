/////////////////////////////////////////////////////////////////////////////
// CTabSeet ウィンドウ

#pragma once

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
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

inline bool CTabSeet::SetPage() {
	return SetPage(GetCurSel());
}
