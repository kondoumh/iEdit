// PageNode.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "iEdit.h"
#include "PageNode.h"
#include "iNode.h"
#include "SetMarginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PageNode �_�C�A���O


PageNode::PageNode(CWnd* pParent /*=NULL*/)
	: CDialog(PageNode::IDD, pParent)
	, m_rdAscending(0)
	, m_rdDescending(0)
{
	//{{AFX_DATA_INIT(PageNode)
	m_rdTLine = -1;
	m_rdShape = -1;
	m_bNoBrs = FALSE;
	//}}AFX_DATA_INIT
}


void PageNode::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PageNode)
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
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_DISABLE_NODE_RESIZE, m_ChkDisableNodeResize);
}


BEGIN_MESSAGE_MAP(PageNode, CDialog)
	//{{AFX_MSG_MAP(PageNode)
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
	ON_BN_CLICKED(IDC_CHK_SYNC_ORDER, &PageNode::OnBnClickedChkSyncOrder)
	ON_BN_CLICKED(IDC_CHK_ENABLE_GROUPING, &PageNode::OnBnClickedChkEnableGrouping)
	ON_BN_CLICKED(IDC_RADIO_ASCENDING, &PageNode::OnBnClickedRadioAscending)
	ON_BN_CLICKED(IDC_RADIO_DESCENDING, &PageNode::OnBnClickedRadioDescending)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_DISABLE_NODE_RESIZE, &PageNode::OnBnClickedDisableNodeResize)
	ON_BN_CLICKED(IDC_BTN_SET_MARGIN, &PageNode::OnBnClickedBtnSetMargin)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PageNode ���b�Z�[�W �n���h��

void PageNode::OnCancel() 
{
	// TODO: ���̈ʒu�ɓ��ʂȌ㏈����ǉ����Ă��������B
	return;
	CDialog::OnCancel();
}

BOOL PageNode::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: ���̈ʒu�ɏ������̕⑫������ǉ����Ă�������
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
	
	return TRUE;  // �R���g���[���Ƀt�H�[�J�X��ݒ肵�Ȃ��Ƃ��A�߂�l�� TRUE �ƂȂ�܂�
	              // ��O: OCX �v���p�e�B �y�[�W�̖߂�l�� FALSE �ƂȂ�܂�
}

void PageNode::OnBtnLineColor() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	CColorDialog dlg(colorLine);
	if (dlg.DoModal() != IDOK) return;
	colorLine = dlg.GetColor();
}

void PageNode::OnBtnBrs() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	CColorDialog dlg(colorFill);
	if (dlg.DoModal() != IDOK) return;
	colorFill = dlg.GetColor();
}

void PageNode::OnBtnFont() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	CFontDialog dlg(&lf);
	dlg.m_cf.Flags |= CF_SELECTSCRIPT;
	dlg.m_cf.rgbColors = colorFont;
	if (dlg.DoModal() != IDOK) return;
	colorFont = dlg.m_cf.rgbColors;
}

void PageNode::OnRadioTline1() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_rdTLine = 0;
	m_cmbHoriz.EnableWindow(TRUE);
	m_cmbVert.EnableWindow(TRUE);
}

void PageNode::OnRadioTline2() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_rdTLine = 1;
	m_cmbHoriz.EnableWindow(TRUE);
	m_cmbVert.EnableWindow(FALSE);
}

void PageNode::OnRadioTline3() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_rdTLine = 2;	
	m_cmbHoriz.EnableWindow(FALSE);
	m_cmbVert.EnableWindow(FALSE);
}

void PageNode::OnRadioShape() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_rdShape = 0;
}

void PageNode::OnRadioShape2() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_rdShape = 1;
}

void PageNode::OnRadioShape3() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_rdShape = 2;
}

void PageNode::OnChkNoBrs() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_bNoBrs = m_chkNoBrs.GetCheck();
	m_BtnBrsColor.EnableWindow(!m_chkNoBrs.GetCheck());
}


void PageNode::OnSelchangeComboline() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
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

void PageNode::OnSelchangeComboHoriz() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	horiz = m_cmbHoriz.GetCurSel();
}

void PageNode::OnSelchangeComboVert() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	vert = m_cmbVert.GetCurSel();
}

void PageNode::OnBnClickedChkSyncOrder()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	m_bSyncOrder = m_chkSyncOrder.GetCheck();
	m_btnAscending.EnableWindow(m_bSyncOrder);
	m_btnDescending.EnableWindow(m_bSyncOrder);
}

void PageNode::OnBnClickedChkEnableGrouping()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	m_bEnableGroup = m_chkEnableGroup.GetCheck();
}

void PageNode::OnBnClickedRadioAscending()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	m_orderDirection = 0;
}

void PageNode::OnBnClickedRadioDescending()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	m_orderDirection = 1;
}

void PageNode::OnBnClickedDisableNodeResize()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	m_bDisableNodeResize = m_ChkDisableNodeResize.GetCheck();
}

void PageNode::OnBnClickedBtnSetMargin()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	SetMarginDlg dlg;
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
