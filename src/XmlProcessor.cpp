#include "stdafx.h"
#include "XmlProcessor.h"
#include "StringUtil.h"
#include "iEdit.h"
#include "FileUtil.h"
#include <locale>

XmlProcessor::XmlProcessor(node_vec& nodesImport, link_vec& linksImport, DWORD& assignKey, NodeKeyPairs& idcVec) : 
	nodesImport(nodesImport), linksImport(linksImport), assignKey(assignKey), idcVec(idcVec)
{
}

XmlProcessor::~XmlProcessor()
{
}

bool XmlProcessor::Import(const CString &fileName)
{
	HRESULT hr;
	hr = CoInitialize(NULL);
	if (!SUCCEEDED(hr)) {
		AfxMessageBox(_T("XML �p�[�T�[�𗘗p�ł��܂���B"));
		return false;
	}

	MSXML2::IXMLDOMDocument		*pDoc = NULL;
	hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
		MSXML2::IID_IXMLDOMDocument, (LPVOID *)&pDoc);
	if (!pDoc) {
		AfxMessageBox(_T("XML �h�L�������g���p�[�X�ł��܂���B"));
		return false;
	}
	pDoc->put_async(VARIANT_FALSE);
	BSTR	bstr = NULL;
	bstr = fileName.AllocSysString();
	hr = pDoc->load(bstr);
	SysFreeString(bstr);

	MSXML2::IXMLDOMParseError	*pParsingErr = NULL;
	if (!hr) {
		long line, linePos;
		BSTR reason = NULL;

		pDoc->get_parseError(&pParsingErr);

		pParsingErr->get_line(&line);
		pParsingErr->get_linepos(&linePos);
		pParsingErr->get_reason(&reason);
		pParsingErr->get_errorCode(&hr);

		SysFreeString(reason);
		return false;
	}
	MSXML2::IXMLDOMElement		*element = NULL;
	pDoc->get_documentElement(&element);

	BSTR s = NULL;
	element->get_nodeTypeString(&s);

	if (!wcscmp(s, L"element")) {
		element->get_nodeName(&s);
		CString elems(s);
		if (elems != _T("iEditDoc")) {
			AfxMessageBox(_T("����� iEdit �p�� XML �t�@�C���ł͂���܂���"));
			return false;
		}
	}

	ComvertToImportData(element);
	return true;
}

void XmlProcessor::ComvertToImportData(MSXML2::IXMLDOMElement *node)
{
	MSXML2::IXMLDOMNodeList	*childs = NULL;
	MSXML2::IXMLDOMNodeList	*childs2 = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	MSXML2::IXMLDOMNode		*childnode2 = NULL;
	node->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i, j;
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		if (ename == _T("inode")) {
			iNode n(_T("add")); n.SetKey(++assignKey);
			n.ToggleFill(FALSE);
			n.SetBound(CRect(-1, -1, 0, 0));
			nodesImport.push_back(n);
			childnode->get_childNodes(&childs2);
			for (j = 0; j < childs2->Getlength(); j++) {
				childs2->get_item(j, &childnode2);
				BSTR name2 = childnode2->nodeName;
				CString ename2(name2);
				if (ename2 == _T("id")) {
					childnode2->firstChild->get_text(&s);
					CString ids(s);
					int id;
					swscanf_s((const wchar_t*)ids.GetBuffer(), _T("%d"), &id);
					NodeKeyPair idc;
					idc.first = (DWORD)id;
					idc.second = nodesImport[nodesImport.size() - 1].GetKey();
					idcVec.push_back(idc);
				}
				else if (ename2 == _T("pid")) {
					childnode2->firstChild->get_text(&s);
					CString pids(s);
					int pid;
					swscanf_s((const wchar_t*)pids.GetBuffer(), _T("%d"), &pid);
					nodesImport[nodesImport.size() - 1].SetParentKey((DWORD)pid);
				}
				else if (ename2 == _T("label")) {
					childnode2->firstChild->get_text(&s);
					CString name(s);
					nodesImport[nodesImport.size() - 1].SetName(name);
				}
				else if (ename2 == _T("text")) {
					childnode2->firstChild->get_text(&s);
					CString text(s);
					text = StringUtil::ReplaceLfToCrlf(text);
					nodesImport[nodesImport.size() - 1].SetText(text);
				}
				else if (ename2 == _T("labelAlign")) {
					childnode2->firstChild->get_text(&s);
					CString align(s);
					nodesImport[nodesImport.size() - 1].SetTextStyle(Dom2TextAlign(align));
				}
				else if (ename2 == _T("shape")) {
					childnode2->firstChild->get_text(&s);
					CString shape(s);
					nodesImport[nodesImport.size() - 1].SetShape(Dom2Shape(shape));
				}
				else if (ename2 == _T("nodeFont")) {
					LOGFONT lf = Dom2Font(childnode2);
					nodesImport[nodesImport.size() - 1].SetFontInfo(lf);
				}
				else if (ename2 == _T("labelColor")) {
					COLORREF lcr = Dom2LabelColor(childnode2);
					nodesImport[nodesImport.size() - 1].SetFontColor(lcr);
				}
				else if (ename2 == _T("bound")) {
					CRect rc = nodesImport[nodesImport.size() - 1].GetBound();
					Dom2Bound(childnode2, rc);
					rc.NormalizeRect();
					nodesImport[nodesImport.size() - 1].SetBound(rc);
				}
				else if (ename2 == _T("ForColor")) {
					COLORREF cr = Dom2ForeColor(childnode2);
					nodesImport[nodesImport.size() - 1].SetFillColor(cr);
				}
				else if (ename2 == _T("nodeLine")) {
					int lineStyle(PS_SOLID), lineWidth(0);
					Dom2NodeLine(childnode2, lineStyle, lineWidth);
					nodesImport[nodesImport.size() - 1].SetLineStyle(lineStyle);
					nodesImport[nodesImport.size() - 1].SetLineWidth(lineWidth);
				}
				else if (ename2 == _T("nodeLineColor")) {
					COLORREF cr = Dom2NodeLineColor(childnode2);
					nodesImport[nodesImport.size() - 1].SetLineColor(cr);
				}
			}
		}
		else if (ename == _T("ilink")) {
			iLink l;
			linksImport.push_back(l);
			childnode->get_childNodes(&childs2);
			for (j = 0; j < childs2->Getlength(); j++) {
				childs2->get_item(j, &childnode2);
				BSTR name2 = childnode2->nodeName;
				CString ename2(name2);
				if (ename2 == _T("from")) {
					childnode2->firstChild->get_text(&s);
					CString from(s); int idfrom; swscanf_s((const wchar_t*)from.GetBuffer(), _T("%d"), &idfrom);
					linksImport[linksImport.size() - 1].SetFromNodeKey(FindPairKey((DWORD)idfrom));
				}
				else if (ename2 == _T("to")) {
					childnode2->firstChild->get_text(&s);
					CString to(s); int idto; swscanf_s((const wchar_t*)to.GetBuffer(), _T("%d"), &idto);
					linksImport[linksImport.size() - 1].SetToNodeKey(FindPairKey((DWORD)idto));
				}
				else if (ename2 == _T("caption")) {
					childnode2->firstChild->get_text(&s);
					linksImport[linksImport.size() - 1].SetName(CString(s));
				}
				else if (ename2 == _T("linkLine")) {
					int style(PS_SOLID); int lineWidth(0); int arrow(iLink::line);
					Dom2LinkStyle(childnode2, style, lineWidth, arrow);
					linksImport[linksImport.size() - 1].SetLineStyle(style);
					linksImport[linksImport.size() - 1].SetLineWidth(lineWidth);
					linksImport[linksImport.size() - 1].SetArrowStyle(arrow);
				}
				else if (ename2 == _T("linkLineColor")) {
					COLORREF rc = Dom2LinkColor(childnode2);
					linksImport[linksImport.size() - 1].SetLinkColor(rc);
				}
				else if (ename2 == _T("pathPt")) {
					CPoint pt = Dom2LinkPathPt(childnode2);
					linksImport[linksImport.size() - 1].SetPathPt(pt);
				}
				else if (ename2 == _T("locate")) {
					childnode2->firstChild->get_text(&s);
					CString path(s);
					linksImport[linksImport.size() - 1].SetPath(path);
					linksImport[linksImport.size() - 1].SetArrowStyle(iLink::other);
				}
			}
		}
	}
}

bool XmlProcessor::SerializeFromXml(const CString& filename)
{
	MSXML2::IXMLDOMDocument		*pDoc = NULL;
	MSXML2::IXMLDOMParseError	*pParsingErr = NULL;
	MSXML2::IXMLDOMElement		*element = NULL;
	MSXML2::IXMLDOMNodeList		*childs = NULL;
	MSXML2::IXMLDOMNode			*node = NULL;

	BSTR	bstr = NULL;
	HRESULT hr;
	int     rc = 0;

	hr = CoInitialize(NULL);
	if (!SUCCEEDED(hr)) {
		AfxMessageBox(_T("XML �p�[�T�[�𗘗p�ł��܂���B"));
		return false;
	}

	hr = CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER,
		MSXML2::IID_IXMLDOMDocument, (LPVOID *)&pDoc);
	if (!pDoc) {
		AfxMessageBox(_T("XML �h�L�������g���p�[�X�ł��܂���B"));
		return false;
	}
	pDoc->put_async(VARIANT_FALSE);
	bstr = filename.AllocSysString();
	hr = pDoc->load(bstr);
	SysFreeString(bstr);

	if (!hr) {
		long line, linePos;
		BSTR reason = NULL;

		pDoc->get_parseError(&pParsingErr);

		pParsingErr->get_line(&line);
		pParsingErr->get_linepos(&linePos);
		pParsingErr->get_reason(&reason);
		pParsingErr->get_errorCode(&hr);

		SysFreeString(reason);
		return false;
	}
	idcVec.clear(); idcVec.resize(0);
	nodesImport.clear(); nodesImport.resize(0);
	pDoc->get_documentElement(&element);

	BSTR s = NULL;
	element->get_nodeTypeString(&s);

	if (!wcscmp(s, L"element")) {
		element->get_nodeName(&s);
		CString elems(s);
		if (elems != _T("iEditDoc")) {
			AfxMessageBox(_T("�����iEdit�p��XML�t�@�C���ł͂���܂���"));
			return false;
		}
	}

	bool ret = ConvertToSerializeData(element);

	if (nodesImport.size() > 0 && nodesImport[0].GetKey() != 0) {
		CString mes = _T("�����I�ɃG�N�X�|�[�g�����f�[�^�𒼐ڊJ�����Ƃ͂ł��܂���B\n";
		mes += _T("�C���|�[�g�@�\���g�p���Ď�荞��ł��������B");
		AfxMessageBox(mes));
		return false;
	}
	return true;
}

bool XmlProcessor::ConvertToSerializeData(MSXML2::IXMLDOMElement *node)
{
	MSXML2::IXMLDOMNodeList	*childs = NULL;
	MSXML2::IXMLDOMNodeList	*childs2 = NULL;
	MSXML2::IXMLDOMNodeList	*childs3 = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	MSXML2::IXMLDOMNode		*childnode2 = NULL;
	node->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i, j;
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		if (ename == _T("inode")) {
			iNode n(_T("add")); n.SetKey(0);
			n.ToggleFill(FALSE);
			n.SetBound(CRect(-1, -1, 0, 0));
			nodesImport.push_back(n);
			childnode->get_childNodes(&childs2);
			for (j = 0; j < childs2->Getlength(); j++) {
				childs2->get_item(j, &childnode2);
				BSTR name2 = childnode2->nodeName;
				CString ename2(name2);
				if (ename2 == _T("id")) {
					childnode2->firstChild->get_text(&s);
					CString ids(s);
					int id;
					swscanf_s((const wchar_t*)ids.GetBuffer(), _T("%d"), &id);

					//	ids += ' '; f->WriteString(ids); // log
					nodesImport[nodesImport.size() - 1].SetKey((DWORD)id);
					if (id == 0) {
						nodesImport[nodesImport.size() - 1].SetTreeState(TVIS_EXPANDED | TVIS_SELECTED);
					}
					else {
						nodesImport[nodesImport.size() - 1].SetTreeState(0);
					}
				}
				else if (ename2 == _T("pid")) {
					childnode2->firstChild->get_text(&s);
					CString pids(s);
					int pid;
					swscanf_s((const wchar_t *)pids.GetBuffer(), _T("%d"), &pid);
					nodesImport[nodesImport.size() - 1].SetParentKey((DWORD)pid);
					/*	} else if (ename2 == "level") {
					childnode2->firstChild->get_text(&s);
					CString sLevel(s);
					int nLevel;
					sscanf(sLevel, "%d", &nLevel);
					nodesImport[nodesImport.size()-1].SetLevel(nLevel); */
				}
				else if (ename2 == _T("label")) {
					childnode2->firstChild->get_text(&s);
					CString name(s);
					nodesImport[nodesImport.size() - 1].SetName(name);

					//	name += '\n'; f->WriteString(name); // log

				}
				else if (ename2 == _T("text")) {
					childnode2->firstChild->get_text(&s);
					CString text(s);
					text = StringUtil::ReplaceLfToCrlf(text);
					nodesImport[nodesImport.size() - 1].SetText(text);
				}
				else if (ename2 == _T("labelAlign")) {
					childnode2->firstChild->get_text(&s);
					CString align(s);
					nodesImport[nodesImport.size() - 1].SetTextStyle(Dom2TextAlign(align));
				}
				else if (ename2 == _T("shape")) {
					childnode2->firstChild->get_text(&s);
					CString shape(s);
					nodesImport[nodesImport.size() - 1].SetShape(Dom2Shape(shape));
				}
				else if (ename2 == _T("bound")) {
					CRect rc = nodesImport[nodesImport.size() - 1].GetBound();
					Dom2Bound(childnode2, rc);
					rc.NormalizeRect();
					nodesImport[nodesImport.size() - 1].SetBound(rc);
				}
				else if (ename2 == _T("ForColor")) {
					COLORREF cr = Dom2ForeColor(childnode2);
					nodesImport[nodesImport.size() - 1].SetFillColor(cr);
				}
				else if (ename2 == _T("nodeLine")) {
					int lineStyle(PS_SOLID), lineWidth(0);
					Dom2NodeLine(childnode2, lineStyle, lineWidth);
					nodesImport[nodesImport.size() - 1].SetLineStyle(lineStyle);
					nodesImport[nodesImport.size() - 1].SetLineWidth(lineWidth);
				}
				else if (ename2 == _T("nodeLineColor")) {
					COLORREF cr = Dom2NodeLineColor(childnode2);
					nodesImport[nodesImport.size() - 1].SetLineColor(cr);
				}
			}
		}
		else if (ename == _T("ilink")) {
			iLink l;
			linksImport.push_back(l);
			childnode->get_childNodes(&childs2);
			for (j = 0; j < childs2->Getlength(); j++) {
				childs2->get_item(j, &childnode2);
				BSTR name2 = childnode2->nodeName;
				CString ename2(name2);
				if (ename2 == _T("from")) {
					childnode2->firstChild->get_text(&s);
					CString from(s); int idfrom; swscanf_s((const wchar_t*)from.GetBuffer(), _T("%d"), &idfrom);
					linksImport[linksImport.size() - 1].SetFromNodeKey(((DWORD)idfrom));
				}
				else if (ename2 == _T("to")) {
					childnode2->firstChild->get_text(&s);
					CString to(s); int idto; swscanf_s((const wchar_t*)to.GetBuffer(), _T("%d"), &idto);
					linksImport[linksImport.size() - 1].SetToNodeKey(((DWORD)idto));
				}
				else if (ename2 == _T("caption")) {
					childnode2->firstChild->get_text(&s);
					linksImport[linksImport.size() - 1].SetName(CString(s));
				}
				else if (ename2 == _T("linkLine")) {
					int style(PS_SOLID); int lineWidth(0); int arrow(iLink::line);
					Dom2LinkStyle(childnode2, style, lineWidth, arrow);
					linksImport[linksImport.size() - 1].SetLineStyle(style);
					linksImport[linksImport.size() - 1].SetLineWidth(lineWidth);
					linksImport[linksImport.size() - 1].SetArrowStyle(arrow);
				}
				else if (ename2 == _T("linkLineColor")) {
					COLORREF rc = Dom2LinkColor(childnode2);
					linksImport[linksImport.size() - 1].SetLinkColor(rc);
				}
				else if (ename2 == _T("pathPt")) {
					CPoint pt = Dom2LinkPathPt(childnode2);
					linksImport[linksImport.size() - 1].SetPathPt(pt);
				}
				else if (ename2 == _T("locate")) {
					childnode2->firstChild->get_text(&s);
					CString path(s);
					linksImport[linksImport.size() - 1].SetPath(path);
					linksImport[linksImport.size() - 1].SetArrowStyle(iLink::other);
				}
			}
		}
	}
	return true;
}


int XmlProcessor::Dom2TextAlign(const CString &tag)
{
	if (tag == _T("single-middle-center")) {
		return iNode::s_cc;
	}
	else if (tag == _T("single-middle-left")) {
		return iNode::s_cl;
	}
	else if (tag == _T("single-midele-right")) {
		return iNode::s_cr;
	}
	else if (tag == _T("single-top-center")) {
		return iNode::s_tc;
	}
	else if (tag == _T("single-top-left")) {
		return iNode::s_tl;
	}
	else if (tag == _T("single-top-right")) {
		return iNode::s_tr;
	}
	else if (tag == _T("single-bottom-center")) {
		return iNode::s_bc;
	}
	else if (tag == _T("single-bottom-left")) {
		return iNode::s_bl;
	}
	else if (tag == _T("single-bottom-right")) {
		return iNode::s_br;
	}
	else if (tag == _T("multi-center")) {
		return iNode::m_c;
	}
	else if (tag == _T("multi-left")) {
		return iNode::m_l;
	}
	else if (tag == _T("multi-right")) {
		return iNode::m_r;
	}
	else if (tag == _T("hidden")) {
		return iNode::notext;
	}
	return iNode::s_cc;
}


int XmlProcessor::Dom2Shape(const CString &tag)
{
	if (tag == _T("Rect")) {
		return iNode::rectangle;
	}
	else if (tag == _T("Oval")) {
		return iNode::arc;
	}
	else if (tag == _T("RoundRect")) {
		return iNode::roundRect;
	}
	else if (tag == _T("MetaFile")) {
		return iNode::MetaFile;
	}
	else if (tag == _T("MMNode")) {
		return (iNode::MindMapNode);
	}
	return iNode::rectangle;
}

void XmlProcessor::Dom2Bound(MSXML2::IXMLDOMNode *pNode, CRect &rc)
{
	MSXML2::IXMLDOMNodeList	*childs = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	pNode->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i;
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		childnode->firstChild->get_text(&s);
		if (ename == _T("left")) {
			CString left(s);
			swscanf_s((const wchar_t*)left.GetBuffer(), _T("%d"), &rc.left);
		}
		else if (ename == _T("right")) {
			CString right(s);
			swscanf_s((const wchar_t*)right.GetBuffer(), _T("%d"), &rc.right);
		}
		else if (ename == _T("top")) {
			CString top(s);
			swscanf_s((const wchar_t*)top.GetBuffer(), _T("%d"), &rc.top);
		}
		else if (ename == _T("bottom")) {
			childnode->firstChild->get_text(&s);
			CString bottom(s);
			swscanf_s((const wchar_t*)bottom.GetBuffer(), _T("%d"), &rc.bottom);
		}
	}
}

COLORREF XmlProcessor::Dom2ForeColor(MSXML2::IXMLDOMNode *pNode)
{
	MSXML2::IXMLDOMNodeList	*childs = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	pNode->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i;
	int r(255), g(255), b(255);
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		childnode->firstChild->get_text(&s);
		if (ename == _T("f_red")) {
			CString red(s);
			swscanf_s((const wchar_t*)red.GetBuffer(), _T("%d"), &r);
		}
		else if (ename == _T("f_green")) {
			CString green(s);
			swscanf_s((const wchar_t*)green.GetBuffer(), _T("%d"), &g);
		}
		else if (ename == _T("f_blue")) {
			CString blue(s);
			swscanf_s((const wchar_t*)blue.GetBuffer(), _T("%d"), &b);
		}
	}
	return RGB(r, g, b);
}

COLORREF XmlProcessor::Dom2LabelColor(MSXML2::IXMLDOMNode *pNode)
{
	MSXML2::IXMLDOMNodeList	*childs = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	pNode->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i;
	int r(255), g(255), b(255);
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		childnode->firstChild->get_text(&s);
		if (ename == _T("lb_red")) {
			CString red(s);
			swscanf_s((const wchar_t*)red.GetBuffer(), _T("%d"), &r);
		}
		else if (ename == _T("lb_green")) {
			CString green(s);
			swscanf_s((const wchar_t*)green.GetBuffer(), _T("%d"), &g);
		}
		else if (ename == _T("lb_blue")) {
			CString blue(s);
			swscanf_s((const wchar_t*)blue.GetBuffer(), _T("%d"), &b);
		}
	}
	return RGB(r, g, b);
}

LOGFONT XmlProcessor::Dom2Font(MSXML2::IXMLDOMNode* pNode)
{
	MSXML2::IXMLDOMNodeList	*childs = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	pNode->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i;
	LOGFONT lf;
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		childnode->firstChild->get_text(&s);
		if (ename == _T("fontName")) {
			lstrcpy(lf.lfFaceName, s);
		}
		else if (ename == _T("point")) {
			CString sPoint(s);
			int point;
			swscanf_s((const wchar_t*)sPoint.GetBuffer(), _T("%d"), &point);
			lf.lfHeight = -MulDiv(point, 96, 72);
		}
		else if (ename == _T("strikeOut")) {
			CString value(s);
			lf.lfStrikeOut = value == _T("yes");
		}
		else if (ename == _T("underLine")) {
			CString value(s);
			lf.lfUnderline = value == _T("yes");
		}
		else if (ename == _T("italic")) {
			CString value(s);
			lf.lfItalic = value == _T("yes");
		}
		else if (ename == _T("bold")) {
			CString value(s);
			lf.lfWeight = value == _T("yes") ? 700 : 400;
		}
	}
	return lf;
}

void XmlProcessor::Dom2NodeLine(MSXML2::IXMLDOMNode *pNode, int &style, int &width)
{
	MSXML2::IXMLDOMNodeList	*childs = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	pNode->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i;
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		childnode->firstChild->get_text(&s);
		if (ename == _T("nodeLineStyle")) {
			CString lstyle(s);
			if (lstyle == _T("solidLine")) {
				style = PS_SOLID;
			}
			else if (lstyle == _T("dotedLine")) {
				style = PS_DOT;
			}
			else if (lstyle == _T("noLine")) {
				style = PS_NULL;
			}
		}
		else if (ename == _T("nodeLineWidth")) {
			CString lwidth(s); int w; swscanf_s((const wchar_t*)lwidth.GetBuffer(), _T("%d"), &w);
			if (w == 1) w = 0;
			width = w;
		}
	}
}


COLORREF XmlProcessor::Dom2NodeLineColor(MSXML2::IXMLDOMNode *pNode)
{
	MSXML2::IXMLDOMNodeList	*childs = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	pNode->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i;
	int r(255), g(255), b(255);
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		childnode->firstChild->get_text(&s);
		if (ename == _T("l_red")) {
			CString red(s);
			swscanf_s((const wchar_t*)red.GetBuffer(), _T("%d"), &r);
		}
		else if (ename == _T("l_green")) {
			CString green(s);
			swscanf_s((const wchar_t*)green.GetBuffer(), _T("%d"), &g);
		}
		else if (ename == _T("l_blue")) {
			CString blue(s);
			swscanf_s((const wchar_t*)blue.GetBuffer(), _T("%d"), &b);
		}
	}
	return RGB(r, g, b);
}

void XmlProcessor::Dom2LinkStyle(MSXML2::IXMLDOMNode *pNode, int &style, int &width, int &arrow)
{
	MSXML2::IXMLDOMNodeList	*childs = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	pNode->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i;
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		childnode->firstChild->get_text(&s);
		if (ename == _T("linkLineStyle")) {
			CString lstyle(s);
			if (lstyle == _T("solidLine")) {
				style = PS_SOLID;
			}
			else if (lstyle == _T("dotedLine")) {
				style = PS_DOT;
			}
		}
		else if (ename == _T("linkLineWidth")) {
			CString lwidth(s); int w; swscanf_s((const wchar_t*)lwidth.GetBuffer(), _T("%d"), &w);
			if (w == 1) w = 0;
			width = w;
		}
		else if (ename == _T("arrow")) {
			CString astyle(s);
			if (astyle == _T("a_none")) {
				arrow = iLink::line;
			}
			else if (astyle == _T("a_single")) {
				arrow = iLink::arrow;
			}
			else if (astyle == _T("a_double")) {
				arrow = iLink::arrow2;
			}
			else if (astyle == _T("a_depend")) {
				arrow = iLink::depend;
			}
			else if (astyle == _T("a_depend_double")) {
				arrow = iLink::depend2;
			}
			else if (astyle == _T("a_inherit")) {
				arrow = iLink::inherit;
			}
			else if (astyle == _T("a_aggregat")) {
				arrow = iLink::aggregat;
			}
			else if (astyle == _T("a_composit")) {
				arrow = iLink::composit;
			}
		}
	}
}

COLORREF XmlProcessor::Dom2LinkColor(MSXML2::IXMLDOMNode *pNode)
{
	MSXML2::IXMLDOMNodeList	*childs = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	pNode->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i;
	int r(255), g(255), b(255);
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		childnode->firstChild->get_text(&s);
		if (ename == _T("n_red")) {
			CString red(s);
			swscanf_s((const wchar_t*)red.GetBuffer(), _T("%d"), &r);
		}
		else if (ename == _T("n_green")) {
			CString green(s);
			swscanf_s((const wchar_t*)green.GetBuffer(), _T("%d"), &g);
		}
		else if (ename == _T("n_blue")) {
			CString blue(s);
			swscanf_s((const wchar_t*)blue.GetBuffer(), _T("%d"), &b);
		}
	}
	return RGB(r, g, b);
}


CPoint XmlProcessor::Dom2LinkPathPt(MSXML2::IXMLDOMNode *pNode)
{
	CPoint pt(0, 0);
	MSXML2::IXMLDOMNodeList	*childs = NULL;
	MSXML2::IXMLDOMNode		*childnode = NULL;
	pNode->get_childNodes(&childs);
	BSTR s = NULL;
	LONG i;
	for (i = 0; i < childs->Getlength(); i++) {
		childs->get_item(i, &childnode);
		BSTR name = childnode->nodeName;
		CString ename(name);
		childnode->firstChild->get_text(&s);
		if (ename == _T("path_x")) {
			CString x(s);
			swscanf_s((const wchar_t*)x.GetBuffer(), _T("%d"), &pt.x);
		}
		else if (ename == _T("path_y")) {
			CString y(s);
			swscanf_s((const wchar_t*)y.GetBuffer(), _T("%d"), &pt.y);
		}

	}
	return pt;
}

DWORD XmlProcessor::FindPairKey(const DWORD first)
{
	for (unsigned int i = 0; i < idcVec.size(); i++) {
		if (idcVec[i].first == first) {
			return idcVec[i].second;
		}
	}
	return -1;
}

bool XmlProcessor::Save(const CString &outPath, bool bSerialize, iNodes& nodes, iLinks& links, NodePropsVec& ls)
{
	FILE* fp;
	if ((fp = FileUtil::CreateStdioFile(outPath)) == NULL) return false;
	CStdioFile f(fp);

	_wsetlocale(LC_ALL, _T("jpn"));
	// Header of XML file
	f.WriteString(_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
	OutputStyleSheetLine(f);
	f.WriteString(_T("<iEditDoc>\n"));

	// iNodes -->iNode Data
	for (unsigned int i = 0; i < ls.size(); i++) {
		node_c_iter it = nodes.FindRead(ls[i].key);

		f.WriteString(_T("\t<inode>\n"));

		CString ids;
		DWORD key, parent;
		key = (*it).second.GetKey(); parent = (*it).second.GetParentKey();
		if (i == 0 && key != parent) {
			parent = key;
		}
		ids.Format(_T("\t\t<id>%d</id>\n\t\t<pid>%d</pid>\n"), key, parent);
		f.WriteString(ids);

		CString sLevel;
		sLevel.Format(_T("\t\t<level>%d</level>\n"), (*it).second.GetLevel());
		f.WriteString(sLevel);

		f.WriteString(_T("\t\t<label>"));
		CString title = _T("<![CDATA[") + (*it).second.GetName() + _T("]]>");
		if ((*it).second.GetTextStyle() >= iNode::m_c) {
			f.WriteString(StringUtil::ReplaceCrToLf(title));
		}
		else {
			f.WriteString(title);
		}
		f.WriteString(_T("</label>\n"));

		f.WriteString(_T("\t\t<text>"));
		CString text = _T("<![CDATA[") + (*it).second.GetText() + _T("]]>");
		f.WriteString(StringUtil::ReplaceCrToLf(text));
		f.WriteString(_T("\n\t\t</text>\n"));

		// ���x���̃A���C�����g
		f.WriteString(_T("\t\t<labelAlign>"));
		int align = (*it).second.GetTextStyle(); CString salign;
		switch (align) {
		case iNode::s_cc: salign = _T("single-middle-center"); break;
		case iNode::s_cl: salign = _T("single-middle-left"); break;
		case iNode::s_cr: salign = _T("single-midele-right"); break;
		case iNode::s_tc: salign = _T("single-top-center"); break;
		case iNode::s_tl: salign = _T("single-top-left"); break;
		case iNode::s_tr: salign = _T("single-top-right"); break;
		case iNode::s_bc: salign = _T("single-bottom-center"); break;
		case iNode::s_bl: salign = _T("single-bottom-left"); break;
		case iNode::s_br: salign = _T("single-bottom-right"); break;
		case iNode::m_c: salign = _T("multi-center"); break;
		case iNode::m_l: salign = _T("multi-left"); break;
		case iNode::m_r: salign = _T("multi-right"); break;
		case iNode::notext: salign = _T("hidden"); break;
		}
		f.WriteString(salign);
		f.WriteString(_T("</labelAlign>\n"));

		// �`
		f.WriteString(_T("\t\t<shape>"));
		int shape = (*it).second.GetShape();
		if (shape == iNode::rectangle) {
			f.WriteString(_T("Rect"));
		}
		else if (shape == iNode::arc) {
			f.WriteString(_T("Oval"));
		}
		else if (shape == iNode::roundRect) {
			f.WriteString(_T("RoundRect"));
		}
		else if (shape == iNode::MetaFile) {
			f.WriteString(_T("MetaFile"));
		}
		else if (shape == iNode::MindMapNode) {
			f.WriteString(_T("MMNode"));
		}
		else {
			f.WriteString(_T("Rect"));
		}
		f.WriteString(_T("</shape>\n"));

		// �ʒu
		CString spt;
		CRect bound = (*it).second.GetBound();

		f.WriteString(_T("\t\t<bound>\n"));
		f.WriteString(_T("\t\t\t<left>"));
		spt.Format(_T("%d"), bound.left);
		f.WriteString(spt);
		f.WriteString(_T("</left>\n"));

		f.WriteString(_T("\t\t\t<right>"));
		spt.Format(_T("%d"), bound.right);
		f.WriteString(spt);
		f.WriteString(_T("</right>\n"));

		f.WriteString(_T("\t\t\t<top>"));
		spt.Format(_T("%d"), bound.top);
		f.WriteString(spt);
		f.WriteString(_T("</top>\n"));

		f.WriteString(_T("\t\t\t<bottom>"));
		spt.Format(_T("%d"), bound.bottom);
		f.WriteString(spt);
		f.WriteString(_T("</bottom>\n"));
		f.WriteString(_T("\t\t</bound>\n"));


		CString sc;
		// �F(�h��Ԃ�)
		if ((*it).second.Filled()) {
			f.WriteString(_T("\t\t<ForColor>\n"));
			COLORREF fc = (*it).second.GetFillColor();
			BYTE fred = GetRValue(fc);
			BYTE fgrn = GetGValue(fc);
			BYTE fblu = GetBValue(fc);

			f.WriteString(_T("\t\t\t<f_red>"));
			sc.Format(_T("%d"), fred);
			f.WriteString(sc);
			f.WriteString(_T("</f_red>\n"));

			f.WriteString(_T("\t\t\t<f_green>"));
			sc.Format(_T("%d"), fgrn);
			f.WriteString(sc);
			f.WriteString(_T("</f_green>\n"));

			f.WriteString(_T("\t\t\t<f_blue>"));
			sc.Format(_T("%d"), fblu);
			f.WriteString(sc);
			f.WriteString(_T("</f_blue>\n"));
			f.WriteString(_T("\t\t</ForColor>\n"));
		}

		// ���̃X�^�C��
		f.WriteString(_T("\t\t<nodeLine>\n"));
		f.WriteString(_T("\t\t\t<nodeLineStyle>"));
		if ((*it).second.GetLineStyle() == PS_NULL) {
			f.WriteString(_T("noLine"));
		}
		else if ((*it).second.GetLineStyle() == PS_SOLID) {
			f.WriteString(_T("solidLine"));
		}
		else if ((*it).second.GetLineStyle() == PS_DOT) {
			f.WriteString(_T("dotedLine"));
		}
		f.WriteString(_T("</nodeLineStyle>\n"));

		if ((*it).second.GetLineStyle() == PS_SOLID) {
			f.WriteString(_T("\t\t\t<nodeLineWidth>"));
			int width = (*it).second.GetLineWidth();
			if (width == 0) {
				width = 1;
			}
			CString sl; sl.Format(_T("%d"), width);
			f.WriteString(sl);
			f.WriteString(_T("</nodeLineWidth>\n"));
		}
		f.WriteString(_T("\t\t</nodeLine>\n"));

		// �F(��)
		if ((*it).second.GetLineStyle() != PS_NULL) {
			f.WriteString(_T("\t\t<nodeLineColor>\n"));
			COLORREF lc = (*it).second.GetLineColor();
			BYTE lred = GetRValue(lc);
			BYTE lgrn = GetGValue(lc);
			BYTE lblu = GetBValue(lc);

			f.WriteString(_T("\t\t\t<l_red>"));
			sc.Format(_T("%d"), lred);
			f.WriteString(sc);
			f.WriteString(_T("</l_red>\n"));

			f.WriteString(_T("\t\t\t<l_green>"));
			sc.Format(_T("%d"), lgrn);
			f.WriteString(sc);
			f.WriteString(_T("</l_green>\n"));

			f.WriteString(_T("\t\t\t<l_blue>"));
			sc.Format(_T("%d"), lblu);
			f.WriteString(sc);
			f.WriteString(_T("</l_blue>\n"));
			f.WriteString(_T("\t\t</nodeLineColor>\n"));
		}

		// �t�H���g
		LOGFONT lf = (*it).second.GetFontInfo();
		f.WriteString(_T("\t\t<nodeFont>\n"));
		CString sFn; sFn.Format(_T("\t\t\t<fontName>%s</fontName>\n"), lf.lfFaceName);
		f.WriteString(sFn);
		int point = MulDiv(-lf.lfHeight, 72, 96);
		CString sPoint; sPoint.Format(_T("\t\t\t<point>%d</point>\n"), point);
		f.WriteString(sPoint);

		if (lf.lfUnderline) {
			f.WriteString(_T("\t\t\t<underLine>yes</underLine>\n"));
		}
		else {
			f.WriteString(_T("\t\t\t<underLine>no</underLine>\n"));
		}
		if (lf.lfItalic) {
			f.WriteString(_T("\t\t\t<italic>yes</italic>\n"));
		}
		else {
			f.WriteString(_T("\t\t\t<italic>no</italic>\n"));
		}
		if (lf.lfStrikeOut) {
			f.WriteString(_T("\t\t\t<strikeOut>yes</strikeOut>\n"));
		}
		else {
			f.WriteString(_T("\t\t\t<strikeOut>no</strikeOut>\n"));
		}
		if (lf.lfWeight >= 600) {
			f.WriteString(_T("\t\t\t<bold>yes</bold>\n"));
		}
		else {
			f.WriteString(_T("\t\t\t<bold>no</bold>\n"));
		}
		f.WriteString(_T("\t\t</nodeFont>\n"));

		// ���x���J���[
		f.WriteString(_T("\t\t<labelColor>\n"));
		COLORREF lc = (*it).second.GetFontColor();
		BYTE lbred = GetRValue(lc);
		BYTE lbgrn = GetGValue(lc);
		BYTE lbblu = GetBValue(lc);

		f.WriteString(_T("\t\t\t<lb_red>"));
		CString slbc;
		slbc.Format(_T("%d"), lbred);
		f.WriteString(slbc);
		f.WriteString(_T("</lb_red>\n"));

		f.WriteString(_T("\t\t\t<lb_green>"));
		slbc.Format(_T("%d"), lbgrn);
		f.WriteString(slbc);
		f.WriteString(_T("</lb_green>\n"));

		f.WriteString(_T("\t\t\t<lb_blue>"));
		slbc.Format(_T("%d"), lbblu);
		f.WriteString(slbc);
		f.WriteString(_T("</lb_blue>\n"));
		f.WriteString(_T("\t\t</labelColor>\n"));

		// end of inode tag
		f.WriteString(_T("\t</inode>\n"));
	}

	// iLinks --> iLink Data
	link_c_iter li = links.begin();
	for (; li != links.end(); li++) {
		if (!NodePropsContainsKey(nodes, ls, (*li).GetFromNodeKey()) || !NodePropsContainsKey(nodes, ls, (*li).GetToNodeKey())) {
			continue;
		}

		f.WriteString(_T("\t<ilink>\n"));
		CString links;
		if ((*li).GetArrowStyle() != iLink::other) {
			links.Format(_T("\t\t<from>%d</from>\n\t\t<to>%d</to>\n"), (*li).GetFromNodeKey(), (*li).GetToNodeKey());
		}
		else {
			links.Format(_T("\t\t<from>%d</from>\n\t\t<to>%d</to>\n"), (*li).GetFromNodeKey(), (*li).GetFromNodeKey());
		}
		f.WriteString(links);
		CString caption = _T("<![CDATA[") + (*li).GetName() + "]]>";
		f.WriteString(_T("\t\t<caption>"));
		f.WriteString(caption);
		f.WriteString(_T("</caption>\n"));

		int astyle = (*li).GetArrowStyle();

		if (astyle == iLink::other && (*li).GetPath() != "") {
			f.WriteString(_T("\t\t<locate>"));
			CString path = _T("<![CDATA[") + (*li).GetPath() + _T("]]>");
			f.WriteString(path);
			f.WriteString(_T("</locate>\n"));
		}
		else {
			f.WriteString(_T("\t\t<linkLine>\n"));

			f.WriteString(_T("\t\t\t<linkLineStyle>"));
			if ((*li).GetLineStyle() == PS_SOLID) {
				f.WriteString(_T("solidLine"));
			}
			else if ((*li).GetLineStyle() == PS_DOT) {
				f.WriteString(_T("dotedLine"));
			}
			f.WriteString(_T("</linkLineStyle>\n"));

			int width = (*li).GetLineWidth();
			if (width == 0) {
				width = 1;
			}
			CString w; w.Format(_T("\t\t\t<linkLineWidth>%d</linkLineWidth>\n"), width);
			f.WriteString(w);

			f.WriteString(_T("\t\t\t<arrow>"));
			if (astyle == iLink::line) {
				f.WriteString(_T("a_none"));
			}
			else if (astyle == iLink::arrow) {
				f.WriteString(_T("a_single"));
			}
			else if (astyle == iLink::arrow2) {
				f.WriteString(_T("a_double"));
			}
			else if (astyle == iLink::depend) {
				f.WriteString(_T("a_depend"));
			}
			else if (astyle == iLink::depend2) {
				f.WriteString(_T("a_depend_double"));
			}
			else if (astyle == iLink::inherit) {
				f.WriteString(_T("a_inherit"));
			}
			else if (astyle == iLink::aggregat) {
				f.WriteString(_T("a_aggregat"));
			}
			else if (astyle == iLink::composit) {
				f.WriteString(_T("a_composit"));
			}
			f.WriteString(_T("</arrow>\n"));

			f.WriteString(_T("\t\t</linkLine>\n"));

			f.WriteString(_T("\t\t<linkLineColor>\n"));
			CString sc;
			COLORREF nc = (*li).GetLinkColor();
			BYTE nred = GetRValue(nc);
			BYTE ngrn = GetGValue(nc);
			BYTE nblu = GetBValue(nc);

			f.WriteString(_T("\t\t\t<n_red>"));
			sc.Format(_T("%d"), nred);
			f.WriteString(sc);
			f.WriteString(_T("</n_red>\n"));

			f.WriteString(_T("\t\t\t<n_green>"));
			sc.Format(_T("%d"), ngrn);
			f.WriteString(sc);
			f.WriteString(_T("</n_green>\n"));

			f.WriteString(_T("\t\t\t<n_blue>"));
			sc.Format(_T("%d"), nblu);
			f.WriteString(sc);
			f.WriteString(_T("</n_blue>\n"));

			f.WriteString(_T("\t\t</linkLineColor>\n"));

			if ((*li).IsCurved()) {
				f.WriteString(_T("\t\t<pathPt>\n"));
				CString sp; sp.Format(_T("\t\t\t<path_x>%d</path_x>\n\t\t\t<path_y>%d</path_y>\n"),
					(*li).GetPtPath().x, (*li).GetPtPath().y);
				f.WriteString(sp);
				f.WriteString(_T("\t\t</pathPt>\n"));
			}
		}
		f.WriteString(_T("\t</ilink>\n"));
	}

	f.WriteString(_T("</iEditDoc>\n"));
	f.Flush();
	f.Close();
	_wsetlocale(LC_ALL, _T(""));
	return true;
}

bool XmlProcessor::NodePropsContainsKey(const iNodes& nodes, const NodePropsVec &labels, DWORD key)
{
	if (labels.size() == nodes.size()) return true;
	for (unsigned int i = 0; i < labels.size(); i++) {
		if (labels[i].key == key) {
			return true;
		}
	}
	return false;
}

template <class T>
void XmlProcessor::OutputStyleSheetLine(T &f)
{
	CiEditApp* pApp = (CiEditApp*)AfxGetApp();
	if (!pApp->m_rgsOther.bSetStylesheet) return;
	CString strStylesheetFile = pApp->m_rgsOther.strStyleSheetFile;
	if (strStylesheetFile == _T("")) {
		strStylesheetFile = _T("iedit.xsl");
	}
	CString s = _T("<?xml-stylesheet type=\"text/xsl\" ");
	s += _T("href=\"") + strStylesheetFile + _T("\"") + _T(" ?>\n");
	//	f.WriteString("<?xml-stylesheet type=\"text/xsl\" href=\"iedit.xsl\" ?>\n");
	f.WriteString(s);
}
