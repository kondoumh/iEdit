// SetHtmlExportDlg.cpp : �����t�@�C��
//

#include "stdafx.h"
#include "iEdit.h"
#include "SetHtmlExportDlg.h"


// SetHtmlExportDlg �_�C�A���O

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
END_MESSAGE_MAP()


BOOL SetHtmlExportDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  �����ɏ�������ǉ����Ă�������
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
	// ��O : OCX �v���p�e�B �y�[�W�͕K�� FALSE ��Ԃ��܂��B
}

// SetHtmlExportDlg ���b�Z�[�W �n���h��

void SetHtmlExportDlg::OnBnClickedRdNavOutline()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
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
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	SetRdImageEnable(TRUE);
}

void SetHtmlExportDlg::OnBnClickedRdNavBoth()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	SetRdImageEnable(TRUE);
}

void SetHtmlExportDlg::OnBnClickedRdTextEverynode()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	SetEdPrfTextEverynodeEnable(TRUE);
	SetEdPrfTextSingleEnable(FALSE);
}

void SetHtmlExportDlg::OnBnClickedRdTextSingle()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	SetEdPrfTextEverynodeEnable(FALSE);
	SetEdPrfTextSingleEnable(TRUE);
}

void SetHtmlExportDlg::OnBnClickedBtnSetPrfByFname()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
}
