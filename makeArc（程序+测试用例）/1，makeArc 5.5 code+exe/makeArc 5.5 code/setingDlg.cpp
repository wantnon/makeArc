// setingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GLmfc.h"
#include "setingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CsetingDlg dialog


CsetingDlg::CsetingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CsetingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CsetingDlg)
	m_thOfRad123 = 0/*-1*/;
	m_check1 = TRUE/*FALSE*/;
	m_thOfRad45 = 0/*-1*/;
	m_thOfRad678 = 0/*-1*/;
	//}}AFX_DATA_INIT
}


void CsetingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CsetingDlg)
	DDX_Radio(pDX, IDC_RADIO1, m_thOfRad123);
	DDX_Check(pDX, IDC_CHECK1, m_check1);
	DDX_Radio(pDX, IDC_RADIO4, m_thOfRad45);
	DDX_Radio(pDX, IDC_RADIO6, m_thOfRad678);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CsetingDlg, CDialog)
	//{{AFX_MSG_MAP(CsetingDlg)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_CHECK1, OnCheck1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CsetingDlg message handlers

void CsetingDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	//恢复默认值
	extern bool ignoreRidTree_baseOnStepMark;
	extern bool ignoreRidTree_baseOnCornerMark;
	extern const bool ignoreRidTree_baseOnStepMark_default;
	extern const bool ignoreRidTree_baseOnCornerMark_default;
	extern bool doRegulation;
	extern const bool doRegulation_default;
	extern bool useRealRoof;
	extern const bool useRealRoof_default;
	extern int regulationDegree;
	extern const int regulationDegree_default;
	ignoreRidTree_baseOnStepMark=ignoreRidTree_baseOnStepMark_default;
	ignoreRidTree_baseOnCornerMark=ignoreRidTree_baseOnCornerMark_default;
	doRegulation=doRegulation_default;
	useRealRoof=useRealRoof_default;
	regulationDegree=regulationDegree_default;
	//更新界面
	this->m_thOfRad123=0;
	this->m_check1=doRegulation;
	this->m_thOfRad45=0;
	this->m_thOfRad678=0;
	this->GetDlgItem(IDC_RADIO6)->EnableWindow(true);
	this->GetDlgItem(IDC_RADIO7)->EnableWindow(true);
	this->GetDlgItem(IDC_RADIO8)->EnableWindow(true);
	UpdateData(false);
}

void CsetingDlg::OnCheck1() 
{
	// TODO: Add your control notification handler code here
	UpdateData(true);
	if((this->m_check1)==false){
		this->GetDlgItem(IDC_RADIO6)->EnableWindow(false);
		this->GetDlgItem(IDC_RADIO7)->EnableWindow(false);
		this->GetDlgItem(IDC_RADIO8)->EnableWindow(false);
	}else{
		this->GetDlgItem(IDC_RADIO6)->EnableWindow(true);
		this->GetDlgItem(IDC_RADIO7)->EnableWindow(true);
		this->GetDlgItem(IDC_RADIO8)->EnableWindow(true);
	
	}
	UpdateData(false);
	
}

BOOL CsetingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//OnInitDialog在每次DoModal时调用
	//MessageBox("hi");
	UpdateData(true);
	if((this->m_check1)==false){
		this->GetDlgItem(IDC_RADIO6)->EnableWindow(false);
		this->GetDlgItem(IDC_RADIO7)->EnableWindow(false);
		this->GetDlgItem(IDC_RADIO8)->EnableWindow(false);
	}else{
		this->GetDlgItem(IDC_RADIO6)->EnableWindow(true);
		this->GetDlgItem(IDC_RADIO7)->EnableWindow(true);
		this->GetDlgItem(IDC_RADIO8)->EnableWindow(true);
	
	}
	UpdateData(false);


	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
