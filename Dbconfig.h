#pragma once


// CDbconfig �Ի���

class CDbconfig : public CDialog
{
	DECLARE_DYNAMIC(CDbconfig)

public:
	CDbconfig(mdldb::Connector& conn, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDbconfig();

// �Ի�������
	enum { IDD = IDD_DATABASE_CONFIG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	mdldb::Connector m_conn;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
