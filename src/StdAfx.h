// stdafx.h : 標準のシステム インクルード ファイル、
//            または参照回数が多く、かつあまり変更されない
//            プロジェクト専用のインクルード ファイルを記述します。

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN        // Windows ヘッダーから使用されていない部分を除外します。
#endif

#include <SDKDDKVer.h>

#ifndef _WIN32_WINNT        // Windows 7 以降のバージョンに固有の機能の使用を許可します。
#define _WIN32_WINNT 0x0601	
#endif

#include <afxwin.h>         // MFC のコアおよび標準コンポーネント
#include <afxext.h>         // MFC の拡張部分
#include <afxcview.h>
#include <afxdisp.h>        // MFC のオートメーション クラス

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>       // MFC の Internet Explorer 4 コモン コントロール サポート
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>         // MFC の Windows コモン コントロール サポート
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxole.h>         // OLE サポート LinkView の DnD 機能で使用

#include <afxcontrolbars.h>

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif

/////////////////////////////////// Standard Library ///////////////////////////////////

#include <vector>
#include <list>
#include <map>
#include <functional>
#include <set>
#include <stack>

/////////////////////////////////// Data Structure /////////////////////////////////////

struct LinkProps {
	DWORD key;
	DWORD keyTo;
	CString sTo;
	CString comment;
	CString path;
	enum { linkSL, linkDL, FileName, WebURL, linkSL2, linkDL2, linkFolder, iedFile };
	int linkType;
	DWORD _keyFrom;
	int _arrowStyle;
	bool selected;
	COLORREF linkColor_;
	int linkWidth_;
	int styleLine_;
	LOGFONT lf_;
	BOOL IsLinkFrom() {
		if (linkType == linkSL2 || linkType == linkDL2) return FALSE;
		return TRUE;
	}
};

struct NodeProps {
	CString name;
	DWORD key;
	DWORD parent;
	UINT state;
	int level;
	int treeIconId;
};

struct ExportOptions {
	int treeOption;
	int htmlOutOption;
	int navOption;
	int imgOption;
	int textOption;
	CString prfIndex;
	CString prfNet;
	CString prfToc;
	CString prfTextSingle;
	CString prfTextEverynode;
	CString pathIndex;
	CString pathTextSingle;
	CString pathNetwork;
	CString pathOutline;
	CString pathSvg;
	CString pathPng;
	CString htmlOutDir;
};

typedef std::vector<NodeProps> NodePropsVec;
typedef std::vector<LinkProps> LinkPropsVec;
typedef std::set<DWORD> NodeKeySet;
typedef std::vector<DWORD> NodeKeyVec;
typedef std::map<DWORD, DWORD> NodeKeyMap;
