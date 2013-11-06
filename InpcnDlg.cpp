// InpcnDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "InpcnDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_SETCHARDATA WM_USER + 12

/////////////////////////////////////////////////////////////////////////////
// CInpcnDlg ダイアログ


CInpcnDlg::CInpcnDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInpcnDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInpcnDlg)
	m_strcn = _T("");
	//}}AFX_DATA_INIT
}


void CInpcnDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInpcnDlg)
	DDX_Control(pDX, IDC_EDIT1, m_editName);
	DDX_Text(pDX, IDC_EDIT1, m_strcn);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInpcnDlg, CDialog)
	//{{AFX_MSG_MAP(CInpcnDlg)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETCHARDATA, OnSetCharData)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInpcnDlg メッセージ ハンドラ

BOOL CInpcnDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	GetWindowRect(&iniRect);
	if (m_iniPt.x != -1 && m_iniPt.y != -1) {
		MoveWindow(m_iniPt.x, m_iniPt.y, iniRect.Width(), iniRect.Height(), TRUE);
	}
	
	// TODO: この位置に初期化の補足処理を追加してください
	if (m_strcn == _T("")) {
		m_editName.SetWindowText(_T("新しいノード"));
	} else if (m_strcn == _T("ノード")) {
		m_editName.SetWindowText(_T("ノード"));
	} else {
		PostMessage(WM_SETCHARDATA, 0);
	}
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void CInpcnDlg::OnClose() 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	MoveWindow(iniRect.left, iniRect.top, iniRect.Width(), iniRect.Height(), TRUE);
	CDialog::OnClose();
}

LRESULT CInpcnDlg::OnSetCharData(UINT wParam, LONG lParam)
{
	m_editName.ReplaceSel(m_strcn);
	return 0;
}
