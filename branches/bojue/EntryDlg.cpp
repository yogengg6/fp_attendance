/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author       : Yusuke(Qiuchengxuan@gmail.com)
 * Author       : Bojue(zembojue@gmail.com)
 * Date	        : 2011-5-11 20:29
 */
// EntryDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FP_Attendance.h"
#include "EntryDlg.h"
#include "RegisterDlg.h"
#include "AttendanceDlg.h"

// CEntryDlg 对话框

IMPLEMENT_DYNAMIC(CEntryDlg, CDialog)

WCHAR CEntryDlg::m_convWchBuf[200];

CEntryDlg::CEntryDlg(mdldb::Connector& conn, CWnd* pParent /*=NULL*/)
	:	CDialog(CEntryDlg::IDD, pParent), m_conn(conn),
		m_courseComboBox(NULL),
		m_sessionComboBox(NULL)
{
	DPFPInit();
	if (FT_OK == FX_init()) {
		if (FT_OK != MC_init())
			MessageBox(L"初始化指纹比对库失败。", L"错误", MB_OK | MB_ICONSTOP);
	} else {
		MessageBox(L"初始化指纹特征提取库失败。", L"错误", MB_OK | MB_ICONSTOP);
	}
}

CEntryDlg::~CEntryDlg()
{
	MC_terminate();
	FX_terminate();
	DPFPTerm();
}

void CEntryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CEntryDlg::OnInitDialog()
{
	if (! m_conn.connected())
		return FALSE;

	m_courseComboBox  = (CComboBox*) GetDlgItem(IDC_COURSE);
	m_sessionComboBox = (CComboBox*) GetDlgItem(IDC_SESSION);

	try {
		m_courseInfo = m_conn.get_all_course();
	} catch (mdldb::MDLDB_Exception& e) {
		MessageBox(CString(e.what()), L"数据库连接");
	}

	SetDlgItemText(IDC_COURSE, L"请选择课程");

	for (vector<mdldb::CourseInfo>::iterator it = m_courseInfo.begin(); it != m_courseInfo.end(); ++it) {
		MultiByteToWideChar(CP_UTF8, NULL, it->fullname.c_str(), -1, m_convWchBuf, it->fullname.length());
		m_courseComboBox->AddString(CString(m_convWchBuf));
	}

	return TRUE;

}

BEGIN_MESSAGE_MAP(CEntryDlg, CDialog)
	ON_BN_CLICKED(ID_NEXT, &CEntryDlg::OnBnClickedNext)
	ON_BN_CLICKED(ID_EXIT, &CEntryDlg::OnBnClickedExit)
	ON_CBN_SELENDOK(IDC_COURSE, &CEntryDlg::OnCbnSelendokCourse)
	ON_CBN_SELENDOK(IDC_SESSION, &CEntryDlg::OnCbnSelendokSession)
	ON_BN_CLICKED(IDC_ATTENDANT, &CEntryDlg::OnBnClickedAttendant)
	ON_BN_CLICKED(IDC_REGISTER, &CEntryDlg::OnBnClickedRegister)
	ON_EN_CHANGE(IDC_EDIT1, &CEntryDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CEntryDlg 消息处理程序

void CEntryDlg::OnCbnSelendokCourse()
{
	int index = m_courseComboBox->GetCurSel();
	m_conn.associate_course(m_courseInfo[index].id);//加载课程信息

	m_sessionComboBox->Clear();           //课程信息加载后清空会话选项为下面向该控件中加信息做准备

	try {
		m_session = m_conn.get_session_discription(  );
	} catch (mdldb::MDLDB_Exception& e) {
		MessageBox(CString(e.what()), L"数据库连接");
	}

	for (vector<string>::iterator it = m_session.begin(); it != m_session.end(); ++it) {
		MultiByteToWideChar(CP_UTF8, NULL, it->c_str(), -1, m_convWchBuf, it->length());
		m_sessionComboBox->AddString(CString(m_convWchBuf));

	}
}

void CEntryDlg::OnCbnSelendokSession()
{
	int index = m_sessionComboBox->GetCurSel();//加载会话信息
	m_conn.associate_session(m_session[index]);
}

void CEntryDlg::OnBnClickedNext()
{
	switch (GetCheckedRadioButton(IDC_REGISTER, IDC_ATTENDANT)) {
	case IDC_REGISTER:
		{
			CRegisterDlg regDlg(m_conn);
			this->ShowWindow(SW_HIDE);
			if (regDlg.DoModal() == ID_EXIT)
				this->EndDialog(ID_EXIT);
			else
				this->ShowWindow(SW_SHOW);
			break;
		}
	case IDC_ATTENDANT:
		{
			CAttendanceDlg attdDlg(m_conn);
			this->ShowWindow(SW_HIDE);
			if (attdDlg.DoModal() == ID_EXIT)
				this->EndDialog(ID_EXIT);
			else
				this->ShowWindow(SW_SHOW);
			break;
			break;
		}
	default:
			break;
	}
}

void CEntryDlg::OnBnClickedExit()
{
	this->EndDialog(ID_EXIT);
}
void CEntryDlg::OnBnClickedAttendant()
{
	m_sessionComboBox->EnableWindow(TRUE);
	m_courseComboBox->EnableWindow(TRUE);
}

void CEntryDlg::OnBnClickedRegister()
{
	m_sessionComboBox->EnableWindow(FALSE);
	m_courseComboBox->EnableWindow(FALSE);
}

void CEntryDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}
