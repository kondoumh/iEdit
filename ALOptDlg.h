#if !defined(AFX_ALOPTDLG_H__A1873983_92DF_11D3_9892_006097789FE5__INCLUDED_)
#define AFX_ALOPTDLG_H__A1873983_92DF_11D3_9892_006097789FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ALOptDlg.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// ALOptDlg �_�C�A���O

class ALOptDlg : public CDialog
{
// �R���X�g���N�V����
public:
	ALOptDlg(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(ALOptDlg)
	enum { IDD = IDD_SEL_LAYOUT };
	CSpinButtonCtrl	m_spin;
	CEdit	m_editCY;
	int		m_showMode;
	int		m_cycle;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(ALOptDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(ALOptDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnRdAni();
	afx_msg void OnRdCy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_ALOPTDLG_H__A1873983_92DF_11D3_9892_006097789FE5__INCLUDED_)
