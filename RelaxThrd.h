#if !defined(AFX_RELAXTHRD_H__DDFD7BE1_98CB_11D3_B42D_00A0C9B72FDD__INCLUDED_)
#define AFX_RELAXTHRD_H__DDFD7BE1_98CB_11D3_B42D_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RelaxThrd.h : �w�b�_�[ �t�@�C��
//

struct iBound {
	DWORD key;
	CRect oldBound;
	CRect newBound;
	CString label;
	BOOL fixed;
	double dy;
	double dx;
};

struct iEdge {
	DWORD from;
	DWORD to;
	double len;
};

struct iBound_less : binary_function<iBound, iBound, bool> {
	bool operator()(const iBound& b1, const iBound& b2) const
	{
		return b1.key < b2.key;
	}
};

typedef set<iBound, iBound_less> Bounds;
typedef vector<iEdge> Edges;

/////////////////////////////////////////////////////////////////////////////
// CRelaxThrd �X���b�h

class CRelaxThrd : public CWinThread
{
	DECLARE_DYNAMIC(CRelaxThrd)
protected:

// �A�g���r���[�g
public:
	static CRITICAL_SECTION m_csGDILock;
	static HANDLE m_hAnotherDead;
	HANDLE m_hEventKill;
	HANDLE m_hEventDead;
	Bounds bounds;
	Edges edges;
// �I�y���[�V����
public:
	virtual void Delete();
	void KillThread();
	CRelaxThrd(CWnd* pWnd, HDC hDC, CSize szData, bool bWhole, const CPoint& ptScroll);
	CRelaxThrd();
	virtual ~CRelaxThrd();

// �I�[�o�[���C�h
	// ClassWizard �͉��z�֐��̃I�[�o�[���C�h�𐶐����܂��B
	//{{AFX_VIRTUAL(CRelaxThrd)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
protected:
	void SingleStep();

	// �������ꂽ���b�Z�[�W �}�b�v�֐�
	//{{AFX_MSG(CRelaxThrd)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ����o�֐���ǉ��܂��͍폜���܂��B
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CPoint m_ptScroll;
	bool m_bWhole;
	double m_rateH;
	double m_rateV;
	HBRUSH m_hBrush;
	HBRUSH m_hBrush2;
	CBrush m_brush;
	CBrush m_brush2;
	CRect m_rectBorder;
	HDC m_hDC;
	CDC m_dc;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_RELAXTHRD_H__DDFD7BE1_98CB_11D3_B42D_00A0C9B72FDD__INCLUDED_)
