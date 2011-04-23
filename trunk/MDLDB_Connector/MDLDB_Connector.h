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

using namespace std;

const time_t   THERE_HOURS               =  3 * 60 * 60; 

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
    int dbconnect(const char * const db_host,
                       const char * const db_user,
                       const char * const db_passwd
                       )
                       throw(MDLDB_Exception);
    int associate_course(const string course_name);
    int associate_session(const string session_name);
    int get_status() const {return this->status;};
protected:
private:
    uint32_t course_id;
    uint32_t session_id;
    int status;
    sql::Statement *statement;
    sql::Connection *connection;
    sql::mysql::MySQL_Driver *driver;
};