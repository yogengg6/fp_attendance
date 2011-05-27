/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author1      : Yusuke(Qiuchengxuan@gmail.com)
 * Author2      : Edward(Edward.zhw@gmail.com)
 * Date	        : 2011-4-23 12:33
 */
#include "stdafx.h"

#include <iostream>
#include <sstream>
#include <windows.h>
#include <cppconn/sqlstring.h>

#include "MDLDB_Connector.h"
#include "MDLDB_Exception.h"
#include "MDLDB_StudentInfo.h"

#pragma warning(disable:4101)

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
								 ) : m_statement(NULL), m_connection(NULL)
{
    m_driver = sql::mysql::get_driver_instance();
    try {
        dbconnect(db_host, db_user, db_passwd);
        associate_course(course_name);
        associate_session(session_name);
    } catch (MDLDB_Exception& e) {
        delete m_statement, m_statement = NULL;
        delete m_connection, m_connection = NULL;
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
                                 ) : m_statement(NULL), m_connection(NULL)
{
    m_driver = sql::mysql::get_driver_instance();
    try {
        dbconnect(db_host, db_user, db_passwd);
    } catch (MDLDB_Exception& e) {
		delete m_statement, m_statement = NULL;
		delete m_connection, m_connection = NULL;
        throw;
    }
}

MDLDB_Connector::MDLDB_Connector(MDLDB_Connector& conn)
{
    m_course_id     = conn.m_course_id;
    m_session_id    = conn.m_session_id;
    m_statement     = conn.m_statement;
    m_connection    = conn.m_connection;
    m_driver        = conn.m_driver;
}

MDLDB_Connector::MDLDB_Connector(void)
{
    m_statement = NULL;
    m_connection = NULL;
    m_driver = sql::mysql::get_driver_instance();
}

MDLDB_Connector::~MDLDB_Connector(void)
{
	if (m_connection != NULL) {
        delete m_connection;
		m_connection = NULL;
	}
    if (m_statement != NULL) {
        delete m_statement;
		m_statement = NULL;
	}
}

/**
 * create connection to MySQL Database server
 */
bool MDLDB_Connector::dbconnect(const char * const db_host,
                                const char * const db_user,
                                const char * const db_passwd
                                ) throw(MDLDB_Exception)
{
    try {
        m_connection = m_driver->connect(db_host, db_user, db_passwd);
        m_connection->setSchema("moodle");
        m_statement = m_connection->createStatement();
        m_statement->execute("SET NAMES utf8");
    } catch (sql::SQLException &e) {
        switch (e.getErrorCode()) {
        case 1045:
            throw MDLDB_Exception("连接被拒绝", MDLDB_CONNECTION_REFUSED);
            break;
        case 10060:
        case 10061:
            throw MDLDB_Exception("连接失败", MDLDB_CONNECTION_FAIL);
            break;
        default:
            throw MDLDB_Exception(e.what(), MDLDB_CONNECTION_UNKNOW);
            break;
        }
    }
    return m_connection != NULL;
}


/**
 * check and get course_id from mdl_course
 */
bool MDLDB_Connector::associate_course(const string course) throw(MDLDB_Exception)
{
    if (!connection_established())
        throw MDLDB_Exception("[MDLDB]:Connection not established", MDLDB_DISCONNECTED);
    sql::PreparedStatement *prep_stmt = NULL;
    sql::ResultSet *rs = NULL;
    try {
        m_connection->setSchema("moodle");
        prep_stmt = m_connection->prepareStatement("SELECT id FROM mdl_course WHERE fullname=?");
        prep_stmt->setString(1, course);
        rs = prep_stmt->executeQuery();
        switch (rs->rowsCount()) {
        case 0:
            throw MDLDB_Exception("[MDLDB]:Course not found", MDLDB_COURSE_NOT_FOUND);
            break;
        case 1:
            rs->next();
            m_course_id = rs->getInt("id");
            break;
        default:
            throw MDLDB_Exception("[MDLDB]:Duplicate course", MDLDB_DUPLICATE_COURSE);
            break;
        }
        delete rs;
		rs = NULL;
        delete prep_stmt;
		prep_stmt = NULL;
    } catch (sql::SQLException &e) {
		if (prep_stmt != NULL) {
            delete prep_stmt;
			prep_stmt = NULL;
		}
		if (rs != NULL) {
            delete rs;
			rs = NULL;
		}
        throw MDLDB_Exception(e.what(), MDLDB_GENERAL_ERROR);
    }
    return m_course_id > 0;
}

/**
 * check and get session_id from mdl_attendance
 * notice that session maybe duplicate
 */
bool MDLDB_Connector::associate_session(const string session) throw(MDLDB_Exception)
{
    if (!connection_established())
        throw MDLDB_Exception("connection not established", MDLDB_DISCONNECTED);
    if (!course_associated())
        throw MDLDB_Exception("course not associated", MDLDB_NO_COURSE);
    time_t now = time(NULL);
    static char now_str[80];
    sql::PreparedStatement *prep_stmt = NULL;
    sql::ResultSet *rs = NULL;
    static const char* prep_sql = "SELECT id FROM mdl_attendance_sessions WHERE courseid=? AND description=? AND ? BETWEEN sessdate AND sessdate + duration";
    try {
        m_connection->setSchema("moodle");
        prep_stmt = m_connection->prepareStatement(prep_sql);
        prep_stmt->setInt(1, m_course_id);
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
            m_session_id = rs->getInt("id");
            break;
        default:
            throw MDLDB_Exception("[MDLDB]:duplicate session", MDLDB_DUPLICATE_SESSION);
            break;
        }
		delete rs;
		rs = NULL;
		delete prep_stmt;
		prep_stmt = NULL;
    } catch(sql::SQLException &e) {
		if (prep_stmt != NULL) {
			delete prep_stmt;
			prep_stmt = NULL;
		}
		if (rs != NULL) {
			delete rs;
			rs = NULL;
		}
        throw MDLDB_Exception(e.what(), MDLDB_GENERAL_ERROR);
    }
    return m_session_id > 0;
}

/**
 * enroll student fingerprint data to database.
 * 
 */

bool MDLDB_Connector::enroll(StudentInfo& stu_info)
{
    if (!connection_established())
        throw MDLDB_Exception("[MDLDB]:Connection not established", MDLDB_UNCOMPLT_CONNECTION);

    static const char* sql = "UPDATE info SET fp_data=?, fp_size=? WHERE idnumber=?";

    try {
        m_connection->setSchema("student");

        auto_ptr<sql::PreparedStatement>prep_stmt(m_connection->prepareStatement(sql));

        string buffer((char*)stu_info.get_fpdata(), stu_info.get_fpsize() * sizeof(byte));
        /*string buffer("A\0B", sizeof("A\0B") - 1);*/

        istringstream str_stream(buffer);

        prep_stmt->setBlob(1, &str_stream);
        prep_stmt->setInt(2, stu_info.get_fpsize());
        prep_stmt->setString(3, stu_info.get_idnumber());
        prep_stmt->executeUpdate();
    } catch(sql::SQLException& e) {
        throw MDLDB_Exception(e.what(), MDLDB_GENERAL_ERROR);
    }
    return true;
}

/**
 * get all info(including idnumber, fingerprint data, fingerprint size) from database to local
 */

size_t MDLDB_Connector::get_all_info(StudentInfo* student_list, const string class1/* ="" */, const string class2/* ="" */) const
{
    if (!is_valid())
        throw MDLDB_Exception("[MDLDB]:Connection not completed", MDLDB_UNCOMPLT_CONNECTION);

    auto_ptr<sql::PreparedStatement> prep_stmt;

    static const string SELECTOR = 
        "SELECT idnumber, fullname, fp_data, fp_size FROM info ";
    static const string ONE_CLASS_INFO = 
        SELECTOR + "WHERE idnumber LIKE ? AND idnumber LIKE ?";
    static const string TWO_CLASS_INFO = 
        SELECTOR + "WHERE idnumber LIKE ?";
    static const string& ALL_COURSE_INFO = SELECTOR;

    try {
        m_connection->setSchema("student");
        if (class2 == "") {
            if (class1 == "") {
                prep_stmt = auto_ptr<sql::PreparedStatement>(m_connection->prepareStatement(ALL_COURSE_INFO));
            } else {
                prep_stmt = auto_ptr<sql::PreparedStatement>(m_connection->prepareStatement(ONE_CLASS_INFO));
                prep_stmt->setString(1, class1+"%");
            }
        } else {
            prep_stmt = auto_ptr<sql::PreparedStatement>(m_connection->prepareStatement(TWO_CLASS_INFO));
            prep_stmt->setString(1, class1+"%");
            prep_stmt->setString(2, class2+"%");
        }
        auto_ptr<sql::ResultSet>rs(prep_stmt->executeQuery());
		size_t sisize = rs->rowsCount();
		student_list = new StudentInfo[sisize];
		while(rs->next())
		{
			StudentInfo* sp = student_list;
			string idnumber = rs->getString("idnumber");
            string fullname = rs->getString("fullname");
			const size_t fpsize = rs->getInt("fp_size");
			byte* fpdata = new byte[fpsize];
			istream * is = rs->getBlob("fp_data") ;
			*is >> const_cast<byte*>(fpdata);
			
			StudentInfo * si = new StudentInfo(idnumber, fullname, fpsize, fpdata);
			sp = si;
			sp++;
		}

    } catch(sql::SQLException& e) {
        throw;
    }
    return true;
}

/************************************************************************/
/* get student info by idnumber											*/
/************************************************************************/
StudentInfo MDLDB_Connector::get_info(const string& idnumber)
{
	if (is_valid())
		throw MDLDB_Exception("没有连接到数据库", MDLDB_UNCOMPLT_CONNECTION);

	const string sql = "SELECT fullname, fp_size FROM info WHERE idnumber=?";

    sql::ResultSet* rs = NULL;

	try {
		m_connection->setSchema("student");
        auto_ptr<sql::PreparedStatement> prep_stmt(m_connection->prepareStatement(sql));

		prep_stmt->setString(1, idnumber);

		auto_ptr<sql::ResultSet> rs(prep_stmt->executeQuery());
        
        if (rs->rowsCount() == 1) {
            rs->next();
            string fullname = rs->getString("fullname");
            size_t fpsize = rs->getInt("fp_size");

            static char conv_buf_ch[100];
            static wchar_t conv_buf_wch[100];

            return StudentInfo(idnumber, fullname, fpsize);
        } else {
            throw MDLDB_Exception("student not found", MDLDB_STUDENT_NOT_FOUND);
        }

	} catch(sql::SQLException& e) {
		throw MDLDB_Exception(e.what(), MDLDB_GENERAL_ERROR);
	}
}