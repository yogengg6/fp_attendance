/**
 * Copyleft : This program is published under GPL
 * Author   : Yusuke
 * Email    : Qiuchengxuan@gmail.com
 * Date	    : 2011-4-22 21:18
 */

#include "MDLDB_Connector.h"

MDLDB_Connector::MDLDB_Connector(void)
{
	this->statement = NULL;
	this->connection = NULL;
    this->driver = sql::mysql::get_driver_instance();
}
MDLDB_Connector::MDLDB_Connector(const char * const db_host,
                                 const char * const db_user,
                                 const char * const db_passwd,
                                 const string course_name,
                                 const string session_name
                                 )
{
    this->statement = NULL;
    this->connection = NULL;
    this->driver = sql::mysql::get_driver_instance();
    try {
        this->dbconnect(db_host, db_user, db_passwd);
        this->associate_course(course_name);
        this->associate_session(session_name);
    } catch (MDLDB_Exception& e) {
        std::cout << e.what() << std::endl;
        delete this->statement;
        delete this->connection;
        throw;
    }
}

MDLDB_Connector::~MDLDB_Connector(void)
{
    if (this->statement != NULL)
        delete this->connection;
    if (this->connection != NULL)
        delete this->statement;
}

/**
 * create connection to MySQL Database server
 */
bool MDLDB_Connector::dbconnect(const char * const db_host,
                               const char * const db_user,
                               const char * const db_passwd
                               )
                               throw(MDLDB_Exception)
{
    try {
        this->connection = this->driver->connect(db_host, db_user, db_passwd);
        this->connection->setSchema("moodle");
        this->statement = this->connection->createStatement();
        this->statement->execute("SET NAMES utf8");
    } catch (sql::SQLException &e) {
        if (e.getErrorCode() == 10061) {
            throw MDLDB_Exception("[MDLDB]:failed to connect dbhost", MDLDB_CONNECTION_FAIL);
        } else if (e.getErrorCode() == 1045) {
            throw MDLDB_Exception("[MDLDB]:connection refused from dbhost", MDLDB_CONNECTION_REFUSED);
        }
    }
    return this->connection != NULL;
}


/**
 * check and get course_id from mdl_course
 */
bool MDLDB_Connector::associate_course(const string course_name) throw(MDLDB_Exception)
{
    sql::PreparedStatement *prep_stmt = NULL;
    sql::ResultSet *rs = NULL;
    try {
        prep_stmt = this->connection->prepareStatement("SELECT id FROM mdl_course WHERE fullname=?");
        prep_stmt->setString(1, course_name);
        rs = prep_stmt->executeQuery();
        switch (rs->rowsCount()) {
        case 0:
            throw MDLDB_Exception("[MDLDB]:course not found", MDLDB_NO_COURSE);
            break;
        case 1:
            rs->next();
            this->course_id = rs->getInt("id");
            break;
        default:
            throw MDLDB_Exception("[MDLDB]:duplicate course", MDLDB_NO_COURSE);
            break;
        }
        delete rs;
        delete prep_stmt;
    } catch (sql::SQLException &e) {
        std::cout << e.what() << std::endl;
        if (prep_stmt != NULL)
            delete prep_stmt;
        if (rs != NULL)
            delete rs;
        throw MDLDB_Exception();
    }
    return this->course_id > 0;
}

/**
 * check and get session_id from mdl_attendance
 * notice that session
 */
bool MDLDB_Connector::associate_session(const string session_name) throw(MDLDB_Exception)
{
    if (!this->conection_established())
        throw MDLDB_Exception("connection not established", MDLDB_DISCONNECTED);
    if (!this->course_associated())
        throw MDLDB_Exception("course not associated", MDLDB_NO_SESSION);
    time_t now = time(NULL);
    static char now_str[80];
    sql::PreparedStatement *prep_stmt = NULL;
    sql::ResultSet *rs = NULL;
    static const char* prep_sql = "SELECT id FROM mdl_attendance_sessions WHERE description=? AND ? BETWEEN sessdate AND sessdate + duration";
    try {
        prep_stmt = this->connection->prepareStatement(prep_sql);
        prep_stmt->setString(1, session_name);
        sprintf_s(now_str, "%lld", now + BEFORE_CLASS_BEGIN);
        prep_stmt->setBigInt(2, string(now_str));
        rs = prep_stmt->executeQuery();
        switch (rs->rowsCount()) {
        case 0:
            throw MDLDB_Exception("[MDLDB]:session not found", MDLDB_NO_SESSION);
            break;
        case 1:
            rs->next();
            this->session_id = rs->getInt("id");
            break;
        default:
            throw MDLDB_Exception("[MDLDB]:duplicate session", MDLDB_DUPLICATE_SESSION);
            break;
        }
        delete rs;
        delete prep_stmt;
    } catch(sql::SQLException &e) {
        std::cout << e.what() << std::endl;
        if (prep_stmt != NULL)
            delete prep_stmt;
        if (rs != NULL)
            delete rs;
        throw MDLDB_Exception();
    }
    return this->session_id > 0;
}