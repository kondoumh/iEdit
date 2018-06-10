// ExportHtmlDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "ExportHtmlDlg.h"
#include "StringUtil.h"

// ExportHtmlDlg ダイアログ

IMPLEMENT_DYNAMIC(ExportHtmlDlg, CDialog)

ExportHtmlDlg::ExportHtmlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ExportHtmlDlg::IDD, pParent)
	, m_xvRdNav(0)
	, m_xvRdImg(0)
	, m_xvRdText(0)
	, m_xvEdPrfIndex(_T(""))
	, m_xvEdPrfToc(_T(""))
	, m_xvEdPrfTextSingle(_T(""))
	, m_xvEdPrfTextEverynode(_T(""))
	, m_xvEdPrfNet(_T(""))
	, m_xvRdTree(FALSE)
	, m_nameOfRoot(_T(""))
	, m_nameOfVisibleRoot(_T(""))
{

}

ExportHtmlDlg::~ExportHtmlDlg()
{
}

void ExportHtmlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RD_NAV_OUTLINE, m_xvRdNav);
	DDX_Radio(pDX, IDC_RD_IMG_SVG, m_xvRdImg);
	DDX_Radio(pDX, IDC_RD_TEXT_SINGLE, m_xvRdText);
	DDX_Text(pDX, IDC_ED_PRF_INDEX, m_xvEdPrfIndex);
	DDX_Text(pDX, IDC_ED_PRF_TOC, m_xvEdPrfToc);
	DDX_Text(pDX, IDC_ED_PRF_TEXT_SINGLE, m_xvEdPrfTextSingle);
	DDX_Text(pDX, IDC_ED_PRF_TEXT_EVERYNODE, m_xvEdPrfTextEverynode);
	DDX_Text(pDX, IDC_ED_PRF_NET, m_xvEdPrfNet);
	DDX_Radio(pDX, IDC_RD_TREE, m_xvRdTree);
}


BEGIN_MESSAGE_MAP(ExportHtmlDlg, CDialog)
	ON_BN_CLICKED(IDC_RD_NAV_OUTLINE, &ExportHtmlDlg::OnBnClickedRdNavOutline)
	ON_BN_CLICKED(IDC_RD_NAV_NETWORK, &ExportHtmlDlg::OnBnClickedRdNavNetwork)
	ON_BN_CLICKED(IDC_RD_NAV_BOTH, &ExportHtmlDlg::OnBnClickedRdNavBoth)
	ON_BN_CLICKED(IDC_RD_TEXT_EVERYNODE, &ExportHtmlDlg::OnBnClickedRdTextEverynode)
	ON_BN_CLICKED(IDC_RD_TEXT_SINGLE, &ExportHtmlDlg::OnBnClickedRdTextSingle)
	ON_BN_CLICKED(IDC_BTN_SET_PRF_BY_FNAME, &ExportHtmlDlg::OnBnClickedBtnSetPrfByFname)
	ON_EN_CHANGE(IDC_ED_PRF_TOC, &ExportHtmlDlg::OnEnChangeEdPrfToc)
	ON_EN_CHANGE(IDC_ED_PRF_INDEX, &ExportHtmlDlg::OnEnChangeEdPrfIndex)
	ON_EN_CHANGE(IDC_ED_PRF_NET, &ExportHtmlDlg::OnEnChangeEdPrfNet)
	ON_EN_CHANGE(IDC_ED_PRF_TEXT_SINGLE, &ExportHtmlDlg::OnEnChangeEdPrfTextSingle)
	ON_EN_CHANGE(IDC_ED_PRF_TEXT_EVERYNODE, &ExportHtmlDlg::OnEnChangeEdPrfTextEverynode)
	ON_BN_CLICKED(IDC_BTN_SET_PRF_BY_ROOT, &ExportHtmlDlg::OnBnClickedBtnSetPrfByRoot)
	ON_BN_CLICKED(IDC_BTN_SET_PRF_BY_VISIBLE_ROOT, &ExportHtmlDlg::OnBnClickedBtnSetPrfByVisibleRoot)
END_MESSAGE_MAP()


BOOL ExportHtmlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetPathIndex();
	SetPathOutline();
	if (m_xvRdNav == 0) {
		SetRdImageEnable(FALSE);
	}
	else if (m_xvRdNav == 1) {
		GetDlgItem(IDC_ED_PRF_TOC)->EnableWindow(FALSE);
		SetRdImageEnable(TRUE);
	}
	else {
		SetRdImageEnable(TRUE);
		SetPathNetwork();
	}
	if (m_xvRdText == 0) {
		SetEdPrfTextEverynodeEnable(FALSE);
		SetEdPrfTextSingleEnable(TRUE);
	}
	else {
		SetEdPrfTextEverynodeEnable(FALSE);
		SetEdPrfTextSingleEnable(TRUE);
		SetPathTextSingle();
	}
	return TRUE;
}

void ExportHtmlDlg::OnBnClickedRdNavOutline()
{
	GetDlgItem(IDC_ED_PRF_TOC)->EnableWindow(TRUE);
	SetRdImageEnable(FALSE);
}

void ExportHtmlDlg::SetRdImageEnable(BOOL bEnable)
{
	GetDlgItem(IDC_RD_IMG_SVG)->EnableWindow(bEnable);
	GetDlgItem(IDC_RD_IMG_PNG)->EnableWindow(bEnable);
	GetDlgItem(IDC_ED_PRF_NET)->EnableWindow(bEnable);
}

void ExportHtmlDlg::SetEdPrfTextSingleEnable(BOOL bEnable)
{
	GetDlgItem(IDC_ED_PRF_TEXT_SINGLE)->EnableWindow(bEnable);
}

void ExportHtmlDlg::SetEdPrfTextEverynodeEnable(BOOL bEnable)
{
	GetDlgItem(IDC_ED_PRF_TEXT_EVERYNODE)->EnableWindow(bEnable);
}

void ExportHtmlDlg::OnBnClickedRdNavNetwork()
{
	GetDlgItem(IDC_ED_PRF_TOC)->EnableWindow(FALSE);
	SetRdImageEnable(TRUE);
}

void ExportHtmlDlg::OnBnClickedRdNavBoth()
{
	GetDlgItem(IDC_ED_PRF_TOC)->EnableWindow(TRUE);
	SetRdImageEnable(TRUE);
}

void ExportHtmlDlg::OnBnClickedRdTextEverynode()
{
	SetEdPrfTextEverynodeEnable(TRUE);
	SetEdPrfTextSingleEnable(FALSE);
}

void ExportHtmlDlg::OnBnClickedRdTextSingle()
{
	SetEdPrfTextEverynodeEnable(FALSE);
	SetEdPrfTextSingleEnable(TRUE);
}

void ExportHtmlDlg::OnBnClickedBtnSetPrfByFname()
{
	GetDlgItem(IDC_ED_PRF_INDEX)->SetWindowText(m_docTitle);
}

void ExportHtmlDlg::OnEnChangeEdPrfIndex()
{
	SetPathIndex();
}

void ExportHtmlDlg::SetPathIndex()
{
	CString s;
	GetDlgItem(IDC_ED_PRF_INDEX)->GetWindowText(s);
	if (GetDlgItem(IDC_ED_PRF_TOC)->IsWindowEnabled()) {
		GetDlgItem(IDC_ED_PRF_TOC)->SetWindowText(s);
	}
	if (GetDlgItem(IDC_ED_PRF_NET)->IsWindowEnabled()) {
		GetDlgItem(IDC_ED_PRF_NET)->SetWindowText(s);
	}
	if (GetDlgItem(IDC_ED_PRF_TEXT_SINGLE)->IsWindowEnabled()) {
		GetDlgItem(IDC_ED_PRF_TEXT_SINGLE)->SetWindowText(s);
	}
	CString prf = s.Trim();
	prf = StringUtil::getSafeFileName(prf);
	if (prf == _T("")) {
		m_pathIndex = _T("index.html");
	}
	else {
		m_pathIndex = prf + _T(".html");
	}
	GetDlgItem(IDC_LB_INDEX)->SetWindowText(m_pathIndex);
}

void ExportHtmlDlg::OnEnChangeEdPrfToc()
{
	SetPathOutline();
}

void ExportHtmlDlg::SetPathOutline()
{
	CString s;
	GetDlgItem(IDC_ED_PRF_TOC)->GetWindowText(s);
	CString prf = s.Trim();
	prf = StringUtil::getSafeFileName(prf);
	if (prf == _T("")) {
		m_pathOutline = _T("outline.html");
	}
	else {
		m_pathOutline = prf + _T("_outline.html");
	}
	GetDlgItem(IDC_LB_TOC)->SetWindowText(m_pathOutline);
}

void ExportHtmlDlg::OnEnChangeEdPrfNet()
{
	SetPathNetwork();
}

void ExportHtmlDlg::SetPathNetwork()
{
	CString s;
	GetDlgItem(IDC_ED_PRF_NET)->GetWindowText(s);
	CString prf = s.Trim();
	prf = StringUtil::getSafeFileName(prf);
	if (prf == _T("")) {
		m_pathNetwork = _T("network.html");
		m_pathSvg = _T("network.svg");
		m_pathPng = _T("network.png");
	}
	else {
		m_pathNetwork = prf + _T("_network.html");
		m_pathSvg = prf + _T("_network.svg");
		m_pathPng = prf + _T("_network.png");
	}
	GetDlgItem(IDC_LB_NET)->SetWindowText(m_pathNetwork);
}

void ExportHtmlDlg::OnEnChangeEdPrfTextSingle()
{
	SetPathTextSingle();
}

void ExportHtmlDlg::SetPathTextSingle()
{
	CString s;
	GetDlgItem(IDC_ED_PRF_TEXT_SINGLE)->GetWindowText(s);
	CString prf = s.Trim();
	prf = StringUtil::getSafeFileName(prf);
	if (prf == _T("")) {
		m_pathTextSingle = _T("text.html");
	}
	else {
		m_pathTextSingle = prf + _T("_text.html");
	}
	GetDlgItem(IDC_LB_TEXT_SINGLE)->SetWindowText(m_pathTextSingle);
}

void ExportHtmlDlg::OnEnChangeEdPrfTextEverynode()
{
	CString s;
	GetDlgItem(IDC_ED_PRF_TEXT_EVERYNODE)->GetWindowText(s);
	CString prf = s.Trim();
	prf = StringUtil::getSafeFileName(prf);
	if (prf == "") {
		m_xvEdPrfTextEverynode = _T("");
	}
	else {
		m_xvEdPrfTextEverynode = prf;
	}
	GetDlgItem(IDC_LB_TEXT_EVERYNODE)->SetWindowText(
		m_xvEdPrfTextEverynode + _T("1.html, ") + m_xvEdPrfTextEverynode + _T("2.html, ..."));
}

void ExportHtmlDlg::OnBnClickedBtnSetPrfByRoot()
{
	GetDlgItem(IDC_ED_PRF_INDEX)->SetWindowText(
		StringUtil::getSafeFileName(m_nameOfRoot));
}

void ExportHtmlDlg::OnBnClickedBtnSetPrfByVisibleRoot()
{
	GetDlgItem(IDC_ED_PRF_INDEX)->SetWindowText(
		StringUtil::getSafeFileName(m_nameOfVisibleRoot));
}
