#pragma once

#include <string>

using namespace std;

const int CFG_DBHOST = 0;
const int CFG_DBPORT = 1;
const int CFG_DBUSER = 2;
const int CFG_DBPASSWD = 3;
const int CFG_PASSWORDSALT = 4;

class Config {
public:
	enum{BUFFER_SIZE = 100};

	Config();
	~Config()
	{
		save();
	}

	bool load();
	void save();
	void create();

	static string m_dbhost;
	static string m_dbport;
	static string m_dbuser;
	static string m_dbpasswd;
	static string m_passwordsalt;

	string& operator [](int index);

protected:
	static CString m_path;
	static CString m_buffer;

	void encode_buffer();
	void decode_buffer();

	inline byte hexchar_to_byte(wchar_t c) {
		return c < 'a'? c-'0': c-'a'+10;
	}

	inline int read_profile_string(wchar_t* section, wchar_t* field) {
		int val = GetPrivateProfileString(section, field, L"", m_buffer.GetBufferSetLength(BUFFER_SIZE), BUFFER_SIZE, m_path);
		m_buffer.ReleaseBuffer();
		return val;
	}

	inline int write_profile_string(const wchar_t* section , const wchar_t* field, const wchar_t* val) {
		return WritePrivateProfileString(section, field, val, m_path);
	}
};