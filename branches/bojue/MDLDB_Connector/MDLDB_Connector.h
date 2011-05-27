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
		std::string description;
	} SessionInfo;

	const time_t  BEFORE_CLASS_BEGIN =  10 * 60 - 1;

	class Connector
	{
	public:
		Connector(void);

		//notice that session_name is session full name
		Connector(const char* const db_host,
				  const char* const db_user,
				  const char* const db_passwd,
				  const std::string	course_name,
				  const std::string	session_name
				  );

		Connector(const char * const db_host,
				  const char * const db_user,
				  const char * const db_passwd
				  );

		Connector(Connector& conn);

		~Connector(void);

		bool    dbconnect(const char * const db_host,
						  const char * const db_user,
						  const char * const db_passwd
						  ) throw(mdldb::MDLDB_Exception);

		void	associate_course(uint32_t id);

		bool    associate_course(const std::string course_name) throw(mdldb::MDLDB_Exception);

		bool    associate_session(const std::string session_name) throw(mdldb::MDLDB_Exception);

		//     bool    enroll(const string &idnumber,
		//                    const byte * const fp_data,
		//                    size_t fp_size);
		bool    enroll(StudentInfo& stu_info);

		std::vector<CourseInfo>  get_all_course();
		std::vector<std::string> get_session_discription(uint32_t course_id = 0);


		size_t  get_all_info(StudentInfo*		student_list, 
							 const std::string	class1="", 
							 const std::string	class2=""
							 ) const;

		/**
		* 获取的字符可能会是乱码（字节码）,因此可能需要使用
		* MultiByteToWideChar()来转换。
		*/
		StudentInfo get_info(const std::string& idnumber) throw(mdldb::MDLDB_Exception);

		inline bool connected() const {return m_connection.get() != NULL;}
		inline bool course_associated()      const {return m_course_id > 0;     }
		inline bool session_associated()     const {return m_session_id > 0;    }
		inline bool is_valid() const
		{
			return  (m_connection.get() != NULL) && 
					(m_session_id > 0)     && 
					(m_course_id  > 0);
		}
	protected:
		uint32_t m_course_id;
		uint32_t m_session_id;

		/**
		* 由于引用拷贝时直接将指针赋值，导致析构时可能会带来重复释放内存，因此
		* 使用静态智能指针保存数据库的链接。
		*/

		static std::auto_ptr<sql::Statement>				m_statement;
		static std::auto_ptr<sql::Connection>			m_connection;
		static std::auto_ptr<sql::mysql::MySQL_Driver>	m_driver;
	};

}