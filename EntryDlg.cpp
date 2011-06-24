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

#include <dpDefs.h>
#include <dpRcodes.h>
#include <DPDevClt.h>
#include <dpFtrEx.h>
#include <dpMatch.h>


#include "FP_Attendance.h"
#include "ultility.h"
#include "EntryDlg.h"
#include "FpManageDlg.h"
#include "AttendanceDlg.h"

// CEntryDlg 对话框

IMPLEMENT_DYNAMIC(CEntryDlg, CDialog)

CEntryDlg::CEntryDlg(mdldb::Mdldb& mdl, CWnd* pParent /*=NULL*/)
	:	CDialog(CEntryDlg::IDD, pParent), 
		m_mdl(mdl)
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
	DDX_Control(pDX, IDC_SESSION, m_sessionComboBox);
	DDX_Control(pDX, IDC_COURSE, m_courseComboBox);
	DDX_Control(pDX, IDC_ENTRY_STATIC_SESSION, m_sessionStatic);
	DDX_Control(pDX, IDC_ENTRY_STATIC_COURSE, m_courseStatic);
	DDX_Control(pDX, ID_NEXT, m_nextButton);
}

BOOL CEntryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (! m_mdl.connected())
		return FALSE;

	try {
		m_mdl.get_authorized_course(m_course);
	} catch (mdldb::MDLDB_Exception& e) {
		MessageBox(CString(e.what()), L"数据库连接");
	}

	CheckRadioButton(IDC_ATTENDANT, IDC_ENTRY_FPMANAGE, IDC_ENTRY_FPMANAGE);
	SetDlgItemText(IDC_COURSE, L"请选择课程");

	for (unsigned int i = 0; i < m_course.size(); ++i)
		m_courseComboBox.AddString(stringToCString(m_course[i].fullname));

	return TRUE;

}

BEGIN_MESSAGE_MAP(CEntryDlg, CDialog)
	ON_BN_CLICKED(ID_NEXT, &CEntryDlg::OnBnClickedNext)
	ON_BN_CLICKED(ID_EXIT, &CEntryDlg::OnBnClickedExit)
	ON_BN_CLICKED(IDC_ATTENDANT, &CEntryDlg::OnBnClickedAttendant)
	ON_BN_CLICKED(IDC_ENTRY_FPMANAGE, &CEntryDlg::OnBnClickedEntryFpmanage)
	ON_CBN_SELENDOK(IDC_COURSE, &CEntryDlg::OnCbnSelendokCourse)
	ON_CBN_SELENDOK(IDC_SESSION, &CEntryDlg::OnCbnSelendokSession)
	ON_CBN_SELCHANGE(IDC_COURSE, &CEntryDlg::OnCbnSelchangeCourse)
END_MESSAGE_MAP()


// CEntryDlg 消息处理程序

void CEntryDlg::OnBnClickedNext()
{
	switch (GetCheckedRadioButton(IDC_ATTENDANT, IDC_ENTRY_FPMANAGE)) {
	case IDC_ENTRY_FPMANAGE:
		{
			CFpManageDlg fpMngDlg(m_mdl);
			this->ShowWindow(SW_HIDE);
			if (fpMngDlg.DoModal() == ID_EXIT)
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

void CEntryDlg::OnBnClickedEntryFpmanage()
{
	m_nextButton.EnableWindow(TRUE);
	m_courseStatic.EnableWindow(FALSE);
	m_sessionStatic.EnableWindow(FALSE);
	m_courseComboBox.EnableWindow(FALSE);
	m_sessionComboBox.EnableWindow(FALSE);
}

void CEntryDlg::OnBnClickedAttendant()
{
	m_courseStatic.EnableWindow(TRUE);
	m_courseComboBox.EnableWindow(TRUE);
	if (m_mdl.course_associated() && m_mdl.course_has_session()) {
		m_sessionStatic.EnableWindow(TRUE);
		m_sessionComboBox.EnableWindow(TRUE);
	}
	if (m_mdl.session_associated())
		m_nextButton.EnableWindow(TRUE);
	else
		m_nextButton.EnableWindow(FALSE);
}

void CEntryDlg::OnCbnSelendokCourse()
{
	int index = m_courseComboBox.GetCurSel();
	m_sessionComboBox.Clear();
	try {
		m_mdl.associate_course(m_course[index].id);

		m_mdl.get_session_discription(m_session);
		if (m_mdl.course_has_session() && m_session.size() > 0) {
			for (unsigned int i = 0; i < m_session.size(); ++i)
				m_sessionComboBox.AddString(stringToCString(m_session[i]));
			m_sessionStatic.EnableWindow(TRUE);
			m_sessionComboBox.EnableWindow(TRUE);
		}
	} catch (mdldb::MDLDB_Exception& e) {
		MessageBox(CString(e.what()), L"数据库连接");
	}
}

void CEntryDlg::OnCbnSelendokSession()
{
	int index = m_sessionComboBox.GetCurSel();
	m_mdl.associate_session(m_session[index]);
	m_nextButton.EnableWindow(TRUE);
}

void CEntryDlg::OnCbnSelchangeCourse()
{
	if (m_mdl.course_has_session()) {
		m_sessionStatic.EnableWindow(TRUE);
		m_sessionComboBox.EnableWindow(TRUE);
	} else {
		m_sessionStatic.EnableWindow(FALSE);
		m_sessionComboBox.EnableWindow(FALSE);
	}
}
