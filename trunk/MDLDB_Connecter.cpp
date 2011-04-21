#include "MDLDB_Connecter.h"


MDLDB_Connecter::MDLDB_Connecter(void)
{
    this->driver = sql::mysql::get_driver_instance();
}
MDLDB_Connecter::MDLDB_Connecter(const char * const db_host,
						 		 const char * const db_user,
								 const char * const db_passwd,
								 const string course_name,
								 const string session_name
								 )
{
	this->driver = sql::mysql::get_driver_instance();
	this->connect(db_host, db_user, db_passwd, course_name, session_name);
}

MDLDB_Connecter::connect(const char * const db_host,
						 const char * const db_user,
						 const char * const db_passwd,
						 const string course_name,
						 const string session_name
						 )
{
	this->connection = driver->connect(db_host, db_user, db_passwd);
}

MDLDB_Connecter::~MDLDB_Connecter(void)
{
	delete this->connection;
}
