/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author       : Yusuke(Qiuchengxuan@gmail.com)
 * Date	        : 2011-5-11 20:29
 */
// RegisterDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ultility.h"
#include "FP_Attendance.h"
#include "RegisterDlg.h"


// CRegisterDlg �Ի���

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
		MessageBox(L"����������ȡ������ʧ�ܡ�", L"ָ��ע��", MB_OK | MB_ICONSTOP);
	}

	if (FT_OK != (rc = MC_createContext(&m_mcContext))) {
		MessageBox(_T("�����ȶ�������ʧ�ܡ�"), _T("ָ��ע��"), MB_OK | MB_ICONSTOP);
	}

	MC_SETTINGS mcSettings = {0};
	if (FT_OK != (rc = MC_getSettings(&mcSettings))) {
		MessageBox(_T("��ȡԤ����ָ����������ȡ����ʧ�ܡ�"), _T("ָ��ע��"), MB_OK | MB_ICONSTOP);
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


// CRegisterDlg ��Ϣ�������

BOOL CRegisterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pRegStatus		= (CEdit*)	  GetDlgItem(IDC_REG_STATUS);
	m_pRegNotify		= (CListBox*) GetDlgItem(IDC_REG_NOTIFY);
	m_pStaticDrawArea	= (CStatic*)  GetDlgItem(IDC_STATIC_DRAWAREA);

	m_pStaticDrawArea->GetClientRect(&m_rectDrawArea);

	SetDlgItemText(IDC_REG_IDNUMBER, L"������������ѧ��");
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

		CString verify_notice = CString(L"ѧ��:") + CString(m_studentInfo.get_idnumber().c_str());
		verify_notice.AppendFormat(	L"\n����:%s\nָ��:%s", 
									stringToCString(fullname), 
									(m_studentInfo.get_fpsize() > 0) ? L"��ע��": L"δע��"
									);


		if (MessageBox(verify_notice, L"������Ϣȷ��", MB_OKCANCEL) == TRUE) {
			m_idnumber = idnumber;
			CString str;

			m_pRegStatus->EnableWindow(TRUE);
			m_pRegNotify->EnableWindow(TRUE);
			m_pStaticDrawArea->EnableWindow(TRUE);

			str.Format(L"%d��", m_nPreRegFtrs);
			SetDlgItemText(IDC_REG_STATUS, L"���Ѿ����Կ�ʼɨ�����ָ�ơ�");
			SetDlgItemText(IDC_REG_FULLNAME, stringToCString(fullname));
			AddStatus(L"�뽫��ָ�Ӵ�ɨ��������ɲ������ܹ���Ҫ" + str);
		} else {
			m_idnumber = "";
			SetDlgItemText(IDC_REG_IDNUMBER, L"������������ѧ��");
			SetDlgItemText(IDC_REG_FULLNAME, L"");
		}
	} catch (mdldb::MDLDB_Exception& e) {
		m_idnumber = "";
		MessageBox(CString(e.what()), L"ȷ��");
		SetDlgItemText(IDC_REG_IDNUMBER, L"������������ѧ��");
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
                CString str("ָ���������ɹ�����");
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

                        AddStatus(_T("ָ��ģ��ɹ�����, ���Ѿ��ɹ����ע�ᡣ"));

                        MessageBox( L"ָ��ģ��ɹ�����, ���Ѿ��ɹ����ע�ᡣ", 
                                    L"ָ��ע��");

						m_pRegNotify->ResetContent();

						m_pRegStatus->EnableWindow(FALSE);
						m_pRegNotify->EnableWindow(FALSE);
						m_pStaticDrawArea->EnableWindow(FALSE);

                        SetDlgItemText(IDC_REG_STATUS, 
                                       L"���Ѿ����ע��");
                        SetDlgItemText(IDC_REG_FULLNAME, L"");
                        SetDlgItemText(IDC_REG_IDNUMBER, L"������������ѧ��");
                    }
                    else {
                        MessageBox( _T("����ָ��ģ��ʧ�ܡ�"),
                                    _T("ָ��ע��"), 
                                    MB_OK | MB_ICONINFORMATION);
                        SetDlgItemText( IDC_REG_STATUS, 
                                        _T("ɨ�����ָ����ע�ᡣ"));
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
                    SetDlgItemText(IDC_REG_STATUS, L"�ٴ�ɨ����ͬ����ָ");
                }
            }
            else {
                MessageBox(L"����ָ��������ʧ�ܡ�",  L"ָ��ע��", 
                           MB_OK | MB_ICONINFORMATION);
                SetDlgItemText(IDC_REG_STATUS, L"ɨ�����ָ����ע�ᡣ");
            }
        }
        else {
            MessageBox(L"ָ��ģ���Ѿ�������ɡ�", L"ָ��ע��", MB_OK | MB_ICONSTOP);
        }
	} catch(mdldb::MDLDB_Exception& e) {
        CString msg(e.what());
        MessageBox(msg, L"ָ��ģ���ϴ�");
    } catch(...) {
        MessageBox(L"δ֪����", L"ָ��ģ���ϴ�");
    }
    delete [] pPreRegTemplate;
	delete [] pRegTemplate;
}

LRESULT CRegisterDlg::OnFpNotify(WPARAM wParam, LPARAM lParam) {
	if (m_idnumber == "")
		return S_OK;

	switch(wParam) {
	case WN_COMPLETED: {
		SetDlgItemText(IDC_REG_STATUS, L"ָ��ͼ���ѻ�ȡ");
		DATA_BLOB* pImageBlob = reinterpret_cast<DATA_BLOB*>(lParam);
		DisplayFingerprintImage(pImageBlob);
		AddToEnroll(pImageBlob->pbData, pImageBlob->cbData);
		break;
					   }
	case WN_ERROR: {
		TCHAR buffer[101] = {0};
		_sntprintf_s(buffer, 100, _TRUNCATE, _T("�������󡣴������: 0x%X"), lParam);
		AddStatus(buffer);
		break;
				   }
	case WN_DISCONNECT:
		/* m_pPrompt->GetDC()->SetBkColor(RGB(255, 0, 0));*/
		SetDlgItemText(IDC_REG_STATUS, L"ָ��ʶ����û�����ӻ�ʧȥ����");
		break;
	case WN_RECONNECT:
		SetDlgItemText(IDC_REG_STATUS, L"ָ��ʶ����������");
		break;
	case WN_FINGER_TOUCHED:
		SetDlgItemText(IDC_REG_STATUS, L"��ָ�ѽӴ�ʶ����");
		break;
	case WN_FINGER_GONE:
		SetDlgItemText(IDC_REG_STATUS, L"��ָ�뿪ʶ����");
		break;
	case WN_IMAGE_READY:
		SetDlgItemText(IDC_REG_STATUS, L"ָ��ͼ����׼����");
		break;
	case WN_OPERATION_STOPPED:
		SetDlgItemText(IDC_REG_STATUS, L"ָ��ע�����ֹͣ");
		break;
	}
	return S_OK;
}
void CRegisterDlg::OnEnKillfocusIdnumber()
{
	CString str;
	GetDlgItemText(IDC_REG_IDNUMBER, str);
	if (str == L"")
		SetDlgItemText(IDC_REG_IDNUMBER, L"������������ѧ��");
}

void CRegisterDlg::OnEnSetfocusIdnumber()
{
	CString str;
	GetDlgItemText(IDC_REG_IDNUMBER, str);
	if (str == L"������������ѧ��")
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
