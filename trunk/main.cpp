#include <ctime>

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>

#include "MDLDB_Connector/MDLDB_Connector.h"

using namespace std;

int main()
{
    char *db_host   = "tcp://172.16.81.156:3306";
    char *db_user   = "attendance";
    char *db_passwd = "neuqcsattendance";
    string course   = "C/C++语言程序设计";
    string session  = "";
    try {
        MDLDB_Connector conn(db_host, db_user, db_passwd, course, session);
    } catch (MDLDB_Exception) {
    }
	return 0;  
}  