#pragma once

#include "Grafico.h"

// CGraficoDeTempo

class CGraficoDeTempo : public CGrafico
{
	DECLARE_DYNAMIC(CGraficoDeTempo)

public:
	CGraficoDeTempo(int, int);
	virtual ~CGraficoDeTempo();

protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnPaint();	
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


