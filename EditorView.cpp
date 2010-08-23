// EditorView.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "iEdit.h"
#include "EditorView.h"
#include "iEditDoc.h"

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
	// �W������R�}���h
	ON_COMMAND(ID_FILE_PRINT, CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CEditView::OnFilePrintPreview)
	ON_CONTROL_REFLECT(EN_VSCROLL, &EditorView::OnEnVscroll)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EditorView �`��

void EditorView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: ���̈ʒu�ɕ`��p�̃R�[�h��ǉ����Ă�������
}

/////////////////////////////////////////////////////////////////////////////
// EditorView �f�f

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
// EditorView ���b�Z�[�W �n���h��

BOOL EditorView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: ���̈ʒu�ɌŗL�̏�����ǉ����邩�A�܂��͊�{�N���X���Ăяo���Ă�������
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
	// TODO: ���̈ʒu�ɌŗL�̏�����ǉ����邩�A�܂��͊�{�N���X���Ăяo���Ă�������
	CString t = GetDocument()->getSelectedNodeText();
	GetEditCtrl().SetWindowText(t);
	m_preKey = GetDocument()->getSelectedNodeKey();
	m_bDrawUnderLine = AfxGetApp()->GetProfileInt(REGS_OTHER, _T("Draw Underline"), FALSE);
	initSizeChar();
}

void EditorView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	// TODO: ���̈ʒu�ɌŗL�̏�����ǉ����邩�A�܂��͊�{�N���X���Ăяo���Ă�������
	DWORD curKey = 	GetDocument()->getSelectedNodeKey();
	m_bPreUpdateReplace = true;
	if (curKey != m_preKey) {
		//CString s; s.Format(_T("pre:%d cur:%d"), curKey, m_preKey);DEBUG_WRITE(s);
		m_nCaretLine = GetCaretLine(); // �e�X�g
		m_preKey = curKey;
		CString t = GetDocument()->getSelectedNodeText();
		GetEditCtrl().SetWindowText(t);
		m_bPreUpdateReplace = false;
	}
	
	iHint* ph = NULL;
	if (pHint != NULL) {
		ph = reinterpret_cast<iHint*>(pHint);
	}
	if (ph == NULL) return;
	if (ph->event == iHint::viewSettingChanged) {
		m_bkColor = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Edit bgColor"), RGB(255, 255, 255));
		m_textColor = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Edit forColor"), RGB(0, 0, 0));
		m_hBrsBack.CreateSolidBrush(m_bkColor);
		Invalidate();
		setViewFont();
		setTabStop();
	}
	GetEditCtrl().LineScroll(GetDocument()->getSelectedNodeScrollPos());
}

int EditorView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CEditView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: ���̈ʒu�ɌŗL�̍쐬�p�R�[�h��ǉ����Ă�������
	m_bkColor = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Edit bgColor"), RGB(255, 255, 255));
	m_textColor = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Edit forColor"), RGB(0, 0, 0));
	m_hBrsBack.CreateSolidBrush(m_bkColor);
	
	setViewFont();
	setTabStop();
	return 0;
}

iEditDoc* EditorView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(iEditDoc)));
	return (iEditDoc*)m_pDocument;
}

void EditorView::initSizeChar()
{
	CClientDC dc(this);
	dc.SelectObject(m_font);
	m_sizeChar = dc.GetOutputTextExtent(_T("W"));
	m_nCaretLine = GetCaretLine();
}

void EditorView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������
	CString t; GetEditCtrl().GetWindowText(t);
	GetDocument()->setCurNodeText(t, GetEditCtrl().GetFirstVisibleLine());
	
	if (nChar == VK_ESCAPE) {
		GetDocument()->selChanged(m_preKey, false, GetDocument()->isShowSubBranch());
	}
	
	CEditView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void EditorView::OnDelete() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	GetEditCtrl().SendMessage(WM_KEYDOWN, VK_DELETE, VK_DELETE);
	GetDocument()->SetModifiedFlag();
}

void EditorView::OnUpdateDelete(CCmdUI* pCmdUI) 
{
	// TODO: ���̈ʒu�� command update UI �n���h���p�̃R�[�h��ǉ����Ă�������
	
}

void EditorView::setTabStop()
{
	int tabSelect = AfxGetApp()->GetProfileInt(REGS_OTHER, _T("Tab Stop"), 2);
	int tab = 16;
	switch (tabSelect) {
	case 0: tab = 8; break;
	case 1: tab = 16; break;
	case 2: tab = 32; break;
	}
	SetTabStops(tab);
}

void EditorView::setViewFont()
{
	LOGFONT lf;
	CFont *pFont = GetFont();
	if (pFont != NULL) {
		pFont->GetObject(sizeof(LOGFONT), &lf);
	} else {
		::GetObject(GetStockObject(SYSTEM_FIXED_FONT), sizeof(LOGFONT), &lf);
	}
	
	::lstrcpy(lf.lfFaceName, AfxGetApp()->GetProfileString(REGS_FRAME, _T("Font3 Name"), _T("�l�r �S�V�b�N")));
	lf.lfHeight = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font3 Height"), 0xffffffed);
	lf.lfWidth = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font3 Width"), 0);
	lf.lfItalic = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font3 Italic"), FALSE);
	lf.lfUnderline = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font3 UnderLine"), FALSE);
	lf.lfStrikeOut = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font3 StrikeOut"), FALSE);
	lf.lfCharSet= AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font3 CharSet"), SHIFTJIS_CHARSET);
	lf.lfWeight = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font3 Weight"), FW_NORMAL);
	
	m_font.CreateFontIndirect(&lf);
	SetFont(&m_font, TRUE);
	
	initSizeChar();
}

void EditorView::OnEditLabel() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	
}

void EditorView::OnEditCut() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	GetEditCtrl().Cut();
	CString t; GetEditCtrl().GetWindowText(t);
	GetDocument()->setCurNodeText(t, GetEditCtrl().GetFirstVisibleLine());
}

void EditorView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	// TODO: ���̈ʒu�� command update UI �n���h���p�̃R�[�h��ǉ����Ă�������
	int start, end;
	GetEditCtrl().GetSel(start, end);
	pCmdUI->Enable(start < end);
}

void EditorView::OnEditPaste() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	GetEditCtrl().Paste();
	CString t; GetEditCtrl().GetWindowText(t);
	GetDocument()->setCurNodeText(t, GetEditCtrl().GetFirstVisibleLine());
}

void EditorView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	// TODO: ���̈ʒu�� command update UI �n���h���p�̃R�[�h��ǉ����Ă�������
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
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	GetEditCtrl().Undo();
	CString t; GetEditCtrl().GetWindowText(t);
	GetDocument()->setCurNodeText(t, GetEditCtrl().GetFirstVisibleLine());
}

void EditorView::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	// TODO: ���̈ʒu�� command update UI �n���h���p�̃R�[�h��ǉ����Ă�������
	pCmdUI->Enable(GetEditCtrl().CanUndo());
}

void EditorView::OnEditClear() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
	GetEditCtrl().Clear();
	CString t; GetEditCtrl().GetWindowText(t);
	GetDocument()->setCurNodeText(t, GetEditCtrl().GetFirstVisibleLine());
}

void EditorView::OnUpdateEditClear(CCmdUI* pCmdUI) 
{
	// TODO: ���̈ʒu�� command update UI �n���h���p�̃R�[�h��ǉ����Ă�������
	int start, end;
	GetEditCtrl().GetSel(start, end);
	pCmdUI->Enable(start < end);
}

void EditorView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����Ă�������
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
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
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
	// TODO: ���̈ʒu�� command update UI �n���h���p�̃R�[�h��ǉ����Ă�������
	
}

void EditorView::OnEditReplace() 
{
	// TODO: ���̈ʒu�ɃR�}���h �n���h���p�̃R�[�h��ǉ����Ă�������
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
	// TODO: ���̈ʒu�� command update UI �n���h���p�̃R�[�h��ǉ����Ă�������
	
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
	} else {
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
	GetDocument()->setCurNodeText(cText, GetEditCtrl().GetFirstVisibleLine());
	GetDocument()->SetModifiedFlag();
	CString mes; mes.Format(_T("%d�̕������u�����܂���"), rep);
	MessageBox(mes, _T("�u���̏I��"), MB_OK);
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
	GetDocument()->setCurNodeText(t, GetEditCtrl().GetFirstVisibleLine());
}

HBRUSH EditorView::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: ���̈ʒu�� DC �̃A�g���r���[�g��ύX���Ă�������
	pDC->SetBkColor(m_bkColor);
	pDC->SetTextColor(m_textColor);
	return m_hBrsBack;
	
	// TODO: �e�n���h�����Ăяo�������Ȃ��ꍇ�͂����Ńu���V�̃n���h���Ƃ��� NULL �ȊO��Ԃ��悤�ɂ��Ă�������
	return NULL;
}

void EditorView::GetLineRect(int nLine, LPRECT lpRect) const
{
	if (nLine == 0)		// the first line;
	{
		GetEditCtrl().GetRect(lpRect);
		lpRect->bottom = lpRect->top + m_sizeChar.cy;
	}
	else if (nLine == GetEditCtrl().GetLineCount() - 1)		// the last line
	{
		// we get previous line's rect, then offset it by one line height.
		int nLineIndex = GetEditCtrl().LineIndex(nLine - 1);
		CPoint ptPos = GetEditCtrl().PosFromChar(nLineIndex);
		GetEditCtrl().GetRect(lpRect);
		lpRect->top = ptPos.y;
		lpRect->bottom = lpRect->top + m_sizeChar.cy;
		OffsetRect(lpRect, 0, m_sizeChar.cy);
	}
	else	// lines between first and last
	{
		int nLineIndex = GetEditCtrl().LineIndex(nLine);
		CPoint ptPos = GetEditCtrl().PosFromChar(nLineIndex);
		GetEditCtrl().GetRect(lpRect);
		lpRect->top = ptPos.y;
		lpRect->bottom = lpRect->top + m_sizeChar.cy;
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
	// the caret position is not always the end of selection
	if (ptEnd.y == ptCaret.y)
		return GetEditCtrl().LineFromChar(nEnd);
	else
		return GetEditCtrl().LineFromChar(nStart);
}

void EditorView::DrawCaretLine(BOOL bInPaint)
{
	int nLine = GetCaretLine();
	// for effective we need not redraw when we
	// just move caret in the same line using arrow keys, simply return.
	if (nLine == m_nCaretLine && !bInPaint)
		return;

	CRect rectClip;
	GetEditCtrl().GetRect(rectClip);
	CClientDC dc(this);
	dc.IntersectClipRect(rectClip);

	int nLineFirst = GetEditCtrl().GetFirstVisibleLine();
	int nLineLast = GetEditCtrl().LineFromChar(GetEditCtrl().CharFromPos(rectClip.BottomRight()));

	// hide caret, else it will be ugly.
	HideCaret();

	if (m_nCaretLine >= nLineFirst && m_nCaretLine <= nLineLast)
	{
		// in this section we must not make WM_PAINT a loop
		// so don't let OnPaint() call our DrawCaretLine()
		m_bCanPaint = FALSE;
		CRect rect;
		GetLineRect(m_nCaretLine, rect);
		InvalidateRect(rect, FALSE);
		// update immediately
		UpdateWindow();
		m_bCanPaint = TRUE;
	}

	// we change the caret line color by ROP
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
		// "capture" the line into our memory dc, and "INVERT" it
		dcMem.BitBlt(0, 0, rect.Width(), rect.Height(), &dc, rect.left, rect.top, SRCINVERT);
		// blt it back to origin place, but change colors
		dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pSaveBmp);
	}

	ShowCaret();
	m_nCaretLine = nLine;
}

void EditorView::OnPaint() 
{
//	CPaintDC dc(this); // �`��p�̃f�o�C�X �R���e�L�X�g
	
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����Ă�������
	Default();
	if (m_bDrawUnderLine && m_bCanPaint)
		DrawCaretLine(TRUE);
	
	// �`��p���b�Z�[�W�Ƃ��� CEditView::OnPaint() ���Ăяo���Ă͂����܂���
}

void EditorView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������
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
	
//	CEditView::OnChar(nChar, nRepCnt, nFlags);
}

void EditorView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������
	Default();
	if (m_bDrawUnderLine && m_bCanPaint)
		DrawCaretLine();
	
//	CEditView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void EditorView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������
	Default();
	if (m_bDrawUnderLine && m_bCanPaint)
		DrawCaretLine();
	
//	CEditView::OnLButtonDown(nFlags, point);
}

void EditorView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����邩�܂��̓f�t�H���g�̏������Ăяo���Ă�������
	Default();
	if (nFlags & MK_LBUTTON)
	{
	int nLine = GetCaretLine();
//	if (m_nCaretLine != nLine)
	{
		CRect rect;
		GetLineRect(m_nCaretLine, rect);
		InvalidateRect(rect, FALSE);
		UpdateWindow();
		m_nCaretLine = nLine;
	}
/*	else
	{
		CRect rect;
		GetLineRect(m_nCaretLine, rect);
		InvalidateRect(rect, FALSE);
		UpdateWindow();
		m_nCaretLine = nLine;
}*/ }	
//	CEditView::OnMouseMove(nFlags, point);
}

void EditorView::OnChange() 
{
	// TODO: ���ꂪ RICHEDIT �R���g���[���̏ꍇ�A�R���g���[���́A lParam �}�X�N
	// ���ł̘_���a�� ENM_CHANGE �t���O�t���� CRichEditCrtl().SetEventMask()
	// ���b�Z�[�W���R���g���[���֑��邽�߂� CEditView::OnInitDialog() �֐����I�[�o�[
	// ���C�h���Ȃ����肱�̒ʒm�𑗂�܂���B
	
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	GetDocument()->SetModifiedFlag();
//	char buf[100];
//	GetEditCtrl().GetLine(m_nCaretLine, buf);
//	CString caretLine(buf);
//	int start = caretLine.Find("http:/");
//	if (start != -1) {
//		AfxMessageBox("�n�P�[��");
//	}
}

void EditorView::OnEnVscroll()
{
	// TODO: �����ɃR���g���[���ʒm�n���h�� �R�[�h��ǉ����܂��B
	if (!m_bPreUpdateReplace) {
		GetDocument()->setSelectedNodeScrollPos(GetEditCtrl().GetFirstVisibleLine());
	}
}
