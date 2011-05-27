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
	 * this constructor only call dbconnect, and deal with exceptions.
	 * So that when you're attempting enroll student fingerprint, just use it.
	 */
	Connector::Connector(const char * const db_host,
						 const char * const db_user,
						 const char * const db_passwd
						 )
	{
		m_sess_info.lasttakenby = 2; // 管理员点名
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
		m_sess_info     = conn.m_sess_info;
		memcpy(m_statuses, conn.m_statuses, sizeof(m_statuses));
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
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", MDLDB_DISCONNECTED);

		m_course_id = id;

		const char* prep_sql = 
			"SELECT id FROM mdl_attendance_statuses WHERE courseid=?";

		const char* sql_statues_ordered = 
			"SELECT id FROM mdl_attendance_statuses WHERE courseid=? ORDER BY grade DESC";

		try {
			int i = 0;
			std::ostringstream sout;

			m_connection->setSchema("moodle");
			std::auto_ptr<sql::PreparedStatement> prep_stmt(m_connection->prepareStatement(prep_sql));
			prep_stmt->setUInt(1, id);

			std::auto_ptr<sql::ResultSet> rs(prep_stmt->executeQuery());
			if (rs->rowsCount() != 4)
				throw MDLDB_Exception("内部错误：错误的课程！", MDLDB_GENERAL_ERROR);

			while (rs->next())
				m_statuses[i++] = rs->getUInt("id");

			prep_stmt.release();
			prep_stmt = std::auto_ptr<sql::PreparedStatement>(m_connection->prepareStatement(sql_statues_ordered));
			prep_stmt->setUInt(1, id);

			rs.release();
			rs = std::auto_ptr<sql::ResultSet>(prep_stmt->executeQuery());
			while (rs->next())
				sout << rs->getUInt("id") << ",";

			m_statuesset = sout.str();
			m_statuesset.erase(m_statuesset.end()-1);
		} catch (sql::SQLException &e) {
			throw MDLDB_Exception(e.what(), MDLDB_GENERAL_ERROR);
		}
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
			throw MDLDB_Exception("没有关联课程", MDLDB_NO_COURSE);

		uint32_t now = (uint32_t)time(NULL);
		
		static const char* prep_sql = "SELECT id, sessdate, duration FROM mdl_attendance_sessions WHERE courseid=? AND description=? AND ? BETWEEN sessdate AND sessdate + duration";
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
			m_sess_info.id = rs->getInt("id");
			m_sess_info.sessdate = rs->getUInt("sessdate");
			m_sess_info.duration = rs->getUInt("duration");
			break;
		default:
			throw MDLDB_Exception("[MDLDB]:duplicate session", MDLDB_DUPLICATE_SESSION);
			break;
			}
		} catch(sql::SQLException &e) {
			throw MDLDB_Exception(e.what(), MDLDB_GENERAL_ERROR);
		}
		return m_sess_info.id > 0;
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

	bool Connector::attendant(std::string idnumber)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", MDLDB_DISCONNECTED);
		if (!course_associated())
			throw MDLDB_Exception("没有关联课程", MDLDB_NO_COURSE);
		if (!session_associated())
			throw MDLDB_Exception("没有关联会话", MDLDB_NO_SESSION);

		uint32_t now = time(NULL);

		return true;
	}

	/**
	 * 获取课程所有的学生指纹
	 * 1.先根据courseid获取contextid:
	 *		SELECT id FROM mdl_context WHERE instanceid=courseid
	 * 2.根据contextid得到课程所有学生的fpdata和fpsize(roleid为5代表学生)
	 *		SELECT i.fpdata, i.fpsize
	 *			FROM mdl_user u, mdl_role_assignments r student.info i 
	 *			WHERE u.id = r.userid AND r.contextid=contextid 
	 *				AND r.roleid=5 AND u.idnumber=i.idnumber;
	 */

	std::vector<StudentInfo> Connector::get_course_student_info(uint32_t courseid) const
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", MDLDB_DISCONNECTED);

		if (courseid == 0)
			courseid = m_course_id;

		const char* prep_stmt_context = 
			"SELECT id FROM mdl_context WHERE instanceid=?";
		const char* prep_stmt_fpinfo = 
			"SELECT u.idnumber, u.fullname, i.fpdata, i.fpsize \
				FROM mdl_user u, mdl_role_assignments r student.info i \
				WHERE u.id = r.userid AND r.contextid=? \
					AND r.roleid=5 AND u.idnumber=i.idnumber";

		try {

			//获取moodle课程的context
			m_connection->setSchema("student");

			std::auto_ptr<sql::PreparedStatement> prep_stmt(m_connection->prepareStatement(prep_stmt_context));
			prep_stmt->setUInt(1, courseid);

			std::auto_ptr<sql::ResultSet> rs(prep_stmt->executeQuery());
			if (rs->rowsCount() != 1)
				throw MDLDB_Exception("内部错误", MDLDB_GENERAL_ERROR);
			rs->next();
			uint32_t contextid = rs->getUInt("id");
			rs.release();
			prep_stmt.release();

			//获取选修moodle课程的所有学生

			prep_stmt = std::auto_ptr<sql::PreparedStatement>(m_connection->prepareStatement(prep_stmt_fpinfo));
			prep_stmt->setUInt(1, contextid);

			rs = std::auto_ptr<sql::ResultSet>(prep_stmt->executeQuery());

			std::vector<StudentInfo> student_info(rs->rowsCount());

			std::vector<StudentInfo>::iterator stu_info_it = student_info.begin();
			
			while (rs->next()) {
				stu_info_it->set_idnumber(rs->getString("idnumber"));

				stu_info_it->set_fullname(rs->getString("fullname"));

				const size_t fpsize = rs->getInt("fpsize");
				stu_info_it->set_fpsize(fpsize);

				byte* fpdata = new byte[fpsize];
				std::istream * is = rs->getBlob("fpdata");
				*is >> const_cast<byte*>(fpdata);
				stu_info_it->set_fpdata(fpdata);

				stu_info_it++;
			}

			return student_info;

		} catch(sql::SQLException& e) {
			throw;
		}
		return std::vector<StudentInfo>();
	}

	StudentInfo Connector::get_student_info(const std::string& idnumber)
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