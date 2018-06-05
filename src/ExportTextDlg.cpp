// ExportTextDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "ExportTextDlg.h"
#include "afxdialogex.h"

// ExportTextDlg ダイアログ

IMPLEMENT_DYNAMIC(ExportTextDlg, CDialogEx)

ExportTextDlg::ExportTextDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ExportTextDlg::IDD, pParent)
	, m_rdTreeOption(0)
	, m_rdFormatOption(0)
	, m_rdChapterNumberOption(0)
	, m_excludeLabelFromFileName(FALSE)
	, m_excludeLabelFromContent(FALSE)
{

}

ExportTextDlg::~ExportTextDlg()
{
}

void ExportTextDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_EXPORT_ALL, m_rdTreeOption);
	DDX_Radio(pDX, IDC_NODE_TEXT, m_rdFormatOption);
	DDX_Radio(pDX, IDC_CHPTER_NUMBER_WZ, m_rdChapterNumberOption);
	DDX_Check(pDX, IDC_FILE_NAME_EXCLUDE_LABEL, m_excludeLabelFromFileName);
	DDX_Check(pDX, IDC_EXCLUDE_LABEL_FROM_TEXT, m_excludeLabelFromContent);
}

BEGIN_MESSAGE_MAP(ExportTextDlg, CDialogEx)
	ON_BN_CLICKED(IDC_NODE_TEXT, &ExportTextDlg::OnBnClickedNodeText)
	ON_BN_CLICKED(IDC_NODE_ONLY, &ExportTextDlg::OnBnClickedNodeOnly)
	ON_BN_CLICKED(IDC_FILE_EVERY_NODE, &ExportTextDlg::OnBnClickedFileEveryNode)
	ON_BN_CLICKED(IDC_MARKDOWN, &ExportTextDlg::OnBnClickedMarkdown)
	ON_BN_CLICKED(IDC_ORG_MODE, &ExportTextDlg::OnBnClickedOrgMode)
END_MESSAGE_MAP()


BOOL ExportTextDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (m_rdFormatOption != 2) {
		EnableContentFileOption(FALSE);
	}

	return TRUE;
}

void ExportTextDlg::EnableContentFileOption(BOOL enable)
{
	GetDlgItem(IDC_FILE_NAME_EXCLUDE_LABEL)->EnableWindow(enable);
	GetDlgItem(IDC_EXCLUDE_LABEL_FROM_TEXT)->EnableWindow(enable);
}

void ExportTextDlg::EnableChapterNumberOption(BOOL enable)
{
	GetDlgItem(IDC_CHPTER_NUMBER_WZ)->EnableWindow(enable);
	GetDlgItem(IDC_CHAPTER_NUMBER_HYPHEN)->EnableWindow(enable);
	GetDlgItem(IDC_CHAPTER_NUMBER_PERIOD)->EnableWindow(enable);
}

void ExportTextDlg::OnBnClickedNodeText()
{
	EnableContentFileOption(FALSE);
	EnableChapterNumberOption(TRUE);
	GetDlgItem(IDC_CHPTER_NUMBER_WZ)->EnableWindow(TRUE);
	GetDlgItem(IDC_EXPORT_SELECTED_CHILDREN)->EnableWindow(TRUE);
}


void ExportTextDlg::OnBnClickedNodeOnly()
{
	EnableContentFileOption(FALSE);
	EnableChapterNumberOption(TRUE);
	GetDlgItem(IDC_EXPORT_SELECTED_CHILDREN)->EnableWindow(TRUE);
}


void ExportTextDlg::OnBnClickedFileEveryNode()
{
	EnableContentFileOption(TRUE);
	EnableChapterNumberOption(TRUE);
	UpdateData();
	if (m_rdChapterNumberOption == 0) {
		m_rdChapterNumberOption = 1;
		UpdateData(FALSE);
	}
	if (m_rdTreeOption == 2) {
		m_rdTreeOption = 1;
		UpdateData(FALSE);
	}
	GetDlgItem(IDC_CHPTER_NUMBER_WZ)->EnableWindow(FALSE);
	GetDlgItem(IDC_EXPORT_SELECTED_CHILDREN)->EnableWindow(FALSE);
}


void ExportTextDlg::OnBnClickedMarkdown()
{
	EnableContentFileOption(FALSE);
	EnableChapterNumberOption(FALSE);
	GetDlgItem(IDC_EXPORT_SELECTED_CHILDREN)->EnableWindow(FALSE);
}


void ExportTextDlg::OnBnClickedOrgMode()
{
	EnableContentFileOption(FALSE);
	EnableChapterNumberOption(FALSE);
	GetDlgItem(IDC_EXPORT_SELECTED_CHILDREN)->EnableWindow(FALSE);
}
