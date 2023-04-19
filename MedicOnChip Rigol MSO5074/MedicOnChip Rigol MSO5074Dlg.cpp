
// MedicOnChip Rigol MSO5074Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MedicOnChip Rigol MSO5074.h"
#include "MedicOnChip Rigol MSO5074Dlg.h"
#include "afxdialogex.h"
#include "TestHandler.h"
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <time.h>
#include <vector>
#include <sstream>

using namespace std;
using namespace std::filesystem;
namespace fs = std::filesystem;
//#define _USE_MATH_DEFINES
//#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


TestHandler tester;
bool started = false;
bool stopped = false;
bool force = false;
FCC_parameters results = tester.get_fcc_parameters();
//Set measurement channels
MeasurementChannel vds_meas(results.vds_meas_params.Id);
MeasurementChannel current_meas(results.current_meas_params.Id);
MeasurementChannel vg_meas(results.vg_meas_params.Id);
Trigger_parameters trigger_parameters;
SourceChannel vds_source, vg_source;
int burst_count = 0;
int vg_index = 0;
CString database_path = CString("database/");
std::string result_path = "";

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMedicOnChipRigolMSO5074Dlg dialog

CMedicOnChipRigolMSO5074Dlg::CMedicOnChipRigolMSO5074Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MEDICONCHIP_RIGOL_MSO5074_DIALOG, pParent)
	, m_receive(_T(""))
	, m_numCanais(3)
	, m_bAquisicaoAtiva(FALSE)
	, m_results_display(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pwndGraficoCanal = new CJanelaDoGrafico[m_numCanais];
}


CMedicOnChipRigolMSO5074Dlg::~CMedicOnChipRigolMSO5074Dlg()
{
	if (m_bAquisicaoAtiva)
		encerrarAquisicao();

	delete[] m_pwndGraficoCanal;
}

void CMedicOnChipRigolMSO5074Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_combox);
	DDX_Text(pDX, IDC_EDIT_RCVD_MSG, m_receive);
	DDX_Text(pDX, IDC_RESULTS, m_results_display);
}

BEGIN_MESSAGE_MAP(CMedicOnChipRigolMSO5074Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SEND_AND_READ, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonSendAndRead)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonSend)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_ADQUIRIR, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonAdquirir)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_FCC, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonFCC)
	ON_BN_CLICKED(IDC_BUTTON_FCS, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonFCS)
	ON_BN_CLICKED(IDC_BUTTON1, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton9)
END_MESSAGE_MAP()


// CMedicOnChipRigolMSO5074Dlg message handlers

BOOL CMedicOnChipRigolMSO5074Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	m_combox.AddString(_T("*IDN?"));
	m_combox.AddString(_T(":TIMebase:MAIN:SCALe?"));
	m_combox.AddString(_T(":CHANnel1:DISPlay?"));
	m_combox.AddString(_T(":CHANnel2:DISPlay?"));
	m_combox.AddString(_T(":CHANnel1:SCALe?"));
	m_combox.AddString(_T(":CHANnel2:SCALe?"));
	m_combox.AddString(_T(":ACQuire:TYPE?"));
	m_combox.AddString(_T(":ACQuire:SRATe?"));
	m_combox.AddString(_T(":ACQuire:MDEPth?"));
	m_combox.AddString(_T(":SOURce1:FUNCtion?"));
	m_combox.AddString(_T(":SOURce2:FUNCtion?"));
	m_combox.AddString(_T(":WAVeform:FORMat?"));
	m_combox.AddString(_T(":WAVeform:POINts?"));
	m_combox.AddString(_T(":TRIGger:STATus?"));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMedicOnChipRigolMSO5074Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMedicOnChipRigolMSO5074Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMedicOnChipRigolMSO5074Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


int CMedicOnChipRigolMSO5074Dlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	//Cria os gráficos dos canais
	for (int i = 0; i<m_numCanais; i++)
		m_pwndGraficoCanal[i].Create(NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), this, NULL, NULL);

	return 0;
}


void CMedicOnChipRigolMSO5074Dlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	float resHorz = (float)cx / 200.;
	float resVert = (float) cy / 200.;

	float alt = ((float) cy - (m_numCanais + 1) * resVert) / m_numCanais;

	for (int i = 0; i < m_numCanais; i++) {
		m_pwndGraficoCanal[i].MoveWindow(
			80 * resHorz,
			resVert + i*(alt+resVert),
			119 * resHorz,
			alt
			);
	}
}

// Trata o botão "Perguntar"
void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonSendAndRead()
{
		// TODO: Add your control notification handler code here
		ViSession defaultRM, vi;
		char buf[256] = { 0 };
		CString s, strTemp;
		//char* stringTemp;
		char stringTemp[256];

		ViChar buffer[VI_FIND_BUFLEN];
		ViRsrc matches = buffer;
		ViUInt32 nmatches;
		ViFindList list;

		viOpenDefaultRM(&defaultRM);
		//Acquire the USB resource of VISA
		viFindRsrc(defaultRM, "USB?*", &list, &nmatches, matches);
		viOpen(defaultRM, matches, VI_NULL, VI_NULL, &vi);

		//Send the command received
		m_combox.GetLBText(m_combox.GetCurSel(), strTemp);
		//m_combox.GetWindowText(strTemp);
		strTemp += "\n";
		//stringTemp = (char*)(LPCTSTR)strTemp;
		for (int i = 0; i < strTemp.GetLength(); i++)
			stringTemp[i] = (char)strTemp.GetAt(i);
		viPrintf(vi, stringTemp);

		//Read the results
		viScanf(vi, "%t\n", &buf);

		//Display the results
		UpdateData(TRUE);
		m_receive = buf;
		UpdateData(FALSE);
		viClose(defaultRM);
}

// Trata o botão "Comandar"
void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonSend()
{
	// TODO: Add your control notification handler code here
	ViSession defaultRM, vi;
	char buf[256] = { 0 };
	CString s, strTemp;
	//char* stringTemp;
	char stringTemp[256];

	ViChar buffer[VI_FIND_BUFLEN];
	ViRsrc matches = buffer;
	ViUInt32 nmatches;
	ViFindList list;

	viOpenDefaultRM(&defaultRM);
	//Acquire the USB resource of VISA
	viFindRsrc(defaultRM, "USB?*", &list, &nmatches, matches);
	viOpen(defaultRM, matches, VI_NULL, VI_NULL, &vi);

	//Send the command received
	//m_combox.GetLBText(m_combox.GetCurSel(), strTemp);
	m_combox.GetWindowText(strTemp);
	strTemp += "\n";
	//stringTemp = (char*)(LPCTSTR)strTemp;
	for (int i = 0; i < strTemp.GetLength(); i++)
		stringTemp[i] = (char)strTemp.GetAt(i);
	viPrintf(vi, stringTemp);

	viClose(defaultRM);
}

// Trata o botão "Adquirir"
void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonAdquirir()
{
	// TODO: Add your control notification handler code here
	if (!m_bAquisicaoAtiva)
	{
		if (iniciarAquisicao()) {
			GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_FCC)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_FCS)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_ADQUIRIR)->SetWindowText(_T("Encerrar"));
			for (int i=0; i<m_numCanais; i++)
				m_pwndGraficoCanal[i].ShowWindow(SW_SHOW);
			SetTimer(ID_TIMER_ADQUIRIR, 550, NULL);
		}
	}
	else
	{
		KillTimer(ID_TIMER_ADQUIRIR);
		encerrarAquisicao();
		for (int i = 0; i < m_numCanais; i++) {
			m_pwndGraficoCanal[i].ShowWindow(SW_HIDE);
			m_pwndGraficoCanal[i].limpaGrafico();
		}
		GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCC)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ADQUIRIR)->SetWindowText(_T("Adquirir"));
	}
}


// Trata o botão "FCC"
void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonFCC()
{
	// TODO: Add your control notification handler code here

	if (!m_bAquisicaoAtiva)
	{
		if (m_SNPrompt.DoModal() == IDOK) {
			if (iniciarAquisicao()) {
				GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_ADQUIRIR)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_FCS)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_FCC)->SetWindowText(_T("Encerrar"));

				std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
				time_t tt;
				tt = std::chrono::system_clock::to_time_t(today);
				char str[26];
				
				ctime_s(str, sizeof str, &tt);
				std::string date_str = "";

				for (int i = 0; i < 20;i++) {
					if (str[i + 4] == ' ' || str[i + 4] == ':') {
						date_str += '-';
					}
					else {
						date_str += str[i + 4];
					}
				}

				CString results_path = database_path + m_SNPrompt.m_Serial_Number + "/FCC/" + date_str.c_str() + "/";
				result_path = CStringA(results_path);
				if (!fs::exists(result_path)) {
					fs::create_directories(result_path);
				}
				
				for (int i = 0; i < m_numCanais; i++)
					m_pwndGraficoCanal[i].ShowWindow(SW_SHOW);



				//Debugging:
				//std::string parameters_log;
				//parameters_log = "Parametros do FCC:\n";
				//parameters_log = parameters_log + "Canal 1 (Vds) - Tipo: " + results.vds_source_params.wave_type;
				//parameters_log = parameters_log + ", " + std::to_string(results.vds_source_params.v_pp) + "Vpp, Offset de " + std::to_string(results.vds_source_params.v_offset);
				//parameters_log = parameters_log + ", " + std::to_string(results.vds_source_params.freq) + " Hz";


				
				//Set time scale
				tester.set_t_scale(results.t_scale);
				//Set measurement channels
				vds_meas.write_parameters_to_osc(results.vds_meas_params);
				current_meas.write_parameters_to_osc(results.current_meas_params);
				vg_meas.write_parameters_to_osc(results.vg_meas_params);
				//Set trigger
				trigger_parameters.source = "CHAN1";
				tester.send_trigger_parameters(trigger_parameters);
				//Set Source channels
				vg_index = 0;
				//fs::create_directory("vg0");

				vds_source.write_parameters_to_osc(results.vds_source_params);
				vg_source.write_parameters_to_osc(results.vg_source_params);

				char buff[10] = { 0 };

				//Stop
				string_to_char_array(sys_commands.STOP, &buff[0]);
				SendCommand(buff);

				//Limpa tela
				string_to_char_array(sys_commands.CLEAR, &buff[0]);
				SendCommand(buff);

				//Desliga canais de fontes
				vds_source.stop(1);
				vg_source.stop(2);

				//Liga canais de medição
				vds_meas.on();
				current_meas.on();
				vg_meas.on();

				string_to_char_array(sys_commands.SINGLE, &buff[0]);
				SendCommand(buff);

				//string_to_char_array(sys_commands.TFORCE, &buff[0]);
				//SendCommand(buff);
				vg_source.start(2);
				vds_source.start(1);

				/*Isso não funciona porque o loop de tempo indeterminado trava a interface.Vamos ter que fazer por TIMER
				while (tester.read_trigger_status() != "RUN") {
					UpdateData(TRUE);
					m_receive = "espera";
					UpdateData(FALSE);
				}

				while (tester.read_trigger_status() != "STOP") {
					UpdateData(TRUE);
					m_receive = "espera2";
					UpdateData(FALSE);
				}

				UpdateData(TRUE);
				m_receive = "foi";
				UpdateData(FALSE);*/
				
				//
				//eDatatoCSV();

				//UpdateData(TRUE);
				//m_receive = tester.log_string.c_str();
				//UpdateData(FALSE);
/*
				//Ajusta as escalas dos canais 1 e 2
				viPrintf(m_vi, ":CHANnel1:SCALe 200E-3\n");
				viPrintf(m_vi, ":CHANnel1:POSition 0\n");

				viPrintf(m_vi, ":CHANnel2:SCALe 50e-3\n");
				viPrintf(m_vi, ":CHANnel2:POSition 0\n");

				//Ajusta a base de tempo horizontal
				viPrintf(m_vi, ":TIMEbase:SCALe 200E-3\n");

				//Ajusta a saída do gerador de sinais 1
				//viPrintf(m_vi, ":SOURce1:TYPE NONE\n");
				viPrintf(m_vi, ":SOURce1:FUNCtion RAMP\n");
				viPrintf(m_vi, ":SOURce1:FUNCtion:RAMP:SYMMetry 50\n");
				viPrintf(m_vi, ":SOURce1:VOLTage 800E-3\n");
				viPrintf(m_vi, ":SOURce1:VOLTage:OFFSet 300E-3\n");
				viPrintf(m_vi, ":SOURce1:FREQuency 2\n");
				viPrintf(m_vi, ":OUTPut1:IMPedance OMEG\n");
				
				//Ajusta a saída do gerador de sinais 2
				viPrintf(m_vi, ":SOURce2:TYPE NONE\n");
				viPrintf(m_vi, ":SOURce2:FUNCtion DC\n");
				viPrintf(m_vi, ":SOURce2:VOLTage:OFFSet 100E-3\n");
				viPrintf(m_vi, ":OUTPut2:IMPedance OMEG\n");

				//Liga os geradores
				viPrintf(m_vi, ":OUTPut1:STATe ON\n");
				viPrintf(m_vi, ":OUTPut2:STATe ON\n");
*/	
				//Ativa o timer
				SetTimer(ID_TIMER_FCC, 1000, NULL);
			}
		}
	}
	else
	{
		KillTimer(ID_TIMER_FCC);
/*
		//Desliga os geradores de sinais
		viPrintf(m_vi, ":OUTPut1:STATe OFF\n");
		viPrintf(m_vi, ":OUTPut2:STATe OFF\n");
*/
		encerrarAquisicao();
		for (int i = 0; i < m_numCanais; i++) {
			m_pwndGraficoCanal[i].ShowWindow(SW_HIDE);
			m_pwndGraficoCanal[i].limpaGrafico();
		}
		GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ADQUIRIR)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCC)->SetWindowText(_T("FCC"));
	}
}


void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonFCS()
{
	char temp[256];

	// TODO: Add your control notification handler code here
	if (!m_bAquisicaoAtiva)
	{
		if (m_FCSParametersDlg.DoModal() == IDOK)
		{
			if (iniciarAquisicao()) {
				GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_ADQUIRIR)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_FCC)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_FCS)->SetWindowText(_T("Encerrar"));

				//Ajusta a escala do canal 1 (Vsd)
				sprintf_s(temp, 256, ":CHANnel1:SCALe %.0e\n", fabs(m_FCSParametersDlg.m_Vsd) / 3);
				viPrintf(m_vi, temp);
				viPrintf(m_vi, ":CHANnel1:POSition 0\n");

				//Ajusta a escala do canal 2 (Vg)
				sprintf_s(temp, 256, ":CHANnel2:SCALe %.0e\n", m_FCSParametersDlg.m_Vgmax / 3);
				viPrintf(m_vi, temp);
				viPrintf(m_vi, ":CHANnel2:POSition 0\n");

				//Ajusta a base de tempo horizontal
				sprintf_s(temp, 256, ":TIMEbase:SCALe %.0e\n", 0.5 * 1 / m_FCSParametersDlg.m_Vgfreq);
				viPrintf(m_vi, temp);

				//Ajusta a saída do gerador de sinais 1 (Vsd)
				viPrintf(m_vi, ":SOURce1:TYPE NONE\n");
				viPrintf(m_vi, ":SOURce1:FUNCtion DC\n");
				sprintf_s(temp, 256, ":SOURce1:VOLTage:OFFSet %f\n", m_FCSParametersDlg.m_Vsd);
				viPrintf(m_vi, temp);
				viPrintf(m_vi, ":OUTPut1:IMPedance OMEG\n");

				//Ajusta a saída do gerador de sinais 2 (Vg)
				viPrintf(m_vi, ":SOURce2:TYPE NONE\n");
				viPrintf(m_vi, ":SOURce2:FUNCtion RAMP\n");
				viPrintf(m_vi, ":SOURce2:FUNCtion:RAMP:SYMMetry 50\n");
				sprintf_s(temp, 256, ":SOURce2:VOLTage %f\n", 
					m_FCSParametersDlg.m_Vgmax - m_FCSParametersDlg.m_Vgmin);
				viPrintf(m_vi, temp);
				sprintf_s(temp, 256, ":SOURce2:FREQuency %f\n",	
					m_FCSParametersDlg.m_Vgfreq);
				viPrintf(m_vi, temp);
				sprintf_s(temp, 256, ":SOURce2:VOLTage:OFFSet %f\n",
					(m_FCSParametersDlg.m_Vgmax + m_FCSParametersDlg.m_Vgmin) / 2);
				viPrintf(m_vi, temp);
				viPrintf(m_vi, ":OUTPut2:IMPedance OMEG\n");

				//Ajusta o trigger
				viPrintf(m_vi, ":TRIGger:EDGE:SOURce CHANnel2\n");
				viPrintf(m_vi, ":TRIGger:EDGE:SLOPe POSitive\n");
				sprintf_s(temp, 256, ":TRIGger:EDGE:LEVel %f\n",
					(m_FCSParametersDlg.m_Vgmax + m_FCSParametersDlg.m_Vgmin) / 2);
				viPrintf(m_vi, temp);

				//Liga os geradores
				viPrintf(m_vi, ":OUTPut1:STATe ON\n");
				viPrintf(m_vi, ":OUTPut2:STATe ON\n");

				//Mostra os gráficos
				for (int i = 0; i < m_numCanais; i++)
					m_pwndGraficoCanal[i].ShowWindow(SW_SHOW);

				//Ativa o timer
				//SetTimer(ID_TIMER_FCS, 1000, NULL);
			}
		}
	}
	else
	{
		KillTimer(ID_TIMER_FCS);

		//Desliga os geradores de sinais
		viPrintf(m_vi, ":OUTPut1:STATe OFF\n");
		viPrintf(m_vi, ":OUTPut2:STATe OFF\n");

		encerrarAquisicao();
		for (int i = 0; i < m_numCanais; i++) {
			m_pwndGraficoCanal[i].ShowWindow(SW_HIDE);
			m_pwndGraficoCanal[i].limpaGrafico();
		}

		GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ADQUIRIR)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCC)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCS)->SetWindowText(_T("FCS"));
	}
}


// Mensagens dos timers
void CMedicOnChipRigolMSO5074Dlg::OnTimer(UINT_PTR nIDEvent)
{
	std::string trg_status = tester.read_trigger_status();
	// TODO: Add your message handler code here and/or call default
	CString temp1, temp2;
	float soma, media;
	int tam;
	float Ts;
	int tam_vds;
	int tam_current;
	int tam_fcc = 0;
	std::ofstream myfile;
	char buff[10] = { 0 };

	switch (nIDEvent) {
	case ID_TIMER_ADQUIRIR:
	case ID_TIMER_FCC:
		if (trg_status == "RUN\n") {
			started = true;
			stopped = false;
		}
		else if (started && (trg_status == "STOP\n")) {
			started = false;
			stopped = true;
		}
		
		if (stopped) {
			if (vg_index < results.vg_vector.size()) {
				if (burst_count < NUM_MEDIAS) {
					vds_source.stop(1);
					vg_source.stop(2);
					std::string path = result_path + "vg" + std::to_string(vg_index);
					if (!fs::exists(path)) {
						fs::create_directories(path);
					}
					path += "/results" + std::to_string(burst_count) + ".csv";
					myfile.open(path);
					myfile << "t,vds,corrente\n";
					Ts = vds_meas.get_sample_period(m_vi);
					leDadosCanal(vds_meas.get_id());
					leDadosCanal(current_meas.get_id());
					leDadosCanal(vg_meas.get_id());
					tam_vds = m_pwndGraficoCanal[vds_meas.get_id() - 1].getTamVetorDeDados();
					tam_current = m_pwndGraficoCanal[current_meas.get_id() - 1].getTamVetorDeDados();
					if (tam_vds >= tam_current) {
						tam_fcc = tam_vds;
					}
					else {
						tam_fcc = tam_current;
					}
					for (int i = 0; i < tam_fcc; i++) {
						myfile << Ts * i << ";" << m_pwndGraficoCanal[vds_meas.get_id() - 1].getAt(i) << ";" << m_pwndGraficoCanal[current_meas.get_id() - 1].getAt(i) << ";" << m_pwndGraficoCanal[vg_meas.get_id() - 1].getAt(i) << "\n";
					}
					burst_count++;
					if (burst_count != NUM_MEDIAS)	//Se for o ultimo burst do ultimo Vg, nao ligar os canais denovo
					{
						string_to_char_array(sys_commands.SINGLE, &buff[0]);
						SendCommand(buff);
						vg_source.start(2);
						vds_source.start(1);
						force = false;
					}
				}
				else {
					burst_count = 0;
					vg_index++;
					if (vg_index < results.vg_vector.size()) {
						results.vg_source_params.v_offset = results.vg_vector[vg_index];
						vg_source.write_parameters_to_osc(results.vg_source_params);
						string_to_char_array(sys_commands.SINGLE, &buff[0]);
						SendCommand(buff);
						vg_source.start(2);
						vds_source.start(1);
						stopped = false;
						started = false;
					}
				}
			}
			else {
				vg_index = 0;
				vg_source.stop(2);
				vds_source.stop(1);

				UpdateData(TRUE);
				m_results_display = _T("ENSAIO: FCC\r\nSN: ") + m_SNPrompt.m_Serial_Number + _T("\r\nRESULTADO: APROVADO");
				UpdateData(FALSE);

				m_SNPrompt.m_Serial_Number = "";

				OnBnClickedButtonFCC();
			}
		}
		break;
	case ID_TIMER_FCS:
		for (int i = 1; i <= m_numCanais; i++) {
			leDadosCanal(i);
			
			soma = 0;
			tam = m_pwndGraficoCanal[i - 1].getTamVetorDeDados();
			for (int j = 0; j < tam; j++)
				soma += m_pwndGraficoCanal[i - 1].getAt(j);
			media = soma / tam;

			temp1.Format(_T("Média canal %d = %f\t"), i, media);
			temp2 += temp1;
		}
		GetDlgItem(IDC_EDIT_RCVD_MSG)->SetWindowTextW(temp2);
		break;

	default:
		break;
	}
	
	CDialogEx::OnTimer(nIDEvent);
}



// Abre a interface de comunicação com o osciloscópio
bool CMedicOnChipRigolMSO5074Dlg::iniciarAquisicao()
{
	// TODO: Add your implementation code here.

	//Abre a comunicação USB
	viOpenDefaultRM(&m_defaultRM);
	if (VI_SUCCESS != viFindRsrc(m_defaultRM, "USB?*", &m_list, &m_nmatches, m_matches)) {
		viClose(m_defaultRM);
		AfxMessageBox(_T("Erro: USB não encontrada"));
		return false;
	}

	//Abre a comunicação com o osciloscópio
	if (VI_SUCCESS != viOpen(m_defaultRM, m_matches, VI_NULL, VI_NULL, &m_vi)) {
		viClose(m_defaultRM);
		AfxMessageBox(_T("Erro: Osciloscópio não encontrado"));
		return false;
	}

	// Ajusta a aquisição
	char temp[200];
	for (int i = 1; i <= m_numCanais; i++) {
		sprintf_s(temp, 200, ":CHANnel%d:DISPlay ON\n", i);
		viPrintf(m_vi, temp);
	}
	for (int i = m_numCanais+1; i <= 4; i++) {
		sprintf_s(temp, 200, ":CHANnel%d:DISPlay OFF\n", i);
		viPrintf(m_vi, temp);
	}
	viPrintf(m_vi, ":WAVeform:MODE NORMal\n");
	viPrintf(m_vi, ":WAVeform:FORMat BYTE\n");

	m_bAquisicaoAtiva = TRUE;

	return true;
}

// Encerra a interface de comunicação
bool CMedicOnChipRigolMSO5074Dlg::encerrarAquisicao()
{
	// TODO: Add your implementation code here.

	viClose(m_vi);
	viClose(m_defaultRM);

	m_bAquisicaoAtiva = FALSE;

	return true;
}

// Lê os dados do canal especificado e imprime no gráfico
void CMedicOnChipRigolMSO5074Dlg::leDadosCanal(unsigned int canal)
{
	ViByte buf[2048];		//unsigned char
	ViUInt32 cnt = 2048;
	ViUInt32  readcnt;

	char* temp;
	float Ts;
	float deltaV;
	int N;
	int tam;
	float* sinal;

	//Seleciona o canal
	temp = new char[256];
	sprintf_s(temp, 256, ":WAVeform:SOURce CHANnel%d\n", canal);
	viPrintf(m_vi, temp);
	delete[] temp;

	//Determina a resolução horizontal
	viPrintf(m_vi, ":WAVeform:XINCrement?\n");
	viRead(m_vi, buf, cnt, &readcnt);
	temp = new char[readcnt];
	for (int i = 0; i < readcnt; i++)
		temp[i] = buf[i];
	Ts = atof(temp);		//Período de amostragem
	delete[] temp;

	//Determina a resolução vertical
	viPrintf(m_vi, ":WAVeform:YINCrement?\n");
	viRead(m_vi, buf, cnt, &readcnt);
	temp = new char[readcnt];
	for (int i = 0; i < readcnt; i++)
		temp[i] = buf[i];
	deltaV = atof(temp);	//Resolução vertical
	delete[] temp;

	//Lê os dados
	viPrintf(m_vi, ":WAVeform:DATA?\n");
	viRead(m_vi, buf, cnt, &readcnt);
	temp = new char[2];					// Obtém o parâmetro N do cabeçalho
	sprintf_s(temp, 2, "%c", buf[1]);
	N = atoi(temp);
	delete[] temp;

	temp = new char[N + 1];				// Obtém o tamanho do buffer de dados
	for (int i = 0; i < N; i++)
		temp[i] = buf[2 + i];
	temp[N] = '\n';
	tam = atoi(temp);
	delete[] temp;

	/*
	CString lixo;						// Imprime o tamanho
	lixo.Format(_T("%d"), tam);
	GetDlgItem(IDC_EDIT_RCVD_MSG)->SetWindowTextW(lixo);
	*/

	//std::ofstream myfile;
	//myfile.open("example.csv");

	sinal = new float[tam];				// Aloca o buffer e preenche
	for (int i = 0; i < tam; i++) {
		sinal[i] = (buf[2 + N + i] - 127) * deltaV;
		//myfile << Ts * i << "," << sinal[i] << "\n";
	}

	//myfile.close();

	m_pwndGraficoCanal[canal - 1].ajustaEscalas(tam * Ts, 127 * deltaV);
	m_pwndGraficoCanal[canal - 1].plotaGrafico(sinal, tam);
	delete[] sinal;
}

void SendCommand(char _command[256])
{
	ViSession defaultRM, vi;
	char buf[256] = { 0 };
	CString s, command;
	char stringTemp[256];

	ViChar buffer[VI_FIND_BUFLEN];
	ViRsrc matches = buffer;
	ViUInt32 nmatches;
	ViFindList list;

	viOpenDefaultRM(&defaultRM);
	//Acquire the USB resource of VISA
	viFindRsrc(defaultRM, "USB?*", &list, &nmatches, matches);
	viOpen(defaultRM, matches, VI_NULL, VI_NULL, &vi);


	command = _command;
	command += "\n";

	for (int i = 0; i < command.GetLength(); i++)
		stringTemp[i] = (char)command.GetAt(i);

	viPrintf(vi, stringTemp);

	viClose(vi);
	viClose(defaultRM);
}

std::string readOsciloscope(char _command[256])
{
	
	ViSession defaultRM, vi;
	char buf[256] = { 0 };
	CString s, strTemp;
	char stringTemp[256];

	ViChar buffer[VI_FIND_BUFLEN];
	ViRsrc matches = buffer;
	ViUInt32 nmatches;
	ViFindList list;

	viOpenDefaultRM(&defaultRM);

	//Acquire the USB resource of VISA
	viFindRsrc(defaultRM, "USB?*", &list, &nmatches, matches);
	viOpen(defaultRM, matches, VI_NULL, VI_NULL, &vi);

	//Send the command received
	strTemp = _command;
	strTemp += "\n";
	for (int i = 0; i < strTemp.GetLength(); i++)
		stringTemp[i] = (char)strTemp.GetAt(i);
	viPrintf(vi, stringTemp);

	//Read the results
	viScanf(vi, "%t\n", &buf);
	//viClose(vi);
	viClose(defaultRM);
	std::string aux = buf;
	return  aux;
};


void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton1()
{
	TestHandler teste;
	teste.clear_screen();
}


void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton2()
{
	SourceChannel gerador1;
	SourceChannel_parameters source_1;
	source_1.freq = 60;
	source_1.Id = 1;
	source_1.v_offset = 0;
	source_1.v_pp = 2;
	source_1.wave_type = "RAMP";
	gerador1.start(source_1.Id);
}


void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton3()
{
	SourceChannel gerador1;
	SourceChannel_parameters source_1;
	source_1.freq = 60;
	source_1.Id = 1;
	source_1.v_offset = 0;
	source_1.v_pp = 2;
	source_1.wave_type = "RAMP";
	gerador1.stop(source_1.Id);
}


void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton4()
{
	SourceChannel gerador1;
	SourceChannel_parameters source_1;
	source_1.freq = 60;
	source_1.Id = 1;
	source_1.v_offset = 0;
	source_1.v_pp = 2;
	source_1.wave_type = "RAMP";
	gerador1.waveForm_write_to_osc(source_1);

}


void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton5()
{
	SourceChannel gerador1;
	SourceChannel_parameters source_1;
	source_1.freq = 60;
	source_1.Id = 1;
	source_1.v_offset = 0;
	source_1.v_pp = 2;
	source_1.wave_type = "RAMP";
	gerador1.Frequency_write_to_osc(source_1);

}


void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton6()
{
	SourceChannel gerador1;
	SourceChannel_parameters source_1;
	source_1.freq = 60;
	source_1.Id = 1;
	source_1.v_offset = 1;
	source_1.v_pp = 2;
	source_1.wave_type = "RAMP";
	gerador1.VPP_write_to_osc(source_1);
}


void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton7()
{
	SourceChannel gerador1;
	SourceChannel_parameters source_1;
	source_1.freq = 60;
	source_1.Id = 1;
	source_1.v_offset = 1;
	source_1.v_pp = 2;
	source_1.wave_type = "RAMP";
	gerador1.Voffset_write_to_osc(source_1);
}


void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton8()
{
	SourceChannel gerador1;
	SourceChannel_parameters source_1;
	source_1.freq = 10000;
	source_1.Id = 1;
	source_1.v_offset = 1;
	source_1.v_pp = 2;
	source_1.wave_type = "RAMP";
	source_1.generatorType = "BURSt";
	source_1.cycles = 15;
	source_1.Burst_Type = "NCYCle";
	gerador1.write_parameters_to_osc(source_1);

}

void Files_Path_in_Directory(std::filesystem::path path)
{
	std::string add = "Danilo ";
	int i = 1;
	

	for (const auto& file : std::filesystem::directory_iterator(path)) {

		
		std::ifstream teste(file.path(), ios::in);


		if (teste.is_open()) {

			ofstream newfile((add + to_string(i) + ".csv"), ios::out | ios::app);
			vector<string> row;
			string line, word;
			word.clear();

			while (!teste.eof())
			{
				row.clear();
				getline(teste, line);

				for (int p = 0; p < line.length(); p++) {
					if (line[p] == ';' || (p==line.length()-1)) {
						
						if (p == line.length() - 1) {
							word = word + line[p];
						}

							row.push_back(word);
							word.clear();
					}
					else {
						word = word + line[p];
					}
				}
				for (int i = 0; i < row.size(); i++)
				{
					if( i == row.size() - 1 ){
						newfile << stod(row[i])*2 << "\n";
					}
					else {
						newfile << stod(row[i])*2 << ";";
					}
				}		
				word.clear();
			}
			i++;
			newfile.close();
		}

	}
}

void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton9()
{
	// TODO: Add your control notification handler code here
	const std::filesystem::path pasta{ "Arquivos csv" };
	Files_Path_in_Directory(pasta);
}
