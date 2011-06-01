#pragma once

#include <iostream>
#include <string>

namespace mdldb
{
	class StudentInfo 
	{
	public:
		StudentInfo();
		StudentInfo(const std::string&		idnumber, 
					const std::string&		fullname, 
					const size_t			fpsize	,
					const byte*				fpdata);
		~StudentInfo();

		inline void set_idnumber(const std::string& idnumber)
		{
			m_idnumber = idnumber;
		}
		inline void set_fullname(const std::string& fullname)
		{
			m_fullname = fullname;
		}
		inline void set_fpdata (const size_t fpsize, const byte* fpdata)
		{
			if (m_fpdata != NULL)
				delete m_fpdata;
			m_fpsize = fpsize;
			m_fpdata = fpdata;
		}

		inline std::string	get_idnumber() const {return m_idnumber; }
		inline std::string	get_fullname() const {return m_fullname; }
		inline size_t		get_fpsize	() const {return m_fpsize;   }
		inline const byte*	get_fpdata	() const {return m_fpdata;   }

	private:
		std::string				m_idnumber;
		std::string				m_fullname;
		size_t					m_fpsize;
		const byte*				m_fpdata;
	};
}