// TabSeet.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "TabSeet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabSeet

CTabSeet::CTabSeet()
{
}

CTabSeet::~CTabSeet()
{
}


BEGIN_MESSAGE_MAP(CTabSeet, CTabCtrl)
	//{{AFX_MSG_MAP(CTabSeet)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabSeet メッセージ ハンドラ

void CTabSeet::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: この位置にコントロール通知ハンドラ用のコードを追加してください
	setPage();
	*pResult = 0;
}

bool CTabSeet::setPage(int page)
{
  CDialog* d;
  if ( page < 0 || page > (int)page_.size() )
    return false;
  if ( curr_ == page ) return true;
  if ( curr_ >= 0 ) {
    d = page_[curr_].first;
    if ( d->GetSafeHwnd() ) {
       d->EnableWindow(FALSE);
       d->ShowWindow(SW_HIDE);
    }
  }
  d = page_[page].first;
  if ( !d->GetSafeHwnd() ) {
    d->Create(page_[page].second, this);
    d->SetWindowPos(&CWnd::wndTop,
                    rect_.left,    rect_.top, 
                    rect_.Width(), rect_.Height(), 
                    SWP_SHOWWINDOW); 
  } else {
    d->EnableWindow(TRUE);
    d->ShowWindow(SW_SHOW);
  }
  curr_ = page;
  return true;
}

void CTabSeet::beginService(int start)
{
  curr_ = -1;
  GetClientRect(rect_);
  AdjustRect(FALSE,rect_);
  setPage(start);
}

void CTabSeet::endService(bool deletePage)
{
  while ( !page_.empty() ) {
    CDialog* d = page_.back().first;
    if ( d->GetSafeHwnd() )
      d->DestroyWindow();
    if ( deletePage )
      delete d;
    page_.pop_back();
  }
}
