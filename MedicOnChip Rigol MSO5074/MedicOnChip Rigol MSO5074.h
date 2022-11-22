
// MedicOnChip Rigol MSO5074.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMedicOnChipRigolMSO5074App:
// See MedicOnChip Rigol MSO5074.cpp for the implementation of this class
//

class CMedicOnChipRigolMSO5074App : public CWinApp
{
public:
	CMedicOnChipRigolMSO5074App();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMedicOnChipRigolMSO5074App theApp;
