/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author       : Yusuke(Qiuchengxuan@gmail.com)
 * Date	        : 2011-5-11 20:29
 */

#pragma once

#include "mdldb/Mdldb.h"
#include "Config.h"

using namespace mdldb;

class CLoginDlg : public CDialog
{
public:
	CLoginDlg(Config& cfg,
			  CWnd* pParent = NULL);

// �Ի�������
	enum { IDD = IDD_FP_ATTENDANCE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON  m_hIcon;

	Config m_cfg;
	Mdldb  m_mdl;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedExit();
	afx_msg void OnMenuClickedConfig();
	afx_msg void OnClickAbout();
};
