// Dbconfig.cpp : 实现文件
//

#include "stdafx.h"
#include "FP_Attendance.h"
#include "ConfigDlg.h"
#include "ultility.h"


// CConfigDlg 对话框

IMPLEMENT_DYNAMIC(CConfigDlg, CDialog)

CConfigDlg::CConfigDlg(Config& cfg, CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent),
	  m_cfg(cfg)
{
}

CConfigDlg::~CConfigDlg()
{
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetDlgItemText(IDC_DBHOST, stringToCString(m_cfg.m_dbhost));
	SetDlgItemText(IDC_DBPORT, stringToCString(m_cfg.m_dbport));
	SetDlgItemText(IDC_DBUSER, stringToCString(m_cfg.m_dbuser));
	SetDlgItemText(IDC_DBPASSWD, stringToCString(m_cfg.m_dbpasswd));

	return TRUE;
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CConfigDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CConfigDlg 消息处理程序

void CConfigDlg::OnBnClickedOk()
{
	CString dbHost, dbPort, dbUser, dbPasswd;
	GetDlgItemText(IDC_DBHOST, dbHost);
	GetDlgItemText(IDC_DBPORT, dbPort);
	GetDlgItemText(IDC_DBUSER, dbUser);
	GetDlgItemText(IDC_DBPASSWD, dbPasswd);

	m_cfg.m_dbhost = CStringToString(dbHost);
	m_cfg.m_dbport = CStringToString(dbPort);
	m_cfg.m_dbuser = CStringToString(dbUser);
	m_cfg.m_dbpasswd = CStringToString(dbPasswd);
	m_cfg.save();

	OnOK();
}
