// SN_PROMPT.cpp : arquivo de implementação
//

#include "pch.h"
#include "MedicOnChip Rigol MSO5074.h"
#include "afxdialogex.h"
#include "SN_PROMPT.h"


// caixa de diálogo de SN_PROMPT

IMPLEMENT_DYNAMIC(SN_PROMPT, CDialogEx)

SN_PROMPT::SN_PROMPT(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SN_PROMPT, pParent)
	, m_Serial_Number(_T(""))
{

}

SN_PROMPT::~SN_PROMPT()
{
}

void SN_PROMPT::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SN, m_Serial_Number);
}


BEGIN_MESSAGE_MAP(SN_PROMPT, CDialogEx)
END_MESSAGE_MAP()


// manipuladores de mensagens de SN_PROMPT
