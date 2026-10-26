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
#include "HeizkalenderInstallation.h"
#include "WGetEngine.h"
#include "Helper.h"

CWGetEngine::CWGetEngine()
    : m_dwLastError{0}
{
}

bool CWGetEngine::Get(CString strURL, CStringW& strOut, bool bUtf8)
{
	m_strLastError.Empty();
	m_dwLastError = m_dwStatus = 0;

    try
    {
        CWaitCursor wait;
        CInternetSession session { _T("HeizkalenderInstallation/1.0"),
            INTERNET_OPEN_TYPE_PRECONFIG
        };
        session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 10000);
        session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 10000);


        // Falls HTTPS-Zertifikate Probleme machen:
        DWORD flags =
            INTERNET_FLAG_RELOAD |
            INTERNET_FLAG_NO_CACHE_WRITE |
            INTERNET_FLAG_SECURE |
            INTERNET_FLAG_TRANSFER_ASCII;

        std::unique_ptr<CInternetFile> pFile
        { 
            static_cast<CInternetFile*>(session.OpenURL(strURL, 1, flags))
        };
        
        // HTTP-Status prüfen, bevor der Body gelesen wird
        m_dwStatus = HTTP_STATUS_OK;
        DWORD_PTR dwConnContext = 0;

        // Es besteht die Möglichkeit, dass wir keine HTTP Connection haben
        auto* pHttpFile = DYNAMIC_DOWNCAST(CHttpFile, pFile.get());
        if (pHttpFile)
        {
			// COntext sichern , damit er auch nach dem Freigeben von pHttpFile fuer AfxThrowInternetException verfuegbar ist.
            dwConnContext = pHttpFile->GetContext();
            pHttpFile->QueryInfoStatusCode(m_dwStatus);
        }

        // Read all data we got
        CStringA strTemp, str;
        int iSize;
        while ((iSize = pFile->Read(strTemp.GetBuffer(1024), 1024)))
        {
            strTemp.ReleaseBufferSetLength(iSize);
            str += strTemp;
        }
        
        if (bUtf8)
            strOut = UTF8toUnicode(str);
        else
            strOut = str;

        pFile->Close();
        pFile = nullptr;

		// Bei erkennbarem HTTP-Fehlercode werfen wir eine Exception, damit der Aufrufer den Fehler abfangen kann.
        if (m_dwStatus != HTTP_STATUS_OK)
        {
            AfxThrowInternetException(dwConnContext);
        }

        return true;
    }
    catch (CInternetException* e)
    {
        m_dwLastError = e->m_dwError;
        e->GetErrorMessage(CStrBuf(m_strLastError,512), 512);
        e->Delete();
        return false;
    }
}

extern CString GetHTTPStatusText(DWORD dwStatus);

CString CWGetEngine::GetLastErrorText()
{
    if (m_dwLastError==0)
        m_strLastError = GetHTTPStatusText(m_dwStatus);
    return m_strLastError;
}
