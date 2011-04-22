#pragma once
#pragma comment (lib, "mysqlcppconn.lib")

#include <cstdio>
#include <cstdarg>
#include <ctime>

#include <iostream>

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>

#include "../common.h"

#ifndef FPA_COMMON
#define print_msg(m) printf("MESSAGE[%s]: %s\n", __FUNCTION__, (m));
#define print_err(e) printf("ERROR[%s at %s line %s]: %s\n", __FUNCTION__, __FILE__, __LINE__, (e));
typedef int status_t;
const int STDOUT_BUFFER_SIZE = 400;
char stdout_buffer[STDOUT_BUFFER_SIZE];
#endif

using namespace std;

const status_t MDLDB_OK                  =  0;
const status_t MDLDB_CONNECTED           = -1;
const status_t MDLDB_DISCONNECTED        = -2;
const status_t MDLDB_CONNECTION_FAIL     = -3;
const status_t MDLDB_CONNECTION_REFUSED  = -4;
const status_t MDLDB_NO_SESSION          = -5;
const status_t MDLDB_DUPLICATE_SESSION   = -6;
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
    status_t dbconnect(const char * const db_host,
                       const char * const db_user,
                       const char * const db_passwd
                       );
    status_t associate_course(const string course_name);
    status_t associate_session(const string session_name);
protected:
private:
    uint32_t course_id;
    uint32_t session_id;
    status_t status;
    sql::Statement *state;
    sql::Connection *connection;
    sql::mysql::MySQL_Driver *driver;
};

