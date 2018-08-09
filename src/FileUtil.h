#pragma once
class FileUtil
{
public:
	FileUtil();
	~FileUtil();

	static FILE* CreateStdioFile(const CString& path);
	static bool SelectFolder(CString& path, const CString& defaultPath, const HWND& hWnd);
	static void SplitPath(const CString& path, CString& sDrive, CString& sDir, CString& sFile, CString& sExt);
};

