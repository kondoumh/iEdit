// iLink.h: iLink クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ILINK_H__FCFD79F4_18EB_11D3_A0D2_000000000000__INCLUDED_)
#define AFX_ILINK_H__FCFD79F4_18EB_11D3_A0D2_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class iLink : public CObject
{
DECLARE_SERIAL(iLink)
public:
	const bool isRectLink() const;
	const bool isDual() const;
	const bool isSingle() const;
	const bool isTerminalNodeKey(const DWORD key) const;
	void setInChain(bool bChain=true);
	const bool isInChain() const;
	const double getLen() const;
	void setLen(double);
	void drawSelectionTo(CDC* pDC);
	void drawSelectionFrom(CDC* pDC);
	void reverseDirection();
	bool hitTestTo_c(const CPoint& pt) const;
	bool hitTestFrom_c(const CPoint &pt) const;
	void setKey(const DWORD key);
	DWORD getKey() const;
	void movePts(const CSize& sz);
	void setKeyTo(const DWORD key);
	void setKeyFrom(const DWORD key);
	bool hitTest2(const CPoint& pt);
	CRect getBound() const;
	CRect getCommentRect() const;
	void setPathPt(const CPoint& pt);
	bool isCurved() const;
	void curve(bool c=true);
	const COLORREF& getLinkColor() const;
	void setLinkColor(const COLORREF& c);
	const CString& getPath() const;
	void setPath(const CString& path);
	void setFontInfo(const LOGFONT& lf);
	const LOGFONT& getFontInfo() const;
	int getLineStyle() const;
	void setLineStyle(int style);
	int getLineWidth() const;
	void setLineWidth(int width);
	const CRect& getSelfRect() const;
	const CPoint& getPtPath() const;
	const CPoint& getPtFrom() const;
	const CPoint& getPtTo() const;
	void selectLink(bool sel = true);
	bool isSelected() const;
	void drawSelection(CDC* pDC);
	bool hitTest(const CPoint& pt);
	int getArrowStyle() const;
	void setArrowStyle(int Style);
	virtual void Serialize(CArchive& ar);
	void SerializeEx(CArchive& ar, int version);
	const CString& getName() const;
	void setName(const CString& name);
	void setRTo(const CRect &rt);
	void setRFrom(const CRect& rf);
	iLink& operator=(const iLink& l);
	const CRect& getRectTo() const;
	const CRect& getRectFrom() const;
	DWORD getKeyTo() const;
	DWORD getKeyFrom() const;
	iLink(const iLink& l);
	bool canDraw() const;
	void setDrawFlag(bool drw = true);
	void setNodes(const CRect& rcf, const CRect& rct, DWORD keyf, DWORD keyt);
	void drawArrow(CDC* pDC);
	void drawLine(CDC* pDC);
	void drawComment(CDC* pDC, bool clipbrd=false);
	iLink();
	virtual ~iLink();
	// 矢印の種類
	enum {
		// 標準
		line,       //   ── 矢印なし
		arrow,      //   ──▼ 一方向
		arrow2,     // ▲──▼ 双方向
		other,
		// UMLライクな終端スタイル。基本的に一方向
		depend,     //   ──＞  依存 一方向
		depend2,    // ＜──＞  依存 双方向
		inherit,    // -|>    中抜き
		aggregat,   // || -<> 中抜き
		composit    // || -<> 色付
	};
	
	enum {upright, upleft, downleft, downright}; // 自己参照線の位置

private:
	void drawRectangles(CDC* pDC);
	void drawTriangles(CDC* pDC);
	CPen penCommentRect;
	int selfPos;
	CRect selfRect;
	CRgn m_rgn;
	bool curved_;
	bool selected_;
	CFont font_;
	LOGFONT lf_;
	CPen penLine;
	int lineWidth;
	COLORREF colorLine;
	int styleLine;
	int styleArrow;
	CString name_;
	CString path_;    // URL Fileなどへのリンク格納用
	bool drwFlag;
	DWORD keyTo;
	DWORD keyFrom;
	DWORD key_;
	CPoint ptTo;
	CPoint ptFrom;
	CPoint ptPath;
	CRect rcTo;
	CRect rcFrom;
	double len_;        // ばねモデル用リンク長(直線距離)
	bool bChain_;
protected:
	void rotateArrow(CPoint *pPoint, int size, CPoint &pFrom, CPoint &pTo, CPoint &ptOrg);
	CPoint getClossPoint(const CRect& target, const CPoint& start);
	void setConnectPoint();
};

inline void iLink::setKey(const DWORD key)
{
	key_ = key;
}

inline DWORD iLink::getKey() const
{
	return key_;
}

inline void iLink::movePts(const CSize& sz)
{
	if (keyFrom != keyTo) {
		ptFrom.x += sz.cx; ptFrom.y += sz.cy;
		ptTo.x +=   sz.cx; ptTo.y +=   sz.cy;
		ptPath.x += sz.cx; ptPath.y += sz.cy;
		rcFrom.OffsetRect(sz);
		rcTo.OffsetRect(sz);
		selfRect.OffsetRect(sz);
	} else {
		ptPath.x += sz.cx; ptPath.y += sz.cy;
		setConnectPoint();
	}
}

inline void iLink::setKeyFrom(const DWORD key)
{
	keyFrom = key;
}

inline void iLink::setKeyTo(const DWORD key)
{
	keyTo = key;
}

inline void iLink::setPathPt(const CPoint& pt)
{
	ptPath = pt;
	curved_ = true;
}

inline void iLink::curve(bool c)
{
	curved_ = c;
	setConnectPoint();
}

inline bool iLink::isCurved() const
{
	return curved_;
}

inline void iLink::setNodes(const CRect &rcf, const CRect &rct, DWORD keyf, DWORD keyt)
{
	rcFrom = rcf; rcTo = rct; keyFrom = keyf; keyTo = keyt;
	if (keyFrom == keyTo) {
		CPoint pt = rcFrom.CenterPoint();
		ptPath.x = pt.x + rcFrom.Width();
		ptPath.y = pt.y - rcFrom.Height();
	}
	setConnectPoint();
}

inline void iLink::setRFrom(const CRect &rf)
{
	rcFrom = rf;
	setConnectPoint();
}

inline void iLink::setRTo(const CRect &rt)
{
	rcTo = rt;
	setConnectPoint();
}

inline void iLink::setDrawFlag(bool drw)
{
	drwFlag = drw;
}

inline bool iLink::canDraw() const
{
	return drwFlag;
}

inline DWORD iLink::getKeyFrom() const
{
	return keyFrom;
}

inline DWORD iLink::getKeyTo() const
{
	return keyTo;
}

inline const CRect& iLink::getRectFrom() const
{
	return rcFrom;
}

inline const CRect& iLink::getRectTo() const
{
	return rcTo;
}

inline const CRect& iLink::getSelfRect() const
{
	return selfRect;
}

inline void iLink::setName(const CString &name)
{
	name_  = name;
}

inline const CString& iLink::getName() const
{
	return name_;
}

inline void iLink::setArrowStyle(int style)
{
	styleArrow = style;
}

inline int iLink::getArrowStyle() const
{
	return styleArrow;
}

inline bool iLink::isSelected() const
{
	return selected_;
}

inline void iLink::selectLink(bool sel) {
	selected_ = sel;
}

inline const CPoint& iLink::getPtFrom() const
{
	return ptFrom;
}

inline const CPoint& iLink::getPtTo() const
{
	return ptTo;
}

inline const CPoint& iLink::getPtPath() const
{
	return ptPath;
}

inline void iLink::setLineWidth(int width)
{
	lineWidth = width;
}

inline int iLink::getLineWidth() const
{
	return lineWidth;
}

inline void iLink::setLineStyle(int style)
{
	styleLine = style;
}

inline int iLink::getLineStyle() const
{
	return styleLine;
}

inline void iLink::setFontInfo(const LOGFONT& lf)
{
	lf_ = lf;
	::lstrcpy(lf_.lfFaceName, lf.lfFaceName);
	font_.CreateFontIndirect(&lf);
}

inline const LOGFONT& iLink::getFontInfo() const
{
	return lf_;
}

inline void iLink::setPath(const CString& path)
{
	path_ = path;
}

inline const CString& iLink::getPath() const
{
	return path_;
}

inline void iLink::setLinkColor(const COLORREF& c)
{
	colorLine = c;
}

inline const COLORREF& iLink::getLinkColor() const
{
	return colorLine;
}

inline const double iLink::getLen() const
{
	return len_;
}

inline void iLink::setLen(double len)
{
	len_ = len;
}

inline void iLink::setInChain(bool bChain)
{
	bChain_ = bChain;
}

inline const bool iLink::isInChain() const
{
	return bChain_;
}

inline const bool iLink::isTerminalNodeKey(const DWORD key) const
{
	return (key == getKeyFrom() || key == getKeyTo());
}

// iLinks : iLink のコレクション
// 
/////////////////////////////////////////////////////////////////////
typedef list<iLink> lContena;
typedef list<iLink>::const_iterator const_literator;
typedef list<iLink>::iterator literator;

class iLinks : public lContena {
public:
	DWORD getFirstVisiblePair(DWORD key) const;
	void drawSelectionTo(CDC* pDC, bool bDrwAll);
	void drawSelectionFrom(CDC* pDC, bool bDrwAll);
	void setSelectedLinkReverse(bool bDrwAll);
	void setSelectedNodeLinkTo(DWORD key, const CRect& bound, bool bDrwAll);
	void setSelectedNodeLinkFrom(DWORD key, const CRect& bound, bool bDrwAll);
	bool hitTestTo(const CPoint &pt, DWORD &key, CString &path, bool bDrwAll);
	bool hitTestFrom(const CPoint& pt, DWORD& key, CString& path, bool bDrwAll);
	CRect getSelectedLinkBound(bool bDrawAll) const;
	bool isIsolated(DWORD key, bool bDrawAll) const;
	int getSelectedLinkLineStyle(bool bDrawAll) const;
	void setSelectedLinkLineStyle(int style, bool bDrwAll);
	int getSelectedLinkWidth(bool bDrwAll) const;
	void setSelectedLinkWidth(int w, bool bDrwAll);
	COLORREF getSelectedLinkLineColor(bool bDrwAll) const;
	void setSelectedLinkLineColor(const COLORREF& c, bool bDrwAll);
	void setSelectedLinkFont(const LOGFONT& lf, bool bDrwAll);
	void getSelectedLinkFont(LOGFONT& lf, bool bDrwAll);
	void selectLinksInBound(const CRect& r, bool bDrwAll);
	literator getSelectedLinkW(bool bDrwAll);
	const_literator getSelectedLink(bool bDrwAll) const;
	void drawSelection(CDC* pDC, bool bDrwAll);
	bool hitTest(const CPoint& pt, DWORD& key, CString& path, bool bDrwAll);
	void drawArrows(CDC* pDC, bool bDrwAll);
	void drawLines(CDC* pDC, bool bDrwAll);
	void drawComments(CDC* pDC, bool bDrwAll, bool clipbrd=false);
};

#endif // !defined(AFX_ILINK_H__FCFD79F4_18EB_11D3_A0D2_000000000000__INCLUDED_)
