#if !defined(AFX_INPCATDLG_H__2ACE9F19_2A02_11D4_AE6A_00A0C9B72FDD__INCLUDED_)
#define AFX_INPCATDLG_H__2ACE9F19_2A02_11D4_AE6A_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InpcatDlg.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// CInpcatDlg �_�C�A���O

class CInpcatDlg : public CDialog
{
// �R���X�g���N�V����
public:
	CInpcatDlg(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CInpcatDlg)
	enum { IDD = IDD_SHPCATDLG };
	CEdit	m_edit;
	CString	m_strName;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CInpcatDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CInpcatDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEdit1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_INPCATDLG_H__2ACE9F19_2A02_11D4_AE6A_00A0C9B72FDD__INCLUDED_)
