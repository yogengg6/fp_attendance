/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author       : Yusuke(Qiuchengxuan@gmail.com)
 * Date	        : 2011-5-11 20:29
 */
// LoginDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FP_Attendance.h"
#include "LoginDlg.h"
#include "ConfigDlg.h"
#include "EntryDlg.h"

#include "ultility.h"
#include "mdldb/Mdldb.h"
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


// CLoginDlg �Ի���


CLoginDlg::CLoginDlg(Config& cfg,
					 CWnd* pParent)
	: CDialog(CLoginDlg::IDD, pParent),
	m_cfg(cfg),
	m_mdl(cfg.m_dbhost, cfg.m_dbport, cfg.m_dbuser, cfg.m_dbpasswd, cfg.m_passwordsalt)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_CONNECT, &CLoginDlg::OnBnClickedConnect)
	ON_BN_CLICKED(ID_EXIT, &CLoginDlg::OnBnClickedExit)
	ON_COMMAND(ID_32772, &CLoginDlg::OnMenuClickedConfig)
	ON_COMMAND(ID_32771, &CLoginDlg::OnClickAbout)
END_MESSAGE_MAP()


// CLoginDlg ��Ϣ�������

BOOL CLoginDlg::OnInitDialog()
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

	CMenu* menu = new CMenu();
	menu->LoadMenu(IDR_MENU1);
	SetMenu(menu);

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CLoginDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CLoginDlg::OnPaint()
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
HCURSOR CLoginDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLoginDlg::OnBnClickedConnect()
{
	CString username, password;
	GetDlgItemText(IDC_USERNAME, username);
	GetDlgItemText(IDC_PASSWORD, password);
    try {
        m_mdl.connect();
		m_mdl.auth(CStringToString(username), CStringToString(password));
        CEntryDlg entryDlg(m_mdl);
		this->ShowWindow(SW_HIDE);
        entryDlg.DoModal();
		this->EndDialog(ID_EXIT);
    } catch (mdldb::MDLDB_Exception& e) {
        switch (e.get_error_code()) {
		case mdldb::CONNECTION_FAIL:
            MessageBox(L"�������ӵ����ݿ⣬�������绷����", L"���ݿ�����");
            break;
        case mdldb::CONNECTION_REFUSED:
            MessageBox(L"���ӱ��ܾ����������ݿ��û��������롣", L"���ݿ�����");
            break;
		case mdldb::NO_USER:
			MessageBox(L"û������û���", L"��¼");
			break;
		case mdldb::INCORRECT_PASSWD:
			MessageBox(L"�������", L"��¼");
			break;
		case mdldb::NO_PERMISSION:
			MessageBox(L"��û��Ȩ��ʹ�ñ�����", L"��¼");
			break;
        default:
            MessageBox(CString(e.what()), L"���ݿ�����");
            break;
        }
    } catch(...) {
        MessageBox(L"��δ��ץȡ���쳣��", L"�ڲ�����");
    }
}

void CLoginDlg::OnBnClickedExit()
{
	this->EndDialog(ID_EXIT);
}

void CLoginDlg::OnMenuClickedConfig()
{
	CDialog* dlg = new CDialog();
	CConfigDlg CConfigDlg(m_cfg);
	CConfigDlg.DoModal();
	m_mdl.set_dbhost(m_cfg.m_dbhost, m_cfg.m_dbport);
	m_mdl.set_dbaccount(m_cfg.m_dbuser, m_cfg.m_dbpasswd);
}

void CLoginDlg::OnClickAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}
