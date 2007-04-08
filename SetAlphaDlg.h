#if !defined(AFX_SETALPHADLG_H__B956C603_D213_44F3_8CE3_785ECD40795B__INCLUDED_)
#define AFX_SETALPHADLG_H__B956C603_D213_44F3_8CE3_785ECD40795B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetAlphaDlg.h : �w�b�_�[ �t�@�C��
//

#define WM_SLIDEALPHA WM_USER + 11

/////////////////////////////////////////////////////////////////////////////
// SetAlphaDlg �_�C�A���O

class SetAlphaDlg : public CDialog
{
// �R���X�g���N�V����
public:
	long m_nLevel;
	SetAlphaDlg(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(SetAlphaDlg)
	enum { IDD = IDD_ALPHASLIDE };
	CStatic	m_lbLevel;
	CSliderCtrl	m_sldrAlpha;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(SetAlphaDlg)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(SetAlphaDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CWnd* m_pParent;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_SETALPHADLG_H__B956C603_D213_44F3_8CE3_785ECD40795B__INCLUDED_)
