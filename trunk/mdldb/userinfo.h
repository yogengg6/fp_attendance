#pragma once

#include <iostream>
#include <string>

using namespace std;

namespace mdldb
{
	typedef unsigned int uint;

	typedef struct {
		uint        index;
		size_t      size;
		const byte* data;
	} Fpdata;

	class StudentInfo 
	{
	public:
		StudentInfo();
		StudentInfo(const string& idnumber,  
					const Fpdata& fpdata);
		StudentInfo(const string& idnumber, 
					const string& fullname, 
					const Fpdata& fpdata);
		~StudentInfo();

		inline void set_idnumber(const string& idnumber)
		{
			m_idnumber = idnumber;
		}
		inline void set_fullname(const string& fullname)
		{
			m_fullname = fullname;
		}
		inline void set_fpdata (const Fpdata& fpdata)
		{
			if (m_fpdata.data != NULL)
				delete m_fpdata.data;
			CopyMemory(&m_fpdata, &fpdata, sizeof(Fpdata));
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