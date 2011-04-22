#include "MDLDB_Connector.h"


MDLDB_Connector::MDLDB_Connector(void)
{
	this->state = NULL;
	this->connection = NULL;
	this->db_status = MDLDB_DISCONNECTED;
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
	this->db_status = MDLDB_DISCONNECTED;
	this->driver = sql::mysql::get_driver_instance();
	this->connect(db_host, db_user, db_passwd, course_name, session_name);
}

MDLDB_Connector::~MDLDB_Connector(void)
{
	if (this->state != NULL)
		delete this->connection;
	if (this->connection != NULL)
		delete this->state;
}

status_t MDLDB_Connector::connect(const char * const db_host,
								  const char * const db_user,
								  const char * const db_passwd,
								  const string course_name,
								  const string session_name
								  )
{
	this->connection = this->driver->connect(db_host, db_user, db_passwd);
	this->state = this->connection->createStatement();
	this->state->execute("USE moodle");

	time_t now = time(NULL);

	string sql_where("fullname=\"");
	sql_where+= course_name + "\"";

	sql::PreparedStatement *prep_stmt = this->connection->prepareStatement("SELECT ? FROM ? WHERE sessdate BETWEEN ? AND ?");

	//get session_id from mdl_attendance
	sql::PreparedStatement *prep_stmt = this->connection->prepareStatement("SELECT ? FROM ? WHERE sessdate BETWEEN ? AND ?");
	prep_stmt->setString(1, "id");
	prep_stmt->setString(2, "mdl_attendance_sessions");
	prep_stmt->setInt(3, now - THERE_HOURS);
	prep_stmt->setInt(4, now + THERE_HOURS);
	sql::ResultSet *rs = prep_stmt->executeQuery();
	if (rs->rowsCount() == 1) {
		this->session_id = rs->first()->getInt("id");
	delete rs;
	if (rs->rowsCount() > 1)
		return MDLDB_DUPLICATE_SESSION;
	else
		return MDLDB_NO_SESSION;

#ifdef DEBUG
	sprintf(stdout_buffer, "connect to %s success", db_host);
	print_msg(stdout_buffer);
#endif
}

sql::ResultSet* MDLDB_Connector::sql_select(const char* const field,
											const char* const table,
											const char* const sql_where
											)
{
	sql::PreparedStatement *prep_stmt = this->connection->prepareStatement("SELECT ? FROM ? WHERE ?");
	prep_stmt->setString(1, field);
	prep_stmt->setString(2, table);
	prep_stmt->setString(3, sql_where);
	sql::ResultSet* rs = prep_stmt->executeQuery();
	delete prep_stmt;
	return rs;
}