#if !defined(AFX_PAGELINK_H__79C43545_7FE6_11D3_9860_006097789FE5__INCLUDED_)
#define AFX_PAGELINK_H__79C43545_7FE6_11D3_9860_006097789FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageLink.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// PageLink �_�C�A���O

class PageLink : public CDialog
{
// �R���X�g���N�V����
public:
	int strength;
	int lineWidth;
	int styleLine;
	COLORREF colorLine;
	LOGFONT lf;
	PageLink(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(PageLink)
	enum { IDD = IDD_PG_LINK };
	CStatic	m_strn;
	CSliderCtrl	m_Slider;
	CButton	m_chkSetStreangth;
	CComboBox	m_cmbLine;
	BOOL	m_bSetStrength;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(PageLink)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(PageLink)
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnColor();
	afx_msg void OnBtnFont();
	afx_msg void OnSelchangeCmbLine();
	afx_msg void OnChkStrength();
	afx_msg void OnReleasedcaptureSlid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_PAGELINK_H__79C43545_7FE6_11D3_9860_006097789FE5__INCLUDED_)
