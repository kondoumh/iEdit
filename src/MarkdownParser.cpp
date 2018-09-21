#include "stdafx.h"
#include "MarkdownParser.h"
#include "StringUtil.h"
#include <regex>


MarkdownParser::MarkdownParser()
{
}


MarkdownParser::~MarkdownParser()
{
}

CString MarkdownParser::Parse(const CString &text)
{
	const std::wregex h2(_T("^#{1} ([^#].+)$"));
	const std::wregex h3(_T("^#{2} ([^#].+)$"));
	const std::wregex h4(_T("^#{3} ([^#].+)$"));
	const std::wregex h5(_T("^#{4,} ([^#].+)$"));
	const std::wregex l1(_T("^- ([^-].+)$"));
	const std::wregex l2(_T("^ - ([^-].+)"));
	const std::wregex l3(_T("^  - ([^-].+)"));
	const std::wregex bq(_T("^```$"));
	std::match_results<std::wstring::const_iterator> result;
	std::vector<CString> lines = StringUtil::GetLines(text);
	int level = 0;
	int prevLevel = 0;
	CString rtnStr;
	bool qt = false;
	for (unsigned int i = 0; i < lines.size(); i++) {
		std::wstring line = static_cast<LPCTSTR>(lines[i]);
		if (!qt && std::regex_match(line, result, bq)) {
			qt = true;
			rtnStr += _T("<div style=\"background:#fff; padding:3px; border:1px solid #a1d8e2; \"><pre>");
			continue;
		}
		else if (qt && std::regex_match(line, result, bq)) {
			qt = false;
			rtnStr += _T("</pre></div><br />");
			continue;
		}
		if (qt) {
			rtnStr += StringUtil::EscapeHtmlSpecialCharPre(lines[i]) + _T("\n");
			continue;
		}
		else {
			line = StringUtil::EscapeHtmlSpecialChar(lines[i]);
		}
		if (std::regex_match(line, result, h2)) {
			UlEnd(rtnStr, level);
			rtnStr += _T("<h2>");
			rtnStr += CreateInlineUrlLink(CString(result[1].str().c_str()));
			rtnStr += _T("</h2>\n");
		}
		else if (std::regex_match(line, result, h3)) {
			UlEnd(rtnStr, level);
			rtnStr += _T("<h3>");
			rtnStr += CreateInlineUrlLink(CString(result[1].str().c_str()));
			rtnStr += _T("</h3>\n");
		}
		else if (std::regex_match(line, result, h4)) {
			UlEnd(rtnStr, level);
			rtnStr += _T("<h4>");
			rtnStr += CreateInlineUrlLink(CString(result[1].str().c_str()));
			rtnStr += _T("</h4>\n");
		}
		else if (std::regex_match(line, result, h5)) {
			UlEnd(rtnStr, level);
			rtnStr += _T("<h5>");
			rtnStr += CreateInlineUrlLink(CString(result[1].str().c_str()));
			rtnStr += _T("</h5>\n");
		}
		else if (std::regex_match(line, result, l1)) {
			prevLevel = level;
			level = 1;
			UlStart(rtnStr, level, prevLevel);
			rtnStr += _T("<li>");
			rtnStr += CreateInlineUrlLink(CString(result[1].str().c_str()));
			rtnStr += _T("</li>\n");
		}
		else if (std::regex_match(line, result, l2)) {
			prevLevel = level;
			level = 2;
			UlStart(rtnStr, level, prevLevel);
			rtnStr += _T("<li>");
			rtnStr += CreateInlineUrlLink(CString(result[1].str().c_str()));
			rtnStr += _T("</li>\n");
		}
		else if (std::regex_match(line, result, l3)) {
			prevLevel = level;
			level = 3;
			UlStart(rtnStr, level, prevLevel);
			rtnStr += _T("<li>");
			rtnStr += CreateInlineUrlLink(CString(result[1].str().c_str()));
			rtnStr += _T("</li>\n");
		}
		else {
			UlEnd(rtnStr, level);
			rtnStr += CreateInlineUrlLink(CString(line.c_str()));
			rtnStr += _T("<br />\n");
		}
	}
	return rtnStr;
}

// ÉCÉìÉâÉCÉìÇÃURLÇåüèoÇ∑ÇÈ
CString MarkdownParser::CreateInlineUrlLink(const CString &line)
{
	const std::wregex uri(_T("^(.*)(https?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+)(.*)$"));
	const std::wregex wikiLink(_T("^(.*)\\[\\[(.+):(https?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+)\\]\\](.*)$"));
	const std::wregex mdLink(_T("^(.*)\\[(.+)\\]\\((https?://[-_.!~*'()a-zA-Z0-9;/?:@&=+$,%#]+)\\)$"));
	std::wstring sLine = static_cast<LPCTSTR>(line);
	std::match_results<std::wstring::const_iterator> result;
	if (std::regex_match(sLine, result, mdLink)) {
		CString rtnStr = result[1].str().c_str();
		rtnStr += _T("<a href=\"");
		rtnStr += result[3].str().c_str();
		rtnStr += _T("\" target=\"_blank\">");
		rtnStr += result[2].str().c_str();
		rtnStr += _T("</a>");
		return rtnStr;
	}
	else if (std::regex_match(sLine, result, wikiLink)) {
		CString rtnStr = result[1].str().c_str();
		rtnStr += _T("<a href=\"");
		rtnStr += result[3].str().c_str();
		rtnStr += _T("\" target=\"_blank\">");
		rtnStr += result[2].str().c_str();
		rtnStr += _T("</a>");
		rtnStr += result[4].str().c_str();
		return rtnStr;
	}
	else if (std::regex_match(sLine, result, uri)) {
		CString rtnStr = result[1].str().c_str();
		rtnStr += _T("<a href=\"");
		rtnStr += result[2].str().c_str();
		rtnStr += _T("\" target=\"_blank\">");
		rtnStr += result[2].str().c_str();
		rtnStr += _T("</a>");
		rtnStr += result[3].str().c_str();
		return rtnStr;
	}
	return line;
}

void MarkdownParser::UlStart(CString& str, int& level, int& prevLevel)
{
	if (prevLevel == level - 1) {
		str += _T("<ul>\n");
	}
	else if (prevLevel == level - 2) {
		str += _T("<ul>\n<ul>\n");
	}
	else if (prevLevel == level + 1) {
		str += _T("</ul>\n");
	}
	else if (prevLevel == level + 2) {
		str += _T("</ul>\n</ul>\n");
	}
}

void MarkdownParser::UlEnd(CString & str, int& level)
{
	if (level == 1) {
		str += _T("</ul>\n");
	}
	else if (level == 2) {
		str += _T("</ul>\n</ul>\n");
	}
	else if (level == 3) {
		str += _T("</ul>\n</ul>\n</ul>\n");
	}
	if (level > 0) level = 0;
}
