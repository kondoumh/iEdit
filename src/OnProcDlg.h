// OnProcDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// ProceedingDlg ダイアログ

class ProceedingDlg : public CDialog
{
// コンストラクション
public:
	ProceedingDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(ProceedingDlg)
	enum { IDD = IDD_ONPROC };
	CStatic	m_ProcName;
	CProgressCtrl	m_ProgProc;
	//}}AFX_DATA

// オーバーライド
	// ClassWizard は仮想関数を生成しオーバーライドします。
	//{{AFX_VIRTUAL(ProceedingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(ProceedingDlg)
		// メモ: ClassWizard はこの位置にメンバ関数を追加します。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
