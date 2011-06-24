/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author       : Yusuke(Qiuchengxuan@gmail.com)
 * Date	        : 2011-5-11 20:29
 */
#pragma once

#include <vector>

#include "afxwin.h"
#include "mdldb/Mdldb.h"

using namespace std;
using namespace mdldb;

// CEntryDlg ¶Ô»°¿ò

class CEntryDlg : public CDialog
{
	DECLARE_DYNAMIC(CEntryDlg)

public:
	CEntryDlg(Mdldb &mdl, CWnd* pParent = NULL);
	virtual ~CEntryDlg();

	enum { IDD = IDD_ENTRY };

protected:
	virtual BOOL OnInitDialog();

	virtual void DoDataExchange(CDataExchange* pDX);

private:
	Mdldb			m_mdl;
	vector<Course>	m_course;
	vector<Group>	m_gourps;
	vector<string>	m_session;

	DECLARE_MESSAGE_MAP()
	afx_msg void OnCbnSelendokCourse();
	afx_msg void OnCbnSelendokSession();
	afx_msg void OnBnClickedNext();
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedAttendant();
	afx_msg void OnCbnSelchangeCourse();
	afx_msg void OnCbnSetfocusSession();
	afx_msg void OnBnClickedEntryFpmanage();
	CComboBox m_sessionComboBox;
	CComboBox m_courseComboBox;
	CStatic m_sessionStatic;
	CStatic m_courseStatic;
	CButton m_nextButton;
};
