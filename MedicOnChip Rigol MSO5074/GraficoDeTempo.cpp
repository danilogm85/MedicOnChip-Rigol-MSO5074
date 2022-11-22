// Grafico.cpp : arquivo de implementação
//

//#include "stdafx.h"
#include "pch.h"
#include "GraficoDeTempo.h"

// CGraficoDeTempo

IMPLEMENT_DYNAMIC(CGraficoDeTempo, CGrafico)

CGraficoDeTempo::CGraficoDeTempo(int numLinhasHorz, int numLinhasVert)
: CGrafico(numLinhasHorz, numLinhasVert)
{
	
}

CGraficoDeTempo::~CGraficoDeTempo()
{
	
}


BEGIN_MESSAGE_MAP(CGraficoDeTempo, CGrafico)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// Manipuladores de mensagens de CGraficoDeTempo


BOOL CGraficoDeTempo::PreCreateWindow(CREATESTRUCT& cs)
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


void CGraficoDeTempo::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Adicione seu código de manipulador de mensagens aqui
	// Não chamar CWnd::OnPaint() para mensagens de pintura

	CBitmap bitmap, *pOldBitmap;
	CDC dcMem;
	CRect retang;
	int i;
	CPen caneta;

		//Obtém as dimensões da janela
	GetClientRect(&retang);

		//Cria o bitmap
	bitmap.CreateCompatibleBitmap(&dc, retang.Width(), retang.Height());

		//Cria o contexto para a memória
	dcMem.CreateCompatibleDC(&dc);

		//Seleciona o bitmap no contexto da memória
	pOldBitmap = dcMem.SelectObject(&bitmap);

		//Pinta o fundo do bitmap
	dcMem.SelectStockObject(WHITE_BRUSH);
	dcMem.SelectStockObject(WHITE_PEN);
	dcMem.Rectangle(&retang);

		//Desenha as linhas verticais
	caneta.CreatePen(PS_DOT, 0, RGB(180, 180, 180));
	dcMem.SelectObject(&caneta);

	for (i = 1; i <= m_numEixosVert; i++){								
		dcMem.MoveTo((int)i*retang.Width() / (m_numEixosVert + 1), 0);
		dcMem.LineTo((int)i*retang.Width() / (m_numEixosVert + 1), retang.Height());
	}

		//Desenha as linhas horizontais
	for (i = 1; i <= m_numEixosHorz; i++){
		dcMem.MoveTo(0, i*retang.Height() / (m_numEixosHorz + 1));
		dcMem.LineTo(retang.Width(), i*retang.Height() / (m_numEixosHorz + 1));
	}

	dcMem.SelectStockObject(BLACK_PEN);
	caneta.DeleteObject();

		//Desenha a linha do zero
	caneta.CreatePen(PS_SOLID, 0, RGB(180, 180, 180));
	dcMem.SelectObject(&caneta);

	dcMem.MoveTo(0, retang.Height() / 2);					
	dcMem.LineTo(retang.Width(), retang.Height() / 2);

	dcMem.SelectStockObject(BLACK_PEN);
	caneta.DeleteObject();

		//Plota o gráfico
	if (m_tamVetor > 0)
	{
		caneta.CreatePen(PS_SOLID, 0, RGB(200, 0, 0));

		dcMem.SelectObject(&caneta);

		float resHorizontal = (float)retang.Width() / m_tamVetor;
		float deslVertical = (float) retang.Height() / 2;

		for (i = 0; i < (m_tamVetor - 1); i++){
			dcMem.MoveTo(i * resHorizontal,
				m_multVertical * (-m_pVetorDadosLocal[i] / m_Vmax) * deslVertical + deslVertical);
			dcMem.LineTo((i + 1) * resHorizontal,
				m_multVertical * (-m_pVetorDadosLocal[i + 1] / m_Vmax) * deslVertical + deslVertical);
		}

		dcMem.SelectStockObject(BLACK_PEN);

		caneta.DeleteObject();
	}
		//Transfere o bitmap para a tela
	dc.BitBlt(0, 0, retang.Width(), retang.Height(), &dcMem, 0, 0, SRCCOPY);
		//Libera o bitmap
	dcMem.SelectObject(pOldBitmap);
}




BOOL CGraficoDeTempo::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Adicione seu código de manipulador de mensagens e/ou chame o padrão

	//return CGrafico::OnEraseBkgnd(pDC);
	return 0;
}
