#if !defined(AFX_SHAPEINSDLG_H__598871A3_2A0E_11D4_AE6A_00A0C9B72FDD__INCLUDED_)
#define AFX_SHAPEINSDLG_H__598871A3_2A0E_11D4_AE6A_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// shapeInsDlg.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// shapeInsDlg �_�C�A���O

class shapeInsDlg : public CDialog
{
// �R���X�g���N�V����
public:
	shapeInsDlg(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(shapeInsDlg)
	enum { IDD = IDD_SHAPEINS };
	CEdit	m_edPath;
	CStatic	m_iShape;
	CButton	m_BtnBrowse;
	CString	m_strPath;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(shapeInsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(shapeInsDlg)
	afx_msg void OnBtnBrowse();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_SHAPEINSDLG_H__598871A3_2A0E_11D4_AE6A_00A0C9B72FDD__INCLUDED_)
