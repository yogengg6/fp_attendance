/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author       : Yusuke(Qiuchengxuan@gmail.com)
 * Date	        : 2011-5-11 20:29
 */
// FP_Attendance.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CFP_AttendanceApp:
// 有关此类的实现，请参阅 FP_Attendance.cpp
//

class CFP_AttendanceApp : public CWinApp
{
public:
	CFP_AttendanceApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CFP_AttendanceApp theApp;