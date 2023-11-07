
// RaceSimulator.h : main header file for the RaceSimulator application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CRaceSimulatorApp:
// See RaceSimulator.cpp for the implementation of this class
//

class CRaceSimulatorApp : public CWinApp
{
public:
	CRaceSimulatorApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CRaceSimulatorApp theApp;
