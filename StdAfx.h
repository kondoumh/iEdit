// stdafx.h : 標準のシステム インクルード ファイル、
//            または参照回数が多く、かつあまり変更されない
//            プロジェクト専用のインクルード ファイルを記述します。
//

#if !defined(AFX_STDAFX_H__96DFF9B9_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
#define AFX_STDAFX_H__96DFF9B9_1881_11D3_808A_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Windows ヘッダーから殆ど使用されないスタッフを除外します。


#ifndef WINVER				// Windows XP 以降のバージョンに固有の機能の使用を許可します。
#define WINVER 0x0501		// これを Windows の他のバージョン向けに適切な値に変更してください。
#endif

#ifndef _WIN32_WINNT		// Windows XP 以降のバージョンに固有の機能の使用を許可します。                   
#define _WIN32_WINNT 0x0501	// これを Windows の他のバージョン向けに適切な値に変更してください。
#endif						

#ifndef _WIN32_WINDOWS		// Windows 98 以降のバージョンに固有の機能の使用を許可します。
#define _WIN32_WINDOWS 0x0410 // これを Windows Me またはそれ以降のバージョン向けに適切な値に変更してください。
#endif

#ifndef _WIN32_IE			// IE 6.0 以降のバージョンに固有の機能の使用を許可します。
#define _WIN32_IE 0x0600	// これを IE の他のバージョン向けに適切な値に変更してください。
#endif

#include <afxwin.h>         // MFC のコアおよび標準コンポーネント
#include <afxext.h>         // MFC の拡張部分
#include <afxcview.h>
#include <afxdisp.h>        // MFC のオートメーション クラス

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC の Internet Explorer 4 コモン コントロール サポート
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC の Windows コモン コントロール サポート
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxole.h>         // OLEサポート LinkViewのDｎD機能で仕様

#import "MSXML3.DLL" named_guids
using namespace MSXML2;

#include <vector>
#include <list>
#include <map>
#include <functional>
#include <set>
#include <stack>
using namespace std;

struct listitem {
	DWORD key;
	DWORD keyTo;
	CString sTo;
	CString comment;
	CString path;
	enum {linkSL, linkDL, FileName, WebURL, linkSL2, linkDL2, linkFolder};
	int linkType;
	DWORD _keyFrom;
	int _arrowStyle;
	bool selected;
	COLORREF linkColor_;
	int linkWidth_;
	int styleLine_;
	LOGFONT lf_;
	BOOL isFromLink() {
		if (linkType == linkSL2 || linkType == linkDL2) return FALSE;
		return TRUE;
	}
};

struct label {
	CString name;
	DWORD key;
	DWORD parent;
	UINT state;
	int level;
	int treeIconId;
};

typedef vector<label> Labels;
typedef set<DWORD> KeySet;
typedef vector<listitem> lsItems;

typedef vector<DWORD> serialVec;
typedef pair<DWORD, DWORD> idConv;
typedef vector<idConv> idCVec;
typedef map<DWORD, DWORD> IdMap;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ は前行の直前に追加の宣言を挿入します。

#endif // !defined(AFX_STDAFX_H__96DFF9B9_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
