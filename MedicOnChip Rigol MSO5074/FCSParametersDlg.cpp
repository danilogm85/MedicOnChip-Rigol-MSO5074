// FCSParametersDlg.cpp : implementation file
//

#include "pch.h"
#include "MedicOnChip Rigol MSO5074.h"
#include "afxdialogex.h"
#include "FCSParametersDlg.h"


// CFCSParametersDlg dialog

IMPLEMENT_DYNAMIC(CFCSParametersDlg, CDialogEx)

CFCSParametersDlg::CFCSParametersDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_FCS, pParent)
	, m_Vgmin (0)
	, m_Vgmax (0)
	, m_Vgfreq (0)
	, m_Vsd (0)
	, m_NAverage (1)
{

}

CFCSParametersDlg::~CFCSParametersDlg()
{
}

void CFCSParametersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_FCS_Vgmin, m_Vgmin);
	DDV_MinMaxFloat(pDX, m_Vgmin, -1.5, 1.5);
	DDX_Text(pDX, IDC_EDIT_FCS_Vgmax, m_Vgmax);
	DDV_MinMaxFloat(pDX, m_Vgmax, m_Vgmin, 1.5);
	DDX_Text(pDX, IDC_EDIT_FCS_Vgfreq, m_Vgfreq);
	DDV_MinMaxFloat(pDX, m_Vgfreq, 0, 100);
	DDX_Text(pDX, IDC_EDIT_FCS_Vsd, m_Vsd);
	DDV_MinMaxFloat(pDX, m_Vsd, -0.1, 0.1);
	DDX_Text(pDX, IDC_EDIT_FCS_NAve, m_NAverage);
	DDV_MinMaxUInt(pDX, m_NAverage, 1, 20);
}


BEGIN_MESSAGE_MAP(CFCSParametersDlg, CDialogEx)
END_MESSAGE_MAP()



