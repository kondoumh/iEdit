#if !defined(AFX_RANDAREADLG_H__5FB3CA53_D7F0_11D3_AA35_006097789FE5__INCLUDED_)
#define AFX_RANDAREADLG_H__5FB3CA53_D7F0_11D3_AA35_006097789FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// randAreaDlg.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// randAreaDlg �_�C�A���O

class randAreaDlg : public CDialog
{
// �R���X�g���N�V����
public:
	randAreaDlg(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(randAreaDlg)
	enum { IDD = IDD_RANDAREA };
	int		m_height;
	int		m_width;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(randAreaDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(randAreaDlg)
		// ����: ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ����܂��B
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_RANDAREADLG_H__5FB3CA53_D7F0_11D3_AA35_006097789FE5__INCLUDED_)
