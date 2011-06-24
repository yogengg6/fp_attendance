#pragma once

#include "ultility.h"
#include "Config.h"

// CDbconfig 对话框

class CConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(Config& cfg, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CConfigDlg();

// 对话框数据
	enum { IDD = IDD_DATABASE_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	Config m_cfg;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
