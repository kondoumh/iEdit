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
		AfxMessageBox(_T("ファイルを作成できませんでした。") + path);
		return NULL;
	}

	return fp;
}

FILE* FileUtil::OpenStdioFile(const CString& path) {
	FILE* fp;

	if (_tfopen_s(&fp, path, _T("r, ccs=UTF-8")) != 0) {
		AfxMessageBox(_T("ファイルを開けませんでした") + path);
		return NULL;
	}

	return fp;
}

bool FileUtil::SelectFolder(CString& path, const CString& defaultPath, const HWND& hWnd)
{
	TCHAR szBuff[MAX_PATH];
	BROWSEINFO bi;
	bi.hwndOwner = hWnd;

	bi.pidlRoot = NULL;
	bi.pszDisplayName = szBuff;
	bi.lpszTitle = _T("出力先フォルダー選択");

	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	bi.lpfn = (BFFCALLBACK)BrowseCallback;
	bi.lParam = (LPARAM)path.GetBuffer();
	bi.ulFlags &= BIF_DONTGOBELOWDOMAIN;
	bi.ulFlags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS | BIF_EDITBOX;
	bi.iImage = 0;
	CString folder = defaultPath;
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

void FileUtil::SplitPath(const CString& path, CString& sDrive, CString& sDir, CString& sFile, CString& sExt)
{
	WCHAR drive[_MAX_DRIVE];
	WCHAR dir[_MAX_DIR];
	WCHAR fileName[_MAX_FNAME];
	WCHAR ext[_MAX_EXT];
	ZeroMemory(drive, _MAX_DRIVE);
	ZeroMemory(dir, _MAX_DIR);
	ZeroMemory(fileName, _MAX_FNAME);
	ZeroMemory(ext, _MAX_EXT);
	_wsplitpath_s((const wchar_t *)path, drive, _MAX_DRIVE, dir, _MAX_DIR, fileName, _MAX_FNAME, ext, _MAX_EXT);
	sDrive = drive;
	sDir = dir;
	sFile = fileName;
	sExt = ext;
}
