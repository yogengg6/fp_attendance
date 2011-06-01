/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author1      : Yusuke(Qiuchengxuan@gmail.com)
 * Author2      : Edward(Edward.zhw@gmail.com)
 * Date	        : 2011-4-23 12:33
 */

#pragma once

#include <cppconn/exception.h>
#include <cppconn/config.h>

namespace mdldb
{
	const int CONNECTED           =  0;
	const int GENERAL_ERROR       = -1;

	//连接错误
	const int DISCONNECTED        = -2;
	const int CONNECTION_FAIL     = -3;
	const int CONNECTION_UNKNOW	= -4;
	const int CONNECTION_REFUSED  = -5;
	const int UNCOMPLT_CONNECTION = -6;

	//课程关联错误
	const int NO_COURSE           = -6;
	const int COURSE_NOT_FOUND    = -7;
	const int DUPLICATE_COURSE    = -8;

	//会话关联错误
	const int NO_SESSION          = -9;
	const int SESSION_NOT_FOUND   = -10;
	const int DUPLICATE_SESSION   = -11;

	const int STUDENT_NOT_FOUND   = -1000;
	const int NO_PERMISSION       = -1001;

	const int NO_USER			  = -1002;
	const int INCORRECT_PASSWD	  = -1003;

	class MDLDB_Exception: public std::runtime_error
	{
	public:
		MDLDB_Exception(const std::string& msg, int err_no = GENERAL_ERROR);
		MDLDB_Exception();
		virtual ~MDLDB_Exception() throw() {};
		int get_error_code() const;
	private:
		int err_no;
	};
}
