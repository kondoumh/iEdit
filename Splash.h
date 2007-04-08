// CG: ���̃t�@�C���̓X�v���b�V�� �X�N���[�� �R���|�[�l���g�ɂ���Ēǉ�����܂����B

#ifndef _SPLASH_SCRN_
#define _SPLASH_SCRN_

// Splash.h : �w�b�_�[ �t�@�C��
//

/////////////////////////////////////////////////////////////////////////////
//   �X�v���b�V�� �X�N���[�� �N���X

class CSplashWnd : public CWnd
{
// �\�z
protected:
	CSplashWnd();

// �A�g���r���[�g:
public:
	CBitmap m_bitmap;

// �I�y���[�V����
public:
	static void EnableSplashScreen(BOOL bEnable = TRUE);
	static void ShowSplashScreen(CWnd* pParentWnd = NULL);
	static BOOL PreTranslateAppMessage(MSG* pMsg);

// �I�[�o�[���C�h
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashWnd)
	//}}AFX_VIRTUAL

// �C���v�������e�[�V����
public:
	~CSplashWnd();
	virtual void PostNcDestroy();

protected:
	BOOL Create(CWnd* pParentWnd = NULL);
	void HideSplashScreen();
	static BOOL c_bShowSplashWnd;
	static CSplashWnd* c_pSplashWnd;

// �������ꂽ���b�Z�[�W �}�b�v�֐�
protected:
	//{{AFX_MSG(CSplashWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


#endif
