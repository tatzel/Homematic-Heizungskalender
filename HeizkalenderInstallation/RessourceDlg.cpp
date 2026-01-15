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

// RessourceDlg.cpp : implementation file
//

#include "pch.h"
#include "HeizkalenderInstallation.h"
#include "afxdialogex.h"
#include "RessourceDlg.h"


// CRessourceDlg dialog

IMPLEMENT_DYNAMIC(CRessourceDlg, CDialogEx)

CRessourceDlg::CRessourceDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RESSOURCE, pParent)
{

}

CRessourceDlg::~CRessourceDlg()
{
}

void CRessourceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LB_DATA, m_lbData);

	if (pDX->m_bSaveAndValidate)
	{
		m_lstSelektierteRaeume.clear();
		for (int i = 0, n = m_lbData.GetCount(); i<n; ++i)
		{
			if (m_lbData.GetCheck(i))
				m_lstSelektierteRaeume.emplace(*static_cast<CString*>(m_lbData.GetItemDataPtr(i)));
		}
	}
	else
	{
		for (auto &e : m_lstAlleRaeume)
		{
			int n = m_lbData.AddString(theApp.RemoveRoomPrefix(e));
			if (n>=0)
			{
				m_lbData.SetItemDataPtr(n, const_cast<CString*>(&e));
				if (m_lstSelektierteRaeume.find(e)!=m_lstSelektierteRaeume.end())
					m_lbData.SetCheck(n,TRUE);
			}
		}
	}
}


BEGIN_MESSAGE_MAP(CRessourceDlg, CDialogEx)
END_MESSAGE_MAP()


// CRessourceDlg message handlers

BOOL CRessourceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Titel setzen
	CString strTitle;
	GetWindowText(strTitle);
	SetWindowText(strTitle + m_strTitel);

	// Minimum Zeilenhöhe bestimmen
	m_lbData.SetItemHeight(0,1);
	return TRUE;  
}
