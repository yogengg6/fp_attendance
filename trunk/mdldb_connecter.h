#include "common.h"
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/statement.h>
#pragma once
using namespace std;

class MDLDB_Connecter
{
public:
	MDLDB_Connecter(void);
	MDLDB_Connecter(const char * const db_host,
					const char * const db_user,
					const char * const db_passwd,
					const string course_name,
					const string session_name
					);
	~MDLDB_Connecter(void);
	status_t connect(const char * const db_host,
					 const char * const db_user,
					 const char * const db_passwd,
					 const string course_name,
					 const string session_name
					 );
protected:
private:
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *connection;
};

