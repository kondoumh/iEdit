// OptionPageForNode.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "OptionPageForNode.h"
#include "iNode.h"
#include "NodeMarginSettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OptionPageForNode ダイアログ


OptionPageForNode::OptionPageForNode(CWnd* pParent /*=NULL*/)
	: CDialog(OptionPageForNode::IDD, pParent)
	, m_rdAscending(0)
	, m_rdDescending(0)
{
	//{{AFX_DATA_INIT(OptionPageForNode)
	m_rdTLine = -1;
	m_rdShape = -1;
	m_bNoBrs = FALSE;
	//}}AFX_DATA_INIT
}


void OptionPageForNode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(OptionPageForNode)
	DDX_Control(pDX, IDC_COMBO_VERT, m_cmbVert);
	DDX_Control(pDX, IDC_COMBO_HORIZ, m_cmbHoriz);
	DDX_Control(pDX, IDC_CHK_NO_BRS, m_chkNoBrs);
	DDX_Control(pDX, IDC_BTN_BRS, m_BtnBrsColor);
	DDX_Control(pDX, IDC_COMBOLINE, m_cmbLineStyle);
	DDX_Radio(pDX, IDC_RADIO_TLINE1, m_rdTLine);
	DDX_Radio(pDX, IDC_RADIO_SHAPE, m_rdShape);
	DDX_Radio(pDX, IDC_RADIO_ASCENDING, m_orderDirection);
	DDX_Check(pDX, IDC_CHK_NO_BRS, m_bNoBrs);
	DDX_Control(pDX, IDC_CHK_SYNC_ORDER, m_chkSyncOrder);
	DDX_Control(pDX, IDC_CHK_ENABLE_GROUPING, m_chkEnableGroup);
	DDX_Control(pDX, IDC_RADIO_ASCENDING, m_btnAscending);
	DDX_Control(pDX, IDC_RADIO_DESCENDING, m_btnDescending);
	DDX_Control(pDX, IDC_DISABLE_NODE_RESIZE, m_ChkDisableNodeResize);
	DDX_Control(pDX, IDC_PRIOR_SELECTION_DRAGGING, m_chkPriorSelectionDragging);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(OptionPageForNode, CDialog)
	//{{AFX_MSG_MAP(OptionPageForNode)
	ON_BN_CLICKED(IDC_BTN_LINE_COLOR, OnBtnLineColor)
	ON_BN_CLICKED(IDC_BTN_BRS, OnBtnBrs)
	ON_BN_CLICKED(IDC_BTN_FONT, OnBtnFont)
	ON_BN_CLICKED(IDC_RADIO_TLINE1, OnRadioTline1)
	ON_BN_CLICKED(IDC_RADIO_TLINE2, OnRadioTline2)
	ON_BN_CLICKED(IDC_RADIO_SHAPE, OnRadioShape)
	ON_BN_CLICKED(IDC_RADIO_SHAPE2, OnRadioShape2)
	ON_BN_CLICKED(IDC_CHK_NO_BRS, OnChkNoBrs)
	ON_CBN_SELCHANGE(IDC_COMBOLINE, OnSelchangeComboline)
	ON_BN_CLICKED(IDC_RADIO_TLINE3, OnRadioTline3)
	ON_CBN_SELCHANGE(IDC_COMBO_HORIZ, OnSelchangeComboHoriz)
	ON_CBN_SELCHANGE(IDC_COMBO_VERT, OnSelchangeComboVert)
	ON_BN_CLICKED(IDC_RADIO_SHAPE3, OnRadioShape3)
	ON_BN_CLICKED(IDC_CHK_SYNC_ORDER, &OptionPageForNode::OnBnClickedChkSyncOrder)
	ON_BN_CLICKED(IDC_CHK_ENABLE_GROUPING, &OptionPageForNode::OnBnClickedChkEnableGrouping)
	ON_BN_CLICKED(IDC_RADIO_ASCENDING, &OptionPageForNode::OnBnClickedRadioAscending)
	ON_BN_CLICKED(IDC_RADIO_DESCENDING, &OptionPageForNode::OnBnClickedRadioDescending)
	ON_BN_CLICKED(IDC_DISABLE_NODE_RESIZE, &OptionPageForNode::OnBnClickedDisableNodeResize)
	ON_BN_CLICKED(IDC_BTN_SET_MARGIN, &OptionPageForNode::OnBnClickedBtnSetMargin)
	ON_BN_CLICKED(IDC_PRIOR_SELECTION_DRAGGING, &OptionPageForNode::OnBnClickedPriorSelectionDragging)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OptionPageForNode メッセージ ハンドラ

void OptionPageForNode::OnCancel() 
{
	// TODO: この位置に特別な後処理を追加してください。
	return;
	CDialog::OnCancel();
}

BOOL OptionPageForNode::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	if (styleLine == PS_DOT) {
		m_cmbLineStyle.SetCurSel(5);
	} else if (styleLine == PS_NULL) {
		m_cmbLineStyle.SetCurSel(6);
	} else {
		if (lineWidth == 0) {
			m_cmbLineStyle.SetCurSel(0);
		} else {
			m_cmbLineStyle.SetCurSel(lineWidth - 1);
		}
	}
	
	m_cmbHoriz.SetCurSel(horiz);
	m_cmbVert.SetCurSel(vert);
	
	m_chkNoBrs.SetCheck(m_bNoBrs);
	m_BtnBrsColor.EnableWindow(!m_chkNoBrs.GetCheck());
	
	if (m_rdTLine == 0) {
		m_cmbHoriz.EnableWindow(TRUE);
		m_cmbVert.EnableWindow(TRUE);
	} else if (m_rdTLine == 1) {
		m_cmbHoriz.EnableWindow(TRUE);
		m_cmbVert.EnableWindow(FALSE);
	} else if (m_rdTLine == 2) {
		m_cmbHoriz.EnableWindow(FALSE);
		m_cmbVert.EnableWindow(FALSE);
	}
	
	m_chkSyncOrder.SetCheck(m_bSyncOrder);
	m_btnAscending.EnableWindow(m_bSyncOrder);
	m_btnDescending.EnableWindow(m_bSyncOrder);
	m_chkEnableGroup.SetCheck(m_bEnableGroup);
	m_ChkDisableNodeResize.SetCheck(m_bDisableNodeResize);
	m_chkPriorSelectionDragging.SetCheck(m_bPriorSelectionDragging);
	
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void OptionPageForNode::OnBtnLineColor() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CColorDialog dlg(colorLine);
	if (dlg.DoModal() != IDOK) return;
	colorLine = dlg.GetColor();
}

void OptionPageForNode::OnBtnBrs() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CColorDialog dlg(colorFill);
	if (dlg.DoModal() != IDOK) return;
	colorFill = dlg.GetColor();
}

void OptionPageForNode::OnBtnFont() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CFontDialog dlg(&lf);
	//dlg.m_cf.Flags |= CF_SELECTSCRIPT;
	dlg.m_cf.rgbColors = colorFont;
	if (dlg.DoModal() != IDOK) return;
	colorFont = dlg.m_cf.rgbColors;
}

void OptionPageForNode::OnRadioTline1() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_rdTLine = 0;
	m_cmbHoriz.EnableWindow(TRUE);
	m_cmbVert.EnableWindow(TRUE);
}

void OptionPageForNode::OnRadioTline2() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_rdTLine = 1;
	m_cmbHoriz.EnableWindow(TRUE);
	m_cmbVert.EnableWindow(FALSE);
}

void OptionPageForNode::OnRadioTline3() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_rdTLine = 2;	
	m_cmbHoriz.EnableWindow(FALSE);
	m_cmbVert.EnableWindow(FALSE);
}

void OptionPageForNode::OnRadioShape() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_rdShape = 0;
}

void OptionPageForNode::OnRadioShape2() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_rdShape = 1;
}

void OptionPageForNode::OnRadioShape3() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_rdShape = 2;
}

void OptionPageForNode::OnChkNoBrs() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_bNoBrs = m_chkNoBrs.GetCheck();
	m_BtnBrsColor.EnableWindow(!m_chkNoBrs.GetCheck());
}


void OptionPageForNode::OnSelchangeComboline() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	int index = m_cmbLineStyle.GetCurSel();
	if (index == 0 || index == 5 || index == 6) {
		lineWidth = 0;
	} else {
		lineWidth = index+1;
	}
	if (index < 5) {
		styleLine = PS_SOLID;
	} else if (index == 5) {
		styleLine = PS_DOT;
	} else if (index == 6) {
		styleLine = PS_NULL;
	}
}

void OptionPageForNode::OnSelchangeComboHoriz() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	horiz = m_cmbHoriz.GetCurSel();
}

void OptionPageForNode::OnSelchangeComboVert() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	vert = m_cmbVert.GetCurSel();
}

void OptionPageForNode::OnBnClickedChkSyncOrder()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	m_bSyncOrder = m_chkSyncOrder.GetCheck();
	m_btnAscending.EnableWindow(m_bSyncOrder);
	m_btnDescending.EnableWindow(m_bSyncOrder);
}

void OptionPageForNode::OnBnClickedChkEnableGrouping()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	m_bEnableGroup = m_chkEnableGroup.GetCheck();
}

void OptionPageForNode::OnBnClickedRadioAscending()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	m_orderDirection = 0;
}

void OptionPageForNode::OnBnClickedRadioDescending()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	m_orderDirection = 1;
}

void OptionPageForNode::OnBnClickedDisableNodeResize()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	m_bDisableNodeResize = m_ChkDisableNodeResize.GetCheck();
}

void OptionPageForNode::OnBnClickedBtnSetMargin()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	NodeMarginSettingsDlg dlg;
	dlg.m_nLeft = margins.l;
	dlg.m_nRight = margins.r;
	dlg.m_nTop = margins.t;
	dlg.m_nBottom = margins.b;
	if (dlg.DoModal() != IDOK) return;
	margins.l = dlg.m_nLeft;
	margins.r = dlg.m_nRight;
	margins.t = dlg.m_nTop;
	margins.b = dlg.m_nBottom;
}


void OptionPageForNode::OnBnClickedPriorSelectionDragging()
{
	// TODO: ここにコントロール通知ハンドラー コードを追加します。
	m_bPriorSelectionDragging = m_chkPriorSelectionDragging.GetCheck();
}
