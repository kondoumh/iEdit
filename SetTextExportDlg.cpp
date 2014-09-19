// SetTextExportDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "SetTextExportDlg.h"
#include "afxdialogex.h"


// SetTextExportDlg ダイアログ

IMPLEMENT_DYNAMIC(SetTextExportDlg, CDialogEx)

SetTextExportDlg::SetTextExportDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(SetTextExportDlg::IDD, pParent)
	, m_rdTreeOption(0)
	, m_rdFormatOption(0)
	, m_rdChapterNumberOption(0)
	, m_excludeLabelFromFileName(FALSE)
	, m_excludeLabelFromContent(FALSE)
{

}

SetTextExportDlg::~SetTextExportDlg()
{
}

void SetTextExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_EXPORT_ALL, m_rdTreeOption);
	DDX_Radio(pDX, IDC_NODE_TEXT, m_rdFormatOption);
	DDX_Radio(pDX, IDC_CHPTER_NUMBER_WZ, m_rdChapterNumberOption);
	DDX_Check(pDX, IDC_FILE_NAME_EXCLUDE_LABEL, m_excludeLabelFromFileName);
	DDX_Check(pDX, IDC_EXCLUDE_LABEL_FROM_TEXT, m_excludeLabelFromContent);
}


BEGIN_MESSAGE_MAP(SetTextExportDlg, CDialogEx)
	ON_BN_CLICKED(IDC_NODE_TEXT, &SetTextExportDlg::OnBnClickedNodeText)
	ON_BN_CLICKED(IDC_NODE_ONLY, &SetTextExportDlg::OnBnClickedNodeOnly)
	ON_BN_CLICKED(IDC_FILE_EVERY_NODE, &SetTextExportDlg::OnBnClickedFileEveryNode)
	ON_BN_CLICKED(IDC_MARKDOWN, &SetTextExportDlg::OnBnClickedMarkdown)
	ON_BN_CLICKED(IDC_ORG_MODE, &SetTextExportDlg::OnBnClickedOrgMode)
END_MESSAGE_MAP()


// SetTextExportDlg メッセージ ハンドラー


BOOL SetTextExportDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	if (m_rdFormatOption != 2) {
		EnableContentFileOption(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}


void SetTextExportDlg::EnableContentFileOption(BOOL enable)
{
	GetDlgItem(IDC_FILE_NAME_EXCLUDE_LABEL)->EnableWindow(enable);
	GetDlgItem(IDC_EXCLUDE_LABEL_FROM_TEXT)->EnableWindow(enable);
}


void SetTextExportDlg::OnBnClickedNodeText()
{
	EnableContentFileOption(FALSE);
}


void SetTextExportDlg::OnBnClickedNodeOnly()
{
	EnableContentFileOption(FALSE);
}


void SetTextExportDlg::OnBnClickedFileEveryNode()
{
	EnableContentFileOption(TRUE);
}


void SetTextExportDlg::OnBnClickedMarkdown()
{
	EnableContentFileOption(FALSE);
}


void SetTextExportDlg::OnBnClickedOrgMode()
{
	EnableContentFileOption(FALSE);
}
