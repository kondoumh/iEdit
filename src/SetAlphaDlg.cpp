// SetAlphaDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "SetAlphaDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SetAlphaDlg ダイアログ


SetAlphaDlg::SetAlphaDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SetAlphaDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SetAlphaDlg)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DATA_INIT
}


void SetAlphaDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SetAlphaDlg)
	DDX_Control(pDX, IDC_LB_LEVEL, m_lbLevel);
	DDX_Control(pDX, IDC_SLIDER_ALPHA, m_sldrAlpha);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SetAlphaDlg, CDialog)
	//{{AFX_MSG_MAP(SetAlphaDlg)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SetAlphaDlg メッセージ ハンドラ

BOOL SetAlphaDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	m_pParent = pParentWnd;
	return CDialog::Create(IDD, pParentWnd);
}

BOOL SetAlphaDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	m_sldrAlpha.SetRange(50, 255);
	m_sldrAlpha.SetPos(m_nLevel);
	CString s; s.Format(_T("%d"), m_sldrAlpha.GetPos());
	m_lbLevel.SetWindowText(s);
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void SetAlphaDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	CString s; s.Format(_T("%d"), m_sldrAlpha.GetPos());
	m_lbLevel.SetWindowText(s);
	m_pParent->PostMessage(WM_SLIDEALPHA, 0, m_sldrAlpha.GetPos());
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
