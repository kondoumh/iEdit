// CRectTrackerPlus

#pragma once

class CRectTrackerPlus : public CRectTracker
{
private:
	bool keepRatio;
	CRect initialRect;

public:
	inline void SetKeepRatio() { keepRatio = true; }
	inline void SetInitialRect(const CRect& rect) { initialRect = rect; initialRect.NormalizeRect(); }
	CRectTrackerPlus();
	virtual ~CRectTrackerPlus();
	virtual void AdjustRect(int nHandle, LPRECT lpRect);
};
