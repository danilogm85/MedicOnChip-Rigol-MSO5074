#pragma once
#include "afxdialogex.h"


// caixa de diálogo de GoNoGo_Confirm

class GoNoGo_Confirm : public CDialogEx
{
	DECLARE_DYNAMIC(GoNoGo_Confirm)

public:
	GoNoGo_Confirm(CWnd* pParent = nullptr);   // construtor padrão
	virtual ~GoNoGo_Confirm();

// Janela de Dados
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_GoNoGo_Confirm };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Suporte DDX/DDV

	DECLARE_MESSAGE_MAP()
};
