// FileDropActionDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "FileDropActionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// FileDropActionDlg ダイアログ


FileDropActionDlg::FileDropActionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(FileDropActionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(FileDropActionDlg)
	m_nDropProc = -1;
	//}}AFX_DATA_INIT
}


void FileDropActionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(FileDropActionDlg)
	DDX_Radio(pDX, IDC_RD_LINK, m_nDropProc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(FileDropActionDlg, CDialog)
	//{{AFX_MSG_MAP(FileDropActionDlg)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// FileDropActionDlg メッセージ ハンドラ
