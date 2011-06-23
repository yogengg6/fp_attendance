/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author0      : Yusuke(Qiuchengxuan@gmail.com)
 * Author1      : Zembojue(zembojue@gmail.com)
 * Date	        : 2011-5-29 20:29
 */
#pragma once

#include "stdafx.h"

#include <vector>
#include <WinCrypt.h>

#include <dpDefs.h>
#include <dpRcodes.h>
#include <DPDevClt.h>

#include "afxwin.h"
#include "mdldb/Mdldb.h"

using namespace std;

// CAttendanceDlg 对话框

class CAttendanceDlg : public CDialog
{
	DECLARE_DYNAMIC(CAttendanceDlg)

public:
	CAttendanceDlg(mdldb::Mdldb& conn, CWnd* pParent = NULL);
	virtual ~CAttendanceDlg();

	void AddStatus(LPCTSTR s);

	// 对话框数据
	enum { IDD = IDD_ATTENDANT };
	enum { FP_NOTIFY = WM_USER + 100 };
	enum { FP_NOT_FOUND = -1, DEVICE_ERROR = -1, LACK_MEMORY = -3};

protected:
	//窗体控件成员变量
	CListBox*		m_notify;

	//指纹识别相关成员变量
	FT_HANDLE       m_fxContext;
	FT_HANDLE       m_mcContext;
	HDPOPERATION    m_hOperationVerify;
	DATA_BLOB       m_RegTemplate;

	//数据结构成员变量
	mdldb::Mdldb				m_mdl;
	vector<mdldb::StudentInfo>	m_student_info;

	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();

	/** 
	 * 从数据库（或本地）中的指纹数据一一比对，找到匹配的数据后返回
	 * 该学生信息于m_student_index向量中的下标
	 */
	int MatchFeatures(DATA_BLOB* const fpTemplate);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedBack();
    afx_msg LRESULT OnFpNotify (WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnFpNotify (WPARAM wParam, LPARAM lParam);
};
