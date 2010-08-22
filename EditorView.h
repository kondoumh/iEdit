#if !defined(AFX_EDITORVIEW_H__96DFF9CB_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
#define AFX_EDITORVIEW_H__96DFF9CB_1881_11D3_808A_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditorView.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// EditorView �r���[
class iEditDoc;

class EditorView : public CEditView
{
protected:
	EditorView();           // ���I�����Ɏg�p�����v���e�N�g �R���X�g���N�^
	DECLARE_DYNCREATE(EditorView)

// �A�g���r���[�g
public:

// �I�y���[�V����
public:
	virtual void OnReplaceAll( LPCTSTR lpszFind, LPCTSTR lpszReplace, BOOL bCase);
	virtual void  OnReplaceSel( LPCTSTR lpszFind, BOOL bNext, BOOL bCase, LPCTSTR lpszReplace);
	virtual void OnFindNext( LPCTSTR lpszFind, BOOL bNext, BOOL bCase );
	void setViewFont();
	void setTabStop();
	iEditDoc* GetDocument();

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B

	//{{AFX_VIRTUAL(EditorView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // ���̃r���[��`�悷�邽�߂ɃI�[�o�[���C�h���܂����B
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	virtual ~EditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
	void initSizeChar();
	void DrawCaretLine(BOOL bInPaint = FALSE);
	int GetCaretLine() const;
	void GetLineRect(int nLine, LPRECT lpRect) const;
	//{{AFX_MSG(EditorView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDelete();
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
	afx_msg void OnEditLabel();
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditClear();
	afx_msg void OnUpdateEditClear(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnEditFind();
	afx_msg void OnUpdateEditFind(CCmdUI* pCmdUI);
	afx_msg void OnEditReplace();
	afx_msg void OnUpdateEditReplace(CCmdUI* pCmdUI);
	afx_msg void OnPaint();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bDrawUnderLine;
	BOOL	m_bCanPaint;
	int		m_nCaretLine;
	CSize m_sizeChar;
	CBrush m_hBrsBack;
	COLORREF m_textColor;
	COLORREF m_bkColor;
	CFindReplaceDialog* m_pFindReplacedlg;
	CFont m_font;
	DWORD m_preKey;
public:
	afx_msg void OnEnVscroll();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_EDITORVIEW_H__96DFF9CB_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
