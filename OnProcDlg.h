// OnProcDlg.h : ヘッダー ファイル
//

/////////////////////////////////////////////////////////////////////////////
// COnProcDlg ダイアログ

class COnProcDlg : public CDialog
{
// コンストラクション
public:
	COnProcDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(COnProcDlg)
	enum { IDD = IDD_ONPROC };
	CStatic	m_ProcName;
	CProgressCtrl	m_ProgProc;
	//}}AFX_DATA

// オーバーライド
	// ClassWizard は仮想関数を生成しオーバーライドします。
	//{{AFX_VIRTUAL(COnProcDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(COnProcDlg)
		// メモ: ClassWizard はこの位置にメンバ関数を追加します。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
