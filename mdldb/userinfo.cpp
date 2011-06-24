#include "StdAfx.h"
#include "userinfo.h"

using namespace std;

namespace mdldb
{
	Student::Student(const string& idnumber, 
					 Fpdata& fpdata)
	{
		m_idnumber	   = idnumber;
		memcpy(&m_fpdata, &fpdata, sizeof(Fpdata));
	}

	Student::Student(const string& idnumber, 
					 const string& fullname, 
					 Fpdata& fpdata)
	{
		m_idnumber	= idnumber;
		m_fullname	= fullname;
		memcpy(&m_fpdata, &fpdata, sizeof(Fpdata));
	}
}
