// PropertyDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "PropertyDlg.h"

#include "PageFrame.h"
#include "PageNode.h"
#include "PageLink.h"
#include "PageOther.h"
#include "iNode.h"
#include "iLink.h"
#include "SystemConfiguration.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGS_NODE _T("Node Properties")
#define REGS_LINK _T("Link Properties")
#define REGS_FRAME _T("Frame Options")
#define REGS_OTHER _T("Settings")

/////////////////////////////////////////////////////////////////////////////
// CPropertyDlg ダイアログ


CPropertyDlg::CPropertyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPropertyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPropertyDlg)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DATA_INIT
}


void CPropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertyDlg)
	DDX_Control(pDX, IDC_TAB, m_tabSeet);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertyDlg, CDialog)
	//{{AFX_MSG_MAP(CPropertyDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyDlg メッセージ ハンドラ

BOOL CPropertyDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: この位置に初期化の補足処理を追加してください
	
	// タブシートにタブを追加
	TC_ITEM item;
	item.mask = TCIF_TEXT;
	item.pszText = _T("フレーム");
	m_tabSeet.InsertItem(0,&item);
	item.pszText = _T("ノード");
	m_tabSeet.InsertItem(1,&item);
	item.pszText = _T("リンク");
	m_tabSeet.InsertItem(2,&item);
	item.pszText = _T("その他");
	m_tabSeet.InsertItem(3,&item);

	// タブシートにページを追加
	m_tabSeet.addPage(pFrame = new PageFrame);
	m_tabSeet.addPage(pNode = new PageNode);
	m_tabSeet.addPage(pLink = new PageLink);
	m_tabSeet.addPage(pOther = new PageOther);
	
	initPageFrame();
	initPageNode();
	initPageLink();
	initPageOther();
	
	// タブシート開始!
	m_tabSeet.beginService();
	
	return TRUE;  // コントロールにフォーカスを設定しないとき、戻り値は TRUE となります
	              // 例外: OCX プロパティ ページの戻り値は FALSE となります
}

void CPropertyDlg::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
	
	// レジストリへの設定値書き込み
	writePageFrame();
	writePageNode();
	writePageLink();
	writePageOther();
	
	m_tabSeet.endService(true);
	CDialog::OnOK();
}

void CPropertyDlg::OnCancel() 
{
	// TODO: この位置に特別な後処理を追加してください。
	m_tabSeet.endService(true);
	CDialog::OnCancel();
}

void CPropertyDlg::initPageNode()
{
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	pNode->colorFill = pApp->m_rgsNode.colorFill;
	pNode->colorLine = pApp->m_rgsNode.colorLine;
	pNode->colorFont = pApp->m_rgsNode.colorFont;
	pNode->m_bNoBrs = !pApp->m_rgsNode.bFillColor;
	pNode->lineWidth = pApp->m_rgsNode.lineWidth;
	pNode->styleLine = pApp->m_rgsNode.styleLine;
	if (pApp->m_rgsNode.shape == iNode::rectangle) {
		pNode->m_rdShape = 0;
	} else if (pApp->m_rgsNode.shape == iNode::roundRect) {
		pNode->m_rdShape = 1;
	} else if (pApp->m_rgsNode.shape == iNode::arc) {
		pNode->m_rdShape = 2;
	}
	
	int ts = pApp->m_rgsNode.styleText;
	if (ts == iNode::notext) {
		pNode->m_rdTLine = 2;
	}

	if (ts == iNode::s_tl || ts == iNode::s_tc || ts == iNode::s_tr ||
		ts == iNode::s_cl || ts == iNode::s_cc || ts == iNode::s_cr ||
		ts == iNode::s_bl || ts == iNode::s_bc || ts == iNode::s_br) {
		pNode->m_rdTLine = 0;
	} else if (ts == iNode::m_l || ts == iNode::m_c || ts == iNode::m_r) {
		pNode->m_rdTLine = 1;
	}
	
	if (ts == iNode::s_cc || ts == iNode::s_tc || ts == iNode::s_bc || ts == iNode::m_c) {
		pNode->horiz = 1;
	} else if (ts == iNode::s_cl || ts == iNode::s_tl || ts == iNode::s_bl || ts == iNode::m_l) {
		pNode->horiz = 0;
	} else if (ts == iNode::s_cr || ts == iNode::s_tr || ts == iNode::s_br || ts == iNode::m_r) {
		pNode->horiz = 2;
	} else {
		pNode->horiz = 1;
	}
	
	if (ts == iNode::s_tc || ts == iNode::s_tl || ts == iNode::s_tr) {
		pNode->vert = 0;
	} else if (ts == iNode::s_cc || ts == iNode::s_cl || ts == iNode::s_cr) {
		pNode->vert = 1;
	} else if (ts == iNode::s_bc || ts == iNode::s_bl || ts == iNode::s_br) {
		pNode->vert = 2;
	} else {
		pNode->vert = 1;
	}
	pNode->lf = pApp->m_rgsNode.lf;
	::lstrcpy(pNode->lf.lfFaceName, pApp->m_rgsNode.lf.lfFaceName);
	
	pNode->m_bSyncOrder = pApp->m_rgsNode.bSyncOrder;
	pNode->m_orderDirection = pApp->m_rgsNode.orderDirection;
	pNode->m_bEnableGroup = pApp->m_rgsNode.bEnableGroup;
	pNode->m_bDisableNodeResize = pApp->m_rgsNode.bDisableNodeResize;
	
	pNode->margins.l = pApp->m_rgsNode.margin_l;
	pNode->margins.r = pApp->m_rgsNode.margin_r;
	pNode->margins.t = pApp->m_rgsNode.margin_t;
	pNode->margins.b = pApp->m_rgsNode.margin_b;

	pNode->m_bPriorSelectionDragging = pApp->m_rgsNode.bPriorSelectionDragging;
}

void CPropertyDlg::initPageLink()
{
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	pLink->colorLine = pApp->m_rgsLink.colorLine;
	pLink->lineWidth = pApp->m_rgsLink.lineWidth;
	pLink->styleLine = pApp->m_rgsLink.styleLine;
	pLink->m_bSetStrength = pApp->m_rgsLink.bSetStrength;
	pLink->m_Slider.SetRange(0, 100);
	pLink->strength = pApp->m_rgsLink.strength;
	pLink->lf = pApp->m_rgsLink.lf;
	::lstrcpy(pLink->lf.lfFaceName, pApp->m_rgsLink.lf.lfFaceName);
}

void CPropertyDlg::writePageNode()
{
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	pApp->WriteProfileInt(REGS_NODE, _T("Fill Color"), pNode->colorFill);
	pApp->WriteProfileInt(REGS_NODE, _T("Line Color"), pNode->colorLine);
	pApp->WriteProfileInt(REGS_NODE, _T("Font Color"), pNode->colorFont);
	pApp->WriteProfileInt(REGS_NODE, _T("Paint"), !pNode->m_bNoBrs);
	pApp->WriteProfileInt(REGS_NODE, _T("Line Width"), pNode->lineWidth);
	pApp->WriteProfileInt(REGS_NODE, _T("Line Style"), pNode->styleLine);
	if (pNode->m_rdShape == 0) {
		pApp->WriteProfileInt(REGS_NODE, _T("Node Shape"), iNode::rectangle);
	} else if (pNode->m_rdShape == 1) {
		pApp->WriteProfileInt(REGS_NODE, _T("Node Shape"), iNode::roundRect);
	} else if (pNode->m_rdShape == 2) {
		pApp->WriteProfileInt(REGS_NODE, _T("Node Shape"), iNode::arc);
	}
	
	
	if (pNode->m_rdTLine == 0) {
		if (pNode->vert == 0 && pNode->horiz == 0) {
			pApp->WriteProfileInt(REGS_NODE, _T("Text Align"), iNode::s_tl);
		} else if (pNode->vert == 1 && pNode->horiz == 0) {
			pApp->WriteProfileInt(REGS_NODE, _T("Text Align"), iNode::s_cl);
		} else if (pNode->vert == 2 && pNode->horiz == 0) {
			pApp->WriteProfileInt(REGS_NODE, _T("Text Align"), iNode::s_bl);
		} else if (pNode->vert == 0 && pNode->horiz == 1) {
			pApp->WriteProfileInt(REGS_NODE, _T("Text Align"), iNode::s_tc);
		} else if (pNode->vert == 1 && pNode->horiz == 1) {
			pApp->WriteProfileInt(REGS_NODE, _T("Text Align"), iNode::s_cc);
		} else if (pNode->vert == 2 && pNode->horiz == 1) {
			pApp->WriteProfileInt(REGS_NODE, _T("Text Align"), iNode::s_bc);
		} else if (pNode->vert == 0 && pNode->horiz == 2) {
			pApp->WriteProfileInt(REGS_NODE, _T("Text Align"), iNode::s_tr);
		} else if (pNode->vert == 1 && pNode->horiz == 2) {
			pApp->WriteProfileInt(REGS_NODE, _T("Text Align"), iNode::s_cr);
		} else if (pNode->vert == 2 && pNode->horiz == 2) {
			pApp->WriteProfileInt(REGS_NODE, _T("Text Align"), iNode::s_br);
		}
	} else if (pNode->m_rdTLine == 1) {
		if (pNode->horiz == 0) {
			pApp->WriteProfileInt(REGS_NODE, _T("Text Align"), iNode::m_l);
		} else if (pNode->horiz == 1) {
			pApp->WriteProfileInt(REGS_NODE, _T("Text Align"), iNode::m_c);
		} else if (pNode->horiz == 2) {
			pApp->WriteProfileInt(REGS_NODE, _T("Text Align"), iNode::m_r);
		}
	} else if (pNode->m_rdTLine == 2) {
		pApp->WriteProfileInt(REGS_NODE, _T("Text Align"), iNode::notext);
	}
	
	pApp->WriteProfileString(REGS_NODE, _T("Font Name"), pNode->lf.lfFaceName);
	pApp->WriteProfileInt(REGS_NODE, _T("Font Height"), pNode->lf.lfHeight);
	pApp->WriteProfileInt(REGS_NODE, _T("Font Width"), pNode->lf.lfWidth);
	pApp->WriteProfileInt(REGS_NODE, _T("Font Italic"), pNode->lf.lfItalic);
	pApp->WriteProfileInt(REGS_NODE, _T("Font UnderLine"), pNode->lf.lfUnderline);
	pApp->WriteProfileInt(REGS_NODE, _T("Font StrikeOut"), pNode->lf.lfStrikeOut);
	pApp->WriteProfileInt(REGS_NODE, _T("Font CharSet"), pNode->lf.lfCharSet);
	pApp->WriteProfileInt(REGS_NODE, _T("Font Weight"), pNode->lf.lfWeight);
	pApp->WriteProfileInt(REGS_NODE, _T("Sync Order"),  pNode->m_bSyncOrder);
	pApp->WriteProfileInt(REGS_NODE, _T("Order Direction"), pNode->m_orderDirection);
	pApp->WriteProfileInt(REGS_NODE, _T("Enable Grouping"), pNode->m_bEnableGroup);
	pApp->WriteProfileInt(REGS_NODE, _T("Disable NodeResize"), pNode->m_bDisableNodeResize);

	pApp->WriteProfileInt(REGS_NODE, _T("Margin Left"), pNode->margins.l);
	pApp->WriteProfileInt(REGS_NODE, _T("Margin Right"), pNode->margins.r);
	pApp->WriteProfileInt(REGS_NODE, _T("Margin Top"), pNode->margins.t);
	pApp->WriteProfileInt(REGS_NODE, _T("Margin Bottom"), pNode->margins.b);
	pApp->WriteProfileInt(REGS_NODE, _T("Prior Selected Node Dragging"), pNode->m_bPriorSelectionDragging);
	
	pApp->getNodeProfile(); // アプリケーションオブジェクトの再設定
}

void CPropertyDlg::writePageLink()
{
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	pApp->WriteProfileInt(REGS_LINK, _T("Line Color"), pLink->colorLine);
	pApp->WriteProfileInt(REGS_LINK, _T("Line Width"), pLink->lineWidth);
	pApp->WriteProfileInt(REGS_LINK, _T("Strength"), pLink->strength);
	pApp->WriteProfileInt(REGS_LINK, _T("Line Style"), pLink->styleLine);
	pApp->WriteProfileInt(REGS_LINK, _T("Set Strength"), pLink->m_bSetStrength);
	
	pApp->WriteProfileString(REGS_LINK, _T("Font Name"), pLink->lf.lfFaceName);
	pApp->WriteProfileInt(REGS_LINK, _T("Font Height"), pLink->lf.lfHeight);
	pApp->WriteProfileInt(REGS_LINK, _T("Font Width"), pLink->lf.lfWidth);
	pApp->WriteProfileInt(REGS_LINK, _T("Font Italic"), pLink->lf.lfItalic);
	pApp->WriteProfileInt(REGS_LINK, _T("Font UnderLine"), pLink->lf.lfUnderline);
	pApp->WriteProfileInt(REGS_LINK, _T("Font StrikeOut"), pLink->lf.lfStrikeOut);
	pApp->WriteProfileInt(REGS_LINK, _T("Font CharSet"), pLink->lf.lfCharSet);
	pApp->WriteProfileInt(REGS_LINK, _T("Font Weight"), pLink->lf.lfWeight);
	
	pApp->getLinkProfile();
}


void CPropertyDlg::initPageOther()
{
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	pOther->m_bShowHS = pApp->GetProfileInt(REGS_OTHER, _T("Show HScroll"), FALSE);
	pOther->m_tabSelect = pApp->GetProfileInt(REGS_OTHER, _T("Tab Stop"), 1);
	
	pOther->m_bInheritParent = pApp->m_rgsNode.bInheritParent;
	pOther->m_bInheritSibling = pApp->m_rgsNode.bInheritSibling;
	pOther->m_bAccelmove = pApp->GetProfileInt(REGS_OTHER, _T("Accel Move"), TRUE);
	pOther->m_bDrawUnderLine = pApp->GetProfileInt(REGS_OTHER, _T("Draw Underline"), TRUE);
	pOther->m_bOpenFilesAfterExport = pApp->GetProfileInt(REGS_OTHER, _T("Open Files After Export"), TRUE);
	pOther->m_bOutputFileLinksOnExport = pApp->GetProfileInt(REGS_OTHER, _T("Output Filelinks On Export"), FALSE);
	pOther->m_bSetStylesheet = pApp->GetProfileInt(REGS_OTHER, _T("XML StyleSheet"), TRUE);
	pOther->m_strStylesheet = pApp->GetProfileString(REGS_OTHER, _T("XML StyleSheet Name"), _T("iedit.xsl"));
}

void CPropertyDlg::writePageOther()
{
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	pApp->WriteProfileInt(REGS_OTHER, _T("Show HScroll"), pOther->m_bShowHS);
	pApp->WriteProfileInt(REGS_OTHER, _T("Tab Stop"), pOther->m_tabSelect);
	pApp->WriteProfileInt(REGS_NODE, _T("Inherit Parent"), pOther->m_bInheritParent);
	pApp->WriteProfileInt(REGS_NODE, _T("Inherit Sibling"), pOther->m_bInheritSibling);
	pApp->WriteProfileInt(REGS_OTHER, _T("Accel Move"), pOther->m_bAccelmove);
	pApp->WriteProfileInt(REGS_OTHER, _T("Draw Underline"), pOther->m_bDrawUnderLine);
	pApp->WriteProfileInt(REGS_OTHER, _T("Open Files After Export"), pOther->m_bOpenFilesAfterExport);
	pApp->WriteProfileInt(REGS_OTHER, _T("Output Filelinks On Export"), pOther->m_bOutputFileLinksOnExport);
	pApp->WriteProfileInt(REGS_OTHER, _T("XML StyleSheet"), pOther->m_bSetStylesheet);
	pApp->WriteProfileString(REGS_OTHER, _T("XML StyleSheet Name"), pOther->m_strStylesheet);
	
	pApp->m_rgsNode.bInheritParent = pOther->m_bInheritParent;
	pApp->m_rgsNode.bInheritSibling = pOther->m_bInheritSibling;
	
	pApp->m_rgsOther.bSetStylesheet = pOther->m_bSetStylesheet;
	pApp->m_rgsOther.strStyleSheetFile = pOther->m_strStylesheet;
	pApp->m_rgsOther.bOpenFilesAfterExport = pOther->m_bOpenFilesAfterExport;
	pApp->m_rgsOther.bOutputFileLinksOnExport = pOther->m_bOutputFileLinksOnExport;
}

void CPropertyDlg::initPageFrame()
{
	SystemConfiguration sc;
	CString defaultFontName = _T("MS UI Gothic");
	if (sc.isMeiryoUiEnabled()) {
		defaultFontName = _T("Meiryo UI");
	}
	CiEditApp* app = (CiEditApp*)AfxGetApp();
	// OullineView Font
	::lstrcpy(pFrame->lfOutline.lfFaceName, app->GetProfileString(REGS_FRAME, _T("Font1 Name"), defaultFontName));
	pFrame->lfOutline.lfHeight = app->GetProfileInt(REGS_FRAME, _T("Font1 Height"), 0xfffffff3);
	pFrame->lfOutline.lfWidth = app->GetProfileInt(REGS_FRAME, _T("Font1 Width"), 0);
	pFrame->lfOutline.lfItalic = app->GetProfileInt(REGS_FRAME, _T("Font1 Italic"), FALSE);
	pFrame->lfOutline.lfUnderline = app->GetProfileInt(REGS_FRAME, _T("Font1 UnderLine"), FALSE);
	pFrame->lfOutline.lfStrikeOut = app->GetProfileInt(REGS_FRAME, _T("Font1 StrikeOut"), FALSE);
	pFrame->lfOutline.lfCharSet= app->GetProfileInt(REGS_FRAME, _T("Font1 CharSet"), SHIFTJIS_CHARSET);
	pFrame->lfOutline.lfWeight = app->GetProfileInt(REGS_FRAME, _T("Font1 Weight"), FW_NORMAL);
	pFrame->lfOutline.lfEscapement = 0;
	pFrame->fntOutline.CreateFontIndirect(&pFrame->lfOutline);
	
	// LinkView Font
	::lstrcpy(pFrame->lfLink.lfFaceName, app->GetProfileString(REGS_FRAME, _T("Font2 Name"), defaultFontName));
	pFrame->lfLink.lfHeight = app->GetProfileInt(REGS_FRAME, _T("Font2 Height"), 0xfffffff3);
	pFrame->lfLink.lfWidth = app->GetProfileInt(REGS_FRAME, _T("Font2 Width"), 0);
	pFrame->lfLink.lfItalic = app->GetProfileInt(REGS_FRAME, _T("Font2 Italic"), FALSE);
	pFrame->lfLink.lfUnderline = app->GetProfileInt(REGS_FRAME, _T("Font2 UnderLine"), FALSE);
	pFrame->lfLink.lfStrikeOut = app->GetProfileInt(REGS_FRAME, _T("Font2 StrikeOut"), FALSE);
	pFrame->lfLink.lfCharSet= app->GetProfileInt(REGS_FRAME, _T("Font2 CharSet"), SHIFTJIS_CHARSET);
	pFrame->lfLink.lfWeight = app->GetProfileInt(REGS_FRAME, _T("Font2 Weight"), FW_NORMAL);
	pFrame->lfLink.lfEscapement = 0;
	pFrame->fntLink.CreateFontIndirect(&pFrame->lfLink);
	
	// TextView Font
	::lstrcpy(pFrame->lfText.lfFaceName, app->GetProfileString(REGS_FRAME, _T("Font3 Name"), defaultFontName));
	pFrame->lfText.lfHeight = app->GetProfileInt(REGS_FRAME, _T("Font3 Height"), 0xfffffff1);
	pFrame->lfText.lfWidth = app->GetProfileInt(REGS_FRAME, _T("Font3 Width"), 0);
	pFrame->lfText.lfItalic = app->GetProfileInt(REGS_FRAME, _T("Font3 Italic"), FALSE);
	pFrame->lfText.lfUnderline = app->GetProfileInt(REGS_FRAME, _T("Font3 UnderLine"), FALSE);
	pFrame->lfText.lfStrikeOut = app->GetProfileInt(REGS_FRAME, _T("Font3 StrikeOut"), FALSE);
	pFrame->lfText.lfCharSet= app->GetProfileInt(REGS_FRAME, _T("Font3 CharSet"), SHIFTJIS_CHARSET);
	pFrame->lfText.lfWeight = app->GetProfileInt(REGS_FRAME, _T("Font3 Weight"), FW_NORMAL);
	pFrame->lfText.lfEscapement = 0;
	pFrame->fntText.CreateFontIndirect(&pFrame->lfText);
	
	// TreeView Color
	pFrame->m_colorOLBG = app->GetProfileInt(REGS_FRAME, _T("Outline bgColor"), app->m_colorOutlineViewBg);
	pFrame->m_colorOLFor = app->GetProfileInt(REGS_FRAME, _T("Outline forColor"), app->m_colorOutlineViewFg);
	pFrame->m_colorInsrtMrk = app->GetProfileInt(REGS_FRAME, _T("InsertMark Color"), RGB(0, 0, 0));
	
	// LinkView Color
	pFrame->m_colorLNBG = app->GetProfileInt(REGS_FRAME, _T("Link bgColor"), app->m_colorLinkViewBg);
	pFrame->m_colorLNFor = app->GetProfileInt(REGS_FRAME, _T("Link forColor"), app->m_colorLinkViewFg);
	
	// EditerView Color
	pFrame->m_colorEditBG = app->GetProfileInt(REGS_FRAME, _T("Edit bgColor"), app->m_colorTextViewBg);
	pFrame->m_colorEditFor = app->GetProfileInt(REGS_FRAME, _T("Edit forColor"), app->m_colorTextViewFg);
	
	// NetView Color
	pFrame->m_colorNWBG = app->GetProfileInt(REGS_FRAME, _T("Net bgColor"), app->m_colorNetViewBg);
	
	// Frame Option
	pFrame->m_bSaveFrame = app->GetProfileInt(REGS_FRAME, _T("Save Frame Sizes"), TRUE);
	
	// Bar State Option
	pFrame->m_bSaveBarState = app->GetProfileInt(REGS_FRAME, _T("Save bar status"), TRUE);
}

void CPropertyDlg::writePageFrame()
{
	// OullineView Font
	AfxGetApp()->WriteProfileString(REGS_FRAME, _T("Font1 Name"), pFrame->lfOutline.lfFaceName);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font1 Height"), pFrame->lfOutline.lfHeight);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font1 Width"), pFrame->lfOutline.lfWidth);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font1 Italic"), pFrame->lfOutline.lfItalic);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font1 UnderLine"), pFrame->lfOutline.lfUnderline);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font1 StrikeOut"), pFrame->lfOutline.lfStrikeOut);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font1 CharSet"), pFrame->lfOutline.lfCharSet);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font1 Weight"), pFrame->lfOutline.lfWeight);
	
	// LinkView Font
	AfxGetApp()->WriteProfileString(REGS_FRAME, _T("Font2 Name"), pFrame->lfLink.lfFaceName);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font2 Height"), pFrame->lfLink.lfHeight);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font2 Width"), pFrame->lfLink.lfWidth);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font2 Italic"), pFrame->lfLink.lfItalic);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font2 UnderLine"), pFrame->lfLink.lfUnderline);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font2 StrikeOut"), pFrame->lfLink.lfStrikeOut);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font2 CharSet"), pFrame->lfLink.lfCharSet);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font2 Weight"), pFrame->lfLink.lfWeight);
	
	// EditorView Font
	AfxGetApp()->WriteProfileString(REGS_FRAME, _T("Font3 Name"), pFrame->lfText.lfFaceName);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font3 Height"), pFrame->lfText.lfHeight);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font3 Width"), pFrame->lfText.lfWidth);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font3 Italic"), pFrame->lfText.lfItalic);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font3 UnderLine"), pFrame->lfText.lfUnderline);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font3 StrikeOut"), pFrame->lfText.lfStrikeOut);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font3 CharSet"), pFrame->lfText.lfCharSet);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Font3 Weight"), pFrame->lfText.lfWeight);
	
	// TreeView Color
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Outline bgColor"), pFrame->m_colorOLBG);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Outline forColor"), pFrame->m_colorOLFor);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("InsertMark Color"), pFrame->m_colorInsrtMrk);
	
	// LinkView Color
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Link bgColor"), pFrame->m_colorLNBG);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Link forColor"), pFrame->m_colorLNFor);
	
	// EditView Color
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Edit bgColor"), pFrame->m_colorEditBG);
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Edit forColor"), pFrame->m_colorEditFor);
	
	// NetView Color
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Net bgColor"), pFrame->m_colorNWBG);
	
	// Frame Option
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Save Frame Sizes"), pFrame->m_bSaveFrame);
	
	// Bar State Option
	AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Save bar status"), pFrame->m_bSaveBarState);
}
