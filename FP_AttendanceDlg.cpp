/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author       : Yusuke(Qiuchengxuan@gmail.com)
 * Date	        : 2011-5-11 20:29
 */
// FP_AttendanceDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FP_Attendance.h"
#include "FP_AttendanceDlg.h"
#include "EntryDlg.h"

#include "mdldb/connector.h"
#include "mdldb/exception.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CFP_AttendanceDlg �Ի���




CFP_AttendanceDlg::CFP_AttendanceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFP_AttendanceDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFP_AttendanceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CFP_AttendanceDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_CONNECT, &CFP_AttendanceDlg::OnBnClickedConnect)
	ON_BN_CLICKED(ID_EXIT, &CFP_AttendanceDlg::OnBnClickedExit)
END_MESSAGE_MAP()


// CFP_AttendanceDlg ��Ϣ�������

BOOL CFP_AttendanceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	SetDlgItemText(IDC_DBHOST, L"172.16.81.156");
	SetDlgItemText(IDC_DBPORT, L"3306");

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CFP_AttendanceDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFP_AttendanceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CFP_AttendanceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFP_AttendanceDlg::OnBnClickedConnect()
{
	size_t strsize;
    CString dbHost, dbPort, dbHostPort, dbUser, dbPasswd;
    GetDlgItemText(IDC_DBHOST, dbHost);
    GetDlgItemText(IDC_DBPORT, dbPort);
    GetDlgItemText(IDC_DBUSER, dbUser);
    GetDlgItemText(IDC_DBPASSWD, dbPasswd);
	dbHostPort.Format(L"tcp://%s:%s", dbHost, dbPort);

    /**
     * convert wide char to char
     */

    strsize         = (dbHostPort.GetLength() + 1) * 2;
    char *pDBHostPort   = new char[strsize];
    wcstombs_s(NULL, pDBHostPort, strsize, dbHost, _TRUNCATE);
    
    strsize         = (dbUser.GetLength() + 1) * 2;
    char *pDBUserId = new char[strsize];
    wcstombs_s(NULL, pDBUserId, strsize, dbUser, _TRUNCATE);

    strsize         = (dbPasswd.GetLength() + 1) * 2;
    char *pDBPasswd = new char[strsize];
    wcstombs_s(NULL, pDBPasswd, strsize, dbPasswd, _TRUNCATE);

    try {
        mdldb::Connector connection(pDBHostPort, pDBUserId, pDBPasswd);
        CEntryDlg entryDlg(connection);
		this->ShowWindow(SW_HIDE);
        entryDlg.DoModal();
		this->EndDialog(ID_EXIT);
        delete pDBHostPort, pDBUserId, pDBPasswd;
    } catch (mdldb::MDLDB_Exception& e) {
        delete pDBHostPort, pDBUserId, pDBPasswd;
        switch (e.get_error_code()) {
        case MDLDB_CONNECTION_FAIL:
            MessageBox(L"�������ӵ����ݿ⣬�������绷����", L"���ݿ�����");
            break;
        case MDLDB_CONNECTION_REFUSED:
            MessageBox(L"���ӱ��ܾ��������û��������롣", L"���ݿ�����");
            break;
        default:
            MessageBox(CString(e.what()), L"���ݿ�����");
            break;
        }
    } catch(...) {
        MessageBox(L"��δ��ץȡ���쳣��", L"�ڲ�����");
    }
}

void CFP_AttendanceDlg::OnBnClickedExit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	this->EndDialog(ID_EXIT);
}
