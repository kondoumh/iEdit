// nodeSrchDlg.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "iEdit.h"
#include "nodeSrchDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// nodeSrchDlg �_�C�A���O


nodeSrchDlg::nodeSrchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(nodeSrchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(nodeSrchDlg)
	m_bLabel = FALSE;
	m_bLinks = FALSE;
	m_bText = FALSE;
	m_bUpper = FALSE;
	//}}AFX_DATA_INIT
}


void nodeSrchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(nodeSrchDlg)
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


BEGIN_MESSAGE_MAP(nodeSrchDlg, CDialog)
	//{{AFX_MSG_MAP(nodeSrchDlg)
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
// nodeSrchDlg ���b�Z�[�W �n���h��

BOOL nodeSrchDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: ���̈ʒu�ɌŗL�̏�����ǉ����邩�A�܂��͊�{�N���X���Ăяo���Ă�������
	m_pParent = pParentWnd;
	
	return CDialog::Create(IDD, pParentWnd);
}

void nodeSrchDlg::OnOk() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	return;
//	m_pParent->PostMessage(WM_CLOSESRCHWINDOW, IDOK);
}

void nodeSrchDlg::OnCancel() 
{
	// TODO: ���̈ʒu�ɓ��ʂȌ㏈����ǉ����Ă��������B
	m_pParent->PostMessage(WM_CLOSESRCHWINDOW, IDCANCEL);
	m_pParent->SetFocus();
	CDialog::OnCancel();
}

void nodeSrchDlg::OnStart() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_combSrch.GetWindowText(m_srchString);
	if (m_srchString == "") return;
	m_btnStart.EnableWindow(FALSE);
	m_lcResult.DeleteAllItems();
	m_labels.clear();
	m_labels.resize(0);
	m_pParent->PostMessage(WM_LISTUPNODES, 0);
}

void nodeSrchDlg::OnBtngo() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	srchNode();
}

void nodeSrchDlg::srchNode()
{
	int index = m_lcResult.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (index == -1) return;
	m_pParent->PostMessage(WM_SRCHNODE, m_labels[index].key/* �m�[�hID*/);
	m_pParent->SetFocus();
}

void nodeSrchDlg::OnChklabel() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_bLabel = m_chLabel.GetCheck();
}

void nodeSrchDlg::OnChklinks() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_bLinks = m_ckLiinks.GetCheck();
}

void nodeSrchDlg::OnChktext() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_bText = m_ckText.GetCheck();
}

BOOL nodeSrchDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: ���̈ʒu�ɏ������̕⑫������ǉ����Ă�������
	m_lcResult.InsertColumn(0, "����");
	m_lcResult.InsertColumn(1, "���");
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
	return TRUE;  // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
	              // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}

void nodeSrchDlg::displayResult()
{
	m_lcResult.DeleteAllItems( );
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

void nodeSrchDlg::OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	srchNode();
	
	*pResult = 0;
}

void nodeSrchDlg::OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	
	if (pDispInfo->item.mask & LVIF_TEXT) {
		int index = (int)pDispInfo->item.lParam;
		switch (pDispInfo->item.iSubItem) {
		case 0:
			::lstrcpy(pDispInfo->item.pszText, m_labels[index].name);
			break;
		case 1:
			int state = m_labels[index].state;
			if (state == 0) {
				::lstrcpy(pDispInfo->item.pszText, "label");
			} else if (state == 1) {
				::lstrcpy(pDispInfo->item.pszText, "text");
			} else if (state == 2) {
				::lstrcpy(pDispInfo->item.pszText, "link");
			} else if (state == 3) {
				::lstrcpy(pDispInfo->item.pszText, "URL");
			}
			break;
		}
	}
	
	*pResult = 0;
}

void nodeSrchDlg::OnChkupper() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_bUpper = m_ckUpper.GetCheck();
}

void nodeSrchDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����Ă�������
	CRect rc;
	GetClientRect(rc);
	CRect newrc(20, m_iniLCRect.top, rc.right-15, rc.bottom - 15);
	
	m_lcResult.MoveWindow(newrc);
	m_lcResult.SetColumnWidth(0, newrc.Width()-70);
	m_lcResult.SetColumnWidth(1, 70);
}

void nodeSrchDlg::OnEditchangeCombo() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_combSrch.GetWindowText(m_srchString);
	m_lcResult.DeleteAllItems();
	if (m_srchString == "") return;
	m_btnStart.EnableWindow(FALSE);
	m_labels.clear();
	m_labels.resize(0);
	m_pParent->PostMessage(WM_LISTUPNODES, 0);
}

void nodeSrchDlg::OnSelchangeCombo() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_combSrch.GetLBText(m_combSrch.GetCurSel(), m_srchString);
	m_lcResult.DeleteAllItems();
	if (m_srchString == "") return;
	m_btnStart.EnableWindow(FALSE);
	m_labels.clear();
	m_labels.resize(0);
	m_pParent->PostMessage(WM_LISTUPNODES, 0);
}
