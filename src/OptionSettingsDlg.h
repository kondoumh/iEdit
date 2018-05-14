﻿#if !defined(AFX_PROPERTYDLG_H__79C43541_7FE6_11D3_9860_006097789FE5__INCLUDED_)
#define AFX_PROPERTYDLG_H__79C43541_7FE6_11D3_9860_006097789FE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyDlg.h : ヘッダー ファイル
//

#include "TabSeet.h"

/////////////////////////////////////////////////////////////////////////////
// OptionSettingsDlg ダイアログ
class OptionPageForFrame;
class OptionPageForNode;
class OptionPageForLink;
class OptionPageForOther;

class OptionSettingsDlg : public CDialog
{
// コンストラクション
public:
	void initPageOther();
	OptionSettingsDlg(CWnd* pParent = NULL);   // 標準のコンストラクタ

// ダイアログ データ
	//{{AFX_DATA(OptionSettingsDlg)
	enum { IDD = IDD_PROPERTIES };
	CTabSeet	m_tabSeet;
	//}}AFX_DATA


// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(OptionSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	void writePageOther();
	void writePageLink();
	void writePageNode();
	void writePageFrame();
	void initPageLink();
	void initPageFrame();
	void initPageNode();

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG(OptionSettingsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	OptionPageForNode* pNode;
	OptionPageForLink* pLink;
	OptionPageForFrame* pFrame;
	OptionPageForOther* pOther;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_PROPERTYDLG_H__79C43541_7FE6_11D3_9860_006097789FE5__INCLUDED_)
