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
#include <vector>

//mysql header
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

#include "exception.h"

using namespace std;
using namespace sql;

namespace mdldb{

	typedef unsigned int uint;

	class SQLContainer
	{
	public:
		SQLContainer(Connection* c, const char* sql)
			: m_count(1)
		{
			m_prep_stmt = 
				auto_ptr<PreparedStatement>(c->prepareStatement(sql));
		}

		inline void		  execute()
		{
			m_prep_stmt->execute();
		}
		inline ResultSet* exeQuery()
		{
			return m_prep_stmt->executeQuery();
		}
		inline int		  exeUpdate()
		{
			return m_prep_stmt->executeUpdate();
		}

		inline friend SQLContainer& operator <<(SQLContainer& c, const int arg)
		{
			c.m_prep_stmt->setInt(c.m_count++, arg);
			return c;
		}

		inline friend SQLContainer& operator <<(SQLContainer& c, const uint arg)
		{
			c.m_prep_stmt->setUInt(c.m_count++, arg);
			return c;
		}

		inline friend SQLContainer& operator <<(SQLContainer& c, const char* arg)
		{
			c.m_prep_stmt->setString(c.m_count++, arg);
			return c;
		}

		inline friend SQLContainer& operator <<(SQLContainer& c, const string arg)
		{
			c.m_prep_stmt->setString(c.m_count++, arg);
			return c;
		}

		inline friend SQLContainer& operator <<(SQLContainer& c, istream* arg)
		{
			c.m_prep_stmt->setBlob(c.m_count++, arg);
			return c;
		}

	private:
		auto_ptr<PreparedStatement>	m_prep_stmt;
		uint						m_count;
	};

	

	class Mdldb_base
	{
	public:
		Mdldb_base(const string& db_host,
				   const string& db_port,
				   const string& db_user,
				   const string& db_passwd
				   );

		Mdldb_base(Mdldb_base& mdl_base) {;}

		~Mdldb_base(void) {;}

		inline void set_dbhost(string dbhost, string dbport) {
			m_dbhost = "tcp://" + dbhost + ":" + dbport;
		}

		inline void set_dbaccount(string dbuser, string dbpasswd)
		{
			m_dbuser = dbuser;
			m_dbpasswd = dbpasswd;
		}

		//连接数据库
		bool		connect() throw(MDLDB_Exception);
		//判断已连接数据库
		inline bool connected() const {return m_mdl.get() != NULL;}

	protected:
		static string	m_dbhost;
		static string	m_dbuser;
		static string	m_dbpasswd;

		static auto_ptr<Connection>			m_mdl;
		static auto_ptr<mysql::MySQL_Driver>	m_driver;
	};

}