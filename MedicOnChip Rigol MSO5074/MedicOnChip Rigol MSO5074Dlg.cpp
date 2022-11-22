
// MedicOnChip Rigol MSO5074Dlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "MedicOnChip Rigol MSO5074.h"
#include "MedicOnChip Rigol MSO5074Dlg.h"
#include "afxdialogex.h"
//#define _USE_MATH_DEFINES
//#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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
	, m_numCanais(2)
	, m_bAquisicaoAtiva(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


CMedicOnChipRigolMSO5074Dlg::~CMedicOnChipRigolMSO5074Dlg()
{
	if (m_bAquisicaoAtiva)
		encerrarAquisicao();
}

void CMedicOnChipRigolMSO5074Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_combox);
	DDX_Text(pDX, IDC_EDIT_RCVD_MSG, m_receive);
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
		m_wndGraficoCanal[i].Create(NULL, NULL, WS_CHILD | WS_CLIPSIBLINGS, CRect(0, 0, 0, 0), this, NULL, NULL);

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
		m_wndGraficoCanal[i].MoveWindow(
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
			GetDlgItem(IDC_BUTTON_ADQUIRIR)->SetWindowText(_T("Encerrar"));
			for (int i=0; i<m_numCanais; i++)
				m_wndGraficoCanal[i].ShowWindow(SW_SHOW);
			SetTimer(ID_TIMER_ADQUIRIR, 550, NULL);
		}
	}
	else
	{
		KillTimer(ID_TIMER_ADQUIRIR);
		encerrarAquisicao();
		for (int i = 0; i < m_numCanais; i++) {
			m_wndGraficoCanal[i].ShowWindow(SW_HIDE);
			m_wndGraficoCanal[i].limpaGrafico();
		}
		GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCC)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ADQUIRIR)->SetWindowText(_T("Adquirir"));
	}
}


// Trata o botão "FCC"
void CMedicOnChipRigolMSO5074Dlg::OnBnClickedButtonFCC()
{
	// TODO: Add your control notification handler code here
/*
	if (!m_bAquisicaoAtiva)
	{
		if (m_FCCParametersDlg.DoModal() == IDOK) {
			if (iniciarAquisicao()) {
				GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_ADQUIRIR)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_FCC)->SetWindowText(_T("Encerrar"));
				for (int i = 0; i < m_numCanais; i++)
					m_wndGraficoCanal[i].ShowWindow(SW_SHOW);


				//Alterar a aquisição para modo single shot
				//Inicializar número de pontos (=Ng)
				//Inicializar período de amostragem (=DeltaTg2)
				//Abrir o arquivo de dados
				//Inicializar Vg = Vgi
				//Sleep DeltaTg1
				//Inicializar Vsd = Vsdi
				//Sleep DeltaTsd1


				SetTimer(ID_TIMER_FCC, 550, NULL);
			}
		}
	}
	else
	{
		KillTimer(ID_TIMER_FCC);
		encerrarAquisicao();
		for (int i = 0; i < m_numCanais; i++) {
			m_wndGraficoCanal[i].ShowWindow(SW_HIDE);
			m_wndGraficoCanal[i].limpaGrafico();
		}
		GetDlgItem(IDC_BUTTON_SEND_AND_READ)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SEND)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_ADQUIRIR)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_FCC)->SetWindowText(_T("FCC"));
	}
*/
}


// Mensagens dos timers
void CMedicOnChipRigolMSO5074Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent) {
	case ID_TIMER_ADQUIRIR:
		for(int i=1; i<=m_numCanais; i++)
			leDadosCanal(i);
		break;

	case ID_TIMER_FCC:

		//Disparar uma aquisição

		//Medir canal 1 (=Vg)
		//Calcular média (Vg_barra) e armazenar
		//Medir canal 2 (=Ig)
		//Calcular média (Ig_barra) e armazenar

		//Ajustar Vsd += DeltaVsd
		//Se Vsd <= Vsdf break

		//Ajustar Vsd = Vsdi
		//Ajustar Vg += DeltaVg
		//Se Vg <= Vgf break

		//Desativar o timer
		//Desativar as fontes
		//Encerrar a aquisição
		//Fechar o arquivo de dados

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

	sinal = new float[tam];				// Aloca o buffer e preenche
	for (int i = 0; i < tam; i++)
		sinal[i] = (buf[2 + N + i] - 127) * deltaV;
	m_wndGraficoCanal[canal - 1].ajustaEscalas(tam * Ts, 127 * deltaV);
	m_wndGraficoCanal[canal - 1].plotaGrafico(sinal, tam);
	delete[] sinal;
}