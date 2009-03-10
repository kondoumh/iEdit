// SetHtmlExportDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "SetHtmlExportDlg.h"
#include "Utilities.h"


// SetHtmlExportDlg ダイアログ

IMPLEMENT_DYNAMIC(SetHtmlExportDlg, CDialog)

SetHtmlExportDlg::SetHtmlExportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SetHtmlExportDlg::IDD, pParent)
	, m_xvRdNav(0)
	, m_xvRdImg(0)
	, m_xvRdText(0)
	, m_xvEdCssToc(_T(""))
	, m_xvEdCssText(_T(""))
	, m_xvEdPrfIndex(_T(""))
	, m_xvEdPrfToc(_T(""))
	, m_xvEdPrfTextSingle(_T(""))
	, m_xvEdPrfTextEverynode(_T(""))
	, m_xvEdPrfNet(_T(""))
{

}

SetHtmlExportDlg::~SetHtmlExportDlg()
{
}

void SetHtmlExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RD_NAV_OUTLINE, m_xvRdNav);
	DDX_Radio(pDX, IDC_RD_IMG_SVG, m_xvRdImg);
	DDX_Radio(pDX, IDC_RD_TEXT_EVERYNODE, m_xvRdText);
	DDX_Text(pDX, IDC_ED_CSS_TOC, m_xvEdCssToc);
	DDX_Text(pDX, IDC_ED_CSS_TEXT, m_xvEdCssText);
	DDX_Text(pDX, IDC_ED_PRF_INDEX, m_xvEdPrfIndex);
	DDX_Text(pDX, IDC_ED_PRF_TOC, m_xvEdPrfToc);
	DDX_Text(pDX, IDC_ED_PRF_TEXT_SINGLE, m_xvEdPrfTextSingle);
	DDX_Text(pDX, IDC_ED_PRF_TEXT_EVERYNODE, m_xvEdPrfTextEverynode);
	DDX_Text(pDX, IDC_ED_PRF_NET, m_xvEdPrfNet);
}


BEGIN_MESSAGE_MAP(SetHtmlExportDlg, CDialog)
	ON_BN_CLICKED(IDC_RD_NAV_OUTLINE, &SetHtmlExportDlg::OnBnClickedRdNavOutline)
	ON_BN_CLICKED(IDC_RD_NAV_NETWORK, &SetHtmlExportDlg::OnBnClickedRdNavNetwork)
	ON_BN_CLICKED(IDC_RD_NAV_BOTH, &SetHtmlExportDlg::OnBnClickedRdNavBoth)
	ON_BN_CLICKED(IDC_RD_TEXT_EVERYNODE, &SetHtmlExportDlg::OnBnClickedRdTextEverynode)
	ON_BN_CLICKED(IDC_RD_TEXT_SINGLE, &SetHtmlExportDlg::OnBnClickedRdTextSingle)
	ON_BN_CLICKED(IDC_BTN_SET_PRF_BY_FNAME, &SetHtmlExportDlg::OnBnClickedBtnSetPrfByFname)
	ON_EN_CHANGE(IDC_ED_PRF_TOC, &SetHtmlExportDlg::OnEnChangeEdPrfToc)
	ON_EN_CHANGE(IDC_ED_PRF_INDEX, &SetHtmlExportDlg::OnEnChangeEdPrfIndex)
	ON_EN_CHANGE(IDC_ED_PRF_NET, &SetHtmlExportDlg::OnEnChangeEdPrfNet)
	ON_EN_CHANGE(IDC_ED_PRF_TEXT_SINGLE, &SetHtmlExportDlg::OnEnChangeEdPrfTextSingle)
	ON_EN_CHANGE(IDC_ED_PRF_TEXT_EVERYNODE, &SetHtmlExportDlg::OnEnChangeEdPrfTextEverynode)
END_MESSAGE_MAP()


BOOL SetHtmlExportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  ここに初期化を追加してください
	SetRdImageEnable(FALSE);
	if (m_xvRdNav == 0) {
		SetRdImageEnable(FALSE);
	} else {
		SetRdImageEnable(TRUE);
	}
	if (m_xvRdText == 0) {
		SetEdPrfTextEverynodeEnable(TRUE);
		SetEdPrfTextSingleEnable(FALSE);
	} else {
		SetEdPrfTextEverynodeEnable(FALSE);
		SetEdPrfTextSingleEnable(TRUE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 例外 : OCX プロパティ ページは必ず FALSE を返します。
}

// SetHtmlExportDlg メッセージ ハンドラ

void SetHtmlExportDlg::OnBnClickedRdNavOutline()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	SetRdImageEnable(FALSE);
}

void SetHtmlExportDlg::SetRdImageEnable(BOOL bEnable)
{
	GetDlgItem(IDC_RD_IMG_SVG)->EnableWindow(bEnable);
	GetDlgItem(IDC_RD_IMG_PNG)->EnableWindow(bEnable);
	GetDlgItem(IDC_ED_PRF_NET)->EnableWindow(bEnable);
}

void SetHtmlExportDlg::SetEdPrfTextSingleEnable(BOOL bEnable)
{
	GetDlgItem(IDC_ED_PRF_TEXT_SINGLE)->EnableWindow(bEnable);
}

void SetHtmlExportDlg::SetEdPrfTextEverynodeEnable(BOOL bEnable)
{
	GetDlgItem(IDC_ED_PRF_TEXT_EVERYNODE)->EnableWindow(bEnable);
}

void SetHtmlExportDlg::OnBnClickedRdNavNetwork()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	SetRdImageEnable(TRUE);
}

void SetHtmlExportDlg::OnBnClickedRdNavBoth()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	SetRdImageEnable(TRUE);
}

void SetHtmlExportDlg::OnBnClickedRdTextEverynode()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	SetEdPrfTextEverynodeEnable(TRUE);
	SetEdPrfTextSingleEnable(FALSE);
}

void SetHtmlExportDlg::OnBnClickedRdTextSingle()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	SetEdPrfTextEverynodeEnable(FALSE);
	SetEdPrfTextSingleEnable(TRUE);
}

void SetHtmlExportDlg::OnBnClickedBtnSetPrfByFname()
{
	// TODO: ここにコントロール通知ハンドラ コードを追加します。
	GetDlgItem(IDC_ED_PRF_INDEX)->SetWindowTextA(m_sDocTitle);
}

void SetHtmlExportDlg::OnEnChangeEdPrfIndex()
{
	// TODO:  ここにコントロール通知ハンドラ コードを追加してください。
	CString s;
	GetDlgItem(IDC_ED_PRF_INDEX)->GetWindowText(s);
	GetDlgItem(IDC_ED_PRF_TOC)->SetWindowTextA(s);
	if (GetDlgItem(IDC_ED_PRF_NET)->IsWindowEnabled()) {
		GetDlgItem(IDC_ED_PRF_NET)->SetWindowTextA(s);
	}
	if (GetDlgItem(IDC_ED_PRF_TEXT_SINGLE)->IsWindowEnabled()) {
		GetDlgItem(IDC_ED_PRF_TEXT_SINGLE)->SetWindowTextA(s);
	}
	CString prf = s.Trim();
	prf = Utilities::getSafeFileName(prf);
	if (prf == "") {
		m_xvEdPrfIndex = "index.html";
	} else {
		m_xvEdPrfIndex = prf + ".html";
	}
	GetDlgItem(IDC_LB_INDEX)->SetWindowTextA(m_xvEdPrfIndex);
}

void SetHtmlExportDlg::OnEnChangeEdPrfToc()
{
	// TODO:  ここにコントロール通知ハンドラ コードを追加してください。
	CString s;
	GetDlgItem(IDC_ED_PRF_TOC)->GetWindowText(s);
	CString prf = s.Trim();
	prf = Utilities::getSafeFileName(prf);
	if (prf == "") {
		m_xvEdPrfToc = "outline.html";
	} else {
		m_xvEdPrfToc = prf + "_outline.html";
	}
	GetDlgItem(IDC_LB_TOC)->SetWindowTextA(m_xvEdPrfToc);
}


void SetHtmlExportDlg::OnEnChangeEdPrfNet()
{
	// TODO:  ここにコントロール通知ハンドラ コードを追加してください。
	CString s;
	GetDlgItem(IDC_ED_PRF_NET)->GetWindowText(s);
	CString prf = s.Trim();
	prf = Utilities::getSafeFileName(prf);
	if (prf == "") {
		m_xvEdPrfNet = "network.html";
	} else {
		m_xvEdPrfNet = prf + "_network.html";
	}
	GetDlgItem(IDC_LB_NET)->SetWindowTextA(m_xvEdPrfNet);
}

void SetHtmlExportDlg::OnEnChangeEdPrfTextSingle()
{
	// TODO:  ここにコントロール通知ハンドラ コードを追加してください。
	CString s;
	GetDlgItem(IDC_ED_PRF_TEXT_SINGLE)->GetWindowText(s);
	CString prf = s.Trim();
	prf = Utilities::getSafeFileName(prf);
	if (prf == "") {
		m_xvEdPrfTextSingle = "text.html";
	} else {
		m_xvEdPrfTextSingle = prf + "_text.html";
	}
	GetDlgItem(IDC_LB_TEXT_SINGLE)->SetWindowTextA(m_xvEdPrfTextSingle);
}

void SetHtmlExportDlg::OnEnChangeEdPrfTextEverynode()
{
	// TODO:  ここにコントロール通知ハンドラ コードを追加してください。
	CString s;
	GetDlgItem(IDC_ED_PRF_TEXT_EVERYNODE)->GetWindowText(s);
	CString prf = s.Trim();
	prf = Utilities::getSafeFileName(prf);
	if (prf == "") {
		m_xvEdPrfTextEverynode = "";
	} else {
		m_xvEdPrfTextEverynode = prf;
	}
	GetDlgItem(IDC_LB_TEXT_EVERYNODE)->SetWindowTextA(
		m_xvEdPrfTextEverynode + "1.html, " + m_xvEdPrfTextEverynode + "2.html, ...");
}
