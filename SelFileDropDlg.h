#if !defined(AFX_SELFILEDROPDLG_H__B17C7D51_E20B_11D5_9ACE_00105A6E744E__INCLUDED_)
#define AFX_SELFILEDROPDLG_H__B17C7D51_E20B_11D5_9ACE_00105A6E744E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelFileDropDlg.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// SelFileDropDlg �_�C�A���O

class SelFileDropDlg : public CDialog
{
// �R���X�g���N�V����
public:
	SelFileDropDlg(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(SelFileDropDlg)
	enum { IDD = IDD_SELFILEDROP };
	int		m_nDropProc;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(SelFileDropDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(SelFileDropDlg)
		// ����: ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ����܂��B
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_SELFILEDROPDLG_H__B17C7D51_E20B_11D5_9ACE_00105A6E744E__INCLUDED_)
