#pragma once
#include "afxwin.h"

class CGrafico : public CWnd
{
	DECLARE_DYNAMIC(CGrafico)

protected:
	float* m_pVetorDadosLocal;
	int m_tamVetor;
	float m_Vmax;
	float m_multVertical;
	int m_numEixosVert;
	int m_numEixosHorz;

public:
	CGrafico(int, int);
	virtual ~CGrafico();

	void plotaGrafico(const float*, const int);
	void limpaGrafico();
	BOOL ajustaEscalaVertical(float);
	void ajustaMultVertical(float);
	int getTamVetorDeDados();
	float getAt(const int);

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()	
};

