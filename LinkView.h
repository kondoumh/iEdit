#if !defined(AFX_LINKVIEW_H__96DFF9CC_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
#define AFX_LINKVIEW_H__96DFF9CC_1881_11D3_808A_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LinkView.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
// LinkView �r���[
class iEditDoc;

typedef stack<DWORD> keyStack;

class LinkView : public CListView
{
protected:
	LinkView();           // ���I�����Ɏg�p�����v���e�N�g �R���X�g���N�^
	DECLARE_DYNCREATE(LinkView)

// �A�g���r���[�g
public:

// �I�y���[�V����
public:
	int m_preWidth;
	void setViewFont();
	void jumpBack();
	iEditDoc* GetDocument();

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B

	//{{AFX_VIRTUAL(LinkView)
	public:
	virtual void OnInitialUpdate();
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	protected:
	virtual void OnDraw(CDC* pDC);      // ���̃r���[��`�悷�邽�߂ɃI�[�o�[���C�h���܂����B
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	virtual ~LinkView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
	void doColorSetting();
	bool isURLStr(const CString& str) const;
	CString getLocationFromURLFile(LPCTSTR path);
	void listConstruct();
	void setLinkInfo();
	void notifySelLink();
	void jumpTo();
	//{{AFX_MSG(LinkView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnJumpTo();
	afx_msg void OnUpdateJumpTo(CCmdUI* pCmdUI);
	afx_msg void OnJumpBack();
	afx_msg void OnUpdateJumpBack(CCmdUI* pCmdUI);
	afx_msg void OnDelete();
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
	afx_msg void OnSetLinkInfo();
	afx_msg void OnUpdateSetLinkInfo(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnReturn(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditLabel();
	afx_msg void OnEditReplace();
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditLabel(CCmdUI* pCmdUI);
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	COleDropTarget m_oleDropTarget;
	CFont m_font;
	int curSel;
	DWORD m_preKey;
	keyStack kstack;
	lsItems items_;
	CImageList m_imageList;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_LINKVIEW_H__96DFF9CC_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
