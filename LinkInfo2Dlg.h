#if !defined(AFX_LINKINFO2DLG_H__BE7CAEF3_2AA4_11D3_80A9_00A0C9B72FDD__INCLUDED_)
#define AFX_LINKINFO2DLG_H__BE7CAEF3_2AA4_11D3_80A9_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LinkInfo2Dlg.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// LinkInfo2Dlg �_�C�A���O

class LinkInfo2Dlg : public CDialog
{
// �R���X�g���N�V����
public:
	CString strComment;
	CString strPath;
	CString strOrg;
	LinkInfo2Dlg(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(LinkInfo2Dlg)
	enum { IDD = IDD_LINKINFO2 };
	CEdit	m_Comment;
	CEdit	m_edit;
	CStatic	m_Org;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(LinkInfo2Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(LinkInfo2Dlg)
	afx_msg void OnBrowse();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_LINKINFO2DLG_H__BE7CAEF3_2AA4_11D3_80A9_00A0C9B72FDD__INCLUDED_)
