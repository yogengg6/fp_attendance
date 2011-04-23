/**
 * Copyleft : This program is published under GPL
 * Author   : Yusuke
 * Email    : Qiuchengxuan@gmail.com
 * Date	    : 2011-4-22 21:18
 */

#pragma once
#pragma comment (lib, "mysqlcppconn.lib")

//c header
#include <cstdio>
#include <cstdarg>
#include <ctime>

//c++ header
#include <iostream>

//mysql header
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>

#include "MDLDB_Exception.h"

#ifdef __GNUC__
#define sprintf_s(buffer, format, ...) sprintf(buffer, format, __VA_ARGS__)
#endif

using namespace std;

const time_t  BEFORE_CLASS_BEGIN =  10 * 60 - 1;

class MDLDB_Connector
{
public:
    MDLDB_Connector(void);
    MDLDB_Connector(const char * const db_host,
                    const char * const db_user,
                    const char * const db_passwd,
                    const string course_name,
                    const string session_name
                    );
    ~MDLDB_Connector(void);
    bool dbconnect(const char * const db_host,
                       const char * const db_user,
                       const char * const db_passwd
                       )
                       throw(MDLDB_Exception);
    bool associate_course(const string course_name) throw(MDLDB_Exception);
    bool associate_session(const string session_name) throw(MDLDB_Exception);

    inline bool is_valid() const
    {
        return  (this->connection != NULL) && (this->course_id > 0) && (this->session_id > 0);
    }
    inline bool connection_established() const {return this->connection != NULL;}
    inline bool course_associated() const {return this->course_id > 0;}
    inline bool session_associated() const {return this->session_id > 0;}
protected:
private:
    uint32_t course_id;
    uint32_t session_id;
    sql::Statement *statement;
    sql::Connection *connection;
    sql::mysql::MySQL_Driver *driver;
};