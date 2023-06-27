
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
#include <thread>
#include <time.h>
#include <windows.h>
#include <algorithm>
#include <cmath>
#include <sstream>
#define csv_columns 4
#define cabecalho "t[s];vds[v];corrente_ctrl[v];vg[v]"
#define LOG_SIZE 10

using namespace std;
using namespace std::filesystem;
namespace fs = std::filesystem;
//#define _USE_MATH_DEFINES
//#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

vector <string> customSplit2(string str, char separator);
bool more_recent(vector<std::string> date, vector<std::string> ref);
//void build_log_message(std::string msg);

TestHandler tester;
//bool started = false;
bool stopped = false;
unsigned int num_bursts;
//bool force = false;
FCC_parameters results = tester.get_fcc_parameters();
FCS_parameters results_fcs = tester.get_fcs_parameters();
FCP_parameters results_fcp = tester.get_fcp_parameters();
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
vector <std::string> log_lines;
bool flag_reset_waveform = true;
bool flag_run_all = false;
bool flag_fcc = false;
bool flag_fcs = false;
bool flag_fcp = false;
bool flag_scale_set = false;
bool flag_scale_set_status = false;
float max_vds = -1000;
float min_vds = 1000;
int Freq_Iterator = 0;
std::string FCPpath = "";
bool new_fcp_flag = false;
CString promptFCP = CString("");

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

	char buff[20] = { 0 };

	//Set High Res and 1M points
	string_to_char_array(sys_commands.RUN, &buff[0]);
	SendCommand(buff);
	string_to_char_array(sys_commands.MDEPTH, &buff[0]);
	SendCommand(buff, false, ":ACQuire:MDEPth?", "1.0000E+06");
	//string_to_char_array(sys_commands.HRES, &buff[0]);
	//SendCommand(buff);
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
	ON_BN_CLICKED(IDC_BUTTON_FCS, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonFCS)/*
	ON_BN_CLICKED(IDC_BUTTON1, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton8)*/
	ON_BN_CLICKED(IDC_BUTTON9, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON_FCP, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonFcp)
	//ON_BN_CLICKED(IDC_BUTTON10, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON_FCS_ALT, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonFcsAlt)
	//ON_BN_CLICKED(IDC_NEWFCP, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedNewfcp)
	ON_BN_CLICKED(IDC_BUTTON_FCP_Alt, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonFcpAlt)
	ON_BN_CLICKED(IDC_BUTTON_RUNALL, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonRunall)
	ON_BN_CLICKED(NewFCP, &CMedicOnChipRigolMSO5074Dlg::OnBnClickedNewfcp)
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

	using namespace std::this_thread; // sleep_for, sleep_until
	using namespace std::chrono; // nanoseconds, system_clock, seconds

	if (!m_bAquisicaoAtiva)
	{
		int status_sn_prompt = 0;
		if (!flag_run_all) {
			status_sn_prompt = m_SNPrompt.DoModal();
		}
		if (status_sn_prompt == IDOK || flag_run_all) {
			if (iniciarAquisicao()) {
				GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_ADQUIRIR)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_FCS)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_FCP)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_FCP_Alt)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_FCS_ALT)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_FCC)->SetWindowText(_T("Encerrar"));

				if (flag_run_all) {
					GetDlgItem(IDC_BUTTON_FCC)->EnableWindow(FALSE);
				}
				else {
					GetDlgItem(IDC_BUTTON_FCC)->EnableWindow(TRUE);
					GetDlgItem(IDC_BUTTON_RUNALL)->EnableWindow(FALSE);
				}

				//char bufff[24] = { 0 };
				//reset_square_wave();

				flag_fcc = true;
				flag_scale_set = false;

				UpdateData(TRUE);
				m_receive = "";
				UpdateData(FALSE);

				num_bursts = results.bursts;
				/*
				std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
				time_t tt;
				tt = std::chrono::system_clock::to_time_t(today);
				char str[27] = { 0 };

				ctime_s(str, sizeof str, &tt);
				std::string date_str = "";

				for (int i = 0; i < 20; i++) {
					if (str[i + 4] == ' ' || str[i + 4] == ':') {
						if (str[i + 5] != ' ' && str[i + 5] != ':') date_str += '-';
					}
					else {
						date_str += str[i + 4];
					}
				}

				CString results_path = database_path + m_SNPrompt.m_Serial_Number + "/FCC/" + date_str.c_str() + "/";*/

				std::time_t currentTime;
				std::time(&currentTime);
				std::tm timeInfo;
				localtime_s(&timeInfo, &currentTime);

				std::string date_str = "";
				date_str += "/" + std::to_string(timeInfo.tm_year + 1900);
				date_str += "/" + std::to_string(timeInfo.tm_mon + 1);
				date_str += "/" + std::to_string(timeInfo.tm_mday);
				date_str += "/";

				std::string time_str = "";
				int hour = timeInfo.tm_hour;
				int minute = timeInfo.tm_min;
				int second = timeInfo.tm_sec;

				if (hour <= 9) {
					time_str += "0" + std::to_string(hour) + "-";
				}
				else {
					time_str += std::to_string(hour) + "-";
				}

				if (minute <= 9) {
					time_str += "0" + std::to_string(minute) + "-";
				}
				else {
					time_str += std::to_string(minute) + "-";
				}

				if (second <= 9) {
					time_str += "0" + std::to_string(second);
				}
				else {
					time_str += std::to_string(second);
				}

				CString results_path = database_path + date_str.c_str() + m_SNPrompt.m_Serial_Number + "/FCC/" + time_str.c_str() + "/";

				result_path = CStringA(results_path);
				if (!fs::exists(result_path)) {
					fs::create_directories(result_path);
				}
				CopyConfig();
				for (int i = 0; i < m_numCanais; i++)
					m_pwndGraficoCanal[i].ShowWindow(SW_SHOW);



				//Debugging:
				//std::string parameters_log;
				//parameters_log = "Parametros do FCC:\n";
				//parameters_log = parameters_log + "Canal 1 (Vds) - Tipo: " + results.vds_source_params.wave_type;
				//parameters_log = parameters_log + ", " + std::to_string(results.vds_source_params.v_pp) + "Vpp, Offset de " + std::to_string(results.vds_source_params.v_offset);
				//parameters_log = parameters_log + ", " + std::to_string(results.vds_source_params.freq) + " Hz";
				/*
				build_log_message("Configurando osciloscópio para FCC");
				UpdateData(TRUE);
				m_receive = tester.log_string.c_str();
				UpdateData(FALSE);
				*/
				//Set time scale
				tester.set_t_scale(results.t_scale);
				//Set measurement channels
				vds_meas.write_parameters_to_osc(results.vds_meas_params);
				current_meas.write_parameters_to_osc(results.current_meas_params);
				vg_meas.write_parameters_to_osc(results.vg_meas_params);
				//Set trigger
				trigger_parameters.source = "CHAN2";
				trigger_parameters.level = 0.01;
				tester.send_trigger_parameters(trigger_parameters);
				//Set Source channels
				vg_index = 0;
				//fs::create_directory("vg0");

				vds_source.write_parameters_to_osc(results.vds_source_params);
				results.vg_source_params.v_offset = results.vg_vector[0];
				vg_source.write_parameters_to_osc(results.vg_source_params);

				char buff[20] = { 0 };

				string_to_char_array(sys_commands.ALIAS_OFF, &buff[0]);
				SendCommand(buff);
				for (int i = 0; i < 20; i++) {
					buff[i] = 0;
				}

				string_to_char_array(sys_commands.RUN, &buff[0]);
				SendCommand(buff);
				sleep_for(milliseconds(500));
				tester.type_Aquire(results.AquireType);
				/*
				//Set High Res and 1M points
				string_to_char_array(sys_commands.RUN, &buff[0]);
				SendCommand(buff);
				string_to_char_array(sys_commands.MDEPTH, &buff[0]);
				SendCommand(buff);
				string_to_char_array(sys_commands.HRES, &buff[0]);
				SendCommand(buff);
				*/
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

				vg_source.start(2);
				vds_source.start(1);
				sleep_for(milliseconds(1000));
				string_to_char_array(sys_commands.SINGLE, &buff[0]);
				SendCommand(buff);
				//string_to_char_array(sys_commands.TFORCE, &buff[0]);
				//SendCommand(buff);


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
		vg_source.stop(2);
		vds_source.stop(1);
		burst_count = 0;
		//stopped = false;
		//started = false;
		if(!flag_run_all) m_SNPrompt.m_Serial_Number = "";
		flag_fcc = false;

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
		GetDlgItem(IDC_BUTTON_FCP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCC)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCP_Alt)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCS_ALT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_RUNALL)->EnableWindow(TRUE);
	}
}


void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonFCS()
{
	using namespace std::this_thread; // sleep_for, sleep_until
	using namespace std::chrono; // nanoseconds, system_clock, seconds

	//char temp[256];

	// TODO: Add your control notification handler code here
	if (!m_bAquisicaoAtiva)
	{
		int status_sn_prompt = 0;
		if (!flag_run_all) {
			status_sn_prompt = m_SNPrompt.DoModal();
		}
		if (status_sn_prompt == IDOK || flag_run_all){
			if (iniciarAquisicao()) {
				GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_ADQUIRIR)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_FCC)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_FCS)->SetWindowText(_T("Encerrar"));
				GetDlgItem(IDC_BUTTON_FCP)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_FCP_Alt)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_FCS_ALT)->EnableWindow(FALSE);

				if (flag_run_all) {
					GetDlgItem(IDC_BUTTON_FCS)->EnableWindow(FALSE);
				}
				else {
					GetDlgItem(IDC_BUTTON_FCS)->EnableWindow(TRUE);
					GetDlgItem(IDC_BUTTON_RUNALL)->EnableWindow(FALSE);
				}

				UpdateData(TRUE);
				m_receive = "";
				UpdateData(FALSE);

				/*
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
				*/
				/*
				build_log_message("Configurando osciloscópio para FCS");
				UpdateData(TRUE);
				m_receive = tester.log_string.c_str();
				UpdateData(FALSE);
				*/
				//reset_square_wave();
				flag_fcs = true;
				flag_scale_set = false;

				num_bursts = results_fcs.bursts;
				/*
				std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
				time_t tt;
				tt = std::chrono::system_clock::to_time_t(today);
				char str[27] = { 0 };

				ctime_s(str, sizeof str, &tt);
				std::string date_str = "";

				for (int i = 0; i < 20; i++) {
					if (str[i + 4] == ' ' || str[i + 4] == ':') {
						if (str[i + 5] != ' ' && str[i + 5] != ':') date_str += '-';
					}
					else {
						date_str += str[i + 4];
					}
				}

				CString results_path = database_path + m_SNPrompt.m_Serial_Number + "/FCS/" + date_str.c_str() + "/";*/

				std::time_t currentTime;
				std::time(&currentTime);
				std::tm timeInfo;
				localtime_s(&timeInfo, &currentTime);

				std::string date_str = "";
				date_str += "/" + std::to_string(timeInfo.tm_year + 1900);
				date_str += "/" + std::to_string(timeInfo.tm_mon + 1);
				date_str += "/" + std::to_string(timeInfo.tm_mday);
				date_str += "/";

				std::string time_str = "";
				int hour = timeInfo.tm_hour;
				int minute = timeInfo.tm_min;
				int second = timeInfo.tm_sec;

				if (hour <= 9) {
					time_str += "0" + std::to_string(hour) + "-";
				}
				else {
					time_str += std::to_string(hour) + "-";
				}

				if (minute <= 9) {
					time_str += "0" + std::to_string(minute) + "-";
				}
				else {
					time_str += std::to_string(minute) + "-";
				}

				if (second <= 9) {
					time_str += "0" + std::to_string(second);
				}
				else {
					time_str += std::to_string(second);
				}

				CString results_path = database_path + date_str.c_str() + m_SNPrompt.m_Serial_Number + "/FCS/" + time_str.c_str() + "/";

				result_path = CStringA(results_path);
				if (!fs::exists(result_path)) {
					fs::create_directories(result_path);
				}
				CopyConfig();
				//Mostra os gráficos
				for (int i = 0; i < m_numCanais; i++)
					m_pwndGraficoCanal[i].ShowWindow(SW_SHOW);

				//Set time scale
				tester.set_t_scale(results_fcs.t_scale);
				//Set measurement channels
				vds_meas.write_parameters_to_osc(results_fcs.vds_meas_params);
				current_meas.write_parameters_to_osc(results_fcs.current_meas_params);
				vg_meas.write_parameters_to_osc(results_fcs.vg_meas_params);
				//Set trigger
				trigger_parameters.source = "CHAN3";
				//trigger_parameters.level = results_fcs.vg_source_params.v_offset;
				tester.send_trigger_parameters(trigger_parameters);

				vds_source.write_parameters_to_osc(results_fcs.vds_source_params);
				vg_source.write_parameters_to_osc(results_fcs.vg_source_params);

				char buff[20] = { 0 };

				string_to_char_array(sys_commands.RUN, &buff[0]);
				SendCommand(buff);
				sleep_for(milliseconds(500));
				tester.type_Aquire(results_fcs.AquireType);

				string_to_char_array(sys_commands.ALIAS_ON, &buff[0]);
				SendCommand(buff);
				for (int i = 0; i < 20; i++) {
					buff[i] = 0;
				}

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

				vg_source.start(2);
				vds_source.start(1);
				sleep_for(milliseconds(100));

				string_to_char_array(sys_commands.SINGLE, &buff[0]);
				SendCommand(buff);
				//string_to_char_array(sys_commands.TFORCE, &buff[0]);
				//SendCommand(buff);


				//Ativa o timer
				SetTimer(ID_TIMER_FCS, 1000, NULL);
			}
		}
	}
	else
	{
		KillTimer(ID_TIMER_FCS);

		//Desliga os geradores de sinais
		//viPrintf(m_vi, ":OUTPut1:STATe OFF\n");
		//viPrintf(m_vi, ":OUTPut2:STATe OFF\n");
		vg_source.stop(2);
		vds_source.stop(1);
		burst_count = 0;
		//stopped = false;
		//started = false;
		if (!flag_run_all) m_SNPrompt.m_Serial_Number = "";
		flag_fcs = false;
		
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
		GetDlgItem(IDC_BUTTON_FCS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCP_Alt)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCS_ALT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_RUNALL)->EnableWindow(TRUE);
	}
}

void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonFcp()
{
	using namespace std::this_thread; // sleep_for, sleep_until
	using namespace std::chrono; // nanoseconds, system_clock, seconds

	if (!m_bAquisicaoAtiva)
	{
		if (m_SNPrompt.DoModal() == IDOK)
		{
			if (iniciarAquisicao()) {
				GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_ADQUIRIR)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_FCC)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_FCS)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_FCP_Alt)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_FCS_ALT)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_RUNALL)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_FCP)->SetWindowText(_T("Encerrar"));
				/*
				build_log_message("Configurando osciloscópio para FCP");
				UpdateData(TRUE);
				m_receive = tester.log_string.c_str();
				UpdateData(FALSE);
				*/
				flag_fcp = true;

				num_bursts = results_fcp.bursts;

				std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
				time_t tt;
				tt = std::chrono::system_clock::to_time_t(today);
				char str[27] = { 0 };

				ctime_s(str, sizeof str, &tt);
				std::string date_str = "";

				for (int i = 0; i < 20; i++) {
					if(str[i + 4] == ' ' || str[i + 4] == ':') {
						if (str[i + 5] != ' ' && str[i + 5] != ':') date_str += '-';
					}
					else {
						date_str += str[i + 4];
					}
				}

				//Verifica se VG_MIN e VG_MAX já foram obtidos para esse SN e pega eles
				vector<string> splitted_values_aux;
				vector<string> splitted_values_ref;
				CString fcs_path = database_path + m_SNPrompt.m_Serial_Number + "/FCS/";	//CString por causa do SN
				std::string vg_values_path = CStringA(fcs_path);	//Jogando na result_path, que é string, porque a função de baixo só aceita string
				std::filesystem::path caminho{ vg_values_path };

				if (fs::exists(vg_values_path)) {
					//Encontrar o teste mais recente
					std::string most_recent = "";
					bool first = true;
					for (auto& p : std::filesystem::recursive_directory_iterator(vg_values_path)) {
						if (p.is_directory()) {
							if (first) {
								first = false;
								most_recent = p.path().string();
								splitted_values_ref = customSplit2(most_recent, '-');

							}
							else {
								splitted_values_aux = customSplit2(p.path().string(), '-');
								if (more_recent(splitted_values_aux, splitted_values_ref)) {
									most_recent = p.path().string();
									splitted_values_ref = customSplit2(most_recent, '-');
								}
							}
						}
					}

					vg_values_path = most_recent + "/vg_values.ini";

					//Verificar se no teste mais recente existem os valores de VG
					if (fs::exists(vg_values_path)) {

						mINI::INIFile file(vg_values_path);
						mINI::INIStructure ini;
						file.read(ini);
						float min = stof(ini.get("VG").get("MIN"));
						float max = stof(ini.get("VG").get("MAX"));

						results_fcp.vg_source_params.v_pp = max - min;
						trigger_parameters.level = max - results_fcp.vg_source_params.v_pp/2;
						results_fcp.vg_source_params.v_offset = max - results_fcp.vg_source_params.v_pp / 2;
						results_fcp.vg_meas_params.volts_div = max / 4;

					}
					else {
						UpdateData(TRUE);
						m_receive = "O ultimo FCS desse chip não calculou os valores de Vg_max e Vg_min para realizar o FCP";
						UpdateData(FALSE);

						encerrarAquisicao();
						for (int i = 0; i < m_numCanais; i++) {
							m_pwndGraficoCanal[i].ShowWindow(SW_HIDE);
							m_pwndGraficoCanal[i].limpaGrafico();
						}

						GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(TRUE);
						GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
						GetDlgItem(IDC_BUTTON_ADQUIRIR)->EnableWindow(TRUE);
						GetDlgItem(IDC_BUTTON_FCC)->EnableWindow(TRUE);
						GetDlgItem(IDC_BUTTON_FCS)->EnableWindow(TRUE);
						GetDlgItem(IDC_BUTTON_FCP_Alt)->EnableWindow(TRUE);
						GetDlgItem(IDC_BUTTON_FCS_ALT)->EnableWindow(TRUE);
						GetDlgItem(IDC_BUTTON_RUNALL)->EnableWindow(TRUE);
						GetDlgItem(IDC_BUTTON_FCP)->SetWindowText(_T("FCP"));

						return;
					}
				}
				else {
					UpdateData(TRUE);
					m_receive = "Esse chip ainda não foi testado no FCS";
					UpdateData(FALSE);

					encerrarAquisicao();
					for (int i = 0; i < m_numCanais; i++) {
						m_pwndGraficoCanal[i].ShowWindow(SW_HIDE);
						m_pwndGraficoCanal[i].limpaGrafico();
					}

					GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(TRUE);
					GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
					GetDlgItem(IDC_BUTTON_ADQUIRIR)->EnableWindow(TRUE);
					GetDlgItem(IDC_BUTTON_FCC)->EnableWindow(TRUE);
					GetDlgItem(IDC_BUTTON_FCS)->EnableWindow(TRUE);
					GetDlgItem(IDC_BUTTON_FCP_Alt)->EnableWindow(TRUE);
					GetDlgItem(IDC_BUTTON_FCS_ALT)->EnableWindow(TRUE);
					GetDlgItem(IDC_BUTTON_RUNALL)->EnableWindow(TRUE);
					GetDlgItem(IDC_BUTTON_FCP)->SetWindowText(_T("FCP"));

					return;
				}

				CString results_path = database_path + m_SNPrompt.m_Serial_Number + "/FCP/" + date_str.c_str();
				
				if (Freq_Iterator == 0) {
					FCPpath = result_path;
				}
				
				result_path = CStringA(results_path);
				if (!fs::exists(result_path)) {
					fs::create_directories(result_path);
				}

				//Mostra os gráficos
				for (int i = 0; i < m_numCanais; i++)
					m_pwndGraficoCanal[i].ShowWindow(SW_SHOW);

				//Set time scale
				tester.set_t_scale(results_fcp.t_scale);
				//Set measurement channels
				vds_meas.write_parameters_to_osc(results_fcp.vds_meas_params);
				current_meas.write_parameters_to_osc(results_fcp.current_meas_params);
				vg_meas.write_parameters_to_osc(results_fcp.vg_meas_params);
				//Set trigger
				trigger_parameters.source = "CHAN3";
				tester.send_trigger_parameters(trigger_parameters);

				vds_source.write_parameters_to_osc(results_fcp.vds_source_params);
				vg_source.write_parameters_to_osc(results_fcp.vg_source_params);

				char buff[10] = { 0 };
				/*
				//Set High Res and 1M points
				string_to_char_array(sys_commands.MDEPTH, &buff[0]);
				SendCommand(buff);
				string_to_char_array(sys_commands.MDEPTH, &buff[0]);
				SendCommand(buff);
				*/
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
				//current_meas.on();
				vg_meas.on();

				//string_to_char_array(sys_commands.TFORCE, &buff[0]);
				//SendCommand(buff);

				vds_source.start(1);
				vg_source.start(2);
				sleep_for(milliseconds(100));
				string_to_char_array(sys_commands.SINGLE, &buff[0]);
				SendCommand(buff);

				

				//Ativa o timer
				SetTimer(ID_TIMER_FCP, 1000, NULL);
			}
		}
	}
	else
	{
		KillTimer(ID_TIMER_FCP);

		vg_source.stop(2);
		//vds_source.stop(1);
		burst_count = 0;
		//stopped = false;
		//started = false;
		m_SNPrompt.m_Serial_Number = "";
		flag_fcp = false;

		encerrarAquisicao();
		for (int i = 0; i < m_numCanais; i++) {
			m_pwndGraficoCanal[i].ShowWindow(SW_HIDE);
			m_pwndGraficoCanal[i].limpaGrafico();
		}

		GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ADQUIRIR)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCC)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCP_Alt)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCS_ALT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_RUNALL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCP)->SetWindowText(_T("FCP"));
	}
}


// Mensagens dos timers
void CMedicOnChipRigolMSO5074Dlg::OnTimer(UINT_PTR nIDEvent)
{
	using namespace std::this_thread; // sleep_for, sleep_until
	using namespace std::chrono; // nanoseconds, system_clock, seconds
	//std::string trg_status = tester.read_trigger_status();
	//ViConstString read = ":TRIGger:STATus?\n";
	char trg_status_buff[256] = { 0 };
	//string_to_char_array(read, SCPI_command);
	viPrintf(m_vi, ":TRIGger:STATus?\n");
	viScanf(m_vi, "%t\n", &trg_status_buff);
	std::string trg_status = trg_status_buff;
	// TODO: Add your message handler code here and/or call default
	CString temp1, temp2;
	float soma, media;
	int tam;
	float Ts;
	int tam_vg;
	int tam_vds;
	int tam_current;
	int tam_fcc = 0;
	std::ofstream myfile;
	char buff[10] = { 0 };
	char buff2[23] = { 0 };
	//float measurements[][];
	
	switch (nIDEvent) {
	case ID_TIMER_ADQUIRIR:
	case ID_TIMER_FCC:
		if (trg_status == "STOP\n") {

			if (vg_index < results.vg_vector.size()) {
				if (burst_count < num_bursts) {
					/*
					std::string msg = "Aquisição encerrada, BURST " + std::to_string(burst_count) + ", Vg = " + std::to_string(results.vg_vector[vg_index]);
					build_log_message(msg);
					UpdateData(TRUE);
					m_receive = tester.log_string.c_str();
					UpdateData(FALSE);
					*/
					vds_source.stop(1);
					vg_source.stop(2);
					std::string vg_str = "";
					if (results.vg_vector[vg_index] == 0) {
						vg_str = "0";
					}
					else {
						vg_str = std::to_string(results.vg_vector[vg_index]);
						bool dot_flag = false;
						for (int i = 0; i < vg_str.size(); i++) {
							if (vg_str[i] == '.') {
								vg_str[i] = 'v';
							}
						}
					}

					std::string path = result_path + "vg_" + vg_str;
					if (!fs::exists(path)) {
						/*
						build_log_message("Criando diretórios");
						UpdateData(TRUE);
						m_receive = tester.log_string.c_str();
						UpdateData(FALSE);*/
						fs::create_directories(path);
					}

					//myfile.open(path);
					//myfile << cabecalho << "\n";
					//Ts = vds_meas.get_sample_period(m_vi);
					/*
					build_log_message("Lendo dados do canal");
					UpdateData(TRUE);
					m_receive = tester.log_string.c_str();
					UpdateData(FALSE);
					*/
					/*
					leDadosCanal(vds_meas.get_id(), BUCKET_SIZE_DEFAULT, raw_data_path, mean_data_path);
					leDadosCanal(current_meas.get_id(), BUCKET_SIZE_DEFAULT, raw_data_path);
					leDadosCanal(vg_meas.get_id(), BUCKET_SIZE_DEFAULT, raw_data_path);

					leDadosCanal(vds_meas.get_id(), BUCKET_SIZE_DEFAULT, raw_data_path);
					leDadosCanal(current_meas.get_id(), BUCKET_SIZE_DEFAULT, raw_data_path);
					leDadosCanal(vg_meas.get_id(), BUCKET_SIZE_DEFAULT, raw_data_path); 

					tam_vds = m_pwndGraficoCanal[vds_meas.get_id() - 1].getTamVetorDeDados();
					tam_current = m_pwndGraficoCanal[current_meas.get_id() - 1].getTamVetorDeDados();
					
					build_log_message("Escrevendo curvas nos arquivos");
					UpdateData(TRUE);
					m_receive = tester.log_string.c_str();
					UpdateData(FALSE);
					
					if (tam_vds >= tam_current) {
						tam_fcc = tam_vds;
					}
					else {
						tam_fcc = tam_current;
					}
					for (int i = 0; i < tam_fcc; i++) {
						myfile << Ts * i << ";" << m_pwndGraficoCanal[vds_meas.get_id() - 1].getAt(i) << ";" << m_pwndGraficoCanal[current_meas.get_id() - 1].getAt(i) << ";" << m_pwndGraficoCanal[vg_meas.get_id() - 1].getAt(i) << "\n";
					}
					myfile.close();
					*/
					std::string raw_data_path = path + "/raw_data" + std::to_string(burst_count) + ".dat";
					path += "/results" + std::to_string(burst_count) + ".csv";
					vector <unsigned int> channels = { vds_meas.get_id(), current_meas.get_id(), vg_meas.get_id() };
					Measure_and_save(channels, BUCKET_SIZE_DEFAULT, raw_data_path, path);

					burst_count++;
					if (burst_count != num_bursts)	//Se for o ultimo burst do ultimo Vg, nao ligar os canais denovo
					{/*
						build_log_message("Preparando próxima aquisição");
						UpdateData(TRUE);
						m_receive = tester.log_string.c_str();
						UpdateData(FALSE);
						*/

						vg_source.start(2);
						vds_source.start(1);
						sleep_for(milliseconds(100));
						string_to_char_array(sys_commands.SINGLE, &buff[0]);
						SendCommand(buff);

						//force = false;
					}
				}
				else {

					burst_count = 0;
					vg_index++;
					if (vg_index < results.vg_vector.size()) {
						/*
						build_log_message("Iniciando medições para o próximo Vg");
						UpdateData(TRUE);
						m_receive = tester.log_string.c_str();
						UpdateData(FALSE);
						*/
						results.vg_source_params.v_offset = results.vg_vector[vg_index];
						vg_source.write_parameters_to_osc(results.vg_source_params);
						sleep_for(milliseconds(300));
						vg_source.start(2);
						vds_source.start(1);
						sleep_for(milliseconds(1000));
						string_to_char_array(sys_commands.SINGLE, &buff[0]);
						SendCommand(buff);

						//stopped = false;
						//started = false;
					}
				}
			}
			else {/*
				build_log_message("Aquisições finalizadas, desligando canais");
				UpdateData(TRUE);
				m_receive = tester.log_string.c_str();
				UpdateData(FALSE);
				*/
				vg_index = 0;
				vg_source.stop(2);
				vds_source.stop(1);
				/*
				build_log_message("Chamando Python para análise dos dados");
				UpdateData(TRUE);
				m_receive = tester.log_string.c_str();
				UpdateData(FALSE);
				*/
				//COLOCAR FUNÇÃO MÉDIA
				std::ofstream myfile;
				myfile.open("diretorio.csv");
				myfile << result_path << "\n";
				myfile.close();

				//const std::string &path = "C:\\Users\\Mediconchip.DESKTOP-K5I25D1\\Desktop\\Repositório Fabrinni\\ResistenciaCanal.py";
				//LPCTSTR blabla = path.c_str();

				//system("C:\\Users\\Mediconchip.DESKTOP-K5I25D1\\Desktop\\Repositório Fabrinni\\ResistenciaCanal.py");
				system("python.exe FCC_Current_SourceFinal.py");

				/*
				build_log_message("Resultado pronto");
				UpdateData(TRUE);
				m_receive = tester.log_string.c_str();
				UpdateData(FALSE);
				
				string res;
				result_path += "/vg_0/resistencia.csv";
				std::ifstream resistance(result_path, ios::in);
				if (resistance.is_open()) {
					getline(resistance, res);
					getline(resistance, res);
				}
				resistance.close();

				std::string res_value = "";
				for (int i = 0; i < res.size();i++) {
					if (res[i] == '[') {
						i++;
						while (res[i] != ']') {
							res_value += res[i];
							i++;
						}
						break;
					}
				}
				//std::ofstream myfile;
				myfile.open(result_path, std::ios_base::app);
				UpdateData(TRUE);
				if (stof(res_value) >= results.rg_limits["MIN"] && stof(res_value) <= results.rg_limits["MAX"]) {
					m_results_display = _T("ENSAIO: FCC\r\nSN: ") + m_SNPrompt.m_Serial_Number + _T("\r\nRESULTADO: APROVADO");
					myfile << "PASS" << "\n";
				}
				else{
					m_results_display = _T("ENSAIO: FCC\r\nSN: ") + m_SNPrompt.m_Serial_Number + _T("\r\nRESULTADO: REPROVADO");
					myfile << "FAIL" << "\n";
				}
				UpdateData(FALSE);
				myfile.close();
				*/
				UpdateData(TRUE);
				m_results_display = _T("ENSAIO: FCC\r\nSN: ") + m_SNPrompt.m_Serial_Number;
				UpdateData(FALSE);
				if (!flag_run_all) m_SNPrompt.m_Serial_Number = "";
				/*
				build_log_message("Fim do FCC");
				UpdateData(TRUE);
				m_receive = tester.log_string.c_str();
				UpdateData(FALSE);
				*/
				OnBnClickedButtonFCC();
				if (flag_run_all) {
					OnBnClickedButtonFCS();
				}
			}
		}
		else {/*
			build_log_message("Aquisição em progresso");
			UpdateData(TRUE);
			m_receive = tester.log_string.c_str();
			UpdateData(FALSE);*/
		}
		break;
	case ID_TIMER_FCS:
		if (trg_status == "STOP\n") {
			//if (vg_index < results.vg_vector.size()) {
				if (burst_count < num_bursts) {
					/*
					std::string msg = "Aquisição encerrada, BURST: " + std::to_string(burst_count);
					build_log_message(msg);
					UpdateData(TRUE);
					m_receive = tester.log_string.c_str();
					UpdateData(FALSE);
					*/
					vds_source.stop(1);
					vg_source.stop(2);
					std::string path = result_path;
					if (!fs::exists(path)) {/*
						build_log_message("Criando diretórios");
						UpdateData(TRUE);
						m_receive = tester.log_string.c_str();
						UpdateData(FALSE);*/
						fs::create_directories(path);
					}/*
					path += "/results" + std::to_string(burst_count) + ".csv";
					myfile.open(path);
					myfile << cabecalho << "\n";
					/*
					build_log_message("Lendo dados do canal");
					UpdateData(TRUE);
					m_receive = tester.log_string.c_str();
					UpdateData(FALSE);

					Ts = vg_meas.get_sample_period(m_vi);
					leDadosCanal(vds_meas.get_id(), BUCKET_SIZE_DEFAULT, "");
					leDadosCanal(current_meas.get_id(), BUCKET_SIZE_DEFAULT, "");
					leDadosCanal(vg_meas.get_id(), BUCKET_SIZE_DEFAULT, "");
					tam_vg = m_pwndGraficoCanal[vg_meas.get_id() - 1].getTamVetorDeDados();
					tam_current = m_pwndGraficoCanal[current_meas.get_id() - 1].getTamVetorDeDados();
					/*
					build_log_message("Escrevendo curvas nos arquivos");
					UpdateData(TRUE);
					m_receive = tester.log_string.c_str();
					UpdateData(FALSE);
					
					if (tam_vg >= tam_current) {
						tam_fcc = tam_vg;
					}
					else {
						tam_fcc = tam_current;
					}
					for (int i = 0; i < tam_fcc; i++) {
						myfile << Ts * i << ";" << m_pwndGraficoCanal[vds_meas.get_id() - 1].getAt(i) << ";" << m_pwndGraficoCanal[current_meas.get_id() - 1].getAt(i) << ";" << m_pwndGraficoCanal[vg_meas.get_id() - 1].getAt(i) << "\n";
					}
					myfile.close();
					*/
					std::string raw_data_path = path + "/raw_data" + std::to_string(burst_count) + ".dat";
					path += "/results" + std::to_string(burst_count) + ".csv";
					vector <unsigned int> channels = { vds_meas.get_id(), current_meas.get_id(), vg_meas.get_id() };
					Measure_and_save(channels, BUCKET_SIZE_DEFAULT, raw_data_path, path);

					burst_count++;
					if (burst_count != num_bursts)	//Se for o ultimo burst do ultimo Vg, nao ligar os canais denovo
					{/*
						build_log_message("Preparando próxima aquisição");
						UpdateData(TRUE);
						m_receive = tester.log_string.c_str();
						UpdateData(FALSE);
						*/

						vg_source.start(2);
						vds_source.start(1);
						sleep_for(milliseconds(100));
						string_to_char_array(sys_commands.SINGLE, &buff[0]);
						SendCommand(buff);

						//force = false;
					}
				}
				else {/*
					build_log_message("Aquisições finalizadas, desligando canais");
					UpdateData(TRUE);
					m_receive = tester.log_string.c_str();
					UpdateData(FALSE);
					*/
					burst_count = 0;
					//stopped = false;
					//started = false;
					//vg_index++;
					//if (vg_index < results.vg_vector.size()) {
					//	results.vg_source_params.v_offset = results.vg_vector[vg_index];
					//	vg_source.write_parameters_to_osc(results.vg_source_params);
					//	string_to_char_array(sys_commands.SINGLE, &buff[0]);
					//	SendCommand(buff);
					//	vg_source.start(2);
					//	vds_source.start(1);

					//}
				
			//}
			//else {
					//vg_index = 0;
					vg_source.stop(2);
					vds_source.stop(1);
					//COLOCAR FUNÇÃO MÉDIA
					/*
					build_log_message("Chamando Python para análise dos dados");
					UpdateData(TRUE);
					m_receive = tester.log_string.c_str();
					UpdateData(FALSE);
					*/
					std::ofstream myfile;
					myfile.open("FCS.csv");
					myfile << result_path << "\n";
					myfile.close();

					system("python.exe FCS_Current_SourceFinal.py");
					/*
					build_log_message("Resultado pronto");
					UpdateData(TRUE);
					m_receive = tester.log_string.c_str();
					UpdateData(FALSE);
					*/
					UpdateData(TRUE);
					m_results_display = _T("ENSAIO: FCS\r\nSN: ") + m_SNPrompt.m_Serial_Number;
					UpdateData(FALSE);

					if (!flag_run_all) m_SNPrompt.m_Serial_Number = "";
					/*
					build_log_message("Fim do FCS");
					UpdateData(TRUE);
					m_receive = tester.log_string.c_str();
					UpdateData(FALSE);
					*/
					OnBnClickedButtonFCS();
					if (flag_run_all) {
						OnBnClickedButtonFcpAlt();
					}
				}
			//}
		}
		else {/*
			build_log_message("Aquisição em progresso");
			UpdateData(TRUE);
			m_receive = tester.log_string.c_str();
			UpdateData(FALSE);*/
		}

		break;
	case ID_TIMER_FCP:
		/*
		if (trg_status == "RUN\n") {
			started = true;
			stopped = false;
		}
		else if (started && (trg_status == "STOP\n")) {
			started = false;
			stopped = true;
		}*/

		//if (trg_status == "STOP\n") {
		if (flag_scale_set_status) {
			string_to_char_array(sys_commands.STOP, &buff[0]);
			SendCommand(buff);
		}
		else {
			if (trg_status == "STOP\n") {
				stopped = true;
			}
		}
		if (stopped || flag_scale_set_status) {
			if (burst_count < num_bursts) {
				/*
				std::string msg = "Aquisição encerrada, BURST " + std::to_string(burst_count);
				build_log_message(msg);
				UpdateData(TRUE);
				m_receive = tester.log_string.c_str();
				UpdateData(FALSE);
				*/
				vds_source.stop(1);
				vg_source.stop(2);
				//std::string path = result_path;
				std::string path = FCPpath;
				if (!fs::exists(path)) {/*
					build_log_message("Criando diretórios");
					UpdateData(TRUE);
					m_receive = tester.log_string.c_str();
					UpdateData(FALSE);*/
					fs::create_directories(path);
				}
				/*
				build_log_message("Lendo dados do canal");
				UpdateData(TRUE);
				m_receive = tester.log_string.c_str();
				UpdateData(FALSE);

				/*
				build_log_message("Escrevendo curvas nos arquivos");
				UpdateData(TRUE);
				m_receive = tester.log_string.c_str();
				UpdateData(FALSE);*/

				//std::string raw_data_path = path + "/raw_data" + std::to_string(burst_count) + ".dat";
				//path += "/results" + std::to_string(burst_count) + ".csv";
				
				std::string raw_data_path = "";
				switch (Freq_Iterator) {
					case 0:
						result_path = path + "FLOW/";
						break;
					case 1:
						result_path = path + "FHIGH/";
						break;
					default:
						break;
				}
				if (!fs::exists(result_path)) {
					fs::create_directories(result_path);
				}
				raw_data_path = result_path + "raw_data" + std::to_string(burst_count) + ".dat";
				path = result_path + "results" + std::to_string(burst_count) + ".csv";
				
				vector <unsigned int> channels = { vds_meas.get_id(), current_meas.get_id(), vg_meas.get_id() };
				Measure_and_save(channels, BUCKET_SIZE_FCP, raw_data_path, path, results_fcp.vds_meas_params.offset);

				if (flag_scale_set_status) {
					burst_count++;
				}
				else {
					burst_count = num_bursts;
				}

				if (burst_count != num_bursts)	//Se for o ultimo burst do ultimo Vg, nao ligar os canais denovo
				{/*
					build_log_message("Preparando próxima aquisição");
					UpdateData(TRUE);
					m_receive = tester.log_string.c_str();
					UpdateData(FALSE);*/
					vds_source.start(1);
					vg_source.start(2);
					sleep_for(milliseconds(100));
					string_to_char_array(sys_commands.SINGLE, &buff[0]);
					SendCommand(buff);
					//vds_source.start(1);
				}
				else {
					KillTimer(ID_TIMER_FCP);
					SetTimer(ID_TIMER_FCP, 200, NULL);
				}
			}
			else {/*
				build_log_message("Aquisições finalizadas, desligando canais");
				UpdateData(TRUE);
				m_receive = tester.log_string.c_str();
				UpdateData(FALSE);*/
				m_SNPrompt.m_Serial_Number = "";
				burst_count = 0;
				stopped = false;
				//started = false;
				vds_source.stop(1);
				vg_source.stop(2);
				//vds_source.stop(1);
				/*
				build_log_message("Chamando Python para análise dos dados");
				UpdateData(TRUE);
				m_receive = tester.log_string.c_str();
				UpdateData(FALSE);
				*/
				if (!flag_scale_set_status) {
					flag_scale_set_status = true;
					flag_scale_set = false;
					KillTimer(ID_TIMER_FCP);
				}
				else {
					flag_scale_set_status = false;
					std::ofstream myfile;
					myfile.open("FCP.csv");
					for (int i = 0; i < result_path.size(); i++) {
						if (result_path[i] == '/') {
							result_path[i] = '\\';
						}
					}
					myfile << result_path << "\n";
					myfile.close();

					system("python.exe FCP_Final.py");

					//COLOCAR FUNÇÃO MÉDIA
					UpdateData(TRUE);
					m_results_display = _T("ENSAIO: FCP\r\nSN: ") + promptFCP;
					UpdateData(FALSE);
					reset_square_wave();
				}

				/*
				build_log_message("Fim do FCP");
				UpdateData(TRUE);
				m_receive = tester.log_string.c_str();
				UpdateData(FALSE);*/

				//m_SNPrompt.m_Serial_Number = "";	//FCP mult. freq. Fabrinni
				OnBnClickedButtonFcpAlt();
			}
			/*}
			else {
				build_log_message("Aquisição em progresso");
				UpdateData(TRUE);
				m_receive = tester.log_string.c_str();
				UpdateData(FALSE);
			}*/
		}
		break;
	/*case ID_TIMER_RESET:
		//string_to_char_array(":SOURce1:FUNCtion RAMP\n", &buff2[0]);
		//SendCommand(buff2);
		string_to_char_array(":SOURce2:FUNCtion RAMP\n", &buff2[0]);
		SendCommand(buff2);
		flag_reset_waveform = false;
		OnBnClickedButton10();
		break;*/
	default:
		break;
	}
	sleep_for(milliseconds(500));
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
	viPrintf(m_vi, ":WAVeform:MODE RAW\n");
	viPrintf(m_vi, ":WAVeform:POINts 1000000\n");
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

void CMedicOnChipRigolMSO5074Dlg::Measure_and_save(const vector <unsigned int>& channels, unsigned int bucket_size, std::string raw_path,std::string mean_path, float offset) {
	ViByte* buf;		//unsigned char
	buf = new ViByte[1000000];
	ViUInt32 cnt = 1000000;
	ViUInt32  readcnt;
	char* temp;
	float Ts;
	float deltaV;
	int N;
	int tam;
	float* sinal;
	int aux = 0;
	int sinal_it = 0;
	float soma = 0;
	float media = 0;
	bool flag_time;

	std::vector<std::vector<float>> raw_signals(1000000, std::vector<float>(4));
	std::vector<std::vector<float>> mean_signals(1000000/bucket_size, std::vector<float>(4));
	for (int i = 0; i < 1000000; i++) {
		for (int j = 4; j < 4; j++) {
			raw_signals[i][j] = 0;
			if (i< 1000000 / bucket_size) {
				mean_signals[i][j] = 0;
			}
		}
	}/*
	for (auto& linha : raw_signals) {
		linha.assign(4, 0);
	}
	for (auto& linha : mean_signals) {
		linha.assign(4, 0);
	}*/

	//Seleciona o primeiro canal
	temp = new char[256];
	sprintf_s(temp, 256, ":WAVeform:SOURce CHANnel%d\n", 1);
	viPrintf(m_vi, temp);
	delete[] temp;

	//Determina a resolução horizontal
	//viPrintf(m_vi, ":WAVeform:XINCrement?\n");
	viPrintf(m_vi, ":ACQuire:SRATe?\n");
	viRead(m_vi, buf, cnt, &readcnt);
	temp = new char[readcnt];
	for (int i = 0; i < readcnt; i++)
		temp[i] = buf[i];
	Ts = 1/atof(temp);		//Período de amostragem
	/*
	std::string srate_path = "";
	for (int i = (mean_path.length() - 1); i > 0;i--) {	//Acha a ultima barra
		if (mean_path[i] ==  '/') {
			for (int j = 0; j < (i+1); j++) {	//Copia string até a barra, obtendo o path do resultado do teste
				srate_path[j] = mean_path[j];
			}
			break;
		}
	}
	srate_path += "srate.txt";
	std::ofstream arquivo_srate(srate_path);*/
	std::ofstream arquivo_srate(result_path+"srate.txt");
	arquivo_srate << temp << "\n";
	arquivo_srate.close();

	delete[] temp;

	//Itera sobre cada canal
	flag_time = false;
	for (int canal = 1; canal < 4; canal++) {
		auto iterador = std::find(channels.begin(), channels.end(), canal);
		//Só mede se o canal estiver selecionado no vetor channels
		if (iterador != channels.end()) {
			//Seleciona o canal
			temp = new char[256];
			sprintf_s(temp, 256, ":WAVeform:SOURce CHANnel%d\n", canal);
			viPrintf(m_vi, temp);
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

			sinal = new float[tam / bucket_size];
			sinal_it = 0;
			for (int i = 0; i < tam; i++) {

				raw_signals[i][canal] = ((buf[2 + N + i] - 127) * deltaV) - offset;

				if (!flag_time) {
					raw_signals[i][0] = Ts*i;
				}
				
				soma += ((buf[2 + N + i] - 127) * deltaV) - offset;
				if (aux == bucket_size-1) {
					media = soma / bucket_size;
					if (sinal_it < tam / bucket_size) {
						mean_signals[sinal_it][canal] = media;
						sinal[sinal_it] = media;

						if (flag_scale_set && !flag_scale_set_status && canal == 1) {
							if (sinal_it > 0.1*(tam / bucket_size) && sinal_it < 0.9 * (tam / bucket_size)) {	//Pega no meio do sinal para evitar outliers
								if (sinal[sinal_it] > max_vds) {
									max_vds = sinal[sinal_it];
								}
								if (sinal[sinal_it] < min_vds) {
									min_vds = sinal[sinal_it];
								}
							}
						}

						if (!flag_time) {
							mean_signals[sinal_it][0] = Ts * i;
						}
					}
					sinal_it++;
					aux = -1;	//O aux é incrementado logo que sai do if, então tem que colcoar -1 para ele zerar daqui 2 linhas
					soma = 0;
				}
				aux++;
			}

			m_pwndGraficoCanal[canal - 1].ajustaEscalas(tam * Ts, (127 * deltaV) - offset);
			m_pwndGraficoCanal[canal - 1].plotaGrafico(sinal, tam / bucket_size);
			delete[] sinal;
			flag_time = true;
		}
	}
	delete[] buf;

	if (!flag_scale_set) {
		//if (bucket_size == BUCKET_SIZE_FCP) {
		std::ofstream arquivo(raw_path, std::ios::binary);
		for (const auto& linha : raw_signals) {
			arquivo.write(reinterpret_cast<const char*>(linha.data()), linha.size() * sizeof(int));
		}
		arquivo.close();
		//}

		std::ofstream arquivo_mean(mean_path);
		arquivo_mean << cabecalho << "\n";
		for (const auto& linha : mean_signals) {
			for (size_t i = 0; i < linha.size(); i++) {
				arquivo_mean << linha[i];
				if (i < linha.size() - 1) {
					arquivo_mean << ";";
				}
			}
			arquivo_mean << "\n";
		}
		arquivo_mean.close();
	}
}

// Lê os dados do canal especificado e imprime no gráfico
void CMedicOnChipRigolMSO5074Dlg::leDadosCanal(unsigned int canal, unsigned int bucket_size, std::string bin_file_path)
{
	ViByte buf[1000000];		//unsigned char
	ViUInt32 cnt = 1000000;
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
	//sinal = new float[tam];
	/*
	const unsigned long long size = 1024ULL * 1024ULL;
	unsigned long long a[size];

	FILE* pFile;
	pFile = fopen("file.binary", "wb");
	for (unsigned long long j = 0; j < 1024; ++j) {
		//Some calculations to fill a[]
		fwrite(a, 1, size * sizeof(unsigned long long), pFile);
	}
	fclose(pFile);
	*/
	sinal = new float[tam/bucket_size];				// Aloca o buffer e preenche
	int aux = 0;
	int sinal_it = 0;
	float soma = 0;
	float media = 0;
	for (int i = 0; i < tam; i++) {
		soma += (buf[2 + N + i] - 127) * deltaV;
		if (aux == bucket_size - 1) {
			media = soma / bucket_size;
			if (sinal_it < tam / bucket_size) {
				sinal[sinal_it] = media;
			}
			sinal_it++;
			aux = 0;
			soma = 0;
		}
				
		//myfile << Ts * i << "," << sinal[i] << "\n";
		aux++;
		//sinal[i] = (buf[2 + N + i] - 127) * deltaV;
	}
	//myfile.close();

	m_pwndGraficoCanal[canal - 1].ajustaEscalas(tam * Ts, 127 * deltaV);
	m_pwndGraficoCanal[canal - 1].plotaGrafico(sinal, tam/bucket_size);
	//m_pwndGraficoCanal[canal - 1].plotaGrafico(sinal, tam);
	delete[] sinal;
}

bool SendCommand(std::string _command, bool check_response_loop, std::string check_command, std::string expected_msg)
{
	using namespace std::this_thread; // sleep_for, sleep_until
	using namespace std::chrono; // nanoseconds, system_clock, seconds

	ViSession defaultRM, vi;
	char buf[256] = { 0 };
	CString s, command;
	char stringTemp[256];
	char stringTemp_check[256];
	char* stringTemp_ptr;
	char* stringTemp_check_ptr;
	unsigned int loop_count = 0;

	ViChar buffer[VI_FIND_BUFLEN];
	ViRsrc matches = buffer;
	ViUInt32 nmatches;
	ViFindList list;

	viOpenDefaultRM(&defaultRM);
	//Acquire the USB resource of VISA
	viFindRsrc(defaultRM, "USB?*", &list, &nmatches, matches);
	viOpen(defaultRM, matches, VI_NULL, VI_NULL, &vi);

	sleep_for(milliseconds(20));
	
	command = _command.c_str();
	command += "\n";
	/*
	stringTemp_ptr = new char[static_cast<int>(_command.length())];

	for (int i = 0; i < _command.length(); i++)
		stringTemp_ptr[i] = _command[i];*/

	for (int i = 0; i < command.GetLength(); i++)
		stringTemp[i] = (char)command.GetAt(i);

	viPrintf(vi, stringTemp);

	sleep_for(milliseconds(20));
	//Send the command received

	if (check_response_loop) {
		command = check_command.c_str();;
		command += "\n";
		/*
		stringTemp_check_ptr = new char[command.GetLength()];

		for (int i = 0; i < command.GetLength(); i++)
			stringTemp_check_ptr[i] = (char)command.GetAt(i);*/

		for (int i = 0; i < command.GetLength(); i++)
			stringTemp_check[i] = (char)command.GetAt(i);

		viPrintf(vi, stringTemp_check);
		sleep_for(milliseconds(20));
		//Read the results
		viScanf(vi, "%t\n", &buf);
		sleep_for(milliseconds(20));


		while (buf != expected_msg + "\n") {
			viPrintf(vi, stringTemp);
			sleep_for(milliseconds(20));
			viPrintf(vi, stringTemp_check);
			sleep_for(milliseconds(20));
			//Read the results
			viScanf(vi, "%t\n", &buf);
			sleep_for(milliseconds(20));
			loop_count++;
			if (loop_count == COMM_LOOP_LIMIT) {
				AfxMessageBox(_T("Erro: Problema de comunicação com o osciloscópio. Reconecte a USB."));
				viClose(vi);
				sleep_for(milliseconds(20));
				viClose(defaultRM);
				sleep_for(milliseconds(20));
				viOpenDefaultRM(&defaultRM);
				sleep_for(milliseconds(20));
				//Acquire the USB resource of VISA
				viFindRsrc(defaultRM, "USB?*", &list, &nmatches, matches);
				sleep_for(milliseconds(20));
				viOpen(defaultRM, matches, VI_NULL, VI_NULL, &vi);
				sleep_for(milliseconds(20));
				loop_count = 0;
			}
		}
	}

	viClose(vi);
	viClose(defaultRM);

	return false;
}

std::string readOsciloscope(char _command[256])
{
	using namespace std::this_thread; // sleep_for, sleep_until
	using namespace std::chrono; // nanoseconds, system_clock, seconds

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

	sleep_for(milliseconds(20));
	//Send the command received
	strTemp = _command;
	strTemp += "\n";
	for (int i = 0; i < strTemp.GetLength(); i++)
		stringTemp[i] = (char)strTemp.GetAt(i);
	viPrintf(vi, stringTemp);
	sleep_for(milliseconds(20));
	//Read the results
	viScanf(vi, "%t\n", &buf);
	sleep_for(milliseconds(20));
	//viClose(vi);
	viClose(defaultRM);
	std::string aux = buf;
	return  aux;
};

/*
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
int get_CSV_numb_Lines(std::filesystem::path caminho) {

	int lines=0;
	string line;
	
		std::ifstream teste(caminho, ios::in);
		if (teste.is_open()) {
			while (!teste.eof()) {
				getline(teste, line);
				lines++;
			}
		}
		teste.close();
	
	line.clear();
	lines = lines - 1;
	return (lines);
}

int get_File_Lines(std::filesystem::path caminho) {
	vector<int> totalLines;
	int aux;
	for (const auto& file : std::filesystem::directory_iterator(caminho)) {
		std::ifstream teste(file.path(), ios::in);
		totalLines.push_back( get_CSV_numb_Lines( file.path() ) );
	}
	aux = totalLines[0];
	for (int i = 0; i < totalLines.size(); i++)
	{
		if (aux!=totalLines[i])
		{
			return 0;
		}

	}
	return aux;
}

void Files_Path_in_Directory(std::filesystem::path caminho)
{
	std::string add = "Danilo ";
	int i = 1;
	int mean = 0;
	int lines=get_File_Lines(caminho);
	//double buffer[1000][4] = {};

	auto buffer = new double[lines - 1][4];
	for (int i = 0; i < (lines-1); i++)
	{
		for (int j = 0; j < 4; j++) {
			buffer[i][j] = 0;
		}
	}
	for (const auto& file : std::filesystem::directory_iterator(caminho)) {

		
		std::ifstream teste(file.path(), ios::in);
		mean++;


		if (teste.is_open()) {
						
			vector<string> row;
			string line, word;
			word.clear();
			int iterator = 0;



			while (!teste.eof())
			{	
				
					row.clear();
					getline(teste, line);
					if (line == cabecalho) { continue; }
					else {
						for (int p = 0; p < line.length(); p++) {
							if (line[p] == ';' || (p == line.length() - 1)) {

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
							buffer[iterator][i] = stod(row[i]) + buffer[iterator][i];
						}
						word.clear();
						iterator++;
					}
			}
			i++;

		}

	}
	ofstream newfile((add + to_string(i) + ".csv"), ios::out | ios::app);
	
	for (int i = 0; i < (lines-1); i++)
	{
		for (int j = 0; j < 4; j++) {
			if(j==3){ 
				newfile << (buffer[i][j])/mean<<"\n"; 
				//newfile << lines << "\n";
			}
			else {
				newfile << buffer[i][j]/mean<<";";
				//newfile <<  lines<< ";";
			}
		}
	}
	delete[] buffer;
}
//Função que lê os arquivos .csv de um diretório, calcula a média das curvas
//associadas, escreve os dados resultantes em um novo arquivo .csv
//parâmetro: objeto path da biblioteca fylesistem que contém o endereço de onde
//devemos manipular os dados

void media_CSV_Osciloscopio(std::filesystem::path caminho) {

	int mean = 0;    //iterador p/ calcular o número de arquivos acessados
	int lines = get_File_Lines(caminho);    //armazena o número de linhas de um arquivo
	auto buffer = new double[lines - 1][csv_columns]; //buffer de dados da de dados dos arquivos;

	//(lines-1) Uma das linhas é o cabeçalho dos dados, "precisa-lá" exluir	
	//loop para inicializar a matriz com zeros em todos os indices
	for (int i = 0; i < (lines - 1); i++)
	{
		for (int j = 0; j < csv_columns; j++) {
			buffer[i][j] = 0;
		}
	}
	//loop que percorre todos arquivos de um diretório, acessa eles e acumula
	// a soma deles em um Buffer
	for (const auto& file : std::filesystem::directory_iterator(caminho)) {

		//acessar arquivo de um endereço no diretório
		std::ifstream arquivo(file.path(), ios::in);
		mean++;


		if (arquivo.is_open()) {

			vector<string> row;
			string line, word;
			word.clear();
			int iterator = 0;

			while (!arquivo.eof())
			{
				row.clear();
				getline(arquivo, line);
				if (line == cabecalho) { continue; }
				else {
					for (int p = 0; p < line.length(); p++) {
						if (line[p] == ';' || (p == line.length() - 1)) {

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
						buffer[iterator][i] = stod(row[i]) + buffer[iterator][i];
					}
					word.clear();
					iterator++;
				}
			}

		}
	}
	ofstream newfile("Media_Dados_ArquivosDiretorio.csv", ios::out | ios::app);
	for (int i = 0; i < (lines - 1); i++)
	{
		for (int j = 0; j < csv_columns; j++) {
			if (j == 3) {
				newfile << (buffer[i][j]) / mean << "\n";
			}
			else {
				newfile << buffer[i][j] / mean << ";";
			}
		}
	}
	delete[] buffer;

}
*/
void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButton9()
{
	// TODO: Add your control notification handler code here
	
	const std::filesystem::path pasta{ "Arquivos csv" };
	
	//media_CSV_Osciloscopio(pasta);
}

// Custom split() function (https://favtutor.com/blogs/split-string-cpp)
vector <string> customSplit2(string str, char separator) {
	int startIndex = 0, endIndex = 0;
	vector <string> strings;
	for (int i = 0; i <= str.size(); i++) {

		// If we reached the end of the word or the end of the input.
		if (str[i] == separator || i == str.size()) {
			endIndex = i;
			string temp;
			temp.append(str, startIndex, endIndex - startIndex);
			strings.push_back(temp);
			startIndex = endIndex + 1;
		}
	}
	return strings;
}

bool more_recent(vector<std::string> date, vector<std::string> ref) {
	/*
	std::string month = date[0];
	std::string month_ref = ref[0];

	std::map<std::string, int> months;
	months["Jan"] = 1;
	months["Feb"] = 2;
	months["Mar"] = 3;
	months["Apr"] = 4;
	months["May"] = 5;
	months["Jun"] = 6;
	months["Jul"] = 7;
	months["Aug"] = 8;
	months["Sep"] = 9;
	months["Oct"] = 10;
	months["Nov"] = 11;
	months["Dec"] = 12;

	int year = stoi(date[5]);
	int year_ref = stoi(ref[5]);
	int day = stoi(date[1]);
	int day_ref = stoi(ref[1]);*/
	int hour = stoi(date[0]);
	int hour_ref = stoi(ref[0]);
	int min = stoi(date[1]);
	int min_ref = stoi(ref[1]);
	int sec = stoi(date[2]);
	int sec_ref = stoi(ref[2]);
	/*
	if (year > year_ref) {
		return true;
	}
	if (months[month] > months[month_ref]) {
		return true;
	}
	if (day > day_ref) {
		return true;
	}*/
	if (hour > hour_ref) {
		return true;
	}
	if (min > min_ref) {
		return true;
	}
	if (sec > sec_ref) {
		return true;
	}

	return false;
}
/*
void build_log_message(std::string msg) {
	
	if (log_lines.size() == 0) {
		log_lines.push_back("");
	}
	if (msg != log_lines[0]) {
		log_lines.insert(log_lines.begin(), msg);
	}
	if (log_lines.size() > LOG_SIZE) {
		log_lines.pop_back();
	}
	tester.log_string = "";
	for (int line_num = 0; line_num < log_lines.size(); line_num++) {
		tester.log_string += log_lines[line_num] + "\r\n";
	}
	return;
	tester.log_string = "";
}*/

void CMedicOnChipRigolMSO5074Dlg::reset_square_wave()
{
	using namespace std::this_thread; // sleep_for, sleep_until
	using namespace std::chrono; // nanoseconds, system_clock, seconds

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

		command = ":SOURce1:FUNCtion SQU";
		command += "\n";

		for (int i = 0; i < command.GetLength(); i++)
			stringTemp[i] = (char)command.GetAt(i);

		viPrintf(vi, stringTemp);

		sleep_for(milliseconds(800));

		viClose(vi);
		viClose(defaultRM);

		viOpenDefaultRM(&defaultRM);
		//Acquire the USB resource of VISA
		viFindRsrc(defaultRM, "USB?*", &list, &nmatches, matches);
		viOpen(defaultRM, matches, VI_NULL, VI_NULL, &vi);

		command = ":SOURce1:FUNCtion RAMP";
		command += "\n";

		for (int i = 0; i < command.GetLength(); i++)
			stringTemp[i] = (char)command.GetAt(i);

		viPrintf(vi, stringTemp);

		viClose(vi);
		viClose(defaultRM);

		sleep_for(milliseconds(800));

		viOpenDefaultRM(&defaultRM);
		//Acquire the USB resource of VISA
		viFindRsrc(defaultRM, "USB?*", &list, &nmatches, matches);
		viOpen(defaultRM, matches, VI_NULL, VI_NULL, &vi);

		command = ":SOURce2:FUNCtion SQU";
		command += "\n";

		for (int i = 0; i < command.GetLength(); i++)
			stringTemp[i] = (char)command.GetAt(i);

		viPrintf(vi, stringTemp);

		sleep_for(milliseconds(800));

		viClose(vi);
		viClose(defaultRM);

		viOpenDefaultRM(&defaultRM);
		//Acquire the USB resource of VISA
		viFindRsrc(defaultRM, "USB?*", &list, &nmatches, matches);
		viOpen(defaultRM, matches, VI_NULL, VI_NULL, &vi);

		command = ":SOURce2:FUNCtion RAMP";
		command += "\n";

		for (int i = 0; i < command.GetLength(); i++)
			stringTemp[i] = (char)command.GetAt(i);

		viPrintf(vi, stringTemp);

		viClose(vi);
		viClose(defaultRM);

		return;
}


void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonFcsAlt()
{
	if (m_SNPrompt.DoModal() == IDOK)
	{
			/*
			std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
			time_t tt;
			tt = std::chrono::system_clock::to_time_t(today);
			char str[27] = { 0 };

			ctime_s(str, sizeof str, &tt);
			std::string date_str = "";

			for (int i = 0; i < 20; i++) {
				if (str[i + 4] == ' ' || str[i + 4] == ':') {
					if (str[i + 5] != ' ' && str[i + 5] != ':') date_str += '-';
				}
				else {
					date_str += str[i + 4];
				}
			}

			CString results_path = database_path + m_SNPrompt.m_Serial_Number + "/FCS/" + date_str.c_str() + "/";
			result_path = CStringA(results_path);
			if (!fs::exists(result_path)) {
				fs::create_directories(result_path);
			}
			*/

			//Verifica se VG_MIN e VG_MAX já foram obtidos para esse SN e pega eles
		vector<string> splitted_values_aux;
		vector<string> splitted_values_ref;
		CString fcc_path = database_path + m_SNPrompt.m_Serial_Number + "/FCC/";	//CString por causa do SN
		std::string fcc_path_str = CStringA(fcc_path);	//Jogando na result_path, que é string, porque a função de baixo só aceita string
		//std::filesystem::path caminho{ fcc_path_str };

		if (fs::exists(fcc_path_str)) {
			//Encontrar o teste mais recente
			std::string most_recent = "";
			bool first = true;
			for (auto& p : std::filesystem::recursive_directory_iterator(fcc_path_str)) {
				if (p.is_directory()) {
					if (first) {
						first = false;
						most_recent = p.path().string();
						splitted_values_ref = customSplit2(most_recent, '-');
					}
					else {
						splitted_values_aux = customSplit2(p.path().string(), '-');
						if (more_recent(splitted_values_aux, splitted_values_ref)) {
							most_recent = p.path().string();
							splitted_values_ref = customSplit2(most_recent, '-');
						}
					}
				}
			}

			std::ofstream myfile;
			myfile.open("ALTERNATIVOFCS.csv");
			myfile << most_recent << "\n";
			myfile.close();

			system("python.exe Alternativo_FCS.py");

		}
		else {
			UpdateData(TRUE);
			m_receive = "Esse chip ainda não foi testado no FCC";
			UpdateData(FALSE);
			return;
		}

		m_SNPrompt.m_Serial_Number = "";
	}
}


void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonFcpAlt()
{
	//vg_index = vg_index;
	using namespace std::this_thread; // sleep_for, sleep_until
	using namespace std::chrono; // nanoseconds, system_clock, seconds
	bool aq_status = false;
	char buff[20] = { 0 };
	float delay_time = 1000;

	if (!m_bAquisicaoAtiva)
	{
		int status_sn_prompt = 0;
		if (!flag_run_all && !flag_scale_set_status && !new_fcp_flag) {
			status_sn_prompt = m_SNPrompt.DoModal();
			if (status_sn_prompt == IDOK) {
				new_fcp_flag = true;
			}
			promptFCP = m_SNPrompt.m_Serial_Number;
		}
		if (status_sn_prompt == IDOK || flag_run_all || flag_scale_set_status || new_fcp_flag)
		{
			if (!flag_scale_set_status) {
				aq_status = iniciarAquisicao();
				m_bAquisicaoAtiva = false;	//Manipulando flag para entrar denovo aqui depois de ter ajustado a escala
			}
			else {
				aq_status = true;
				m_bAquisicaoAtiva = true;
			}
			if (aq_status) {
				if (!flag_scale_set_status) {

					GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(FALSE);
					GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
					GetDlgItem(IDC_BUTTON_ADQUIRIR)->EnableWindow(FALSE);
					GetDlgItem(IDC_BUTTON_FCC)->EnableWindow(FALSE);
					GetDlgItem(IDC_BUTTON_FCS)->EnableWindow(FALSE);
					GetDlgItem(IDC_BUTTON_FCP)->EnableWindow(FALSE);
					GetDlgItem(IDC_BUTTON_FCS_ALT)->EnableWindow(FALSE);

					GetDlgItem(IDC_BUTTON_FCP_Alt)->SetWindowText(_T("Encerrar"));

					if (flag_run_all) {
						GetDlgItem(IDC_BUTTON_FCP_Alt)->EnableWindow(FALSE);
					}
					else {
						GetDlgItem(IDC_BUTTON_FCP_Alt)->EnableWindow(TRUE);
						GetDlgItem(IDC_BUTTON_RUNALL)->EnableWindow(FALSE);
					}

					if (Freq_Iterator == 0) {
						results_fcp = tester.get_fcp_parameters();
					}

					flag_fcp = true;
					flag_scale_set = true;

					UpdateData(TRUE);
					m_receive = "";
					UpdateData(FALSE);

					num_bursts = results_fcp.bursts;

					if (Freq_Iterator == 0) {	//P/ multiplas frequencia
						/*
						std::chrono::system_clock::time_point today = std::chrono::system_clock::now();
						time_t tt;
						tt = std::chrono::system_clock::to_time_t(today);
						char str[27] = { 0 };

						ctime_s(str, sizeof str, &tt);
						std::string date_str = "";

						for (int i = 0; i < 20; i++) {
							if (str[i + 4] == ' ' || str[i + 4] == ':') {
								if (str[i + 5] != ' ' && str[i + 5] != ':') date_str += '-';
							}
							else {
								date_str += str[i + 4];
							}
						}*/

						promptFCP = m_SNPrompt.m_Serial_Number;

						std::time_t currentTime;
						std::time(&currentTime);
						std::tm timeInfo;
						localtime_s(&timeInfo, &currentTime);

						std::string date_str = "";
						date_str += std::to_string(timeInfo.tm_year + 1900);
						date_str += "/" + std::to_string(timeInfo.tm_mon + 1);
						date_str += "/" + std::to_string(timeInfo.tm_mday);
						date_str += "/";

						std::string time_str = "";
						int hour = timeInfo.tm_hour;
						int minute = timeInfo.tm_min;
						int second = timeInfo.tm_sec;

						if (hour <= 9) {
							time_str += "0" + std::to_string(hour) + "-";
						}
						else {
							time_str += std::to_string(hour) + "-";
						}

						if (minute <= 9) {
							time_str += "0" + std::to_string(minute) + "-";
						}
						else {
							time_str += std::to_string(minute) + "-";
						}

						if (second <= 9) {
							time_str += "0" + std::to_string(second);
						}
						else {
							time_str += std::to_string(second);
						}

						//Verifica se VG_MIN e VG_MAX já foram obtidos para esse SN e pega eles
						vector<string> aux;
						vector<string> splitted_values_aux;
						vector<string> splitted_values_ref;
						CString fcs_path = database_path + date_str.c_str() + m_SNPrompt.m_Serial_Number + "/FCS/";	//CString por causa do SN

						/*
						CString fcs_path;
						if (Freq_Iterator != 0)
						{
							fcs_path.Append(database_path + promptFCP + "/FCS/");
							m_SNPrompt.m_Serial_Number.~CStringT();
							m_SNPrompt.m_Serial_Number.Append(promptFCP);

						}
						else
						{
							fcs_path.Append(database_path + m_SNPrompt.m_Serial_Number + "/FCS/");	//CString por causa do SN
						}*/
					
						std::string vg_values_path = CStringA(fcs_path);	//Jogando na result_path, que é string, porque a função de baixo só aceita string
						std::filesystem::path caminho{ vg_values_path };

						if (fs::exists(vg_values_path)) {
							//Encontrar o teste mais recente
							std::string most_recent = "";
							bool first = true;
							for (auto& p : std::filesystem::recursive_directory_iterator(vg_values_path)) {
								if (p.is_directory()) {
									if (first) {
										first = false;
										most_recent = p.path().string();
										aux = customSplit2(most_recent, '/');
										splitted_values_ref = customSplit2(aux[aux.size()-1], '-');
									}
									else {
										aux = customSplit2(p.path().string(), '/');
										splitted_values_aux = customSplit2(aux[aux.size() - 1], '-');
										if (more_recent(splitted_values_aux, splitted_values_ref)) {
											most_recent = p.path().string();
											aux = customSplit2(most_recent, '/');
											splitted_values_ref = customSplit2(aux[aux.size() - 1], '-');
										}
									}
								}
							}

							vg_values_path = most_recent + "/vg_values.ini";

							//Verificar se no teste mais recente existem os valores de VG
							if (fs::exists(vg_values_path)) {

								mINI::INIFile file(vg_values_path);
								mINI::INIStructure ini;
								file.read(ini);
								float min = stof(ini.get("VG").get("MIN"));
								float max = stof(ini.get("VG").get("MAX"));

								results_fcp.vg_source_params.v_pp = max - min;
								trigger_parameters.level = max - results_fcp.vg_source_params.v_pp / 2;
								results_fcp.vg_source_params.v_offset = max - results_fcp.vg_source_params.v_pp / 2;
								results_fcp.vg_meas_params.volts_div = max / 4;

							}
							else {
								UpdateData(TRUE);
								m_receive = "O ultimo FCS desse chip não calculou os valores de Vg_max e Vg_min para realizar o FCP";
								UpdateData(FALSE);

								encerrarAquisicao();
								for (int i = 0; i < m_numCanais; i++) {
									m_pwndGraficoCanal[i].ShowWindow(SW_HIDE);
									m_pwndGraficoCanal[i].limpaGrafico();
								}

								GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(TRUE);
								GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
								GetDlgItem(IDC_BUTTON_ADQUIRIR)->EnableWindow(TRUE);
								GetDlgItem(IDC_BUTTON_FCC)->EnableWindow(TRUE);
								GetDlgItem(IDC_BUTTON_FCS)->EnableWindow(TRUE);
								GetDlgItem(IDC_BUTTON_FCS_ALT)->EnableWindow(TRUE);
								GetDlgItem(IDC_BUTTON_FCP)->EnableWindow(TRUE);
								GetDlgItem(IDC_BUTTON_RUNALL)->EnableWindow(TRUE);
								GetDlgItem(IDC_BUTTON_FCP_Alt)->SetWindowText(_T("FCP"));
								GetDlgItem(IDC_BUTTON_FCP_Alt)->EnableWindow(TRUE);
								if (flag_run_all) {
									flag_fcp = false;
									OnBnClickedButtonRunall();
								}
								return;
							}
						}
						else {
							UpdateData(TRUE);
							m_receive = "Esse chip ainda não foi testado no FCS";
							UpdateData(FALSE);

							encerrarAquisicao();
							for (int i = 0; i < m_numCanais; i++) {
								m_pwndGraficoCanal[i].ShowWindow(SW_HIDE);
								m_pwndGraficoCanal[i].limpaGrafico();
							}
							GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(TRUE);
							GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
							GetDlgItem(IDC_BUTTON_ADQUIRIR)->EnableWindow(TRUE);
							GetDlgItem(IDC_BUTTON_FCC)->EnableWindow(TRUE);
							GetDlgItem(IDC_BUTTON_FCS)->EnableWindow(TRUE);
							GetDlgItem(IDC_BUTTON_FCS_ALT)->EnableWindow(TRUE);
							GetDlgItem(IDC_BUTTON_FCP)->EnableWindow(TRUE);
							GetDlgItem(IDC_BUTTON_RUNALL)->EnableWindow(TRUE);
							GetDlgItem(IDC_BUTTON_FCP_Alt)->SetWindowText(_T("FCP Alt."));
							GetDlgItem(IDC_BUTTON_FCP_Alt)->EnableWindow(TRUE);
							if (flag_run_all) {
								flag_fcp = false;
								OnBnClickedButtonRunall();
							}
							return;
						}

						CString results_path = database_path + date_str.c_str() + m_SNPrompt.m_Serial_Number + "/FCP/" + time_str.c_str() + "/";
						//CString results_path = database_path + m_SNPrompt.m_Serial_Number + "/FCP/" + date_str.c_str() + "/";
						result_path = CStringA(results_path);
						
						FCPpath = result_path;

						if (!fs::exists(result_path)) {
							fs::create_directories(result_path);
						}

						CopyConfig();
						std::ifstream inputFile(vg_values_path, std::ios::binary); // Abre o arquivo de origem em modo binário
						std::ofstream outputFile(result_path + "vg_values.ini", std::ios::binary); // Abre o arquivo de destino em modo binário
						outputFile << inputFile.rdbuf(); // Copia o conteúdo do arquivo de origem para o arquivo de destino
					}
					//Mostra os gráficos
					for (int i = 0; i < m_numCanais; i++)
						m_pwndGraficoCanal[i].ShowWindow(SW_SHOW);

					//Set time scale
					//tester.set_t_scale(results_fcp.t_scale);
					switch (Freq_Iterator) {
					case 0:
						tester.set_t_scale(results_fcp.Low_t_scale);
						results_fcp.vg_source_params.freq = results_fcp.vg_source_params.LOWfreq;
						break;
					case 1:
						tester.set_t_scale(results_fcp.High_t_scale);
						results_fcp.vg_source_params.freq = results_fcp.vg_source_params.HIGHfreq;
						break;
					default:
						tester.set_t_scale(results_fcp.t_scale);
						break;
					}
					//Set measurement channels
					vds_meas.write_parameters_to_osc(results_fcp.vds_meas_params);
					current_meas.write_parameters_to_osc(results_fcp.current_meas_params);
					vg_meas.write_parameters_to_osc(results_fcp.vg_meas_params);
					//Set trigger
					trigger_parameters.source = "CHAN3";
					tester.send_trigger_parameters(trigger_parameters);
					//tester.triggerMode_FCP();//qlq coisa vai nessa função e muda trigger para Normal
					vds_source.write_parameters_to_osc(results_fcp.vds_source_params);
					vg_source.write_parameters_to_osc(results_fcp.vg_source_params);
					
					string_to_char_array(sys_commands.RUN, &buff[0]);
					SendCommand(buff);

				//Medida rápida para definir escala vertical
					results_fcp.AquireType = "HRES";
					tester.type_Aquire(results_fcp.AquireType);
					delay_time = 1000;
				}
				else {
					//set Acquire
					//sleep_for(milliseconds(500));

					results_fcp.vds_meas_params.offset = -(max_vds + min_vds)/2;
					results_fcp.vds_meas_params.volts_div = (max_vds)*1.1/8;
					vds_meas.write_parameters_to_osc(results_fcp.vds_meas_params);

					string_to_char_array(sys_commands.RUN, &buff[0]);
					SendCommand(buff);

					results_fcp.AquireType = "AVER";
					tester.type_Aquire(results_fcp.AquireType);
					sleep_for(milliseconds(500));
					tester.aquire_Numb_averages(results_fcp);
					delay_time = 1000 * results_fcp.AquireAverages * results_fcp.vg_source_params.cycles / results_fcp.vg_source_params.freq;
					if (Freq_Iterator == 0) {
						delay_time = delay_time * 1.2;
					}
					else {
						delay_time = delay_time * 5;
					}
				}
				/*
				//Set High Res and 1M points
				string_to_char_array(sys_commands.MDEPTH, &buff[0]);
				SendCommand(buff);
				string_to_char_array(sys_commands.MDEPTH, &buff[0]);
				SendCommand(buff);
				*/

				string_to_char_array(sys_commands.ALIAS_OFF, &buff[0]);
				SendCommand(buff);
				for (int i = 0; i < 20; i++) {
					buff[i] = 0;
				}

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
				//current_meas.on();
				vg_meas.on();

				//string_to_char_array(sys_commands.TFORCE, &buff[0]);
				//SendCommand(buff);

				vds_source.start(1);
				vg_source.start(2);
				if (flag_scale_set_status) {
					sleep_for(milliseconds(100));
					string_to_char_array(sys_commands.RUN + "\n", &buff[0]);
					SendCommand(buff);
				}
				else {
					sleep_for(milliseconds(3000));
					string_to_char_array(sys_commands.SINGLE + "\n", &buff[0]);
					SendCommand(buff);
				}

				//Ativa o timer
				SetTimer(ID_TIMER_FCP, delay_time, NULL);
			}
		}
	}
	else
	{
		KillTimer(ID_TIMER_FCP);

		vg_source.stop(2);
		//vds_source.stop(1);
		burst_count = 0;
		//stopped = false;
		//started = false;
		m_SNPrompt.m_Serial_Number = "";
		//flag_fcp = false;

		char buff[10] = { 0 };
		string_to_char_array(sys_commands.RUN+"\n", &buff[0]);
		SendCommand(buff);
		sleep_for(milliseconds(500));
		results_fcp.AquireType = "HRES";
		tester.type_Aquire(results_fcp.AquireType);

		encerrarAquisicao();
		for (int i = 0; i < m_numCanais; i++) {
			m_pwndGraficoCanal[i].ShowWindow(SW_HIDE);
			m_pwndGraficoCanal[i].limpaGrafico();
		}

		GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ADQUIRIR)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCC)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCS_ALT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_RUNALL)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCP_Alt)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCP_Alt)->SetWindowText(_T("FCP"));

		if (Freq_Iterator < 1) {
			Freq_Iterator++;
			OnBnClickedNewfcp();

		}
		else {
			Freq_Iterator = 0;
			new_fcp_flag = false;
			promptFCP = CString("");
			FCPpath = "";
			flag_fcp = false;
			if (flag_run_all) {
				OnBnClickedButtonRunall();
			}
		}

	}
}

void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonRunall()
{
	if (!flag_run_all) {
		if (m_SNPrompt.DoModal() == IDOK) {
			GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_ADQUIRIR)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_FCC)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_FCS)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_FCS_ALT)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_FCP)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_FCP_Alt)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_RUNALL)->SetWindowText(_T("Encerrar"));

			flag_run_all = true;
			OnBnClickedButtonFCC();
		}
	}
	else {
		flag_run_all = false;
		if (flag_fcc) OnBnClickedButtonFCC();
		else if (flag_fcs) OnBnClickedButtonFCS();
		else if (flag_fcp) OnBnClickedButtonFcpAlt();

		GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ADQUIRIR)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCC)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCS)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCS_ALT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCP_Alt)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_RUNALL)->SetWindowText(_T("Run All"));
	}
}

void CMedicOnChipRigolMSO5074Dlg::OnBnClickedNewfcp()
{
	OnBnClickedButtonFcpAlt();
}

void CMedicOnChipRigolMSO5074Dlg::CopyConfig()
{
	std::ifstream inputFile("config.ini", std::ios::binary); // Abre o arquivo de origem em modo binário
	std::ofstream outputFile(result_path+"config.ini", std::ios::binary); // Abre o arquivo de destino em modo binário
	outputFile << inputFile.rdbuf(); // Copia o conteúdo do arquivo de origem para o arquivo de destino
	return;
}