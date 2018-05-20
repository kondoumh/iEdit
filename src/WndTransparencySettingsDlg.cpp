// WndTransparencySettingsDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "WndTransparencySettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// WndTransparencySettingsDlg ダイアログ

WndTransparencySettingsDlg::WndTransparencySettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(WndTransparencySettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(WndTransparencySettingsDlg)
	//}}AFX_DATA_INIT
}


void WndTransparencySettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(WndTransparencySettingsDlg)
	DDX_Control(pDX, IDC_LB_LEVEL, m_lbLevel);
	DDX_Control(pDX, IDC_SLIDER_ALPHA, m_sldrAlpha);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(WndTransparencySettingsDlg, CDialog)
	//{{AFX_MSG_MAP(WndTransparencySettingsDlg)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL WndTransparencySettingsDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	m_pParent = pParentWnd;
	return CDialog::Create(IDD, pParentWnd);
}

BOOL WndTransparencySettingsDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_sldrAlpha.SetRange(50, 255);
	m_sldrAlpha.SetPos(m_nLevel);
	CString s; s.Format(_T("%d"), m_sldrAlpha.GetPos());
	m_lbLevel.SetWindowText(s);
	return TRUE;
}

void WndTransparencySettingsDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CString s; s.Format(_T("%d"), m_sldrAlpha.GetPos());
	m_lbLevel.SetWindowText(s);
	m_pParent->PostMessage(WM_SLIDEALPHA, 0, m_sldrAlpha.GetPos());
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
