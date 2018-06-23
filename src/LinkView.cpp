// LinkView.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "LinkView.h"
#include "iEditDoc.h"
#include "LinkPropertiesDlg.h"
#include "LinkForPathDlg.h"
#include "FileDropActionDlg.h"
#include "SystemConfiguration.h"
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGS_FRAME _T("Frame Options")

int sortBy = 0;

bool operator <(const LinkProps& i1, const LinkProps& i2) {
	if (sortBy == 0) {
		return (i1.comment < i2.comment);
	}
	else if (sortBy == 1) {
		CString s1, s2;
		if (i1.linkType == LinkProps::linkSL || i1.linkType == LinkProps::linkDL) {
			s1 = i1.sTo;
		}
		else if (i1.linkType == LinkProps::FileName ||
			i1.linkType == LinkProps::WebURL ||
			i1.linkType == LinkProps::linkFolder ||
			i1.linkType == LinkProps::iedFile) {
			s1 = i1.path;
		}
		if (i2.linkType == LinkProps::linkSL || i2.linkType == LinkProps::linkDL) {
			s2 = i2.sTo;
		}
		else if (i2.linkType == LinkProps::FileName ||
			i2.linkType == LinkProps::WebURL ||
			i2.linkType == LinkProps::linkFolder ||
			i2.linkType == LinkProps::iedFile) {
			s2 = i2.path;
		}
		return (s1 < s2);
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////
// LinkView

IMPLEMENT_DYNCREATE(LinkView, CListView)

LinkView::LinkView()
{
}

LinkView::~LinkView()
{
}


BEGIN_MESSAGE_MAP(LinkView, CListView)
	//{{AFX_MSG_MAP(LinkView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_COMMAND(ID_JUMP_TO, OnJumpTo)
	ON_UPDATE_COMMAND_UI(ID_JUMP_TO, OnUpdateJumpTo)
	ON_COMMAND(ID_JUMP_BACK, OnJumpBack)
	ON_UPDATE_COMMAND_UI(ID_JUMP_BACK, OnUpdateJumpBack)
	ON_COMMAND(ID_DELETE, OnDelete)
	ON_UPDATE_COMMAND_UI(ID_DELETE, OnUpdateDelete)
	ON_COMMAND(ID_SET_LINK_INFO, OnSetLinkInfo)
	ON_UPDATE_COMMAND_UI(ID_SET_LINK_INFO, OnUpdateSetLinkInfo)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(NM_RETURN, OnReturn)
	ON_WM_KEYUP()
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_COMMAND(ID_EDIT_LABEL, OnEditLabel)
	ON_COMMAND(ID_EDIT_REPLACE, OnEditReplace)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_LABEL, OnUpdateEditLabel)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_LINK_MOVE_UP, &LinkView::OnLinkMoveUp)
	ON_UPDATE_COMMAND_UI(ID_LINK_MOVE_UP, &LinkView::OnUpdateLinkMoveUp)
	ON_COMMAND(ID_LINK_MOVE_DOWN, &LinkView::OnLinkMoveDown)
	ON_UPDATE_COMMAND_UI(ID_LINK_MOVE_DOWN, &LinkView::OnUpdateLinkMoveDown)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// LinkView 描画

void LinkView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
}

/////////////////////////////////////////////////////////////////////////////
// LinkView 診断

#ifdef _DEBUG
void LinkView::AssertValid() const
{
	CListView::AssertValid();
}

void LinkView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// LinkView メッセージ ハンドラ

BOOL LinkView::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style &= ~LVS_TYPEMASK;
	// 複数選択をサポートするには、LVS_SINGLSELをはずす
	cs.style |= LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_EDITLABELS /*| LVS_NOSORTHEADER*/;
	return CListView::PreCreateWindow(cs);
}

void LinkView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	doColorSetting();

	m_preKey = GetDocument()->GetSelectedNodeKey();
	CString s = GetDocument()->GetSelectedNodeLabel();
	GetListCtrl().DeleteColumn(0);
	GetListCtrl().InsertColumn(0, s);
	CRect rc; GetClientRect(&rc);
	GetListCtrl().SetColumnWidth(0, rc.Width() / 2);
	GetListCtrl().SetColumnWidth(1, rc.Width() / 2);
	m_preWidth = rc.Width();

	reflesh();
	setSelection(0);
}

void LinkView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	iHint* ph = NULL;
	if (pHint != NULL) {
		ph = reinterpret_cast<iHint*>(pHint);
	}
	if ((ph != NULL && ph->event == iHint::linkAdd ||
		ph != NULL && ph->event == iHint::linkDelete ||
		ph != NULL && ph->event == iHint::linkSel ||
		ph != NULL && ph->event == iHint::linkModified ||
		ph != NULL && ph->event == iHint::linkDeleteMulti) ||
		(m_preKey != GetDocument()->GetSelectedNodeKey())) {

		CString s = GetDocument()->GetSelectedNodeLabel();
		GetListCtrl().DeleteColumn(0);
		GetListCtrl().InsertColumn(0, s);
		CRect rc; GetClientRect(&rc);
		GetListCtrl().SetColumnWidth(0, rc.Width() / 2);
		GetListCtrl().SetColumnWidth(1, rc.Width() / 2);

		reflesh();
		setSelection(0);

		if (GetListCtrl().GetItemCount() == 0) return;
		int index = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED) - 1;
		CSize sz = GetListCtrl().ApproximateViewRect();
		sz.cy = sz.cy*index / GetListCtrl().GetItemCount();
		GetListCtrl().Scroll(sz);
	}
	if (ph != NULL && ph->event == iHint::viewSettingChanged) {
		setViewFont();
		doColorSetting();
	}
}

void LinkView::listConstruct()
{
	int selindex = 0;
	for (unsigned int i = 0; i < items_.size(); i++) {
		LV_ITEM lvi;
		lvi.iItem = i;
		lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		lvi.iSubItem = 0;
		lvi.lParam = (LPARAM)i;
		lvi.pszText = LPSTR_TEXTCALLBACK;
		lvi.iImage = items_[i].linkType;
		if (items_[i].selected) {
			selindex = i;
		}
		GetListCtrl().InsertItem(&lvi);
	}
	m_preKey = GetDocument()->GetSelectedNodeKey();
}

int LinkView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_oleDropTarget.Register(this);
	GetListCtrl().InsertColumn(0, _T("リンク名"), LVCFMT_LEFT, 100);
	GetListCtrl().InsertColumn(1, _T("リンク先"), LVCFMT_LEFT, 100);
	m_imageList.Create(IDB_LINKS, 16, 1, RGB(255, 0, 255));
	GetListCtrl().SetImageList(&m_imageList, LVSIL_SMALL);
	setViewFont();
	return 0;
}

void LinkView::OnSize(UINT nType, int cx, int cy)
{
	if (cx != m_preWidth) {
		CHeaderCtrl* pHeader = GetListCtrl().GetHeaderCtrl();
		CRect r;
		pHeader->GetItemRect(0, r);
		if (cy >= r.Height()) {
			GetListCtrl().SetColumnWidth(0, cx / 2);
			GetListCtrl().SetColumnWidth(1, cx / 2);
			m_preWidth = cx;
		}
	}
	CListView::OnSize(nType, cx, cy);
}

iEditDoc* LinkView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(iEditDoc)));
	return (iEditDoc*)m_pDocument;
}

void LinkView::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	if (pDispInfo->item.mask & LVIF_TEXT) {
		int index = (int)pDispInfo->item.lParam;
		CString sComment;
		switch (pDispInfo->item.iSubItem) {
		case 0:
			sComment = items_[index].comment;
			if (sComment.GetLength() >= 260) {
				sComment = sComment.Left(259);
			}
			::lstrcpy(pDispInfo->item.pszText, sComment);
			break;
		case 1:
			int type = items_[index].linkType;
			CString sTo;
			if (type == LinkProps::linkSL || type == LinkProps::linkDL ||
				type == LinkProps::linkSL2 || type == LinkProps::linkDL2) {
				sTo = items_[index].sTo;
			}
			else if (type == LinkProps::FileName || type == LinkProps::WebURL || type == LinkProps::linkFolder || type == LinkProps::iedFile) {
				sTo = items_[index].path;
			}
			if (sTo.GetLength() >= 260) {
				sTo = sTo.Left(259);
			}
			::lstrcpy(pDispInfo->item.pszText, sTo);
			break;
		}
	}

	*pResult = 0;
}

void LinkView::OnJumpTo()
{
	jumpTo();
}

void LinkView::OnUpdateJumpTo(CCmdUI* pCmdUI)
{
	int index = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	pCmdUI->Enable(index != -1 ? 1 : 0);
}

void LinkView::OnJumpBack()
{
	jumpBack();
}

void LinkView::OnUpdateJumpBack(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(kstack.size() > 0 ? 1 : 0);
}

void LinkView::OnDelete()
{
	int index = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (index == -1) return;
	if (!items_[index].IsLinkFrom()) return;

	CEdit* pEdit = GetListCtrl().GetEditControl();
	if (pEdit == NULL) {
		CString s = '<' + items_[index].comment + _T(">\n削除しますか?");
		if (MessageBox(s, _T("リンクの削除"), MB_YESNO) != IDYES) return;
		GetDocument()->DeleteSpecifidLink(items_[index]);
	}
	else {
		pEdit->SendMessage(WM_KEYDOWN, VK_DELETE, VK_DELETE);
	}
}

void LinkView::OnUpdateDelete(CCmdUI* pCmdUI)
{
	int index = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	pCmdUI->Enable(index != -1 && items_[index].IsLinkFrom());
}

void LinkView::OnSetLinkInfo()
{
	setLinkInfo();
}

void LinkView::setLinkInfo()
{
	int index = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	int type = items_[index].linkType;
	LinkProps i = items_[index];
	if (type == LinkProps::linkSL || type == LinkProps::linkDL) {
		LinkPropertiesDlg dlg;
		dlg.strComment = i.comment;
		dlg.strFrom = GetDocument()->GetSelectedNodeLabel();
		dlg.strTo = i.sTo;
		dlg.styleArrow = i._arrowStyle;

		dlg.lineWidth = i.linkWidth_;
		dlg.colorLine = i.linkColor_;
		dlg.styleLine = i.styleLine_;
		dlg.lf = i.lf_;
		::lstrcpy(dlg.lf.lfFaceName, i.lf_.lfFaceName);

		GetDocument()->GetSelectedLinkFont(dlg.lf);

		if (dlg.DoModal() != IDOK) return;

		i.comment = dlg.strComment;
		i._arrowStyle = dlg.styleArrow;
		i.linkWidth_ = dlg.lineWidth;
		i.linkColor_ = dlg.colorLine;
		i.styleLine_ = dlg.styleLine;
		i.lf_ = dlg.lf;
		::lstrcpy(i.lf_.lfFaceName, dlg.lf.lfFaceName);
		GetDocument()->SetSpecifiedLinkProps(items_[index], i);
	}
	else if (type == LinkProps::FileName || type == LinkProps::WebURL ||
		type == LinkProps::linkFolder || type == LinkProps::iedFile) {
		LinkForPathDlg dlg;
		dlg.strComment = i.comment;
		dlg.strOrg = GetDocument()->GetSelectedNodeLabel();
		dlg.strPath = i.path;
		if (dlg.DoModal() != IDOK) return;
		i.comment = dlg.strComment;
		i.path = dlg.strPath;
		if (i.comment == "" && i.path != "") {
			WCHAR drive[_MAX_DRIVE];
			WCHAR dir[_MAX_DIR];
			WCHAR fileName[_MAX_FNAME];
			WCHAR ext[_MAX_EXT];
			ZeroMemory(drive, _MAX_DRIVE);
			ZeroMemory(dir, _MAX_DIR);
			ZeroMemory(fileName, _MAX_FNAME);
			ZeroMemory(ext, _MAX_EXT);

			_wsplitpath_s((const wchar_t *)i.path, drive, _MAX_DRIVE, dir, _MAX_DIR, fileName, _MAX_FNAME, ext, _MAX_EXT);
			i.comment.Format(_T("%s%s"), fileName, ext);
		}
		GetDocument()->SetSpecifiedLinkProps(items_[index], i);
	}
}

void LinkView::OnUpdateSetLinkInfo(CCmdUI* pCmdUI)
{
	int index = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	pCmdUI->Enable(index != -1 && items_[index].IsLinkFrom());
}

void LinkView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	jumpTo();
	CListView::OnLButtonDblClk(nFlags, point);
}

void LinkView::jumpTo()
{
	int index = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	if (index == -1) {
		return;
	}
	int type = items_[index].linkType;
	if (type == LinkProps::FileName || type == LinkProps::linkFolder || type == LinkProps::iedFile) {
		CString path = items_[index].path;
		WCHAR drive[_MAX_DRIVE];
		WCHAR dir[_MAX_DIR];
		WCHAR fileName[_MAX_FNAME];
		WCHAR ext[_MAX_EXT];
		ZeroMemory(drive, _MAX_DRIVE);
		ZeroMemory(dir, _MAX_DIR);
		ZeroMemory(fileName, _MAX_FNAME);
		ZeroMemory(ext, _MAX_EXT);
		_wsplitpath_s((const wchar_t *)path, drive, _MAX_DRIVE, dir, _MAX_DIR, fileName, _MAX_FNAME, ext, _MAX_EXT);
		CString workdir; workdir.Format(_T("%s%s"), drive, dir);

		CString sdrive(drive);
		if (sdrive == _T("")) {
			// ドライブレターが無い場合、編集中のieditファイルとの
			// 相対位置と見なして、ファイルオープンを試みる
			CString ieditFilePath = GetDocument()->GetPathName();
			WCHAR drive2[_MAX_DRIVE];
			WCHAR dir2[_MAX_DIR];
			WCHAR fileName2[_MAX_FNAME];
			WCHAR ext2[_MAX_EXT];
			ZeroMemory(drive2, _MAX_DRIVE);
			ZeroMemory(dir2, _MAX_DIR);
			ZeroMemory(fileName2, _MAX_FNAME);
			ZeroMemory(ext2, _MAX_EXT);
			_wsplitpath_s((const wchar_t *)ieditFilePath, drive2, _MAX_DRIVE, dir2, _MAX_DIR, fileName2, _MAX_FNAME, ext2, _MAX_EXT);
			CString combPath; combPath.Format(_T("%s%s%s%s%s"), drive2, dir2, dir, fileName, ext);
			workdir.Format(_T("%s%s"), drive2, dir2, dir2);
			path = combPath;
		}
		CString extention(ext);
		if (extention == _T(".ied") || extention == _T(".iedx")) {
			AfxGetApp()->OpenDocumentFile(path);
		}
		else {
			ShellExecute(m_hWnd, _T("open"), path, NULL, workdir, SW_SHOW);
		}
	}
	else if (type == LinkProps::WebURL) {
		ShellExecute(m_hWnd, _T("open"), items_[index].path, NULL, NULL, SW_SHOW);
	}
	else {
		kstack.push(GetDocument()->GetSelectedNodeKey());
		GetDocument()->SelectionChanged(items_[index].keyTo, true, GetDocument()->ShowSubBranch());
	}
}

void LinkView::jumpBack()
{
	if (kstack.size() == 0) return;
	DWORD prekey = kstack.top();
	kstack.pop();
	GetDocument()->SelectionChanged(prekey, true, GetDocument()->ShowSubBranch());
}

void LinkView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CListCtrl& List = GetListCtrl();
	int index = List.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);

	CMenu menu;
	menu.LoadMenu(IDR_CONTEXT);

	CMenu* pPopup = menu.GetSubMenu(5);
	if (point.x < 0 || point.y < 0) {
		CRect rc; GetWindowRect(&rc);
		point = rc.TopLeft();
		if (index != -1) {
			List.GetItemRect(index, &rc, LVIR_LABEL);
			point.x += rc.Width() / 2;
			point.y += rc.top + rc.Height() / 2;
		}
	}
	ASSERT(pPopup != NULL);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, AfxGetMainWnd());
}

void LinkView::OnReturn(NMHDR* pNMHDR, LRESULT* pResult)
{
	jumpTo();
	*pResult = 0;
}

void LinkView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_BACK) {
		jumpBack();
	}
	if (nChar == VK_ESCAPE) {
		GetDocument()->SelectionChanged(m_preKey, true, GetDocument()->ShowSubBranch());
	}
	CListView::OnKeyUp(nChar, nRepCnt, nFlags);
}

void LinkView::notifySelLink()
{
	for (unsigned int i = 0; i < items_.size(); i++) {
		items_[i].selected = false;
	}
	int index = GetListCtrl().GetNextItem(-1, LVNI_SELECTED);
	if (index != -1) {
		items_[index].selected = true;
		GetDocument()->NotifyLinkSelected(items_, index);
	}
}

void LinkView::OnEditCut()
{
	CEdit* pEdit = GetListCtrl().GetEditControl();
	if (pEdit == NULL) {
		notifySelLink();
		GetDocument()->setCpLinkOrg();
		GetDocument()->DeleteSelectedLink2();
	}
	else {
		pEdit->Cut();
	}
}

void LinkView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	int index = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	pCmdUI->Enable(index != -1 && items_[index].IsLinkFrom());
}

void LinkView::OnEditCopy()
{
	CEdit* pEdit = GetListCtrl().GetEditControl();
	if (pEdit == NULL) {
		notifySelLink();
		GetDocument()->setCpLinkOrg();
	}
	else {
		pEdit->Copy();
	}
}

void LinkView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	int index = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	pCmdUI->Enable(index != -1 && items_[index].IsLinkFrom());
}

void LinkView::OnEditPaste()
{
	CEdit* pEdit = GetListCtrl().GetEditControl();
	if (pEdit == NULL) {
		GetDocument()->addSetLinkOrg();
	}
	else {
		pEdit->Paste();
	}
}

void LinkView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	CEdit* pEdit = GetListCtrl().GetEditControl();
	pCmdUI->Enable(GetDocument()->canCopyLink() || pEdit != NULL);
}

void LinkView::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int index = GetListCtrl().GetNextItem(-1, LVNI_SELECTED);
	if (index != curSel && index != -1) {
		curSel = index;
		notifySelLink();
	}

	*pResult = 0;
}

void LinkView::setViewFont()
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
	if (sc.isMeiryoUiEnabled()) {
		defaultFont = _T("Meiryo UI");
	}
	::lstrcpy(lf.lfFaceName, AfxGetApp()->GetProfileString(REGS_FRAME, _T("Font2 Name"), defaultFont));
	lf.lfHeight = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font2 Height"), 0xfffffff3);
	lf.lfWidth = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font2 Width"), 0);
	lf.lfItalic = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font2 Italic"), FALSE);
	lf.lfUnderline = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font2 UnderLine"), FALSE);
	lf.lfStrikeOut = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font2 StrikeOut"), FALSE);
	lf.lfCharSet = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font2 CharSet"), SHIFTJIS_CHARSET);
	lf.lfWeight = AfxGetApp()->GetProfileInt(REGS_FRAME, _T("Font2 Weight"), FW_NORMAL);
	m_font.CreateFontIndirect(&lf);
	SetFont(&m_font, TRUE);
}

void LinkView::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iSubItem == 0) {
		sortBy = 0;
	}
	else {
		sortBy = 1;
	}
	sort(items_.begin(), items_.end());
	GetListCtrl().DeleteAllItems();
	listConstruct();
	setSelection(0);
	*pResult = 0;
}

void LinkView::OnEditLabel()
{
	int index = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	GetListCtrl().EditLabel(index);
}

void LinkView::OnUpdateEditLabel(CCmdUI* pCmdUI)
{
	int index = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	pCmdUI->Enable(index != -1 && items_[index].IsLinkFrom());
}

void LinkView::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	CString editString = pDispInfo->item.pszText;
	if (editString == _T("")) return;

	int index = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	int type = items_[index].linkType;
	LinkProps i = items_[index];

	if (!i.IsLinkFrom()) {
		MessageBox(_T("このリンクを編集するにはリンク元のノードを選択して下さい"));
		return;
	}
	i.comment = editString;
	GetDocument()->SetSpecifiedLinkProps(items_[index], i);

	Invalidate();
	*pResult = 0;
}

void LinkView::OnEditReplace()
{
}

void LinkView::OnEditUndo()
{
}

void LinkView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
}

DROPEFFECT LinkView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	return CListView::OnDragEnter(pDataObject, dwKeyState, point);
}

void LinkView::OnDragLeave()
{
	CListView::OnDragLeave();
}

DROPEFFECT LinkView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	// check for force link
	DROPEFFECT de = DROPEFFECT_NONE;
	if ((dwKeyState & (MK_CONTROL | MK_SHIFT)) == (MK_CONTROL | MK_SHIFT)) {
		de = DROPEFFECT_LINK;
	}
	// コピー
	else if ((dwKeyState & MK_CONTROL) == MK_CONTROL) {
		de = DROPEFFECT_COPY;
	}
	// 移動
	else if ((dwKeyState & MK_SHIFT) == MK_SHIFT) {
		de = DROPEFFECT_MOVE;
	}
	// デフォルトはコピー
	else {
		// pDataObjectをQueryして、COPY/MOVEを切り分けるのも吉
		de = DROPEFFECT_COPY;
	}
	return de;
}

BOOL LinkView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	if (pDataObject->IsDataAvailable(CF_TEXT)) {
		// IEのアドレスバーからのドラッグ＆ドロップ

		HGLOBAL hmem = pDataObject->GetGlobalData(CF_TEXT);
		CMemFile sf((BYTE*) ::GlobalLock(hmem), ::GlobalSize(hmem));
		CString buffer;

		LPSTR str = (LPSTR)buffer.GetBufferSetLength(::GlobalSize(hmem));
		sf.Read(str, ::GlobalSize(hmem));
		::GlobalUnlock(hmem);
		if (isURLStr(str)) {
			GetDocument()->AddUrlLink(str, _T("URLリンク"));
		}
		return TRUE;
	}

	HGLOBAL hData = pDataObject->GetGlobalData(CF_HDROP);
	const HDROP hdrop = (const HDROP)::GlobalLock(hData);
	int n = ::DragQueryFile(hdrop, 0xffffffff, NULL, 0);
	WCHAR path[_MAX_PATH];
	if (n > 0) {
		GetDocument()->DisableUndo();
	}

	WCHAR drive[_MAX_DRIVE];
	WCHAR dir[_MAX_DIR];
	WCHAR fname[_MAX_FNAME];
	WCHAR ext[_MAX_EXT];

	for (int i = 0; i < n; i++) {
		::DragQueryFile(hdrop, i, path, sizeof(path));
		memset(drive, '\0', _MAX_DRIVE);
		memset(dir, '\0', _MAX_DIR);
		memset(fname, '\0', _MAX_FNAME);
		memset(ext, '\0', _MAX_EXT);
		_wsplitpath_s((const wchar_t *)path, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
		CString extention(ext);
		extention.MakeLower();
		if (extention == _T(".url")) {
			CString url = getLocationFromURLFile(path);
			GetDocument()->AddUrlLink(url, CString(fname));
			continue;
		}
		else if (extention == _T(".ied") || extention == _T(".iedx")) {
			FileDropActionDlg dlg;
			dlg.m_nDropProc = 0;
			if (dlg.DoModal() != IDOK) return TRUE;
			if (dlg.m_nDropProc == 1) {
				return CListView::OnDrop(pDataObject, dropEffect, point);
			}
			else {
				;
			}
		}

		CString fileName;
		fileName.Format(_T("%s%s"), fname, ext);
		GetDocument()->AddUrlLink(path, fileName);
	}
	::GlobalUnlock(hData);

	return TRUE;
}

CString LinkView::getLocationFromURLFile(LPCTSTR path)
{
	CStdioFile f;
	CFileStatus status;
	CFileException e;

	if (!f.Open(path, CFile::typeText | CFile::modeRead, &e)) {
		return _T("");
	}
	CString line;
	CString url;
	while (f.ReadString(line)) {
		if (line.Find(_T("BASEURL=")) != -1) {
			url = line.Right(line.GetLength() - 8);
			break;
		}
	}
	f.Close();
	if (url == _T("")) {
		if (!f.Open(path, CFile::typeText | CFile::modeRead, &e)) {
			return _T("");
		}
		while (f.ReadString(line)) {
			if (line.Find(_T("URL=")) != -1) {
				url = line.Right(line.GetLength() - 4);
				break;
			}
		}
		f.Close();
	}
	return url;
}

bool LinkView::isURLStr(const CString &str) const
{
	if (str.Find(_T("http://")) != 0 && str.Find(_T("https://")) != 0 && str.Find(_T("ftp://")) != 0) {
		return false;
	}

	if (str.Find(_T("\r")) != -1 || str.Find(_T("\n")) != -1) return false;

	return true;
}

void LinkView::doColorSetting()
{
	CiEditApp* app = (CiEditApp*)AfxGetApp();
	COLORREF colorBG = app->GetProfileInt(REGS_FRAME, _T("Link bgColor"), app->m_colorLinkViewBg);
	COLORREF colorFor = app->GetProfileInt(REGS_FRAME, _T("Link forColor"), app->m_colorLinkViewFg);
	ListView_SetBkColor(m_hWnd, colorBG);
	ListView_SetTextBkColor(m_hWnd, colorBG);
	ListView_SetTextColor(m_hWnd, colorFor);
}

void LinkView::reflesh()
{
	GetListCtrl().DeleteAllItems();
	items_.clear();
	items_.resize(0);
	GetDocument()->CollectLinkProps(items_);
	listConstruct();
}

void LinkView::setSelection(int index)
{
	GetListCtrl().SetItemState(index,
		LVIS_SELECTED | LVIS_FOCUSED, LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE);
	curSel = index;
}

void LinkView::OnLinkMoveUp()
{
	int index = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	GetDocument()->SwapLinkOrder(items_[index - 1].key, items_[index].key);
	reflesh();
	setSelection(index - 1);
}

void LinkView::OnUpdateLinkMoveUp(CCmdUI *pCmdUI)
{
	int index = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	pCmdUI->Enable(index > 0);
}

void LinkView::OnLinkMoveDown()
{
	int index = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	GetDocument()->SwapLinkOrder(items_[index].key, items_[index + 1].key);
	reflesh();
	setSelection(index + 1);
}

void LinkView::OnUpdateLinkMoveDown(CCmdUI *pCmdUI)
{
	unsigned int index = GetListCtrl().GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	pCmdUI->Enable(index != -1 && index < items_.size() - 1);
}
