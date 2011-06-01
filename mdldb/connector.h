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
		//����ִ����id
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

		//�������ݿ�
		bool		dbconnect() throw(mdldb::MDLDB_Exception);

		//�������û���Ϣ��֤
		void		auth(string username, string password);

		//�����γ̲���ȡ��Ӧ��״̬��m_statuses
		void		associate_course(uint32_t id);

		//����ʱ�������Ӧ�ĻỰ
		bool		associate_session(const string session_name) 
						throw(mdldb::MDLDB_Exception);

		inline void set_dbhost(string dbhost, string dbport) {
			m_dbhost = "tcp://" + dbhost + ":" + dbport;
		}

		inline void set_dbuser(string dbuser){m_dbuser = dbuser;}

		inline void set_dbpasswd(string dbpasswd){m_dbpasswd = dbpasswd;}
		//�ж����������ݿ�
		inline bool connected() const {return m_conn.get() != NULL;}

		//�жϿγ��Ƿ���ڻỰ
		inline bool course_has_session() const {return m_course_has_session;}

		//�ж��ѹ����γ�
		inline bool course_associated()  const {return m_course_id > 0;}

		//�ж��ѹ����Ự
		inline bool session_associated() const {return m_sess_info.id > 0;}

		//����ѧ��ע��ָ����Ϣ
		bool		enroll(StudentInfo& stu_info);

		//����ѧ���жϳ���״̬
		bool		attendant(string idnumber);

		//��ȡ���пγ���Ϣ
		vector<CourseInfo>  get_all_course();

		//����ʱ���ȡ���ܵĻỰ������
		vector<string> get_session_discription(uint32_t course_id = 0);

		// �����ز�������Ҫ�õ���ָ������ʱ��Ӧ��ʹ�ñ�����
		void  get_course_student_info(vector<StudentInfo> &student_info);

		/**
		 * ��ȡ���ַ����ܻ������루�ֽ��룩,��˿�����Ҫʹ��
		 * MultiByteToWideChar()��ת����
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

		//moodle��Ϊ��ǿ���밲ȫ�Զ�ʹ�õ�passwordsalt
		static string	m_passwordsalt;

		//�����˵�id
		static unsigned int	m_userid;

		//�����Ŀγ�id
		static unsigned int m_course_id;

		//�γ��Ƿ����˻Ự
		static bool			m_course_has_session;

		//����״̬�����ڡ��ٵ�����١����Σ�
		static unsigned int	m_statuses[4];

		//��Ҫд���statusset������grade����
		static string	m_statuesset;

		//�Ự��Ϣ
		static SessionInfo	m_sess_info;

		/**
		 * �������ÿ���ʱֱ�ӽ�ָ�븳ֵ����������ʱ���ܻ�����ظ��ͷ��ڴ棬
		 * ���ʹ�þ�̬����ָ�뱣�����ݿ�����ӡ�
		 */
		static auto_ptr<sql::Statement>			m_statement;
		static auto_ptr<sql::Connection>			m_conn;
		static auto_ptr<sql::mysql::MySQL_Driver>	m_driver;
	};

}