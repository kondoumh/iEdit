// NodePropDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "NodePropDlg.h"
#include "iNode.h"
#include "SetMarginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// NodePropDlg ダイアログ


NodePropDlg::NodePropDlg(CWnd* pParent /*=NULL*/)
	: CDialog(NodePropDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(NodePropDlg)
	m_shape = -1;
	m_TLine = -1;
	m_bNoBrush = FALSE;
	//}}AFX_DATA_INIT
	bOldBynary = FALSE;
}


void NodePropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(NodePropDlg)
	DDX_Control(pDX, IDC_COMBO_HORIZ, m_cmbHoriz);
	DDX_Control(pDX, IDC_COMBO_VERT, m_cmbVert);
	DDX_Control(pDX, IDC_CHK_NO_BRS, m_chkBoxNoBrs);
	DDX_Control(pDX, IDC_BTN_BRS, m_BtnBrsColor);
	DDX_Control(pDX, IDC_COMBOLINE, m_CombTLine);
	DDX_Control(pDX, IDC_EDIT_LABEL, m_editLabel);
	DDX_Radio(pDX, IDC_RADIO_SHAPE, m_shape);
	DDX_Radio(pDX, IDC_RADIO_TLINE1, m_TLine);
	DDX_Check(pDX, IDC_CHK_NO_BRS, m_bNoBrush);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(NodePropDlg, CDialog)
	//{{AFX_MSG_MAP(NodePropDlg)
	ON_CBN_SELCHANGE(IDC_COMBOLINE, OnSelchangeComboline)
	ON_BN_CLICKED(IDC_BTN_LINE_COLOR, OnBtnLineColor)
	ON_BN_CLICKED(IDC_BTN_BRS, OnBtnBrs)
	ON_BN_CLICKED(IDC_BTN_FONT, OnBtnFont)
	ON_BN_CLICKED(IDC_RADIO_TLINE1, OnRadioTline1)
	ON_BN_CLICKED(IDC_RADIO_TLINE2, OnRadioTline2)
	ON_BN_CLICKED(IDC_RADIO_SHAPE, OnRadioShape)
	ON_BN_CLICKED(IDC_RADIO_SHAPE2, OnRadioShape2)
	ON_BN_CLICKED(IDC_CHK_NO_BRS, OnChkNoBrs)
	ON_BN_CLICKED(IDC_RADIO_TLINE3, OnRadioTline3)
	ON_CBN_SELCHANGE(IDC_COMBO_HORIZ, OnSelchangeComboHoriz)
	ON_CBN_SELCHANGE(IDC_COMBO_VERT, OnSelchangeComboVert)
	ON_BN_CLICKED(IDC_RADIO_SHAPE3, OnRadioShape3)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_SET_MARGIN, &NodePropDlg::OnBnClickedBtnSetMargin)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// NodePropDlg メッセージ ハンドラ

void NodePropDlg::OnSelchangeComboline() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	int index = m_CombTLine.GetCurSel();
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

void NodePropDlg::OnBtnLineColor() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CColorDialog dlg(colorLine);
	if (dlg.DoModal() != IDOK) return;
	colorLine = dlg.GetColor();
}

void NodePropDlg::OnBtnBrs() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CColorDialog dlg(colorFill);
	if (dlg.DoModal() != IDOK) return;
	colorFill = dlg.GetColor();
}

void NodePropDlg::OnBtnFont() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	CFontDialog dlg(&lf);
	dlg.m_cf.rgbColors = colorFont;
	if (dlg.DoModal() != IDOK) return;
	colorFont = dlg.m_cf.rgbColors;
}

BOOL NodePropDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	m_editLabel.ReplaceSel(m_strLabel);
	if (styleLine == PS_DOT) {
		m_CombTLine.SetCurSel(5);
	} else if (styleLine == PS_NULL) {
		m_CombTLine.SetCurSel(6);
	} else {
		if (lineWidth == 0) {
			m_CombTLine.SetCurSel(0);
		} else {
			m_CombTLine.SetCurSel(lineWidth - 1);
		}
	}
	m_cmbHoriz.SetCurSel(horiz);
	m_cmbVert.SetCurSel(vert);
	
	m_BtnBrsColor.EnableWindow(!m_chkBoxNoBrs.GetCheck());
	if (m_TLine == 0) {
		m_editLabel.ModifyStyle(ES_MULTILINE | ES_WANTRETURN | WS_VSCROLL | ES_AUTOVSCROLL | WS_HSCROLL, ES_AUTOHSCROLL);
		m_editLabel.MoveWindow(86,11 ,263,25);
		m_cmbHoriz.EnableWindow(TRUE);
		m_cmbVert.EnableWindow(TRUE);
	} else if (m_TLine == 1) {
		m_cmbHoriz.EnableWindow(TRUE);
		m_cmbVert.EnableWindow(FALSE);
		m_editLabel.ModifyStyle( ES_AUTOHSCROLL, WS_HSCROLL | ES_MULTILINE | ES_WANTRETURN | WS_VSCROLL | ES_AUTOVSCROLL | WS_HSCROLL);
		m_editLabel.MoveWindow(86,11,263,65);
	} else if (m_TLine == 2) {
		m_editLabel.ModifyStyle(ES_MULTILINE | ES_WANTRETURN | WS_VSCROLL | ES_AUTOVSCROLL | WS_HSCROLL , ES_AUTOHSCROLL);
		m_editLabel.MoveWindow(86,11,263,25);
		m_cmbHoriz.EnableWindow(FALSE);
		m_cmbVert.EnableWindow(FALSE);
	}
	
	if (bOldBynary) {
		GetDlgItem(IDC_BTN_SET_MARGIN)->EnableWindow(FALSE);
	}
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}


void NodePropDlg::OnRadioTline1() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_editLabel.ModifyStyle(ES_MULTILINE | ES_WANTRETURN | WS_VSCROLL | ES_AUTOVSCROLL | WS_HSCROLL, ES_AUTOHSCROLL);
	m_editLabel.MoveWindow(86,11,263,25);
	m_TLine = 0;
	m_cmbHoriz.EnableWindow(TRUE);
	m_cmbVert.EnableWindow(TRUE);
}

void NodePropDlg::OnRadioTline2() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_editLabel.ModifyStyle( ES_AUTOHSCROLL, ES_MULTILINE | ES_WANTRETURN | WS_VSCROLL | ES_AUTOVSCROLL | WS_HSCROLL);
	m_editLabel.MoveWindow(86,11,263,65);
	m_TLine = 1;
	m_cmbHoriz.EnableWindow(TRUE);
	m_cmbVert.EnableWindow(FALSE);
}

void NodePropDlg::OnRadioTline3() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_editLabel.ModifyStyle(ES_MULTILINE | ES_WANTRETURN | WS_VSCROLL | ES_AUTOVSCROLL | WS_HSCROLL, ES_AUTOHSCROLL);
	m_editLabel.MoveWindow(86,11,263,25);
	m_TLine = 2;
	m_cmbHoriz.EnableWindow(FALSE);
	m_cmbVert.EnableWindow(FALSE);
}

void NodePropDlg::OnRadioShape() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_shape = 0;
}

void NodePropDlg::OnRadioShape2() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_shape = 1;
}

void NodePropDlg::OnRadioShape3() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_shape = 2;
}

void NodePropDlg::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
	m_editLabel.GetWindowText(m_strLabel);
	CDialog::OnOK();
}

void NodePropDlg::OnChkNoBrs() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_BtnBrsColor.EnableWindow(!m_chkBoxNoBrs.GetCheck());
}

void NodePropDlg::OnSelchangeComboHoriz() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	horiz = m_cmbHoriz.GetCurSel();
}

void NodePropDlg::OnSelchangeComboVert() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	vert = m_cmbVert.GetCurSel();
}

void NodePropDlg::OnBnClickedBtnSetMargin()
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
