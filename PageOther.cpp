// PageOther.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "PageOther.h"
#include "SetMFSizeDlg.h"
#include "MfSizer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGS_OTHER _T("Settings")

/////////////////////////////////////////////////////////////////////////////
// PageOther ダイアログ


PageOther::PageOther(CWnd* pParent /*=NULL*/)
	: CDialog(PageOther::IDD, pParent)
{
	//{{AFX_DATA_INIT(PageOther)
	m_tabSelect = -1;
	m_bShowHS = FALSE;
	m_bInheritParent = FALSE;
	m_bInheritSibling = FALSE;
	m_bAccelmove = FALSE;
	m_bDrawUnderLine = FALSE;
	m_bSetStylesheet = FALSE;
	m_strStylesheet = _T("");
	//}}AFX_DATA_INIT
}


void PageOther::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PageOther)
	DDX_Control(pDX, IDC_CHK_SET_STYLESHEET, m_chkStylesheet);
	DDX_Control(pDX, IDC_EDIT_STYLESHEET, m_edStylesheet);
	DDX_Control(pDX, IDC_CHK_DRW_UNDRLN, m_chkDrawUnderLine);
	DDX_Control(pDX, IDC_CHK_ACCEL, m_chkAccelMove);
	DDX_Control(pDX, IDC_CHK_INHERIT_SIBLING, m_chkInheritSibling);
	DDX_Control(pDX, IDC_CHK_INHERIT_PARENT, m_chkInheritParent);
	DDX_Control(pDX, IDC_CHK_SHOW_HS, m_chkShowHS);
	DDX_Radio(pDX, IDC_RD_TAB1, m_tabSelect);
	DDX_Check(pDX, IDC_CHK_SHOW_HS, m_bShowHS);
	DDX_Check(pDX, IDC_CHK_INHERIT_PARENT, m_bInheritParent);
	DDX_Check(pDX, IDC_CHK_INHERIT_SIBLING, m_bInheritSibling);
	DDX_Check(pDX, IDC_CHK_ACCEL, m_bAccelmove);
	DDX_Check(pDX, IDC_CHK_DRW_UNDRLN, m_bDrawUnderLine);
	DDX_Check(pDX, IDC_CHK_SET_STYLESHEET, m_bSetStylesheet);
	DDX_Text(pDX, IDC_EDIT_STYLESHEET, m_strStylesheet);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PageOther, CDialog)
	//{{AFX_MSG_MAP(PageOther)
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
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PageOther メッセージ ハンドラ

void PageOther::OnCancel() 
{
	// TODO: この位置に特別な後処理を追加してください。
	return;
	CDialog::OnCancel();
}

BOOL PageOther::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	m_edStylesheet.EnableWindow(m_bSetStylesheet);
	
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void PageOther::OnRdTab1() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_tabSelect = 0;
}

void PageOther::OnRdTab2() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_tabSelect = 1;
}

void PageOther::OnRdTab3() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_tabSelect = 2;
}

void PageOther::OnChkShowHs() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_bShowHS = m_chkShowHS.GetCheck();
}

void PageOther::OnChkInheritParent() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_bInheritParent = m_chkInheritParent.GetCheck();
}

void PageOther::OnChkInheritSibling() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_bInheritSibling = m_chkInheritSibling.GetCheck();
}

void PageOther::OnChkAccel() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_bAccelmove = m_chkAccelMove.GetCheck();
}

void PageOther::OnBtnSetmfsize() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	SetMFSizeDlg dlg;
	double mfWidth = AfxGetApp()->GetProfileInt(REGS_OTHER, _T("MF rWidth"), 0)/10.0;
	double mfHeight = AfxGetApp()->GetProfileInt(REGS_OTHER, _T("MF rHeight"), 0)/10.0;
	
	
	if (mfWidth != 0 && mfHeight != 0) {
		dlg.m_rx = mfWidth;
		dlg.m_ry = mfHeight;
	} else {
		CSize szMF = MfSizer::getMFSize();
		dlg.m_rx = ((double)szMF.cx)/10.0;
		dlg.m_ry = ((double)szMF.cy)/10.0;;
	}
	
	if (dlg.DoModal() == IDOK) {
		AfxGetApp()->WriteProfileInt(REGS_OTHER, _T("MF rWidth"), (int)(dlg.m_rx*10));
		AfxGetApp()->WriteProfileInt(REGS_OTHER, _T("MF rHeight"), (int)(dlg.m_ry*10));
		MessageBox(_T("この設定は再起動後に反映されます"));
	}
}

void PageOther::OnChkDrwUndrln() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_bDrawUnderLine = m_chkDrawUnderLine.GetCheck();
}

void PageOther::OnChkSetStylesheet() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_bSetStylesheet = m_chkStylesheet.GetCheck();
	m_edStylesheet.EnableWindow(m_bSetStylesheet);
}

void PageOther::OnChangeEditStylesheet() 
{
	// TODO: これが RICHEDIT コントロールの場合、コントロールは、 lParam マスク
	// 内での論理和の ENM_CHANGE フラグ付きで CRichEditCrtl().SetEventMask()
	// メッセージをコントロールへ送るために CDialog::OnInitDialog() 関数をオーバー
	// ライドしない限りこの通知を送りません。
	
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_edStylesheet.GetWindowText(m_strStylesheet);
	
}
