
// MedicOnChip Rigol MSO5074Dlg.h : header file
//

#pragma once

#include <visa.h>
#include "JanelaDoGrafico.h"
#include "FCCParametersDlg.h"
#include "FCSParametersDlg.h"
//#include "TestHandler.h"
#include <string>
//#define MAX_CANAIS			4

#define ID_TIMER_ADQUIRIR	1
#define ID_TIMER_FCC		2
#define ID_TIMER_FCS		3



// CMedicOnChipRigolMSO5074Dlg dialog
class CMedicOnChipRigolMSO5074Dlg : public CDialogEx
{
// Construction
public:
	CMedicOnChipRigolMSO5074Dlg(CWnd* pParent = nullptr);	// standard constructor
	virtual ~CMedicOnChipRigolMSO5074Dlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MEDICONCHIP_RIGOL_MSO5074_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	CComboBox m_combox;
	CString m_receive;

protected:
	//Gráficos da interface principal
	unsigned int m_numCanais;
	CJanelaDoGrafico* m_pwndGraficoCanal;	//m_wndGraficoCanal[MAX_CANAIS];
	BOOL m_bAquisicaoAtiva;
	ViSession m_defaultRM;
	ViSession m_vi;
	ViChar m_buffer[VI_FIND_BUFLEN];
	ViRsrc m_matches = m_buffer;
	ViUInt32 m_nmatches;
	ViFindList m_list;
	CFCCParametersDlg m_FCCParametersDlg;
	CFCSParametersDlg m_FCSParametersDlg;

public:
	afx_msg void OnBnClickedButtonSendAndRead();
	afx_msg void OnBnClickedButtonSend();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedButtonAdquirir();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

protected:
	bool iniciarAquisicao();
	bool encerrarAquisicao();
	void leDadosCanal(unsigned int);
	void SaveDatatoCSV();

public:
	afx_msg void OnBnClickedButtonFCC();
	afx_msg void OnBnClickedButtonFCS();
	
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton8();

};

void SendCommand(char command[256]);
std::string readOsciloscope(char command[256]);
