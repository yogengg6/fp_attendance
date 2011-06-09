/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author       : Yusuke(Qiuchengxuan@gmail.com)
 * Date	        : 2011-5-11 20:29
 */
// EntryDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FP_Attendance.h"
#include "ultility.h"
#include "EntryDlg.h"
#include "RegisterDlg.h"
#include "AttendanceDlg.h"

// CEntryDlg 对话框

IMPLEMENT_DYNAMIC(CEntryDlg, CDialog)

CEntryDlg::CEntryDlg(mdldb::Mdldb& mdl, CWnd* pParent /*=NULL*/)
	:	CDialog(CEntryDlg::IDD, pParent), m_mdl(mdl),
		m_nextButton(NULL),
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
	CDialog::OnInitDialog();

	if (! m_mdl.connected())
		return FALSE;

	m_nextButton	  = (CButton *)	 GetDlgItem(ID_NEXT);
	m_courseComboBox  = (CComboBox*) GetDlgItem(IDC_COURSE);
	m_sessionComboBox = (CComboBox*) GetDlgItem(IDC_SESSION);
	m_static_course	  = (CStatic *)	 GetDlgItem(IDC_ENTRY_STATIC_COURSE);
	m_static_session  = (CStatic *)  GetDlgItem(IDC_ENTRY_STATIC_SESSION);

	try {
		m_courseInfo = m_mdl.get_all_course();
	} catch (mdldb::MDLDB_Exception& e) {
		MessageBox(CString(e.what()), L"数据库连接");
	}

	SetDlgItemText(IDC_COURSE, L"请选择课程");

	for (unsigned int i = 0; i < m_courseInfo.size(); ++i)
		m_courseComboBox->AddString(stringToCString(m_courseInfo[i].fullname));

	return TRUE;

}

BEGIN_MESSAGE_MAP(CEntryDlg, CDialog)
	ON_BN_CLICKED(ID_NEXT, &CEntryDlg::OnBnClickedNext)
	ON_BN_CLICKED(ID_EXIT, &CEntryDlg::OnBnClickedExit)
	ON_CBN_SELENDOK(IDC_COURSE, &CEntryDlg::OnCbnSelendokCourse)
	ON_CBN_SELENDOK(IDC_SESSION, &CEntryDlg::OnCbnSelendokSession)
	ON_BN_CLICKED(IDC_ATTENDANT, &CEntryDlg::OnBnClickedAttendant)
	ON_BN_CLICKED(IDC_REGISTER, &CEntryDlg::OnBnClickedRegister)
	ON_CBN_SELCHANGE(IDC_COURSE, &CEntryDlg::OnCbnSelchangeCourse)
	ON_CBN_SETFOCUS(IDC_SESSION, &CEntryDlg::OnCbnSetfocusSession)
END_MESSAGE_MAP()


// CEntryDlg 消息处理程序

void CEntryDlg::OnBnClickedNext()
{
	switch (GetCheckedRadioButton(IDC_REGISTER, IDC_ATTENDANT)) {
	case IDC_REGISTER:
		{
			CRegisterDlg regDlg(m_mdl);
			this->ShowWindow(SW_HIDE);
			if (regDlg.DoModal() == ID_EXIT)
				this->EndDialog(ID_EXIT);
			else
				this->ShowWindow(SW_SHOW);
			break;
		}
	case IDC_ATTENDANT:
		{
			CAttendanceDlg attdDlg(m_mdl);
			this->ShowWindow(SW_HIDE);
			if (attdDlg.DoModal() == ID_EXIT)
				this->EndDialog(ID_EXIT);
			else
				this->ShowWindow(SW_SHOW);
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
	m_static_course->EnableWindow(TRUE);
	m_courseComboBox->EnableWindow(TRUE);
	if (m_mdl.course_associated() && m_mdl.course_has_session()) {
		m_static_session->EnableWindow(TRUE);
		m_sessionComboBox->EnableWindow(TRUE);
	}
	if (m_mdl.session_associated())
		m_nextButton->EnableWindow(TRUE);
	else
		m_nextButton->EnableWindow(FALSE);
}

void CEntryDlg::OnBnClickedRegister()
{
	m_nextButton->EnableWindow(TRUE);
	m_static_course->EnableWindow(FALSE);
	m_static_session->EnableWindow(FALSE);
	m_courseComboBox->EnableWindow(FALSE);
	m_sessionComboBox->EnableWindow(FALSE);
}

void CEntryDlg::OnCbnSelendokCourse()
{
	int index = m_courseComboBox->GetCurSel();
	m_mdl.associate_course(m_courseInfo[index].id);
	if (m_mdl.course_has_session()) {
		m_static_session->EnableWindow(TRUE);
		m_sessionComboBox->EnableWindow(TRUE);
	}
}

void CEntryDlg::OnCbnSelendokSession()
{
	int index = m_sessionComboBox->GetCurSel();
	m_mdl.associate_session(m_session[index]);
	m_nextButton->EnableWindow(TRUE);
}

void CEntryDlg::OnCbnSelchangeCourse()
{
	if (m_mdl.course_has_session()) {
		m_static_session->EnableWindow(TRUE);
		m_sessionComboBox->EnableWindow(TRUE);
	} else {
		m_static_session->EnableWindow(FALSE);
		m_sessionComboBox->EnableWindow(FALSE);
	}
}

void CEntryDlg::OnCbnSetfocusSession()
{
	m_sessionComboBox->Clear();

	try {
		m_session = m_mdl.get_session_discription(  );
	} catch (mdldb::MDLDB_Exception& e) {
		MessageBox(CString(e.what()), L"数据库连接");
	}

	for (unsigned int i = 0; i < m_session.size(); ++i)
		m_sessionComboBox->AddString(stringToCString(m_session[i]));
}
