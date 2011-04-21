#pragma once

#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <cstdint>

#include <iostream>

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include "common.h"
using namespace std;

const status_t MDLDB_OK					=  0;
const status_t MDLDB_DISCONNECTED		= -1;
const status_t MDLDB_NO_SESSION			= -2;
const status_t MDLDB_DUPLICATE_SESSION	= -3;
const time_t   THERE_HOURS				= 3 * 60 * 60; 

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
	status_t connect(const char * const db_host,
					 const char * const db_user,
					 const char * const db_passwd,
					 const string course_name,
					 const string session_name
					 );
protected:
	sql::ResultSet* sql_select(const char* const field,
						 	   const char* const table,
							   const char* const sql_where
							   );
private:
	uint32_t course_id;
	uint32_t session_id;
	status_t db_status;
	sql::Statement *state;
    sql::Connection *connection;
    sql::mysql::MySQL_Driver *driver;
};

