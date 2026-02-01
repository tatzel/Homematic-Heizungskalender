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

//-----------------------------------------------------------------------------

class CEditBase : public CEdit
{
	DECLARE_DYNAMIC(CEditBase)
public:
// Construction/Destruction
	CEditBase();
	~CEditBase();

// Creation/Subclassing of the Window
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	// check if contents is valid, and function to force validation
	virtual bool IsValid() = 0;	
	virtual bool ValidateControl()=0;
	//check if current entry is not validated or unformated
	virtual bool IsDirty();
	
	// Associated controls
	virtual bool CreateSpinBtnCtrl(bool bInplace=true);

	// Neutral functions
	virtual void GetRectForAssocControl(CRect &rectMain, CRect &rectControl);
	virtual void DestroyAssocControl();
	virtual CWnd *GetAssocControl();

// Common attributes
	bool EnableSelectOnSetFocus(bool bOn=true);
	bool EnableEmptyCtrl(bool bEnableEmpty=true);

// ClassWizard generated virtual function overrides
	virtual void PreSubclassWindow();
	virtual void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Helper for new functions not implemented in the MFC
	void SetCueBanner(PCTSTR pszText);

protected:                   
// Reformating (virtual Callback)     
	enum eFormatControl {
		formatAndNotify,
		formatAfterKillFocus,
		formatDoNotNotify,
	};
	virtual void FormatControl(eFormatControl eFlag);	
	virtual void PrepareEditText();
	virtual void InitControl();
	void ReformatControl();

// Helper
	virtual void EnableAssocControl();

// Implementation Helper
	void SmartSetWindowText(PCTSTR pcStr, bool fLockOut=false);	

protected:
	bool	m_fIsDirty,					// Flag indicating a forreign SetText
			m_fInCreate,				// Flag set if created by ::Create Funktion
			m_fEnableEmptyCtrl,			// Allows a control to stay empty
			m_fEnableSelectOnSetFocus;	// Controls DLGC_HASSETSEL

	CWnd	*m_pWndCtrl;				// Associated Spincontrol or other Button

protected:                                                 
	DECLARE_MESSAGE_MAP()
	afx_msg LPARAM OnSetReadOnly(WPARAM wParam, LPARAM lParam);
	afx_msg LPARAM OnSetText(WPARAM wParam, LPARAM lParam);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg UINT OnGetDlgCode();
};

//-----------------------------------------------------------------------------

class CEditInt : public CEditBase
{   
	DECLARE_DYNCREATE(CEditInt)
public:
	typedef int TBaseType;
// Construction/Destruction
	CEditInt(int iMin=0, int iMax=10000);
	virtual ~CEditInt();

	// Subclassing of the Window
	BOOL SubclassDlgItem(UINT nID, CWnd* pParent, int iMin=0, int iMax=10000);

	// Check if Control cotains a valid int
	bool IsValid();	

	// Get/Set Value of the Control
	int	GetValue();                   
	void SetValue(int iValue);
	int SetStepValue(int iValue);	
	bool EnableEmptyOnZero(bool fEmptyOnZero=true);
	
	// Set Get Minima Maximia
	void SetMinMax(int iMin, int iMax);	
	void GetMinMax(int &iMin, int &iMax);	

protected:                   
// Reformating (virtual Callback)
	bool ValidateControl();
	void FormatControl(eFormatControl eFlag);	
	void InitControl();

protected:
	int	m_iMin,			// Actual Minimum
		m_iMax,			// Actual Maximum, allowed
		m_iStep,		// Step Value
		m_iValue;		// Current Value
	bool m_fEmptyOnZero;
	static const int m_cMaxInputLength;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};

void AFXAPI DDX_EditInt(CDataExchange* pDX, int nIDC, int &iValue);
void AFXAPI DDX_EditInt(CDataExchange* pDX, int nIDC, long &iValue);

class CEditDouble : public CEditBase
{   
	DECLARE_DYNCREATE(CEditDouble)
public:
	typedef double TBaseType;
// Construction/Destruction
	CEditDouble(int iPrecision=-1);
	virtual ~CEditDouble();

// Subclassing of the Window
	BOOL SubclassDlgItem(UINT nID, CWnd* pParent, int iPrecision=-1);

// Check if contents is valid
	bool IsValid();	

// Get/Set Value of the Control
	double GetValue();                   
	void SetValue(double fValue);
	double SetStepValue(double fValue);

// Set Get Minima Maximia
	int SetPrecision(int iPrecision);
	void SetMinMaxDefault();
	void SetMinMax(double fMin, double fMax);	
	void GetMinMax(double &fMin, double &fMax);	

protected:                   
// Reformating (virtual Callback)
	bool ValidateControl();
	void FormatControl(eFormatControl eFlag);	
	void InitControl();
	void PrepareEditText();

protected:
	int		m_iPrecision;	// Kommastelle
	double	m_fMin,			// Actual Minimum
			m_fMax,			// Actual Maximum, allowed
			m_fStep,		// Step rate for Microscroll
			m_fValue;		// Current Value
	static const int m_cMaxInputLength;

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};


void AFXAPI DDX_EditDouble(CDataExchange* pDX, int nIDC, double &dValue);

//-----------------------------------------------------------------------------

class CEditText : public CEditBase
{   
	DECLARE_DYNCREATE(CEditText)
public:
	typedef CString TBaseType;
// Construction/Destruction
	CEditText(int iLimitTextLength=0, UINT iStyle=fTypeAll);
	~CEditText();

// Subclass des Fensters	
	BOOL SubclassDlgItem(UINT nID, CWnd* pParent, int iLimitTextLength=0, UINT iStyle=fTypeAll);

// Field Types
	enum {                   
		fTypeSpaces		= 0x0001,
		fTypeNumeric	= 0x0002,
		fTypeAlpha		= 0x0004,
		fTypePunct		= 0x0008,
		fTypeUnderscore	= 0x0010,
		fTypeSpecial	= 0x0020,	// Sonderzeichen °§ etc...
		fTypeUpperCase	= 0x4000,
		fTypeKey		= fTypeAlpha|fTypeNumeric|fTypeUnderscore,
		fTypeAll		= 0x00ff,
	};
	int SetEditType(int iStyle)
	{
		int iOldTyle = m_iEditType;
		m_iEditType = iStyle;
		return iOldTyle;
	}
	int GetEditType() const
	{
		return m_iEditType;
	}

// Check if contents is valid
	bool IsValid();	
	void SetAllowedCharList(PCTSTR pszAllowedChars);
	const CString &GetAllowedCharList();
	void SetDisallowedCharList(PCTSTR pszAllowedChars);
	const CString &GetDisallowedCharList();

// Get/Set Value of the Control
	void SetValue(PCTSTR pszStr);
	CString GetValue();

protected:                   
// Reformating (virtual Callback)
	bool ValidateControl();
	void InitControl();

// Filter helper
	TCHAR Filter(TCHAR bChar);
	CString Filter(PCTSTR sString);
protected:
	int m_cInputLength;
	int	m_iEditType;
	CString m_strAllowedChars, m_strDisallowedChars;

protected:                                         
	DECLARE_MESSAGE_MAP()
	afx_msg LPARAM OnLimitText(WPARAM wParam, LPARAM lParam);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LPARAM OnPaste(WPARAM wParam, LPARAM lParam);
};

