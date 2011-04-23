/**
 * Copyleft : This program is published under GPL
 * Author   : Yusuke
 * Email    : Qiuchengxuan@gmail.com
 * Date	    : 2011-4-22 21:18
 */

#include "MDLDB_Exception.h"

MDLDB_Exception::MDLDB_Exception(const string& msg, int err_no) : std::runtime_error(msg)
{
    this->err_no = err_no;
}

MDLDB_Exception::MDLDB_Exception() : std::runtime_error("")
{
    this->err_no = MDLDB_UNKNOWN_ERROR;
}

int MDLDB_Exception::get_error_code() const
{
    return this->err_no;
}