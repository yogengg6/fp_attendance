#include "stdafx.h"

#include <iostream>
#include <fstream>
#include <memory>

#include "ultility.h"

using namespace std;

CString stringToCString(string& str)
{
	static size_t max_size = 0;
	static auto_ptr<wchar_t> conv_buf;

	size_t str_size = MultiByteToWideChar (CP_UTF8, NULL, str.c_str(), -1, NULL, 0);
	if (str_size > max_size) {
		conv_buf.release();
		conv_buf = auto_ptr<wchar_t>(new wchar_t[max_size = str_size]);
	}
	ZeroMemory(conv_buf.get(), str_size);
	MultiByteToWideChar(CP_UTF8, NULL, str.c_str(), -1, conv_buf.get(), str_size);
	return CString(conv_buf.get());
}

string CStringToString(CString& cstr)
{
	static size_t max_size = 0;
	static auto_ptr<char> conv_buf;

	size_t cstr_size = (cstr.GetLength()+1)*2;
	if (cstr_size > max_size) {
		conv_buf.release();
		conv_buf = auto_ptr<char>(new char[max_size = cstr_size]);
	}
	ZeroMemory(conv_buf.get(), cstr_size);
	wcstombs_s(NULL, conv_buf.get(), cstr_size, cstr, _TRUNCATE);
	return string(conv_buf.get());
}