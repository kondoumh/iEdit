#if !defined(AFX_TABSEET_H__A07C8081_FCC0_11D2_B43A_00A0C9B72F81__INCLUDED_)
#define AFX_TABSEET_H__A07C8081_FCC0_11D2_B43A_00A0C9B72F81__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TabSeet.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// CTabSeet �E�B���h�E

class CTabSeet : public CTabCtrl
{
  typedef std::pair<CDialog*,UINT> page;
  std::vector<page> page_;
  int               curr_;
  CRect             rect_;

// �R���X�g���N�V����
public:
	CTabSeet();

// �A�g���r���[�g
public:

// �I�y���[�V����
public:

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CTabSeet)
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
public:
	template<class Dialog> 
	void addPage(Dialog* dialog) { 
		page_.push_back(page(dialog,Dialog::IDD)); 
	}
	void endService(bool deletePage);
	void beginService(int start = 0);
	bool setPage(int page);
	bool setPage();
	virtual ~CTabSeet();

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
	//{{AFX_MSG(CTabSeet)
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

inline bool CTabSeet::setPage() {
  return setPage(GetCurSel()); 
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_TABSEET_H__A07C8081_FCC0_11D2_B43A_00A0C9B72F81__INCLUDED_)
