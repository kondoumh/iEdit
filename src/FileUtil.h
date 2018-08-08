#pragma once
class FileUtil
{
public:
	FileUtil();
	~FileUtil();

	static FILE* CreateStdioFile(const CString& path);
	static bool SelectFolder(CString& path, const HWND& hWnd);
};

