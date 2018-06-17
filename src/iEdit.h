// iEdit.h : IEDIT アプリケーションのメイン ヘッダー ファイル
//

#if !defined(AFX_IEDIT_H__96DFF9B7_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
#define AFX_IEDIT_H__96DFF9B7_1881_11D3_808A_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // メイン シンボル

// レジストリ情報格納用変数
struct RegValueNode {
	LOGFONT lf;
	int styleText;
	int styleLine;
	int lineWidth;
	int shape;
	COLORREF colorFill;
	COLORREF colorLine;
	COLORREF colorFont;
	BOOL bFillColor;
	BOOL bInheritParent;    // 親ノードの属性を継承
	BOOL bInheritSibling;   // 兄弟ノードの属性を継承
	BOOL bSyncOrder;
	int  orderDirection;
	BOOL bEnableGroup;
	BOOL bDisableNodeResize;
	BOOL bPriorSelectionDragging;
	int margin_l;
	int margin_r;
	int margin_t;
	int margin_b;
};

struct RegValueLink {
	LOGFONT lf;
	int lineWidth;
	int styleLine;
	int strength;
	COLORREF colorLine;
	BOOL bSetStrength;
};

struct RegValueMisc {
	BOOL bSetStylesheet;
	CString strStyleSheetFile;
	BOOL bOpenFilesAfterExport;
	BOOL bOutputFileLinksOnExport;
};

struct RegValueFileType {
	BOOL registFiletype;
	BOOL registOldFiletype;
};

typedef map<int, HENHMETAFILE> HMetaFiles;

void showLastErrorMessage();


/////////////////////////////////////////////////////////////////////////////
// CiEditApp:

class CiEditApp : public CWinApp
{
public:
	COLORREF m_colorNodeBtn;
	COLORREF m_colorLineBtn;
	COLORREF m_colorFontBtn;
	COLORREF m_colorOutlineViewBg;
	COLORREF m_colorOutlineViewFg;
	COLORREF m_colorLinkViewBg;
	COLORREF m_colorLinkViewFg;
	COLORREF m_colorTextViewBg;
	COLORREF m_colorTextViewFg;
	COLORREF m_colorNetViewBg;
	int m_curLinkArrow;
	int m_curLinkLineStyle;
	void getOtherProfile();
	void notifyViewSetting();
	bool m_bShapeModified;
	void saveMetaFiles(const CString& fname);
	void loadMetaFiles(const CString& fname);
	void getLinkProfile();
	void getNodeProfile();
	void getOptionsProfile();
	RegValueNode m_rgsNode;
	RegValueLink m_rgsLink;
	RegValueMisc m_rgsOther;
	RegValueFileType m_rgsOptions;
	HMetaFiles m_hMetaFiles;
	CString m_mfCategories[10];
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CiEditApp();

	// リンク線の種類
	enum {
		LS_R0,
		LS_DOT,
		LS_R1,
		LS_R2,
		LS_R3,
		LS_R4
	};
	// リンク矢印のつき方の種類
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

protected:

	//{{AFX_VIRTUAL(CiEditApp)
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	afx_msg void OnFileNew();
	afx_msg void OnFiletypeRegDel();
	DECLARE_MESSAGE_MAP()

private:
	void LoadShapeMetafile(CArchive& ar);
	CMultiDocTemplate* m_pDocTemplate;
	CMultiDocTemplate* m_pDocTemplate2;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_IEDIT_H__96DFF9B7_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
