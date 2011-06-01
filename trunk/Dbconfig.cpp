// Dbconfig.cpp : 实现文件
//

#include "stdafx.h"
#include "FP_Attendance.h"
#include "Dbconfig.h"
#include "ultility.h"


// CDbconfig 对话框

IMPLEMENT_DYNAMIC(CDbconfig, CDialog)

CDbconfig::CDbconfig(mdldb::Connector& conn, CWnd* pParent /*=NULL*/)
	: CDialog(CDbconfig::IDD, pParent),
	m_conn(conn)
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

	SetDlgItemText(IDC_DBHOST, L"172.16.81.156");
	SetDlgItemText(IDC_DBPORT, L"3306");
	SetDlgItemText(IDC_DBUSER, L"attendance");
	SetDlgItemText(IDC_DBPASSWD, L"attendance");

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

	m_conn.set_dbhost(CStringToString(dbHost), CStringToString(dbPort));
	m_conn.set_dbuser(CStringToString(dbUser));
	m_conn.set_dbpasswd(CStringToString(dbPasswd));

	OnOK();
}
