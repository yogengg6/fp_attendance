#include "stdafx.h"

#include <iostream>
#include <fstream>

#include "ultility.h"

using namespace std;

CString stringToCString(string& str)
{
	static wchar_t conv_buf[200];
	MultiByteToWideChar(CP_UTF8, NULL, str.c_str(), -1, conv_buf, str.length());
	return CString(conv_buf);
}

string CStringToString(CString& cstr)
{
	static char conv_buf[200];
	wcstombs_s(NULL, conv_buf, (cstr.GetLength() + 1) * 2, cstr, _TRUNCATE);
	return string(conv_buf);
}

// bool fcreate_conf()
// {
// 	ofstream fout("config.ini");
// 
// 	if (!fout.is_open())
// 		return false;
// 
// 	fout << "dbhost = 172.16.81.156\n";
// 	fout << "dbport = 3306\n";
// 	fout << "dbuser = attendance\n";
// 	fout << "dbpassword = 0C00000000000000C748F2CAE20498F5DF0CCF5D0EB1A9D1";
// 	return true;
// }
// 
// bool fread_conf()
// {
// 	ifstream fin("config.ini");
// 
// 	if (!fout.is_open())
// 		return false;
// 
// 	return true;
// }