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

#include "MDLDB_Exception.h"

#ifdef __GNUC__
#define sprintf_s(buffer, format, ...) sprintf(buffer, format, __VA_ARGS__)
#endif

namespace mdldb{

	typedef struct {
		uint32_t id;
		std::string fullname;
	} CourseInfo;

	typedef struct {
		uint32_t id;
		uint32_t sessdate;
		uint32_t duration;
		//考勤执行者id
		uint32_t lasttakenby;
	} SessionInfo;

	const time_t  BEFORE_CLASS_BEGIN =  10 * 60 - 1;

	const int	  STATUS_ATTEND = 0;
	const int	  STATUS_LATE	= 1;
	const int	  STATUS_EXCUSE = 2;
	const int	  STATUS_ABSENT = 3;

	class Connector
	{
	public:
		Connector(void);

		Connector(const char * const db_host,
				  const char * const db_user,
				  const char * const db_passwd
				  );

		Connector(Connector& conn);

		~Connector(void);

		//连接数据库
		bool		dbconnect(const char * const db_host,
						  const char * const db_user,
						  const char * const db_passwd
						  ) throw(mdldb::MDLDB_Exception);

		//关联课程并获取相应的状态集m_statuses
		void		associate_course(uint32_t id);

		//根据时间关联相应的会话
		bool		associate_session(const std::string session_name) 
						throw(mdldb::MDLDB_Exception);

		//判断已连接数据库
		inline bool connected() const {return m_connection.get() != NULL;}

		//判断已关联课程
		inline bool course_associated()  const {return m_course_id > 0;}

		//判断已关联会话
		inline bool session_associated() const {return m_sess_info.id > 0;}

		//根据学号注册指纹信息
		bool		enroll(StudentInfo& stu_info);

		//根据学号判断出勤状态
		bool		attendant(std::string idnumber);

		//获取所有课程信息
		std::vector<CourseInfo>  get_all_course();

		//根据时间获取可能的会话的描述
		std::vector<std::string> get_session_discription(uint32_t course_id = 0);

		// 当本地不存在需要用到的指纹数据时才应当使用本函数
		std::vector<StudentInfo>  get_course_student_info(uint32_t courseid = 0) const;

		/**
		 * 获取的字符可能会是乱码（字节码）,因此可能需要使用
		 * MultiByteToWideChar()来转换。
		 */
		StudentInfo get_student_info(const std::string& idnumber) throw(mdldb::MDLDB_Exception);
	protected:
		//关联的课程id
		uint32_t m_course_id;

		//四种状态（出勤、迟到、请假、旷课）
		uint32_t m_statuses[4];

		//需要写入的statusset
		std::string m_statuesset;

		//会话信息
		SessionInfo m_sess_info;

		/**
		 * 由于引用拷贝时直接将指针赋值，导致析构时可能会带来重复释放内存，
		 * 因此使用静态智能指针保存数据库的链接。
		 */
		static std::auto_ptr<sql::Statement>			m_statement;
		static std::auto_ptr<sql::Connection>			m_connection;
		static std::auto_ptr<sql::mysql::MySQL_Driver>	m_driver;
	};

}