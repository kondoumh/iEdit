#if !defined(AFX_PAGEFRAME_H__79C43546_7FE6_11D3_9860_006097789FE5__INCLUDED_)
#define AFX_PAGEFRAME_H__79C43546_7FE6_11D3_9860_006097789FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageFrame.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// PageFrame �_�C�A���O

class PageFrame : public CDialog
{
// �R���X�g���N�V����
public:
	CFont fntText;
	CFont fntLink;
	CFont fntOutline;
	COLORREF m_colorInsrtMrk;
	COLORREF m_colorEditBG;
	COLORREF m_colorEditFor;
	COLORREF m_colorOLBG;
	COLORREF m_colorOLFor;
	COLORREF m_colorLNBG;
	COLORREF m_colorLNFor;
	COLORREF m_colorNWBG;
	LOGFONT lfText;
	LOGFONT lfLink;
	LOGFONT lfOutline;
	PageFrame(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(PageFrame)
	enum { IDD = IDD_PG_FRAME };
	CButton	m_ChkSaveBarState;
	CButton	m_chkSaveFrame;
	BOOL	m_bSaveFrame;
	BOOL	m_bSaveBarState;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(PageFrame)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	void drawNWPreView(CDC* pDC);
	void drawLNPreView(CDC* pDC);
	void drawOLPreView(CDC* pDC);
	void drawTextPreView(CDC* pDC);

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(PageFrame)
	virtual void OnCancel();
	afx_msg void OnBtnLink();
	afx_msg void OnBtnOutline();
	afx_msg void OnBtnText();
	afx_msg void OnChkSaveBarState();
	afx_msg void OnChkSaveFrame();
	afx_msg void OnPaint();
	afx_msg void OnBtnFclrOl();
	afx_msg void OnBtnBclrOl();
	afx_msg void OnBtnFclrLn();
	afx_msg void OnBtnBclrLn();
	afx_msg void OnBtnBclrNw();
	afx_msg void OnBtnFclrTx();
	afx_msg void OnBtnBclrTx();
	afx_msg void OnBtnInsertmark();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void updateFont(LOGFONT* plf, CFont& font);
	void drawFontPreview(CDC *pDC, CRect& rc, CFont& font, COLORREF bgColor, COLORREF fontColor);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_PAGEFRAME_H__79C43546_7FE6_11D3_9860_006097789FE5__INCLUDED_)
