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

#include <shlwapi.h>


class CFileVersionInfo : public VS_FIXEDFILEINFO 
{
public:
	CFileVersionInfo();
	virtual ~CFileVersionInfo();

	bool GetFileVersionInfo(HMODULE hModule=NULL);
	bool GetFileVersionInfo(LPCTSTR modulename);
	CString	GetValue(LPCTSTR lpKeyName) const;
	static bool DllGetVersion(LPCTSTR modulename, DLLVERSIONINFO& dvi);

	bool IsValid() const
	{
		return m_pVersionInfo!=NULL;
	}
	CString GetFileVersion() const
	{
		return GetValue(_T("FileVersion"));
	}
	CString GetProductName() const
	{
		return GetValue(_T("ProductName"));
	}
	CString GetProductVersion() const
	{
		return GetValue(_T("ProductVersion"));
	}
	CString GetLegalCopyright() const
	{
		return GetValue(_T("LegalCopyright"));
	}
protected:
	void Clear();

protected:
// Ausgelesene Daten aus der Datei
	BYTE* m_pVersionInfo;	// all version info

// Überstetzung ode Code Page
	struct TRANSLATION 
	{
		WORD langID;			// language ID
		WORD charset;			// character set (code page)
	} 
	m_translation;
};
