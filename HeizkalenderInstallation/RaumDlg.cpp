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
#include "RaumDlg.h"


// CRaumDlg dialog

IMPLEMENT_DYNAMIC(CRaumDlg, CDialogEx)

CRaumDlg::CRaumDlg(bool bModify, SRaumDaten &raum, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ROOM, pParent)
	, m_bModify{ bModify } 
	, m_raum{ raum }
{

}

CRaumDlg::~CRaumDlg()
{
}

void CRaumDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ED_NAME, m_edName);
	DDX_Control(pDX, IDC_ED_TEMP, m_edTemp);
	DDX_Control(pDX, IDC_ED_GRUNDTEMP, m_edTempG);
	DDX_Control(pDX, IDC_ED_VORZEIT_AN, m_edVBegin);
	DDX_Control(pDX, IDC_ED_VORZEIT_AUS, m_edVEnde);
	DDX_Control(pDX, IDC_ED_FAKTOR, m_edFaktor);
	DDX_Control(pDX, IDC_CB_AKTOR, m_cbAktor);

	if (pDX->m_bSaveAndValidate)
	{
		CString strVal = _T("H");
		int nSel = m_cbMode.GetCurSel();
		if (nSel>=0)
		{
			// Ersten Buchstaben nehmen
			strVal = TCHAR(m_cbMode.GetItemData(nSel));
		}
		m_raum.m_strMode = strVal;

		strVal = _T("IP");
		nSel = m_cbDevTyp.GetCurSel();
		if (nSel>=0)
		{
			// Ersten Buchstaben nehmen
			auto dwMode = m_cbDevTyp.GetItemData(nSel);
			strVal = CString{ TCHAR(LOWORD(dwMode)) } + TCHAR(HIWORD(dwMode));
		}
		m_raum.m_strDevTyp = strVal;
	}
	else if (m_cbMode.GetSafeHwnd())
	{
		int iMode = m_raum.m_strMode!=_T("H") ? _T('S') : _T('H');
		m_cbMode.SetCurSel(0);
		for (int i=0, n=m_cbMode.GetCount(); i<n; ++i)
		{
			if (m_cbMode.GetItemData(i)==iMode)
			{
				m_cbMode.SetCurSel(n);
				break;
			}
		}
		CString str = m_raum.m_strDevTyp;
		DWORD dwMode = MAKELPARAM(str[0],str[1]);
		m_cbDevTyp.SetCurSel(0);
		for (int i=0, n=m_cbDevTyp.GetCount(); i<n; ++i)
		{
			if (m_cbDevTyp.GetItemData(i)==dwMode)
			{
				m_cbDevTyp.SetCurSel(n);
				break;
			}
		}
	}
	DDX_Control(pDX, IDC_CB_MODE, m_cbMode);
	DDX_Control(pDX, IDC_CB_GERAETETYP, m_cbDevTyp);

	DDX_Text(pDX,IDC_ED_NAME, m_raum.m_strName);
	DDX_EditDouble(pDX,IDC_ED_TEMP, m_raum.m_dblTemp);
	DDX_EditDouble(pDX,IDC_ED_GRUNDTEMP, m_raum.m_dblTempG);
	DDX_EditInt(pDX,IDC_ED_VORZEIT_AN, m_raum.m_iVBegin);
	DDX_EditInt(pDX,IDC_ED_VORZEIT_AUS, m_raum.m_iVEnde);
	DDX_EditDouble(pDX,IDC_ED_FAKTOR, m_raum.m_dblFaktor);
	DDX_Text(pDX,IDC_CB_AKTOR,m_raum.m_strAktor);
}


BEGIN_MESSAGE_MAP(CRaumDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BT_TEST, &CRaumDlg::OnBnClickedBtTest)
	ON_CBN_SELCHANGE(IDC_CB_MODE, &CRaumDlg::OnCbnSelchangeCbMode)
END_MESSAGE_MAP()


// CRaumDlg message handlers

BOOL CRaumDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Alten Namen merken
	m_strNameAlt = m_raum.m_strName;

	// Titel setzen
	CString strTitle;
	GetWindowText(strTitle);
	SetWindowText(strTitle + m_raum.m_strName);

	CString strMode{ CStringRes(IDS_RAUM_MODUS) }, str;
	for (int i=0; !(str=StrValueByIndex(strMode,i,_T(';'))).IsEmpty(); ++i)
	{
		int n = m_cbMode.AddString(str);
		if (n>=0)
			// Ersten Buchtstaben nehmen
			m_cbMode.SetItemData(n,str[0]);
	}
	// IP- Thermostate-Aktoren-Gerätetyp (Kanal 1)
	// RT- Kennung Kanal Klassik-Thermostate-Aktoren-Gerätetyp (Kanal 4)
	// TC- Kennung Kanal Klassik-Thermostate-Aktoren-Gerätetyp (Kanal 4)
	// IT- Kennung Kanal Klassik-Thermostate-Aktoren-Gerätetyp (Kanal 4)
	// SW- Kennung Kanal Klassik-Schalter-Aktoren-Gerätetyp (Kanal 1 bzw. 2)
	//		HM-LC-Sw1-FM HM-LC-Sw1PBU-FM, HM-LC-Sw2-FM, HM-ES-PMSw1-DR HM-LC-Sw1-PCB
	//		Kennung Kanal IP-Schalter-Aktoren-Gerätetyp (Kanal 1)
	//		SW  Noch unerprobt
	CString strDevTyp{ CStringRes(IDS_RAUM_DEVTYP) };
	for (int i = 0; !(str = StrValueByIndex(strDevTyp, i, _T(';'))).IsEmpty(); ++i)
	{
		int n = m_cbDevTyp.AddString(str);
		if (n>=0)
		{
			DWORD dwMode = MAKELPARAM(str[0],str[1]);
			m_cbDevTyp.SetItemData(n,dwMode);
		}
	}	

	m_edName.LimitText(25);
	m_edTemp.SetMinMax(0,30);
	m_edTemp.SetPrecision(1);
	m_edTemp.CreateSpinBtnCtrl();
	m_edTempG.SetMinMax(0,30);
	m_edTempG.SetPrecision(1);
	m_edTempG.CreateSpinBtnCtrl();
	m_edVBegin.SetMinMax(0,60*24);
	m_edVBegin.CreateSpinBtnCtrl();
	m_edVEnde.SetMinMax(0,60*24);
	m_edVEnde.CreateSpinBtnCtrl();
	m_edFaktor.SetMinMax(0.25,3.0);
	m_edFaktor.SetPrecision(2);
	m_edFaktor.CreateSpinBtnCtrl();
	m_edFaktor.SetStepValue(0.25);
	
	// Endgültige Daten laden
	UpdateData(FALSE);
	OnCbnSelchangeCbMode();
	return TRUE;  
}

void CRaumDlg::OnBnClickedBtTest()
{
	AfxMessageBox(_T("ToDo: Testen des Aktors muss noch eingebaut werden!"));
}


void CRaumDlg::OnCbnSelchangeCbMode()
{
	int nSel = m_cbMode.GetCurSel();
	if (nSel<0)
		return;

	bool bSchalten = m_cbMode.GetItemData(nSel)==TCHAR('S');
	m_edTemp.EnableWindow(!bSchalten);
	m_edTempG.EnableWindow(!bSchalten);
	m_edFaktor.EnableWindow(!bSchalten);
}

void CRaumDlg::OnOK()
{
	// Daten laden
	UpdateData(TRUE);

	// Ein Name muss angegeben werden
	if (m_raum.m_strName.IsEmpty())
	{
		AfxMessageBox(IDP_RAUM_NAME_FEHLT);
		return;
	}

	// Prüfen ob der Name geändert wurde und ob wir ein Duplikat haben
	if (!m_bModify || (m_bModify && m_strNameAlt!=m_raum.m_strName))
	{
		// Prüfe ob der Name exisitert 
		if (theApp.m_lstSysVars.Find(theApp.AddRoomPrefix(m_raum.m_strName)))
		{
			CString strMsg;
			strMsg.FormatMessage(IDP_RAUM_NAME_EXISTIERT, m_raum.m_strName);
			AfxMessageBox(strMsg);
			return;
		}
	}

	CDialogEx::OnOK();
}
