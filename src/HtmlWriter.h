#pragma once

class HtmlWriter
{
public:
	HtmlWriter();
	~HtmlWriter();

	static void CreateFrame(CStdioFile& f, const CString& title, const CString& keystr, const ExportOptions& options);
	static void WriteOutlineHeader(CStdioFile& olf, const CString& keystr, const CString& rootStr, const ExportOptions& options);
	static void WriteTextStart(CStdioFile& tf);
	static void WriteRootTextStart(CStdioFile& rootTf);
	static void WriteRootTextEnd(CStdioFile& rootTf);
	static void WriteOutline(const CString& keyStr, const CString& itemStr, CStdioFile& olf, const ExportOptions& options);

	static void WriteHtmlHeader(CStdioFile& f);
	static void WriteTextStyle(CStdioFile& f, bool single = true);
	static void WriteOutlineEnd(CStdioFile& f);

	static void WriteText(CStdioFile& f, const CString& keyStr, const CString& label, const CString& text);
	static void BuildLinkTo(CString& strLinks, const CString& keyStr, bool textIsolated, const CString& nodeLabel, const CString& linkLabel, const CString& textPrefix);
	static void BuildLinkFrom(CString& strLinks, const CString& keyStr, bool textIsolated, const CString& nodeLabel, const CString& linkLabel, const CString& textPrefix);
	static void BuildUrlLink(CString& strLinks, const CString& url, const CString& label);
	static void WriteLinks(CStdioFile& f, const CString& strLinks);
};
