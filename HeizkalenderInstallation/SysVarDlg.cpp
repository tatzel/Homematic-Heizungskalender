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


// SysVarDlg.cpp : implementation file
//

#include "pch.h"
#include "HeizkalenderInstallation.h"
#include "SysVarDlg.h"


// CSysVarDlg dialog

IMPLEMENT_DYNAMIC(CSysVarDlg, CDialogEx)

CSysVarDlg::CSysVarDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SYSVAR, pParent)
{

}

CSysVarDlg::~CSysVarDlg()
{
}

void CSysVarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_ED_ID, m_edId);
	DDX_Control(pDX, IDC_ED_NAME, m_edName);
	DDX_Control(pDX, IDC_ED_TYP, m_edTyp);
	DDX_Control(pDX, IDC_ED_WERT_ALT, m_edWertAlt);
	DDX_Control(pDX, IDC_ED_WERT_NEU, m_edWertNeu);
	DDX_Control(pDX, IDC_ED_STATUS, m_edStatus);

	CString strText = m_pSysVar->m_strName;
	DDX_Text(pDX, IDC_ED_NAME, strText);

	strText = IntToString(m_pSysVar->m_id);
	DDX_Text(pDX, IDC_ED_ID, strText);

	strText.Empty();
#define DECL_TEXT(x)	_T(#x)
	switch (m_pSysVar->m_dataType)
	{
	case DataType::vtString:
		strText = DECL_TEXT(vtString);
		break;
	case DataType::vtInteger:
		strText = DECL_TEXT(vtInteger);
		break;
	case DataType::vtboolean:
		strText = DECL_TEXT(vtBoolean);
		break;
	default:
		ASSERT(FALSE);
		strText = DECL_TEXT(vtUnknown);
		break;
	}
	DDX_Text(pDX, IDC_ED_TYP, strText);

	strText = m_pSysVar->m_strContent;
	DDX_Text(pDX, IDC_ED_WERT_NEU, strText);

	strText = m_pSysVar->m_strContentOld;
	DDX_Text(pDX, IDC_ED_WERT_ALT, strText);

	strText = BoolToString(m_pSysVar->m_bProtocoll);
	DDX_Text(pDX, IDC_ED_PROTOKOLLIERT, strText);

	strText.Empty();
	UINT uiText=0;
	if (m_pSysVar->m_bNew)
		uiText = IDS_NEW;
	else if (m_pSysVar->m_bModified)
		uiText = IDS_UPDATE;
	else 
		uiText = IDS_UNVERAENDERT;
	if (uiText)
		strText = CStringRes(uiText);
	DDX_Text(pDX, IDC_ED_STATUS, strText);
}


BEGIN_MESSAGE_MAP(CSysVarDlg, CDialogEx)
END_MESSAGE_MAP()


// CSysVarDlg message handlers

BOOL CSysVarDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString strTemp;
	GetWindowText(strTemp);
	strTemp += m_pSysVar->m_strName;
	SetWindowText(strTemp);

	return TRUE;  
}
