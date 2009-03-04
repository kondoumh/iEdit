// SetHtmlExportDlg.cpp : 実装ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "SetHtmlExportDlg.h"


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
}


BEGIN_MESSAGE_MAP(SetHtmlExportDlg, CDialog)
	ON_BN_CLICKED(IDC_RD_NAV_OUTLINE, &SetHtmlExportDlg::OnBnClickedRdNavOutline)
	ON_BN_CLICKED(IDC_RD_NAV_NETWORK, &SetHtmlExportDlg::OnBnClickedRdNavNetwork)
	ON_BN_CLICKED(IDC_RD_NAV_BOTH, &SetHtmlExportDlg::OnBnClickedRdNavBoth)
	ON_BN_CLICKED(IDC_RD_TEXT_EVERYNODE, &SetHtmlExportDlg::OnBnClickedRdTextEverynode)
	ON_BN_CLICKED(IDC_RD_TEXT_SINGLE, &SetHtmlExportDlg::OnBnClickedRdTextSingle)
END_MESSAGE_MAP()


BOOL SetHtmlExportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  ここに初期化を追加してください
	SetRdImageEnable(FALSE);
	SetEdPrfTextEverynodeEnable(TRUE);
	SetEdPrfTextSingleEnable(FALSE);
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
