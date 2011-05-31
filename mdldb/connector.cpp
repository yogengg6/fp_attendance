/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author1      : Yusuke(Qiuchengxuan@gmail.com)
 * Author2      : Edward(Edward.zhw@gmail.com)
 * Date	        : 2011-4-23 12:33
 */

#pragma warning(disable: 4244)

#include "stdafx.h"

#include <iostream>
#include <sstream>
#include <windows.h>
#include <cppconn/sqlstring.h>

#include "connector.h"
#include "exception.h"
#include "studentinfo.h"

#pragma warning(disable:4101)

namespace mdldb{
	bool									Connector::m_course_has_session = false;
	uint32_t								Connector::m_course_id = 0;
	uint32_t								Connector::m_statuses[4] = {0};
	std::string								Connector::m_statuesset = "";
	SessionInfo								Connector::m_sess_info = {0};
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

			if (rs->rowsCount() != 4) {
				m_course_has_session = false;
				return;
			}
			else
				m_course_has_session = true;
			while (rs->next())
				m_statuses[i++] = rs->getUInt("id");

			prep_stmt = std::auto_ptr<sql::PreparedStatement>(m_connection->prepareStatement(sql_statues_ordered));
			prep_stmt->setUInt(1, id);

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

		const char* sql = "UPDATE info SET fp_data=?, fp_size=? WHERE idnumber=?";

		try {
			m_connection->setSchema("student");

			std::auto_ptr<sql::PreparedStatement> 
				prep_stmt(m_connection->prepareStatement(sql));

			std::string buffer((char*)stu_info.get_fpdata(), stu_info.get_fpsize() * sizeof(byte));

			std::istringstream str_stream(buffer);

			prep_stmt->setBlob(1, &str_stream);
			prep_stmt->setUInt(2, stu_info.get_fpsize());
			prep_stmt->setString(3, stu_info.get_idnumber());
			if (prep_stmt->executeUpdate() <= 0)
				throw MDLDB_Exception("内部错误：学生信息不合法", MDLDB_GENERAL_ERROR);

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

		uint32_t status;
		int studentid, rows_affected;

		std::string sql_get_studentid = "SELECT id FROM mdl_user WHERE idnumber=?";

		std::auto_ptr<sql::PreparedStatement> 
			prep_stmt(m_connection->prepareStatement(sql_get_studentid));
		prep_stmt->setString(1, idnumber);

		std::auto_ptr<sql::ResultSet> rs(prep_stmt->executeQuery());
		if (rs->rowsCount() != 1)
			throw MDLDB_Exception("内部错误：不合法的学号", MDLDB_GENERAL_ERROR);
		rs->next();
		studentid = rs->getUInt("id");

		if (now <= m_sess_info.sessdate)
			status = m_statuses[ATTEND];
		else if (now <= m_sess_info.sessdate + m_sess_info.duration)
			status = m_statuses[LATE];
		else
			status = m_statuses[ABSENT];

		//如果数据库存在考勤记录，更新之
		const char* sql_update = 
			"UPDATE mdl_attendance_log SET statusid=? WHERE sessionid=? AND studentid=?";

		prep_stmt = std::auto_ptr<sql::PreparedStatement>(m_connection->prepareStatement(sql_update));
		prep_stmt->setUInt(1, status);
		prep_stmt->setUInt(2, m_sess_info.id);
		prep_stmt->setUInt(3, studentid);
		if ((rows_affected = prep_stmt->executeUpdate()) == 1)
			return true;
		else if (rows_affected != 0)
			throw MDLDB_Exception("内部错误", MDLDB_GENERAL_ERROR);
		
		//数据库不存在相应考勤记录，插入一条记录
		const char* sql_insert = 
			"INSERT INTO mdl_attendance_log \
			(sessionid, studentid, statusid, statusset, timetaken, takenby, remarks) \
			VALUES(?, ?, ?, ?, ?, ?, ?)";

		prep_stmt =
			std::auto_ptr<sql::PreparedStatement>(m_connection->prepareStatement(sql_insert));

		prep_stmt->setUInt(1, m_sess_info.id);
		prep_stmt->setUInt(2, studentid);
		prep_stmt->setUInt(3, status);
		prep_stmt->setString(4, m_statuesset);
		prep_stmt->setUInt(5, now);
		prep_stmt->setUInt(6, 2);//takenby admin
		prep_stmt->setString(7, "");
		prep_stmt->execute();

		return true;
	}

	/**
	 * 获取课程所有的学生指纹
	 * 1.先根据courseid获取contextid:
	 * 2.根据contextid得到课程所有学生的fpdata和fpsize(roleid为5代表学生)
	 */
	void Connector::get_course_student_info(std::vector<StudentInfo> &student_info)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", MDLDB_DISCONNECTED);

		/**
		 * contextlevel = 50意味着选择的是课程的实例
		 * moodle中如此定义：define('CONTEXT_COURSE', 50);
		 */
		const char* prep_stmt_context = 
			"SELECT id FROM mdl_context WHERE instanceid=? AND contextlevel=50";
		const char* prep_stmt_fpinfo = 
			"SELECT idnumber, fullname, fp_size, fp_data\
				FROM student.info WHERE idnumber IN \
				(SELECT u.idnumber \
					FROM moodle.mdl_user AS u, moodle.mdl_role_assignments AS ra \
					WHERE ra.contextid=? AND ra.roleid=5 AND u.id=ra.userid)";

		try {

			//获取moodle课程的context
			m_connection->setSchema("moodle");

			std::auto_ptr<sql::PreparedStatement> 
				prep_stmt(m_connection->prepareStatement(prep_stmt_context));
			prep_stmt->setUInt(1, m_course_id);

			std::auto_ptr<sql::ResultSet> rs(prep_stmt->executeQuery());
			if (rs->rowsCount() != 1)
				throw MDLDB_Exception("会话不唯一", MDLDB_GENERAL_ERROR);
			rs->next();
			uint32_t contextid = rs->getUInt("id");

			//获取选修moodle课程的所有学生

			prep_stmt = std::auto_ptr<sql::PreparedStatement>(m_connection->prepareStatement(prep_stmt_fpinfo));
			prep_stmt->setUInt(1, contextid);

			rs = std::auto_ptr<sql::ResultSet>(prep_stmt->executeQuery());

			if (rs->rowsCount() == 0) {
				throw MDLDB_Exception("该课程没有人选修！", MDLDB_GENERAL_ERROR);
			}

			student_info = std::vector<StudentInfo>(rs->rowsCount());
			std::vector<StudentInfo>::iterator stu_info_it = student_info.begin();

			while (rs->next()) {
				const size_t fpsize = rs->getUInt("fp_size");
// 				if (fpsize == 0)
// 					throw MDLDB_Exception("存在已选课但是没有指纹的学生！", MDLDB_GENERAL_ERROR);
				std::istream * is = rs->getBlob("fp_data");
				byte* fpdata = NULL;
				if (fpsize != 0) {
					fpdata = new byte[fpsize];
					is->read((char*)fpdata, fpsize);
				}

				stu_info_it->set_fpdata(fpsize, fpdata);
				stu_info_it->set_idnumber(rs->getString("idnumber"));
				stu_info_it->set_fullname(rs->getString("fullname"));
				stu_info_it++;
			}

		} catch(sql::SQLException& e) {
			throw MDLDB_Exception(e.what(), e.getErrorCode());
		}
	}

	//只获取信息，不获取指纹
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

				return StudentInfo(idnumber, fullname, fpsize, NULL);
			} else {
				throw MDLDB_Exception("student not found", MDLDB_STUDENT_NOT_FOUND);
			}

		} catch(sql::SQLException& e) {
			throw MDLDB_Exception(e.what(), MDLDB_GENERAL_ERROR);
		}
	}
}