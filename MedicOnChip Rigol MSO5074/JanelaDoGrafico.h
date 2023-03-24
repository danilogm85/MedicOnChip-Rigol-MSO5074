#pragma once

#include "GraficoDeTempo.h"

#define MAX_SCROLL_POS 100

#define TIPO_GRAFICO_DE_TEMPO	1


// CJanelaDoGrafico

class CJanelaDoGrafico : public CWnd
{
	DECLARE_DYNAMIC(CJanelaDoGrafico)

public:
	CJanelaDoGrafico(int = TIPO_GRAFICO_DE_TEMPO, int = 7, int = 5);
	virtual ~CJanelaDoGrafico();

	void plotaGrafico(const float* pDados, const int tam);
	void limpaGrafico();
	BOOL ajustaEscalas(float xMax, float yMax);
	int getTamVetorDeDados();
	float getAt(const int);

private:
	CWnd* m_pWndGrafico;
	CStatic* m_pRotulosVerticais;
	CStatic* m_pRotulosHorizontais;
	int m_numRotulosVert;
	int m_numRotulosHorz;
	CStatic m_rotuloEixoX;
	CStatic m_rotuloEixoY;
	CScrollBar m_vertScrollBar;
	int	m_vertScrollPos;
	float m_Ymax;
	float m_Xmax;
	float m_multVertical;
	int m_tipoDeGrafico;

public:
	CBrush m_pincelFundo;

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	DECLARE_MESSAGE_MAP()
};


