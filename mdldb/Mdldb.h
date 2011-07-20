#pragma once
#pragma warning(disable:4244)
#include <string>

#include "Mdldb_base.h"
#include "userinfo.h"

using namespace std;

namespace mdldb{

	typedef unsigned int uint;

	typedef struct {
		uint id;
		string fullname;
		//�γ��Ƿ�����attendance���ʵ��
		bool   has_attendance;
	} Course;

	typedef struct {
		uint id;
		string name;
	} Group;

	enum {
		ATTEND	= 0,
		ABSENT	= 1,
		LATE	= 2,
		EXCUSE	= 3
	};

	typedef struct{
		uint id;
		uint sessdate;
		uint duration;
		//����ִ����id
		uint lasttakenby;
		//����״̬�����ڡ��ٵ�����١����Σ�
		uint statuses[4];
		//��Ҫд���״̬��(statusset)�����ݷ���(grade)����
		string	statuesset;
	} Session;

	//Ҳ����10��������
	const time_t  BEFORE_CLASS_BEGIN =  10 * 60 - 1;

	/**
	 * �γ̵�Context Level
	 * moodle����˶��壺define('CONTEXT_COURSE', 50);
	 */
	const int     COURSE_CONTEXT_LEVEL = 50;

	class Mdldb : public Mdldb_base
	{
	public:
		Mdldb(const string& db_host,
			  const string& db_port,
			  const string& db_user,
			  const string& db_passwd,
			  const string& passwordsalt
			  );
		Mdldb(Mdldb& mdl) : Mdldb_base(mdl){;}
		~Mdldb(void);

		//�жϿγ��Ƿ����attendance���ʵ��
		inline bool course_has_attendance() const {return m_course.has_attendance;}

		//�ж��ѹ����γ�
		inline bool course_associated()  const {return m_course.id > 0;}

		//�ж��ѹ����Ự
		inline bool session_associated() const {return m_sess.id > 0;}

		//�������û���Ϣ��֤
		void		auth(string username, string password);

		//�����γ̲���ȡ��Ӧ��״̬��m_statuses
		void		associate_course(uint32_t id);

		/** 
		 * ��ȡ������Ȩ�γ���Ϣ
		 * ֻ�пγ̵�ֱ�Ӹ����ˣ���ʦ��������ʦ����ӵ�п���Ȩ��
		 */
		void		get_authorized_course(vector<Course>& courses);

		//����ʱ�������Ӧ�ĻỰ
		bool		associate_session(const string session_name) 
			throw(mdldb::MDLDB_Exception);

		//����ʱ���ȡ���ܵĻỰ������
		void		get_session_discription(vector<string>& sessions, uint32_t course_id = 0);

		//����ѧ��ע����޸�ָ����Ϣ
		bool		fpenroll(Student& stu_info);

		//����ѧ��ɾ��ָ��
		void        fpdelete(string idnumber);

		//����ѧ���жϳ���״̬
		void		attendant(string idnumber, uint status, const string &description);

		// ��ȡ�γ̵�С��
		void		get_course_groups(vector<Group>& groups);

		// �����ز�������Ҫ�õ���ָ������ʱ��Ӧ��ʹ�ñ�����
		void		get_course_students(vector<Student>& students);

		//��ȡ����ѧ����Ϣ
		void		get_students(vector<Student>& students, const string& idnumber, size_t limit)
			throw(mdldb::MDLDB_Exception);

		inline Session get_session() {return m_sess;}
			
	private:
		bool		auth_user_exists(string username);
		bool		auth_check_passwd(uint id, string password);
		bool		auth_check_permission(uint id);

		bool		get_statusset();
		void		get_ordered_statusset();

		bool		attendance_log_exists(uint studentid);
		void		attendance_log_insert(uint studentid, uint status, const string& description);
		void		attendance_log_update(uint studentid, uint status, const string& description);

		int			get_userid(string idnumber);
		int         get_contextid(uint instanceid, uint contextlevel);
		int			get_course_contextid(uint instanceid);

		//moodle��Ϊ��ǿ���밲ȫ�Զ�ʹ�õ�passwordsalt
		static string	m_passwordsalt;

		//����ʹ����id
		static uint		m_userid;

		//�����Ŀγ�
		static Course   m_course;

		//�����ĻỰ
		static Session	m_sess;
	};
}