#pragma once
#pragma warning(disable:4244)
#include <string>

#include "Mdldb_base.h"
#include "userinfo.h"

using namespace std;

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

		//����ʱ�������Ӧ�ĻỰ
		bool		associate_session(const string session_name) 
					throw(mdldb::MDLDB_Exception);

		//�жϿγ��Ƿ���ڻỰ
		inline bool course_has_session() const {return m_course_has_session;}

		//�ж��ѹ����γ�
		inline bool course_associated()  const {return m_course_id > 0;}

		//�ж��ѹ����Ự
		inline bool session_associated() const {return m_sess_info.id > 0;}

		//����ѧ��ע����޸�ָ����Ϣ
		bool		fpenroll(StudentInfo& stu_info);

		//����ѧ��ɾ��ָ��
		void        fpdelete(string idnumber);

		//����ѧ���жϳ���״̬
		bool		attendant(string idnumber);

		/** 
		 * ��ȡ������Ȩ�γ���Ϣ
		 * ֻ�пγ̵�ֱ�Ӹ����ˣ���ʦ�������ʦ����ӵ�п���Ȩ��
		 */
		vector<CourseInfo>  get_authorized_course();

		//����ʱ���ȡ���ܵĻỰ������
		vector<string> get_session_discription(uint32_t course_id = 0);

		// �����ز�������Ҫ�õ���ָ������ʱ��Ӧ��ʹ�ñ�����
		void  get_course_student_info(vector<StudentInfo> &student_info);


		/**
		* ��ȡ���ַ����ܻ������루�ֽ��룩,��˿�����Ҫʹ��
		* MultiByteToWideChar()��ת����
		*/

		//��ȡ����ѧ����Ϣ
		vector<StudentInfo> get_student_info(const string& idnumber, size_t limit)
			throw(mdldb::MDLDB_Exception);
			

	protected:
		enum {
			ATTEND	= 0,
			LATE	= 1,
			EXCUSE	= 2,
			ABSENT	= 3
		};

		//moodle��Ϊ��ǿ���밲ȫ�Զ�ʹ�õ�passwordsalt
		static string	m_passwordsalt;

		//����ʹ����id
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
	};
}