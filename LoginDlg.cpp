/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author       : Yusuke(Qiuchengxuan@gmail.com)
 * Date	        : 2011-5-11 20:29
 */
// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FP_Attendance.h"
#include "LoginDlg.h"
#include "Dbconfig.h"
#include "EntryDlg.h"

#include "ultility.h"
#include "mdldb/connector.h"
#include "mdldb/exception.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CLoginDlg 对话框




CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDlg::IDD, pParent),
	m_conn("172.16.81.156", "3306", "attendance", "attendance")
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
	ON_COMMAND(ID_32772, &CLoginDlg::On32772)
END_MESSAGE_MAP()


// CLoginDlg 消息处理程序

BOOL CLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLoginDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
        m_conn.dbconnect();
		m_conn.auth(CStringToString(username), CStringToString(password));
        CEntryDlg entryDlg(m_conn);
		this->ShowWindow(SW_HIDE);
        entryDlg.DoModal();
		this->EndDialog(ID_EXIT);
    } catch (mdldb::MDLDB_Exception& e) {
        switch (e.get_error_code()) {
		case mdldb::CONNECTION_FAIL:
            MessageBox(L"不能连接到数据库，请检查网络环境。", L"数据库连接");
            break;
        case mdldb::CONNECTION_REFUSED:
            MessageBox(L"连接被拒绝，请检查用户名和密码。", L"数据库连接");
            break;
		case mdldb::NO_USER:
			MessageBox(L"没有这个用户。", L"登录");
			break;
		case mdldb::INCORRECT_PASSWD:
			MessageBox(L"密码错误。", L"登录");
			break;
		case mdldb::NO_PERMISSION:
			MessageBox(L"您没有权限使用本程序。", L"登录");
			break;
        default:
            MessageBox(CString(e.what()), L"数据库连接");
            break;
        }
    } catch(...) {
        MessageBox(L"有未被抓取的异常！", L"内部错误");
    }
}

void CLoginDlg::OnBnClickedExit()
{
	// TODO: 在此添加控件通知处理程序代码
	this->EndDialog(ID_EXIT);
}

void CLoginDlg::On32772()
{
	CDialog* dlg = new CDialog();
	CDbconfig dbConfigDlg(m_conn);
	dbConfigDlg.DoModal();
}
