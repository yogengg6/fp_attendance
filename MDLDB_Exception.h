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

using namespace std;

const int MDLDB_CONNECTED           =  0;
const int MDLDB_GENERAL_ERROR       = -1;

//CONNECTION ERRORS
const int MDLDB_DISCONNECTED        = -2;
const int MDLDB_CONNECTION_FAIL     = -3;
const int MDLDB_CONNECTION_UNKNOW	= -4;
const int MDLDB_CONNECTION_REFUSED  = -5;
const int MDLDB_UNCOMPLT_CONNECTION = -6;

//COURSE ASSOCIATION ERRORS
const int MDLDB_NO_COURSE           = -6;
const int MDLDB_COURSE_NOT_FOUND    = -7;
const int MDLDB_DUPLICATE_COURSE    = -8;

//SESSION ASSOCIATION ERRORS
const int MDLDB_NO_SESSION          = -9;
const int MDLDB_SESSION_NOT_FOUND   = -10;
const int MDLDB_DUPLICATE_SESSION   = -11;

const int MDLDB_STUDENT_NOT_FOUND    = -1000;
const int MDLDB_NO_PERMISSION        = -1001;

class MDLDB_Exception: public std::runtime_error
{
public:
    MDLDB_Exception(const std::string& msg, int err_no = MDLDB_GENERAL_ERROR);
    MDLDB_Exception();
    virtual ~MDLDB_Exception() throw() {};
    int get_error_code() const;
private:
    int err_no;
};
