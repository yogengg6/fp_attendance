/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author       : Yusuke(Qiuchengxuan@gmail.com)
 * Author       : Bojue(zembojue@gmail.com)
 * Date	        : 2011-5-11 20:29
 */
// AttendanceDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FP_Attendance.h"
#include "AttendanceDlg.h"
#include "MDLDB_Connector/MDLDB_Exception.h"

// CAttendanceDlg �Ի���

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

BOOL CAttendanceDlg::OnInitDialog()                     //�Ի����ʼ��
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


// CAttendanceDlg ��Ϣ�������

void CAttendanceDlg::OnBnClickedExit()
{
	this->EndDialog(ID_EXIT);
}

void CAttendanceDlg::OnBnClickedBack()
{
	this->EndDialog(ID_BACK);
}

//��������ǳ�������Ķ�������IDC_LIST_NOTIFY�������Ϣ
void CAttendanceDlg::AddStatus(LPCTSTR s) {
	int lIdx = SendDlgItemMessage(IDC_LIST_NOTIFY, LB_ADDSTRING, 0, (LPARAM)s);
	SendDlgItemMessage(IDC_LIST_NOTIFY, LB_SETTOPINDEX, lIdx, 0);
}



//�Ƚϵĺ�������δ��ɡ�����
// bool CAttendanceDlg::VerifyFeatures(mdldb::StudentInfo* m_StudentInfo_list, DATA_BLOB* pImageBlob){
// 	
// 	//��������Ҫ�õ�m_StudentInfo_list��ָ�������ĳ���listsize������
// 	//��m_StudentInfo_list�Ĳ�����Ҫ�о�һ�¡�������������
// 	int listsize=
// 
//     
// 	for(int i=0;i<=listsize;i++)
// 	{
//         FT_BOOL* pComparisonDecision;//���رȶԽ����ָ��
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


//�õ��ɼ���ָ������,���ݽӿ�pImageBlob
// LRESULT CAttendanceDlg::OnFpNotify(WPARAM wParam, LPARAM lParam) {
// 	if (m_idnumber == "")//���������Ӧ����ʲô��
// 		return S_OK;
// 
// 	switch(wParam) {
// 	case WN_COMPLETED: {
// 		SetDlgItemText(IDC_EDIT_PROMPT, L"ָ��ͼ���ѻ�ȡ");//IDC_EDIT_PROMPT����û�ж�Ӧ�Ŀؼ�����
// 		DATA_BLOB* pImageBlob = reinterpret_cast<DATA_BLOB*>(lParam);
// 		//����Ӧ���ǱȶԵĺ����Լ���Ӧ�Ĵ���
//         if(true==VerifyFeatures( m_StudentInfo_list, pImageBlob))
// 		{   //�ȶԳɹ�
// 			//��δ���ǳٵ�����
//             AddStatus(_T("ָ�Ƴɹ�ʶ��ף����졣"));
// 
// 		}
// 		else
// 		{   //�ȶ�ʧ��
//             AddStatus(_T("ָ��ʶ��ʧ�ܣ�"));
// 		}
// 		
// 
//      
// 
// 	   
// 	   //�ԱȺ�����Ӧ�������ṹ���浽��־��¼�У���������κͳٵ���ѧ������
// 	   //����־��¼��ʽ����������moodle������Ӧ���ݱ�ѯ��ѧ����
// 
// 		break;
// 					   }
// 	case WN_ERROR: {
// 		TCHAR buffer[101] = {0};
// 		_sntprintf_s(buffer, 100, _TRUNCATE, _T("�������󡣴������: 0x%X"), lParam);
// 		AddStatus(buffer);
// 		break;
// 				   }
// 	case WN_DISCONNECT:
// 		SetDlgItemText(IDC_EDIT_PROMPT, L"ָ��ʶ����û�����ӻ�ʧȥ����");
// 		break;
// 	case WN_RECONNECT:
// 		SetDlgItemText(IDC_EDIT_PROMPT, L"ָ��ʶ����������");
// 		break;
// 	case WN_FINGER_TOUCHED:
// 		SetDlgItemText(IDC_EDIT_PROMPT, L"��ָ�ѽӴ�ʶ����");
// 		break;
// 	case WN_FINGER_GONE:
// 		SetDlgItemText(IDC_EDIT_PROMPT, L"��ָ�뿪ʶ����");
// 		break;
// 	case WN_IMAGE_READY:
// 		SetDlgItemText(IDC_EDIT_PROMPT, L"ָ��ͼ����׼����");
// 		break;
// 	case WN_OPERATION_STOPPED:
// 		SetDlgItemText(IDC_EDIT_PROMPT, L"ָ��ע�����ֹͣ");
// 		break;
// 		
// 	}
// 	return S_OK;
// }

