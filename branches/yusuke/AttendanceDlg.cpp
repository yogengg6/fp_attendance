/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author       : Yusuke(Qiuchengxuan@gmail.com)
 * Author       : Bojue(zembojue@gmail.com)
 * Date	        : 2011-5-11 20:29
 */
// AttendanceDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FP_Attendance.h"
#include "AttendanceDlg.h"
#include "MDLDB_Connector/MDLDB_Exception.h"

// CAttendanceDlg 对话框

IMPLEMENT_DYNAMIC(CAttendanceDlg, CDialog)

CAttendanceDlg::CAttendanceDlg(mdldb::Connector& conn, CWnd* pParent /*=NULL*/)
	: CDialog(CAttendanceDlg::IDD, pParent), 
	  m_conn(conn),
	  m_notifyListBox(NULL)
{

}

CAttendanceDlg::~CAttendanceDlg()
{
}

void CAttendanceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CAttendanceDlg::OnInitDialog()                     //对话框初始化
{
	if (! m_conn.connected())
		return FALSE;
	
	m_notifyListBox	  = (CListBox *) GetDlgItem(IDC_LIST_NOTIFY);

	m_student_info	  = m_conn.get_course_student_info();
	
	return TRUE;
}

BEGIN_MESSAGE_MAP(CAttendanceDlg, CDialog)
	ON_BN_CLICKED(ID_EXIT, &CAttendanceDlg::OnBnClickedExit)
	ON_BN_CLICKED(ID_BACK, &CAttendanceDlg::OnBnClickedBack)
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

//这个函数是抽象出来的动作，向IDC_LIST_NOTIFY中添加信息
void CAttendanceDlg::AddStatus(LPCTSTR s) {
	int lIdx = SendDlgItemMessage(IDC_LIST_NOTIFY, LB_ADDSTRING, 0, (LPARAM)s);
	SendDlgItemMessage(IDC_LIST_NOTIFY, LB_SETTOPINDEX, lIdx, 0);
}



//比较的函数，尚未完成。。。
// bool CAttendanceDlg::VerifyFeatures(mdldb::StudentInfo* m_StudentInfo_list, DATA_BLOB* pImageBlob){
// 	
// 	//这里我需要得到m_StudentInfo_list所指向的数组的长度listsize？？？
// 	//对m_StudentInfo_list的操作需要研究一下。。。。？？？
// 	int listsize=
// 
//     
// 	for(int i=0;i<=listsize;i++)
// 	{
//         FT_BOOL* pComparisonDecision;//返回比对结果的指针
// 		MC_verifyFeaturesEx(
// 			IN FT_HANDLE mcContext,           /* Handle to the comparison context */
// 			IN int TemplateSize,              /* Size of the fingerprint template */
// 			m_StudentInfo_list,               /* Pointer to the location of the buffer containing the fingerprint template */
// 			pImageBlob->cbData,               /* Size of the fingerprint feature set */
// 			pImageBlob->pbData,                       /* Pointer to the location of the buffer containing the fingerprint feature set */
// 			0,                                /* This parameter is deprecated and should always be set to 0. */
// 			NULL ,                            /* This parameter is deprecated and should always be set to NULL. */
// 			NULL ,                            /* This parameter is deprecated and should always be set to NULL. */
// 			NULL ,                            /* This parameter is deprecated and should always be set to NULL. */
// 			NULL ,                            /* Pointer to the value of the achieved FAR for this comparison. If the
// 												 achieved FAR is not required, a NULL pointer can be passed. */
// 		    pComparisonDecision);             /* Pointer to the memory receiving the comparison decision. This parameter
// 												 indicates whether the comparison of the fingerprint feature set and the
// 											 fingerprint template resulted in a decision of match (FT_TRUE) or non-match
// 											 (FT_FALSE) at the security level of the specified comparison context. */
// 		if(FT_TRUE==pComparisonDecision)
// 		{
// 			return true;
// 		}
// 		else
// 		{
// 			m_StudentInfo_list++;
// 		}
// 	
// 	
// 	}
// 	return false;
// 
// }


//得到采集的指纹数据,数据接口pImageBlob
// LRESULT CAttendanceDlg::OnFpNotify(WPARAM wParam, LPARAM lParam) {
// 	if (m_idnumber == "")//这个变量对应的是什么？
// 		return S_OK;
// 
// 	switch(wParam) {
// 	case WN_COMPLETED: {
// 		SetDlgItemText(IDC_EDIT_PROMPT, L"指纹图像已获取");//IDC_EDIT_PROMPT这里没有对应的控件？？
// 		DATA_BLOB* pImageBlob = reinterpret_cast<DATA_BLOB*>(lParam);
// 		//这里应该是比对的函数以及相应的处理
//         if(true==VerifyFeatures( m_StudentInfo_list, pImageBlob))
// 		{   //比对成功
// 			//暂未考虑迟到问题
//             AddStatus(_T("指纹成功识别，祝您愉快。"));
// 
// 		}
// 		else
// 		{   //比对失败
//             AddStatus(_T("指纹识别失败！"));
// 		}
// 		
// 
//      
// 
// 	   
// 	   //对比后做相应处理，将结构保存到日志记录中（仅保存旷课和迟到的学生）。
// 	   //？日志记录格式不明，可能moodle中有相应数据表，询问学长？
// 
// 		break;
// 					   }
// 	case WN_ERROR: {
// 		TCHAR buffer[101] = {0};
// 		_sntprintf_s(buffer, 100, _TRUNCATE, _T("发生错误。错误代码: 0x%X"), lParam);
// 		AddStatus(buffer);
// 		break;
// 				   }
// 	case WN_DISCONNECT:
// 		SetDlgItemText(IDC_EDIT_PROMPT, L"指纹识别仪没有连接或失去连接");
// 		break;
// 	case WN_RECONNECT:
// 		SetDlgItemText(IDC_EDIT_PROMPT, L"指纹识别仪已连接");
// 		break;
// 	case WN_FINGER_TOUCHED:
// 		SetDlgItemText(IDC_EDIT_PROMPT, L"手指已接触识别区");
// 		break;
// 	case WN_FINGER_GONE:
// 		SetDlgItemText(IDC_EDIT_PROMPT, L"手指离开识别区");
// 		break;
// 	case WN_IMAGE_READY:
// 		SetDlgItemText(IDC_EDIT_PROMPT, L"指纹图像已准备好");
// 		break;
// 	case WN_OPERATION_STOPPED:
// 		SetDlgItemText(IDC_EDIT_PROMPT, L"指纹注册操作停止");
// 		break;
// 		
// 	}
// 	return S_OK;
// }

