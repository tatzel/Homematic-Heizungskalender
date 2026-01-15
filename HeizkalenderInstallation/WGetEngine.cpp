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

bool CWGetEngine::Get(CString strURL, CStringW& strOut)
{
    try
    {
        CInternetSession session { _T("HeizkalenderInstallation/1.0"),
            INTERNET_OPEN_TYPE_PRECONFIG
        };

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
        
        // Read all data we got
        CStringA strTemp, str;
        int iSize;
        while ((iSize = pFile->Read(strTemp.GetBuffer(1024), 1024)))
        {
            strTemp.ReleaseBufferSetLength(iSize);
            str += strTemp;
        }
        strOut = str;

        pFile->Close();
        pFile = nullptr;

        return true;
    }
    catch (CInternetException* e)
    {
        e->Delete();
        m_dwLastError = ::GetLastError();
        return false;
    }
}

CString CWGetEngine::GetLastErrorText()
{
    CString result;
    result.Format(_T("Last Errror=%d"),m_dwLastError);
    return result.Trim();
}
