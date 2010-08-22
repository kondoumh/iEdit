// iEdit.h : IEDIT �A�v���P�[�V�����̃��C�� �w�b�_�[ �t�@�C��
//

#if !defined(AFX_IEDIT_H__96DFF9B7_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
#define AFX_IEDIT_H__96DFF9B7_1881_11D3_808A_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // ���C�� �V���{��

// ���W�X�g�����i�[�p�ϐ�
struct rgsNode {
	LOGFONT lf;
	int styleText;
	int styleLine;
	int lineWidth;
	int shape;
	COLORREF colorFill;
	COLORREF colorLine;
	COLORREF colorFont;
	BOOL bFillColor;
	BOOL bInheritParent;    // �e�m�[�h�̑������p��
	BOOL bInheritSibling;   // �Z��m�[�h�̑������p��
	BOOL bSyncOrder;
	int  orderDirection;
	BOOL bEnableGroup;
	BOOL bDisableNodeResize;
	int margin_l;
	int margin_r;
	int margin_t;
	int margin_b;
};

struct rgsLink {
	LOGFONT lf;
	int lineWidth;
	int styleLine;
	int strength;
	COLORREF colorLine;
	BOOL bSetStrength;
};

struct rgsOther {
	BOOL bSetStylesheet;
	CString strStyleSheetFile;
	BOOL bOpenFilesAfterExport;
	BOOL bOutputFileLinksOnExport;
};

typedef map<int, HENHMETAFILE> HMetaFiles;

void showLastErrorMessage();


/////////////////////////////////////////////////////////////////////////////
// CiEditApp:
// ���̃N���X�̓���̒�`�Ɋւ��Ă� iEdit.cpp �t�@�C�����Q�Ƃ��Ă��������B
//

class CiEditApp : public CWinApp
{
public:
	void DebugWriteLine(const CString& message);
	COLORREF m_colorNodeBtn;
	COLORREF m_colorLineBtn;
	COLORREF m_colorFontBtn;
	int m_curLinkArrow;
	int m_curLinkLineStyle;
	void getOtherProfile();
	void notifyViewSetting();
	bool m_bShapeModified;
	void saveMetaFiles(const CString& fname);
	void loadMetaFiles(const CString& fname);
	void getLinkProfile();
	void getNodeProfile();
	rgsNode m_rgsNode;
	rgsLink m_rgsLink;
	rgsOther m_rgsOther;
	HMetaFiles m_hMetaFiles;
	CString m_mfCategories[10];
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CiEditApp();
	
	// �����N���̎��
	enum {
		LS_R0,
		LS_DOT,
		LS_R1,
		LS_R2,
		LS_R3,
		LS_R4
	};
	// �����N���̂����̎��
	enum {
		LA_NONE,
		LA_SINGLE,
		LA_DOUBLE,
		LA_DEPEND_SINGLE,
		LA_DEPEND_DOUBLE,
		LA_INHERIT,
		LA_AGGREGAT,
		LA_COMPOSIT
	};

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CiEditApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();
	DECLARE_MESSAGE_MAP()
protected:
	void loadmfs(CArchive& ar);
private:
	CMultiDocTemplate* m_pDocTemplate;
	CMultiDocTemplate* m_pDocTemplate2;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_IEDIT_H__96DFF9B7_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
