#if !defined(AFX_LINKINFODLG_H__6557B4B6_290A_11D3_80A7_00A0C9B72FDD__INCLUDED_)
#define AFX_LINKINFODLG_H__6557B4B6_290A_11D3_80A7_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LinkInfoDlg.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// LinkInfoDlg �_�C�A���O

class LinkInfoDlg : public CDialog
{
// �R���X�g���N�V����
public:
	int lineWidth;
	int styleLine;
	COLORREF colorLine;
	LOGFONT lf;
	int styleArrow;
	CString strTo;
	CString strFrom;
	CString strComment;
	LinkInfoDlg(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(LinkInfoDlg)
	enum { IDD = IDD_LINKINFO };
	CComboBox	m_cmbLineStyle;
	CStatic	m_LabelTo;
	CStatic	m_LabelFrom;
	CEdit	m_edit;
	CComboBox	m_combo;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(LinkInfoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(LinkInfoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCombo();
	virtual void OnOK();
	afx_msg void OnBtnColor();
	afx_msg void OnBtnFont();
	afx_msg void OnSelchangeCmbLine();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_LINKINFODLG_H__6557B4B6_290A_11D3_80A7_00A0C9B72FDD__INCLUDED_)
