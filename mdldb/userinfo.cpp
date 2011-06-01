#include "StdAfx.h"
#include "userinfo.h"

namespace mdldb
{
	StudentInfo::StudentInfo()
	{
		m_idnumber = "";
		m_fullname = "";
		m_fpsize = 0;
		m_fpdata = NULL;
	}

	StudentInfo::StudentInfo(const std::string&		idnumber, 
							 const std::string&		fullname, 
							 const size_t			fpsize, 
							 const byte*			fpdata)
	{
		m_idnumber	= idnumber;
		m_fullname	= fullname;

		m_fpsize	= fpsize;
		m_fpdata	= fpdata;
	}

	StudentInfo::~StudentInfo()
	{
		if (m_fpdata != NULL)
			delete m_fpdata;
	}
}
