// InpcatDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "InpcatDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// EditShapeCategoryDlg ダイアログ


EditShapeCategoryDlg::EditShapeCategoryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(EditShapeCategoryDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(EditShapeCategoryDlg)
	m_strName = _T("");
	//}}AFX_DATA_INIT
}


void EditShapeCategoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(EditShapeCategoryDlg)
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Text(pDX, IDC_EDIT1, m_strName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(EditShapeCategoryDlg, CDialog)
	//{{AFX_MSG_MAP(EditShapeCategoryDlg)
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditShapeCategoryDlg メッセージ ハンドラ

BOOL EditShapeCategoryDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	m_edit.SetSel(0, -1);
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void EditShapeCategoryDlg::OnChangeEdit1() 
{
	// TODO: これが RICHEDIT コントロールの場合、コントロールは、 lParam マスク
	// 内での論理和の ENM_CHANGE フラグ付きで CRichEditCrtl().SetEventMask()
	// メッセージをコントロールへ送るために CDialog::OnInitDialog() 関数をオーバー
	// ライドしない限りこの通知を送りません。
	
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_edit.GetWindowText(m_strName);
}
