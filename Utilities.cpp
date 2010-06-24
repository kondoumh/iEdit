#include "stdafx.h"
#include "Utilities.h"
#include <shlwapi.h>

Utilities::Utilities(void)
{
}

Utilities::~Utilities(void)
{
}

CString Utilities::getSafeFileName(const CString& str)
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

CString Utilities::removeCR(const CString &str)
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

vector<CString> Utilities::getLines(const CString &text)
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

bool Utilities::isDependChar(int nByte)
{
	/*
	�Q�l�F
	// Machintosh�@��ˑ������R�[�h(0x8540�`0x889E)
	if ((nByte >= 0x8540) && (nByte <= 0x886F))
	return TRUE;

	// Machintosh�O���y�яc�g�p(0xEAA5�`0xFCFC)
	if ((nByte >= 0xEAA5) && (nByte <= 0xFCFC))
	return TRUE;
	*/

	// ���p�J�i
	//if ((0xA1 <= nByte) && (nByte <= 0xDF))
	//	return true;

	// 13��̓��ꕶ���̒��o 83����
	if ((nByte >= 0x8740) && (nByte <= 0x879C))
		return true;

	// NEC�I��IBM�g������(��_�R�[�h89��`92��) 374����
	if ((nByte >= 0xED40) && (nByte <= 0xEEFC))
		return true;

	// IBM�g������(��_�R�[�h(���Z)��115��`119��̒��o) 388����
	if ((nByte >= 0xFA40) && (nByte <= 0xFC4B))
		return true;

	// �O���͈�
	if ((nByte >= 0xF040) && (nByte <= 0xF9FC))
		return true;

	return false;
}

CString Utilities::removeDependChar(LPCTSTR moji)
{
	CString strChkMoji;  // ���ؑΏۂ̕�����
	strChkMoji.Format(_T("%s"), moji);
	INT nLen = strChkMoji.GetLength();
	INT nByte; // �����R�[�h 
	CString strOKWords;
	strOKWords.Empty();

	for (int nPos = 0; nPos < nLen; nPos++) {
		TBYTE p_ch =
			static_cast<TBYTE>(strChkMoji.GetAt(nPos));
		// �}���`�o�C�g����
		if (((0x81 <= p_ch) && (p_ch <= 0x9F)) || ((0xE0 <= p_ch) && (p_ch <= 0xFC))) {
			TBYTE p_ch1 = (TBYTE)strChkMoji.GetAt(nPos);
			TBYTE p_ch2 = (TBYTE)strChkMoji.GetAt(nPos + 1);
			nByte = (p_ch1 << 8) | (p_ch2);
			// �@��ˑ����� or Shift-JIS�łȂ��ꍇ
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
			// ���p����
			nByte = (TBYTE)strChkMoji.GetAt(nPos);
			// ���p�����̋@��ˑ��`�F�b�N
			if (isDependChar(nByte)) {
				strOKWords += _T("?");
				continue;
			}

			strOKWords += strChkMoji.GetAt(nPos);
		}
	}
	return strOKWords;
}
