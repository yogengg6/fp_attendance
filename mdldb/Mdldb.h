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
		//�γ��Ƿ����˻Ự
		bool   has_session;
	} Course;

	typedef struct {
		uint id;
		string name;
	} Group;

	enum {
		ATTEND	= 0,
		ABSENT	= 1,
		LATE	= 2,
		EXCUSE	= 3,
	};

	typedef struct{
		uint id;
		uint sessdate;
		uint duration;
		//����ִ����id
		uint lasttakenby;
		//����״̬�����ڡ��ٵ�����١����Σ�
		uint statuses[4];
		//��Ҫд���statusset������grade����
		string	statuesset;
	} Session;

	const time_t  BEFORE_CLASS_BEGIN =  10 * 60 - 1;

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

		//�������û���Ϣ��֤
		void		auth(string username, string password);

		//�����γ̲���ȡ��Ӧ��״̬��m_statuses
		void		associate_course(uint32_t id);

		//�жϿγ��Ƿ���ڻỰ
		inline bool course_has_session() const {return m_course.has_session;}

		//�ж��ѹ����γ�
		inline bool course_associated()  const {return m_course.id > 0;}

		/** 
		 * ��ȡ������Ȩ�γ���Ϣ
		 * ֻ�пγ̵�ֱ�Ӹ����ˣ���ʦ�������ʦ����ӵ�п���Ȩ��
		 */
		void get_authorized_course(vector<Course>& courses);

		//����ʱ�������Ӧ�ĻỰ
		bool		associate_session(const string session_name) 
			throw(mdldb::MDLDB_Exception);

		//�ж��ѹ����Ự
		inline bool session_associated() const {return m_sess.id > 0;}

		//����ʱ���ȡ���ܵĻỰ������
		void get_session_discription(vector<string>& sessions, uint32_t course_id = 0);

		//����ѧ��ע����޸�ָ����Ϣ
		bool		fpenroll(Student& stu_info);

		//����ѧ��ɾ��ָ��
		void        fpdelete(string idnumber);

		//����ѧ���жϳ���״̬
		bool		attendant(string idnumber);


		// ��ȡ�γ̵�С��
		void get_course_groups(vector<Group>& groups);

		// �����ز�������Ҫ�õ���ָ������ʱ��Ӧ��ʹ�ñ�����
		void get_course_students(vector<Student>& students);


		/**
		* ��ȡ���ַ����ܻ������루�ֽ��룩,��˿�����Ҫʹ��
		* MultiByteToWideChar()��ת����
		*/

		//��ȡ����ѧ����Ϣ
		void get_students(vector<Student>& students, const string& idnumber, size_t limit)
			throw(mdldb::MDLDB_Exception);
			

	protected:

		//moodle��Ϊ��ǿ���밲ȫ�Զ�ʹ�õ�passwordsalt
		static string	m_passwordsalt;

		//����ʹ����id
		static uint	m_userid;

		//�����Ŀγ�
		static Course   m_course;

		//�����ĻỰ
		static Session	m_sess;
	};
}