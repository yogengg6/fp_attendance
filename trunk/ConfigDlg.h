#pragma once

#include "ultility.h"
#include "Config.h"

// CDbconfig �Ի���

class CConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigDlg)

public:
	CConfigDlg(Config& cfg, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CConfigDlg();

// �Ի�������
	enum { IDD = IDD_DATABASE_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	Config m_cfg;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
