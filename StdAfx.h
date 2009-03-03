// stdafx.h : �W���̃V�X�e�� �C���N���[�h �t�@�C���A
//            �܂��͎Q�Ɖ񐔂������A�����܂�ύX����Ȃ�
//            �v���W�F�N�g��p�̃C���N���[�h �t�@�C�����L�q���܂��B
//

#if !defined(AFX_STDAFX_H__96DFF9B9_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
#define AFX_STDAFX_H__96DFF9B9_1881_11D3_808A_00A0C9B72FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Windows �w�b�_�[����w�ǎg�p����Ȃ��X�^�b�t�����O���܂��B


#ifndef WINVER				// Windows XP �ȍ~�̃o�[�W�����ɌŗL�̋@�\�̎g�p�������܂��B
#define WINVER 0x0501		// ����� Windows �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#ifndef _WIN32_WINNT		// Windows XP �ȍ~�̃o�[�W�����ɌŗL�̋@�\�̎g�p�������܂��B                   
#define _WIN32_WINNT 0x0501	// ����� Windows �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif						

#ifndef _WIN32_WINDOWS		// Windows 98 �ȍ~�̃o�[�W�����ɌŗL�̋@�\�̎g�p�������܂��B
#define _WIN32_WINDOWS 0x0410 // ����� Windows Me �܂��͂���ȍ~�̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#ifndef _WIN32_IE			// IE 6.0 �ȍ~�̃o�[�W�����ɌŗL�̋@�\�̎g�p�������܂��B
#define _WIN32_IE 0x0600	// ����� IE �̑��̃o�[�W���������ɓK�؂Ȓl�ɕύX���Ă��������B
#endif

#include <afxwin.h>         // MFC �̃R�A����ѕW���R���|�[�l���g
#include <afxext.h>         // MFC �̊g������
#include <afxcview.h>
#include <afxdisp.h>        // MFC �̃I�[�g���[�V���� �N���X

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC �� Internet Explorer 4 �R���� �R���g���[�� �T�|�[�g
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC �� Windows �R���� �R���g���[�� �T�|�[�g
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxole.h>         // OLE�T�|�[�g LinkView��D��D�@�\�Ŏd�l

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
// Microsoft Visual C++ �͑O�s�̒��O�ɒǉ��̐錾��}�����܂��B

#endif // !defined(AFX_STDAFX_H__96DFF9B9_1881_11D3_808A_00A0C9B72FDD__INCLUDED_)
