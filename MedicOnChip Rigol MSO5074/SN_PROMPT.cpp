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
	ON_EN_CHANGE(IDC_EDIT_SN, &SN_PROMPT::OnEnChangeEditSn)
	ON_BN_CLICKED(IDOK, &SN_PROMPT::OnBnClickedOk)
END_MESSAGE_MAP()


// manipuladores de mensagens de SN_PROMPT


void SN_PROMPT::OnEnChangeEditSn()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}


void SN_PROMPT::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}
