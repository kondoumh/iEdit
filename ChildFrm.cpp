// ChildFrm.cpp : CChildFrame �N���X�̓���̒�`���s���܂��B
//

#include "stdafx.h"
#include "iEdit.h"

#include "ChildFrm.h"

#include "iEditDoc.h"
#include "OutlineView.h"
#include "EditorView.h"
#include "NetView.h"
#include "LinkView.h"

#include "PropertyDlg.h"
#include "nodeSrchDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGS_FRAME _T("Frame Options")
#define REGS_SHAPES _T("Shapes")

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_CLOSE()
	ON_COMMAND(ID_CHANGE_VIEW_FOCUS, OnChangeViewFocus)
	ON_UPDATE_COMMAND_UI(ID_CHANGE_VIEW_FOCUS, OnUpdateChangeViewFocus)
	ON_WM_MDIACTIVATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame �N���X�̍\�z/����

CChildFrame::CChildFrame()
{
	// TODO: �����o�������R�[�h�����̈ʒu�ɒǉ����Ă��������B
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: ���̈ʒu�� CREATESTRUCT cs �̐ݒ���s���āAWindow �N���X�܂���
	//       �X�^�C����ύX���Ă��������B
	cs.style |= WS_CHILD | WS_VISIBLE |WS_OVERLAPPEDWINDOW | WS_MAXIMIZE;
	
	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;
	
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CChildFrame �N���X�̐f�f

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame �N���X�̃��b�Z�[�W�n���h��

BOOL CChildFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: ���̈ʒu�ɌŗL�̏�����ǉ����邩�A�܂��͊�{�N���X���Ăяo���Ă�������
	long cxLeft = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Outline Width"), lpcs->cx*1/3);
	long cyTree = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Outline Height"), lpcs->cy*2/3);
	long cyNet = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Net Height"), lpcs->cy*1/2);
	
	m_Splitter1.CreateStatic(this, 1, 2);
//	m_Splitter1.SetColumnInfo(0, cxLeft, 0);
	m_Splitter2.CreateStatic(&m_Splitter1, 2, 1, WS_CHILD | WS_VISIBLE | WS_BORDER, m_Splitter1.IdFromRowCol(0, 0));
	m_Splitter2.CreateView(0, 0, RUNTIME_CLASS(OutlineView), CSize(cxLeft, cyTree), pContext);
	m_Splitter2.CreateView(1, 0, RUNTIME_CLASS(LinkView), CSize(cxLeft, 0), pContext);
	m_Splitter3.CreateStatic(&m_Splitter1, 2, 1, WS_CHILD | WS_VISIBLE | WS_BORDER, m_Splitter1.IdFromRowCol(0, 1));
	m_Splitter3.CreateView(0, 0, RUNTIME_CLASS(NetView), CSize(0, cyNet), pContext);
	m_Splitter3.CreateView(1, 0, RUNTIME_CLASS(EditorView), CSize(0, 0), pContext);
	
	m_Splitter1.SetColumnInfo(0, cxLeft, 0);
	m_Splitter1.RecalcLayout();
	m_Splitter2.SetRowInfo(0, cyTree, 0);
	m_Splitter2.RecalcLayout();
	m_Splitter3.SetRowInfo(0, cyNet, 0);
	m_Splitter3.RecalcLayout();
	return TRUE;
}

BOOL CChildFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// TODO: ���̈ʒu�ɌŗL�̏�����ǉ����邩�A�܂��͊�{�N���X���Ăяo���Ă�������
	if (CMDIChildWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) {
		return TRUE;
	}
	iEditDoc* pDoc = (iEditDoc*)GetActiveDocument();
	ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(iEditDoc)));
	
	// �R�}���h�t�B���^ (�S���̃r���[�Ƀ��[�e�B���O�������Ȃ��Ƃ��͂����ɒǉ�����
	if (nID == ID_INSERT_SIBLING || nID == ID_INSERT_CHILD) {
		return FALSE;
	}
	
	if (pDoc != NULL) {
		if (pDoc->RouteCmdToAllViews(GetActiveView(), nID, nCode, pExtra, pHandlerInfo)) {
			return TRUE;
		}
	}
	return FALSE;
}

/*void CChildFrame::OnSetProperties() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	CPropertyDlg dlg;
	if (dlg.DoModal() != IDOK) return;
	
	// �e�r���[�̎擾
	iEditDoc* pDoc = (iEditDoc*)GetActiveView()->GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	OutlineView* pOutlineView = (OutlineView*)pDoc->GetNextView(pos);
	LinkView* pLinkView = (LinkView*)pDoc->GetNextView(pos);
	NetView* pNetView = (NetView*)pDoc->GetNextView(pos);
	EditorView* pEditorView = (EditorView*)pDoc->GetNextView(pos);
	
	// �e�r���[�̍Đݒ�
	pOutlineView->setViewFont();
	pLinkView->setViewFont();
	pEditorView->setViewFont();
	
	pEditorView->setTabStop();
}*/


void CChildFrame::OnDestroy() 
{
	CMDIChildWnd::OnDestroy();
	
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����Ă�������
	CMDIFrameWnd* pFrame = GetMDIFrame();
	WINDOWPLACEMENT wndplm;
	pFrame->GetWindowPlacement(&wndplm);
	
	BOOL saveFrame = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Save Frame Sizes"), TRUE);
	if (saveFrame && 
		  (wndplm.showCmd == SW_SHOWNORMAL || wndplm.showCmd == SW_SHOWMAXIMIZED)) {
		WINDOWPLACEMENT wndpl;
		GetWindowPlacement(&wndpl);
		int cur1, min1, cur2, min2, cur3, min3;
		m_Splitter1.GetColumnInfo(0, cur1, min1);
		m_Splitter2.GetRowInfo(0, cur2, min2);
		m_Splitter3.GetRowInfo(0, cur3, min3);
		if (cur1 > 0 && cur1 < 1600 && cur2 > 0 && cur2 <1600 && cur3 > 0 && cur3 < 1600 && 
			wndpl.showCmd == SW_SHOWMAXIMIZED) {
			AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Outline Width"), cur1);
			AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Outline Height"), cur2);
			AfxGetApp()->WriteProfileInt(REGS_FRAME, _T("Net Height"), cur3);
		}
	}
}


void CChildFrame::OnClose() 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������
	CMDIChildWnd::OnClose();
}

void CChildFrame::OnChangeViewFocus() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	iEditDoc* pDoc = (iEditDoc*)GetActiveView()->GetDocument();
	
	POSITION pos = pDoc->GetFirstViewPosition();
	CView* pfirstView = pDoc->GetNextView(pos);
	CView* psecondView = pDoc->GetNextView(pos);
	CView* pthirdView = pDoc->GetNextView(pos);
	CView* pforthView = pDoc->GetNextView(pos);
	
	CView* pcurView = GetActiveView();
	if (pcurView == pfirstView) {
		SetActiveView(pthirdView, TRUE);
	} else if (pcurView == psecondView) {
		SetActiveView(pfirstView, TRUE);
	} else if (pcurView == pthirdView) {
		SetActiveView(pforthView);
	} else if (pcurView == pforthView) {
		SetActiveView(psecondView);
	}
}

void CChildFrame::OnUpdateChangeViewFocus(CCmdUI* pCmdUI) 
{
	// TODO: ���̈ʒu�� command update UI �n���h���p�̃R�[�h��ǉ����Ă�������
	
}

void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd) 
{
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);
	
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����Ă�������
	if (!bActivate) {
		CView* pView = GetActiveView();
		if (pView == NULL) return;
		iEditDoc* pDoc = (iEditDoc*)pView->GetDocument();
		POSITION pos = pDoc->GetFirstViewPosition();
		OutlineView* pOutlineView = (OutlineView*)pDoc->GetNextView(pos);
		LinkView* pLinkView = (LinkView*)pDoc->GetNextView(pos);
		NetView* pNetView = (NetView*)pDoc->GetNextView(pos);
		EditorView* pEditorView = (EditorView*)pDoc->GetNextView(pos);
		pOutlineView->hideModeless();
		pNetView->hideModeless();
	}
}

void CChildFrame::changeSelectedLinkArrow()
{
	iEditDoc* pDoc = (iEditDoc*)GetActiveView()->GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	pDoc->GetNextView(pos);
	pDoc->GetNextView(pos);
	NetView* pNetView = (NetView*)pDoc->GetNextView(pos);
	pNetView->changeSelectedLinkArrow();
}

void CChildFrame::changeSelectedLineWidth()
{
	iEditDoc* pDoc = (iEditDoc*)GetActiveView()->GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	pDoc->GetNextView(pos);
	pDoc->GetNextView(pos);
	NetView* pNetView = (NetView*)pDoc->GetNextView(pos);
	pNetView->changeSelectedLineWidth();
}

void CChildFrame::changeSelectedNodeColor(void)
{
	iEditDoc* pDoc = (iEditDoc*)GetActiveView()->GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	pDoc->GetNextView(pos);
	pDoc->GetNextView(pos);
	NetView* pNetView = (NetView*)pDoc->GetNextView(pos);
	pNetView->changeSelectedNodeColor();
}

void CChildFrame::changeSelectedLineColor(void)
{
	iEditDoc* pDoc = (iEditDoc*)GetActiveView()->GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	pDoc->GetNextView(pos);
	pDoc->GetNextView(pos);
	NetView* pNetView = (NetView*)pDoc->GetNextView(pos);
	pNetView->changeSelectedLineColor();
}

void CChildFrame::changeSelectedFontColor(void)
{
	iEditDoc* pDoc = (iEditDoc*)GetActiveView()->GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	pDoc->GetNextView(pos);
	pDoc->GetNextView(pos);
	NetView* pNetView = (NetView*)pDoc->GetNextView(pos);
	pNetView->changeSelectedFontColor();
}
