// MfSizer.cpp: MfSizer クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "iEdit.h"
#include "MfSizer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// 構築/消滅
//////////////////////////////////////////////////////////////////////

MfSizer::MfSizer()
{

}

MfSizer::~MfSizer()
{

}

CSize MfSizer::getMFSize()
{
	int mfWidth = 267;
	int mfHeight = 267;
	OSVERSIONINFO info; info.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	
	::GetVersionEx(&info);
	if (info.dwPlatformId == VER_PLATFORM_WIN32_NT) {
		if (info.dwMajorVersion == 4) { 
			mfWidth = 253;
			mfHeight = 238;
		} else if (info.dwMajorVersion == 5) {
			if (info.dwMinorVersion == 0) {
				mfWidth = 253;
				mfHeight = 238;
			} else if (info.dwMinorVersion == 1) {
				mfWidth = 260;
				mfHeight = 255;
			}
		} else if (info.dwMajorVersion >= 6) {
			mfWidth = 353;
			mfHeight = 353;
		}
	}
	return CSize(mfWidth, mfHeight);
}
