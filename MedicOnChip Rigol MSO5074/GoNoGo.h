#pragma once
#include "afxdialogex.h"


// caixa de diálogo de GoNoGo

class GoNoGo : public CDialogEx
{
	DECLARE_DYNAMIC(GoNoGo)

public:
	GoNoGo(CWnd* pParent = nullptr);   // construtor padrão
	virtual ~GoNoGo();

// Janela de Dados
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_GoNoGo };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Suporte DDX/DDV

	DECLARE_MESSAGE_MAP()
};
