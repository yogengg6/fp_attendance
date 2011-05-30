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
		//����ִ����id
		uint32_t lasttakenby;
	} SessionInfo;

	const time_t  BEFORE_CLASS_BEGIN =  10 * 60 - 1;

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

		//�������ݿ�
		bool		dbconnect(const char * const db_host,
						  const char * const db_user,
						  const char * const db_passwd
						  ) throw(mdldb::MDLDB_Exception);

		//�����γ̲���ȡ��Ӧ��״̬��m_statuses
		void		associate_course(uint32_t id);

		//����ʱ�������Ӧ�ĻỰ
		bool		associate_session(const std::string session_name) 
						throw(mdldb::MDLDB_Exception);

		//�ж����������ݿ�
		inline bool connected() const {return m_connection.get() != NULL;}

		//�жϿγ��Ƿ���ڻỰ
		inline bool course_has_session() const {return m_course_has_session;}

		//�ж��ѹ����γ�
		inline bool course_associated()  const {return m_course_id > 0;}

		//�ж��ѹ����Ự
		inline bool session_associated() const {return m_sess_info.id > 0;}

		//����ѧ��ע��ָ����Ϣ
		bool		enroll(StudentInfo& stu_info);

		//����ѧ���жϳ���״̬
		bool		attendant(std::string idnumber);

		//��ȡ���пγ���Ϣ
		std::vector<CourseInfo>  get_all_course();

		//����ʱ���ȡ���ܵĻỰ������
		std::vector<std::string> get_session_discription(uint32_t course_id = 0);

		// �����ز�������Ҫ�õ���ָ������ʱ��Ӧ��ʹ�ñ�����
		void  get_course_student_info(std::vector<StudentInfo> &student_info);

		/**
		 * ��ȡ���ַ����ܻ������루�ֽ��룩,��˿�����Ҫʹ��
		 * MultiByteToWideChar()��ת����
		 */
		StudentInfo get_student_info(const std::string& idnumber) throw(mdldb::MDLDB_Exception);
	protected:
		enum {
			ATTEND	= 0,
			LATE	= 1,
			EXCUSE	= 2,
			ABSENT	= 3
		};

		//�����Ŀγ�id
		static uint32_t m_course_id;

		//�γ��Ƿ����˻Ự
		static bool		m_course_has_session;

		//����״̬�����ڡ��ٵ�����١����Σ�
		static uint32_t m_statuses[4];

		//��Ҫд���statusset������grade����
		static std::string m_statuesset;

		//�Ự��Ϣ
		static SessionInfo m_sess_info;

		/**
		 * �������ÿ���ʱֱ�ӽ�ָ�븳ֵ����������ʱ���ܻ�����ظ��ͷ��ڴ棬
		 * ���ʹ�þ�̬����ָ�뱣�����ݿ�����ӡ�
		 */
		static std::auto_ptr<sql::Statement>			m_statement;
		static std::auto_ptr<sql::Connection>			m_connection;
		static std::auto_ptr<sql::mysql::MySQL_Driver>	m_driver;
	};

}