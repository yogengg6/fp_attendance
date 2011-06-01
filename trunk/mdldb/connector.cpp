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

#include <md5.h>

#include "connector.h"
#include "exception.h"
#include "userinfo.h"

using namespace std;
using namespace CryptoPP;

namespace mdldb{
	string								Connector::m_dbhost = "";
	string								Connector::m_dbuser = "";
	string								Connector::m_dbpasswd = "";
	string								Connector::m_passwordsalt = "";
	bool								Connector::m_course_has_session = false;
	unsigned int						Connector::m_userid = 2;
	unsigned int						Connector::m_course_id = 0;
	unsigned int						Connector::m_statuses[4] = {0};
	string								Connector::m_statuesset = "";
	SessionInfo							Connector::m_sess_info = {0};
	auto_ptr<sql::mysql::MySQL_Driver>	Connector::m_driver(NULL);
	auto_ptr<sql::Statement>			Connector::m_statement(NULL);
	auto_ptr<sql::Connection>			Connector::m_conn(NULL);

	/**
	 * this constructor only call dbconnect, and deal with exceptions.
	 * So that when you're attempting enroll student fingerprint, just use it.
	 */
	Connector::Connector(const string db_host,
						 const string db_port,
						 const string db_user,
						 const string db_passwd,
						 const string passwordsalt
						 )
	{
		m_userid = 2; // 默认为管理员
		m_dbhost = "tcp://" + db_host + ":" + db_port;
		m_dbuser = db_user;
		m_dbpasswd = db_passwd;
		m_passwordsalt = passwordsalt;
		try {
			if (m_driver.get() == NULL)
				m_driver = auto_ptr<sql::mysql::MySQL_Driver>(sql::mysql::get_driver_instance());
		} catch (...) {
			throw;
		}
	}

	Connector::Connector(Connector& conn)
	{
	}

	Connector::~Connector(void)
	{
	}

	/**
	 * create connection to MySQL Database server
	 */
	bool Connector::dbconnect() throw(MDLDB_Exception)
	{
		try {
			m_conn = auto_ptr<sql::Connection>(m_driver->connect(m_dbhost, m_dbuser, m_dbpasswd));
			m_conn->setSchema("moodle");
			m_statement = auto_ptr<sql::Statement>(m_conn->createStatement());
			m_statement->execute("SET NAMES utf8");
		} catch (sql::SQLException &e) {
			switch (e.getErrorCode()) {
			case 1045:
				throw MDLDB_Exception("连接被拒绝", CONNECTION_REFUSED);
				break;
			case 10060:
			case 10061:
				throw MDLDB_Exception("连接失败", CONNECTION_FAIL);
				break;
			default:
				throw MDLDB_Exception(e.what(), CONNECTION_UNKNOW);
				break;
			}
		}
		return m_conn.get() != NULL;
	}

	void Connector::auth(string username, string password)
	{
		static char md5passwd[32] = {0};
		static byte md5result[32] = {0};

		password += m_passwordsalt;

		MD5 md5;
		md5.Update((byte*) password.c_str(), password.size());
		md5.Final(md5result);

		for (int i = 0; i < 16; i++)
			sprintf(md5passwd+i*2, "%02x", md5result[i]);

		try {
			m_conn->setSchema("moodle");
			const char* sql_get_userid = "SELECT id FROM mdl_user WHERE username=?";
			auto_ptr<sql::PreparedStatement> prep_stmt(m_conn->prepareStatement(sql_get_userid));
			prep_stmt->setString(1, username);
			auto_ptr<sql::ResultSet> rs(prep_stmt->executeQuery());

			if (rs->rowsCount() != 1)
				throw MDLDB_Exception("no such user", NO_USER);

			rs->next();
			m_userid = rs->getUInt("id");
			m_sess_info.lasttakenby = m_userid;

			const char* sql_auth = "SELECT id FROM mdl_user WHERE id=? AND password=?";
			prep_stmt = auto_ptr<sql::PreparedStatement>(m_conn->prepareStatement(sql_auth));
			prep_stmt->setUInt(1, m_userid);
			prep_stmt->setString(2, md5passwd);
			rs = auto_ptr<sql::ResultSet>(prep_stmt->executeQuery());
			if (rs->rowsCount() != 1)
				throw MDLDB_Exception("incorrect password", INCORRECT_PASSWD);

			const char* sql_permission = "SELECT id FROM mdl_role_assignments WHERE userid=? AND roleid<5";
			prep_stmt = auto_ptr<sql::PreparedStatement>(m_conn->prepareStatement(sql_permission));
			prep_stmt->setUInt(1, m_userid);
			rs = auto_ptr<sql::ResultSet>(prep_stmt->executeQuery());
			if (rs->rowsCount() < 1)
				throw MDLDB_Exception("no permission", NO_PERMISSION);
		} catch (sql::SQLException& e) {
			throw MDLDB_Exception(e.what(), GENERAL_ERROR);
		}
	}

	void Connector::associate_course(uint32_t id)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", DISCONNECTED);

		m_course_id = id;

		const char* prep_sql = 
			"SELECT id FROM mdl_attendance_statuses WHERE courseid=?";

		const char* sql_statues_ordered = 
			"SELECT id FROM mdl_attendance_statuses WHERE courseid=? ORDER BY grade DESC";

		try {
			int i = 0;
			ostringstream sout;

			m_conn->setSchema("moodle");
			auto_ptr<sql::PreparedStatement> prep_stmt(m_conn->prepareStatement(prep_sql));
			prep_stmt->setUInt(1, id);

			auto_ptr<sql::ResultSet> rs(prep_stmt->executeQuery());

			if (rs->rowsCount() != 4) {
				m_course_has_session = false;
				return;
			}
			else
				m_course_has_session = true;
			while (rs->next())
				m_statuses[i++] = rs->getUInt("id");

			prep_stmt = auto_ptr<sql::PreparedStatement>(m_conn->prepareStatement(sql_statues_ordered));
			prep_stmt->setUInt(1, id);

			rs = auto_ptr<sql::ResultSet>(prep_stmt->executeQuery());
			while (rs->next())
				sout << rs->getUInt("id") << ",";

			m_statuesset = sout.str();
			m_statuesset.erase(m_statuesset.end()-1);
		} catch (sql::SQLException &e) {
			throw MDLDB_Exception(e.what(), GENERAL_ERROR);
		}
	}

	/**
	 * check and get session_id from mdl_attendance
	 * notice that session maybe duplicate
	 */
	bool Connector::associate_session(const string session) throw(MDLDB_Exception)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", DISCONNECTED);

		if (!course_associated())
			throw MDLDB_Exception("没有关联课程", NO_COURSE);

		uint32_t now = (uint32_t)time(NULL);
		
		const char* prep_sql = "SELECT id, sessdate, duration FROM mdl_attendance_sessions WHERE courseid=? AND description=? AND ? BETWEEN sessdate AND sessdate + duration";
		try {
			m_conn->setSchema("moodle");

			auto_ptr<sql::PreparedStatement> prep_stmt(m_conn->prepareStatement(prep_sql));
			prep_stmt->setInt(1, m_course_id);
			prep_stmt->setString(2, session);
			prep_stmt->setUInt(3, now);

			auto_ptr<sql::ResultSet> rs(prep_stmt->executeQuery());
			switch (rs->rowsCount()) {
		case 0:
			throw MDLDB_Exception("[MDLDB]:session not found", SESSION_NOT_FOUND);
			break;
		case 1:
			rs->next();
			m_sess_info.id = rs->getInt("id");
			m_sess_info.lasttakenby = m_userid;
			m_sess_info.sessdate = rs->getUInt("sessdate");
			m_sess_info.duration = rs->getUInt("duration");
			break;
		default:
			throw MDLDB_Exception("[MDLDB]:duplicate session", DUPLICATE_SESSION);
			break;
			}
		} catch(sql::SQLException &e) {
			throw MDLDB_Exception(e.what(), GENERAL_ERROR);
		}
		return m_sess_info.id > 0;
	}

	vector<CourseInfo> Connector::get_all_course()
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", DISCONNECTED);

		static const char* sql = "SELECT id, fullname FROM mdl_course";

		try {
			m_conn->setSchema("moodle");

			auto_ptr<sql::ResultSet> rs(m_statement->executeQuery(sql));

			vector<CourseInfo> ci(rs->rowsCount());

			for (int i = 0; rs->next(); ++i) {
				ci[i].id = rs->getUInt("id");
				ci[i].fullname = rs->getString("fullname");
			}

			return ci;
		} catch(sql::SQLException& e) {
			throw MDLDB_Exception(e.what(), GENERAL_ERROR);
		}
		
	}

	vector<string> Connector::get_session_discription(uint32_t course_id)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", DISCONNECTED);

		if (course_id == 0)
			course_id = m_course_id;

		time_t now = time(NULL);

		const char* sql = "SELECT DISTINCT(description) FROM mdl_attendance_sessions WHERE courseid=? AND ? BETWEEN sessdate AND sessdate + duration";

		try {
			m_conn->setSchema("moodle");

			auto_ptr<sql::PreparedStatement> prep_stmt(m_conn->prepareStatement(sql));

			prep_stmt->setUInt(1, course_id);
			prep_stmt->setUInt(2, now);

			auto_ptr<sql::ResultSet> rs(prep_stmt->executeQuery());

			vector<string> session_info(rs->rowsCount());

			for (int i = 0; rs->next(); ++i)
				session_info[i] = rs->getString("description");

			return session_info;
		} catch(sql::SQLException& e) {
			throw MDLDB_Exception(e.what(), GENERAL_ERROR);
		}
	}

	bool Connector::enroll(StudentInfo& stu_info)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", DISCONNECTED);

		const char* sql = "UPDATE info SET fp_data=?, fp_size=? WHERE idnumber=?";

		try {
			m_conn->setSchema("student");

			auto_ptr<sql::PreparedStatement> 
				prep_stmt(m_conn->prepareStatement(sql));

			string buffer((char*)stu_info.get_fpdata(), stu_info.get_fpsize() * sizeof(byte));

			istringstream str_stream(buffer);

			prep_stmt->setBlob(1, &str_stream);
			prep_stmt->setUInt(2, stu_info.get_fpsize());
			prep_stmt->setString(3, stu_info.get_idnumber());
			if (prep_stmt->executeUpdate() <= 0)
				throw MDLDB_Exception("内部错误：学生信息不合法", GENERAL_ERROR);

		} catch(sql::SQLException& e) {
			throw MDLDB_Exception(e.what(), GENERAL_ERROR);
		}
		return true;
	}

	bool Connector::attendant(string idnumber)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", DISCONNECTED);
		if (!course_associated())
			throw MDLDB_Exception("没有关联课程", NO_COURSE);
		if (!session_associated())
			throw MDLDB_Exception("没有关联会话", NO_SESSION);

		uint32_t now = time(NULL);

		uint32_t status;
		int studentid, rows_affected;

		const char* sql_get_studentid = "SELECT id FROM mdl_user WHERE idnumber=?";

		auto_ptr<sql::PreparedStatement> 
			prep_stmt(m_conn->prepareStatement(sql_get_studentid));
		prep_stmt->setString(1, idnumber);

		auto_ptr<sql::ResultSet> rs(prep_stmt->executeQuery());
		if (rs->rowsCount() != 1)
			throw MDLDB_Exception("内部错误：不合法的学号", GENERAL_ERROR);
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

		prep_stmt = auto_ptr<sql::PreparedStatement>(m_conn->prepareStatement(sql_update));
		prep_stmt->setUInt(1, status);
		prep_stmt->setUInt(2, m_sess_info.id);
		prep_stmt->setUInt(3, studentid);
		if ((rows_affected = prep_stmt->executeUpdate()) == 1)
			return true;
		else if (rows_affected != 0)
			throw MDLDB_Exception("内部错误", GENERAL_ERROR);
		
		//数据库不存在相应考勤记录，插入一条记录
		const char* sql_insert = 
			"INSERT INTO mdl_attendance_log \
			(sessionid, studentid, statusid, statusset, timetaken, takenby, remarks) \
			VALUES(?, ?, ?, ?, ?, ?, ?)";

		prep_stmt =
			auto_ptr<sql::PreparedStatement>(m_conn->prepareStatement(sql_insert));

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
	void Connector::get_course_student_info(vector<StudentInfo> &student_info)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", DISCONNECTED);

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
			m_conn->setSchema("moodle");

			auto_ptr<sql::PreparedStatement> 
				prep_stmt(m_conn->prepareStatement(prep_stmt_context));
			prep_stmt->setUInt(1, m_course_id);

			auto_ptr<sql::ResultSet> rs(prep_stmt->executeQuery());
			if (rs->rowsCount() != 1)
				throw MDLDB_Exception("会话不唯一", GENERAL_ERROR);
			rs->next();
			uint32_t contextid = rs->getUInt("id");

			//获取选修moodle课程的所有学生

			prep_stmt = auto_ptr<sql::PreparedStatement>(m_conn->prepareStatement(prep_stmt_fpinfo));
			prep_stmt->setUInt(1, contextid);

			rs = auto_ptr<sql::ResultSet>(prep_stmt->executeQuery());

			if (rs->rowsCount() == 0) {
				throw MDLDB_Exception("该课程没有人选修！", GENERAL_ERROR);
			}

			student_info = vector<StudentInfo>(rs->rowsCount());
			vector<StudentInfo>::iterator stu_info_it = student_info.begin();

			while (rs->next()) {
				const size_t fpsize = rs->getUInt("fp_size");
// 				if (fpsize == 0)
// 					throw MDLDB_Exception("存在已选课但是没有指纹的学生！", GENERAL_ERROR);
				istream * is = rs->getBlob("fp_data");
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
	StudentInfo Connector::get_student_info(const string& idnumber)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", DISCONNECTED);

		const char* sql = "SELECT fullname, fp_size FROM info WHERE idnumber=?";

		try {
			m_conn->setSchema("student");
			auto_ptr<sql::PreparedStatement> prep_stmt(m_conn->prepareStatement(sql));

			prep_stmt->setString(1, idnumber);

			auto_ptr<sql::ResultSet> rs(prep_stmt->executeQuery());

			if (rs->rowsCount() == 1) {
				rs->next();
				string fullname = rs->getString("fullname");
				size_t fpsize = rs->getInt("fp_size");

				static char conv_buf_ch[100];
				static wchar_t conv_buf_wch[100];

				return StudentInfo(idnumber, fullname, fpsize, NULL);
			} else {
				throw MDLDB_Exception("student not found", STUDENT_NOT_FOUND);
			}

		} catch(sql::SQLException& e) {
			throw MDLDB_Exception(e.what(), GENERAL_ERROR);
		}
	}
}