#pragma once

#include <iostream>
#include <string>

using namespace std;

class StudentInfo 
{
public:
	StudentInfo();
    StudentInfo(const string& idnumber, 
                const string& fullname, 
                const size_t  fpsize);
	StudentInfo(const string& idnumber, 
                const string& fullname, 
                const size_t  fpsize, 
                      byte*&  fpdata);
	~StudentInfo();

	inline void set_idnumber(const string& idnumber)
    {
        this->idnumber = idnumber;
    }
    inline void set_fullname(const string& fullname)
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

	inline string       get_idnumber() const {return this->idnumber; }
    inline string       get_fullname() const {return this->fullname; }
	inline size_t       get_fpsize()   const {return this->fpsize;   }
	inline byte*        get_fpdata()   const {return this->fpdata;   }

private:
	string      idnumber;
    string      fullname;
	size_t      fpsize;
	byte* fpdata;
} ;