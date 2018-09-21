/////////////////////////////////////////////////////////////////////////////
// ShapesManagementDlg ダイアログ

#pragma once

#define WM_CLOSESHAPEWINDOW WM_USER + 5
#define WM_ADDSHAPE WM_USER + 6
#define WM_GETSHAPE WM_USER + 10

class ShapesManagementDlg : public CDialog
{
	// コンストラクション
public:
	ShapesManagementDlg(CWnd* pParent = NULL);
	void regNodeShape(HENHMETAFILE hMF);

	enum { IDD = IDD_SHAPES };
	CButton	m_btnGet;
	CButton	m_btnDrop;
	CButton	m_btnRR;
	CButton	m_btnR;
	CButton	m_btnLL;
	CButton	m_btnL;
	CListBox	m_catListBox;

public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	afx_msg void OnBtnDrop();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangeCatlist();
	afx_msg void OnDblclkCatlist();
	afx_msg void OnUpdateDiagram();
	afx_msg void OnNewShape();
	afx_msg void OnEditCatName();
	afx_msg void OnDeleteShape();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBtnL();
	afx_msg void OnBtnR();
	afx_msg void OnBtnLl();
	afx_msg void OnBtnRr();
	afx_msg void OnSaveShapeFile();
	afx_msg void OnOpenShapeFile();
	afx_msg void OnBtnget();
	afx_msg void OnRegistNodeshape();
	afx_msg void OnPasteFromClpbrd();
	afx_msg void OnUpdatePasteFromClpbrd(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
private:
	CRect m_oldRect;
	void updateNetVew();
	int m_indexIncat;
	CRect m_bounds[20];
	CRect m_selRect;
	CWnd* m_pParent;

	void SpecifyShapefile();
	void TakeoutShapeFromView();
	void EditCategoryName();
};
