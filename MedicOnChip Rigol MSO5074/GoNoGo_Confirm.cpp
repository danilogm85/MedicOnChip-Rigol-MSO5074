// GoNoGo_Confirm.cpp : arquivo de implementação
//

#include "pch.h"
#include "MedicOnChip Rigol MSO5074.h"
#include "afxdialogex.h"
#include "GoNoGo_Confirm.h"


// caixa de diálogo de GoNoGo_Confirm

IMPLEMENT_DYNAMIC(GoNoGo_Confirm, CDialogEx)

GoNoGo_Confirm::GoNoGo_Confirm(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_GoNoGo_Confirm, pParent)
{

}

GoNoGo_Confirm::~GoNoGo_Confirm()
{
}

void GoNoGo_Confirm::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(GoNoGo_Confirm, CDialogEx)
END_MESSAGE_MAP()


// manipuladores de mensagens de GoNoGo_Confirm
