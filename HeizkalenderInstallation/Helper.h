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

#pragma once

const int MAX_DATE_LENGTH = 12;			// maximum length is yyyy-mm-dd (there are formats with an additional blank in it)
const int MAX_NUMERIC_LENGTH  = 17;     // double with 12 sig digits has a maximum of 16 digits
const int MAX_SIGFLOAT_DIGITS = 12;     // a maximum of 12 significant Digits (double has 15)
const int DEFAULT_PRECISION	= 6;		// Default precision of the CCU

// This is a real missing typedef. LP should be avoided 
using PCVOID = LPCVOID;

//-----------------------------------------------------------------------------

inline CString CStringRes(UINT uiRes)	
{	
	if (uiRes)
		return CString(MAKEINTRESOURCE(uiRes));		
	else
		return CString{};
}

bool LoadStringFromResource(CStringA &str, LPCTSTR pszName, LPCTSTR pszType, bool bForceUTF8=false);
bool LoadStringFromResource(CStringW &str, LPCTSTR pszName, LPCTSTR pszType, bool bForceUTF8=false);

bool GetFileContent(const CString &strFileName, CString &strData);

//-----------------------------------------------------------------------------

inline CString IntToString(int i)
{             
	TCHAR szText[16] = {};
	VERIFY(_itot_s(i,szText,10)==0);
	return szText;
}

inline int StringToInt(PCTSTR pszText)
{
	return _tcstol(pszText,NULL,10);
}

// If there is a precision value there are special handling for negative values
// 
// -1 means no special number of digits (display maximum precision)
// The flag bUseLocale set to false allows the formating of doubles in a neutral way
// this means the decimal point is the period.
double StringToDouble(PCTSTR pszText);
CString DoubleToString(double dValue, int iPrec);
double RoundDouble(double dVal, int iPrec);

bool StringToBool(CString const &strText);
CString BoolToString(bool bVal);
bool IsBool(CString strText);

//-----------------------------------------------------------------------------

#define DATE_FORMAT_DEU _T("dd.MM.yyyy")
#define DATE_FORMAT_NEU _T("yyyy-MM-dd")

int DateToString(DATE dDate, PTSTR pszBuffer, int iMaxLen, PCTSTR pszFormat=DATE_FORMAT_DEU);
int DateToString(const SYSTEMTIME &sTime, PTSTR pszBuffer, int iMaxLen, PCTSTR pszFormat=DATE_FORMAT_DEU);

CString DateToString(DATE dDate,PCTSTR pszFormat=DATE_FORMAT_DEU);
CString DateToString(const COleDateTime &date,PCTSTR pszFormat=DATE_FORMAT_DEU);
CString DateToString(const SYSTEMTIME &sTime,PCTSTR pszFormat=DATE_FORMAT_DEU);
COleDateTime ParseDate(PCSTR pszStr);
COleDateTime ParseDate(PCWSTR pszStr);

static inline bool IsSystemTimeNull(const SYSTEMTIME &st)
{
	return st.wYear==0 && st.wMonth==0 && st.wDay==0 &&
		   st.wHour==0 && st.wMinute==0 && st.wSecond==0 && st.wMilliseconds==0;
}

inline bool IsDateValid(COleDateTime const &date)
{
	return date.GetStatus()==COleDateTime::valid && date.m_dt>0;
}

inline bool IsDateValid(DATE dt)
{
	return dt>0;
}

//-----------------------------------------------------------------------------

struct COMPARE_NOCASE
{
	bool operator() (PCTSTR l,PCTSTR r) const 
	{ 
		return _tcsicmp(l,r)<0; 
	}
};

struct COMPARE_LOGICAL
{
	bool operator() (PCTSTR l,PCTSTR r) const 
	{ 
		// Nur wenn es ungleich ist verwenden wir den StrCmpLogicalW
		// Weil StrCmpLogicalW ein caseless test ist.
		if (_tcsicmp(l,r)==0)
			return _tcscmp(l,r)<0;
		else			
			return ::StrCmpLogicalW(l,r)<0;
	}
};

CStringW UTF8toUnicode(const char * utf8, int utf8Length = -1);
CStringW ISO8859_1_toUnicode(const char* isostr, int isostrLength=-1);

CString StrValueByIndex(PCTSTR pszFullString, int iSubString, TCHAR chSep=_T('\t'));
int NumberOfStrValues(PCTSTR pszFullString, TCHAR chSep=_T('\t'));

//-----------------------------------------------------------------------------

// Some static character testing routines
bool IsUnicodeSpace(wchar_t c);

#define UNERLAUBTE_ZEICHEN_FUER_RAEUME	_T(" ;+=\'\"#.")
CString CleanupNameForRoom(CString str);

//-----------------------------------------------------------------------------

CString QuoteString(CString str);
void AppendDelimiter(CString &str, TCHAR c=_T(';'));
void AppendDelimiter(CString &str, CString const &strDelim);
void AppendTextWithDelimiter(CString &str, CString const &toAdd, TCHAR c=_T(';'));
void AppendTextWithDelimiter(CString &str, CString const &toAdd, CString const &strDelim);
