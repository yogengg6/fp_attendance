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

namespace mdldb{
	std::auto_ptr<sql::mysql::MySQL_Driver>	Connector::m_driver(NULL);
	std::auto_ptr<sql::Statement>			Connector::m_statement(NULL);
	std::auto_ptr<sql::Connection>			Connector::m_connection(NULL);


	/**
	* this constructor only do call dbconnect, associate_course,
	* associate_session and deal with exceptions. So that when you're
	* attempting take attendance, just use it.
	*/
	Connector::Connector(const char * const db_host,
						 const char * const db_user,
						 const char * const db_passwd,
						 const std::string	course_name,
						 const std::string	session_name
						 )
	{
		try {
			if (m_driver.get() == NULL)
				m_driver = std::auto_ptr<sql::mysql::MySQL_Driver>(sql::mysql::get_driver_instance());
			if (m_connection.get() == NULL)
				dbconnect(db_host, db_user, db_passwd);
			associate_course(course_name);
			associate_session(session_name);
		} catch (MDLDB_Exception& e) {
			throw;
		}
	}

	/**
	* this constructor only call dbconnect, and deal with exceptions.
	* So that when you're attempting enroll student fingerprint, just use it.
	*/
	Connector::Connector(const char * const db_host,
						 const char * const db_user,
						 const char * const db_passwd
						)
	{
		try {
			if (m_driver.get() == NULL)
				m_driver = std::auto_ptr<sql::mysql::MySQL_Driver>(sql::mysql::get_driver_instance());
			if (m_connection.get() == NULL)
				dbconnect(db_host, db_user, db_passwd);
		} catch (MDLDB_Exception& e) {
			throw;
		}
	}

	Connector::Connector(Connector& conn)
	{
		m_course_id     = conn.m_course_id;
		m_session_id    = conn.m_session_id;
	}

	Connector::Connector(void)
	{
		if (m_driver.get() == NULL)
			m_driver = std::auto_ptr<sql::mysql::MySQL_Driver>(sql::mysql::get_driver_instance());
	}

	Connector::~Connector(void)
	{
	}

	/**
	* create connection to MySQL Database server
	*/
	bool Connector::dbconnect(const char * const db_host,
							  const char * const db_user,
							  const char * const db_passwd
							  ) throw(MDLDB_Exception)
	{
		try {
			m_connection = std::auto_ptr<sql::Connection>(m_driver->connect(db_host, db_user, db_passwd));
			m_connection->setSchema("moodle");
			m_statement = std::auto_ptr<sql::Statement>(m_connection->createStatement());
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
		return m_connection.get() != NULL;
	}

	void Connector::associate_course(uint32_t id)
	{
		m_course_id = id;
	}

	/**
	* check and get course_id from mdl_course
	*/
	bool Connector::associate_course(const std::string course) throw(MDLDB_Exception)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", MDLDB_DISCONNECTED);

		static const char* prep_sql = "SELECT id FROM mdl_course WHERE fullname=?";

		try {
			m_connection->setSchema("moodle");
			std::auto_ptr<sql::PreparedStatement> prep_stmt(m_connection->prepareStatement(prep_sql));
			prep_stmt->setString(1, course);

			std::auto_ptr<sql::ResultSet> rs(prep_stmt->executeQuery());
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
		} catch (sql::SQLException &e) {
			throw MDLDB_Exception(e.what(), MDLDB_GENERAL_ERROR);
		}
		return m_course_id > 0;
	}

	/**
	* check and get session_id from mdl_attendance
	* notice that session maybe duplicate
	*/
	bool Connector::associate_session(const std::string session) throw(MDLDB_Exception)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", MDLDB_DISCONNECTED);

		if (!course_associated())
			throw MDLDB_Exception("course not associated", MDLDB_NO_COURSE);

		uint32_t now = (uint32_t)time(NULL);
		
		static const char* prep_sql = "SELECT id FROM mdl_attendance_sessions WHERE courseid=? AND description=? AND ? BETWEEN sessdate AND sessdate + duration";
		try {
			m_connection->setSchema("moodle");

			std::auto_ptr<sql::PreparedStatement> prep_stmt(m_connection->prepareStatement(prep_sql));
			prep_stmt->setInt(1, m_course_id);
			prep_stmt->setString(2, session);
			prep_stmt->setUInt(3, now);

			std::auto_ptr<sql::ResultSet> rs(prep_stmt->executeQuery());
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
		} catch(sql::SQLException &e) {
			throw MDLDB_Exception(e.what(), MDLDB_GENERAL_ERROR);
		}
		return m_session_id > 0;
	}

	std::vector<CourseInfo> Connector::get_all_course()
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", MDLDB_DISCONNECTED);

		static const char* sql = "SELECT id, fullname FROM mdl_course";

		try {
			m_connection->setSchema("moodle");

			std::auto_ptr<sql::ResultSet> rs(m_statement->executeQuery(sql));

			std::vector<CourseInfo> ci(rs->rowsCount());

			for (int i = 0; rs->next(); ++i) {
				ci[i].id = rs->getUInt("id");
				ci[i].fullname = rs->getString("fullname");
			}

			return ci;
		} catch(sql::SQLException& e) {
			throw MDLDB_Exception(e.what(), MDLDB_GENERAL_ERROR);
		}
		
	}

	std::vector<std::string> Connector::get_session_discription(uint32_t course_id)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", MDLDB_DISCONNECTED);

		if (course_id == 0)
			course_id = m_course_id;

		time_t now = time(NULL);

		static const char* sql = "SELECT DISTINCT(description) FROM mdl_attendance_sessions WHERE courseid=? AND ? BETWEEN sessdate AND sessdate + duration";

		try {
			m_connection->setSchema("moodle");

			std::auto_ptr<sql::PreparedStatement> prep_stmt(m_connection->prepareStatement(sql));

			prep_stmt->setUInt(1, course_id);
			prep_stmt->setUInt(2, now);

			std::auto_ptr<sql::ResultSet> rs(prep_stmt->executeQuery());

			std::vector<std::string> session_info(rs->rowsCount());

			for (int i = 0; rs->next(); ++i)
				session_info[i] = rs->getString("description");

			return session_info;
		} catch(sql::SQLException& e) {
			throw MDLDB_Exception(e.what(), MDLDB_GENERAL_ERROR);
		}
	}

	/**
	* enroll student fingerprint data to database.
	* 
	*/

	bool Connector::enroll(StudentInfo& stu_info)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", MDLDB_DISCONNECTED);

		static const char* sql = "UPDATE info SET fp_data=?, fp_size=? WHERE idnumber=?";

		try {
			m_connection->setSchema("student");

			std::auto_ptr<sql::PreparedStatement>prep_stmt(m_connection->prepareStatement(sql));

			std::string buffer((char*)stu_info.get_fpdata(), stu_info.get_fpsize() * sizeof(byte));

			std::istringstream str_stream(buffer);

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

	size_t Connector::get_all_info(StudentInfo* student_list, 
								   const std::string class1/* ="" */, 
								   const std::string class2/* ="" */) const
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", MDLDB_DISCONNECTED);

		std::auto_ptr<sql::PreparedStatement> prep_stmt;

		static const std::string SELECTOR = 
			"SELECT idnumber, fullname, fp_data, fp_size FROM info ";
		static const std::string ONE_CLASS_INFO = 
			SELECTOR + "WHERE idnumber LIKE ? AND idnumber LIKE ?";
		static const std::string TWO_CLASS_INFO = 
			SELECTOR + "WHERE idnumber LIKE ?";
		static const std::string& ALL_COURSE_INFO = SELECTOR;

		try {
			m_connection->setSchema("student");
			if (class2 == "") {
				if (class1 == "") {
					prep_stmt = std::auto_ptr<sql::PreparedStatement>(m_connection->prepareStatement(ALL_COURSE_INFO));
				} else {
					prep_stmt = std::auto_ptr<sql::PreparedStatement>(m_connection->prepareStatement(ONE_CLASS_INFO));
					prep_stmt->setString(1, class1+"%");
				}
			} else {
				prep_stmt = std::auto_ptr<sql::PreparedStatement>(m_connection->prepareStatement(TWO_CLASS_INFO));
				prep_stmt->setString(1, class1+"%");
				prep_stmt->setString(2, class2+"%");
			}

			std::auto_ptr<sql::ResultSet>rs(prep_stmt->executeQuery());
			size_t sisize = rs->rowsCount();
			student_list = new StudentInfo[sisize];

			while(rs->next())
			{
				StudentInfo* sp = student_list;
				std::string idnumber = rs->getString("idnumber");
				std::string fullname = rs->getString("fullname");
				const size_t fpsize = rs->getInt("fp_size");
				byte* fpdata = new byte[fpsize];
				std::istream * is = rs->getBlob("fp_data") ;
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
	StudentInfo Connector::get_info(const std::string& idnumber)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", MDLDB_DISCONNECTED);

		const std::string sql = "SELECT fullname, fp_size FROM info WHERE idnumber=?";

		try {
			m_connection->setSchema("student");
			std::auto_ptr<sql::PreparedStatement> prep_stmt(m_connection->prepareStatement(sql));

			prep_stmt->setString(1, idnumber);

			std::auto_ptr<sql::ResultSet> rs(prep_stmt->executeQuery());

			if (rs->rowsCount() == 1) {
				rs->next();
				std::string fullname = rs->getString("fullname");
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
}