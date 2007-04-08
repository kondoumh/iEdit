#if !defined(AFX_PAGEOTHER_H__023C7320_8088_11D3_B4D3_00A0C9B72FDD__INCLUDED_)
#define AFX_PAGEOTHER_H__023C7320_8088_11D3_B4D3_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageOther.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// PageOther �_�C�A���O

class PageOther : public CDialog
{
// �R���X�g���N�V����
public:
	PageOther(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(PageOther)
	enum { IDD = IDD_PG_OTHER };
	CButton	m_chkStylesheet;
	CEdit	m_edStylesheet;
	CButton	m_chkDrawUnderLine;
	CButton	m_chkAccelMove;
	CButton	m_chkInheritSibling;
	CButton	m_chkInheritParent;
	CButton	m_chkShowHS;
	int		m_tabSelect;
	BOOL	m_bShowHS;
	BOOL	m_bInheritParent;
	BOOL	m_bInheritSibling;
	BOOL	m_bAccelmove;
	BOOL	m_bDrawUnderLine;
	BOOL	m_bSetStylesheet;
	CString	m_strStylesheet;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(PageOther)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(PageOther)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnRdTab1();
	afx_msg void OnRdTab2();
	afx_msg void OnRdTab3();
	afx_msg void OnChkShowHs();
	afx_msg void OnChkInheritParent();
	afx_msg void OnChkInheritSibling();
	afx_msg void OnChkAccel();
	afx_msg void OnBtnSetmfsize();
	afx_msg void OnChkDrwUndrln();
	afx_msg void OnChkSetStylesheet();
	afx_msg void OnChangeEditStylesheet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_PAGEOTHER_H__023C7320_8088_11D3_B4D3_00A0C9B72FDD__INCLUDED_)
