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
#include "Data.h"

void CListSystemVariables::LoadFromString(CStringA const &strOut)
{
	for (int iStart = 0; iStart<strOut.GetLength(); )
	{
		int iPos = strOut.Find('\n', iStart);
		if (iPos<0)
			iPos = strOut.GetLength();
		CString strLine{ strOut.Mid(iStart,iPos-iStart) };

		// Werte übernehmen
		CDataSystemVariable data{ strLine };
		iStart = iPos+1;
		push_back(data);
	}
}


void CListPrograms::LoadFromString(CStringA const &strOut)
{
	for (int iStart = 0; iStart<strOut.GetLength(); )
	{
		int iPos = strOut.Find('\n', iStart);
		if (iPos<0)
			iPos = strOut.GetLength();
		CString strLine{ strOut.Mid(iStart,iPos-iStart) };

		// Werte übernehmen
		CDataProgram data{ strLine };
		iStart = iPos+1;
		push_back(data);
	}
}

//-----------------------------------------------------------------------------

CDataSystemVariable::CDataSystemVariable(
	int id,
	CString const& strName,
	CString const& strContent,
	CString const& strDescription,
	DataType dt,
	bool bProtocoll,
	bool bVisible
)	
	: m_id{ id }
	, m_strName{ strName }
	, m_strContent{ strContent }
	, m_strDescription{ strDescription }
	, m_dataType{ dt }
	, m_bProtocoll{ bProtocoll }
	, m_bVisible{ bVisible }
{
	m_strContentOld = m_strContent;
}

CDataSystemVariable::CDataSystemVariable(CString const& strLine) : 
	// Id, Name, Inhalt, Beschreibung, Type, Protokolliert, Sichtbar
	CDataSystemVariable(
		StringToInt(StrValueByIndex(strLine, 0)),
		StrValueByIndex(strLine, 1),
		StrValueByIndex(strLine, 2),
		StrValueByIndex(strLine, 3),
		static_cast<DataType>(StringToInt(StrValueByIndex(strLine, 4))),
		StringToBool(StrValueByIndex(strLine, 5)),
		StringToBool(StrValueByIndex(strLine, 6))
	)
{
}

void CDataSystemVariable::SetContent(CString const& str)
{
	switch (m_dataType)
	{
	default:
		ASSERT(FALSE);
	case DataType::vtString:
		m_strContent = str;
		break;
	case DataType::vtboolean:
		m_strContent = BoolToString(StringToBool(str));
		break;
	case DataType::vtInteger:
		m_strContent = DoubleToString(StringToDouble(str), DEFAULT_PRECISION);
		break;
	}
	m_bModified = m_strContent!=m_strContentOld;
}

CString CDataSystemVariable::GetDataAsLine()
{
	// Id, Name, Beschreibung, Aktiv, Zeile1, Stand
	return
		IntToString(m_id) + _T('\t') +
		m_strName + _T('\t') +
		m_strContent + _T('\t') +
		m_strDescription + _T('\t') +
		IntToString(static_cast<int>(m_dataType)) + _T('\t') +
		BoolToString(m_bProtocoll) + _T('\t') +
		BoolToString(m_bVisible);
}

void CDataSystemVariable::InitNeuerRaum(CString const& strName)
{
	// Alles löschen
	*this = CDataSystemVariable{};
	ASSERT(theApp.IsMatchingRoomPrefix(strName));
	m_strName = strName;
	m_strContent = RAUM_DEFAULT + theApp.RemoveRoomPrefix(strName);
	m_dataType = DataType::vtString;
	m_bNew = true;
	m_bProtocoll = m_bVisible = true;
}

//-----------------------------------------------------------------------------

CDataProgram::CDataProgram(CString const& strLine) 
	: CDataProgram(
		// Id, Name, Beschreibung, Aktiv, Zeile1, Stand
		StringToInt(StrValueByIndex(strLine, 0)),
		StrValueByIndex(strLine, 1),
		StrValueByIndex(strLine, 2)
	)
{
	m_bActive = StringToBool(StrValueByIndex(strLine, 3));
	m_strLine1 = StrValueByIndex(strLine, 4);
	m_date = ParseDate(StrValueByIndex(strLine, 5));
}


CString CDataProgram::GetDataAsLine()
{
	return
		IntToString(m_id) + _T('\t') +
		m_strName + _T('\t') +
		m_strDescription + _T('\t') +
		BoolToString(m_bActive) + _T('\t') +
		m_strLine1 + _T('\t') +
		DateToString(m_date, DATE_FORMAT_DEU);
}

//-----------------------------------------------------------------------------

void SRaumDaten::LoadFromString(CString const &strName, CString const &str)
{
	m_strName = strName;
	m_strMode = StrValueByIndex(str,1,_T(';'));
	m_strDevTyp = StrValueByIndex(str,2,_T(';'));
	m_dblTemp = StringToDouble(StrValueByIndex(StrValueByIndex(str,3,_T(';')),0,_T('/')));
	m_dblTempG = StringToDouble(StrValueByIndex(StrValueByIndex(str,3,_T(';')),1,_T('/')));
	if (m_dblTempG==0)
		m_dblTempG = theApp.GetGrundTemperatur();
	m_iVBegin = StringToInt(StrValueByIndex(StrValueByIndex(str,4,_T(';')),0,_T('*')));
	m_dblFaktor = StringToDouble(StrValueByIndex(StrValueByIndex(str,4,_T(';')),1,_T('*')));
	m_iVEnde = StringToInt(StrValueByIndex(str,5,_T(';')));
	if (m_dblFaktor==0)
		m_dblFaktor = 1.0;

	// Nun müssen wir den 6ten Eintrag schen
	int iPos = 0;
	for (int i=0; i<6; ++i)
	{
		iPos = str.Find(_T(';'),iPos);
		if (iPos==-1)
			iPos= str.GetLength();
		else
			iPos=iPos+1;
	}
	m_strAktor = str.Mid(iPos);;
}

void SRaumDaten::GetAsString(CString &strName, CString &str)
{
	strName = m_strName;
	str = _T("0;") +
		m_strMode + _T(";") +
		m_strDevTyp + _T(";") +
		DoubleToString(m_dblTemp,-1);
	if (m_dblTempG!=theApp.GetGrundTemperatur())
		str += _T("/") + DoubleToString(m_dblTemp,-1);
	str += _T(";") +
		IntToString(m_iVBegin);
	if (m_dblFaktor!=1.0)
		str += _T("*") + DoubleToString(m_dblFaktor,-1);
	str += _T(";") +
		IntToString(m_iVEnde) + _T(";") +
		m_strAktor;
	
	// Nun müssen wir den 6ten Eintrag schen
	int iPos = 0;
	for (int i=0; i<6; ++i)
	{
		iPos = str.Find(_T(';'),iPos);
		if (iPos==-1)
			iPos= str.GetLength();
		else
			iPos=iPos+1;
	}
	m_strAktor = str.Mid(iPos);;
}
