#if !defined(AFX_SELIMPORTDLG_H__A62F3CA6_D771_11D3_AE0C_00A0C9B72FDD__INCLUDED_)
#define AFX_SELIMPORTDLG_H__A62F3CA6_D771_11D3_AE0C_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelImportDlg.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// SelImportDlg �_�C�A���O

class SelImportDlg : public CDialog
{
// �R���X�g���N�V����
public:
	SelImportDlg(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(SelImportDlg)
	enum { IDD = IDD_SELIMPORT };
	int		m_importMode;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(SelImportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(SelImportDlg)
	afx_msg void OnRdRep();
	afx_msg void OnRdSub();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_SELIMPORTDLG_H__A62F3CA6_D771_11D3_AE0C_00A0C9B72FDD__INCLUDED_)
