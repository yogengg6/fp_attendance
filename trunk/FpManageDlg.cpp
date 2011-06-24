// FpManageDlg.cpp : 实现文件
//

#include "stdafx.h"

#include <DpUIApi.h>

#include "FP_Attendance.h"
#include "FpManageDlg.h"
#include "ultility.h"
#include "mdldb/Mdldb.h"

// CFpManageDlg 对话框

IMPLEMENT_DYNAMIC(CFpManageDlg, CDialog)

CFpManageDlg::CFpManageDlg(Mdldb& mdl, CWnd* pParent /*=NULL*/)
	: CDialog(CFpManageDlg::IDD, pParent),
	m_mdl(mdl)
{
	
}

CFpManageDlg::~CFpManageDlg()
{
}

void CFpManageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IDNUMBER, m_editIdnumber);
	DDX_Control(pDX, IDC_LIST_STUDENTINFO, m_studentInfoList);
}

BOOL CFpManageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_listColumns = 0;

	SetDlgItemText(IDC_IDNUMBER, L"请输入学号");

	m_studentInfoList.InsertColumn(m_listColumns++, L"学号", LVCFMT_LEFT, 80, 0);
	m_studentInfoList.InsertColumn(m_listColumns++, L"姓名", LVCFMT_LEFT, 80, 1);
	m_studentInfoList.InsertColumn(m_listColumns++, L"指纹1", LVCFMT_LEFT, 80, 1);
	m_studentInfoList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

	m_editIdnumber.SetLimitText(7);

	vector<Student> students;
	m_mdl.get_students(students, "", 22);

	int row = -1, col = 0;
	vector<Student>::iterator it = students.begin();

	CString str;
	CString prev_idnumber = L"";

	do {
		CString idnumber = stringToCString(it->get_idnumber());
		CString fullname = stringToCString(it->get_fullname());
		Fpdata fpdata = it->get_fpdata();

		if (idnumber == prev_idnumber) {
			if (col+1 > m_listColumns){
				m_listColumns++;
				str.Format(L"指纹%d", m_listColumns-2);
				m_studentInfoList.InsertColumn(m_listColumns, str, LVCFMT_LEFT, 80);
			}
			str.LoadString(IDS_FINGER_INDEX + fpdata.index);
			m_studentInfoList.SetItemText(row, col++, str);
		} else if (row < 10) {
			col = 0;
			row++;
			m_studentInfoList.InsertItem(row, L"");
			m_studentInfoList.SetItemText(row, col++, idnumber);
			m_studentInfoList.SetItemText(row, col++, fullname);
			prev_idnumber = idnumber;
			if (fpdata.index > 0) {
				str.LoadString(IDS_FINGER_INDEX + fpdata.index);
				m_studentInfoList.SetItemText(row, col++, str);
			}
		} else break;
	} while (++it != students.end());
	return TRUE;
}

BEGIN_MESSAGE_MAP(CFpManageDlg, CDialog)
	ON_BN_CLICKED(IDBACK, &CFpManageDlg::OnBnClickedBack)
	ON_BN_CLICKED(IDEXIT, &CFpManageDlg::OnBnClickedExit)
	ON_EN_KILLFOCUS(IDC_IDNUMBER, &CFpManageDlg::OnEnKillfocusIdnumber)
	ON_EN_SETFOCUS(IDC_IDNUMBER, &CFpManageDlg::OnEnSetfocusIdnumber)
	ON_EN_CHANGE(IDC_IDNUMBER, &CFpManageDlg::OnEnChangeIdnumber)
	ON_BN_CLICKED(IDC_FP_ADD_MODIFY, &CFpManageDlg::OnBnClickedFpAddModify)
	ON_BN_CLICKED(IDC_FP_DELETE, &CFpManageDlg::OnBnClickedFpDelete)
END_MESSAGE_MAP()


// CFpManageDlg 消息处理程序

void CFpManageDlg::OnBnClickedBack()
{
	this->EndDialog(ID_BACK);
}

void CFpManageDlg::OnBnClickedExit()
{
	this->EndDialog(ID_EXIT);
}

void CFpManageDlg::OnEnKillfocusIdnumber()
{
	CString idnumber;
	GetDlgItemText(IDC_IDNUMBER, idnumber);
	if (idnumber.GetLength() == 0)
		SetDlgItemText(IDC_IDNUMBER, L"请输入学号");
}

void CFpManageDlg::OnEnSetfocusIdnumber()
{
	CString idnumber;
	GetDlgItemText(IDC_IDNUMBER, idnumber);
	if (idnumber == L"请输入学号")
		SetDlgItemText(IDC_IDNUMBER, L"");
}

void CFpManageDlg::OnEnChangeIdnumber()
{
	CString idnumber;
	GetDlgItemText(IDC_IDNUMBER, idnumber);
	if (idnumber == L"请输入学号" || idnumber == L"")
		return;

	m_studentInfoList.DeleteAllItems();

	vector<Student> students;
	m_mdl.get_students(students, CStringToString(idnumber), 22);

	if (students.size() == 0)
		return;

	int row = -1, col = 0;
	CString str;
	CString prev_idnumber = L"";
	vector<Student>::iterator it = students.begin();

	do {
		CString idnumber = stringToCString(it->get_idnumber());
		CString fullname = stringToCString(it->get_fullname());
		Fpdata fpdata = it->get_fpdata();

		if (idnumber == prev_idnumber) {
			if (col+1 > m_listColumns){
				m_listColumns++;
				str.Format(L"指纹%d", m_listColumns-2);
				m_studentInfoList.InsertColumn(m_listColumns, str, LVCFMT_LEFT, 80);
			}
			str.LoadString(IDS_FINGER_INDEX + fpdata.index);
			m_studentInfoList.SetItemText(row, col++, str);
		} else if (row < 10) {
			col = 0;
			row++;
			m_studentInfoList.InsertItem(row, L"");
			m_studentInfoList.SetItemText(row, col++, idnumber);
			m_studentInfoList.SetItemText(row, col++, fullname);
			prev_idnumber = idnumber;
			if (fpdata.index > 0) {
				str.LoadString(IDS_FINGER_INDEX + fpdata.index);
				m_studentInfoList.SetItemText(row, col++, str);
			}
		} else break;
	} while (++it != students.end());
}

static DATA_BLOB fpTemplate[11] = {0};

HRESULT CALLBACK EnrollmentProc(HWND hParentWnd, DP_ENROLLMENT_ACTION enrollmentAction, 
								UINT fingerIndex, DATA_BLOB* newFpTemplate, LPVOID pData)
{
	switch (enrollmentAction) {
		case DP_ENROLLMENT_ADD:
			if (fpTemplate[fingerIndex].pbData)
				delete [] fpTemplate[fingerIndex].pbData;
			fpTemplate[fingerIndex].cbData = 0;
			fpTemplate[fingerIndex].pbData = NULL;
			fpTemplate[fingerIndex].pbData = new byte[newFpTemplate->cbData];
			memcpy(fpTemplate[fingerIndex].pbData, newFpTemplate->pbData, newFpTemplate->cbData);
			fpTemplate[fingerIndex].cbData = newFpTemplate->cbData;
			break;
		case DP_ENROLLMENT_DELETE:
			if (fpTemplate[fingerIndex].pbData != NULL)
				delete [] fpTemplate[fingerIndex].pbData;
			fpTemplate[fingerIndex].pbData = NULL;
			fpTemplate[fingerIndex].cbData = 0;
	}
	return S_OK;
}

void CFpManageDlg::OnBnClickedFpAddModify()
{
	unsigned long fpMask = 0;
	HWND hWnd = this->GetSafeHwnd();
	POSITION pos = m_studentInfoList.GetFirstSelectedItemPosition();
	if (pos != 0) {
		int id = (int) m_studentInfoList.GetNextSelectedItem(pos);
		string idnumber = CStringToString(m_studentInfoList.GetItemText(id, 0));
		DPEnrollUI(hWnd, 2, &fpMask, (DPENROLLMENTPROC)EnrollmentProc, NULL);
		if (fpMask > 0) {
			m_mdl.fpdelete(idnumber);
			for (int i = 1; i <= 10; ++i) {
				if (fpTemplate[i].cbData != 0) {
					Fpdata fpdata = {i, fpTemplate[i].cbData, fpTemplate[i].pbData};
					m_mdl.fpenroll(Student(idnumber, fpdata));
				}
			}
		}
		OnEnChangeIdnumber();
	}
}

void CFpManageDlg::OnBnClickedFpDelete()
{
	POSITION pos = m_studentInfoList.GetFirstSelectedItemPosition();
	if (pos != 0) {
		int id = (int) m_studentInfoList.GetNextSelectedItem(pos);
		CString idnumber = m_studentInfoList.GetItemText(id, 0);
		m_mdl.fpdelete(CStringToString(idnumber));
		OnEnChangeIdnumber();
	}
}
