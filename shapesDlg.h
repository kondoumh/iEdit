#if !defined(AFX_SHAPESDLG_H__EE06AEF3_1CC4_11D4_AE5E_00A0C9B72FDD__INCLUDED_)
#define AFX_SHAPESDLG_H__EE06AEF3_1CC4_11D4_AE5E_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// shapesDlg.h : �w�b�_�[ �t�@�C��
//

#define WM_CLOSESHAPEWINDOW WM_USER + 5
#define WM_ADDSHAPE WM_USER + 6
#define WM_GETSHAPE WM_USER + 10

/////////////////////////////////////////////////////////////////////////////
// shapesDlg �_�C�A���O

class shapesDlg : public CDialog
{
// �R���X�g���N�V����
public:
	shapesDlg(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^
	void regNodeShape(HENHMETAFILE hMF);

// �_�C�A���O �f�[�^
	//{{AFX_DATA(shapesDlg)
	enum { IDD = IDD_SHAPES };
	CButton	m_btnGet;
	CButton	m_btnDrop;
	CButton	m_btnRR;
	CButton	m_btnR;
	CButton	m_btnLL;
	CButton	m_btnL;
	CListBox	m_catListBox;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(shapesDlg)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	void decideShapef();
	void getFromView();
	void editCatName();

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(shapesDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnDrop();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangeCatlist();
	afx_msg void OnDblclkCatlist();
	afx_msg void OnUpdateDiagram();
	afx_msg void OnNewShape();
	afx_msg void OnEditCatName();
	afx_msg void OnDeleteShape();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBtnL();
	afx_msg void OnBtnR();
	afx_msg void OnBtnLl();
	afx_msg void OnBtnRr();
	afx_msg void OnSaveShapeFile();
	afx_msg void OnOpenShapeFile();
	afx_msg void OnBtnget();
	afx_msg void OnRegistNodeshape();
	afx_msg void OnPasteFromClpbrd();
	afx_msg void OnUpdatePasteFromClpbrd(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CRect m_oldRect;
	void updateNetVew();
	int m_indexIncat;
	CRect m_bounds[20];
	CRect m_selRect;
	CWnd* m_pParent;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_SHAPESDLG_H__EE06AEF3_1CC4_11D4_AE5E_00A0C9B72FDD__INCLUDED_)
