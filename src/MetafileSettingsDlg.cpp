// MetafileSettingsDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "SystemConfiguration.h"
#include "MetafileSettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// MetafileSettingsDlg ダイアログ


MetafileSettingsDlg::MetafileSettingsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(MetafileSettingsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(MetafileSettingsDlg)
	//}}AFX_DATA_INIT
}


void MetafileSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MetafileSettingsDlg)
	DDX_Control(pDX, IDC_ST_INFO, m_stInfo);
	DDX_Control(pDX, IDC_MF, m_stMF);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MetafileSettingsDlg, CDialog)
	//{{AFX_MSG_MAP(MetafileSettingsDlg)
	ON_BN_CLICKED(IDC_BTN_LEFT, OnBtnLeft)
	ON_BN_CLICKED(IDC_BTN_RIGHT, OnBtnRight)
	ON_BN_CLICKED(IDC_BTN_DOWN, OnBtnDown)
	ON_BN_CLICKED(IDC_BTN_UP, OnBtnUp)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_RESET, OnBnClickedBtnReset)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// MetafileSettingsDlg メッセージ ハンドラ

BOOL MetafileSettingsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_stMF.SetEnhMetaFile(createMF());
	showSizeInfo();
	return TRUE;
}

HENHMETAFILE MetafileSettingsDlg::createMF()
{
	CMetaFileDC mfDC;
	CRect rc(0, 0, (int)(100 * m_rx), (int)(100 * m_ry));
	mfDC.CreateEnhanced(NULL, NULL, &rc, NULL);

	mfDC.Ellipse(0, 0, 100, 100);

	return mfDC.CloseEnhanced();
}

void MetafileSettingsDlg::showSizeInfo()
{
	CString info;
	info.Format(_T("幅 : %.01f  高さ : %.01f"), m_rx, m_ry);
	m_stInfo.SetWindowText(info);
}

void MetafileSettingsDlg::OnBtnLeft()
{
	m_rx -= 0.1;
	if (m_rx <= 0) {
		m_rx = 0.1;
	}
	m_stMF.SetEnhMetaFile(createMF());
	showSizeInfo();
}

void MetafileSettingsDlg::OnBtnRight()
{
	m_rx += 0.1;
	m_stMF.SetEnhMetaFile(createMF());
	showSizeInfo();
}

void MetafileSettingsDlg::OnBtnDown()
{
	m_ry -= 0.1;
	if (m_ry <= 0) {
		m_ry = 0.1;
	}
	m_stMF.SetEnhMetaFile(createMF());
	showSizeInfo();
}

void MetafileSettingsDlg::OnBtnUp()
{
	m_ry += 0.1;
	m_stMF.SetEnhMetaFile(createMF());
	showSizeInfo();
}

void MetafileSettingsDlg::OnBnClickedBtnReset()
{
	CSize szMF = SystemConfiguration::GetMetafileSize();
	m_rx = ((double)szMF.cx) / 10.0;
	m_ry = ((double)szMF.cy) / 10.0;;
	m_stMF.SetEnhMetaFile(createMF());
	showSizeInfo();
}
