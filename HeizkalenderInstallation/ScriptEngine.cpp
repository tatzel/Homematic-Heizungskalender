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
#include "ScriptEngine.h"
#include "Helper.h"

CScriptEngine::CScriptEngine()
    : m_dwStatus{0}
{
    // Wir bauen das Skript mit einem start und Endbefehl auf, der eindeitig ist
    // Dazu nehmen wir eine GUID für diese Session:
    GUID guid { };
    ::CoCreateGuid(&guid);
    CStringW strGuid;      
    ::StringFromGUID2(guid, strGuid.GetBuffer(64), 64);
    m_strStartToken.Format("<<Start:%s>>\n", CStringA{ strGuid }.GetString());
    m_strEndeToken.Format("<<Ende:%s>>\n", CStringA{ strGuid }.GetString());
}

bool CScriptEngine::ExecuteScript(PCSTR pcScript, CStringA& strOut)
{
    try
    {
        CInternetSession session(_T("HeizkalenderInstallation/1.0"));
        session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 5000);
        session.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, 5000);

        std::unique_ptr<CHttpConnection> pConn
        {
            session.GetHttpConnection(
                theApp.m_strCCU_host,
                0,
                theApp.m_iCCU_port,
                !theApp.m_strCCU_username.IsEmpty() || !theApp.m_strCCU_password.IsEmpty() ? theApp.m_strCCU_username.GetString() : nullptr,
                !theApp.m_strCCU_username.IsEmpty() || !theApp.m_strCCU_password.IsEmpty() ? theApp.m_strCCU_password.GetString() : nullptr
            )
        };

        std::unique_ptr<CHttpFile> pFile
        { 
            pConn->OpenRequest(
                CHttpConnection::HTTP_VERB_POST,
                theApp.m_strCCU_url,
                NULL,
                1,
                NULL,
                NULL,
                INTERNET_FLAG_NO_CACHE_WRITE
            ) 
        };

        // Set headers
        CString headers = _T("Content-Type: text/xml\r\n");

        // Convert to ANSI
        // Wir bauen am Start und am Ende eine Testsugabe um die Ausführung zu konntrollieren.
        CStringA strScript{pcScript}, strToken;
        strToken.Format(R"x(Write("%s");)x" "\n", m_strStartToken.GetString());
        strScript.Insert(0,strToken);
        strToken.Format(R"x(Write("%s");)x" "\n", m_strEndeToken.GetString());
        strScript.Append(strToken);

        pFile->SendRequest(headers,strScript.GetBuffer(0),strScript.GetLength());

        // Fehler Code ermitteln.
        pFile->QueryInfoStatusCode(m_dwStatus);

        // Read all data we got
        CStringA strTemp;
        int iSize;
        while ((iSize = pFile->Read(strTemp.GetBuffer(1024), 1024)))
        {
            strTemp.ReleaseBufferSetLength(iSize);
            strOut += strTemp;
        }

        // Resultstring is ISO8859_1 
        // auto strTest = ISO8859_1_toUnicode(strOut.GetString());

        // XML Part abschneiden.
        auto iPos = strOut.Find("<xml><exec>/rega.exe</exec><sessionId></sessionId><httpUserAgent>User-Agent:");
        if (iPos>=0)
        {
            // XML Part abschneiden
            strOut.GetBufferSetLength(iPos);

            // Ausgabe \r\n tauschen in \n
            strOut.Replace("\r\n","\n");

            // Start und Ende müssen passen
            if (strOut.Mid(0, m_strStartToken.GetLength())==m_strStartToken &&
                strOut.Mid(strOut.GetLength()-m_strEndeToken.GetLength())==m_strEndeToken)
            {
                // Ergebnis passt. Beide Tokens sind drin.DIese löschen wir nun.
                strOut.Delete(0,m_strStartToken.GetLength());
                strOut.Delete(strOut.GetLength()-m_strEndeToken.GetLength(),m_strStartToken.GetLength());
            } 
            else
            {
                m_dwStatus = HTTP_STATUS_BAD_REQUEST;
            }
        }
        else
        {
            // Skript error. Wir haben keinen Fehler aber der XML Block ist nicht da.
            m_dwStatus = HTTP_STATUS_BAD_REQUEST;
        }

        pFile->Close();
        pFile = nullptr;
        pConn->Close();
        pConn = nullptr;

        return (m_dwStatus == HTTP_STATUS_OK);
    }
    catch (CInternetException* e)
    {
        e->Delete();
        return false;
    }
}

CString CScriptEngine::GetLastErrorText()
{
    CString result;
    result.Format(_T("HTTP_STATUS=%d"),m_dwStatus);
    return result.Trim();
}
