#pragma once
#include "afxdialogex.h"


// CFCSParametersDlg dialog

class CFCSParametersDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFCSParametersDlg)

public:
	CFCSParametersDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CFCSParametersDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_FCS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float m_Vgmin;
	float m_Vgmax;
	float m_Vgfreq;
	float m_Vsd;
	unsigned int m_NAverage;
};
