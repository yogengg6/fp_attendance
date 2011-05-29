/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author       : Yusuke(Qiuchengxuan@gmail.com)
 * Author       : Bojue(zembojue@gmail.com)
 * Date	        : 2011-5-11 20:29
 */
#pragma once

#include <vector>

#include "afxwin.h"
#include "MDLDB_Connector/MDLDB_Connector.h"

using namespace std;

// CAttendanceDlg 对话框

class CAttendanceDlg : public CDialog
{
	DECLARE_DYNAMIC(CAttendanceDlg)


public:
	CAttendanceDlg(mdldb::Connector& conn, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAttendanceDlg();

	void AddStatus(LPCTSTR s);
    bool VerifyFeatures(mdldb::StudentInfo* m_StudentInfo_list, DATA_BLOB* pImageBlob);

// 对话框数据
	enum { IDD = IDD_ATTENDANT };

protected:
	mdldb::Connector			m_conn;

	CListBox  *					m_notifyListBox;
	
	vector<mdldb::StudentInfo>	m_student_info;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()；

    CTime tm;                 //用于得到系统时间的变量
    

private:

    FT_HANDLE       m_Context;
public:
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedBack();
    afx_msg LRESULT OnFpNotify (WPARAM wParam, LPARAM lParam);
};
