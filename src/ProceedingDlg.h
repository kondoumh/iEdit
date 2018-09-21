/////////////////////////////////////////////////////////////////////////////
// ProceedingDlg ダイアログ

class ProceedingDlg : public CDialog
{
public:
	ProceedingDlg(CWnd* pParent = NULL);

	enum { IDD = IDD_ONPROC };
	CStatic	m_ProcName;
	CProgressCtrl	m_ProgProc;

protected:

	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
};
