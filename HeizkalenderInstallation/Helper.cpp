// Heizkalender-Installer
// Copyright (C) 2026 Martin Richter (xMRi-Software) - heizkalender@m-ri.de
//
// Dieses Programm ist freie Software: Sie können es unter den Bedingungen
// der GNU General Public License, wie von der Free Software Foundation
// veröffentlicht, weitergeben und/oder modifizieren, entweder gemäß
// Version 3 der Lizenz oder (nach Ihrer Wahl) jeder späteren Version.
//
// Dieses Programm wird in der Hoffnung verteilt, dass es nützlich ist,
// jedoch OHNE JEDE GEWÄHRLEISTUNG; sogar ohne die implizite Gewährleistung
// der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
// Weitere Details finden Sie in der GNU General Public License.
//
// Sie sollten eine Kopie der GNU General Public License zusammen mit
// diesem Programm erhalten haben. Falls nicht, siehe
// <https://www.gnu.org/licenses/>.
// 
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pch.h"
#include "Helper.h"

CString StrValueByIndex(PCTSTR pszFullString, int iSubString, TCHAR chSep)
{                         
	CString rString;

	// Wenn nichts da leeren String zurück
	if (pszFullString == NULL)
		return rString;
	while (iSubString--)
	{
		// Siche nächsten delimiter  
		pszFullString = _tcschr(pszFullString, chSep);
		if (pszFullString == NULL)
			// Nichts mehr da also leeres Ergebnis
			return rString;
		pszFullString++;
	}
	// Ende errechnen
	PCTSTR pchEnd = _tcschr(pszFullString, chSep);
	size_t nLen = (pchEnd == NULL) ? _tcslen(pszFullString) : (pchEnd - pszFullString);
	ASSERT(nLen >= 0);
	// Daten kopieren
	CopyMemory(rString.GetBufferSetLength(static_cast<int>(nLen)), pszFullString, nLen*sizeof(TCHAR));		
	return rString;
}

int NumberOfStrValues(PCTSTR pszStr, TCHAR chSep)
{
	// Ist der String leer, ist das Ergebnis 0
	if (!*pszStr)
		return 0;
	// Separatoren zählen. Damit haben wir mindestens einen substr
	int iCount = 1;
	while (pszStr = _tcschr(pszStr, chSep))
		++iCount;
	return iCount;
}

//	UTF-8 converter
//	  Taken from https://code.msdn.microsoft.com/C-UTF-8-Conversion-Helpers-22c0a664

CStringW UTF8toUnicode(const char* utf8, int utf8Length)
{
	//
	// Special case of empty input string
	//
	if (utf8 == NULL || *utf8 == '\0')
		return {};


	// Prefetch the length of the input UTF-8 string
	if (utf8Length == -1)
		utf8Length = static_cast<int>(strlen(utf8));

	// Skip BOM if any
	if (utf8Length >= 3 && utf8[0] == '\xef' && utf8[1] == '\xbb' && utf8[2] == '\xbf')
	{
		// SKIP BOM
		utf8 += 3;
		utf8Length -= 3;
	}

	// Never fail if an invalid input character is encountered!!! Previous versions
	// used MB_ERR_INVALID_CHARS here. But in most cases we just always want a result.
	// Even if it contains some "invalid characters" !
	const DWORD conversionFlags = 0;

	//
	// Get length (in wchar_t's) of resulting UTF-16 string
	//
	const int utf16Length = ::MultiByteToWideChar(
		CP_UTF8,            // convert from UTF-8
		conversionFlags,    // flags
		utf8,               // source UTF-8 string
		utf8Length,         // length (in chars) of source UTF-8 string
		NULL,               // unused - no conversion done in this step
		0                   // request size of destination buffer, in wchar_t's
	);
	if (utf16Length == 0)
		return {};

	//
	// Allocate destination buffer for UTF-16 string
	//
	CStringW utf16;

	//
	// Do the conversion from UTF-8 to UTF-16
	//
	if (!::MultiByteToWideChar(
		CP_UTF8,            // convert from UTF-8
		0,                  // validation was done in previous call, 
		// so speed up things with default flags
		utf8,               // source UTF-8 string
		utf8Length,         // length (in chars) of source UTF-8 string
		utf16.GetBuffer(utf16Length),       // destination buffer
		utf16Length			// size of destination buffer, in wchar_t's
	))
	{
		// Error
		ASSERT(FALSE);
		return {};
	}

	utf16.ReleaseBuffer(utf16Length);

	//
	// Return resulting UTF-16 string
	//
	return utf16;
}

CStringW ISO8859_1_toUnicode(const char* isostr, int isostrLength)
{
	//
	// Special case of empty input string
	//
	if (isostr == NULL || *isostr == '\0')
		return {};


	// Prefetch the length of the input iso-8859-1 string
	if (isostrLength == -1)
		isostrLength = static_cast<int>(strlen(isostr));

	// Never fail if an invalid input character is encountered!!! Previous versions
	// used MB_ERR_INVALID_CHARS here. But in most cases we just always want a result.
	// Even if it contains some "invalid characters" !
	const DWORD conversionFlags = 0;

	//
	// Get length (in wchar_t's) of resulting UTF-16 string
	//
	const int utf16Length = ::MultiByteToWideChar(
		28591,				// convert from iso-8859-1
		conversionFlags,    // flags
		isostr,             // source UTF-8 string
		isostrLength,       // length (in chars) of source UTF-8 string
		NULL,               // unused - no conversion done in this step
		0                   // request size of destination buffer, in wchar_t's
	);
	if (utf16Length == 0)
		return {};

	//
	// Allocate destination buffer for UTF-16 string
	//
	CStringW utf16;

	//
	// Do the conversion from UTF-8 to UTF-16
	//
	if (!::MultiByteToWideChar(
		28591,				// convert from iso-8859-1
		0,                  // validation was done in previous call, 
		// so speed up things with default flags
		isostr,             // source UTF-8 string
		isostrLength,       // length (in chars) of source UTF-8 string
		utf16.GetBuffer(utf16Length),       // destination buffer
		utf16Length			// size of destination buffer, in wchar_t's
	))
	{
		// Error
		ASSERT(FALSE);
		return {};
	}

	utf16.ReleaseBuffer(utf16Length);

	//
	// Return resulting UTF-16 string
	//
	return utf16;
}

CStringA UnicodeToUTF8(const wchar_t* utf16, int utf16Length)
{
	// Check if we have a length
	if (utf16Length==-1)
		utf16Length = static_cast<int>(wcslen(utf16));

	// Safely fail if an invalid UTF-16 character sequence is encountered
	constexpr DWORD kFlags = WC_ERR_INVALID_CHARS;

	// Get the length, in chars, of the resulting UTF-8 string
	const int utf8Len = ::WideCharToMultiByte(
		CP_UTF8,            // convert to UTF-8
		kFlags,             // conversion flags
		utf16,				// source UTF-16 string
		utf16Length,	        // length of source UTF-16 string, in wchar_ts
		nullptr,            // unused - no conversion required in this step
		0,                  // request size of destination buffer, in chars
		nullptr, nullptr    // unused
	);

	if (utf8Len == 0)
		return {};

	// Make room in the destination string for the converted bits
	CStringA utf8;
	auto* utf8Ptr = utf8.GetBuffer(utf8Len);

	// Do the actual conversion from UTF-16 to UTF-8
	int result = ::WideCharToMultiByte(
		CP_UTF8,            // convert to UTF-8
		kFlags,             // conversion flags
		utf16,			    // source UTF-16 string
		utf16Length,			// length of source UTF-16 string, in wchar_ts
		utf8Ptr,            // pointer to destination buffer
		utf8Len,			// size of destination buffer, in chars
		nullptr, nullptr    // unused
	);

	if (result==0)
	{
		ASSERT(FALSE);
		return {};
	}
	utf8.ReleaseBufferSetLength(result);

	// Return the converted result string
	return utf8;
}

const UINT UTF16LE_BOM     = 0xFEFF;    /* UTF16 Little Endian Byte Order Mark */
const UINT BOM_MASK        = 0xFFFF;    /* Mask for testing Byte Order Mark */
const UINT UTF8_BOM        = 0xBFBBEF;  /* UTF8 Byte Order Mark */
const UINT UTF16_BOMLEN    = 2;			/* No of Bytes in a UTF16 BOM */
const UINT UTF8_BOMLEN     = 3;			/* No of Bytes in a UTF8 BOM */

bool LoadStringFromResource(CStringW &str, LPCTSTR pszName, LPCTSTR pszType, bool bForceUTF8)
{
	ASSERT(pszName!=NULL && pszType!=NULL);

	// Usually the strings in the DB are stored as TXT data
	// So we need to check if we have Unicode, UTF-8 or plain text here!

	// Search for the Resource
	HINSTANCE	hInst = AfxFindResourceHandle(pszName, pszType);
	if (!hInst)
	{
		ASSERT(FALSE);
		return false;
	}

	HRSRC hRes = ::FindResource(hInst, pszName, pszType);
	if (!hRes) 
	{
		ASSERT(FALSE);
		return false;
	}

	HGLOBAL hGlobal = ::LoadResource(hInst, hRes);
	if (!hGlobal) 
	{
		ASSERT(FALSE);
		return false;
	}

	PCVOID pData = ::LockResource(hGlobal);
	if (!pData) 
	{
		ASSERT(FALSE);
		return false;
	}

	// Get the data and examine the type
	int nSize= (int)::SizeofResource(hInst, hRes);		

	// Now we have the size and the data. We need to check what encoding
	// was used. We recognize UTF-8, if we sense UTF-16 little-endian we treat it
	// as just as UCS-2!

	// Get the first 3 chars (for the BOM)
	int bom=0, bomLen=0;
	if (nSize>UTF16_BOMLEN)
		bomLen = UTF8_BOMLEN;
	else if (nSize==UTF16_BOMLEN)
		bomLen = UTF16_BOMLEN;
	::CopyMemory(&bom,pData,bomLen);

	// Check the bom length
	if ((bom & BOM_MASK)==UTF16LE_BOM)
	{
		// Fine we just copy this data and skip the BOM
		pData = static_cast<PCSTR>(pData)+UTF16_BOMLEN;
		str = static_cast<const wchar_t*>(pData);
	}
	else
	{
		// Check if UTF (recognized)
		// If there is no BOM we just treat it as MBC chars for the current active codepage
		UINT codepage = CP_ACP;
		if (bom==UTF8_BOM)
		{
			// OK found UTF8 code and skip the bom
			codepage = CP_UTF8;
			nSize -= bomLen;
			pData = static_cast<PCSTR>(pData)+UTF8_BOMLEN;
		}
		if (codepage==CP_ACP && bForceUTF8)
			codepage = CP_UTF8;

		// Get the size of the unicode string		
		int iSize = ::MultiByteToWideChar(codepage,0,static_cast<PCSTR>(pData),nSize,NULL,0);
		if (iSize>0)
		{
			// Only if there is something to convert, remember that there is a terminating 0 char.
			::MultiByteToWideChar(codepage,0,static_cast<PCSTR>(pData),nSize,str.GetBufferSetLength(iSize),iSize);

			// We truncate any trailing 0 chars if any. The above code may copy a trailing 0
			// if the resource contained one.
			while (iSize && str[iSize-1]==_T('\0'))
				--iSize;
			str.ReleaseBuffer(iSize);
		}
	}

	// succeeded
	return true;
}

bool LoadStringFromResource(CStringA &str, LPCTSTR pszName, LPCTSTR pszType, bool bForceUTF8)
{
	// Get the unicode version and convert it
	CStringW strW;
	bool bReturn = LoadStringFromResource(strW, pszName, pszType, bForceUTF8);
	str = strW;
	return bReturn;
}

double StringToDouble(PCTSTR pszText)
{
	// Convert it
	return _tstof(pszText);
}

CString DoubleToString(double dValue, int iPrec)
{
	std::stringstream stream;
	if (iPrec>=0)
		stream << std::fixed << std::setprecision(iPrec) << dValue;
	else 
		stream << std::fixed << dValue;
	auto str = CStringW{ stream.str().c_str()};

	// Wenn wir -1 als Genauigkeit haben, entfernen wir angehängt nullen
	// und evtl. den Dezimal Punkt
	if (iPrec==-1 && str.Find(_T('.'))>=0)
	{
		while (str.Right(1)==_T("0"))
			str = str.GetBufferSetLength(str.GetLength()-1);
		if (str.Right(1)==_T("."))
			str = str.GetBufferSetLength(str.GetLength()-1);
	}
	return str;
}

double RoundDouble(double dVal, int iPrec)
{
	// Need rounding?
	if (iPrec<0)
		return dVal;

	// Shift
	double 	dShift=1.0;
	for (int i=0; i<iPrec; ++i)
		dShift *= 10.0;

	// multiply and split (get integral Part)
	double	dResult, dFrac;
	if (fabs(dFrac=modf(dVal*dShift,&dResult))>=0.5)
		// Check sign and round
		dResult += dFrac<0.0 ? -1.0:1.0;

	// Shift it back
	return dResult/dShift;
}

bool StringToBool(CString const &strText)
{
	if (strText.CompareNoCase(_T("true"))==0)
		return true;
	else if (strText.CompareNoCase(_T("false"))==0)
		return false;
	else
		return StringToInt(strText)!=0;
}

CString BoolToString(bool bVal)
{
	return bVal ? _T("true") : _T("false");
}

static inline void DATEToSystemTime(DATE dDate, SYSTEMTIME &st)
{
	COleDateTime date(dDate);
	date.GetAsSystemTime(st);
}

int DateToString(DATE dDate, PTSTR pszBuffer, int iMaxLen, PCTSTR pszFormat)
{
	ASSERT(iMaxLen>0);
	// Null date?
	if (dDate<=0)
	{
		*pszBuffer = _T('\0');
		return 0;
	}

	// Convert Date to System Time
	SYSTEMTIME st;
	DATEToSystemTime(dDate,st);
	return DateToString(st,pszBuffer,iMaxLen,pszFormat);
}

int DateToString(const SYSTEMTIME &sTime, PTSTR pszBuffer, int iMaxLen, PCTSTR pszFormat)
{
	ASSERT(iMaxLen>0);
	// Null date?
	if (IsSystemTimeNull(sTime))
	{
		*pszBuffer = _T('\0');
		return 0;
	}

	// Get mask
	int iLen = ::GetDateFormat(NULL, NULL, &sTime, pszFormat, pszBuffer,iMaxLen);
	// do not return length including '\0'
	return iLen ? iLen-1 : 0;
}

CString DateToString(DATE dDate, PCTSTR pszFormat)
{
	CString strText;
	int iLen = DateToString(dDate,strText.GetBuffer(MAX_DATE_LENGTH),MAX_DATE_LENGTH+1);
	strText.ReleaseBuffer(iLen);
	return strText;
}

CString DateToString(const COleDateTime &date, PCTSTR pszFormat)
{
	return DateToString(date.GetStatus()==COleDateTime::valid ? date.m_dt : 0);
}

bool GetFileContent(const CString& strFileName, CString& str)
{
	str.Empty();

	CFile file;
	CFileException e;
	if (file.Open(strFileName, CFile::modeRead, &e))
	{
		CStringA strData;
		auto size = static_cast<int>(file.GetLength());
		file.Read(CStrBufA(strData,size),size);
		str = UTF8toUnicode(strData.GetString());

		// /r/n nach newline
		str.Replace(_T("\r\n"),_T("\n"));
		return true;
	}
	else
		return false;
}

CString DateToString(const SYSTEMTIME &sTime)
{
	CString strText;
	int iLen = DateToString(sTime,strText.GetBuffer(MAX_DATE_LENGTH),MAX_DATE_LENGTH+1);
	strText.ReleaseBuffer(iLen);
	return strText;
}

COleDateTime ParseDate(PCSTR pszStr)
{
	// Skip until we found a digit or period
	//while (*pszStr!=_T('.') && (*pszStr<_T('0') || *pszStr>_T('9'))
	//	++pszStr;

	int iDay{}, iMonth{}, iYear{};
	if (sscanf_s(pszStr, "%2d.%2d.%4d", &iDay, &iMonth, &iYear)==3)
		return COleDateTime{ iYear,iMonth,iDay,0,0,0 };
	else
		return COleDateTime{ 0.0 };
}

COleDateTime ParseDate(PCWSTR pszStr)
{
	// Skip until we found a digit or period
	//while (*pszStr!=_T('.') && (*pszStr<_T('0') || *pszStr>_T('9'))
	//	++pszStr;

	int iDay{}, iMonth{}, iYear{};
	if (swscanf_s(pszStr, _T("%2d.%2d.%4d"), &iDay, &iMonth, &iYear)==3)
		return COleDateTime{ iYear,iMonth,iDay,0,0,0 };
	else
		return COleDateTime{ 0.0 };
}

#define DISALLOWEDFILENAMECHARS			_T("\x7f\\/\":*?<>|")
#define DISALLOWEDNAMECHARS				DISALLOWEDFILENAMECHARS _T("'´`[]{}~!;^")
#define DISALLOWEDIDENTIFIERCHARS		DISALLOWEDNAMECHARS _T("()+-=.,&%§@#$ª²³µ¹º")

bool IsUnicodeSpace(wchar_t c)
{
	//	This test return true for a simple space too.
	return	c==0x0020 ||	// SPACE
			c==0x00A0 ||	// NO-BREAK SPACE
			c==0x1680 ||	// OGHAM SPACE MARK
			c==0x180E ||	// MONGOLIAN VOWEL SEPARATOR
			c==0x2000 ||	// EN QUAD
			c==0x2001 ||	// EM QUAD
			c==0x2002 ||	// EN SPACE (nut)
			c==0x2003 ||	// EM SPACE (mutton)
			c==0x2004 ||	// THREE-PER-EM SPACE (thick space)
			c==0x2005 ||	// FOUR-PER-EM SPACE (mid space)
			c==0x2006 ||	// SIX-PER-EM SPACE
			c==0x2007 ||	// FIGURE SPACE
			c==0x2008 ||	// PUNCTUATION SPACE
			c==0x2009 ||	// THIN SPACE
			c==0x200A ||	// HAIR SPACE
			c==0x200B ||	// ZERO WIDTH SPACE
			c==0x202F ||	// NARROW NO-BREAK SPACE
			c==0x205F ||	// MEDIUM MATHEMATICAL SPACE
			c==0x3000 ||	// IDEOGRAPHIC SPACE
			c==0xFEFF;		// ZERO WIDTH NO-BREAK SPACE	
}

bool IsValidFilenameChar(TCHAR c)
{
#ifdef _UNICODE
	// Unicode spaces are not allowed
	if (c!=L' ' && IsUnicodeSpace(c))
		return false;
#endif	
	// No chars in the control range, but nearly everything is allowed 
	return (c<0 || c>=' ') && _tcschr(DISALLOWEDFILENAMECHARS,c)==NULL;
}

CString CleanupNameForRoom(CString str)
{
	// Siehe auch code in den Init Skripen
	str.Replace(_T(" "),_T(""));
	str.Replace(_T("\'"),_T(""));
	str.Replace(_T("\""),_T(""));
	str.Replace(_T("+"),_T(""));
	str.Replace(_T("#"),_T(""));
	str.Replace(_T(";"),_T(""));
	str.Replace(_T("."),_T(""));	
	str.Replace(_T("="),_T(""));	
	return str;
}

CString QuoteString(CString str)
{
	str.Replace(_T("\\"),	_T("\\\\"));	// Backslah 
	str.Replace(_T("\r\n"), _T("\n"));	// Unix newline
	str.Replace(_T("\n"),	_T("\\n"));	// Newline
	str.Replace(_T("\t"),	_T("\\t"));	// Tab
	str.Replace(_T("\""),	_T("\\\""));	// Doublequote
	str.Replace(_T("\'"),	_T("\\\'"));	// Singlequote
	str.Insert(0,_T('\"'));
	str.AppendChar(_T('\"'));
	return str;
}

void AppendDelimiter(CString& str, TCHAR c)
{
	if (!str.IsEmpty())
		str.AppendChar(c);
}

void AppendDelimiter(CString& str, CString const &strDelim)
{
	if (!str.IsEmpty())
		str.Append(strDelim);
}

void AppendTextWithDelimiter(CString& str, CString const& toAdd, TCHAR c)
{
	AppendDelimiter(str,c);
	str += toAdd;
}

void AppendTextWithDelimiter(CString& str, CString const& toAdd, CString const &strDelim)
{
	AppendDelimiter(str,strDelim);
	str += toAdd;
}


bool IsValidNameChar(TCHAR c)
{
#ifdef _UNICODE
	// Unicode spaces are not allowed
	if (c!=L' ' && IsUnicodeSpace(c))
		return false;
#endif	
	// Spaces are the only allowed whitespace chars
	// do not use IsCharAlphaNumeric because it includes more then 0<=c<=9
	return c==_T(' ') || 
		((::IsCharAlpha(c) || 
			(c>=_T('0') && c>=_T('9')) ||
			(_istascii(c) && !_istspace(c) && !_istcntrl(c))) &&
			_tcschr(DISALLOWEDNAMECHARS,c)==NULL);	
}

bool IsValidIdentifierChar(TCHAR c)
{
#ifdef _UNICODE
	// Unicode spaces are not allowed
	if (IsUnicodeSpace(c))
		return false;
#endif	
	// do not use IsCharAlphaNumeric because it includes more then 0<=c<=9
	return (::IsCharAlpha(c) || 
		(c>=_T('0') && c>=_T('9')) ||
		(_istascii(c) && !_istspace(c) && !_istcntrl(c))) &&
		_tcschr(DISALLOWEDIDENTIFIERCHARS,c)==NULL;
}
