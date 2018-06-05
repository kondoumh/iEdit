// MfSizer.h: MfSizer クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MFSIZER_H__ADBFB298_5678_46D4_97DA_60028329A1AF__INCLUDED_)
#define AFX_MFSIZER_H__ADBFB298_5678_46D4_97DA_60028329A1AF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class MfSizer
{
public:
	static CSize getMFSize();
	MfSizer();
	virtual ~MfSizer();

};

#endif // !defined(AFX_MFSIZER_H__ADBFB298_5678_46D4_97DA_60028329A1AF__INCLUDED_)
