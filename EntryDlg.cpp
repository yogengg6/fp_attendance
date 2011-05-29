/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author       : Yusuke(Qiuchengxuan@gmail.com)
 * Date	        : 2011-5-11 20:29
 */
// EntryDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FP_Attendance.h"
#include "EntryDlg.h"
#include "RegisterDlg.h"
#include "AttendanceDlg.h"

// CEntryDlg �Ի���

IMPLEMENT_DYNAMIC(CEntryDlg, CDialog)

WCHAR CEntryDlg::m_convWchBuf[200];

CEntryDlg::CEntryDlg(mdldb::Connector& conn, CWnd* pParent /*=NULL*/)
	:	CDialog(CEntryDlg::IDD, pParent), m_conn(conn),
		m_nextButton(NULL),
		m_courseComboBox(NULL),
		m_sessionComboBox(NULL)
{
	DPFPInit();
	if (FT_OK == FX_init()) {
		if (FT_OK != MC_init())
			MessageBox(L"��ʼ��ָ�ƱȶԿ�ʧ�ܡ�", L"����", MB_OK | MB_ICONSTOP);
	} else {
		MessageBox(L"��ʼ��ָ��������ȡ��ʧ�ܡ�", L"����", MB_OK | MB_ICONSTOP);
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

	m_nextButton	  = (CButton *)	 GetDlgItem(ID_NEXT);
	m_courseComboBox  = (CComboBox*) GetDlgItem(IDC_COURSE);
	m_sessionComboBox = (CComboBox*) GetDlgItem(IDC_SESSION);

	try {
		m_courseInfo = m_conn.get_all_course();
	} catch (mdldb::MDLDB_Exception& e) {
		MessageBox(CString(e.what()), L"���ݿ�����");
	}

	SetDlgItemText(IDC_COURSE, L"��ѡ��γ�");

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
	ON_CBN_SELCHANGE(IDC_COURSE, &CEntryDlg::OnCbnSelchangeCourse)
	ON_CBN_SETFOCUS(IDC_SESSION, &CEntryDlg::OnCbnSetfocusSession)
END_MESSAGE_MAP()


// CEntryDlg ��Ϣ��������

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
	m_courseComboBox->EnableWindow(TRUE);
	if (m_conn.course_associated())
		m_sessionComboBox->EnableWindow(TRUE);
	if (m_conn.session_associated())
		m_nextButton->EnableWindow(TRUE);
	else
		m_nextButton->EnableWindow(FALSE);
}

void CEntryDlg::OnBnClickedRegister()
{
	m_nextButton->EnableWindow(TRUE);
	m_courseComboBox->EnableWindow(FALSE);
	m_sessionComboBox->EnableWindow(FALSE);
}

void CEntryDlg::OnCbnSelendokCourse()
{
	int index = m_courseComboBox->GetCurSel();
	m_conn.associate_course(m_courseInfo[index].id);
}

void CEntryDlg::OnCbnSelendokSession()
{
	int index = m_sessionComboBox->GetCurSel();
	m_conn.associate_session(m_session[index]);
	m_nextButton->EnableWindow(TRUE);
}

void CEntryDlg::OnCbnSelchangeCourse()
{
	m_sessionComboBox->EnableWindow(TRUE);
}

void CEntryDlg::OnCbnSetfocusSession()
{
	m_sessionComboBox->Clear();

	try {
		m_session = m_conn.get_session_discription(  );
	} catch (mdldb::MDLDB_Exception& e) {
		MessageBox(CString(e.what()), L"���ݿ�����");
	}

	for (vector<string>::iterator it = m_session.begin(); it != m_session.end(); ++it) {
		MultiByteToWideChar(CP_UTF8, NULL, it->c_str(), -1, m_convWchBuf, it->length());
		m_sessionComboBox->AddString(CString(m_convWchBuf));
	}
}