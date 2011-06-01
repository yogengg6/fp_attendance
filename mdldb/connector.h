/**
* Project Name : FP_Attendance
* description  : a fingerprint based attendance(work with moodle attendanc
*                e module)
* Copyleft     : This program is published under GPL
* Author1      : Yusuke(Qiuchengxuan@gmail.com)
* Author2      : Edward(Edward.zhw@gmail.com)
* Date	        : 2011-4-23 12:33
*/

#pragma once

using namespace std;

//c header
#include <cstdio>
#include <ctime>

//c++ header
#include <iostream>
#include <vector>

//mysql header
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn\prepared_statement.h>

#include "exception.h"

#ifdef __GNUC__
#define sprintf_s(buffer, format, ...) sprintf(buffer, format, __VA_ARGS__)
#endif

namespace mdldb{

	typedef struct {
		uint32_t id;
		string fullname;
	} CourseInfo;

	typedef struct {
		uint32_t id;
		uint32_t sessdate;
		uint32_t duration;
		//考勤执行者id
		uint32_t lasttakenby;
	} SessionInfo;

	const time_t  BEFORE_CLASS_BEGIN =  10 * 60 - 1;

	class Connector
	{
	public:

		Connector(const string db_host,
				  const string db_port,
				  const string db_user,
				  const string db_passwd,
				  const string passwordsalt = "r+a~,qm_5(.M D]9[4-9T]u=JT/fu&z"
				  );

		Connector(Connector& conn);

		~Connector(void);

		//连接数据库
		bool		dbconnect() throw(mdldb::MDLDB_Exception);

		//考勤者用户信息验证
		void		auth(string username, string password);

		//关联课程并获取相应的状态集m_statuses
		void		associate_course(uint32_t id);

		//根据时间关联相应的会话
		bool		associate_session(const string session_name) 
						throw(mdldb::MDLDB_Exception);

		inline void set_dbhost(string dbhost, string dbport) {
			m_dbhost = "tcp://" + dbhost + ":" + dbport;
		}

		inline void set_dbuser(string dbuser){m_dbuser = dbuser;}

		inline void set_dbpasswd(string dbpasswd){m_dbpasswd = dbpasswd;}
		//判断已连接数据库
		inline bool connected() const {return m_conn.get() != NULL;}

		//判断课程是否存在会话
		inline bool course_has_session() const {return m_course_has_session;}

		//判断已关联课程
		inline bool course_associated()  const {return m_course_id > 0;}

		//判断已关联会话
		inline bool session_associated() const {return m_sess_info.id > 0;}

		//根据学号注册指纹信息
		bool		enroll(StudentInfo& stu_info);

		//根据学号判断出勤状态
		bool		attendant(string idnumber);

		//获取所有课程信息
		vector<CourseInfo>  get_all_course();

		//根据时间获取可能的会话的描述
		vector<string> get_session_discription(uint32_t course_id = 0);

		// 当本地不存在需要用到的指纹数据时才应当使用本函数
		void  get_course_student_info(vector<StudentInfo> &student_info);

		/**
		 * 获取的字符可能会是乱码（字节码）,因此可能需要使用
		 * MultiByteToWideChar()来转换。
		 */
		StudentInfo get_student_info(const string& idnumber) throw(mdldb::MDLDB_Exception);
	protected:
		enum {
			ATTEND	= 0,
			LATE	= 1,
			EXCUSE	= 2,
			ABSENT	= 3
		};

		static string	m_dbhost;
		static string	m_dbuser;
		static string	m_dbpasswd;

		//moodle中为加强密码安全性而使用的passwordsalt
		static string	m_passwordsalt;

		//考勤人的id
		static unsigned int	m_userid;

		//关联的课程id
		static unsigned int m_course_id;

		//课程是否建立了会话
		static bool			m_course_has_session;

		//四种状态（出勤、迟到、请假、旷课）
		static unsigned int	m_statuses[4];

		//需要写入的statusset，根据grade排序
		static string	m_statuesset;

		//会话信息
		static SessionInfo	m_sess_info;

		/**
		 * 由于引用拷贝时直接将指针赋值，导致析构时可能会带来重复释放内存，
		 * 因此使用静态智能指针保存数据库的链接。
		 */
		static auto_ptr<sql::Statement>			m_statement;
		static auto_ptr<sql::Connection>			m_conn;
		static auto_ptr<sql::mysql::MySQL_Driver>	m_driver;
	};

}