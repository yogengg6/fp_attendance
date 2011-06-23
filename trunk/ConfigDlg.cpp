// Dbconfig.cpp : 实现文件
//

#include "stdafx.h"
#include "FP_Attendance.h"
#include "ConfigDlg.h"
#include "ultility.h"


// CDbconfig 对话框

IMPLEMENT_DYNAMIC(CDbconfig, CDialog)

CDbconfig::CDbconfig(mdldb::Mdldb& mdl, CWnd* pParent /*=NULL*/)
	: CDialog(CDbconfig::IDD, pParent),
	m_mdl(mdl)
{
}

CDbconfig::~CDbconfig()
{
}

void CDbconfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CDbconfig::OnInitDialog()
{
	CDialog::OnInitDialog();

	cfg.load();

	SetDlgItemText(IDC_DBHOST, stringToCString(cfg.m_dbhost));
	SetDlgItemText(IDC_DBPORT, stringToCString(cfg.m_dbport));
	SetDlgItemText(IDC_DBUSER, stringToCString(cfg.m_dbuser));
	SetDlgItemText(IDC_DBPASSWD, stringToCString(cfg.m_dbpasswd));

	return TRUE;
}


BEGIN_MESSAGE_MAP(CDbconfig, CDialog)
	ON_BN_CLICKED(IDOK, &CDbconfig::OnBnClickedOk)
END_MESSAGE_MAP()


// CDbconfig 消息处理程序

void CDbconfig::OnBnClickedOk()
{
	CString dbHost, dbPort, dbUser, dbPasswd;
	GetDlgItemText(IDC_DBHOST, dbHost);
	GetDlgItemText(IDC_DBPORT, dbPort);
	GetDlgItemText(IDC_DBUSER, dbUser);
	GetDlgItemText(IDC_DBPASSWD, dbPasswd);

	cfg.m_dbhost = CStringToString(dbHost);
	cfg.m_dbport = CStringToString(dbPort);
	cfg.m_dbuser = CStringToString(dbPort);
	cfg.m_dbpasswd = CStringToString(dbPasswd);

	m_mdl.set_dbhost(cfg.m_dbhost, cfg.m_dbport);
	m_mdl.set_dbuser(cfg.m_dbuser);
	m_mdl.set_dbpasswd(cfg.m_dbpasswd);

	OnOK();
}
