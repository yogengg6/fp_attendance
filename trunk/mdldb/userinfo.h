#pragma once

#ifdef _MSC_VER
#include <windows.h>
#endif

#include <iostream>
#include <memory>
#include <string>

using namespace std;

namespace mdldb
{
	typedef unsigned int uint;

	typedef struct {
		uint    index;
		size_t  size;
		byte*	data;
	} Fpdata;

	class Student
	{
	public:
		Student()
		{
			memset(&m_fpdata, 0, sizeof(Fpdata));
		}

		Student(const string& idnumber,  
				Fpdata& fpdata);

		Student(const string& idnumber, 
				const string& fullname, 
				Fpdata& fpdata);

		~Student()
		{
			if (m_fpdata.data != NULL)
				delete m_fpdata.data;
		}

		inline void set_idnumber(const string& idnumber)
		{
			m_idnumber = idnumber;
		}
		inline void set_fullname(const string& fullname)
		{
			m_fullname = fullname;
		}
		inline void set_fpdata (Fpdata& fpdata)
		{
			if (m_fpdata.data != NULL)
				delete m_fpdata.data;
			memcpy(&m_fpdata, &fpdata, sizeof(Fpdata));
		}

		inline string get_idnumber() const {return m_idnumber; }
		inline string get_fullname() const {return m_fullname; }
		inline Fpdata get_fpdata  () const {return m_fpdata;   }

	private:
		string m_idnumber;
		string m_fullname;
		Fpdata m_fpdata;		
	};
}