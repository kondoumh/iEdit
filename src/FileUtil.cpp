#include "stdafx.h"
#include "FileUtil.h"

static int _stdcall BrowseCallback(
	HWND hWnd, UINT uiMsg, LPARAM lParam, LPARAM lpData)
{
	if (uiMsg == BFFM_INITIALIZED) {
		if (lpData != NULL) {
			SendMessage(hWnd,
				BFFM_SETSELECTION,
				TRUE,
				lpData);
		}
	}
	return(0);
}

FileUtil::FileUtil()
{
}


FileUtil::~FileUtil()
{
}

FILE* FileUtil::CreateStdioFile(const CString& path) {
	FILE* fp;
	if (_tfopen_s(&fp, path, _T("w, ccs=UTF-8")) != 0) {
		AfxMessageBox(_T("coud not open file. ") + path);
		return NULL;
	}

	return fp;
}

bool SelectFolder(CString& path, const HWND& hWnd)
{
	TCHAR szBuff[MAX_PATH];
	BROWSEINFO bi;
	bi.hwndOwner = hWnd;

	bi.pidlRoot = NULL;
	bi.pszDisplayName = szBuff;
	bi.lpszTitle = _T("HTML出力先フォルダー選択");

	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = (BFFCALLBACK)BrowseCallback;
	bi.lParam = (LPARAM)path.GetBuffer();
	bi.ulFlags &= BIF_DONTGOBELOWDOMAIN;
	bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS | BIF_EDITBOX;
	bi.iImage = 0;
	CString folder = AfxGetApp()->GetProfileString(_T("Settings"), _T("HTML OutputDir"), _T(""));
	bi.lParam = (LPARAM)folder.GetBuffer(folder.GetLength());

	LPITEMIDLIST pList = ::SHBrowseForFolder(&bi);
	if (pList == NULL) return false;
	if (::SHGetPathFromIDList(pList, szBuff)) {
		//szBuffに選択したフォルダ名が入る
		path = CString(szBuff);
	}
	else {
		AfxMessageBox(_T("出力先フォルダーを指定して下さい"));
		return false;
	}
	return true;
}
