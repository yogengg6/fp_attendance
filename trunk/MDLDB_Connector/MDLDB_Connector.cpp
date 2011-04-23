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
	this->status = MDLDB_DISCONNECTED;
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
    this->status = MDLDB_DISCONNECTED;
    this->driver = sql::mysql::get_driver_instance();
    this->dbconnect(db_host, db_user, db_passwd);
    if (this->status != MDLDB_CONNECTED)
        return;
    this->associate_course(course_name);
    if (this->status != MDLDB_CONNECTED)
        return;
    this->associate_session(session_name);
    if (this->status == MDLDB_CONNECTED)
        this->status = MDLDB_OK;
}

MDLDB_Connector::~MDLDB_Connector(void)
{
    if (this->statement != NULL)
        delete this->connection;
    if (this->connection != NULL)
        delete this->statement;
}

/*
 * create connection to MySQL Database server
 */
status_t MDLDB_Connector::dbconnect(const char * const db_host,
                                    const char * const db_user,
                                    const char * const db_passwd
                                    )
{
    try {
        this->connection = this->driver->connect(db_host, db_user, db_passwd);
    } catch (sql::SQLException &e) {
        if (e.getErrorCode() == 10061) {
            this->status = MDLDB_CONNECTION_FAIL;
        } else if (e.getErrorCode() == 1045) {
            this->status = MDLDB_CONNECTION_REFUSED;
        }
        return this->status;
    }
    this->connection->setSchema("moodle");
    this->statement = this->connection->createStatement();
    this->statement->execute("SET NAMES utf8");
    this->status = MDLDB_CONNECTED;
    return this->status;
}


/*
 * check and get course_id from mdl_course
 */
status_t MDLDB_Connector::associate_course(const string course_name)
{
    sql::PreparedStatement *prep_stmt = NULL;
    try {
        prep_stmt = this->connection->prepareStatement("SELECT id FROM mdl_course WHERE fullname=?");
    } catch (sql::SQLException &e) {
        cout << e.what() << endl;
        this->status = MDLDB_UNKNOWN_ERROR;
        return MDLDB_UNKNOWN_ERROR;
    }
    
	prep_stmt->setString(1, course_name);
    sql::ResultSet *rs = prep_stmt->executeQuery();
    switch (rs->rowsCount()) {
    case 0:
        this->status = MDLDB_NO_COURSE;
        break;
    case 1:
        rs->next();
        this->session_id = rs->getInt("id");
        break;
    default:
        this->status = MDLDB_DUPLICATE_COURSE;
    }
    delete rs;
    delete prep_stmt;
    return this->status;
    return MDLDB_CONNECTED;
}

/*
 * check and get session_id from mdl_attendance
 */
status_t MDLDB_Connector::associate_session(const string session_name)
{
    time_t now = time(NULL);
    sql::PreparedStatement *prep_stmt = NULL;
    static const char* prep_sql = "SELECT id FROM mdl_attendance_sessions WHERE fullname=? AND sessdate BETWEEN ? AND ?";

    try {
        prep_stmt = this->connection->prepareStatement(prep_sql);
    } catch(sql::SQLException &e) {
        cout << e.what() << endl;
        this->status = MDLDB_UNKNOWN_ERROR;
        return MDLDB_UNKNOWN_ERROR;
    }
    
    prep_stmt->setString(1, session_name);
    prep_stmt->setInt(2, static_cast<unsigned long>(now - THERE_HOURS));
    prep_stmt->setInt(3, static_cast<unsigned long>(now + THERE_HOURS));
    sql::ResultSet *rs = prep_stmt->executeQuery();
	switch (rs->rowsCount()) {
	case 0:
		this->status = MDLDB_NO_SESSION;
		break;
	case 1:
		rs->next();
		this->session_id = rs->getInt("id");
		break;
	default:
		this->status = MDLDB_DUPLICATE_SESSION;
	}
	delete rs;
	return this->status;
}