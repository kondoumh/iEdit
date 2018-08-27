// iNode.h: iNode クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INODE_H__FCFD79F3_18EB_11D3_A0D2_000000000000__INCLUDED_)
#define AFX_INODE_H__FCFD79F3_18EB_11D3_A0D2_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class iNode : public CObject
{
	DECLARE_SERIAL(iNode)
public:
	bool Dragging() const;
	void SetDragging(bool dragging = true);
	void FitSizeToLabel();
	int GetTextTopPos() const;
	void SetTextTopPos(int pos);
	int GetMarginLeft() const;
	int GetMarginRight() const;
	int GetMarginTop() const;
	int GetMarginBottom() const;
	void SetMarginLeft(int margin);
	void SetMarginRight(int margin);
	void SetMarginTop(int margin);
	void SetMarginBottom(int margin);
	void SetTreeIconId(int id);
	int GetTreeIconId() const;
	void SetDrawOrder(int drawOrder);
	int GetDrawOrder() const;
	void RestoreFixState();
	void BackupFixState();
	void SetInChain(bool bChain = true);
	const bool IsInChain() const;
	const CRect& GetPrevBound() const;
	void SetPrevBound(const CRect& r);
	HENHMETAFILE GetMetaFile() const;
	void SetLevel(const int nLevel);
	const int GetLevel() const;
	void SetMetaFile(HENHMETAFILE& hMF);
	void Fix(BOOL f = TRUE);
	BOOL Fixed() const;

	iNode(const CString& name);
	double dy;
	double dx;
	int GetShape() const;
	void SetShape(int shape);
	void MoveBound(const CSize& sz);
	bool Visible() const;
	bool Deleted() const;
	void Delete();
	void SetVisible(bool v = true);
	bool Selected() const;
	void Select(bool sel = true);
	int GetTextStyle() const;
	void SetTextStyle(int s);
	int GetLineWidth() const;
	void SetLineWidth(int width);
	int GetLineStyle() const;
	void SetLineStyle(int style);
	const COLORREF& GetLineColor() const;
	void SetLineColor(const COLORREF& c);
	void SetFontColor(const COLORREF& c);
	void SetFillColor(const COLORREF& c);
	const COLORREF& GetFillColor() const;
	void ToggleFill(BOOL noBrush = TRUE);
	BOOL Filled() const;
	const COLORREF& GetFontColor() const;
	const LOGFONT& GetFontInfo() const;
	void SetFontInfo(const LOGFONT& lf, bool resize = true);
	bool operator ==(iNode &n);
	bool operator <(iNode &n);
	void SetBound(const CRect& r);
	void moveTo(const CPoint& pt);
	iNode& operator=(const iNode& n);
	iNode(const iNode& n);
	virtual void Serialize(CArchive& ar);
	void SerializeEx(CArchive& ar, int version);
	const CRect& GetBound() const;
	void SetParentKey(const DWORD& parent);
	const DWORD& GetParentKey() const;
	void SetKey(const DWORD& key);
	const DWORD& GetKey() const;
	void SetText(const CString& text);
	const CString& GetText() const;
	void SetName(const CString& name);
	const CString& GetName() const;
	void SetTreeState(UINT state);
	UINT GetTreeState() const;
	void SetChapterNumber(const CString& chapterNumber);
	const CString& GetChapterNumber() const;
	iNode();
	virtual ~iNode();

	// 文字列アライメント(12種類)
	enum {
		s_cc,
		s_cl,
		s_cr,
		s_tc,
		s_tl,
		s_tr,
		s_bc,
		s_bl,
		s_br,
		m_c,
		m_l,
		m_r,
		notext
	};

	// 描画図形
	enum {
		// 標準
		rectangle,
		arc,
		roundRect,
		MetaFile,
		// MindMap
		MindMapNode,
		// UML用
		classIcon,
		paramClassIcon,
		interfaceIcon,
		interfaceIcon2,
		objectIcon,
		actorIcon,
		usecase
	};

private:
	int scrollpos_;
	int margin_l_;
	int margin_r_;
	int margin_t_;
	int margin_b_;
	int treeIconId_;
	unsigned int drawOrder_; // 描画順序を保持
	bool bChain_;  // 芋づる
	CRect boundPre_;
	int nLevel_;
	HENHMETAFILE hMF_;
	BOOL fixed_;
	BOOL fixed_back_;
	BOOL bfillcolor;
	int shape_;
	bool visible;
	bool selected_;
	int styleText;
	int styleLine;
	CPen penLine;
	CPen penArc;
	COLORREF colorFont;
	COLORREF colorLine;
	COLORREF colorFill;
	int lineWidth;
	LOGFONT lf_;
	UINT treeState_;
	CRect bound_;
	CString text_;
	CString name_;
	DWORD parent_;
	DWORD key_;
	bool deleted_;
	bool dragging_;
	CString chapterNumber_; // 章番号 transient な属性

	void Init();
	void CopyProps(const iNode& n);
	void ExpandBoundGradually(int area);
	void ExtendLineOriented(const CSize& sz);
	void GetInnerLineInfo(const CString& str, int& lineCount, int& maxLength);
	void AdjustFont(bool bForceResize = false);
	CSize GetNodeTextSize();
};

inline int iNode::GetTextTopPos() const
{
	return scrollpos_;
}

inline void iNode::SetTextTopPos(int pos)
{
	scrollpos_ = pos;
}

inline void iNode::SetMarginBottom(int margin)
{
	margin_b_ = margin;
}

inline int iNode::GetMarginBottom() const
{
	return margin_b_;
}

inline void iNode::SetMarginTop(int margin)
{
	margin_t_ = margin;
}

inline int iNode::GetMarginTop() const
{
	return margin_t_;
}

inline void iNode::SetMarginRight(int margin)
{
	margin_r_ = margin;
}

inline int iNode::GetMarginRight() const
{
	return margin_r_;
}

inline void iNode::SetMarginLeft(int margin)
{
	margin_l_ = margin;
}

inline int iNode::GetMarginLeft() const
{
	return margin_l_;
}

inline void iNode::SetTreeIconId(int id)
{
	treeIconId_ = id;
}

inline int iNode::GetTreeIconId() const
{
	return treeIconId_;
}


inline void iNode::SetDrawOrder(int drawOrder)
{
	drawOrder_ = drawOrder;
}

inline int iNode::GetDrawOrder() const
{
	return drawOrder_;
}

inline void iNode::SetLevel(const int nLevel)
{
	nLevel_ = nLevel;
}

inline const int iNode::GetLevel() const
{
	return nLevel_;
}

inline BOOL iNode::Fixed() const
{
	return fixed_;
}

inline void iNode::Fix(BOOL f) {
	fixed_ = f;
}

inline const CString& iNode::GetName() const
{
	return name_;
}

inline const CString& iNode::GetText() const
{
	return text_;
}

inline void iNode::SetText(const CString &text)
{
	text_ = text;
}

inline const DWORD& iNode::GetKey() const
{
	return key_;
}

inline void iNode::SetKey(const DWORD &key)
{
	key_ = key;
}

inline const DWORD& iNode::GetParentKey() const
{
	return parent_;
}

inline void iNode::SetParentKey(const DWORD& parent)
{
	parent_ = parent;
}

inline const CRect& iNode::GetBound() const
{
	return bound_;
}

inline void iNode::SetTreeState(UINT state)
{
	treeState_ = state;
}

inline UINT iNode::GetTreeState() const
{
	return treeState_;
}

inline void iNode::moveTo(const CPoint &pt)
{
	int width = bound_.Width();
	int height = bound_.Height();
	bound_.left = pt.x;
	bound_.top = pt.y;
	bound_.right = pt.x + width;
	bound_.bottom = pt.y + height;
}

inline void iNode::SetBound(const CRect &r)
{
	bound_ = r;
}

inline const LOGFONT& iNode::GetFontInfo() const
{
	return lf_;
}

inline const COLORREF& iNode::GetFontColor() const
{
	return colorFont;
}

inline void iNode::SetFontColor(const COLORREF &c)
{
	colorFont = c;
}

inline void iNode::SetLineColor(const COLORREF &c)
{
	colorLine = c;
}

inline const COLORREF& iNode::GetLineColor() const
{
	return colorLine;
}

inline void iNode::SetLineStyle(int style)
{
	styleLine = style;
	if (styleLine == PS_NULL) {
		AdjustFont();
	}
}

inline int iNode::GetLineStyle() const
{
	return styleLine;
}

inline void iNode::SetLineWidth(int width)
{
	lineWidth = width;
}

inline int iNode::GetLineWidth() const
{
	return lineWidth;
}

inline void iNode::SetFillColor(const COLORREF &c)
{
	colorFill = c;
	bfillcolor = TRUE;
}

inline const COLORREF& iNode::GetFillColor() const
{
	return colorFill;
}

inline void iNode::ToggleFill(BOOL noBrush)
{
	bfillcolor = noBrush;
	if (!bfillcolor) {
		AdjustFont();
	}
}

inline BOOL iNode::Filled() const
{
	return bfillcolor;
}

inline int iNode::GetTextStyle() const
{
	return styleText;
}

inline void iNode::Select(bool sel)
{
	selected_ = sel;
}

inline bool iNode::Selected() const
{
	return selected_;
}

inline void iNode::SetVisible(bool v)
{
	visible = v;
}

inline bool iNode::Visible() const
{
	return visible;
}

inline void iNode::MoveBound(const CSize& sz)
{
	int width = bound_.Width();
	int height = bound_.Height();
	bound_.left += sz.cx;
	bound_.top += sz.cy;
	bound_.right = bound_.left + width;
	bound_.bottom = bound_.top + height;
}

inline int iNode::GetShape() const
{
	return shape_;
}

inline void iNode::SetShape(int shape)
{
	shape_ = shape;
}

inline const CRect& iNode::GetPrevBound() const
{
	return boundPre_;
}

inline void iNode::SetPrevBound(const CRect& r)
{
	boundPre_ = r;
}

inline void iNode::SetInChain(bool bChain)
{
	bChain_ = bChain;
}

inline const bool iNode::IsInChain() const
{
	return bChain_;
}

inline bool iNode::Deleted() const
{
	return deleted_;
}

inline void iNode::Delete()
{
	deleted_ = true;
}

inline void iNode::SetChapterNumber(const CString& chapterNumber)
{
	chapterNumber_ = chapterNumber;
}

inline const CString& iNode::GetChapterNumber() const
{
	return chapterNumber_;
}

// iNodeDrawer ノード描画ベースクラス
////////////////////////////////////////////////////////////////
class iNodeDrawer {

public:
	void Draw(const iNode& node, CDC* pDC, BOOL bDrawOrderInfo);
protected:
	void DrawTracker(const iNode& node, CDC* pDC);
	virtual void AdjustTextArea(const iNode& node);
	CRect m_textRect;
	virtual void DrawLabelSpecific(const iNode& node, CDC* pDC);
	virtual void FillBoundSpecific(const iNode& node, CDC* pDC, CBrush* brush);
	virtual void DrawShapeSpecific(const iNode & node, CDC* pDC, const CPen* pen);
	virtual void DrawLabel(const iNode& node, CDC* pDC, BOOL bDrawOrderInfo);
	virtual void DrawShape(const iNode& node, CDC* pDC);
	virtual void FillBound(const iNode& node, CDC* pDC);
};


// iNodeRectDrawer 矩形ノード描画クラス
////////////////////////////////////////////////////////////////
class iNodeRectDrawer : public iNodeDrawer {
protected:
	void DrawShapeSpecific(const iNode & node, CDC* pDC, const CPen* pen);
	void FillBoundSpecific(const iNode& node, CDC* pDC, CBrush* brush);
};

// iNodeRoundRectDrawer 角丸矩形ノード描画クラス
////////////////////////////////////////////////////////////////
class iNodeRoundRectDrawer : public iNodeDrawer {
protected:
	void DrawShape(const iNode& node, CDC* pDC);
	void AdjustTextArea(const iNode& node);
private:
	int m_r;
};

// iNodeArcDrawer 楕円ノード描画クラス
////////////////////////////////////////////////////////////////
class iNodeArcDrawer : public iNodeDrawer {
protected:
	void FillBoundSpecific(const iNode& node, CDC* pDC, CBrush* brush);
	void DrawShapeSpecific(const iNode & node, CDC* pDC, const CPen* pen);
	void AdjustTextArea(const iNode& node);
};

// iNodeMetafileDrawer メタファイルノード描画クラス
////////////////////////////////////////////////////////////////
class iNodeMetafileDrawer : public iNodeDrawer {

protected:
	void FillBoundSpecific(const iNode& node, CDC* pDC, CBrush* brush);
	void DrawShape(const iNode& node, CDC* pDC);
};

// MindMap形式ノード描画クラス
////////////////////////////////////////////////////////////////
class iNodeMMNodeDrawer : public iNodeDrawer {
	void DrawShapeSpecific(const iNode & node, CDC* pDC, const CPen* pen);
};

typedef map<DWORD, iNode> node_map;
typedef node_map::const_iterator node_c_iter;
typedef node_map::iterator node_iter;
typedef vector<iNode> node_vec;
typedef pair<DWORD, DWORD> NodeKeyPair;
typedef vector<NodeKeyPair> NodeKeyPairs;

class iNodes : public node_map {
public:
	BOOL m_bDrawOrderInfo;

	void DragSelected(bool dragging = true);
	CString CreateClickableMapString(const CString& fileName, bool singleText = true);
	void SetMarginToSelected(int l, int r, int t, int b);
	NodeKeyVec GetSelectedKeys() const;
	void SetDrawOrder(const NodeKeyVec svec);
	void RestoreFixState(DWORD keyExcluded);
	void FixReversibly(DWORD keyExcluded);
	void DrawSelected(CDC *pDC);
	iNodes();
	~iNodes();
	CSize GetNodeSizeMax(bool selection = true) const;
	void FixSelected(BOOL f = TRUE);
	BOOL IsSelectedFixed() const;
	node_c_iter GetSelectedConstIter() const;
	node_iter FindWrite(DWORD key);
	node_c_iter FindRead(DWORD key) const;
	void SetSelectedShape(int shape);
	int GetSelectedShape() const;
	void ToggleSelectedFill(BOOL noBrush = TRUE);
	COLORREF GetSelectedFillColor() const;
	BOOL SelectedFilled() const;
	void SetSelectedFillColor(const COLORREF& c);
	void PrepareVisibles(DWORD key);
	vector<iNode*> GetVisibles() const;
	void PrepareVisibles(NodeKeySet& keySet);
	int SelectInBound(const CRect& bound, CRect& selRect);
	int GetSelectedTextStyle() const;
	void SetSelectedTextStyle(int style);
	void SetSelectedTreeIcon(int id);
	int GetSelectedLineWidth() const;
	int GetSelectedLineStyle() const;
	void SetSelectedLineWidth(int w);
	void SetSelectedLineStyle(int style);
	COLORREF GetSelectedLineColor() const;
	void SetSelectedLineColor(const COLORREF& c);
	void SetSelectedFontColor(const COLORREF& c);
	COLORREF GetSelectedFontColor() const;
	void GetSelectedFont(LOGFONT& lf);
	void SetSelectedFont(const LOGFONT& lf);
	DWORD GetCurrentParent() const;
	iNode* HitTestSilently(const CPoint& pt) const;
	void SetSelectedBound(const CRect& r);
	void MoveSelected(const CSize& sz);
	iNode* HitTest(const CPoint& pt);
	void Draw(CDC* pDC);
	void InitSelection();
	node_iter GetSelectedIter();
	void Select(DWORD key);
	DWORD GetSelectedKey() const;
	void ResizeSelectedFont(bool bEnlarge);

private:
	vector<iNode*> nodesDraw_; // 描画用ノード配列
	NodeKeyVec svec_; // 描画順序情報
	iNodeDrawer* getNodeDrawer(const iNode& node);
	DWORD curParent_;
	DWORD selKey_;
	iNodeRectDrawer* m_pNodeRectDrawer;
	iNodeRoundRectDrawer *m_pNodeRoundRectDrawer;
	iNodeArcDrawer* m_pNodeArcDrawer;
	iNodeMetafileDrawer* m_pMetafileDrawer;
	iNodeMMNodeDrawer* m_pMMNodeDrawer;
};

inline void iNodes::SetDrawOrder(const NodeKeyVec svec)
{
	svec_ = svec;
}

inline DWORD iNodes::GetSelectedKey() const
{
	return selKey_;
}

inline DWORD iNodes::GetCurrentParent() const
{
	return curParent_;
}

#endif // !defined(AFX_INODE_H__FCFD79F3_18EB_11D3_A0D2_000000000000__INCLUDED_)
