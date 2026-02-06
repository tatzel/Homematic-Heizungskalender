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
	DDX_Control(pDX, IDC_CB_AKTOR, m_cbChannel);

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
				m_cbMode.SetCurSel(i);
				break;
			}
		}
		OnCbnSelchangeCbMode();

		int nSel = m_cbChannel.FindStringExact(-1,m_raum.m_strAktor);
		if (nSel>=0)
			m_cbChannel.SetCurSel(nSel);
		OnCbnSelchangeCbChannel();

		CString str = m_raum.m_strDevTyp;
		DWORD dwMode = MAKELPARAM(str[0],str[1]);
		m_cbDevTyp.SetCurSel(0);
		for (int i = 0, n = m_cbDevTyp.GetCount(); i<n; ++i)
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
	ON_CBN_SELCHANGE(IDC_CB_AKTOR, &CRaumDlg::OnCbnSelchangeCbChannel)
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
	for (auto str : SplitString(strMode, _T(';')))
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
	for (auto str : SplitString(strDevTyp,_T(';')))
	{
		int n = m_cbDevTyp.AddString(str);
		if (n>=0)
		{
			DWORD dwMode = MAKELPARAM(str[0],str[1]);
			m_cbDevTyp.SetItemData(n,dwMode);
		}
	}	

	m_edName.LimitText(64);
	m_edName.SetDisallowedCharList(UNERLAUBTE_ZEICHEN_FUER_RAEUME);	
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
	// Schalten Testen können wir noch nicht
	int nSel = m_cbMode.GetCurSel();
	if (nSel==-1)
		return;
	bool bModeHeizen = static_cast<TCHAR>(m_cbMode.GetItemData(nSel))==_T('H');

	// Es kann sein, dass der Typ manuell eingegeben wurde.
	CString strChannel;
	CString strParam;

	// Bestimme den gewählten Aktor
	nSel = m_cbChannel.GetCurSel();
	if (nSel<0)
	{
		m_cbChannel.GetWindowText(strChannel);
	}
	else
	{
		m_cbChannel.GetLBText(nSel,strChannel);
	}

	// Es können mehrere Aktoren durch Semikolon getrennt werden
	CString strAllMessages;
	CString strCh;
	for (auto strCh : SplitString(strChannel, _T(';')))
	{
		// Suche den Channel
		auto const* pDev = theApp.m_lstDevices.Find(strCh);
		if (pDev)
		{
			// Wir kennen nun den Device. Holle den Datapoint und dort den hinteren Part
			CString strDataPoint{ pDev->m_strDataPoint };
			strParam = StrValueByIndex(strDataPoint, 2, _T('.'));
		}
		else 
			strParam = _T("STATE");
	
		CStringA strTestSkript = 
		R"x(
var ch = dom.GetObject("%1%");
if (!ch){
  WriteLine("Kanal wurde nicht gefunden");
  WriteLine("");
} else {
!// Aktueler Wert der Einstellung 
  var dp = ch.DPByHssDP("%2%");
  if (!dp) {
    WriteLine("Datenpunkt/Parameter \"%2%\" wurde nicht gefunden");
  } else {
	WriteLine(dp.State());
  }
  if ("%3%"!=""){
    var dp = ch.DPByHssDP("%3%");
	if (!dp) {
	  WriteLine("Datenpunkt/Parameter \"%3%\" wurde nicht gefunden");
	} else {
	  WriteLine(dp.State());
	}
  }
}
)x";

		strTestSkript.Replace("%1%",CStringA{strCh});
		strTestSkript.Replace("%2%",CStringA{strParam});
		strTestSkript.Replace("%3%", bModeHeizen ? "ACTUAL_TEMPERATURE" : "");

		CStringA strOut;
		CScriptEngine engine;
		if (engine.ExecuteScript(strTestSkript, strOut))
		{
			CString strMsg;
			CString strOutW{ strOut};
			CString strResult1{ StrValueByIndex(strOutW,0,_T('\n')) },
					strResult2{ StrValueByIndex(strOutW,1,_T('\n')) };
			if (!bModeHeizen)
			{
				if (IsBool(strResult1))
					strResult1 = StrValueByIndex(
						CStringRes(IDS_AN_AUS),
						StringToBool(StrValueByIndex(strOutW,1,_T('\n'))),
						_T(';')
					);
			}
			strMsg.FormatMessage(bModeHeizen ? IDP_TEST_AKTOR_HEIZEN : IDP_TEST_AKTOR_SCHALTEN,
				strCh.GetString(), 
				strResult1, strResult2
			);
			AppendTextWithDelimiter(strAllMessages,strMsg,_T("\n\n"));
		}
		else
		{
			AfxMessageBox(engine.GetLastErrorText(),MB_OK|MB_ICONERROR);
		}
	}
	AfxMessageBox(strAllMessages);
}


void CRaumDlg::OnCbnSelchangeCbMode()
{
	int nSel = m_cbMode.GetCurSel();
	if (nSel<0)
		return;
	bool bModusHeizen = m_cbMode.GetItemData(nSel)==_T('H');

	// Alten Inhalt sichern
	CString strChannel;
	m_cbChannel.GetWindowText(strChannel);

	// Devices laden
	m_cbChannel.ResetContent();
	auto const &lst = theApp.m_lstDevices;
	for (auto const& e : lst)
	{
		if (bModusHeizen==e.ChannelTypeHeizung())
			m_cbChannel.AddString(e.m_strName);
	}
	m_cbChannel.SetWindowText(strChannel);
	nSel = m_cbChannel.FindStringExact(-1,strChannel);
	if (nSel>=0)
		m_cbChannel.SetCurSel(nSel);

	m_edTemp.EnableWindow(bModusHeizen);
	m_edTempG.EnableWindow(bModusHeizen);
	m_edFaktor.EnableWindow(bModusHeizen);
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

void CRaumDlg::OnCbnSelchangeCbChannel()
{
	int nSel = m_cbChannel.GetCurSel();
	if (nSel<0)
		return;

	CString strChannel;
	m_cbChannel.GetLBText(nSel,strChannel);

	// Suche den Channel
	auto const* pDev = theApp.m_lstDevices.Find(strChannel);
	if (!pDev)
		return;

	// Wir kennen nun den Device. Holle den Datapoint und dort den hinteren Part
	CString strDataPoint{ pDev->m_strDataPoint };
	CString strParam{ StrValueByIndex(strDataPoint,2,_T('.')) };

	// IP- Thermostate-Aktoren-Gerätetyp (Kanal 1)
	//	SET_POINT_TEMPERATURE, SETPOINT
	// RT- Kennung Kanal Klassik-Thermostate-Aktoren-Gerätetyp (Kanal 4)
	//	SET_TEMPERATURE, 
	// TC- Kennung Kanal Klassik-Thermostate-Aktoren-Gerätetyp (Kanal 4)
	//  SET_POINT
	// IT- Kennung Kanal Klassik-Thermostate-Aktoren-Gerätetyp (Kanal 4)
	// SW- Kennung Kanal Klassik-Schalter-Aktoren-Gerätetyp (Kanal 1 bzw. 2)
	CString strType{ _T("SW") };
	struct {
		PCTSTR pszMode, pszParam;
	} const aTypes[] = {
		_T("IP"), _T("SET_POINT_TEMPERATURE"),
		_T("RT"), _T("SET_TEMPERATURE"),
		_T("TC"), _T("SETPOINT"),
	};
	for (auto const& e : aTypes)
	{
		if (strParam.CompareNoCase(e.pszParam)==0)
		{
			strType = e.pszMode;
			break;
		}
	}
	// Suche disen Mode
	DWORD dwMode = MAKELPARAM(strType[0],strType[1]);
	for (int i = 0, n = m_cbDevTyp.GetCount(); i<n; ++i)
	{
		if (m_cbDevTyp.GetItemData(i)==dwMode)
		{
			m_cbDevTyp.SetCurSel(i);
			break;
		}
	}
}
