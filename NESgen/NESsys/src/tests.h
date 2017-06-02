
#ifndef TESTS_H_
#define TESTS_H_
#include "platform.h"


#define TESTS_HANDLE_ASSERTS		TRUE
#if TESTS_HANDLE_ASSERTS
#define assert(condition, ...)		if(!(condition)) error(__VA_ARGS__)
#else
#define assert(condition, ...)
#endif

void test_all();

#endif /* TESTS_H_ */
