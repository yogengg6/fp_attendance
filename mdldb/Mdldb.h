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
		//考勤执行者id
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

		//考勤者用户信息验证
		void		auth(string username, string password);

		//关联课程并获取相应的状态集m_statuses
		void		associate_course(uint32_t id);

		//根据时间关联相应的会话
		bool		associate_session(const string session_name) 
					throw(mdldb::MDLDB_Exception);

		//判断课程是否存在会话
		inline bool course_has_session() const {return m_course_has_session;}

		//判断已关联课程
		inline bool course_associated()  const {return m_course_id > 0;}

		//判断已关联会话
		inline bool session_associated() const {return m_sess_info.id > 0;}

		//根据学号注册或修改指纹信息
		bool		fpenroll(StudentInfo& stu_info);

		//根据学号删除指纹
		void        fpdelete(string idnumber);

		//根据学号判断出勤状态
		bool		attendant(string idnumber);

		/** 
		 * 获取所有授权课程信息
		 * 只有课程的直接负责人（教师、助理教师）才拥有考勤权限
		 */
		vector<CourseInfo>  get_authorized_course();

		//根据时间获取可能的会话的描述
		vector<string> get_session_discription(uint32_t course_id = 0);

		// 当本地不存在需要用到的指纹数据时才应当使用本函数
		void  get_course_student_info(vector<StudentInfo> &student_info);


		/**
		* 获取的字符可能会是乱码（字节码）,因此可能需要使用
		* MultiByteToWideChar()来转换。
		*/

		//获取部分学生信息
		vector<StudentInfo> get_student_info(const string& idnumber, size_t limit)
			throw(mdldb::MDLDB_Exception);
			

	protected:
		enum {
			ATTEND	= 0,
			LATE	= 1,
			EXCUSE	= 2,
			ABSENT	= 3
		};

		//moodle中为加强密码安全性而使用的passwordsalt
		static string	m_passwordsalt;

		//程序使用者id
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
	};
}