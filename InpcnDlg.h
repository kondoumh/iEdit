#if !defined(AFX_INPCNDLG_H__3912E181_B749_11D2_BB6F_F08B54D33F0D__INCLUDED_)
#define AFX_INPCNDLG_H__3912E181_B749_11D2_BB6F_F08B54D33F0D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InpcnDlg.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// CInpcnDlg �_�C�A���O

class CInpcnDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CRect iniRect;
	CPoint m_iniPt;
	CInpcnDlg(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CInpcnDlg)
	enum { IDD = IDD_INPCN };
	CEdit	m_editName;
	CString	m_strcn;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CInpcnDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	LRESULT afx_msg OnSetCharData(UINT wParam, LONG lParam);

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CInpcnDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_INPCNDLG_H__3912E181_B749_11D2_BB6F_F08B54D33F0D__INCLUDED_)
