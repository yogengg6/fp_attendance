/**
 * Copyleft : This program is published under GPL
 * Author   : Yusuke
 * Email    : Qiuchengxuan@gmail.com
 * Date	    : 2011-4-22 21:18
 */

#pragma once
#define DEBUG

#include <cstdio>

#define FPA_COMMON
#define print_msg(module, msg) printf("[%s]MESSAGE: %s\n", module, (msg));
#define print_err(module, err) printf("[%s](%s at %s line %s)ERROR: %s\n", module, __FUNCTION__, __FILE__, __LINE__, (err));

using namespace std;

typedef int status_t;

const int STDOUT_BUFFER_SIZE = 400;
extern char stdout_buffer[STDOUT_BUFFER_SIZE];