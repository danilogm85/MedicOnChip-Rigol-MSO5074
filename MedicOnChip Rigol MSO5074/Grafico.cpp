//#include "stdafx.h"
#include "pch.h"
#include "Grafico.h"


IMPLEMENT_DYNAMIC(CGrafico, CWnd)

CGrafico::CGrafico(int numLinhasHorz, int numLinhasVert)
: m_tamVetor(0)
, m_pVetorDadosLocal(NULL)
, m_multVertical(1.0)
{
	m_Vmax = 5;
	m_numEixosHorz = numLinhasHorz - 2;
	m_numEixosVert = numLinhasVert - 2;
}


CGrafico::~CGrafico()
{
	if (m_pVetorDadosLocal)
		delete[] m_pVetorDadosLocal;
}


BEGIN_MESSAGE_MAP(CGrafico, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CGrafico::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Adicione seu código especializado aqui e/ou chame a classe base
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	//cs.dwExStyle |= WS_EX_CLIENTEDGE;
	//cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), NULL);

	return TRUE;
}


void CGrafico::plotaGrafico(const float* dados, const int tam)
{
	if (tam <= 0)
		return;

	if (m_tamVetor != tam){
		m_tamVetor = tam;
		if (m_pVetorDadosLocal)
			delete[] m_pVetorDadosLocal;
		m_pVetorDadosLocal = new float[m_tamVetor];
	}

	for (int i = 0; i < m_tamVetor; i++)
		m_pVetorDadosLocal[i] = dados[i];

	Invalidate();
}


void CGrafico::limpaGrafico()
{
	if (m_pVetorDadosLocal)
		delete[] m_pVetorDadosLocal;

	m_pVetorDadosLocal = NULL;
	m_tamVetor = 0;
}


BOOL CGrafico::ajustaEscalaVertical(float vMaximo)
{
	m_Vmax = vMaximo;

	return 0;
}


void CGrafico::ajustaMultVertical(float mult)
{
	m_multVertical = mult;
}


int CGrafico::getTamVetorDeDados()
{
	return m_tamVetor;
}


void CGrafico::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Adicione seu código de manipulador de mensagens aqui
	// Não chamar CWnd::OnPaint() para mensagens de pintura
}


float CGrafico::getAt(const int pos)
{
	if ((m_pVetorDadosLocal) && (pos >= 0) && (pos < m_tamVetor))
		return m_pVetorDadosLocal[pos];
	else
		return FLT_MIN;
}
