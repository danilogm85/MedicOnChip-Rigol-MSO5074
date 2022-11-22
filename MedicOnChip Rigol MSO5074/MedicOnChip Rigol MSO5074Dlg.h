
// MedicOnChip Rigol MSO5074Dlg.h : header file
//

#pragma once

#include <visa.h>
#include "JanelaDoGrafico.h"
#include "FCCParametersDlg.h"

#define MAX_CANAIS			4

#define ID_TIMER_ADQUIRIR	1
#define ID_TIMER_FCC		2


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
	CJanelaDoGrafico m_wndGraficoCanal[MAX_CANAIS];
	BOOL m_bAquisicaoAtiva;
	ViSession m_defaultRM, m_vi;
	ViChar m_buffer[VI_FIND_BUFLEN];
	ViRsrc m_matches = m_buffer;
	ViUInt32 m_nmatches;
	ViFindList m_list;
	FCCParametersDlg m_FCCParametersDlg;

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

public:
	afx_msg void OnBnClickedButtonFCC();
};
