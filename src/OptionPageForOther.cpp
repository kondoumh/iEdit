// OptionPageForOther.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "OptionPageForOther.h"
#include "MetafileSettingsDlg.h"
#include "MfSizer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGS_OTHER _T("Settings")

/////////////////////////////////////////////////////////////////////////////
// OptionPageForOther ダイアログ

OptionPageForOther::OptionPageForOther(CWnd* pParent /*=NULL*/)
	: CDialog(OptionPageForOther::IDD, pParent)
{
	//{{AFX_DATA_INIT(OptionPageForOther)
	m_tabSelect = -1;
	m_bShowHS = FALSE;
	m_bInheritParent = FALSE;
	m_bInheritSibling = FALSE;
	m_bAccelmove = FALSE;
	m_bDrawUnderLine = FALSE;
	m_bOpenFilesAfterExport = TRUE;
	m_bSetStylesheet = FALSE;
	m_strStylesheet = _T("");
	//}}AFX_DATA_INIT
}

void OptionPageForOther::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OptionPageForOther)
	DDX_Control(pDX, IDC_CHK_SET_STYLESHEET, m_chkStylesheet);
	DDX_Control(pDX, IDC_EDIT_STYLESHEET, m_edStylesheet);
	DDX_Control(pDX, IDC_CHK_DRW_UNDRLN, m_chkDrawUnderLine);
	DDX_Control(pDX, IDC_CHK_ACCEL, m_chkAccelMove);
	DDX_Control(pDX, IDC_CHK_INHERIT_SIBLING, m_chkInheritSibling);
	DDX_Control(pDX, IDC_CHK_INHERIT_PARENT, m_chkInheritParent);
	DDX_Control(pDX, IDC_CHK_POST_ACTION, m_chkOpenFilesAfterExport);
	DDX_Control(pDX, IDC_CHK_EXPORT_FILELINK, m_chkOutputFileLinksOnExport);
	DDX_Control(pDX, IDC_CHK_SHOW_HS, m_chkShowHS);
	DDX_Radio(pDX, IDC_RD_TAB1, m_tabSelect);
	DDX_Check(pDX, IDC_CHK_SHOW_HS, m_bShowHS);
	DDX_Check(pDX, IDC_CHK_INHERIT_PARENT, m_bInheritParent);
	DDX_Check(pDX, IDC_CHK_INHERIT_SIBLING, m_bInheritSibling);
	DDX_Check(pDX, IDC_CHK_ACCEL, m_bAccelmove);
	DDX_Check(pDX, IDC_CHK_DRW_UNDRLN, m_bDrawUnderLine);
	DDX_Check(pDX, IDC_CHK_SET_STYLESHEET, m_bSetStylesheet);
	DDX_Check(pDX, IDC_CHK_POST_ACTION, m_bOpenFilesAfterExport);
	DDX_Check(pDX, IDC_CHK_EXPORT_FILELINK, m_bOutputFileLinksOnExport);
	DDX_Text(pDX, IDC_EDIT_STYLESHEET, m_strStylesheet);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(OptionPageForOther, CDialog)
	//{{AFX_MSG_MAP(OptionPageForOther)
	ON_BN_CLICKED(IDC_RD_TAB1, OnRdTab1)
	ON_BN_CLICKED(IDC_RD_TAB2, OnRdTab2)
	ON_BN_CLICKED(IDC_RD_TAB3, OnRdTab3)
	ON_BN_CLICKED(IDC_CHK_SHOW_HS, OnChkShowHs)
	ON_BN_CLICKED(IDC_CHK_INHERIT_PARENT, OnChkInheritParent)
	ON_BN_CLICKED(IDC_CHK_INHERIT_SIBLING, OnChkInheritSibling)
	ON_BN_CLICKED(IDC_CHK_ACCEL, OnChkAccel)
	ON_BN_CLICKED(IDC_BTN_SETMFSIZE, OnBtnSetmfsize)
	ON_BN_CLICKED(IDC_CHK_DRW_UNDRLN, OnChkDrwUndrln)
	ON_BN_CLICKED(IDC_CHK_SET_STYLESHEET, OnChkSetStylesheet)
	ON_EN_CHANGE(IDC_EDIT_STYLESHEET, OnChangeEditStylesheet)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CHK_POST_ACTION, &OptionPageForOther::OnBnClickedChkPostAction)
	ON_BN_CLICKED(IDC_CHK_EXPORT_FILELINK, &OptionPageForOther::OnBnClickedChkExportFilelink)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void OptionPageForOther::OnCancel()
{
	return;
	CDialog::OnCancel();
}

BOOL OptionPageForOther::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_edStylesheet.EnableWindow(m_bSetStylesheet);

	return TRUE;
}

void OptionPageForOther::OnRdTab1()
{
	m_tabSelect = 0;
}

void OptionPageForOther::OnRdTab2()
{
	m_tabSelect = 1;
}

void OptionPageForOther::OnRdTab3()
{
	m_tabSelect = 2;
}

void OptionPageForOther::OnChkShowHs()
{
	m_bShowHS = m_chkShowHS.GetCheck();
}

void OptionPageForOther::OnChkInheritParent()
{
	m_bInheritParent = m_chkInheritParent.GetCheck();
}

void OptionPageForOther::OnChkInheritSibling()
{
	m_bInheritSibling = m_chkInheritSibling.GetCheck();
}

void OptionPageForOther::OnChkAccel()
{
	m_bAccelmove = m_chkAccelMove.GetCheck();
}

void OptionPageForOther::OnBtnSetmfsize()
{
	MetafileSettingsDlg dlg;
	double mfWidth = AfxGetApp()->GetProfileInt(REGS_OTHER, _T("MF rWidth"), 0) / 10.0;
	double mfHeight = AfxGetApp()->GetProfileInt(REGS_OTHER, _T("MF rHeight"), 0) / 10.0;


	if (mfWidth != 0 && mfHeight != 0) {
		dlg.m_rx = mfWidth;
		dlg.m_ry = mfHeight;
	}
	else {
		CSize szMF = MfSizer::getMFSize();
		dlg.m_rx = ((double)szMF.cx) / 10.0;
		dlg.m_ry = ((double)szMF.cy) / 10.0;;
	}

	if (dlg.DoModal() == IDOK) {
		AfxGetApp()->WriteProfileInt(REGS_OTHER, _T("MF rWidth"), (int)(dlg.m_rx * 10));
		AfxGetApp()->WriteProfileInt(REGS_OTHER, _T("MF rHeight"), (int)(dlg.m_ry * 10));
		MessageBox(_T("この設定は再起動後に反映されます"));
	}
}

void OptionPageForOther::OnChkDrwUndrln()
{
	m_bDrawUnderLine = m_chkDrawUnderLine.GetCheck();
}

void OptionPageForOther::OnChkSetStylesheet()
{
	m_bSetStylesheet = m_chkStylesheet.GetCheck();
	m_edStylesheet.EnableWindow(m_bSetStylesheet);
}

void OptionPageForOther::OnChangeEditStylesheet()
{
	m_edStylesheet.GetWindowText(m_strStylesheet);

}

void OptionPageForOther::OnBnClickedChkPostAction()
{
	m_bOpenFilesAfterExport = m_chkOpenFilesAfterExport.GetCheck();
}

void OptionPageForOther::OnBnClickedChkExportFilelink()
{
	m_bOutputFileLinksOnExport = m_chkOutputFileLinksOnExport.GetCheck();
}
