#pragma once

#include "ultility.h"

// CDbconfig �Ի���

class CDbconfig : public CDialog
{
	DECLARE_DYNAMIC(CDbconfig)

public:
	CDbconfig(mdldb::Mdldb& mdl, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDbconfig();

// �Ի�������
	enum { IDD = IDD_DATABASE_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	mdldb::Mdldb m_mdl;
	Config cfg;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
