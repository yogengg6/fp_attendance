#include "StdAfx.h"

#include <sstream>

#include <md5.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>

#include "Mdldb.h"
#include "Mdldb_base.h"

using namespace std;
using namespace sql;
using namespace CryptoPP;

namespace mdldb{

	string		 Mdldb::m_passwordsalt = "";
	unsigned int Mdldb::m_userid = 2;
	Course		 Mdldb::m_course = {0};
	Session		 Mdldb::m_sess   = {0};

	Mdldb::Mdldb(const string& db_host,
				 const string& db_port,
				 const string& db_user,
				 const string& db_passwd,
				 const string& passwordsalt
				 ) 
				 : Mdldb_base(db_host, db_port, db_user, db_passwd)
	{
		m_userid = 2; // 默认为管理员
		m_passwordsalt = passwordsalt;
	}

	Mdldb::~Mdldb(void)
	{
	}

	void Mdldb::auth(string username, string password)
	{
		static char md5passwd[32] = {0};
		static byte md5result[32] = {0};

		password += m_passwordsalt;

		MD5 md5;
		md5.Update((byte*) password.c_str(), password.size());
		md5.Final(md5result);

		for (int i = 0; i < 16; i++)
			sprintf(md5passwd + i*2, "%02x", md5result[i]);

		try {
			m_mdl->setSchema("moodle");

			//验证用户是否存在（不存在则抛出异常）
			const char* sql_getuserid = 
				"SELECT id FROM mdl_user WHERE username=?";
			SQLContainer sql_container(m_mdl.get(), sql_getuserid);
			sql_container << username;
			auto_ptr<ResultSet> rs(sql_container.exeQuery());
			if (rs->rowsCount() != 1)
				throw MDLDB_Exception("no such user", NO_USER);
			rs->next();
			m_userid = rs->getUInt("id");
			m_sess.lasttakenby = m_userid;

			//验证用户密码
			const char* sql_auth_password = 
				"SELECT id FROM mdl_user WHERE id=? AND password=?";
			sql_container = SQLContainer(m_mdl.get(), sql_auth_password);
			sql_container << m_userid << md5passwd;
			rs = auto_ptr<ResultSet>(sql_container.exeQuery());
			if (rs->rowsCount() != 1)
				throw MDLDB_Exception("incorrect password", INCORRECT_PASSWD);

			//检查用户权限
			const char* sql_auth_permission = 
				"SELECT id FROM mdl_role_assignments WHERE userid=? AND roleid<5";
			sql_container = SQLContainer(m_mdl.get(), sql_auth_permission);
			sql_container << m_userid;
			rs = auto_ptr<ResultSet>(sql_container.exeQuery());
			if (rs->rowsCount() < 1)
				throw MDLDB_Exception("no permission", NO_PERMISSION);
		} catch (SQLException& e) {
			throw MDLDB_Exception(e.what(), GENERAL_ERROR);
		}
	}

	void Mdldb::associate_course(uint id)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", DISCONNECTED);

		m_course.id = id;

		try {
			int i = 0;

			m_mdl->setSchema("moodle");

			const char* prep_sql = 
				"SELECT id FROM mdl_attendance_statuses WHERE courseid=?";
			SQLContainer sql_container(m_mdl.get(), prep_sql);
			sql_container << id;
			auto_ptr<ResultSet> rs(sql_container.exeQuery());
			if (rs->rowsCount() != 4) {
				m_course.has_session = false;
				return;
			}
			else
				m_course.has_session = true;
			while (rs->next())
				m_sess.statuses[i++] = rs->getUInt("id");

			const char* sql_statues_ordered = 
				"SELECT id FROM mdl_attendance_statuses WHERE courseid=? \
					ORDER BY grade DESC";

			sql_container = SQLContainer(m_mdl.get(), sql_statues_ordered);
			sql_container << id;
			rs = auto_ptr<ResultSet>(sql_container.exeQuery());

			ostringstream sout;
			while (rs->next())
				sout << rs->getUInt("id") << ",";
			m_sess.statuesset = sout.str();
			m_sess.statuesset.erase(m_sess.statuesset.end()-1);

		} catch (SQLException &e) {
			throw MDLDB_Exception(e.what(), GENERAL_ERROR);
		}
	}

	void Mdldb::get_authorized_course(vector<Course>& courses)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", DISCONNECTED);

		const char* sql = 
			"SELECT id, fullname FROM mdl_course WHERE id IN \
				(SELECT instanceid FROM mdl_context WHERE contextlevel=50 AND id IN \
					(SELECT contextid FROM mdl_role_assignments WHERE userid=? AND roleid<=4)\
				)";

		try {
			m_mdl->setSchema("moodle");

			SQLContainer sql_container(m_mdl.get(), sql);
			sql_container << m_userid;
			auto_ptr<ResultSet> rs(sql_container.exeQuery());

			courses = vector<Course>(rs->rowsCount());

			for (int i = 0; rs->next(); ++i) {
				courses[i].id = rs->getUInt("id");
				courses[i].fullname = rs->getString("fullname");
			}
		} catch(SQLException& e) {
			throw MDLDB_Exception(e.what(), GENERAL_ERROR);
		}
		
	}

	/**
	 * check and get session_id from mdl_attendance
	 * notice that session maybe duplicate
	 */
	bool Mdldb::associate_session(const string session) throw(MDLDB_Exception)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", DISCONNECTED);

		if (!course_associated())
			throw MDLDB_Exception("没有关联课程", NO_COURSE);

		uint32_t now = (uint32_t)time(NULL);
		
		const char* prep_sql = 
			"SELECT id, sessdate, duration FROM mdl_attendance_sessions \
				WHERE courseid=? AND description=? \
				AND ? BETWEEN sessdate AND sessdate + duration";
		try {
			m_mdl->setSchema("moodle");

			SQLContainer sql_container(m_mdl.get(), prep_sql);
			sql_container << m_course.id << session << now;

			auto_ptr<ResultSet> rs(sql_container.exeQuery());
			switch (rs->rowsCount()) {
		case 0:
			throw MDLDB_Exception("[MDLDB]:session not found", SESSION_NOT_FOUND);
			break;
		case 1:
			rs->next();
			m_sess.id = rs->getInt("id");
			m_sess.lasttakenby = m_userid;
			m_sess.sessdate = rs->getUInt("sessdate");
			m_sess.duration = rs->getUInt("duration");
			break;
		default:
			throw MDLDB_Exception("[MDLDB]:duplicate session", DUPLICATE_SESSION);
			break;
			}
		} catch(SQLException &e) {
			throw MDLDB_Exception(e.what(), GENERAL_ERROR);
		}
		return m_sess.id > 0;
	}

	void Mdldb::get_session_discription(vector<string>& sessions, uint32_t course_id)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", DISCONNECTED);

		if (course_id == 0)
			course_id = m_course.id;

		uint32_t now = time(NULL);

		const char* sql = 
			"SELECT DISTINCT(description) FROM mdl_attendance_sessions \
				WHERE courseid=? \
				AND ? BETWEEN sessdate AND sessdate + duration";

		try {
			m_mdl->setSchema("moodle");
			SQLContainer sql_container(m_mdl.get(), sql);
			sql_container << course_id << now;
			auto_ptr<ResultSet> rs(sql_container.exeQuery());

			sessions = vector<string>(rs->rowsCount());

			for (int i = 0; rs->next(); ++i)
				sessions[i] = rs->getString("description");
		} catch(SQLException& e) {
			throw MDLDB_Exception(e.what(), GENERAL_ERROR);
		}
	}

	bool Mdldb::fpenroll(Student& stu_info)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", DISCONNECTED);

		char* sql = "INSERT INTO fingerprint (infoid, fp_index, fp_size, fp_data) \
					VALUES ((SELECT id FROM info WHERE idnumber=?), ?, ?, ?)";

		try {
			m_mdl->setSchema("student");
			SQLContainer sql_container(m_mdl.get(), sql);
			Fpdata fpdata = stu_info.get_fpdata();
			string buffer((char*)fpdata.data, fpdata.size);
			istringstream str_stream(buffer);
			sql_container << stu_info.get_idnumber() << fpdata.index << fpdata.size << &str_stream;
			sql_container.execute();
		} catch(SQLException& e) {
			throw MDLDB_Exception(e.what(), GENERAL_ERROR);
		}
		return true;
	}

	void Mdldb::fpdelete(string idnumber)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", DISCONNECTED);

		char* sql = "DELETE FROM fingerprint \
					 WHERE infoid IN (SELECT id FROM info WHERE idnumber=?)";

		try {
			m_mdl->setSchema("student");
			SQLContainer sql_container(m_mdl.get(), sql);
			sql_container << idnumber;
			sql_container.execute();
		} catch(SQLException& e) {
			throw MDLDB_Exception(e.what(), GENERAL_ERROR);
		}
	}

	bool Mdldb::attendant(string idnumber)
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

		try {
			m_mdl->setSchema("moodle");
			const char* sql_get_studentid = "SELECT id FROM mdl_user WHERE idnumber=?";
			SQLContainer sql_container(m_mdl.get(), sql_get_studentid);
			sql_container << idnumber;
			auto_ptr<ResultSet> rs(sql_container.exeQuery());
			if (rs->rowsCount() != 1)
				throw MDLDB_Exception("内部错误：不合法的学号", GENERAL_ERROR);
			rs->next();
			studentid = rs->getUInt("id");

			if (now <= m_sess.sessdate)
				status = m_sess.statuses[ATTEND];
			else if (now <= m_sess.sessdate + m_sess.duration)
				status = m_sess.statuses[LATE];
			else
				status = m_sess.statuses[ABSENT];

			//如果数据库存在考勤记录，更新之
			const char* sql_update = 
				"UPDATE mdl_attendance_log SET statusid=? \
				 WHERE sessionid=? AND studentid=?";
			sql_container = SQLContainer(m_mdl.get(), sql_update);
			sql_container << status << m_sess.id << studentid;
			if ((rows_affected = sql_container.exeUpdate()) >= 1)
				return true;
			else if (rows_affected != 0)
				throw MDLDB_Exception("内部错误", GENERAL_ERROR);

			//数据库不存在相应考勤记录，插入一条记录
			const char* sql_insert = 
				"INSERT INTO mdl_attendance_log \
				(sessionid, studentid, statusid, statusset, timetaken, takenby, remarks) \
				VALUES(?, ?, ?, ?, ?, ?, ?)";

			sql_container = SQLContainer(m_mdl.get(), sql_insert);
			sql_container << m_sess.id << studentid << status 
				<< m_sess.statuesset << now << m_userid << "";
			sql_container.execute();

			return true;
		} catch(SQLException& e) {
			throw MDLDB_Exception(e.what(), GENERAL_ERROR);
		}
		
	}

	/**
	 * 获取课程所有的学生指纹
	 * 1.先根据courseid获取contextid:
	 * 2.根据contextid得到课程所有学生的fpdata和fpsize(roleid为5代表学生)
	 */
	void Mdldb::get_course_students(vector<Student>& students)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", DISCONNECTED);

		try {
			/**
			 * 获取moodle课程的context
			 * contextlevel = 50意味着选择的是课程的实例
			 * moodle中如此定义：define('CONTEXT_COURSE', 50);
			 */
			m_mdl->setSchema("moodle");
			const char* sql_context = 
				"SELECT id FROM mdl_context WHERE instanceid=? AND contextlevel=50";
			SQLContainer sql_container(m_mdl.get(), sql_context);
			sql_container << m_course.id;
			auto_ptr<ResultSet> rs(sql_container.exeQuery());
			if (rs->rowsCount() != 1)
				throw MDLDB_Exception("会话不唯一", GENERAL_ERROR);
			rs->next();
			uint32_t contextid = rs->getUInt("id");

			//获取选修moodle课程的所有学生
			m_mdl->setSchema("student");
			const char* sql = "SELECT idnumber, fullname, fp_index, fp_size, fp_data\
							   FROM info AS i LEFT JOIN fingerprint AS fp \
							   ON i.id=fp.infoid WHERE idnumber IN \
									(SELECT idnumber \
									 FROM moodle.mdl_user AS u, moodle.mdl_role_assignments AS ra \
							    	 WHERE ra.contextid=? AND ra.roleid=5 AND u.id=ra.userid) \
							   ORDER BY idnumber";

			sql_container = SQLContainer(m_mdl.get(), sql);
			sql_container << contextid;
			rs = auto_ptr<ResultSet>(sql_container.exeQuery());

			if (rs->rowsCount() > 0) {
				students = vector<Student>(rs->rowsCount());
				vector<Student>::iterator it = students.begin();

				Fpdata fpdata = {0};

				while (rs->next()) {
					istream* is = NULL;

					it->set_idnumber(rs->getString("idnumber"));
					it->set_fullname(rs->getString("fullname"));

					fpdata.index = rs->getUInt("fp_index");
					fpdata.size = rs->getUInt("fp_size");

					is = rs->getBlob("fp_data");
					if (fpdata.size != 0) {
						fpdata.data = new byte[fpdata.size];
						is->read((char*)fpdata.data, fpdata.size);
					} else
						fpdata.data = NULL;

					it->set_fpdata(fpdata);
					it++;
				}
			} else {
				throw MDLDB_Exception("该课程没有人选修！", GENERAL_ERROR);
			}
		} catch(SQLException& e) {
			throw MDLDB_Exception(e.what(), e.getErrorCode());
		}
	}

	void Mdldb::get_students(vector<Student>& students, const string& idnumber, size_t limit)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", DISCONNECTED);

		const char* sql = 
			"SELECT i.idnumber, i.fullname, fp.fp_index \
			FROM info AS i LEFT JOIN fingerprint AS fp \
			ON i.id=fp.infoid \
			WHERE i.idnumber LIKE ? \
			ORDER BY i.idnumber LIMIT ?";

		try {
			m_mdl->setSchema("student");
			SQLContainer sql_container(m_mdl.get(), sql);
			sql_container << idnumber+"%" << limit;
			auto_ptr<ResultSet> rs(sql_container.exeQuery());

			students = vector<Student>(rs->rowsCount());
			vector<Student>::iterator it = students.begin();

			Fpdata fpdata;
			ZeroMemory(&fpdata, sizeof(Fpdata));

			while (rs->next()) {

				it->set_idnumber(rs->getString("idnumber"));
				it->set_fullname(rs->getString("fullname"));
				fpdata.index = rs->getUInt("fp_index");
				it->set_fpdata(fpdata);
				it++;
			}
		} catch(SQLException& e) {
			throw MDLDB_Exception(e.what(), GENERAL_ERROR);
		}
	}

	void Mdldb::get_course_groups(vector<Group>& groups)
	{
		if (!connected())
			throw MDLDB_Exception("没有连接到数据库", DISCONNECTED);
		if (!course_associated())
			throw MDLDB_Exception("没有关联课程", NO_COURSE);

		const char* sql = "SELECT id, name FROM mdl_groups WHERE courseid=?";

		try {
			m_mdl->setSchema("moodle");
			SQLContainer sql_container(m_mdl.get(), sql);
			sql_container << m_course.id;
			auto_ptr<ResultSet> rs(sql_container.exeQuery());

			groups = vector<Group>(rs->rowsCount());

			vector<Group>::iterator it = groups.begin();

			while (rs->next()) {
				it->id = rs->getUInt("id");
				it->name = rs->getString("name");
				it++;
			}
		} catch(SQLException& e) {
			throw MDLDB_Exception(e.what(), GENERAL_ERROR);
		}
	}
}
