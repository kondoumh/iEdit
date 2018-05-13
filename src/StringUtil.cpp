#include "stdafx.h"
#include "StringUtil.h"
#include <shlwapi.h>

StringUtil::StringUtil(void)
{
}

StringUtil::~StringUtil(void)
{
}

CString StringUtil::getSafeFileName(const CString& str)
{
	CString rs;
	for (int i = 0; i < str.GetLength(); i++) {
		TCHAR a = str.GetAt(i);
		if ((a == '|' || a == '\\') && i > 0) {
			if (_ismbblead(str.GetAt(i-1))) {
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

CString StringUtil::removeCR(const CString &str)
{
	CString toStr;
	for (int i = 0; i < str.GetLength(); i++) {
		if (str[i] == '\n') {
			;
		} else if (str[i] == '\r') {
			toStr += " ";
		} else {
			toStr += str[i];
		}
	}
	return toStr;
}

vector<CString> StringUtil::getLines(const CString &text)
{
	vector<CString> lines;
	CString line;
	for (int i = 0; i < text.GetLength(); i++) {
		if (text[i] != '\r' && text[i] != '\n') {
			line += text[i];
		} else if (text[i] == '\n') {
			lines.push_back(line);
			line = "";
		}
	}
	if (line != "") {
		lines.push_back(line);
	}
	return lines;
}

bool StringUtil::isDependChar(int nByte)
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

CString StringUtil::removeDependChar(LPCTSTR moji)
{
	CString strChkMoji;  // 検証対象の文字列
	strChkMoji.Format(_T("%s"), moji);
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
			if (isDependChar(nByte) || (p_ch > 0xef) ) {
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
			if (isDependChar(nByte)) {
				strOKWords += _T("?");
				continue;
			}

			strOKWords += strChkMoji.GetAt(nPos);
		}
	}
	return strOKWords;
}
