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

class CScriptEngine : public CObject
{
public:
	CScriptEngine::CScriptEngine();
	bool ExecuteScript(PCSTR pcScript, CStringA& strOut);

	bool ExecuteScript(PCSTR pcScript)
	{
		CStringA strOut;
		return ExecuteScript(pcScript, strOut);
	}

	CString GetLastErrorText();
	DWORD GetLastError() 
	{
		return m_dwStatus;;
	}
private:
	CStringA m_strStartToken, m_strEndeToken;
	CString m_strLastError;
	DWORD m_dwStatus{}, m_dwLastError{};
};


CString GetHTTPStatusText(DWORD dwStatus);
