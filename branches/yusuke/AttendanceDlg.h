/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author0      : Yusuke(Qiuchengxuan@gmail.com)
 * Author1      : Zembojue(zembojue@gmail.com)
 * Date	        : 2011-5-11 20:29
 */
#pragma once

#include <vector>

#include "afxwin.h"
#include "MDLDB_Connector/MDLDB_Connector.h"

using namespace std;

// CAttendanceDlg �Ի���

class CAttendanceDlg : public CDialog
{
	DECLARE_DYNAMIC(CAttendanceDlg)

public:
	CAttendanceDlg(mdldb::Connector& conn, CWnd* pParent = NULL);
	virtual ~CAttendanceDlg();

	void AddStatus(LPCTSTR s);
    //bool VerifyFeatures(mdldb::StudentInfo* m_StudentInfo_list, DATA_BLOB* pImageBlob);

// �Ի�������
	enum { IDD = IDD_ATTENDANT };
	enum { FP_NOTIFY = WM_USER + 100 };
	enum { FP_NOT_FOUND = -1, DEVICE_ERROR = -1, LACK_MEMORY = -3};

protected:
	//����ؼ���Ա����
	CListBox*		m_notify;

	//ָ��ʶ����س�Ա����
	FT_HANDLE       m_fxContext;
	FT_HANDLE       m_mcContext;
	HDPOPERATION    m_hOperationVerify;
	DATA_BLOB       m_RegTemplate;

	//���ݽṹ��Ա����
	mdldb::Connector			m_conn;
	vector<mdldb::StudentInfo>	m_student_info;

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	int MatchFeatures(DATA_BLOB* const fpTemplate);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedBack();
    afx_msg LRESULT OnFpNotify (WPARAM wParam, LPARAM lParam);
};
