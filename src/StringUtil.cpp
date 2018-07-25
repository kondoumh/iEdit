#include "stdafx.h"
#include "StringUtil.h"
#include <shlwapi.h>

StringUtil::StringUtil(void)
{
}

StringUtil::~StringUtil(void)
{
}

CString StringUtil::GetSafeFileName(const CString& target)
{
	CString rs;
	for (int i = 0; i < target.GetLength(); i++) {
		TCHAR a = target.GetAt(i);
		if ((a == '|' || a == '\\') && i > 0) {
			if (_ismbblead(target.GetAt(i - 1))) {
				rs += a;
				continue;
			}
		}
		if (a == '\n' || a == '\r') continue;
		UINT type = ::PathGetCharType(a);
		if (type == GCT_INVALID || type & GCT_WILD || type & GCT_SEPARATOR) continue;
		rs += a;
	}
	return rs;
}

CString StringUtil::RemoveCr(const CString &target)
{
	CString toStr;
	for (int i = 0; i < target.GetLength(); i++) {
		if (target[i] == '\n') {
			;
		}
		else if (target[i] == '\r') {
			toStr += " ";
		}
		else {
			toStr += target[i];
		}
	}
	return toStr;
}

vector<CString> StringUtil::GetLines(const CString &target)
{
	vector<CString> lines;
	CString line;
	for (int i = 0; i < target.GetLength(); i++) {
		if (target[i] != '\r' && target[i] != '\n') {
			line += target[i];
		}
		else if (target[i] == '\n') {
			lines.push_back(line);
			line = "";
		}
	}
	if (line != "") {
		lines.push_back(line);
	}
	return lines;
}

bool StringUtil::IsMachineDependentChar(int nByte)
{
	/*
	参考：
	// Machintosh機種依存文字コード(0x8540～0x889E)
	if ((nByte >= 0x8540) && (nByte <= 0x886F))
	return TRUE;

	// Machintosh外字及び縦組用(0xEAA5～0xFCFC)
	if ((nByte >= 0xEAA5) && (nByte <= 0xFCFC))
	return TRUE;
	*/

	// 半角カナ
	//if ((0xA1 <= nByte) && (nByte <= 0xDF))
	//	return true;

	// 13区の特殊文字の抽出 83文字
	if ((nByte >= 0x8740) && (nByte <= 0x879C))
		return true;

	// NEC選定IBM拡張文字(区点コード89区～92区) 374文字
	if ((nByte >= 0xED40) && (nByte <= 0xEEFC))
		return true;

	// IBM拡張文字(区点コード(換算)で115区～119区の抽出) 388文字
	if ((nByte >= 0xFA40) && (nByte <= 0xFC4B))
		return true;

	// 外字範囲
	if ((nByte >= 0xF040) && (nByte <= 0xF9FC))
		return true;

	return false;
}

CString StringUtil::RemoveMachineDependentChar(LPCTSTR target)
{
	CString strChkMoji;  // 検証対象の文字列
	strChkMoji.Format(_T("%s"), target);
	INT nLen = strChkMoji.GetLength();
	INT nByte; // 文字コード 
	CString strOKWords;
	strOKWords.Empty();

	for (int nPos = 0; nPos < nLen; nPos++) {
		TBYTE p_ch =
			static_cast<TBYTE>(strChkMoji.GetAt(nPos));
		// マルチバイト判定
		if (((0x81 <= p_ch) && (p_ch <= 0x9F)) || ((0xE0 <= p_ch) && (p_ch <= 0xFC))) {
			TBYTE p_ch1 = (TBYTE)strChkMoji.GetAt(nPos);
			TBYTE p_ch2 = (TBYTE)strChkMoji.GetAt(nPos + 1);
			nByte = (p_ch1 << 8) | (p_ch2);
			// 機種依存文字 or Shift-JISでない場合
			if (IsMachineDependentChar(nByte) || (p_ch > 0xef)) {
				++nPos;
				strOKWords += _T("?");
				continue;
			}
			strOKWords += strChkMoji.GetAt(nPos);
			strOKWords += strChkMoji.GetAt(nPos + 1);
			++nPos;
		}
		else {
			// 半角文字
			nByte = (TBYTE)strChkMoji.GetAt(nPos);
			// 半角文字の機種依存チェック
			if (IsMachineDependentChar(nByte)) {
				strOKWords += _T("?");
				continue;
			}

			strOKWords += strChkMoji.GetAt(nPos);
		}
	}
	return strOKWords;
}

CString StringUtil::ReplaceCrToLf(const CString &target)
{
	CString toStr;
	for (int i = 0; i < target.GetLength(); i++) {
		if (target[i] == '\n') {
			;
		}
		else if (target[i] == '\r') {
			toStr += _T("\n");
		}
		else {
			toStr += target[i];
		}
	}
	return toStr;
}

CString StringUtil::ReplaceLfToCrlf(const CString &target)
{
	CString toStr;
	for (int i = 0; i < target.GetLength(); i++) {
		if (target[i] == '\r') {
			;
		}
		else if (target[i] == '\n') {
			toStr += _T("\r\n");
		}
		else {
			toStr += target[i];
		}
	}
	return toStr;
}

bool StringUtil::IsUrl(const CString &target)
{
	if (target.Find(_T("http://")) != 0 && target.Find(_T("https://")) != 0 && target.Find(_T("ftp://")) != 0) {
		return false;
	}

	if (target.Find(_T("\r")) != -1 || target.Find(_T("\n")) != -1) return false;

	return true;
}

int StringUtil::GetIndent(const CString &target)
{
	CString res = target.SpanIncluding(_T("\t 　."));

	if (res.IsEmpty()) {
		return 0;
	}
	else {
		if (res.Find(_T("　"), 0) == 0) {
			return res.GetLength() / 2;
		}
		else {
			return res.GetLength();
		}
	}
	return 0;
}

CString StringUtil::TrimLeft(const CString &target)
{
	CString str(target);
	str.TrimLeft(_T("\t ."));
	if (str.Find(_T("　"), 0) == 0) {
		CString res = str.SpanIncluding(_T("　"));
		return str.Right(str.GetLength() - res.GetLength());
	}
	return str;
}

CString StringUtil::EscapeHtmlSpecialChar(const CString &target)
{
	CString str(target);
	str.Replace(_T("&"), _T("&amp;"));
	str.Replace(_T("<"), _T("&lt;"));
	str.Replace(_T(">"), _T("&gt;"));
	str.Replace(_T("\""), _T("&quot;"));
	return str;
}

CString StringUtil::EscapeHtmlSpecialCharPre(const CString &target)
{
	CString str(target);
	str.Replace(_T("<"), _T("&lt;"));
	str.Replace(_T(">"), _T("&gt;"));
	return str;
}
