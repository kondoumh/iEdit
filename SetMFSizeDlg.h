#if !defined(AFX_SETMFSIZEDLG_H__4DDA3CFA_849A_4875_87EE_0CACC6A6E93F__INCLUDED_)
#define AFX_SETMFSIZEDLG_H__4DDA3CFA_849A_4875_87EE_0CACC6A6E93F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetMFSizeDlg.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// SetMFSizeDlg �_�C�A���O

class SetMFSizeDlg : public CDialog
{
// �R���X�g���N�V����
public:
	SetMFSizeDlg(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^
	double m_ry;
	double m_rx;

// �_�C�A���O �f�[�^
	//{{AFX_DATA(SetMFSizeDlg)
	enum { IDD = IDD_MFSIZE };
	CStatic	m_stInfo;
	CStatic	m_stMF;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(SetMFSizeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	void showSizeInfo();
	HENHMETAFILE createMF();

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(SetMFSizeDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnLeft();
	afx_msg void OnBtnRight();
	afx_msg void OnBtnDown();
	afx_msg void OnBtnUp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnReset();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_SETMFSIZEDLG_H__4DDA3CFA_849A_4875_87EE_0CACC6A6E93F__INCLUDED_)
