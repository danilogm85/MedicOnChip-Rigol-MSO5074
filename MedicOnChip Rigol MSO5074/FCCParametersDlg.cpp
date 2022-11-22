// FCCParametersDlg.cpp : implementation file
//

#include "pch.h"
#include "MedicOnChip Rigol MSO5074.h"
#include "afxdialogex.h"
#include "FCCParametersDlg.h"


// FCCParametersDlg dialog

IMPLEMENT_DYNAMIC(FCCParametersDlg, CDialogEx)

FCCParametersDlg::FCCParametersDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_FCC, pParent)
	, m_Vgi (0)
	, m_Vgf (0)
	, m_Vsdi (0)
	, m_Vsdf (0)
	, m_DeltaVg (0)
	, m_DeltaVsd (0)
	, m_DeltaTg1 (0)
	, m_DeltaTg2 (0)
	, m_DeltaTsd1 (0)
	, m_DeltaTsd2 (0)
	, m_Ng (0)
	, m_Nsd (0)
{

}

FCCParametersDlg::~FCCParametersDlg()
{
}

void FCCParametersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_Vgi, m_Vgi);
	DDV_MinMaxFloat(pDX, m_Vgi, -1.5, 1.5);
	DDX_Text(pDX, IDC_EDIT_Vgf, m_Vgf);
	DDV_MinMaxFloat(pDX, m_Vgf, -1.5, 1.5);
	DDX_Text(pDX, IDC_EDIT_Vsdi, m_Vsdi);
	DDV_MinMaxFloat(pDX, m_Vsdi, -0.1, 0.1);
	DDX_Text(pDX, IDC_EDIT_Vsdf, m_Vsdf);
	DDV_MinMaxFloat(pDX, m_Vsdf, -0.1, 0.1);
	DDX_Text(pDX, IDC_EDIT_DeltaVg, m_DeltaVg);
	DDV_MinMaxFloat(pDX, m_DeltaVg, 0, 1.5);
	DDX_Text(pDX, IDC_EDIT_DeltaVsd, m_DeltaVsd);
	DDV_MinMaxFloat(pDX, m_DeltaVsd, 0, 0.1);
	DDX_Text(pDX, IDC_EDIT_DeltaTg1, m_DeltaTg1);
	DDV_MinMaxFloat(pDX, m_DeltaTg1, 0, 1);
	DDX_Text(pDX, IDC_EDIT_DeltaTg2, m_DeltaTg2);
	DDV_MinMaxFloat(pDX, m_DeltaTg2, 0, 1);
	DDX_Text(pDX, IDC_EDIT_DeltaTsd1, m_DeltaTsd1);
	DDV_MinMaxFloat(pDX, m_DeltaTsd1, 0, 1);
	DDX_Text(pDX, IDC_EDIT_DeltaTsd2, m_DeltaTsd2);
	DDV_MinMaxFloat(pDX, m_DeltaTsd2, 0, 1);
	DDX_Text(pDX, IDC_EDIT_Ng, m_Ng);
	DDV_MinMaxUInt(pDX, m_Ng, 0, 1000);
	DDX_Text(pDX, IDC_EDIT_Nsd, m_Nsd);
	DDV_MinMaxUInt(pDX, m_Nsd, 0, 1000);
}


BEGIN_MESSAGE_MAP(FCCParametersDlg, CDialogEx)
END_MESSAGE_MAP()


// CFCCParametersDlg message handlers
