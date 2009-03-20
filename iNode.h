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
	int getMarginL() const;
	int getMarginR() const;
	int getMarginT() const;
	int getMarginB() const;
	void setMarginL(int margin);
	void setMarginR(int margin);
	void setMarginT(int margin);
	void setMarginB(int margin);
	void setTreeIconId(int id);
	int getTreeIconId() const;
	void setDrawOrder(int drawOrder);
	int getDrawOrder() const;
	void restoreState();
	void backupState();
	void setInChain(bool bChain = true);
	const bool isInChain() const;
	
	const CRect& getBoundPre() const;
	void setBoundPre(const CRect& r);
	HENHMETAFILE getMetaFile() const;
	void setLevel(const int nLevel);
	const int getLevel() const;
	void setMetaFile(HENHMETAFILE& hMF);
	void fix(BOOL f=TRUE);
	BOOL isFixed() const;
	iNode(const CString& name);
	double dy;
	double dx;
	int getNodeShape() const;
	void setNodeShape(int shape);
	void moveBound(const CSize& sz);
	bool isVisible() const;
	void setVisible(bool v=true);
	bool isSelected() const;
	void selectNode(bool sel=true);
	int getTextStyle() const;
	void setTextStyle(int s);
	int getLineWidth() const;
	void setLineWidth(int width);
	int getLineStyle() const;
	void setLineStyle(int style);
	const COLORREF& getLineColor() const;
	void setLineColor(const COLORREF& c);
	void setFontColor(const COLORREF& c);
	void setBrush(const COLORREF& c);
	const COLORREF& getBrsColor() const;
	void setNoBrush(BOOL noBrush=TRUE);
	BOOL isFilled() const;
	const COLORREF& getFontColor() const;
	const LOGFONT& getFontInfo() const;
	void setFontInfo(const LOGFONT& lf, bool resize=true);
	bool operator ==(iNode &n);
	bool operator <(iNode &n);
	void setBound(const CRect& r);
	void moveTo(const CPoint& pt);
	iNode& operator=(const iNode& n);
	iNode(const iNode& n);
	virtual void Serialize(CArchive& ar);
	void SerializeEx(CArchive& ar, int version);
	const CRect& getBound() const;
	void setParent(const DWORD& parent);
	const DWORD& getParent() const;
	void setKey(const DWORD& key);
	const DWORD& getKey() const;
	void setText(const CString& text);
	const CString& getText() const;
	void setName(const CString& name);
	const CString& getName() const;
	void setTreeState(UINT state);
	UINT getTreeState() const;
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
	void enhanceBoundGradualy(int area, int wmargin, int hmargin);
	void procSingleLineInner(const CSize& sz, int wmargin, int hmargin);
	void procMultiLineInner(const CSize& sz, int wmargin, int hmargin);
	int margin_l_;
	int margin_r_;
	int margin_t_;
	int margin_b_;
	int treeIconId_;
	unsigned int drawOrder_; // 描画順序を保持
	bool bChain_;  // 芋づる
	CRect boundPre_; // AutoLayout用に・・
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
protected:
	void adjustFont(bool bForceResize=false);
	void procMultiLine();
	CSize getNodeTextSize();
};

inline void iNode::setMarginB(int margin)
{
	margin_b_ = margin;
}

inline int iNode::getMarginB() const
{
	return margin_b_;
}

inline void iNode::setMarginT(int margin)
{
	margin_t_ = margin;
}

inline int iNode::getMarginT() const
{
	return margin_t_;
}

inline void iNode::setMarginR(int margin)
{
	margin_r_ = margin;
}

inline int iNode::getMarginR() const
{
	return margin_r_;
}

inline void iNode::setMarginL(int margin)
{
	margin_l_ = margin;
}

inline int iNode::getMarginL() const
{
	return margin_l_;
}

inline void iNode::setTreeIconId(int id)
{
	treeIconId_ = id;
}

inline int iNode::getTreeIconId() const
{
	return treeIconId_;
}
	

inline void iNode::setDrawOrder(int drawOrder)
{
	drawOrder_ = drawOrder;
}

inline int iNode::getDrawOrder() const
{
	return drawOrder_;
}

inline void iNode::setLevel(const int nLevel)
{
	nLevel_ = nLevel;
}

inline const int iNode::getLevel() const
{
	return nLevel_;
}

inline BOOL iNode::isFixed() const
{
	return fixed_;
}

inline void iNode::fix(BOOL f) {
	fixed_ = f;
}

inline const CString& iNode::getName() const
{
	return name_;
}

inline const CString& iNode::getText() const
{
	return text_;
}

inline void iNode::setText(const CString &text)
{
	text_ = text;
}

inline const DWORD& iNode::getKey() const
{
	return key_;
}

inline void iNode::setKey(const DWORD &key)
{
	key_ = key;
}

inline const DWORD& iNode::getParent() const
{
	return parent_;
}

inline void iNode::setParent(const DWORD& parent)
{
	parent_ = parent;
}

inline const CRect& iNode::getBound() const
{
	return bound_;
}

inline void iNode::setTreeState(UINT state)
{
	treeState_ = state;
}

inline UINT iNode::getTreeState() const
{
	return treeState_;
}

inline void iNode::moveTo(const CPoint &pt)
{
	int width = bound_.Width();
	int height = bound_.Height();
	bound_.left = pt.x;
	bound_.top=  pt.y;
	bound_.right = pt.x + width;
	bound_.bottom = pt.y + height;
}

inline void iNode::setBound(const CRect &r)
{
	bound_ = r;
}

inline const LOGFONT& iNode::getFontInfo() const
{
	return lf_;
}

inline const COLORREF& iNode::getFontColor() const
{
	return colorFont;
}

inline void iNode::setFontColor(const COLORREF &c)
{
	colorFont = c;
}

inline void iNode::setLineColor(const COLORREF &c)
{
	colorLine = c;
}

inline const COLORREF& iNode::getLineColor() const
{
	return colorLine;
}

inline void iNode::setLineStyle(int style)
{
	styleLine = style;
	if (styleLine == PS_NULL) {
		adjustFont();
	}
}

inline int iNode::getLineStyle() const
{
	return styleLine;
}

inline void iNode::setLineWidth(int width)
{
	lineWidth = width;
}

inline int iNode::getLineWidth() const
{
	return lineWidth;
}

inline void iNode::setBrush(const COLORREF &c)
{
	colorFill = c;
	bfillcolor = TRUE;
}

inline const COLORREF& iNode::getBrsColor() const
{
	return colorFill;
}

inline void iNode::setNoBrush(BOOL noBrush)
{
	bfillcolor = noBrush;
	if (!bfillcolor) {
		adjustFont();
	}
}

inline BOOL iNode::isFilled() const
{
	return bfillcolor;
}

inline int iNode::getTextStyle() const
{
	return styleText;
}

inline void iNode::selectNode(bool sel)
{
	selected_ = sel;
}

inline bool iNode::isSelected() const
{
	return selected_;
}

inline void iNode::setVisible(bool v)
{
	visible = v;
}

inline bool iNode::isVisible() const
{
	return visible;
}

inline void iNode::moveBound(const CSize& sz)
{
	int width = bound_.Width();
	int height = bound_.Height();
	bound_.left += sz.cx;
	bound_.top += sz.cy;
	bound_.right = bound_.left + width;
	bound_.bottom = bound_.top+ height;
}

inline int iNode::getNodeShape() const
{
	return shape_;
}

inline void iNode::setNodeShape(int shape)
{
	shape_ = shape;
}

inline const CRect& iNode::getBoundPre() const
{
	return boundPre_;
}

inline void iNode::setBoundPre(const CRect& r)
{
	boundPre_ = r;
}

inline void iNode::setInChain(bool bChain)
{
	bChain_ = bChain;
}

inline const bool iNode::isInChain() const
{
	return bChain_;
}



// iNodeDrawer ノード描画ベースクラス
////////////////////////////////////////////////////////////////
class iNodeDrawer {

public:
	void draw(const iNode& node, CDC* pDC, BOOL bDrawOrderInfo);
protected:
	virtual void adjustTextArea(const iNode& node);
	CRect m_textRect;
	virtual void drawLabelSpecific(const iNode& node, CDC* pDC);
	virtual void fillBoundSpecific(const iNode& node, CDC* pDC, CBrush* brush);
	virtual void drawShapeSpecific(const iNode & node, CDC* pDC, const CPen* pen);
	virtual void drawLabel(const iNode& node, CDC* pDC, BOOL bDrawOrderInfo);
	virtual void drawShape(const iNode& node, CDC* pDC);
	virtual void fillBound(const iNode& node, CDC* pDC);
};


// iNodeRectDrawer 矩形ノード描画クラス
////////////////////////////////////////////////////////////////
class iNodeRectDrawer : public iNodeDrawer {
protected:
	void drawShapeSpecific(const iNode & node, CDC* pDC, const CPen* pen);
	void fillBoundSpecific(const iNode& node, CDC* pDC, CBrush* brush);
};

// iNodeRoundRectDrawer 角丸矩形ノード描画クラス
////////////////////////////////////////////////////////////////
class iNodeRoundRectDrawer : public iNodeDrawer {
protected:
	void drawShape(const iNode& node, CDC* pDC);
	void adjustTextArea(const iNode& node);
private:
	int m_r;
};

// iNodeArcDrawer 楕円ノード描画クラス
////////////////////////////////////////////////////////////////
class iNodeArcDrawer : public iNodeDrawer {
protected:
	void fillBoundSpecific(const iNode& node, CDC* pDC, CBrush* brush);
	void drawShapeSpecific(const iNode & node, CDC* pDC, const CPen* pen);
	void adjustTextArea(const iNode& node);
};

// iNodeMetafileDrawer メタファイルノード描画クラス
////////////////////////////////////////////////////////////////
class iNodeMetafileDrawer : public iNodeDrawer {

protected:
	void fillBoundSpecific(const iNode& node, CDC* pDC, CBrush* brush);
	void drawShape(const iNode& node, CDC* pDC);
};

// MindMap形式ノード描画クラス
////////////////////////////////////////////////////////////////
class iNodeMMNodeDrawer : public iNodeDrawer {
	void drawShapeSpecific(const iNode & node, CDC* pDC, const CPen* pen);
};

// iNodes : iNode のコレクション
////////////////////////////////////////////////////////////////
struct iNode_less : binary_function<iNode, iNode, bool> {
	bool operator()(const iNode& n1, const iNode& n2) const
	{
		return n1.getKey() < n2.getKey();
	}
};


typedef set<iNode, iNode_less> nContena;
typedef nContena::const_iterator const_niterator;
typedef nContena::iterator niterator;
typedef vector<iNode> nVec;

class iNodes : public nContena {
public:
	CString createClickableMapString();
	void setSelectedNodeMargin(int l, int r, int t, int b);
	BOOL m_bDrawOrderInfo;
	serialVec getSelectedNodeKeys() const;
	void setDrawOrder(const serialVec svec);
	void restoreNodesFixState(DWORD keyExcluded);
	void fixNodesReversibly(DWORD keyExcluded);
	void drawNodesSelected(CDC *pDC);
	iNodes();
	~iNodes();
	CSize getMaxNodeSize(bool selection=true, bool bDrwAll=false) const;
	void setSelectedNodeFixed(BOOL f=TRUE);
	BOOL isSelectedNodeFixed() const;
	const_niterator getSelectedNodeR() const;
	niterator findNodeW(const iNode& n);
	const_niterator findNode(const iNode& n) const;
	void setSelectedNodeShape(int shape);
	int getSelectedNodeShape() const;
	void setSelectedNodeNoBrush(BOOL noBrush=TRUE);
	COLORREF getSelectedNodeBrsColor() const;
	BOOL isSelectedNodeFilled() const;
	void setSelectedNodeBrush(const COLORREF& c);
	void setVisibleNodes(DWORD key);
	vector<iNode*> getVisibleNodes() const;
	void setVisibleNodes(KeySet& keySet);
	int selectNodesInBound(const CRect& bound, CRect& selRect, bool bDrwAll=false);
	int getSelectedNodeTextStyle() const;
	void setSelectedNodeTextStyle(int style);
	void setSelectedNodeTreeIconId(int id);
	int getSelectedNodeLineWidth() const;
	int getSelectedNodeLineStyle() const;
	void setSelectedNodeLineWidth(int w);
	void setSelectedNodeLineStyle(int style);
	COLORREF getSelectedNodeLineColor() const;
	void setSelectedNodeLineColor(const COLORREF& c);
	void setSelectedNodeFontColor(const COLORREF& c);
	COLORREF getSelectedNodeFontColor() const;
	void getSelectedNodeFont(LOGFONT& lf);
	void setSelectedNodeFont(const LOGFONT& lf);
	DWORD getCurParent() const;
	iNode* hitTest2(const CPoint& pt, bool bTestAll = false) const;
	void setSelectedNodeBound(const CRect& r);
	void moveSelectedNode(const CSize& sz);
	iNode* hitTest(const CPoint& pt, bool bTestAll = false);
	void drawNodes(CDC* pDC, bool bDrwAll = false);
	void initSelection();
	niterator getSelectedNode();
	void setSelKey(DWORD key);
	DWORD getSelKey() const;
private:
	vector<iNode*> nodesDraw_; // 描画用ノード配列
	serialVec svec_; // 描画順序情報
	iNodeDrawer* getNodeDrawer(const iNode& node);
	iNode nodeFind;
	DWORD curParent_;
	DWORD selKey_;
	iNodeRectDrawer* m_pNodeRectDrawer;
	iNodeRoundRectDrawer *m_pNodeRoundRectDrawer;
	iNodeArcDrawer* m_pNodeArcDrawer;
	iNodeMetafileDrawer* m_pMetafileDrawer;
	iNodeMMNodeDrawer* m_pMMNodeDrawer;
	
protected:
public:
	void resizeSelectedNodeFont(bool bEnlarge);
};

inline void iNodes::setDrawOrder(const serialVec svec)
{
	svec_ = svec;
}

inline DWORD iNodes::getSelKey() const
{
	return selKey_;
}

inline DWORD iNodes::getCurParent() const
{
	return curParent_;
}

#endif // !defined(AFX_INODE_H__FCFD79F3_18EB_11D3_A0D2_000000000000__INCLUDED_)
