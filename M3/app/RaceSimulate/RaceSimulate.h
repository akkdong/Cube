
// RaceSimulate.h : main header file for the RaceSimulate application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CRaceSimulateApp:
// See RaceSimulate.cpp for the implementation of this class
//

class CRaceSimulateApp : public CWinApp
{
public:
	CRaceSimulateApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CRaceSimulateApp theApp;
