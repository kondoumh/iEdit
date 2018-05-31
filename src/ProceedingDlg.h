// OnProcDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// ProceedingDlg ダイアログ

class ProceedingDlg : public CDialog
{
public:
	ProceedingDlg(CWnd* pParent = NULL);

	//{{AFX_DATA(ProceedingDlg)
	enum { IDD = IDD_ONPROC };
	CStatic	m_ProcName;
	CProgressCtrl	m_ProgProc;
	//}}AFX_DATA

protected:

	//{{AFX_VIRTUAL(ProceedingDlg)
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(ProceedingDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
