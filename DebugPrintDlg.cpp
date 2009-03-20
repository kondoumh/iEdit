// DebugPrintDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "DebugPrintDlg.h"


// DebugPrintDlg ダイアログ

IMPLEMENT_DYNAMIC(DebugPrintDlg, CDialog)

DebugPrintDlg::DebugPrintDlg(CWnd* pParent /*=NULL*/)
	: CDialog(DebugPrintDlg::IDD, pParent)
{

}

DebugPrintDlg::~DebugPrintDlg()
{
}

void DebugPrintDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PRINT, m_edConsole);
}

BOOL DebugPrintDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, 
						   DWORD dwStyle, const RECT &rect, CWnd *pParentWnd, 
						   UINT nID, CCreateContext *pContext)
{
	m_pParent = pParentWnd;
	return CDialog::Create(IDD, pParentWnd);
}

void DebugPrintDlg::OnOK()
{
}

void DebugPrintDlg::OnCancel()
{
}

BEGIN_MESSAGE_MAP(DebugPrintDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &DebugPrintDlg::OnBnClickedBtnClear)
	ON_BN_CLICKED(IDC_BTN_COPY, &DebugPrintDlg::OnBnClickedBtnCopy)
END_MESSAGE_MAP()


// DebugPrintDlg メッセージ ハンドラ

void DebugPrintDlg::OnBnClickedBtnClear()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	m_edConsole.SetSel(0, -1);
	m_edConsole.Clear();
}

void DebugPrintDlg::OnBnClickedBtnCopy()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	m_edConsole.SetSel(0, -1);
	m_edConsole.Copy();
}
