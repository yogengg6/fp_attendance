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

#ifdef __GNUC__
#define sprintf_s(buffer, format, ...) sprintf(buffer, format, __VA_ARGS__)
#endif

using namespace std;

const time_t  BEFORE_CLASS_BEGIN =  10 * 60 - 1;


class MDLDB_Connector
{
public:
    MDLDB_Connector(void);

    //notice that session_name is session full name
    MDLDB_Connector(const char * const db_host,
                    const char * const db_user,
                    const char * const db_passwd,
                    const string course_name,
                    const string session_name
                    );

    MDLDB_Connector(const char * const db_host,
                    const char * const db_user,
                    const char * const db_passwd
                    );

    MDLDB_Connector(MDLDB_Connector& conn);

    ~MDLDB_Connector(void);

    bool    dbconnect(const char * const db_host,
                      const char * const db_user,
                      const char * const db_passwd
                      ) throw(MDLDB_Exception);

    bool    associate_course(const string course_name) throw(MDLDB_Exception);

    bool    associate_session(const string session_name) throw(MDLDB_Exception);

//     bool    enroll(const string &idnumber,
//                    const byte * const fp_data,
//                    size_t fp_size);
    bool    enroll(StudentInfo& stu_info);

    size_t  get_all_info(StudentInfo* student_list, 
                         const string class1="", 
                         const string class2=""
                         ) const;

    /**
     * string retrieved might be unreadable characters,
     * use MultiByteToWideChar() to convert.
     */
	StudentInfo get_info(const string& idnumber) throw(MDLDB_Exception);

	inline bool connection_established() const {return m_connection != NULL;}
    inline bool course_associated()      const {return m_course_id > 0;     }
    inline bool session_associated()     const {return m_session_id > 0;    }
    inline bool is_valid() const
    {
        return  (m_connection != NULL) && 
                (m_session_id > 0)     && 
                (m_course_id  > 0);
    }
protected:
    uint32_t m_course_id;
    uint32_t m_session_id;
    sql::Statement *m_statement;
    sql::Connection *m_connection;
    sql::mysql::MySQL_Driver *m_driver;
};

class DataBuf : public streambuf
{
public:
    DataBuf(char* d, const size_t s) {
        setg(d, d, d + s);
    }
};
