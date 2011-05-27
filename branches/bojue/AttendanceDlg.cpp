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
	  m_Context(0),
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



//�Ƚϵĺ���
 bool CAttendanceDlg::VerifyFeatures(mdldb::StudentInfo* m_StudentInfo_list, DATA_BLOB* pImageBlob){
 	
 	//��������Ҫ�õ�m_StudentInfo_list��ָ�������ĳ���ListSize������������������
 	//��m_StudentInfo_list�Ĳ�����Ҫ�о�һ��(�ѽ��)
 	int ListSize=
 
     
 	for(int i=0;i<=ListSize;i++)
 	{
        FT_BOOL* pComparisonDecision;//���رȶԽ����ָ��

 		MC_verifyFeaturesEx(
 		    m_Context,               //����������������⣿������������ ������������                 
 			m_StudentInfo_list[i].fpsize,     
 			m_StudentInfo_list[i].fpdata,     
 			pImageBlob->cbData,               
 			pImageBlob->pbData,               
 			0,                                
 			NULL ,                           
 			NULL ,                            
 			NULL ,                            
 			NULL ,                            									
 		    pComparisonDecision);            									
 											 
 		if(FT_TRUE == pComparisonDecision)
		{
			return true;
		}
 		else
		{
			m_StudentInfo_list++;
		}

 	
 	}
	
	return false;
 
 }


//�õ��ɼ���ָ������,���ݽӿ�pImageBlob
 LRESULT CAttendanceDlg::OnFpNotify(WPARAM wParam, LPARAM lParam) {
 	if (m_idnumber == "")//���������Ӧ����ʲô��������������������������
 		return S_OK;
 
 	switch(wParam) {
 	case WN_COMPLETED: {
 		SetDlgItemText(IDC_EDIT_PROMPT, L"ָ��ͼ���ѻ�ȡ");
 		DATA_BLOB* pImageBlob = reinterpret_cast<DATA_BLOB*>(lParam);
		//m_StudentInfo_listָ������IDD_ENTRY�еõ��ģ�
		//����Ҫ��ô�������أ�������������������������������������������
         if(true == VerifyFeatures( m_StudentInfo_list, pImageBlob))
 			{   //�ȶԳɹ�
 				//��δ���ǳٵ�����
				 AddStatus(_T("ָ�Ƴɹ�ʶ��ף����졣"));
	 
 			}
 		else
 			{   //�ȶ�ʧ��
				 AddStatus(_T("ָ��ʶ��ʧ�ܣ�"));
 			}



		 //������Ҫ��ʶ������Ӧ����������ѧ���ĺ�����
		
	 delete []m_StudentInfo_list;//�ͷ��ڴ棿����������������������������������

 
 		break;
 					   }
 	case WN_ERROR: {
 		TCHAR buffer[101] = {0};
 		_sntprintf_s(buffer, 100, _TRUNCATE, _T("�������󡣴������: 0x%X"), lParam);
 		AddStatus(buffer);
 		break;
 				   }
 	case WN_DISCONNECT:
 		SetDlgItemText(IDC_EDIT_PROMPT, L"ָ��ʶ����û�����ӻ�ʧȥ����");
 		break;
 	case WN_RECONNECT:
 		SetDlgItemText(IDC_EDIT_PROMPT, L"ָ��ʶ����������");
 		break;
 	case WN_FINGER_TOUCHED:
 		SetDlgItemText(IDC_EDIT_PROMPT, L"��ָ�ѽӴ�ʶ����");
 		break;
 	case WN_FINGER_GONE:
 		SetDlgItemText(IDC_EDIT_PROMPT, L"��ָ�뿪ʶ����");
		break;
 	case WN_IMAGE_READY:
 		SetDlgItemText(IDC_EDIT_PROMPT, L"ָ��ͼ����׼����");
 		break;
 	case WN_OPERATION_STOPPED:
		SetDlgItemText(IDC_EDIT_PROMPT, L"ָ��ע�����ֹͣ");
		break;
 		
 	}
 	return S_OK;
 }

