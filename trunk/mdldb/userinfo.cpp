#include "StdAfx.h"
#include "userinfo.h"

using namespace std;

namespace mdldb
{
	StudentInfo::StudentInfo()
	{
		m_idnumber = "";
		m_fullname = "";
		ZeroMemory(&m_fpdata, sizeof(Fpdata));
	}

	StudentInfo::StudentInfo(const string& idnumber, 
							 const Fpdata& fpdata)
	{
		m_idnumber	= idnumber;
		CopyMemory(&m_fpdata, &fpdata, sizeof(Fpdata));
	}

	StudentInfo::StudentInfo(const string& idnumber, 
							 const string& fullname, 
							 const Fpdata& fpdata)
	{
		m_idnumber	= idnumber;
		m_fullname	= fullname;
		CopyMemory(&m_fpdata, &fpdata, sizeof(Fpdata));
	}

	StudentInfo::~StudentInfo()
	{
		if (m_fpdata.data != NULL)
			delete m_fpdata.data;
	}
}
