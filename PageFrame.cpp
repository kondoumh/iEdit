// PageFrame.cpp : �C���v�������e�[�V���� �t�@�C��
//

#include "stdafx.h"
#include "iEdit.h"
#include "PageFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define REGS_FRAME _T("Frame Options")

/////////////////////////////////////////////////////////////////////////////
// PageFrame �_�C�A���O


PageFrame::PageFrame(CWnd* pParent /*=NULL*/)
	: CDialog(PageFrame::IDD, pParent)
{
	//{{AFX_DATA_INIT(PageFrame)
	m_bSaveFrame = FALSE;
	m_bSaveBarState = FALSE;
	//}}AFX_DATA_INIT
}


void PageFrame::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(PageFrame)
	DDX_Control(pDX, IDC_CHK_SAVE_BAR_STATE, m_ChkSaveBarState);
	DDX_Control(pDX, IDC_CHK_SAVE_FRAME, m_chkSaveFrame);
	DDX_Check(pDX, IDC_CHK_SAVE_FRAME, m_bSaveFrame);
	DDX_Check(pDX, IDC_CHK_SAVE_BAR_STATE, m_bSaveBarState);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PageFrame, CDialog)
	//{{AFX_MSG_MAP(PageFrame)
	ON_BN_CLICKED(IDC_BTN_LINK, OnBtnLink)
	ON_BN_CLICKED(IDC_BTN_OUTLINE, OnBtnOutline)
	ON_BN_CLICKED(IDC_BTN_TEXT, OnBtnText)
	ON_BN_CLICKED(IDC_CHK_SAVE_BAR_STATE, OnChkSaveBarState)
	ON_BN_CLICKED(IDC_CHK_SAVE_FRAME, OnChkSaveFrame)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN__FCLR_OL, OnBtnFclrOl)
	ON_BN_CLICKED(IDC_BTN__BCLR_OL, OnBtnBclrOl)
	ON_BN_CLICKED(IDC_BTN_FCLR_LN, OnBtnFclrLn)
	ON_BN_CLICKED(IDC_BTN_BCLR_LN, OnBtnBclrLn)
	ON_BN_CLICKED(IDC_BTN_BCLR_NW, OnBtnBclrNw)
	ON_BN_CLICKED(IDC_BTN_FCLR_TX, OnBtnFclrTx)
	ON_BN_CLICKED(IDC_BTN_BCLR_TX, OnBtnBclrTx)
	ON_BN_CLICKED(IDC_BTN__INSERTMARK, OnBtnInsertmark)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PageFrame ���b�Z�[�W �n���h��

void PageFrame::OnCancel() 
{
	// TODO: ���̈ʒu�ɓ��ʂȌ㏈����ǉ����Ă��������B
	return;
	CDialog::OnCancel();
}

void PageFrame::OnBtnLink() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	CFontDialog dlg(&lfLink);
	dlg.m_cf.Flags |= CF_SELECTSCRIPT;
	if (dlg.DoModal() != IDOK) return;
	fntLink.CreateFontIndirect(&lfLink);
	Invalidate();
}

void PageFrame::OnBtnOutline() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	CFontDialog dlg(&lfOutline);
	dlg.m_cf.Flags |= CF_SELECTSCRIPT;
	if (dlg.DoModal() != IDOK) return;
	fntOutline.CreateFontIndirect(&lfOutline);
	Invalidate();
}

void PageFrame::OnBtnText() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	CFontDialog dlg(&lfText);
	dlg.m_cf.Flags |= CF_SELECTSCRIPT;
	if (dlg.DoModal() != IDOK) return;
	fntText.CreateFontIndirect(&lfText);
	Invalidate();
}

void PageFrame::OnChkSaveBarState() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_bSaveBarState = m_ChkSaveBarState.GetCheck();
}

void PageFrame::OnChkSaveFrame() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	m_bSaveFrame = m_chkSaveFrame.GetCheck();
}

void PageFrame::OnPaint() 
{
	CPaintDC dc(this); // �`��p�̃f�o�C�X �R���e�L�X�g
	
	// TODO: ���̈ʒu�Ƀ��b�Z�[�W �n���h���p�̃R�[�h��ǉ����Ă�������
	drawOLPreView(&dc);
	drawLNPreView(&dc);
	drawTextPreView(&dc);
	drawNWPreView(&dc);
}

void PageFrame::drawOLPreView(CDC *pDC)
{
	CRect rc(200, 28, 310, 78);
	
	CBrush brs(m_colorOLBG);
	CBrush* brsOld = pDC->SelectObject(&brs);
	pDC->FillRect(rc, &brs);
	pDC->SelectObject(brsOld);
	
	pDC->MoveTo(rc.TopLeft());
	pDC->LineTo(CPoint(rc.right, rc.top));
	pDC->LineTo(rc.BottomRight());
	pDC->LineTo(CPoint(rc.left, rc.bottom));
	pDC->LineTo(rc.TopLeft());
	
	CFont* pOldFont = pDC->SelectObject(&fntOutline);
	COLORREF preColor = pDC->SetTextColor(m_colorOLFor);
	int oldBkMode = pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(_T("A �� ��"), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->SelectObject(pOldFont);
	pDC->SetTextColor(preColor);
	pDC->SetBkMode(oldBkMode);
}

void PageFrame::drawLNPreView(CDC *pDC)
{
	CRect rc(200, 103, 310, 153);
	
	CBrush brs(m_colorLNBG);
	CBrush* brsOld = pDC->SelectObject(&brs);
	pDC->FillRect(rc, &brs);
	pDC->SelectObject(brsOld);
	
	pDC->MoveTo(rc.TopLeft());
	pDC->LineTo(CPoint(rc.right, rc.top));
	pDC->LineTo(rc.BottomRight());
	pDC->LineTo(CPoint(rc.left, rc.bottom));
	pDC->LineTo(rc.TopLeft());
	
	CFont* pOldFont = pDC->SelectObject(&fntLink);
	COLORREF preColor = pDC->SetTextColor(m_colorLNFor);
	int oldBkMode = pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(_T("A �� ��"), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->SelectObject(pOldFont);
	pDC->SetTextColor(preColor);
	pDC->SetBkMode(oldBkMode);
}

void PageFrame::drawTextPreView(CDC *pDC)
{
	CRect rc(200, 180, 310, 230);
	
	CBrush brs(m_colorEditBG);
	CBrush* brsOld = pDC->SelectObject(&brs);
	pDC->FillRect(rc, &brs);
	pDC->SelectObject(brsOld);
	
	pDC->MoveTo(rc.TopLeft());
	pDC->LineTo(CPoint(rc.right, rc.top));
	pDC->LineTo(rc.BottomRight());
	pDC->LineTo(CPoint(rc.left, rc.bottom));
	pDC->LineTo(rc.TopLeft());
	
	CFont* pOldFont = pDC->SelectObject(&fntText);
	COLORREF preColor = pDC->SetTextColor(m_colorEditFor);
	int oldBkMode = pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(_T("A �� ��"), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	pDC->SelectObject(pOldFont);
	pDC->SetTextColor(preColor);
	pDC->SetBkMode(oldBkMode);
}

void PageFrame::drawNWPreView(CDC *pDC)
{
	CRect rc(200, 255, 310, 280);
	
	CBrush brs(m_colorNWBG);
	CBrush* brsOld = pDC->SelectObject(&brs);
	pDC->FillRect(rc, &brs);
	pDC->SelectObject(brsOld);
	
	pDC->MoveTo(rc.TopLeft());
	pDC->LineTo(CPoint(rc.right, rc.top));
	pDC->LineTo(rc.BottomRight());
	pDC->LineTo(CPoint(rc.left, rc.bottom));
	pDC->LineTo(rc.TopLeft());
}

void PageFrame::OnBtnFclrOl() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	CColorDialog dlg(m_colorOLFor);
	if (dlg.DoModal() != IDOK) return;
	m_colorOLFor = dlg.GetColor();
	Invalidate();
}

void PageFrame::OnBtnBclrOl() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	CColorDialog dlg(m_colorOLBG);
	if (dlg.DoModal() != IDOK) return;
	m_colorOLBG = dlg.GetColor();
	Invalidate();
}

void PageFrame::OnBtnFclrLn() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	CColorDialog dlg(m_colorLNFor);
	if (dlg.DoModal() != IDOK) return;
	m_colorLNFor = dlg.GetColor();
	Invalidate();
}

void PageFrame::OnBtnBclrLn() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	CColorDialog dlg(m_colorLNBG);
	if (dlg.DoModal() != IDOK) return;
	m_colorLNBG = dlg.GetColor();
	Invalidate();
}

void PageFrame::OnBtnFclrTx() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	CColorDialog dlg(m_colorEditFor);
	if (dlg.DoModal() != IDOK) return;
	m_colorEditFor = dlg.GetColor();
	Invalidate();
}

void PageFrame::OnBtnBclrTx() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	CColorDialog dlg(m_colorEditBG);
	if (dlg.DoModal() != IDOK) return;
	m_colorEditBG = dlg.GetColor();
	Invalidate();
}

void PageFrame::OnBtnBclrNw() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	CColorDialog dlg(m_colorNWBG);
	if (dlg.DoModal() != IDOK) return;
	m_colorNWBG = dlg.GetColor();
	Invalidate();
}

void PageFrame::OnBtnInsertmark() 
{
	// TODO: ���̈ʒu�ɃR���g���[���ʒm�n���h���p�̃R�[�h��ǉ����Ă�������
	CColorDialog dlg(m_colorInsrtMrk);
	if (dlg.DoModal() != IDOK) return;
	m_colorInsrtMrk = dlg.GetColor();
}
