/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author       : Yusuke(Qiuchengxuan@gmail.com)
 * Author       : Bojue(zembojue@gmail.com)
 * Date	        : 2011-5-29 20:29
 */

#include "stdafx.h"

#include <dpDefs.h>
#include <dpRcodes.h>
#include <DPDevClt.h>
#include <dpFtrEx.h>
#include <dpMatch.h>

#include "ultility.h"
#include "FP_Attendance.h"
#include "AttendanceDlg.h"
#include "mdldb/userinfo.h"
#include "mdldb/exception.h"

// CAttendanceDlg 对话框

using namespace mdldb;

IMPLEMENT_DYNAMIC(CAttendanceDlg, CDialog)

CAttendanceDlg::CAttendanceDlg(mdldb::Mdldb& mdl, CWnd* pParent /*=NULL*/)
	:	CDialog(CAttendanceDlg::IDD, pParent), 
		m_hOperationVerify(0),
		m_fxContext(0), m_mcContext(0),
		m_mdl(mdl),
		m_notify(NULL)
{
}

CAttendanceDlg::~CAttendanceDlg()
{
	delete [] m_RegTemplate.pbData;
	m_RegTemplate.cbData = 0;
	m_RegTemplate.pbData = NULL;

	if (m_hOperationVerify) {
		DPFPStopAcquisition(m_hOperationVerify);
		DPFPDestroyAcquisition(m_hOperationVerify);
		m_hOperationVerify = 0;
	}

	if (m_fxContext) {
		FX_closeContext(m_fxContext);
		m_fxContext = 0;
	}

	if (m_mcContext) {
		MC_closeContext(m_mcContext);
		m_mcContext = 0;
	}

}

void CAttendanceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CAttendanceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (! m_mdl.connected())
		return FALSE;

	::ZeroMemory(&m_RegTemplate, sizeof(m_RegTemplate));

	m_notify = (CListBox *) GetDlgItem(IDC_ATTENDANT_NOTIFY);

	try {
		m_mdl.get_course_student_info(m_student_info);
	} catch (mdldb::MDLDB_Exception& e) {
		MessageBox(CString(e.what()), L"考勤");
	}

	if (FT_OK != FX_createContext(&m_fxContext)) {
		MessageBox(L"创建特征提取上下文失败。", L"指纹注册", MB_OK | MB_ICONSTOP);
	}

	if (FT_OK != MC_createContext(&m_mcContext)) {
		MessageBox(_T("创建比对上下文失败。"), _T("指纹注册"), MB_OK | MB_ICONSTOP);
	}

	MC_SETTINGS mcSettings = {0};
	if (FT_OK != MC_getSettings(&mcSettings)) {
		MessageBox(_T("获取预设置指纹特征集提取次数失败。"), _T("指纹注册"), MB_OK | MB_ICONSTOP);
	}
	
	DPFPCreateAcquisition(	DP_PRIORITY_NORMAL, GUID_NULL, 
							DP_SAMPLE_TYPE_IMAGE, m_hWnd, 
							FP_NOTIFY, &m_hOperationVerify
						 );
	DPFPStartAcquisition(m_hOperationVerify);

	return TRUE;
}

BEGIN_MESSAGE_MAP(CAttendanceDlg, CDialog)
	ON_MESSAGE	 (FP_NOTIFY,	&CAttendanceDlg::OnFpNotify)
	ON_BN_CLICKED(ID_EXIT,		&CAttendanceDlg::OnBnClickedExit)
	ON_BN_CLICKED(ID_BACK,		&CAttendanceDlg::OnBnClickedBack)
    OnFpNotify(WPARAM wParam, LPARAM lParam)
	
END_MESSAGE_MAP()


// CAttendanceDlg 消息处理程序

void CAttendanceDlg::OnBnClickedExit()
{
	this->EndDialog(ID_EXIT);
}

void CAttendanceDlg::OnBnClickedBack()
{
	this->EndDialog(ID_BACK);
}

void CAttendanceDlg::AddStatus(LPCTSTR s) {
	int lIdx = SendDlgItemMessage(IDC_ATTENDANT_NOTIFY, LB_ADDSTRING, 0, (LPARAM)s);
	SendDlgItemMessage(IDC_ATTENDANT_NOTIFY, LB_SETTOPINDEX, lIdx, 0);
}

int CAttendanceDlg::MatchFeatures(DATA_BLOB* const fpImage)
{
	if (m_student_info.size() == 0)
		return DEVICE_ERROR;

	FT_BOOL extractStatus;
	HRESULT hr = S_OK;
	FT_BYTE* fpTemplate = NULL;

	try {
		FT_RETCODE rc = FT_OK;

		int iRecommendedVerFtrLen = 0;
		rc = FX_getFeaturesLen(FT_VER_FTR, &iRecommendedVerFtrLen, NULL);

		if (FT_OK != rc)
			return DEVICE_ERROR;

		if ((fpTemplate = new FT_BYTE[iRecommendedVerFtrLen]) == NULL)
			return LACK_MEMORY;

		FT_IMG_QUALITY imgQuality;
		FT_FTR_QUALITY ftrQuality;

		rc = FX_extractFeatures(m_fxContext,
								fpImage->cbData,
								fpImage->pbData,
								FT_VER_FTR,
								iRecommendedVerFtrLen,
								fpTemplate,
								&imgQuality,
								&ftrQuality,
								&extractStatus);
		if (FT_OK <= rc && extractStatus == FT_TRUE) {
			FT_BOOL bRegFeaturesChanged = FT_FALSE;
			FT_BOOL match_result = FT_FALSE;

			FT_VER_SCORE VerScore = FT_UNKNOWN_SCORE;
			double dFalseAcceptProbability = 0.0;

			//由这里开始逐一比较
			for (unsigned int i = 0; i < m_student_info.size(); ++i) {
				Fpdata fpdata = m_student_info[i].get_fpdata();
				rc = MC_verifyFeaturesEx(m_mcContext, 
										 fpdata.size, 
										 fpdata.data, 
										 iRecommendedVerFtrLen,
									 	 fpTemplate,
										 0,
										 &bRegFeaturesChanged,
										 NULL,
										 &VerScore,
										 &dFalseAcceptProbability,
										 &match_result);
				if (rc >= FT_OK) {
					if (rc == FT_OK)
						if (match_result == FT_TRUE) {
							delete fpTemplate;
							return i;
						}
				}
				else {
					delete fpTemplate;
					return DEVICE_ERROR;
				}
			}
			delete fpTemplate;
			return FP_NOT_FOUND;
		}
	} catch (...) {
		throw;
	}
	return DEVICE_ERROR;
}

LRESULT CAttendanceDlg::OnFpNotify(WPARAM wParam, LPARAM lParam) {

	switch(wParam) {
	case WN_COMPLETED:
		{
			DATA_BLOB* fpImage = reinterpret_cast<DATA_BLOB*>(lParam);
			int index;

			if ((index = MatchFeatures(fpImage)) >= 0) {
				string& idnumber = m_student_info[index].get_idnumber();
				string& fullname = m_student_info[index].get_fullname();
				m_mdl.attendant(idnumber);
				SetDlgItemText(IDC_ATTENDANT_IDNUMBER, CString(idnumber.c_str()));
				SetDlgItemText(IDC_ATTENDANT_FULLNAME, stringToCString(fullname));
				AddStatus(_T("您的出勤信息登入成功，祝您愉快！"));
			} else {
				AddStatus(_T("对不起，没有找到指纹相应的数据。"));
			}

			break;
		}
	case WN_ERROR:
		{
			TCHAR buffer[101] = {0};
			_sntprintf_s(buffer, 100, _TRUNCATE, _T("发生错误。错误代码: 0x%X"), lParam);
			AddStatus(buffer);
			break;
		}
	case WN_DISCONNECT:
		AddStatus(_T("指纹识别仪没有连接或失去连接"));
		break;
	case WN_RECONNECT:
		AddStatus(_T("指纹识别仪已连接"));
		break;
	}
	return S_OK;
}

