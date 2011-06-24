/**
 * Project Name : FP_Attendance
 * description  : a fingerprint based attendance(work with moodle attendanc
 *                e module)
 * Copyleft     : This program is published under GPL
 * Author1      : Yusuke(Qiuchengxuan@gmail.com)
 * Author2      : Edward(Edward.zhw@gmail.com)
 * Date	        : 2011-4-23 12:33
 */

#pragma warning(disable: 4244)

#include "stdafx.h"

#include <iostream>
#include <sstream>

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/sqlstring.h>
#include <cppconn/prepared_statement.h>

#include "Mdldb_base.h"
#include "exception.h"

using namespace std;
using namespace sql;

namespace mdldb
{
	typedef unsigned char byte;
	string								Mdldb_base::m_dbhost = "";
	string								Mdldb_base::m_dbuser = "";
	string								Mdldb_base::m_dbpasswd = "";

	auto_ptr<Connection>			Mdldb_base::m_mdl(NULL);
	auto_ptr<mysql::MySQL_Driver>	Mdldb_base::m_driver(NULL);

	/**
	 * this constructor only call dbconnect, and deal with exceptions.
	 * So that when you're attempting enroll student fingerprint, just use it.
	 */
	Mdldb_base::Mdldb_base(const string& db_host,
						   const string& db_port,
						   const string& db_user,
						   const string& db_passwd
						   )
	{
		m_dbhost = "tcp://" + db_host + ":" + db_port;
		m_dbuser = db_user;
		m_dbpasswd = db_passwd;
		try {
			if (m_driver.get() == NULL)
				m_driver = auto_ptr<mysql::MySQL_Driver>(mysql::get_driver_instance());
		} catch (...) {
			throw;
		}
	}

	/**
	 * create connection to MySQL Database server
	 */
	bool Mdldb_base::connect() throw(MDLDB_Exception)
	{
		try {
			m_mdl = auto_ptr<Connection>(m_driver->connect(m_dbhost, m_dbuser, m_dbpasswd));
			m_mdl->setSchema("moodle");
			m_mdl->setClientOption("OPT_SET_CHARSET_NAME", "utf8");
		} catch (SQLException &e) {
			switch (e.getErrorCode()) {
			case 1045:
				throw MDLDB_Exception("连接被拒绝", CONNECTION_REFUSED);
				break;
			case 2003:
			case 10060:
			case 10061:
				throw MDLDB_Exception("连接失败", CONNECTION_FAIL);
				break;
			default:
				throw MDLDB_Exception(e.what(), CONNECTION_UNKNOW);
				break;
			}
		}
		return m_mdl.get() != NULL;
	}
}