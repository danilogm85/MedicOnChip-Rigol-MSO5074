#pragma once
#include "afxdialogex.h"
#include <string>

// caixa de diálogo de SN_PROMPT

class SN_PROMPT : public CDialogEx
{
	DECLARE_DYNAMIC(SN_PROMPT)

public:
	SN_PROMPT(CWnd* pParent = nullptr);   // construtor padrão
	virtual ~SN_PROMPT();

// Janela de Dados
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SN_PROMPT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Suporte DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	CString m_Serial_Number;
	afx_msg void OnEnChangeEditSn();
	afx_msg void OnBnClickedOk();
};
