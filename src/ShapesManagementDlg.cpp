﻿// ShapesManagementDlg.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "ShapesManagementDlg.h"
#include "EditShapeCategoryDlg.h"
#include "ShapeRegistrationDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGS_SHAPES _T("Shapes")

/////////////////////////////////////////////////////////////////////////////
// ShapesManagementDlg ダイアログ

ShapesManagementDlg::ShapesManagementDlg(CWnd* pParent /*=NULL*/)
	: CDialog(ShapesManagementDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(ShapesManagementDlg)
	//}}AFX_DATA_INIT
}

void ShapesManagementDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ShapesManagementDlg)
	DDX_Control(pDX, IDC_BTNGET, m_btnGet);
	DDX_Control(pDX, IDC_BTN_DROP, m_btnDrop);
	DDX_Control(pDX, IDC_BTN_RR, m_btnRR);
	DDX_Control(pDX, IDC_BTN_R, m_btnR);
	DDX_Control(pDX, IDC_BTN_LL, m_btnLL);
	DDX_Control(pDX, IDC_BTN_L, m_btnL);
	DDX_Control(pDX, IDC_CATLIST, m_catListBox);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(ShapesManagementDlg, CDialog)
	//{{AFX_MSG_MAP(ShapesManagementDlg)
	ON_BN_CLICKED(IDC_BTN_DROP, OnBtnDrop)
	ON_LBN_SELCHANGE(IDC_CATLIST, OnSelchangeCatlist)
	ON_LBN_DBLCLK(IDC_CATLIST, OnDblclkCatlist)
	ON_COMMAND(ID_UPDATE_DIAGRAM, OnUpdateDiagram)
	ON_COMMAND(ID_NEW_SHAPE, OnNewShape)
	ON_COMMAND(ID_EDIT_CAT_NAME, OnEditCatName)
	ON_COMMAND(ID_DELETE_SHAPE, OnDeleteShape)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_BTN_L, OnBtnL)
	ON_BN_CLICKED(IDC_BTN_R, OnBtnR)
	ON_BN_CLICKED(IDC_BTN_LL, OnBtnLl)
	ON_BN_CLICKED(IDC_BTN_RR, OnBtnRr)
	ON_COMMAND(ID_SAVE_SHAPE_FILE, OnSaveShapeFile)
	ON_COMMAND(ID_OPEN_SHAPE_FILE, OnOpenShapeFile)
	ON_BN_CLICKED(IDC_BTNGET, OnBtnget)
	ON_COMMAND(ID_REGIST_NODESHAPE, OnRegistNodeshape)
	ON_COMMAND(ID_PASTE_FROM_CLPBRD, OnPasteFromClpbrd)
	ON_UPDATE_COMMAND_UI(ID_PASTE_FROM_CLPBRD, OnUpdatePasteFromClpbrd)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BOOL ShapesManagementDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_bounds[0] = CRect(137, 22, 167, 52);
	m_bounds[1] = CRect(169, 22, 199, 52);
	m_bounds[2] = CRect(201, 22, 231, 52);
	m_bounds[3] = CRect(233, 22, 263, 52);

	m_bounds[4] = CRect(137, 54, 167, 84);
	m_bounds[5] = CRect(169, 54, 199, 84);
	m_bounds[6] = CRect(201, 54, 231, 84);
	m_bounds[7] = CRect(233, 54, 263, 84);

	m_bounds[8] = CRect(137, 86, 167, 116);
	m_bounds[9] = CRect(169, 86, 199, 116);
	m_bounds[10] = CRect(201, 86, 231, 116);
	m_bounds[11] = CRect(233, 86, 263, 116);

	m_bounds[12] = CRect(137, 118, 167, 148);
	m_bounds[13] = CRect(169, 118, 199, 148);
	m_bounds[14] = CRect(201, 118, 231, 148);
	m_bounds[15] = CRect(233, 118, 263, 148);

	m_bounds[16] = CRect(137, 150, 167, 180);
	m_bounds[17] = CRect(169, 150, 199, 180);
	m_bounds[18] = CRect(201, 150, 231, 180);
	m_bounds[19] = CRect(233, 150, 263, 180);

	m_indexIncat = 0;
	m_selRect = m_bounds[0];
	m_oldRect = m_bounds[0];

	CiEditApp* pApp = (CiEditApp*)AfxGetApp();

	CString mfname = pApp->GetProfileString(REGS_SHAPES, _T("recent file"), _T(""));
	pApp->loadMetaFiles(mfname);

	for (int i = 0; i < 10; i++) {
		m_catListBox.AddString(pApp->m_mfCategories[i]);
	}
	m_catListBox.SetCurSel(0);

	return TRUE;
}

void ShapesManagementDlg::OnBtnDrop()
{
	updateNetVew();
}

void ShapesManagementDlg::OnUpdateDiagram()
{
	updateNetVew();
}

void ShapesManagementDlg::updateNetVew()
{
	if (m_selRect == CRect(0, 0, 0, 0)) {
		AfxMessageBox(_T("シェイプを選択してください"));
		return;
	}
	int index = m_catListBox.GetCurSel() * 100 + m_indexIncat;
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	if (pApp->m_hMetaFiles[index] == NULL) {
		AfxMessageBox(_T("シェイプが登録されていません"));
		return;
	}
	m_pParent->PostMessage(WM_ADDSHAPE, index/* メタファイルID*/);
	m_pParent->SetFocus();
}

BOOL ShapesManagementDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	m_pParent = pParentWnd;
	return CDialog::Create(IDD, pParentWnd);
}

void ShapesManagementDlg::OnOK()
{
	return;
}

void ShapesManagementDlg::OnCancel()
{
	m_pParent->PostMessage(WM_CLOSESHAPEWINDOW, IDCANCEL);
	CDialog::OnCancel();
}

void ShapesManagementDlg::OnSelchangeCatlist()
{
	InvalidateRect(m_selRect);
	m_indexIncat = 0;
	m_selRect = m_bounds[0];
	CRect rc = m_bounds[0];
	for (int i = 0; i < 20; i++) {
		rc += m_bounds[i];
	}
	InvalidateRect(rc);
}

void ShapesManagementDlg::OnDblclkCatlist()
{
	EditCategoryName();
}

void ShapesManagementDlg::OnEditCatName()
{
	EditCategoryName();
}

void ShapesManagementDlg::EditCategoryName()
{
	EditShapeCategoryDlg dlg;
	CString s; m_catListBox.GetText(m_catListBox.GetCurSel(), s);
	dlg.m_name = s;
	int index = m_catListBox.GetCurSel();
	if (dlg.DoModal() != IDCANCEL && dlg.m_name != _T("")) {
		m_catListBox.InsertString(m_catListBox.GetCurSel(), dlg.m_name);
		m_catListBox.DeleteString(m_catListBox.GetCurSel());
		m_catListBox.SetCurSel(index);

		CiEditApp* pApp = (CiEditApp*)AfxGetApp();
		pApp->m_mfCategories[index] = dlg.m_name;
		pApp->m_bShapeModified = true;
		SpecifyShapefile();
	}
}

void ShapesManagementDlg::OnNewShape()
{
	if (m_selRect == CRect(0, 0, 0, 0)) {
		AfxMessageBox(_T("登録する場所を選択してください"));
		return;
	}
	ShapeRegistrationDlg dlg;
	if (dlg.DoModal() != IDOK) return;
	if (dlg.m_strPath == _T("")) return;
	HENHMETAFILE hm = ::GetEnhMetaFile(dlg.m_strPath);
	if (hm == NULL) {
		AfxMessageBox(_T("メタファイルが読み込めませんでした"));
		return;
	}

	if (MessageBox(_T("シェイプを登録しますか?"), _T("シェイプの登録"), MB_YESNO) != IDYES) return;
	int index = m_catListBox.GetCurSel() * 100 + m_indexIncat;
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	pApp->m_hMetaFiles[index] = hm;
	pApp->m_bShapeModified = true;
	InvalidateRect(m_selRect);
	SpecifyShapefile();
}

void ShapesManagementDlg::OnDeleteShape()
{
	int index = m_catListBox.GetCurSel() * 100 + m_indexIncat;
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	if (pApp->m_hMetaFiles[index] == NULL) {
		AfxMessageBox(_T("シェイプが登録されていません"));
		return;
	}
	if (MessageBox(_T("選択されたシェイプを削除します"), _T("シェイプの削除"), MB_YESNO) != IDYES) return;

	pApp->m_hMetaFiles[index] = NULL;
	pApp->m_bShapeModified = true;
	InvalidateRect(m_selRect);
	SpecifyShapefile();
}

void ShapesManagementDlg::OnPaint()
{
	CPaintDC dc(this);

	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	for (int i = 0; i < 20; i++) {
		if (pApp->m_hMetaFiles[m_catListBox.GetCurSel() * 100 + i]) {
			dc.PlayMetaFile(pApp->m_hMetaFiles[m_catListBox.GetCurSel() * 100 + i], m_bounds[i]);
		}
		else {
			dc.MoveTo(m_bounds[i].TopLeft());
			dc.LineTo(m_bounds[i].right, m_bounds[i].top);
			dc.LineTo(m_bounds[i].BottomRight());
			dc.LineTo(m_bounds[i].left, m_bounds[i].bottom);
			dc.LineTo(m_bounds[i].TopLeft());
		}
	}
	CRectTracker tracker;
	tracker.m_rect = m_selRect;
	tracker.m_nStyle = CRectTracker::resizeInside;
	tracker.Draw(&dc);
}

void ShapesManagementDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	for (int i = 0; i < 20; i++) {
		if (m_bounds[i].PtInRect(point)) {
			m_indexIncat = i;
			m_oldRect = m_selRect;
			m_selRect = m_bounds[i];
			InvalidateRect(m_oldRect);
			InvalidateRect(m_selRect);
			break;
		}
	}
	CDialog::OnLButtonDown(nFlags, point);
}

void ShapesManagementDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	for (int i = 0; i < 20; i++) {
		if (m_bounds[i].PtInRect(point)) {
			m_indexIncat = i;
			updateNetVew();
			break;
		}
	}
	CDialog::OnLButtonDblClk(nFlags, point);
}

void ShapesManagementDlg::OnBtnL()
{
	if (m_indexIncat > 0) {
		m_indexIncat--;
		m_oldRect = m_selRect;
		m_selRect = m_bounds[m_indexIncat];
		InvalidateRect(m_oldRect);
		InvalidateRect(m_selRect);
	}
	else if (m_indexIncat == 0) {
		int index = m_catListBox.GetCurSel();
		if (index > 0) {
			m_catListBox.SetCurSel(index - 1);
			m_indexIncat = 19;
			m_selRect = m_bounds[19];
			Invalidate();
		}
	}
}

void ShapesManagementDlg::OnBtnR()
{
	if (m_indexIncat < 19) {
		m_indexIncat++;
		m_oldRect = m_selRect;
		m_selRect = m_bounds[m_indexIncat];
		InvalidateRect(m_oldRect);
		InvalidateRect(m_selRect);
	}
	else if (m_indexIncat == 19) {
		int index = m_catListBox.GetCurSel();
		if (index < 9) {
			m_catListBox.SetCurSel(index + 1);
			m_indexIncat = 0;
			m_selRect = m_bounds[0];
			Invalidate();
		}
	}
}

void ShapesManagementDlg::OnBtnLl()
{
	int index = m_catListBox.GetCurSel();
	if (index > 0) {
		m_catListBox.SetCurSel(index - 1);
		Invalidate();
	}
}

void ShapesManagementDlg::OnBtnRr()
{
	int index = m_catListBox.GetCurSel();
	if (index < 9) {
		m_catListBox.SetCurSel(index + 1);
		Invalidate();
	}
}

void ShapesManagementDlg::OnSaveShapeFile()
{
	WCHAR szFilters[] = _T("シェイプファイル (*.ies)|*.ies||");
	CFileDialog dlg(FALSE, _T("ies"), _T("shapes.ies"), OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, this);
	if (dlg.DoModal() != IDOK) return;
	CString outfileName = dlg.GetPathName();

	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	pApp->saveMetaFiles(outfileName);
	pApp->m_bShapeModified = false;
}

void ShapesManagementDlg::OnOpenShapeFile()
{
	CString infile;
	WCHAR szFilters[] = _T("シェイプファイル (*.ies)|*.ies||");
	CFileDialog dlg(TRUE, _T("ies"), infile, OFN_HIDEREADONLY, szFilters, this);
	if (dlg.DoModal() != IDOK) return;
	CString infileName = dlg.GetPathName();
	CFile f;
	if (!f.Open(infileName, CFile::modeRead)) {
		AfxMessageBox(_T("指定されたシェイプファイル存在しません"));
		return;
	}
	f.Close();

	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	pApp->loadMetaFiles(infileName);

	for (int i = 9; i >= 0; i--) {
		m_catListBox.DeleteString(i);
	}
	for (int i = 0; i < 10; i++) {
		m_catListBox.AddString(pApp->m_mfCategories[i]);
	}
	m_catListBox.SetCurSel(0);
	m_indexIncat = 0;
	m_selRect = m_bounds[0];
	m_oldRect = m_bounds[0];
	Invalidate();
}

void ShapesManagementDlg::OnBtnget()
{
	TakeoutShapeFromView();
}

void ShapesManagementDlg::OnRegistNodeshape()
{
	TakeoutShapeFromView();
}

void ShapesManagementDlg::TakeoutShapeFromView()
{
	if (m_selRect == CRect(0, 0, 0, 0)) {
		AfxMessageBox(_T("シェイプを選択してください"));
		return;
	}
	m_pParent->PostMessage(WM_GETSHAPE, 0);
}

void ShapesManagementDlg::regNodeShape(HENHMETAFILE hMF)
{
	int index = m_catListBox.GetCurSel() * 100 + m_indexIncat;
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	if (pApp->m_hMetaFiles[index] != NULL) {
		if (MessageBox(_T("選択されたシェイプを差し替えます"), _T("シェイプの差し替え"), MB_YESNO) != IDYES) return;
	}

	pApp->m_hMetaFiles[index] = hMF;
	pApp->m_bShapeModified = true;

	InvalidateRect(m_selRect);
	SpecifyShapefile();
}

void ShapesManagementDlg::OnPasteFromClpbrd()
{
	if (::IsClipboardFormatAvailable(CF_ENHMETAFILE) ||
		::IsClipboardFormatAvailable(CF_METAFILEPICT) ||
		::IsClipboardFormatAvailable(CF_DIB)) {

		if (!OpenClipboard()) {
			AfxMessageBox(_T("Cannot open the Clipboard"));
			return;
		}
		int index = m_catListBox.GetCurSel() * 100 + m_indexIncat;
		CiEditApp* pApp = (CiEditApp*)AfxGetApp();
		if (pApp->m_hMetaFiles[index] != NULL) {
			if (MessageBox(_T("選択されたシェイプを差し替えます"), _T("シェイプの差し替え"), MB_YESNO) != IDYES) return;
		}
		HENHMETAFILE hm = (HENHMETAFILE)GetClipboardData(CF_ENHMETAFILE);
		CloseClipboard();
		pApp->m_hMetaFiles[index] = CopyEnhMetaFile(hm, NULL);
		pApp->m_bShapeModified = true;
		InvalidateRect(m_selRect);
		SpecifyShapefile();
	}
}

void ShapesManagementDlg::OnUpdatePasteFromClpbrd(CCmdUI* pCmdUI)
{
}

void ShapesManagementDlg::SpecifyShapefile()
{
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	CString mfname = pApp->GetProfileString(REGS_SHAPES, _T("recent file"), _T(""));
	if (mfname != _T("")) return;

	AfxMessageBox(_T("シェイプを保存するファイル名を指定してください"));
	WCHAR szFilters[] = _T("シェイプファイル (*.ies)|*.ies||");
	CFileDialog dlg(FALSE, _T("ies"), _T("shapes1"), OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY, szFilters, this);
	if (dlg.DoModal() != IDOK) return;
	CString outfileName = dlg.GetPathName();
	pApp->m_bShapeModified = false;

	pApp->saveMetaFiles(outfileName);
}

void ShapesManagementDlg::OnDestroy()
{
	CDialog::OnDestroy();

	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	CString mfname = pApp->GetProfileString(REGS_SHAPES, _T("recent file"), _T(""));
	if (mfname != _T("") && pApp->m_bShapeModified) {
		if (MessageBox(_T("シェイプファイルの変更を保存しますか"), _T("シェイプファイルの保存"), MB_YESNO) == IDYES) {
			pApp->m_bShapeModified = false;
			pApp->saveMetaFiles(mfname);
		}
	}
}
