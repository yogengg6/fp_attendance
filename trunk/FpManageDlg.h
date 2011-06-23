#pragma once

#include "mdldb/Mdldb.h"
#include "afxwin.h"
#include "afxcmn.h"

using namespace mdldb;

// CFpManageDlg 对话框

class CFpManageDlg : public CDialog
{
	DECLARE_DYNAMIC(CFpManageDlg)

public:
	CFpManageDlg(Mdldb& mdl, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFpManageDlg();

// 对话框数据
	enum { IDD = IDD_FPMANAGE };
	enum { MAX_FP_COUNT = 2};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();

	CEdit m_editIdnumber;
	CListCtrl m_studentInfoList;
	int m_listColumns;

	Mdldb m_mdl;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBack();
	afx_msg void OnBnClickedExit();
	afx_msg void OnEnKillfocusIdnumber();
	afx_msg void OnEnSetfocusIdnumber();
	afx_msg void OnEnChangeIdnumber();
	afx_msg void OnBnClickedFpAddModify();
	afx_msg void OnBnClickedFpDelete();
};
