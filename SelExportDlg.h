#if !defined(AFX_SELEXPORTDLG_H__52A28553_FB9A_11D3_AE3F_00A0C9B72FDD__INCLUDED_)
#define AFX_SELEXPORTDLG_H__52A28553_FB9A_11D3_AE3F_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelExportDlg.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// SelExportDlg �_�C�A���O

class SelExportDlg : public CDialog
{
// �R���X�g���N�V����
public:
	SelExportDlg(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^
	bool m_bShowChekPrintText;
	
// �_�C�A���O �f�[�^
	//{{AFX_DATA(SelExportDlg)
	enum { IDD = IDD_SELEXPORT };
	CButton	m_chkPrintText;
	BOOL	m_bPrintText;
	int		m_nTreeOp;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(SelExportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(SelExportDlg)
	afx_msg void OnChkPrintText();
	virtual BOOL OnInitDialog();
	afx_msg void OnRdTree1();
	afx_msg void OnRdTree2();
	afx_msg void OnRdTree3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_SELEXPORTDLG_H__52A28553_FB9A_11D3_AE3F_00A0C9B72FDD__INCLUDED_)
