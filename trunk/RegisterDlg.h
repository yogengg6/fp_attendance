/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author       : Yusuke(Qiuchengxuan@gmail.com)
 * Date	        : 2011-5-11 20:29
 */
#pragma once

// CRegisterDlg 对话框

using namespace std;

class CRegisterDlg : public CDialog
{
	DECLARE_DYNAMIC(CRegisterDlg)

public:
	CRegisterDlg(mdldb::Mdldb& mdl, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CRegisterDlg();

	void AddStatus(LPCTSTR s);
	void AddToEnroll(FT_IMAGE_PT pFingerprintImage, int iFingerprintImageSize);
	void DisplayFingerprintImage(const DATA_BLOB* pFingerprintImage);

// 对话框数据
	enum { IDD = IDD_REGISTER };
	enum { FP_NOTIFY = WM_USER + 100 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:

	//窗体控件成员变量
	CStatic*        m_pStaticDrawArea;
	CEdit*			m_pRegStatus;
	CListBox*		m_pRegNotify;
	CRect           m_rectDrawArea;

	//指纹识别相关成员变量
	FT_HANDLE       m_fxContext;
	FT_HANDLE       m_mcContext;
	HDPOPERATION    m_hOperationEnroll;
	int             m_nPreRegFtrs;
	FT_BYTE**       m_TemplateArray;
	int             m_nRegFingerprint;
	FT_REG_OPTIONS  m_mcRegOptions;
	DATA_BLOB       m_RegTemplate;

	//数据结构成员变量
	CString				m_idnumber;
	mdldb::StudentInfo  m_studentInfo;
	mdldb::Mdldb	m_mdl;

	DECLARE_MESSAGE_MAP()

public:
	afx_msg LRESULT OnFpNotify (WPARAM wParam, LPARAM lParam);
	afx_msg virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedGetinfo();
	afx_msg void OnEnKillfocusIdnumber();
	afx_msg void OnEnSetfocusIdnumber();
	afx_msg void OnBnClickedExit();
	afx_msg void OnBnClickedBack();
};
