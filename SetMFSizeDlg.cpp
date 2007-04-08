// SetMFSizeDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "SetMFSizeDlg.h"
#include "MfSizer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SetMFSizeDlg ダイアログ


SetMFSizeDlg::SetMFSizeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(SetMFSizeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(SetMFSizeDlg)
	//}}AFX_DATA_INIT
}


void SetMFSizeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(SetMFSizeDlg)
	DDX_Control(pDX, IDC_ST_INFO, m_stInfo);
	DDX_Control(pDX, IDC_MF, m_stMF);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SetMFSizeDlg, CDialog)
	//{{AFX_MSG_MAP(SetMFSizeDlg)
	ON_BN_CLICKED(IDC_BTN_LEFT, OnBtnLeft)
	ON_BN_CLICKED(IDC_BTN_RIGHT, OnBtnRight)
	ON_BN_CLICKED(IDC_BTN_DOWN, OnBtnDown)
	ON_BN_CLICKED(IDC_BTN_UP, OnBtnUp)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_RESET, OnBnClickedBtnReset)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SetMFSizeDlg メッセージ ハンドラ

BOOL SetMFSizeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	m_stMF.SetEnhMetaFile(createMF());
	showSizeInfo();
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

HENHMETAFILE SetMFSizeDlg::createMF()
{
	CMetaFileDC mfDC;
	CRect rc(0, 0, (int)(100*m_rx) , (int)(100*m_ry));
	mfDC.CreateEnhanced(NULL, NULL, &rc, NULL);
	
	mfDC.Ellipse(0, 0, 100, 100);
	
	return mfDC.CloseEnhanced();
}

void SetMFSizeDlg::showSizeInfo()
{
	CString info;
	info.Format("幅 : %.01f  高さ : %.01f", m_rx, m_ry);
	m_stInfo.SetWindowText(info);
}

void SetMFSizeDlg::OnBtnLeft() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_rx -= 0.1;
	if (m_rx <= 0) {
		m_rx = 0.1;
	} 
	m_stMF.SetEnhMetaFile(createMF());
	showSizeInfo();
}

void SetMFSizeDlg::OnBtnRight() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_rx += 0.1;
	m_stMF.SetEnhMetaFile(createMF());
	showSizeInfo();
}

void SetMFSizeDlg::OnBtnDown() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_ry -= 0.1;
	if (m_ry <= 0) {
		m_ry = 0.1;
	}
	m_stMF.SetEnhMetaFile(createMF());
	showSizeInfo();
}

void SetMFSizeDlg::OnBtnUp() 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	m_ry += 0.1;
	m_stMF.SetEnhMetaFile(createMF());
	showSizeInfo();
}

void SetMFSizeDlg::OnBnClickedBtnReset()
{
	// TODO : ここにコントロール通知ハンドラ コードを追加します。
	MfSizer sizer;
	CSize szMF = sizer.getMFSize();
	m_rx = ((double)szMF.cx)/10.0;
	m_ry = ((double)szMF.cy)/10.0;;
	m_stMF.SetEnhMetaFile(createMF());
	showSizeInfo();
}
