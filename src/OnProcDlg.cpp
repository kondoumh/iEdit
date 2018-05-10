// OnProcDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "OnProcDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COnProcDlg ダイアログ


COnProcDlg::COnProcDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COnProcDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COnProcDlg)
	//}}AFX_DATA_INIT
}


void COnProcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COnProcDlg)
	DDX_Control(pDX, IDC_PROC_NAME, m_ProcName);
	DDX_Control(pDX, IDC_PROGPROC, m_ProgProc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COnProcDlg, CDialog)
	//{{AFX_MSG_MAP(COnProcDlg)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COnProcDlg メッセージ ハンドラ
