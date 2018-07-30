#include "stdafx.h"
#include "HtmlWriter.h"
#include <locale>
#include "iEdit.h"
#include "StringUtil.h"

HtmlWriter::HtmlWriter()
{
}


HtmlWriter::~HtmlWriter()
{
}

void HtmlWriter::CreateFrame(CStdioFile& f, const CString& title, const CString& keystr, const ExportOptions& options)
{
	f.WriteString(_T("<title>") + title + _T("</title>\n"));
	f.WriteString(_T("</head>\n"));
	if (options.navOption != 1) {
		f.WriteString(_T("  <frameset cols=\"35%,*\" >\n"));
		f.WriteString(_T("    <frame src=\"") + options.pathOutline + _T("\" name=\"outline\">\n"));
	}
	if (options.navOption == 1 || options.navOption == 2) {
		f.WriteString(_T("    <frameset rows=\"65%,*\">\n"));
		f.WriteString(_T("      <frame src=\"") + options.pathNetwork + _T("\" name=\"network\">\n  "));
	}
	CString textLink = options.pathTextSingle;
	if (options.textOption == 1) {
		textLink = _T("text/") + options.prfTextEverynode + keystr + _T(".html");
	}
	f.WriteString(_T("    <frame src=\"") + textLink + _T("\" name=\"text\">\n"));
	if (options.navOption == 1 || options.navOption == 2) {
		f.WriteString(_T("    </frameset>\n"));
	}
	if (options.navOption != 1) {
		f.WriteString(_T("  </frameset>\n"));
	}
	f.WriteString(_T("</html>\n"));
}

void HtmlWriter::WriteOutlineHeader(CStdioFile& olf, const CString& keystr, const CString& rootStr, const ExportOptions& options)
{
	if (options.navOption != 1) {
		WriteHtmlHeader(olf);
		olf.WriteString(_T("<style type=\"text/css\">\n"));
		olf.WriteString(_T(" h1 {font-size: 100%; background: #F3F3F3; padding: 5px 5px 5px;}\n"));
		olf.WriteString(_T(" li {font-size: 95%; padding: 0px;}\n"));
		olf.WriteString(_T("</style>\n"));
		olf.WriteString(_T("<title>Outline</title>\n"));
		olf.WriteString(_T("</head>\n"));
		olf.WriteString(_T("<body>\n<h1>"));
		olf.WriteString(_T("<a href="));
		if (options.textOption == 0) {
			olf.WriteString(_T("\"") + options.pathTextSingle + _T("#") + keystr);
		}
		else {
			olf.WriteString(_T("\"text/") + options.prfTextEverynode + keystr + _T(".html"));
		}
		olf.WriteString(_T("\" target=text>"));
		olf.WriteString(rootStr);
		olf.WriteString(_T("</a></h1>\n"));
		olf.WriteString(_T("<ul>\n"));
	}
}

void HtmlWriter::WriteHtmlHeader(CStdioFile &f)
{
	f.WriteString(_T("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n"));
	f.WriteString(_T("<html lang=\"ja\">\n<head>\n"));
	f.WriteString(_T("<meta http-equiv=\"content-Type\" content=\"text/html; charset=UTF-8\">\n"));
}

void HtmlWriter::WriteTextStart(CStdioFile& tf)
{
	WriteHtmlHeader(tf);
	WriteTextStyle(tf);
	tf.WriteString(_T("</head>\n<body>\n"));
}

void HtmlWriter::WriteRootTextStart(CStdioFile& rootTf)
{
	WriteHtmlHeader(rootTf);
	WriteTextStyle(rootTf, false);
	rootTf.WriteString(_T("</head>\n<body>\n"));
}

void HtmlWriter::WriteRootTextEnd(CStdioFile& rootTf)
{
	rootTf.WriteString(_T("</body>\n</html>\n"));
}

void HtmlWriter::WriteTextStyle(CStdioFile &f, bool single)
{
	f.WriteString(_T("<style type=\"text/css\">\n"));
	if (single) {
		f.WriteString(_T(" h1 {font-size: 120%; border-bottom:2pt solid #9999FF; border-left:7pt solid #9999FF; padding: 5px 5px 5px;}\n"));
	}
	else {
		f.WriteString(_T(" h1 {font-size: 120%; background: #F3F3F3; padding: 5px 5px 5px;}\n"));
	}
	f.WriteString(_T(" h2 {font-size: 110%; border-bottom:2pt solid #9999FF; border-left:3pt solid #9999FF; padding: 5px 5px 5px;}\n"));
	f.WriteString(_T(" h3 {font-size: 100%; border-bottom:1pt solid #9999FF; padding: 5px 5px 5px;}\n"));
	f.WriteString(_T(" h4 {font-size: 100%; border-bottom: 1px dashed #999999; padding: 5px 5px 5px;}\n"));
	f.WriteString(_T(" li {font-size: 80%; padding: 0px;}\n"));
	f.WriteString(_T("</style>\n"));
}

