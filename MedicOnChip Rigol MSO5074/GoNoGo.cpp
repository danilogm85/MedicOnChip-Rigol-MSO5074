// GoNoGo.cpp : arquivo de implementação
//

#include "pch.h"
#include "MedicOnChip Rigol MSO5074.h"
#include "afxdialogex.h"
#include "GoNoGo.h"


// caixa de diálogo de GoNoGo

IMPLEMENT_DYNAMIC(GoNoGo, CDialogEx)

GoNoGo::GoNoGo(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_GoNoGo, pParent)
{

}

GoNoGo::~GoNoGo()
{
}

void GoNoGo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(GoNoGo, CDialogEx)
END_MESSAGE_MAP()


// manipuladores de mensagens de GoNoGo
