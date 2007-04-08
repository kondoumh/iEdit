#if !defined(AFX_PROPERTYDLG_H__79C43541_7FE6_11D3_9860_006097789FE5__INCLUDED_)
#define AFX_PROPERTYDLG_H__79C43541_7FE6_11D3_9860_006097789FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyDlg.h : �w�b�_�[ �t�@�C��
//

#include "TabSeet.h"

/////////////////////////////////////////////////////////////////////////////
// CPropertyDlg �_�C�A���O
class PageFrame;
class PageNode;
class PageLink;
class PageOther;

class CPropertyDlg : public CDialog
{
// �R���X�g���N�V����
public:
	void initPageOther();
	CPropertyDlg(CWnd* pParent = NULL);   // �W���̃R���X�g���N�^

// �_�C�A���O �f�[�^
	//{{AFX_DATA(CPropertyDlg)
	enum { IDD = IDD_PROPERTIES };
	CTabSeet	m_tabSeet;
	//}}AFX_DATA


// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CPropertyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	void writePageOther();
	void writePageLink();
	void writePageNode();
	void writePageFrame();
	void initPageLink();
	void initPageFrame();
	void initPageNode();

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CPropertyDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	PageNode* pNode;
	PageLink* pLink;
	PageFrame* pFrame;
	PageOther* pOther;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_PROPERTYDLG_H__79C43541_7FE6_11D3_9860_006097789FE5__INCLUDED_)
