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
	const bool IsDropTarget() const;
	void SetAsDropTarget(bool dropTarget = true);
	const bool IsTerminalNodeKey(const DWORD key) const;
	void SetInChain(bool bChain = true);
	const bool IsInChain() const;
	const double GetBoundStrength() const;
	void SetBoundStrength(double);
	void DrawSelectionTo(CDC* pDC);
	void DrawSelectionFrom(CDC* pDC);
	void ReverseDirection();
	bool HitTestConnectionPtTo(const CPoint& pt) const;
	bool HitTestConnectionPtFrom(const CPoint &pt) const;
	void SetKey(const DWORD key);
	DWORD GetKey() const;
	void MovePoints(const CSize& sz);
	void SetKeyTo(const DWORD key);
	void SetKeyFrom(const DWORD key);
	bool HitTestExcludePathPt(const CPoint& pt);
	CRect getBound() const;
	CRect getCommentRect() const;
	void SetPathPt(const CPoint& pt);
	bool IsCurved() const;
	bool IsAngled() const;
	void Curve(bool c = true);
	void Angle(bool a = true);
	const COLORREF& GetLinkColor() const;
	void SetLinkColor(const COLORREF& c);
	const CString& GetPath() const;
	void SetPath(const CString& path);
	void SetFontInfo(const LOGFONT& lf);
	const LOGFONT& GetFontInfo() const;
	int GetLineStyle() const;
	void SetLineStyle(int style);
	int SetLineWidth() const;
	void SetLineWidth(int width);
	const CRect& GetSelfRect() const;
	const CPoint& GetPtPath() const;
	const CPoint& GetPtFrom() const;
	const CPoint& GetPtTo() const;
	void Select(bool sel = true);
	bool IsSelected() const;
	void DrawSelection(CDC* pDC);
	bool HitTest(const CPoint& pt);
	int GetArrowStyle() const;
	void SetArrowStyle(int Style);
	virtual void Serialize(CArchive& ar);
	void SerializeEx(CArchive& ar, int version);
	const CString& GetName() const;
	void SetName(const CString& name);
	void SetToNodeRect(const CRect &rt);
	void SetFromNodeRect(const CRect& rf);
	iLink& operator=(const iLink& l);
	const CRect& GetToNodeRect() const;
	const CRect& GetFromNodeRect() const;
	DWORD GetToNodeKey() const;
	DWORD GetFromNodeKey() const;
	iLink(const iLink& l);
	bool CanDraw() const;
	void SetDrawable(bool drw = true);
	void SetNodes(const CRect& rcf, const CRect& rct, DWORD keyf, DWORD keyt);
	void DrawArrow(CDC* pDC);
	void DrawLine(CDC* pDC);
	void DrawAsDropTarget(CDC* pDC);
	void DrawComment(CDC* pDC, bool clipbrd = false);
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

	enum { upright, upleft, downleft, downright }; // 自己参照線の位置

private:
	CPen penCommentRect;
	int selfPos;
	CRect selfRect;
	CRgn m_rgn;
	bool curved_;  // 曲がってる
	bool angled_;  // 曲げ方が角張ってる(Defaultはfalse)
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
	bool deleted_;
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
	bool dropTarget_;

	bool Deleted() const;
	void Delete();
	const bool IsRectTermLink() const;
	const bool IsTwoWay() const;
	const bool IsOneWay() const;
	void CopyProps(const iLink& l);
	void DrawRectArraws(CDC* pDC);
	void DrawTriangleArrows(CDC* pDC);
	void RotateArrow(CPoint *pPoint, int size, CPoint &pFrom, CPoint &pTo, CPoint &ptOrg);
	CPoint GetIntersection(const CRect& target, const CPoint& start);
	void SetConnectionPoint();
};

inline void iLink::SetKey(const DWORD key)
{
	key_ = key;
}

inline DWORD iLink::GetKey() const
{
	return key_;
}

inline void iLink::MovePoints(const CSize& sz)
{
	if (keyFrom != keyTo) {
		ptFrom.x += sz.cx; ptFrom.y += sz.cy;
		ptTo.x += sz.cx; ptTo.y += sz.cy;
		ptPath.x += sz.cx; ptPath.y += sz.cy;
		rcFrom.OffsetRect(sz);
		rcTo.OffsetRect(sz);
		selfRect.OffsetRect(sz);
	}
	else {
		ptPath.x += sz.cx; ptPath.y += sz.cy;
		SetConnectionPoint();
	}
}

inline void iLink::SetKeyFrom(const DWORD key)
{
	keyFrom = key;
}

inline void iLink::SetKeyTo(const DWORD key)
{
	keyTo = key;
}

inline void iLink::SetPathPt(const CPoint& pt)
{
	ptPath = pt;
	curved_ = true;
}

inline void iLink::Curve(bool c)
{
	curved_ = c;
	SetConnectionPoint();
}

inline bool iLink::IsCurved() const
{
	return curved_;
}

inline bool iLink::IsAngled() const
{
	return angled_;
}

inline void iLink::Angle(bool a)
{
	this->angled_ = a;
	SetConnectionPoint();
}

inline void iLink::SetNodes(const CRect &rcf, const CRect &rct, DWORD keyf, DWORD keyt)
{
	rcFrom = rcf; rcTo = rct; keyFrom = keyf; keyTo = keyt;
	if (keyFrom == keyTo) {
		CPoint pt = rcFrom.CenterPoint();
		ptPath.x = pt.x + rcFrom.Width();
		ptPath.y = pt.y - rcFrom.Height();
	}
	SetConnectionPoint();
}

inline void iLink::SetFromNodeRect(const CRect &rf)
{
	rcFrom = rf;
	SetConnectionPoint();
}

inline void iLink::SetToNodeRect(const CRect &rt)
{
	rcTo = rt;
	SetConnectionPoint();
}

inline void iLink::SetDrawable(bool drw)
{
	drwFlag = drw;
}

inline bool iLink::CanDraw() const
{
	return drwFlag;
}

inline bool iLink::Deleted() const
{
	return deleted_;
}

inline void iLink::Delete()
{
	deleted_ = true;
}

inline DWORD iLink::GetFromNodeKey() const
{
	return keyFrom;
}

inline DWORD iLink::GetToNodeKey() const
{
	return keyTo;
}

inline const CRect& iLink::GetFromNodeRect() const
{
	return rcFrom;
}

inline const CRect& iLink::GetToNodeRect() const
{
	return rcTo;
}

inline const CRect& iLink::GetSelfRect() const
{
	return selfRect;
}

inline void iLink::SetName(const CString &name)
{
	name_ = name;
}

inline const CString& iLink::GetName() const
{
	return name_;
}

inline void iLink::SetArrowStyle(int style)
{
	styleArrow = style;
}

inline int iLink::GetArrowStyle() const
{
	return styleArrow;
}

inline bool iLink::IsSelected() const
{
	return selected_;
}

inline void iLink::Select(bool sel) {
	selected_ = sel;
}

inline const CPoint& iLink::GetPtFrom() const
{
	return ptFrom;
}

inline const CPoint& iLink::GetPtTo() const
{
	return ptTo;
}

inline const CPoint& iLink::GetPtPath() const
{
	return ptPath;
}

inline void iLink::SetLineWidth(int width)
{
	lineWidth = width;
}

inline int iLink::SetLineWidth() const
{
	return lineWidth;
}

inline void iLink::SetLineStyle(int style)
{
	styleLine = style;
}

inline int iLink::GetLineStyle() const
{
	return styleLine;
}

inline void iLink::SetFontInfo(const LOGFONT& lf)
{
	lf_ = lf;
	::lstrcpy(lf_.lfFaceName, lf.lfFaceName);
	font_.CreateFontIndirect(&lf);
}

inline const LOGFONT& iLink::GetFontInfo() const
{
	return lf_;
}

inline void iLink::SetPath(const CString& path)
{
	path_ = path;
}

inline const CString& iLink::GetPath() const
{
	return path_;
}

inline void iLink::SetLinkColor(const COLORREF& c)
{
	colorLine = c;
}

inline const COLORREF& iLink::GetLinkColor() const
{
	return colorLine;
}

inline const double iLink::GetBoundStrength() const
{
	return len_;
}

inline void iLink::SetBoundStrength(double len)
{
	len_ = len;
}

inline void iLink::SetInChain(bool bChain)
{
	bChain_ = bChain;
}

inline const bool iLink::IsInChain() const
{
	return bChain_;
}

inline const bool iLink::IsTerminalNodeKey(const DWORD key) const
{
	return (key == GetFromNodeKey() || key == GetToNodeKey());
}

inline const bool iLink::IsDropTarget() const
{
	return dropTarget_;
}

inline void iLink::SetAsDropTarget(bool dropTarget)
{
	dropTarget_ = dropTarget;
}

// iLinks : iLink のコレクション
// 
/////////////////////////////////////////////////////////////////////
typedef list<iLink> link_list;
typedef list<iLink>::const_iterator link_c_iter;
typedef list<iLink>::iterator link_iter;
typedef vector<iLink> link_vec;

class iLinks : public link_list {
public:
	void ClearDividedKey();
	DWORD GetDividedKey() const;
	void DivideTargetLinks(DWORD dropNodekey, DWORD newLinkKey);
	link_c_iter findByKey(DWORD key) const;
	void ResizeSelectedFont(bool bEnlarge);
	DWORD GetFirstVisiblePair(DWORD key) const;
	void DrawSelectionTo(CDC* pDC);
	void DrawSelectionFrom(CDC* pDC);
	void ReversetSelected();
	void SetSelectedLinkNodeTo(DWORD key, const CRect& bound);
	void SetSelectedLinkNodeFrom(DWORD key, const CRect& bound);
	bool HitTestTo(const CPoint &pt, DWORD &key, CString &path);
	bool HitTestFrom(const CPoint& pt, DWORD& key, CString& path);
	CRect GetSelectedBound() const;
	bool IsNodeIsolated(DWORD nodeKey) const;
	int GetSelectedLineStyle() const;
	void SetSelectedLineStyle(int style);
	int GetSelectedWidth() const;
	void SetSelectedWidth(int w);
	COLORREF GetSelectedLineColor() const;
	void SetSelectedLineColor(const COLORREF& c);
	void SetSelectedFont(const LOGFONT& lf);
	void GetSelectedFont(LOGFONT& lf);
	void SelectInBound(const CRect& r);
	link_iter GetSelectedWrite();
	link_iter GetSelectedWrite2();
	link_c_iter GetSelectedRead() const;
	link_c_iter GetSelectedRead2() const;
	void DrawSelection(CDC* pDC);
	bool HitTest(const CPoint& pt, DWORD& key, CString& path);
	DWORD HitTestDropTarget(const CPoint& pt, const DWORD selectedNodeKey);
	void DrawArrows(CDC* pDC);
	void DrawLines(CDC* pDC);
	void DrawComments(CDC* pDC, bool clipbrd = false);
	iLinks();
private:
	DWORD dividedLinkKey_;
};

#endif // !defined(AFX_ILINK_H__FCFD79F4_18EB_11D3_A0D2_000000000000__INCLUDED_)
