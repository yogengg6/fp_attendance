#include "MDLDB_Connector.h"


MDLDB_Connector::MDLDB_Connector(void)
{
	this->state = NULL;
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
    this->state = NULL;
    this->connection = NULL;
    this->status = MDLDB_DISCONNECTED;
    this->driver = sql::mysql::get_driver_instance();
    this->dbconnect(db_host, db_user, db_passwd);
    if (this->status != MDLDB_CONNECTED)
        return;
}

MDLDB_Connector::~MDLDB_Connector(void)
{
    if (this->state != NULL)
        delete this->connection;
    if (this->connection != NULL)
        delete this->state;
}

status_t MDLDB_Connector::dbconnect(const char * const db_host,
                                    const char * const db_user,
                                    const char * const db_passwd
                                    )
{
    try {
        this->connection = this->driver->connect(db_host, db_user, db_passwd);
    } catch (sql::SQLException &e) {
        if (e.getErrorCode() == 10061) {
            #ifdef DEBUG
            sprintf(stdout_buffer, "connect to %s fail", db_host);
            print_msg(stdout_buffer);
            #endif
            this->status = MDLDB_CONNECTION_FAIL;
        } else if (e.getErrorCode() == 1045) {
            #ifdef DEBUG
            print_msg("connection refused");
            #endif
            this->status = MDLDB_CONNECTION_REFUSED;
        }
        return this->status;
    }
    this->state->execute("USE moodle");
    this->state = this->connection->createStatement();
    this->status = MDLDB_CONNECTED;
    return this->status;
}

status_t MDLDB_Connector::associate_course(const string course_name)
{
    sql::PreparedStatement *prep_stmt = this->connection->prepareStatement("SELECT id FROM mdl_session WHERE fullname=\"?\"");
    return MDLDB_CONNECTED;
}

status_t MDLDB_Connector::associate_session(const string session_name)
{
    //get session_id from mdl_attendance
    time_t now = time(NULL);
    static const char* prep_sql = "SELECT id FROM mdl_attendance_sessions WHERE fullname=\"?\" AND sessdate BETWEEN ? AND ?";
    sql::PreparedStatement *prep_stmt = this->connection->prepareStatement(prep_sql);
    prep_stmt->setString(1, session_name);
    prep_stmt->setInt(2, static_cast<unsigned long>(now - THERE_HOURS));
    prep_stmt->setInt(3, static_cast<unsigned long>(now + THERE_HOURS));
    sql::ResultSet *rs = prep_stmt->executeQuery();
    int rows_count = rs->rowsCount();
    if (rows_count == 0) {
		this->status = MDLDB_NO_SESSION;
	} else if (rows_count == 1) {
		rs->next();
		this->session_id = rs->getInt("id");
		this->status = MDLDB_OK;
    } else {
        this->status = MDLDB_DUPLICATE_SESSION;
	}
	delete rs;
	return this->status;

#ifdef DEBUG
    sprintf(stdout_buffer, "connect to %s success", db_host);
    print_msg(stdout_buffer);
#endif
}