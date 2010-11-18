// RelaxThrd.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "RelaxThrd.h"
#include <complex>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// critical section to protect while drawing to the DC
CRITICAL_SECTION CRelaxThrd::m_csGDILock;

// m_hAnotherDead is used to signal that one or more threads have ended
//  (it is an auto-reset event; and starts out not signaled)
HANDLE CRelaxThrd::m_hAnotherDead = CreateEvent(NULL, FALSE, FALSE, NULL);

/////////////////////////////////////////////////////////////////////////////
// CRelaxThrd

IMPLEMENT_DYNAMIC(CRelaxThrd, CWinThread)

CRelaxThrd::CRelaxThrd(CWnd* pWnd, HDC hDC, CSize szData, bool bWhole, const CPoint& ptScroll)
{
	m_pMainWnd = pWnd;
	m_pMainWnd->GetClientRect(&m_rectBorder);
	m_hDC = hDC;
	CBrush brush(RGB(100, 100, 100));
	m_hBrush = (HBRUSH)brush.Detach();
	CBrush brush2(RGB(255, 255, 255));
	m_hBrush2 = (HBRUSH)brush2.Detach();
	m_rateH = (double)m_rectBorder.Width()/(double)szData.cx;
	m_rateV = (double)m_rectBorder.Height()/(double)szData.cy;
	m_bWhole = bWhole;
	m_hEventKill = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hEventDead = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_ptScroll = ptScroll;
}

CRelaxThrd::~CRelaxThrd()
{
	CloseHandle(m_hEventKill);
	CloseHandle(m_hEventDead);
}

BOOL CRelaxThrd::InitInstance()
{
	// TODO: この位置にスレッド単位の初期化コードを追加してください。
	// thread setup
	m_brush.Attach(m_hBrush);
	m_brush2.Attach(m_hBrush2);
	m_dc.Attach(m_hDC);

	// loop but check for kill notification
	while (WaitForSingleObject(m_hEventKill, 0) == WAIT_TIMEOUT)
		SingleStep();

	// thread cleanup
	m_dc.Detach();

	// avoid entering standard message loop by returning FALSE
	return FALSE;
}

int CRelaxThrd::ExitInstance()
{
	// TODO: この位置にスレッド単位の初期化コードを追加してください。
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CRelaxThrd, CWinThread)
	//{{AFX_MSG_MAP(CRelaxThrd)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加します。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRelaxThrd メッセージ ハンドラ

CRelaxThrd::CRelaxThrd()
{

}

void CRelaxThrd::SingleStep()
{
	// Since all threads share the same HDC it is necessary
	// to block all other threads while we render in the HDC
	for (unsigned int i = 0; i < edges.size(); i++) {
		iBound find; find.key = edges[i].from;
		Bounds::iterator itFrom = bounds.find(find);
		find.key = edges[i].to;
		Bounds::iterator itTo = bounds.find(find);

		double gxto = (*itTo).oldBound.CenterPoint().x;
		double gyto = (*itTo).oldBound.CenterPoint().y;
		double gxfrom = (*itFrom).oldBound.CenterPoint().x;
		double gyfrom = (*itFrom).oldBound.CenterPoint().y;
		
		double vx = gxto - gxfrom;
		double vy = gyto - gyfrom;
		double len = sqrt(vx*vx + vy*vy);
		double f = (edges[i].len - len)/(len*3);
		double dx = f*vx;
		double dy = f*vy;
		const_cast<iBound&>(*itTo).dx += dx;
		const_cast<iBound&>(*itTo).dy += dy;
		const_cast<iBound&>(*itFrom).dx -= dx;
		const_cast<iBound&>(*itFrom).dy -= dy;
	}
	
	srand((unsigned)time(NULL));
	Bounds::iterator it1 = bounds.begin();
	for ( ; it1 != bounds.end(); it1++) {
		double dx = 0; double dy = 0;
		Bounds::iterator it2 = bounds.begin();
		for (; it2 != bounds.end(); it2++) {
			if (it1 == it2) { continue; }
			
			double gx1 = (*it1).oldBound.CenterPoint().x;
			double gy1 = (*it1).oldBound.CenterPoint().y;
			double gx2 = (*it2).oldBound.CenterPoint().x;
			double gy2 = (*it2).oldBound.CenterPoint().y;
			
			double vx = gx1 - gx2;
			double vy = gy1 - gy2;
			double len = vx*vx + vy*vy;
			if (len == 0) {
				dx += ((double)rand())/(double)RAND_MAX;
				dy += ((double)rand())/(double)RAND_MAX;
			} else /*if (len < 100*100)*/ {
				dx += vx / len;
				dy += vy / len;
			}
		}
		double dlen = dx*dx + dy*dy;
		if (dlen > 0) {
			dlen = sqrt(dlen)/2;
			const_cast<iBound&>(*it1).dx += dx/dlen;
			const_cast<iBound&>(*it1).dy += dy/dlen;
		}
	}
	
	it1 = bounds.begin();
	for ( ; it1 != bounds.end(); it1++) {
		double x = max(-5, min(5, (*it1).dx));
		double y = max(-5, min(5, (*it1).dy));
		CRect rc = (*it1).oldBound;
		rc.OffsetRect((int)x, (int)y);
		// 領域のチェック
		if (rc.left < 0) {
			rc.right = (*it1).oldBound.Width();
			rc.left = 0;
		}
		if (rc.top < 0) {
			rc.bottom = (*it1).oldBound.Height();
			rc.top = 0;
		}
		if (!(*it1).fixed) {
			const_cast<iBound&>(*it1).newBound = rc;
		}	
		const_cast<iBound&>(*it1).dx /= 2;
		const_cast<iBound&>(*it1).dy /= 2;
	}
	
	EnterCriticalSection(&CRelaxThrd::m_csGDILock);
	{
		Bounds::iterator it = bounds.begin();
		for ( ; it != bounds.end(); it++) {
			CRect old = (*it).oldBound;
			CRect nw = (*it).newBound;
			if (m_bWhole) {
				old.left*= (LONG)m_rateH;
				old.right*= (LONG)m_rateH;
				old.top *= (LONG)m_rateV;
				old.bottom *= (LONG)m_rateV;
				nw.left*= (LONG)m_rateH;
				nw.right*= (LONG)m_rateH;
				nw.top *= (LONG)m_rateV;
				nw.bottom *= (LONG)m_rateV;
			} else {
				old.OffsetRect(-m_ptScroll);
				nw.OffsetRect(-m_ptScroll);
			}
			m_dc.FrameRect(old, &m_brush2);
			m_dc.FrameRect(nw, &m_brush);
			const_cast<iBound&>(*it).oldBound = const_cast<iBound&>(*it).newBound;
		}
	
		// Win32 optimizes GDI calls by collecting them in a batch
		// and then thunking the whole batch at once on a per
		// thread basis.  Since we share an HDC with multiple
		// threads, we must flush the batch before yielding to
		// other threads that will adjust the HDC.  To see what
		// I mean, comment out the GdiFlush() function call and
		// watch the results.
		GdiFlush();
		if (bounds.size() < 30) {
			Sleep(10);
		}
	}
	LeaveCriticalSection(&CRelaxThrd::m_csGDILock);
}


void CRelaxThrd::KillThread()
{
	// Note: this function is called in the context of other threads,
	//  not the thread itself.

	// reset the m_hEventKill which signals the thread to shutdown
	VERIFY(SetEvent(m_hEventKill));

	// allow thread to run at higher priority during kill process
	SetThreadPriority(THREAD_PRIORITY_ABOVE_NORMAL);
	WaitForSingleObject(m_hEventDead, INFINITE);
	WaitForSingleObject(m_hThread, INFINITE);

	// now delete CWinThread object since no longer necessary
	bounds.clear();
	edges.clear();
	delete this;
}

void CRelaxThrd::Delete()
{
	// calling the base here won't do anything but it is a good habit
	CWinThread::Delete();

	// acknowledge receipt of kill notification
	VERIFY(SetEvent(m_hEventDead));
	VERIFY(SetEvent(m_hAnotherDead));
}
