#pragma once

class MarkdownParser
{
public:
	MarkdownParser();
	~MarkdownParser();
	static CString Parse(const CString& text);

private:
	static void UlStart(CString& str, int& level, int& prevLevel);
	static void UlEnd(CString& str, int& level);
	static CString CreateInlineUrlLink(const CString& line);
};

