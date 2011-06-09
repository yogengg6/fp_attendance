#pragma once

#include "ultility.h"

// CDbconfig 对话框

class CDbconfig : public CDialog
{
	DECLARE_DYNAMIC(CDbconfig)

public:
	CDbconfig(mdldb::Mdldb& mdl, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDbconfig();

// 对话框数据
	enum { IDD = IDD_DATABASE_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	mdldb::Mdldb m_mdl;
	Config cfg;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
