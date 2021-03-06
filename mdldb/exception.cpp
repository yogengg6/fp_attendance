/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author1      : Yusuke(Qiuchengxuan@gmail.com)
 * Author2      : Edward(Edward.zhw@gmail.com)
 * Date	        : 2011-4-23 12:33
 */
#include "stdafx.h"
#include "exception.h"


namespace mdldb{
	MDLDB_Exception::MDLDB_Exception(const std::string& msg, int err_no) : std::runtime_error(msg)
	{
		this->err_no = err_no;
	}

	MDLDB_Exception::MDLDB_Exception() : std::runtime_error("")
	{
		this->err_no = GENERAL_ERROR;
	}

	int MDLDB_Exception::get_error_code() const
	{
		return this->err_no;
	}
}