#if !defined(AFX_SETFOLDUPDLG_H__9B186546_554F_11D4_AB29_006097789FE5__INCLUDED_)
#define AFX_SETFOLDUPDLG_H__9B186546_554F_11D4_AB29_006097789FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// setFoldUpDlg.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// setFoldUpDlg �_�C�A���O

class setFoldUpDlg : public CDialog
{
// �R���X�g���N�V����
public:
	setFoldUpDlg(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(setFoldUpDlg)
	enum { IDD = IDD_SETFOLDUP };
	CSpinButtonCtrl	m_spin;
	int		m_level;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(setFoldUpDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(setFoldUpDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_SETFOLDUPDLG_H__9B186546_554F_11D4_AB29_006097789FE5__INCLUDED_)
