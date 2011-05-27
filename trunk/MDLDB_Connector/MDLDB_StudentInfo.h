#pragma once

#include <iostream>
#include <string>

namespace mdldb
{
	class StudentInfo 
	{
	public:
		StudentInfo();
		StudentInfo(const std::string& idnumber, 
					const std::string& fullname, 
					const size_t  fpsize);
		StudentInfo(const std::string& idnumber, 
					const std::string& fullname, 
					const size_t  fpsize, 
					byte*&  fpdata);
		~StudentInfo();

		inline void set_idnumber(const std::string& idnumber)
		{
			this->idnumber = idnumber;
		}
		inline void set_fullname(const std::string& fullname)
		{
			this->fullname = fullname;
		}
		inline void set_fpsize (const size_t fpsize)
		{
			this->fpsize = fpsize;
		}
		inline void set_fpdata (byte*& fpdata)
		{
			if (this->fpdata != NULL)
				delete this->fpdata;
			this->fpdata = fpdata;
			fpdata = NULL;
		}

		inline std::string  get_idnumber() const {return this->idnumber; }
		inline std::string  get_fullname() const {return this->fullname; }
		inline size_t       get_fpsize()   const {return this->fpsize;   }
		inline byte*        get_fpdata()   const {return this->fpdata;   }

	private:
		std::string idnumber;
		std::string fullname;
		size_t      fpsize;
		byte* fpdata;
	} ;
}