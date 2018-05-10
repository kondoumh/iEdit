#pragma once

// CRectTrackerPlus

class CRectTrackerPlus : public CRectTracker
{
private:
	bool keepRatio;
	CRect initialRect;

public:
	inline void setKeepRatio() { keepRatio = true; }
	inline void setInitialRect(const CRect& rect) { initialRect = rect; initialRect.NormalizeRect(); }
	CRectTrackerPlus();
	virtual ~CRectTrackerPlus();
	virtual void AdjustRect(int nHandle, LPRECT lpRect);
};


