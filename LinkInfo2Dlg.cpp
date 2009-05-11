// LinkInfo2Dlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "LinkInfo2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// LinkInfo2Dlg ダイアログ


LinkInfo2Dlg::LinkInfo2Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(LinkInfo2Dlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(LinkInfo2Dlg)
	//}}AFX_DATA_INIT
}


void LinkInfo2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(LinkInfo2Dlg)
	DDX_Control(pDX, IDC_EDIT2, m_Comment);
	DDX_Control(pDX, IDC_EDIT1, m_edit);
	DDX_Control(pDX, IDC_ORG, m_Org);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(LinkInfo2Dlg, CDialog)
	//{{AFX_MSG_MAP(LinkInfo2Dlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LinkInfo2Dlg メッセージ ハンドラ

void LinkInfo2Dlg::OnBrowse() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	char szFilters[] = "全てのファイル (*.*)|*.*|";
	CFileDialog dlg(TRUE, "*.*", NULL, OFN_HIDEREADONLY, szFilters, NULL);
	if (dlg.DoModal() != IDOK) return;
	m_edit.SetSel(0, -1);
	m_edit.ReplaceSel(dlg.GetPathName());
}

void LinkInfo2Dlg::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
	m_edit.GetWindowText(strPath);
	m_Comment.GetWindowText(strComment);
	if (strComment.GetLength() > 260) {
		strComment = strComment.Left(260);
	}
	if (strPath == "") {
		MessageBox("ファイル名または、URLを入力してください");
		return;
	}
	CDialog::OnOK();
}

BOOL LinkInfo2Dlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	m_Org.SetWindowText(strOrg);
	m_Comment.ReplaceSel(strComment);
	m_edit.ReplaceSel(strPath);
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}
