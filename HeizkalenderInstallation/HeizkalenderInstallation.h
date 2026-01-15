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

#include"Helper.h"

#ifndef __AFXWIN_H__
	#error "'pch.h' vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"		// Hauptsymbole

#include "Data.h"

// Modis für Skript 1
enum ModeScript1 
{
	Unknown = 0,
	ChurchToolsAPI = 1,
	ChurchDeskAPI = 2,
	ChurchDeskiCal = 3,
};

#define HK1_SKRIPT_1		_T("HK-Skript 1")
#define HK1_RAUMLISTE		_T("HK1-R-Liste")
#define HK2_RAUMLISTE		_T("HK2-HKG-Liste")
#define HKG_RAUM_PREFIX		_T("HKG-Raum-")
#define HK2_GRUNDTEMP		_T("HK2-Grundtemperatur")
#define RAUM_DEFAULT		_T("0;H;IP;20;0;30;Heizgruppe: ")
// CHeizkalenderInstallationApp:
// Siehe HeizkalenderInstallation.cpp für die Implementierung dieser Klasse
//

class CHeizkalenderInstallationApp : public CWinApp
{
public:
	CHeizkalenderInstallationApp();

// Überschreibungen
public:
	virtual BOOL InitInstance();

	// Laden der Daten
	bool ConnectToCCU();
	bool ConnectToSimulation();

	bool AnalyseLoadedData();
	void UpdatePrograms();
	
	// Speichern der Daten
	void FixScript1BeforeUpdate();
	void UpdateSimulation();
	void UpdateCCU();

	void ClearAll();
	bool IsModeScript1Modified();
	bool IsDataModified();
	bool IsSysVarCompatibeWithModeScript1(CString const& strName, ModeScript1 mode);
	bool LoadSystemVariablesFromCCU();
	bool LoadProgramsFromCCU(const CString &strPrefix);
	double GetGrundTemperatur();

	void ReadResources();
	void ReadResourcesChurchTool(CMapRaumListe &mapNewRooms);
	void ReadResourcesChurchDesk(CMapRaumListe &mapNewRooms);
	void LoadRoomMapFromSysVars(CMapRaumListe &mapRaueme);
	void SaveRoomMapToSysVars(CMapRaumListe const& mapRaeume);
	CString GenerateNewRoomName(CString strTemplate);

	CString RemovePrefix(CString const str) const;
	CString AddPrefix(CString const& str) const;
	bool IsMatchingPrefix(CString const& str) const;
	bool IsMatchingSysVar(CString const& str) const;

	CString AddRoomPrefix(CString const& str) const;
	CString RemoveRoomPrefix(CString str) const;
	bool IsMatchingRoomPrefix(CString str) const;

	CString GetNameFromScrip1Mode(ModeScript1 mode);

// Implementierung
	DECLARE_MESSAGE_MAP()

public: 
	// Applikationsdaten
	CString m_strAppPath;
	CString m_strScriptPath;
	CString m_strAppVersion;
	CString m_strScriptVersion;

	// Data Connect
	bool m_bSimulation = {};
	bool m_bConnected = {};
	CString m_strCCU_host;
	int		m_iCCU_port;			// 8181;
	const CString m_strCCU_url = _T("/rega.exe");
	CString m_strCCU_username;
	CString m_strCCU_password;
	CString m_strPrefix;

	// Aktueller mode UI
	ModeScript1 m_modeScript1;

	// Das Skript 1 richtet sich nach dem Modus. deshalb ist das
	// Variabel.
	CString	GetScript1Name()
	{
		return m_modeScript1==ModeScript1::ChurchToolsAPI ? _T("HK-Skript 1_ChurchTools") :
			   m_modeScript1==ModeScript1::ChurchDeskAPI ? _T("HK-Skript 1_ChurchDeskAPI") :
			   m_modeScript1==ModeScript1::ChurchToolsAPI ? _T("HK-Skript 1_ChurchDeskiCal") :
			   _T("");
	}

	// Daten aus der CCU. (Alle Systemvariablen wobei der Prefix egal ist, 
	// sowie die Programme gefilter auf den Prefix). Diese Listen werden auch angepasst
	ModeScript1				m_modeScript1Installed;
	CListSystemVariables	m_lstSysVars;				// Alle Varoablen. Man muss evtl. IsMatchingPrefix nutzen.
	CListPrograms			m_lstPrograms;				// Nur Programme mit passendem Prefix.

	// Default data. Diese Daten enthalten, die Systemvariablen, wie sie durch die Skripte angelegt wurden.
	// Diese zweite Liste dient dem Vergleich, ob die original Daten verwendet wurden.
	//  - HK-Init-Variablen Logging.hsc
	//	- HK-Init-Skript 1_ChurchDesk.hsc
	//  - HK-Init-Skript 1_CurchTools.hsc	
	//  - HK-Init-Skript 2.hsc
	CListSystemVariables	m_lstSysVarsDefault; // Id=0, Keine Prefixe, geladen bei Programmstart

	// Map aller Skripte, die wir nutzen, Code ist bereits Latin. Das sind die Skripte, die im 
	// Programmverzeichnis liegen sollten 
	//		HK-Außentemperatur-Open-Meteo
	//		HK-Heizkurvenkontrolle
	//		HK-Skript 1_ChurchDeskAPI
	//		HK-Skript 1_ChurchDeskiCal
	//		HK-Skript 1_ChurchTools
	//		HK-Skript 2
	//		HK-Systemprotokoll sichern
	COleDateTime	m_dateMaxPrograms;
	CListPrograms   m_lstAppPrograms;		 // Id = 0, Keine Prefixe, geladen bei Programmstart

	//  Liste der erlaubten Programmnamen
	//		HK-Außentemperatur-Open-Meteo
	//		HK-Heizkurvenkontrolle
	//		HK-Skript 1
	//		HK-Skript 2
	//		HK-Systemprotokoll sichern
	std::set<CString,COMPARE_NOCASE> m_setProgramNames;	
};

extern CHeizkalenderInstallationApp theApp;
