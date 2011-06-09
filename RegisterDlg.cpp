/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author       : Yusuke(Qiuchengxuan@gmail.com)
 * Date	        : 2011-5-11 20:29
 */
// RegisterDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ultility.h"
#include "FP_Attendance.h"
#include "RegisterDlg.h"


// CRegisterDlg 对话框

IMPLEMENT_DYNAMIC(CRegisterDlg, CDialog)

CRegisterDlg::CRegisterDlg(mdldb::Mdldb& mdl, CWnd* pParent /*=NULL*/)
	:	CDialog(CRegisterDlg::IDD, pParent),
		m_hOperationEnroll(0),
		m_fxContext(0), m_mcContext(0),
		m_TemplateArray(NULL),
		m_nRegFingerprint(0),
		m_mcRegOptions(FT_DEFAULT_REG),
		m_mdl(mdl)
{
	FT_RETCODE rc = FT_OK;
	if (FT_OK != (rc = FX_createContext(&m_fxContext))) {
		MessageBox(L"创建特征提取上下文失败。", L"指纹注册", MB_OK | MB_ICONSTOP);
	}

	if (FT_OK != (rc = MC_createContext(&m_mcContext))) {
		MessageBox(_T("创建比对上下文失败。"), _T("指纹注册"), MB_OK | MB_ICONSTOP);
	}

	MC_SETTINGS mcSettings = {0};
	if (FT_OK != (rc = MC_getSettings(&mcSettings))) {
		MessageBox(_T("获取预设置指纹特征集提取次数失败。"), _T("指纹注册"), MB_OK | MB_ICONSTOP);
	}

	m_nPreRegFtrs = mcSettings.numPreRegFeatures;
	m_TemplateArray = new FT_BYTE*[m_nPreRegFtrs];
		

	::ZeroMemory(m_TemplateArray, sizeof(FT_BYTE**) * m_nPreRegFtrs);

	m_nRegFingerprint = 0;
}

CRegisterDlg::~CRegisterDlg()
{
	if (m_hOperationEnroll) {
		DPFPStopAcquisition(m_hOperationEnroll);
		DPFPDestroyAcquisition(m_hOperationEnroll);
		m_hOperationEnroll = 0;
	}

	if (m_fxContext) {
		FX_closeContext(m_fxContext);
		m_fxContext = 0;
	}

	if (m_mcContext) {
		MC_closeContext(m_mcContext);
		m_mcContext = 0;
	}

	if (m_TemplateArray) {
		for (int i = 0; i < m_nPreRegFtrs; ++i)
			if (m_TemplateArray[i]) {
				delete [] m_TemplateArray[i];
				m_TemplateArray[i] = NULL;
			}
			delete [] m_TemplateArray;
			m_TemplateArray = NULL;
	}
}

void CRegisterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BOOL CRegisterDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN) 
	{
		this->OnBnClickedGetinfo();
		pMsg->wParam = VK_TAB; 
	} 
	return   CDialog::PreTranslateMessage(pMsg); 
}

BEGIN_MESSAGE_MAP(CRegisterDlg, CDialog)
	ON_MESSAGE		(FP_NOTIFY,			&CRegisterDlg::OnFpNotify)
	ON_EN_KILLFOCUS	(IDC_REG_IDNUMBER,	&CRegisterDlg::OnEnKillfocusIdnumber)
	ON_EN_SETFOCUS	(IDC_REG_IDNUMBER,	&CRegisterDlg::OnEnSetfocusIdnumber)
	ON_BN_CLICKED	(IDC_REG_GETINFO,	&CRegisterDlg::OnBnClickedGetinfo)
	ON_BN_CLICKED	(IDEXIT,			&CRegisterDlg::OnBnClickedExit)
	ON_BN_CLICKED	(IDBACK,			&CRegisterDlg::OnBnClickedBack)
END_MESSAGE_MAP()


// CRegisterDlg 消息处理程序

BOOL CRegisterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pRegStatus		= (CEdit*)	  GetDlgItem(IDC_REG_STATUS);
	m_pRegNotify		= (CListBox*) GetDlgItem(IDC_REG_NOTIFY);
	m_pStaticDrawArea	= (CStatic*)  GetDlgItem(IDC_STATIC_DRAWAREA);

	m_pStaticDrawArea->GetClientRect(&m_rectDrawArea);

	SetDlgItemText(IDC_REG_IDNUMBER, L"请在这里输入学号");
	SetDlgItemText(IDC_REG_FULLNAME, L"");

	DPFPCreateAcquisition(	DP_PRIORITY_NORMAL, GUID_NULL, 
							DP_SAMPLE_TYPE_IMAGE, m_hWnd, 
							FP_NOTIFY, &m_hOperationEnroll
							);
	DPFPStartAcquisition(m_hOperationEnroll);

	return TRUE;
}
void CRegisterDlg::OnBnClickedGetinfo()
{
	CString idnumber;

	GetDlgItemText(IDC_REG_IDNUMBER, idnumber);

	try {
		m_studentInfo = m_mdl.get_student_info(CStringToString(idnumber));
		string fullname = m_studentInfo.get_fullname();

		CString verify_notice = CString(L"学号:") + CString(m_studentInfo.get_idnumber().c_str());
		verify_notice.AppendFormat(	L"\n姓名:%s\n指纹:%s", 
									stringToCString(fullname), 
									(m_studentInfo.get_fpsize() > 0) ? L"已注册": L"未注册"
									);


		if (MessageBox(verify_notice, L"个人信息确认", MB_OKCANCEL) == TRUE) {
			m_idnumber = idnumber;
			CString str;

			m_pRegStatus->EnableWindow(TRUE);
			m_pRegNotify->EnableWindow(TRUE);
			m_pStaticDrawArea->EnableWindow(TRUE);

			str.Format(L"%d次", m_nPreRegFtrs);
			SetDlgItemText(IDC_REG_STATUS, L"你已经可以开始扫描你的指纹。");
			SetDlgItemText(IDC_REG_FULLNAME, stringToCString(fullname));
			AddStatus(L"请将手指接触扫描区以完成操作，总共需要" + str);
		} else {
			m_idnumber = "";
			SetDlgItemText(IDC_REG_IDNUMBER, L"请在这里输入学号");
			SetDlgItemText(IDC_REG_FULLNAME, L"");
		}
	} catch (mdldb::MDLDB_Exception& e) {
		m_idnumber = "";
		MessageBox(CString(e.what()), L"确认");
		SetDlgItemText(IDC_REG_IDNUMBER, L"请在这里输入学号");
		SetDlgItemText(IDC_REG_FULLNAME, L"");
	}
}

void CRegisterDlg::AddStatus(LPCTSTR s) {
	int lIdx = SendDlgItemMessage(IDC_REG_NOTIFY, LB_ADDSTRING, 0, (LPARAM)s);
	SendDlgItemMessage(IDC_REG_NOTIFY, LB_SETTOPINDEX, lIdx, 0);
}

DPFP_STDAPI DPFPConvertSampleToBitmap(const DATA_BLOB* pSample, PBYTE pBitmap, size_t* pSize);

void CRegisterDlg::DisplayFingerprintImage(const DATA_BLOB* pImageBlob)
{
	HRESULT hr = S_OK;
	PBITMAPINFO pOutBmp = NULL;
	size_t size = 0;
	hr = DPFPConvertSampleToBitmap(pImageBlob, 0, &size);//to get size.
	pOutBmp = (PBITMAPINFO)new BYTE[size];
	hr = DPFPConvertSampleToBitmap(pImageBlob, (PBYTE)pOutBmp, &size);

	int srcWidth   = pOutBmp->bmiHeader.biWidth;
	int srcHeight  = pOutBmp->bmiHeader.biHeight;
	int dstWidth   = m_rectDrawArea.Width();
	int dstHeight  = m_rectDrawArea.Height();

	if (SUCCEEDED(hr)) {
		size_t dwColorsSize = pOutBmp->bmiHeader.biClrUsed * sizeof(PALETTEENTRY);
		const BYTE* pBmpBits = (PBYTE)pOutBmp + sizeof(BITMAPINFOHEADER) + dwColorsSize;

		CDC* pDC = m_pStaticDrawArea->GetDC();
		
		int i = StretchDIBits(pDC->GetSafeHdc(), 
			0, 0, dstWidth, dstHeight,
			0, 0, srcWidth, srcHeight, 
			pBmpBits, pOutBmp, DIB_RGB_COLORS, SRCCOPY);
		if (i == 0)
			DWORD dErr = GetLastError();

		

		ReleaseDC(pDC);
	}

	delete [] (PBYTE)pOutBmp;
}

void CRegisterDlg::AddToEnroll(FT_IMAGE_PT pFingerprintImage,
                               int iFingerprintImageSize)
{
    HRESULT hr = S_OK;
    FT_BYTE* pPreRegTemplate = NULL;
	FT_BYTE* pRegTemplate	 = NULL;
    try {
        if (m_nRegFingerprint < m_nPreRegFtrs) {
            int iRcmdFtrSetLen = 0;

            FT_RETCODE rc = FT_OK;
            
            FX_getFeaturesLen( FT_PRE_REG_FTR, &iRcmdFtrSetLen,  NULL);

            // Extract Features (i.e. create fingerprint template)
            FT_IMG_QUALITY imgQuality;
            FT_FTR_QUALITY ftrQuality;
            FT_BOOL bEextractOK = FT_FALSE;

            pPreRegTemplate = new FT_BYTE[iRcmdFtrSetLen];

            rc = FX_extractFeatures(m_fxContext,
                                    iFingerprintImageSize,
                                    pFingerprintImage,
                                    FT_PRE_REG_FTR,
                                    iRcmdFtrSetLen,
                                    pPreRegTemplate,
                                    &imgQuality,
                                    &ftrQuality,
                                    &bEextractOK);

            /**
             * If feature extraction succeeded, add the pre-Enrollment
             * feature sets to the set of 4 templates needed to create 
             * Enrollment template.
             */
            if (FT_OK <= rc && bEextractOK == FT_TRUE) {
                CString str("指纹特征集成功生成");
                str.AppendFormat(L"[%d/%d]", m_nRegFingerprint+1, m_nPreRegFtrs);
                AddStatus(str);

                m_TemplateArray[m_nRegFingerprint++] = pPreRegTemplate;
                pPreRegTemplate = NULL;

                TCHAR buffer[101] = {0};
                ULONG uSize = 100;

                if (m_nRegFingerprint == m_nPreRegFtrs) {
                    int iRcmdRegFtrLen = 0;
                    rc = MC_getFeaturesLen( FT_REG_FTR,
                                            m_mcRegOptions,
                                            &iRcmdRegFtrLen,
                                            NULL);

					pRegTemplate = new FT_BYTE[iRcmdRegFtrLen];

                    FT_BOOL bRegSucceeded = FT_FALSE;
                    rc = MC_generateRegFeatures(m_mcContext,
                                                m_mcRegOptions,
                                                m_nPreRegFtrs,
                                                iRcmdRegFtrLen,
                                                m_TemplateArray,
                                                iRcmdRegFtrLen,
                                                pRegTemplate,
                                                NULL,
                                                &bRegSucceeded);

                    if (FT_OK <= rc && bRegSucceeded == FT_TRUE) {
                        m_studentInfo.set_fpdata(iRcmdRegFtrLen, (byte*)pRegTemplate);
                        m_mdl.enroll(m_studentInfo);
//                         m_RegTemplate.pbData = pRegTemplate;
//                         m_RegTemplate.cbData = iRcmdRegFtrLen;
                        //Add Enroll code here

                        AddStatus(_T("指纹模版成功生成, 你已经成功完成注册。"));

                        MessageBox( L"指纹模版成功生成, 你已经成功完成注册。", 
                                    L"指纹注册");

						m_pRegNotify->ResetContent();

						m_pRegStatus->EnableWindow(FALSE);
						m_pRegNotify->EnableWindow(FALSE);
						m_pStaticDrawArea->EnableWindow(FALSE);

                        SetDlgItemText(IDC_REG_STATUS, 
                                       L"你已经完成注册");
                        SetDlgItemText(IDC_REG_FULLNAME, L"");
                        SetDlgItemText(IDC_REG_IDNUMBER, L"请在这里输入学号");
                    }
                    else {
                        MessageBox( _T("生成指纹模版失败。"),
                                    _T("指纹注册"), 
                                    MB_OK | MB_ICONINFORMATION);
                        SetDlgItemText( IDC_REG_STATUS, 
                                        _T("扫描你的指纹以注册。"));
                        // Enrolment faled, cleanup data.
                        m_nRegFingerprint = 0;

                        for (int i = 0; i < m_nPreRegFtrs; ++i)
                            if (m_TemplateArray[i]) {
                                delete [] m_TemplateArray[i];
                                m_TemplateArray[i] = NULL;
                            }
                    }
                }
                else {
                    SetDlgItemText(IDC_REG_STATUS, L"再次扫描相同的手指");
                }
            }
            else {
                MessageBox(L"制作指纹特征集失败。",  L"指纹注册", 
                           MB_OK | MB_ICONINFORMATION);
                SetDlgItemText(IDC_REG_STATUS, L"扫描你的指纹以注册。");
            }
        }
        else {
            MessageBox(L"指纹模版已经创建完成。", L"指纹注册", MB_OK | MB_ICONSTOP);
        }
	} catch(mdldb::MDLDB_Exception& e) {
        CString msg(e.what());
        MessageBox(msg, L"指纹模版上传");
    } catch(...) {
        MessageBox(L"未知错误", L"指纹模版上传");
    }
    delete [] pPreRegTemplate;
	delete [] pRegTemplate;
}

LRESULT CRegisterDlg::OnFpNotify(WPARAM wParam, LPARAM lParam) {
	if (m_idnumber == "")
		return S_OK;

	switch(wParam) {
	case WN_COMPLETED: {
		SetDlgItemText(IDC_REG_STATUS, L"指纹图像已获取");
		DATA_BLOB* pImageBlob = reinterpret_cast<DATA_BLOB*>(lParam);
		DisplayFingerprintImage(pImageBlob);
		AddToEnroll(pImageBlob->pbData, pImageBlob->cbData);
		break;
					   }
	case WN_ERROR: {
		TCHAR buffer[101] = {0};
		_sntprintf_s(buffer, 100, _TRUNCATE, _T("发生错误。错误代码: 0x%X"), lParam);
		AddStatus(buffer);
		break;
				   }
	case WN_DISCONNECT:
		/* m_pPrompt->GetDC()->SetBkColor(RGB(255, 0, 0));*/
		SetDlgItemText(IDC_REG_STATUS, L"指纹识别仪没有连接或失去连接");
		break;
	case WN_RECONNECT:
		SetDlgItemText(IDC_REG_STATUS, L"指纹识别仪已连接");
		break;
	case WN_FINGER_TOUCHED:
		SetDlgItemText(IDC_REG_STATUS, L"手指已接触识别区");
		break;
	case WN_FINGER_GONE:
		SetDlgItemText(IDC_REG_STATUS, L"手指离开识别区");
		break;
	case WN_IMAGE_READY:
		SetDlgItemText(IDC_REG_STATUS, L"指纹图像已准备好");
		break;
	case WN_OPERATION_STOPPED:
		SetDlgItemText(IDC_REG_STATUS, L"指纹注册操作停止");
		break;
	}
	return S_OK;
}
void CRegisterDlg::OnEnKillfocusIdnumber()
{
	CString str;
	GetDlgItemText(IDC_REG_IDNUMBER, str);
	if (str == L"")
		SetDlgItemText(IDC_REG_IDNUMBER, L"请在这里输入学号");
}

void CRegisterDlg::OnEnSetfocusIdnumber()
{
	CString str;
	GetDlgItemText(IDC_REG_IDNUMBER, str);
	if (str == L"请在这里输入学号")
		SetDlgItemText(IDC_REG_IDNUMBER, L"");
}

void CRegisterDlg::OnBnClickedExit()
{
	this->EndDialog(ID_EXIT);
}

void CRegisterDlg::OnBnClickedBack()
{
	this->EndDialog(ID_BACK);
}
