// NodeSearchDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "NodeSearchDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NodeSearchDlg ダイアログ

NodeSearchDlg::NodeSearchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(NodeSearchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(NodeSearchDlg)
	m_bLabel = FALSE;
	m_bLinks = FALSE;
	m_bText = FALSE;
	m_bUpper = FALSE;
	//}}AFX_DATA_INIT
}


void NodeSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NodeSearchDlg)
	DDX_Control(pDX, IDC_CHKUPPER, m_ckUpper);
	DDX_Control(pDX, IDC_BTNGO, m_btnGo);
	DDX_Control(pDX, IDC_START, m_btnStart);
	DDX_Control(pDX, IDC_COMBO, m_combSrch);
	DDX_Control(pDX, IDC_LIST, m_lcResult);
	DDX_Control(pDX, IDC_CHKTEXT, m_ckText);
	DDX_Control(pDX, IDC_CHKLINKS, m_ckLiinks);
	DDX_Control(pDX, IDC_CHKLABEL, m_chLabel);
	DDX_Check(pDX, IDC_CHKLABEL, m_bLabel);
	DDX_Check(pDX, IDC_CHKLINKS, m_bLinks);
	DDX_Check(pDX, IDC_CHKTEXT, m_bText);
	DDX_Check(pDX, IDC_CHKUPPER, m_bUpper);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(NodeSearchDlg, CDialog)
	//{{AFX_MSG_MAP(NodeSearchDlg)
	ON_COMMAND(IDOK, OnOk)
	ON_BN_CLICKED(IDC_START, OnStart)
	ON_BN_CLICKED(IDC_BTNGO, OnBtngo)
	ON_BN_CLICKED(IDC_CHKLABEL, OnChklabel)
	ON_BN_CLICKED(IDC_CHKLINKS, OnChklinks)
	ON_BN_CLICKED(IDC_CHKTEXT, OnChktext)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, OnDblclkList)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST, OnGetdispinfoList)
	ON_BN_CLICKED(IDC_CHKUPPER, OnChkupper)
	ON_WM_SIZE()
	ON_CBN_EDITCHANGE(IDC_COMBO, OnEditchangeCombo)
	ON_CBN_SELCHANGE(IDC_COMBO, OnSelchangeCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL NodeSearchDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	m_pParent = pParentWnd;

	return CDialog::Create(IDD, pParentWnd);
}

void NodeSearchDlg::OnOk()
{
	return;
}

void NodeSearchDlg::OnCancel()
{
	m_pParent->PostMessage(WM_CLOSESRCHWINDOW, IDCANCEL);
	m_pParent->SetFocus();
	CDialog::OnCancel();
}

void NodeSearchDlg::OnStart()
{
	m_combSrch.GetWindowText(m_srchString);
	if (m_srchString == _T("")) return;
	m_btnStart.EnableWindow(FALSE);
	m_lcResult.DeleteAllItems();
	m_labels.clear();
	m_labels.resize(0);
	m_pParent->PostMessage(WM_LISTUPNODES, 0);
}

void NodeSearchDlg::OnBtngo()
{
	srchNode();
}

void NodeSearchDlg::srchNode()
{
	int index = m_lcResult.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (index == -1) return;
	m_pParent->PostMessage(WM_SRCHNODE, m_labels[index].key/* ノードID*/);
	m_pParent->SetFocus();
}

void NodeSearchDlg::OnChklabel()
{
	m_bLabel = m_chLabel.GetCheck();
}

void NodeSearchDlg::OnChklinks()
{
	m_bLinks = m_ckLiinks.GetCheck();
}

void NodeSearchDlg::OnChktext()
{
	m_bText = m_ckText.GetCheck();
}

BOOL NodeSearchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_lcResult.InsertColumn(0, _T("項目"));
	m_lcResult.InsertColumn(1, _T("種別"));
	CRect rc; m_lcResult.GetClientRect(&rc);
	m_lcResult.SetColumnWidth(0, rc.Width() - 70);
	m_lcResult.SetColumnWidth(1, 70);
	m_chLabel.SetCheck(TRUE);
	m_bLabel = TRUE;
	m_ckText.SetCheck(TRUE);
	m_bText = TRUE;
	m_ckLiinks.SetCheck(TRUE);
	m_bLinks = TRUE;
	m_ckUpper.SetCheck(TRUE);
	m_bUpper = TRUE;
	GetWindowRect(&rc);
	CRect crc; GetClientRect(crc);
	int cy = rc.Height() - crc.Height();
	m_lcResult.GetWindowRect(m_iniLCRect);
	m_iniLCRect.OffsetRect(-rc.left, -rc.top - cy);
	return TRUE;
}

void NodeSearchDlg::ShowResult()
{
	m_lcResult.DeleteAllItems();
	for (unsigned int i = 0; i < m_labels.size(); i++) {
		LV_ITEM lvi;
		lvi.iItem = i;
		lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		lvi.iSubItem = 0;
		lvi.lParam = (LPARAM)i;
		lvi.pszText = LPSTR_TEXTCALLBACK;
		m_lcResult.InsertItem(&lvi);
	}
	m_btnStart.EnableWindow(TRUE);
	if (m_combSrch.FindStringExact(-1, m_srchString) == CB_ERR) {
		m_combSrch.InsertString(0, m_srchString);
	}
	if (m_labels.size() > 0) {
		m_lcResult.SetItemState(0, LVIS_SELECTED | LVIS_FOCUSED, LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE);
	}
}

void NodeSearchDlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult)
{
	srchNode();

	*pResult = 0;
}

void NodeSearchDlg::OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if (pDispInfo->item.mask & LVIF_TEXT) {
		int index = (int)pDispInfo->item.lParam;
		switch (pDispInfo->item.iSubItem) {
		case 0:
			::lstrcpy(pDispInfo->item.pszText, m_labels[index].name);
			break;
		case 1:
			int state = m_labels[index].state;
			if (state == 0) {
				::lstrcpy(pDispInfo->item.pszText, _T("label"));
			}
			else if (state == 1) {
				::lstrcpy(pDispInfo->item.pszText, _T("text"));
			}
			else if (state == 2) {
				::lstrcpy(pDispInfo->item.pszText, _T("link"));
			}
			else if (state == 3) {
				::lstrcpy(pDispInfo->item.pszText, _T("URL"));
			}
			break;
		}
	}

	*pResult = 0;
}

void NodeSearchDlg::OnChkupper()
{
	m_bUpper = m_ckUpper.GetCheck();
}

void NodeSearchDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	CRect rc;
	GetClientRect(rc);
	CRect newrc(20, m_iniLCRect.top, rc.right - 15, rc.bottom - 15);

	m_lcResult.MoveWindow(newrc);
	m_lcResult.SetColumnWidth(0, newrc.Width() - 70);
	m_lcResult.SetColumnWidth(1, 70);
}

void NodeSearchDlg::OnEditchangeCombo()
{
	m_combSrch.GetWindowText(m_srchString);
	m_lcResult.DeleteAllItems();
	if (m_srchString == _T("")) return;
	m_btnStart.EnableWindow(FALSE);
	m_labels.clear();
	m_labels.resize(0);
	m_pParent->PostMessage(WM_LISTUPNODES, 0);
}

void NodeSearchDlg::OnSelchangeCombo()
{
	m_combSrch.GetLBText(m_combSrch.GetCurSel(), m_srchString);
	m_lcResult.DeleteAllItems();
	if (m_srchString == _T("")) return;
	m_btnStart.EnableWindow(FALSE);
	m_labels.clear();
	m_labels.resize(0);
	m_pParent->PostMessage(WM_LISTUPNODES, 0);
}
