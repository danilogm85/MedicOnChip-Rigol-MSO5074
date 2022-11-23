// GraficoDeTempo.cpp : arquivo de implementação
//

#include "pch.h" 
#include "JanelaDoGrafico.h"


// CJanelaDoGrafico

IMPLEMENT_DYNAMIC(CJanelaDoGrafico, CWnd)

CJanelaDoGrafico::CJanelaDoGrafico(	int tipoGrafico, int numRotulosHorz, int numRotulosVert)
: m_numRotulosHorz(numRotulosHorz)
, m_numRotulosVert(numRotulosVert)
, m_multVertical(1.0)
, m_tipoDeGrafico(tipoGrafico)
, m_Xmax(1000)
, m_Ymax(5)
, m_vertScrollPos(MAX_SCROLL_POS / 2)
{
	switch (m_tipoDeGrafico){
	case TIPO_GRAFICO_DE_TEMPO:
		m_pWndGrafico = new CGraficoDeTempo(numRotulosVert, numRotulosHorz);
		break;
	default:
		AfxMessageBox(_T("ERRO no construtor de CJanelaDoGrafico: tipo de gráfico inválido."));
		return;
	}

	m_pRotulosHorizontais = new CStatic[m_numRotulosHorz];
	m_pRotulosVerticais = new CStatic[m_numRotulosVert];
}

CJanelaDoGrafico::~CJanelaDoGrafico()
{
	delete[] m_pRotulosVerticais;
	delete[] m_pRotulosHorizontais;
	delete[] m_pWndGrafico;
}


BEGIN_MESSAGE_MAP(CJanelaDoGrafico, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()



// Manipuladores de mensagens de CJanelaDoGrafico


BOOL CJanelaDoGrafico::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Adicione seu código especializado aqui e/ou chame a classe base
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	//cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), NULL);

	return TRUE;
}


int CJanelaDoGrafico::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Adicione seu código de criação especializada aqui

	//Cria o gráfico de tempo
	if (!m_pWndGrafico->Create(NULL, NULL, WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS | WS_VISIBLE, CRect(0, 0, 0, 0), this, NULL, NULL))
	{
		TRACE0("Falha ao criar o gráfico interno na classe CJanelaDoGrafico\n");
		return -1;
	}

	//Cria os rótulos horizontais
	m_pRotulosHorizontais[0].Create(NULL, ES_LEFT | WS_VISIBLE, CRect(0, 0, 0, 0), this, NULL);
	m_pRotulosHorizontais[m_numRotulosHorz - 1].Create(NULL, ES_RIGHT | WS_VISIBLE, CRect(0, 0, 0, 0), this, NULL);
	for (int i = 1; i < m_numRotulosHorz - 1; i++){
		m_pRotulosHorizontais[i].Create(NULL, ES_CENTER | WS_VISIBLE, CRect(0, 0, 0, 0), this, NULL);
	}

	//Cria os rótulos verticais
	for (int i = 0; i < m_numRotulosVert; i++)
		m_pRotulosVerticais[i].Create(NULL, ES_RIGHT | WS_VISIBLE, CRect(0, 0, 0, 0), this, NULL);

	//Cria o rótulo do eixo X
	m_rotuloEixoX.Create(NULL, ES_CENTER | WS_VISIBLE, CRect(0, 0, 0, 0), this, NULL);

	//Cria o pincel para pintar o fundo dos rótulos
	CClientDC dc(this);
	m_pincelFundo.CreateSolidBrush(dc.GetBkColor());

	//Cria a barra de rolagem vertical
	m_vertScrollBar.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | SBS_VERT, CRect(0, 0, 0, 0), this, NULL);
	m_vertScrollBar.SetScrollRange(0, MAX_SCROLL_POS, FALSE);
	m_vertScrollBar.SetScrollPos(m_vertScrollPos, FALSE);

	return 0;
}


void CJanelaDoGrafico::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Adicione seu código de manipulador de mensagens aqui
			
	float delta = (float)cx / 100;
	
	int largRotulo = 8 * delta;
	int altRotulo = 3 * delta;

	int largBarraVert = 2 * delta;

	int largGrafico = cx - largBarraVert - largRotulo - delta;
	int altGrafico = cy - 2 * altRotulo - delta;

	//Ajusta a posição do gráfico
	m_pWndGrafico->MoveWindow(largBarraVert + largRotulo, delta, largGrafico, altGrafico);

	//Ajusta as posições dos rótulos verticais
	int i;

	m_pRotulosVerticais[0].MoveWindow(largBarraVert, delta, largRotulo, altRotulo);
	m_pRotulosVerticais[m_numRotulosVert - 1].MoveWindow(largBarraVert, delta + altGrafico - altRotulo, largRotulo, altRotulo);
	
	for (i = 1; i < m_numRotulosVert - 1; i++)
		m_pRotulosVerticais[i].MoveWindow(largBarraVert, 
		delta + altGrafico / (m_numRotulosVert - 1) * i - altRotulo / 2, largRotulo, altRotulo);

	//Ajusta as posições dos rótulos horizontais
	m_pRotulosHorizontais[0].MoveWindow(largBarraVert + largRotulo,	cy - 2 * altRotulo, largRotulo, altRotulo);
	m_pRotulosHorizontais[m_numRotulosHorz - 1].MoveWindow(cx - largRotulo,	cy - 2 * altRotulo, largRotulo, altRotulo);

	for (i = 1; i < m_numRotulosHorz - 1; i++)
		m_pRotulosHorizontais[i].MoveWindow(largBarraVert + largRotulo + i * largGrafico / (m_numRotulosHorz - 1) - largRotulo / 2,
		cy - 2 * altRotulo, largRotulo, altRotulo);

	//Ajusta a posição do rótulo do eixo X
	m_rotuloEixoX.MoveWindow(largBarraVert + largGrafico / 2, cy - altRotulo, 2 * largRotulo, altRotulo);

	//Ajusta o tamanho da barra vertical e a altura do elevador 
	m_vertScrollBar.MoveWindow(0, delta, largBarraVert, altGrafico);

	SCROLLINFO sbInfo;
	sbInfo.fMask = SIF_PAGE;
	sbInfo.nPage = 2 * cy / 100 ;

	m_vertScrollBar.SetScrollInfo(&sbInfo);
}



void CJanelaDoGrafico::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Adicione seu código de manipulador de mensagens aqui
	// Não chamar CWnd::OnPaint() para mensagens de pintura

	CString str;
	int i;
	float tamIntervalo;

	switch (m_tipoDeGrafico){
	case TIPO_GRAFICO_DE_TEMPO:
		//Pinta os rótulos verticais
		for (i = 0; i < m_numRotulosVert; i++){
			str.Format(_T("%0.2f"), 1 / m_multVertical * (2 - i) * m_Ymax / 2);
			m_pRotulosVerticais[i].SetWindowTextW(str);
		}
		
		//Pinta os rótulos horizontais
		if (m_Xmax < 1e-6) {
			tamIntervalo = 1e9 * m_Xmax / (m_numRotulosHorz - 1);
			str = _T("Tempo (ns)");
		}
		else if(m_Xmax < 1e-3) {
			tamIntervalo = 1e6 * m_Xmax / (m_numRotulosHorz - 1);
			str = _T("Tempo (us)");
		}
		else if (m_Xmax < 1) {
			tamIntervalo = 1e3 * m_Xmax / (m_numRotulosHorz - 1);
			str = _T("Tempo (ms)");
		}
		else {
			tamIntervalo = m_Xmax / (m_numRotulosHorz - 1);
			str = _T("Tempo (s)");
		}

		m_rotuloEixoX.SetWindowTextW(str);		
		for (i = 0; i < m_numRotulosHorz; i++){
			str.Format(_T("%0.1f"), tamIntervalo * i);
			m_pRotulosHorizontais[i].SetWindowTextW(str);
		}
		break;
	default:
		break;
	}
}



HBRUSH CJanelaDoGrafico::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CWnd::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Altere todos os atributos DC aqui

	if (nCtlColor == CTLCOLOR_STATIC){
		pDC->SelectStockObject(ANSI_FIXED_FONT);
		pDC->SetBkMode(TRANSPARENT);

		return (HBRUSH)m_pincelFundo;
	}

	// TODO:  Retorne um pincel diferente se o padrão não for o desejado
	return hbr;
}


void CJanelaDoGrafico::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Adicione seu código de manipulador de mensagens e/ou chame o padrão

	switch (nSBCode){
	case SB_LINEUP:
		if (m_vertScrollPos > 0)
			m_vertScrollPos--;
		break;
	case SB_LINEDOWN:
		if (m_vertScrollPos < MAX_SCROLL_POS)
			m_vertScrollPos++;
		break;
	case SB_THUMBTRACK:
		m_vertScrollPos = nPos;
		break;
	case SB_PAGEUP:
		if ((m_vertScrollPos - 10) > 0)
			m_vertScrollPos -= 10;
		else
			m_vertScrollPos = 0;
		break;
	case SB_PAGEDOWN:
		if ((m_vertScrollPos + 10) < MAX_SCROLL_POS)
			m_vertScrollPos += 10;
		else
			m_vertScrollPos = MAX_SCROLL_POS;
		break;
	}

	pScrollBar->SetScrollPos(m_vertScrollPos, FALSE);

	float max = MAX_SCROLL_POS / 2.0;

	m_multVertical = pow((float)10.0, (max - m_vertScrollPos) / max);

	static_cast<CGraficoDeTempo*>(m_pWndGrafico)->ajustaMultVertical(m_multVertical);

	Invalidate();

	//CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CJanelaDoGrafico::plotaGrafico(const float* pDados, const int tam)
{
	static_cast<CGraficoDeTempo*>(m_pWndGrafico)->plotaGrafico(pDados, tam);
}


void CJanelaDoGrafico::limpaGrafico()
{
	if (m_tipoDeGrafico == TIPO_GRAFICO_DE_TEMPO){
		static_cast<CGraficoDeTempo*>(m_pWndGrafico)->limpaGrafico();
	}
}



BOOL CJanelaDoGrafico::ajustaEscalas(float Xmax, float Ymax)
{
	if ((Xmax != m_Xmax) || (Ymax != m_Ymax)) {
		m_Ymax = Ymax;

		if (m_tipoDeGrafico == TIPO_GRAFICO_DE_TEMPO)
			m_Xmax = Xmax;
		else
			m_Xmax = 360;

		static_cast<CGraficoDeTempo*>(m_pWndGrafico)->ajustaEscalaVertical(Ymax);

		Invalidate();
	}

	return TRUE;
}






