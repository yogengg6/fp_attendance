#pragma once

#include <cstdio>

#define FPA_COMMON
#define print_msg(m) printf("MESSAGE[%s]: %s\n", __FUNCTION__, (m));
#define print_err(e) printf("ERROR[%s at %s line %s]: %s\n", __FUNCTION__, __FILE__, __LINE__, (e));

using namespace std;

typedef int status_t;

const int STDOUT_BUFFER_SIZE = 400;
char stdout_buffer[STDOUT_BUFFER_SIZE];