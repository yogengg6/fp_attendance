#include "StdAfx.h"
#include "MDLDB_StudentInfo.h"

StudentInfo::StudentInfo()
{
    this->idnumber = "";
    this->fullname = "";

    this->fpdata = NULL;
    this->fpsize = 0;
}

StudentInfo::StudentInfo(const string& idnumber, 
                         const string& fullname, 
                         const size_t  fpsize)
{
    this->idnumber = idnumber;
    this->fullname = fullname;

    this->fpsize = fpsize;
    this->fpdata = NULL;
}

StudentInfo::StudentInfo(const string& idnumber, 
                         const string& fullname, 
                         const size_t  fpsize, 
                               byte*&  fpdata)
{
    this->idnumber = idnumber;
    this->fullname = fullname;

    this->fpsize = fpsize;
    this->fpdata = fpdata;

    fpdata = NULL;
}

StudentInfo::~StudentInfo()
{
    if (this->fpdata != NULL)
        delete this->fpdata;
}
