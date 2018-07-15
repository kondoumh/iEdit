// EditorView.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "EditorView.h"
#include "iEditDoc.h"
#include "SystemConfiguration.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGS_OTHER _T("Settings")
#define REGS_FRAME _T("Frame Options")

/////////////////////////////////////////////////////////////////////////////
// EditorView

IMPLEMENT_DYNCREATE(EditorView, CEditView)

EditorView::EditorView()
{
	m_bCanPaint = TRUE;
	m_bPreUpdateReplace = false;
}

EditorView::~EditorView()
{
}

BEGIN_MESSAGE_MAP(EditorView, CEditView)
	//{{AFX_MSG_MAP(EditorView)
	ON_WM_CREATE()
	ON_WM_KEYUP()
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_UPDATE_COMMAND_UI(ID_DELETE, OnUpdateDelete)
	ON_COMMAND(ID_EDIT_LABEL, OnEditLabel)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_FIND, OnEditFind)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FIND, OnUpdateEditFind)
	ON_COMMAND(ID_EDIT_REPLACE, OnEditReplace)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REPLACE, OnUpdateEditReplace)
	ON_WM_PAINT()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	//}}AFX_MSG_MAP
	// 標準印刷コマンド
	ON_COMMAND(ID_FILE_PRINT, CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CEditView::OnFilePrintPreview)
	ON_CONTROL_REFLECT(EN_VSCROLL, &EditorView::OnEnVscroll)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void EditorView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
}

/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
void EditorView::AssertValid() const
{
	CEditView::AssertValid();
}

void EditorView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////

BOOL EditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL shwHScroll = AfxGetApp()->GetProfileInt(REGS_OTHER, _T("Show HScroll"), FALSE);
	cs.style |= WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_NOHIDESEL;
	if (shwHScroll) {
		cs.style |= WS_HSCROLL;
	}
	return CEditView::PreCreateWindow(cs);
}

void EditorView::OnInitialUpdate()
{
	CEditView::OnInitialUpdate();
	CString t = GetDocument()->GetSelectedNodeText();
	GetEditCtrl().SetWindowText(t);
	m_preKey = GetDocument()->GetSelectedNodeKey();
	m_bDrawUnderLine = AfxGetApp()->GetProfileInt(REGS_OTHER, _T("Draw Underline"), TRUE);
	UpdateTextExtent();
}

void EditorView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	DWORD curKey = GetDocument()->GetSelectedNodeKey();
	m_bPreUpdateReplace = false;
	if (curKey != m_preKey) {
		m_bPreUpdateReplace = true;
		m_nCaretLine = GetCaretLine();
		m_preKey = curKey;
		CString t = GetDocument()->GetSelectedNodeText();
		GetEditCtrl().SetWindowText(t);
	}

	iHint* ph = NULL;
	if (pHint != NULL) {
		ph = reinterpret_cast<iHint*>(pHint);
	}
	CiEditApp* app = (CiEditApp*)AfxGetApp();
	if (ph != NULL && ph->event == iHint::viewSettingChanged) {
		m_bkColor = app->GetProfileInt(REGS_FRAME, _T("Edit bgColor"), app->m_colorTextViewBg);
		m_textColor = app->GetProfileInt(REGS_FRAME, _T("Edit forColor"), app->m_colorTextViewFg);
		m_hBrsBack.CreateSolidBrush(m_bkColor);
		Invalidate();
		SetViewFont();
		SetTabStop();
	}
	if (m_bPreUpdateReplace) {
		GetEditCtrl().LineScroll(GetDocument()->GetSelectedNodeScrollPos());
	}
}

int EditorView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEditView::OnCreate(lpCreateStruct) == -1)
		return -1;
	CiEditApp* app = (CiEditApp*)AfxGetApp();
	m_bkColor = app->GetProfileInt(REGS_FRAME, _T("Edit bgColor"), app->m_colorTextViewBg);
	m_textColor = app->GetProfileInt(REGS_FRAME, _T("Edit forColor"), app->m_colorTextViewFg);
	m_hBrsBack.CreateSolidBrush(m_bkColor);

	SetViewFont();
	SetTabStop();
	return 0;
}

iEditDoc* EditorView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(iEditDoc)));
	return (iEditDoc*)m_pDocument;
}

void EditorView::UpdateTextExtent()
{
	CClientDC dc(this);
	dc.SelectObject(m_font);
	m_textExtent = dc.GetOutputTextExtent(_T("W"));
	m_nCaretLine = GetCaretLine();
}

void EditorView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString t; GetEditCtrl().GetWindowText(t);
	GetDocument()->SetCurrentNodeText(t, GetEditCtrl().GetFirstVisibleLine());

	if (nChar == VK_ESCAPE) {
		GetDocument()->SelectionChanged(m_preKey, false, GetDocument()->ShowSubBranch());
	}

	CEditView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void EditorView::OnDelete()
{
	GetEditCtrl().SendMessage(WM_KEYDOWN, VK_DELETE, VK_DELETE);
	GetDocument()->SetModifiedFlag();
}

void EditorView::OnUpdateDelete(CCmdUI* pCmdUI)
{
}

void EditorView::SetTabStop()
{
	int tabSelect = AfxGetApp()->GetProfileInt(REGS_OTHER, _T("Tab Stop"), 1);
	int tab = 16;
	switch (tabSelect) {
	case 0: tab = 8; break;
	case 1: tab = 16; break;
	case 2: tab = 32; break;
	}
	SetTabStops(tab);
}

void EditorView::SetViewFont()
{
	LOGFONT lf;
	CFont *pFont = GetFont();
	if (pFont != NULL) {
		pFont->GetObject(sizeof(LOGFONT), &lf);
	}
	else {
		::GetObject(GetStockObject(SYSTEM_FIXED_FONT), sizeof(LOGFONT), &lf);
	}
	CString defaultFont = _T("MS UI Gothic");
	SystemConfiguration sc;
	if (sc.IsMeiryoUiAvailable()) {
		defaultFont = _T("Meiryo UI");
	}
	::lstrcpy(lf.lfFaceName, AfxGetApp()->GetProfileString(REGS_FRAME, _T("Font3 Name"), defaultFont));
	lf.lfHeight = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font3 Height"), 0xffffffed);
	lf.lfWidth = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font3 Width"), 0);
	lf.lfItalic = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font3 Italic"), FALSE);
	lf.lfUnderline = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font3 UnderLine"), FALSE);
	lf.lfStrikeOut = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font3 StrikeOut"), FALSE);
	lf.lfCharSet = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font3 CharSet"), SHIFTJIS_CHARSET);
	lf.lfWeight = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font3 Weight"), FW_NORMAL);

	m_font.CreateFontIndirect(&lf);
	SetFont(&m_font, TRUE);

	UpdateTextExtent();
}

void EditorView::OnEditLabel()
{
}

void EditorView::OnEditCut()
{
	GetEditCtrl().Cut();
	CString t; GetEditCtrl().GetWindowText(t);
	GetDocument()->SetCurrentNodeText(t, GetEditCtrl().GetFirstVisibleLine());
}

void EditorView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	int start, end;
	GetEditCtrl().GetSel(start, end);
	pCmdUI->Enable(start < end);
}

void EditorView::OnEditPaste()
{
	GetEditCtrl().Paste();
	CString t; GetEditCtrl().GetWindowText(t);
	GetDocument()->SetCurrentNodeText(t, GetEditCtrl().GetFirstVisibleLine());
}

void EditorView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	if (!::OpenClipboard(m_hWnd)) {
		pCmdUI->Enable(FALSE);
		return;
	}
	UINT cf = EnumClipboardFormats(CF_TEXT);
	::CloseClipboard();
	pCmdUI->Enable(cf != 0);
}

void EditorView::OnEditUndo()
{
	GetEditCtrl().Undo();
	CString t; GetEditCtrl().GetWindowText(t);
	GetDocument()->SetCurrentNodeText(t, GetEditCtrl().GetFirstVisibleLine());
}

void EditorView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetEditCtrl().CanUndo());
}

void EditorView::OnEditClear()
{
	GetEditCtrl().Clear();
	CString t; GetEditCtrl().GetWindowText(t);
	GetDocument()->SetCurrentNodeText(t, GetEditCtrl().GetFirstVisibleLine());
}

void EditorView::OnUpdateEditClear(CCmdUI* pCmdUI)
{
	int start, end;
	GetEditCtrl().GetSel(start, end);
	pCmdUI->Enable(start < end);
}

void EditorView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_CONTEXT);

	CMenu* pPopup = menu.GetSubMenu(6);
	if (point.x < 0 || point.y < 0) {
		CRect rc; GetWindowRect(&rc);
		point = rc.TopLeft();
	}
	ASSERT(pPopup != NULL);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
}

void EditorView::OnEditFind()
{
	CString cText; GetEditCtrl().GetWindowText(cText);
	int start, end;
	GetEditCtrl().GetSel(start, end);
	CString fstr;
	for (int i = start; i < end; i++) {
		if (cText[i] == '\r') break;
		fstr += cText[i];
	}
	m_pFindReplacedlg = new CFindReplaceDialog;
	m_pFindReplacedlg->Create(TRUE, fstr, NULL, FR_DOWN | FR_HIDEWHOLEWORD, this);
	m_pFindReplacedlg->ShowWindow(SW_SHOW);
}

void EditorView::OnUpdateEditFind(CCmdUI* pCmdUI)
{
}

void EditorView::OnEditReplace()
{
	CString cText; GetEditCtrl().GetWindowText(cText);
	int start, end;
	GetEditCtrl().GetSel(start, end);
	CString fstr;
	for (int i = start; i < end; i++) {
		if (cText[i] == '\r') break;
		fstr += cText[i];
	}
	m_pFindReplacedlg = new CFindReplaceDialog;
	m_pFindReplacedlg->Create(FALSE, fstr, NULL, FR_DOWN | FR_HIDEWHOLEWORD | FR_REPLACEALL | FR_REPLACE, this);
	m_pFindReplacedlg->ShowWindow(SW_SHOW);
}

void EditorView::OnUpdateEditReplace(CCmdUI* pCmdUI)
{
}

void EditorView::OnFindNext(LPCTSTR lpszFind, BOOL bNext, BOOL bCase)
{
	CString cText; GetEditCtrl().GetWindowText(cText);
	int nStart, nEnd;
	GetEditCtrl().GetSel(nStart, nEnd);
	CString findStr = lpszFind;
	if (!bCase) {
		cText.MakeLower();
		findStr.MakeLower();
	}

	if (bNext) {
		CString searchText = cText.Right(cText.GetLength() - nEnd);
		int ns = searchText.Find(findStr);
		if (ns != -1) {
			GetEditCtrl().SetSel(nEnd + ns, nEnd + ns + findStr.GetLength());
		}
	}
	else {
		CString searchText = cText.Left(nStart);
		for (int i = nStart - findStr.GetLength(); i > 0; i--) {
			if (searchText.Right(searchText.GetLength() - i).Find(findStr) != -1) {
				GetEditCtrl().SetSel(i, i + findStr.GetLength());
				break;
			}
		}
	}
}

void EditorView::OnReplaceAll(LPCTSTR lpszFind, LPCTSTR lpszReplace, BOOL bCase)
{
	CString cText; GetEditCtrl().GetWindowText(cText);
	int rep = cText.Replace(lpszFind, lpszReplace);
	GetEditCtrl().SetWindowText(cText);
	GetDocument()->SetCurrentNodeText(cText, GetEditCtrl().GetFirstVisibleLine());
	GetDocument()->SetModifiedFlag();
	CString mes; mes.Format(_T("%d個の文字列を置換しました"), rep);
	MessageBox(mes, _T("置換の終了"), MB_OK);
}

void EditorView::OnReplaceSel(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, LPCTSTR lpszReplace)
{
	CString cText; GetEditCtrl().GetWindowText(cText);
	int nStart, nEnd;
	GetEditCtrl().GetSel(nStart, nEnd);
	CString findStr = lpszFind;
	if (!bCase) {
		cText.MakeLower();
		findStr.MakeLower();
	}

	CString searchText = cText.Right(cText.GetLength() - nStart);
	int ns = searchText.Find(findStr);
	if (ns != -1) {
		GetEditCtrl().SetSel(nStart + ns, nStart + ns + findStr.GetLength());
		GetEditCtrl().ReplaceSel(lpszReplace);
	}
	CString t; GetEditCtrl().GetWindowText(t);
	GetDocument()->SetCurrentNodeText(t, GetEditCtrl().GetFirstVisibleLine());
}

HBRUSH EditorView::CtlColor(CDC* pDC, UINT nCtlColor)
{
	pDC->SetBkColor(m_bkColor);
	pDC->SetTextColor(m_textColor);
	return m_hBrsBack;

	return NULL;
}

void EditorView::GetLineRect(int nLine, LPRECT lpRect) const
{
	if (nLine == 0)
	{
		GetEditCtrl().GetRect(lpRect);
		lpRect->bottom = lpRect->top + m_textExtent.cy;
	}
	else if (nLine == GetEditCtrl().GetLineCount() - 1)
	{
		int nLineIndex = GetEditCtrl().LineIndex(nLine - 1);
		CPoint ptPos = GetEditCtrl().PosFromChar(nLineIndex);
		GetEditCtrl().GetRect(lpRect);
		lpRect->top = ptPos.y;
		lpRect->bottom = lpRect->top + m_textExtent.cy;
		OffsetRect(lpRect, 0, m_textExtent.cy);
	}
	else
	{
		int nLineIndex = GetEditCtrl().LineIndex(nLine);
		CPoint ptPos = GetEditCtrl().PosFromChar(nLineIndex);
		GetEditCtrl().GetRect(lpRect);
		lpRect->top = ptPos.y;
		lpRect->bottom = lpRect->top + m_textExtent.cy;
	}
	lpRect->top = lpRect->bottom - 1;
}

int EditorView::GetCaretLine() const
{
	int nStart, nEnd;
	GetEditCtrl().GetSel(nStart, nEnd);
	ASSERT(nStart <= nEnd);
	CPoint ptEnd, ptCaret;
	ptEnd = GetEditCtrl().PosFromChar(nEnd);
	ptCaret = GetCaretPos();
	if (ptEnd.y == ptCaret.y)
		return GetEditCtrl().LineFromChar(nEnd);
	else
		return GetEditCtrl().LineFromChar(nStart);
}

void EditorView::DrawCaretLine(BOOL bInPaint)
{
	int nLine = GetCaretLine();
	if (nLine == m_nCaretLine && !bInPaint)
		return;

	CRect rectClip;
	GetEditCtrl().GetRect(rectClip);
	CClientDC dc(this);
	dc.IntersectClipRect(rectClip);

	int nLineFirst = GetEditCtrl().GetFirstVisibleLine();
	int nLineLast = GetEditCtrl().LineFromChar(GetEditCtrl().CharFromPos(rectClip.BottomRight()));

	HideCaret();

	if (m_nCaretLine >= nLineFirst && m_nCaretLine <= nLineLast)
	{
		m_bCanPaint = FALSE;
		CRect rect;
		GetLineRect(m_nCaretLine, rect);
		InvalidateRect(rect, FALSE);
		UpdateWindow();
		m_bCanPaint = TRUE;
	}

	if (nLine >= nLineFirst && nLine <= nLineLast)
	{
		CRect rect;
		GetLineRect(nLine, rect);
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		CBitmap bmp;
		bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
		CBitmap* pSaveBmp = dcMem.SelectObject(&bmp);
		CBrush br(RGB(128, 128, 128));
		dcMem.FillRect(CRect(0, 0, rect.Width(), rect.Height()), &br);
		dcMem.BitBlt(0, 0, rect.Width(), rect.Height(), &dc, rect.left, rect.top, SRCINVERT);
		dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pSaveBmp);
	}

	ShowCaret();
	m_nCaretLine = nLine;
}

void EditorView::OnPaint()
{
	Default();
	if (m_bDrawUnderLine && m_bCanPaint)
		DrawCaretLine(TRUE);
}

void EditorView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	Default();
	int nLine = GetCaretLine();
	if (m_nCaretLine == nLine)
	{
		CRect rect;
		GetLineRect(m_nCaretLine, rect);
		InvalidateRect(rect, FALSE);
		UpdateWindow();
	}
	else
	{
		CRect rect;
		GetLineRect(m_nCaretLine, rect);
		InvalidateRect(rect, FALSE);
		UpdateWindow();
		m_nCaretLine = nLine;
	}
}

void EditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	Default();
	if (m_bDrawUnderLine && m_bCanPaint)
		DrawCaretLine();
}

void EditorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	Default();
	if (m_bDrawUnderLine && m_bCanPaint)
		DrawCaretLine();
}

void EditorView::OnMouseMove(UINT nFlags, CPoint point)
{
	Default();
	if (nFlags & MK_LBUTTON) {
		int nLine = GetCaretLine();
		CRect rect;
		GetLineRect(m_nCaretLine, rect);
		InvalidateRect(rect, FALSE);
		UpdateWindow();
		m_nCaretLine = nLine;
	}
}

void EditorView::OnChange()
{
	GetDocument()->SetModifiedFlag();
}

void EditorView::OnEnVscroll()
{
	GetDocument()->SetSelectedNodeScrollPos(GetEditCtrl().GetFirstVisibleLine());
}
