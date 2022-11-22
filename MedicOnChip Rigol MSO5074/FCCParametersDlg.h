#pragma once
#include "afxdialogex.h"


// FCCParametersDlg dialog

class FCCParametersDlg : public CDialogEx
{
	DECLARE_DYNAMIC(FCCParametersDlg)

public:
	FCCParametersDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~FCCParametersDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FCC };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()

public:
	float m_Vgi;
	float m_Vgf;
	float m_Vsdi;
	float m_Vsdf;
	float m_DeltaVg;
	float m_DeltaVsd;
	float m_DeltaTg1;
	float m_DeltaTg2;
	float m_DeltaTsd1;
	float m_DeltaTsd2;
	unsigned int m_Ng;
	unsigned int m_Nsd;
};
