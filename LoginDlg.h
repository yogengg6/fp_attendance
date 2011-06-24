/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author       : Yusuke(Qiuchengxuan@gmail.com)
 * Date	        : 2011-5-11 20:29
 */
// LoginDlg.h : 头文件
//

#pragma once

#include "mdldb/Mdldb.h"
#include "Config.h"

using namespace mdldb;

// CLoginDlg 对话框
class CLoginDlg : public CDialog
{
// 构造
public:
	CLoginDlg(Config& cfg,
			  CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_FP_ATTENDANCE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	Config m_cfg;
	Mdldb m_mdl;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedExit();
	afx_msg void On32772();
	afx_msg void OnClickAbout();
};
