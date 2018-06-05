// RectTrackerPlus.cpp : 実装ファイル
//

#include "stdafx.h"
#include "iEdit.h"
#include "RectTrackerPlus.h"
#include <afxext.h>


// CRectTrackerPlus

CRectTrackerPlus::CRectTrackerPlus() : keepRatio(false)
{

}

CRectTrackerPlus::~CRectTrackerPlus()
{
}

void CRectTrackerPlus::AdjustRect(int nHandle, LPRECT lpRect)
{
	if (!keepRatio) return;
	LONG width = lpRect->left - lpRect->right;
	double ratio = (double)width / (double)initialRect.Width();
	LONG height = (LONG)((double)initialRect.Height() * ratio);

	if (nHandle == 0 || nHandle == 1) {
		lpRect->top = initialRect.bottom;
		lpRect->bottom = lpRect->top + height;
	}
	else if (nHandle == 2 || nHandle == 3) {
		lpRect->top = initialRect.top;
		lpRect->bottom = lpRect->top - height;
	}
}
