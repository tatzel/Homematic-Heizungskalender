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
	m_strLastError.Empty();
	m_dwStatus = 0;

    try
    {
	    CWaitCursor wait;
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
				if (m_dwStatus == HTTP_STATUS_OK)
                    m_dwStatus = HTTP_STATUS_BAD_REQUEST;
                AfxThrowInternetException(pConn->GetContext());
            }
        }
        else
        {
            // Skript error. Wir haben keinen Fehler aber der XML Block ist nicht da.
            if (m_dwStatus == HTTP_STATUS_OK)
                m_dwStatus = HTTP_STATUS_BAD_REQUEST;
            AfxThrowInternetException(pConn->GetContext());
        }

        pFile->Close();
        pFile = nullptr;
        pConn->Close();
        pConn = nullptr;

        if (m_dwStatus == HTTP_STATUS_OK)
            return true;
        else
        {
            AfxThrowInternetException(pConn->GetContext());
            return false;
        }
    }
    catch (CInternetException* e)
    {
		m_dwLastError = e->m_dwError;
		e->GetErrorMessage(CStrBuf(m_strLastError,512), 512);
        e->Delete();
        return false;
    }
}

CString CScriptEngine::GetLastErrorText()
{
    if (m_dwLastError==0)
        m_strLastError = GetHTTPStatusText(m_dwStatus);
    return m_strLastError;
}

//------------------------------------------------------------------------


#define DECL_ELEMENT(x, y)	{x, _T(#x) y}

static struct {
    DWORD   dwSTatus;
    PCTSTR  pText;
} const aHTTPStatus[] = 
{
    DECL_ELEMENT(HTTP_STATUS_CONTINUE            ,  _T(" (100 = OK to continue with request)")),
    DECL_ELEMENT(HTTP_STATUS_SWITCH_PROTOCOLS    ,  _T(" (101 = server has switched protocols in upgrade header)")),
    DECL_ELEMENT(HTTP_STATUS_OK                  ,  _T(" (200 = request completed)")  ),
    DECL_ELEMENT(HTTP_STATUS_CREATED             ,  _T(" (201 = object created, reason = new URI)")   ),
    DECL_ELEMENT(HTTP_STATUS_ACCEPTED            ,  _T(" (202 = async completion (TBS))") ),
    DECL_ELEMENT(HTTP_STATUS_PARTIAL             ,  _T(" (203 = partial completion)") ),
    DECL_ELEMENT(HTTP_STATUS_NO_CONTENT          ,  _T(" (204 = no info to return)")  ),
    DECL_ELEMENT(HTTP_STATUS_RESET_CONTENT       ,  _T(" (205 = request completed, but clear form)")  ),
    DECL_ELEMENT(HTTP_STATUS_PARTIAL_CONTENT     ,  _T(" (206 = partial GET furfilled)")  ),
    DECL_ELEMENT(HTTP_STATUS_AMBIGUOUS           ,  _T(" (300 = server couldn't decide what to return)")  ),
    DECL_ELEMENT(HTTP_STATUS_MOVED               ,  _T(" (301 = object permanently moved)")   ),
    DECL_ELEMENT(HTTP_STATUS_REDIRECT            ,  _T(" (302 = object temporarily moved)")   ),
    DECL_ELEMENT(HTTP_STATUS_REDIRECT_METHOD     ,  _T(" (303 = redirection w/ new access method)")   ),
    DECL_ELEMENT(HTTP_STATUS_NOT_MODIFIED        ,  _T(" (304 = if-modified-since was not modified)") ),
    DECL_ELEMENT(HTTP_STATUS_USE_PROXY           ,  _T(" (305 = redirection to proxy, location header specifies proxy to use)")   ),
    DECL_ELEMENT(HTTP_STATUS_REDIRECT_KEEP_VERB  ,  _T(" (307 = HTTP/1.1: keep same verb)")   ),
    DECL_ELEMENT(HTTP_STATUS_PERMANENT_REDIRECT  ,  _T(" (308 = Object permanently moved keep verb)") ),
    DECL_ELEMENT(HTTP_STATUS_BAD_REQUEST         ,  _T(" (400 = invalid syntax)") ),
    DECL_ELEMENT(HTTP_STATUS_DENIED              ,  _T(" (401 = access denied)")  ),
    DECL_ELEMENT(HTTP_STATUS_PAYMENT_REQ         ,  _T(" (402 = payment required)")   ),
    DECL_ELEMENT(HTTP_STATUS_FORBIDDEN           ,  _T(" (403 = request forbidden)")  ),
    DECL_ELEMENT(HTTP_STATUS_NOT_FOUND           ,  _T(" (404 = object not found)")   ),
    DECL_ELEMENT(HTTP_STATUS_BAD_METHOD          ,  _T(" (405 = method is not allowed)")  ),
    DECL_ELEMENT(HTTP_STATUS_NONE_ACCEPTABLE     ,  _T(" (406 = no response acceptable to client found)") ),
    DECL_ELEMENT(HTTP_STATUS_PROXY_AUTH_REQ      ,  _T(" (407 = proxy authentication required)")  ),
    DECL_ELEMENT(HTTP_STATUS_REQUEST_TIMEOUT     ,  _T(" (408 = server timed out waiting for request)")   ),
    DECL_ELEMENT(HTTP_STATUS_CONFLICT            ,  _T(" (409 = user should resubmit with more info)")),
    DECL_ELEMENT(HTTP_STATUS_GONE                ,  _T(" (410 = the resource is no longer available)")),
    DECL_ELEMENT(HTTP_STATUS_LENGTH_REQUIRED     ,  _T(" (411 = the server refused to accept request w/o a length)")  ),
    DECL_ELEMENT(HTTP_STATUS_PRECOND_FAILED      ,  _T(" (412 = precondition given in request failed)")   ),
    DECL_ELEMENT(HTTP_STATUS_REQUEST_TOO_LARGE   ,  _T(" (413 = request entity was too large)")   ),
    DECL_ELEMENT(HTTP_STATUS_URI_TOO_LONG        ,  _T(" (414 = request URI too long)")   ),
    DECL_ELEMENT(HTTP_STATUS_UNSUPPORTED_MEDIA   ,  _T(" (415 = unsupported media type)") ),
    DECL_ELEMENT(HTTP_STATUS_MISDIRECTED_REQUEST ,  _T(" (421 = misdirected request)")),
    DECL_ELEMENT(HTTP_STATUS_RETRY_WITH          ,  _T(" (449 = retry after doing the appropriate action.)")  ),
    DECL_ELEMENT(HTTP_STATUS_SERVER_ERROR        ,  _T(" (500 = internal server error)")  ),
    DECL_ELEMENT(HTTP_STATUS_NOT_SUPPORTED       ,  _T(" (501 = required not supported)") ),
    DECL_ELEMENT(HTTP_STATUS_BAD_GATEWAY         ,  _T(" (502 = error response received from gateway)")   ),
    DECL_ELEMENT(HTTP_STATUS_SERVICE_UNAVAIL     ,  _T(" (503 = temporarily overloaded)") ),
    DECL_ELEMENT(HTTP_STATUS_GATEWAY_TIMEOUT     ,  _T(" (504 = timed out waiting for gateway)")  ),
    DECL_ELEMENT(HTTP_STATUS_VERSION_NOT_SUP     ,  _T(" (505 = HTTP version not supported)") ),
    0,
};

CString GetHTTPStatusText(DWORD dwStatus)
{
    for (auto const &e : aHTTPStatus)
    {
        if (e.dwSTatus==dwStatus)
            return CString(e.pText);
	}
    return CString();
}
