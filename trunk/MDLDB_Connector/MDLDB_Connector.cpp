/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author1      : Yusuke(Qiuchengxuan@gmail.com)
 * Author2      : Edward(edward9092@gmail.com)
 * Date	        : 2011-4-23 12:33
 */

#include "MDLDB_Connector.h"

/**
 * this constructor only do call dbconnect, associate_course,
 * associate_session and deal with exceptions. So that when you're
 * attempting take attendance, just use it.
 */
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
        cout << e.what() << endl;
        delete this->statement;
        delete this->connection;
        throw;
    }
}

/**
 * this constructor only call dbconnect, and deal with exceptions.
 * So that when you're attempting enroll student fingerprint, just use it.
 */
MDLDB_Connector::MDLDB_Connector(const char * const db_host,
                                 const char * const db_user,
                                 const char * const db_passwd
                                 )
{
    this->statement = NULL;
    this->connection = NULL;
    this->driver = sql::mysql::get_driver_instance();
    try {
        this->dbconnect(db_host, db_user, db_passwd);
    } catch (MDLDB_Exception& e) {
        delete this->statement;
        delete this->connection;
        throw;
    }
}

MDLDB_Connector::MDLDB_Connector(void)
{
    this->statement = NULL;
    this->connection = NULL;
    this->driver = sql::mysql::get_driver_instance();
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
            throw MDLDB_Exception("[MDLDB]:Failed to connect dbhost", MDLDB_CONNECTION_FAIL);
        } else if (e.getErrorCode() == 1045) {
            throw MDLDB_Exception("[MDLDB]:Connection refused from dbhost", MDLDB_CONNECTION_REFUSED);
        }
    }
    return this->connection != NULL;
}


/**
 * check and get course_id from mdl_course
 */
bool MDLDB_Connector::associate_course(const string course) throw(MDLDB_Exception)
{
    if (!this->connection_established())
        throw MDLDB_Exception("[MDLDB]:Connection not established", MDLDB_DISCONNECTED);
    sql::PreparedStatement *prep_stmt = NULL;
    sql::ResultSet *rs = NULL;
    try {
        this->connection->setSchema("moodle");
        prep_stmt = this->connection->prepareStatement("SELECT id FROM mdl_course WHERE fullname=?");
        prep_stmt->setString(1, course);
        rs = prep_stmt->executeQuery();
        switch (rs->rowsCount()) {
        case 0:
            throw MDLDB_Exception("[MDLDB]:Course not found", MDLDB_COURSE_NOT_FOUND);
            break;
        case 1:
            rs->next();
            this->course_id = rs->getInt("id");
            break;
        default:
            throw MDLDB_Exception("[MDLDB]:Duplicate course", MDLDB_DUPLICATE_COURSE);
            break;
        }
        delete rs;
        delete prep_stmt;
    } catch (sql::SQLException &e) {
        if (prep_stmt != NULL)
            delete prep_stmt;
        if (rs != NULL)
            delete rs;
        throw MDLDB_Exception(e.what(), MDLDB_GENERAL_ERROR);
    }
    return this->course_id > 0;
}

/**
 * check and get session_id from mdl_attendance
 * notice that session maybe duplicate
 */
bool MDLDB_Connector::associate_session(const string session) throw(MDLDB_Exception)
{
    if (!this->connection_established())
        throw MDLDB_Exception("connection not established", MDLDB_DISCONNECTED);
    if (!this->course_associated())
        throw MDLDB_Exception("course not associated", MDLDB_NO_COURSE);
    time_t now = time(NULL);
    static char now_str[80];
    sql::PreparedStatement *prep_stmt = NULL;
    sql::ResultSet *rs = NULL;
    static const char* prep_sql = "SELECT id FROM mdl_attendance_sessions WHERE courseid=? AND description=? AND ? BETWEEN sessdate AND sessdate + duration";
    try {
        this->connection->setSchema("moodle");
        prep_stmt = this->connection->prepareStatement(prep_sql);
        prep_stmt->setInt(1, this->course_id);
        prep_stmt->setString(2, session);
        sprintf_s(now_str, "%lld", now + BEFORE_CLASS_BEGIN);
        prep_stmt->setBigInt(3, string(now_str));
        rs = prep_stmt->executeQuery();
        switch (rs->rowsCount()) {
        case 0:
            throw MDLDB_Exception("[MDLDB]:session not found", MDLDB_SESSION_NOT_FOUND);
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
        if (prep_stmt != NULL)
            delete prep_stmt;
        if (rs != NULL)
            delete rs;
        throw MDLDB_Exception(e.what(), MDLDB_GENERAL_ERROR);
    }
    return this->session_id > 0;
}

/**
 * enroll student fingerprint data to database.
 * 
 */

bool MDLDB_Connector::enroll(const string &idnumber,
                             const unsigned char * const fingerprint_data,
                             size_t fingerprint_size)
{
    if (!this->connection_established())
        throw MDLDB_Exception("[MDLDB]:Connection not established", MDLDB_UNCOMPLT_CONNECTION);
    sql::PreparedStatement *prep_stmt = NULL;
    static const char* sql = "UPDATE info SET fingerprint_data=?, fingerprint_size=? where idnumber=?";
    try {
        this->connection->setSchema("student");
        prep_stmt = this->connection->prepareStatement(sql);
        std::string value(fingerprint_data, fingerprint_size);
        std::istringstream blob_stream(value);
        prep_stmt->setBlob(1, &blob_stream);
        prep_stmt->setString(2, idnumber);
    } catch(sql::SQLException& e) {
        cout << e.what() << endl;
    }
}
/**
 * get student info(including idnumber, fingerprint data) from database to local
 */
bool MDLDB_Connector::get_all_info()
{
    if (!this->is_valid())
        throw MDLDB_Exception("[MDLDB]:Connection not completed", MDLDB_UNCOMPLT_CONNECTION);
    sql::PreparedStatement *prep_stmt = NULL;
    static const char *sql = "SELECT idnumber, fingerprint_data, fingerprint_size FROM info WHERE";
    try {
        this->connection->setSchema("student");
        prep_stmt = this->connection->prepareStatement(sql);

    } catch(sql::SQLException& e) {
        cout << e.what() << endl;
    }
}