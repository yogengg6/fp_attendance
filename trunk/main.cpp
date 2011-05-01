/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author1      : Yusuke(Qiuchengxuan@gmail.com)
 * Author2      : Edward(Edward.zhw@gmail.com)
 * Date	        : 2011-4-23 12:33
 */

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
        MDLDB_Connector conn(db_host, db_user, db_passwd);
        if (conn.is_valid())
            cout << "connect to database success" << endl;
    } catch (MDLDB_Exception& e) {
        cout << e.what() << endl;
    }
	return 0;  
}  