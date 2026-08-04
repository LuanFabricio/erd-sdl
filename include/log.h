#pragma once

#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

typedef enum {
	LOG_LABEL_INFO,
	LOG_LABEL_WARNING,
	LOG_LABEL_ERROR
} log_label_e;

#define log_label_format(label) "["label"]"

const char* log_label_cstr(const log_label_e label);

#define log_vector3(vec) _log_vector3(vec, #vec)
#define log_format(file, label, ...) \
	do {\
		fprintf(file, "[%s|%s:%d]: ", log_label_cstr(label), __FILE__, __LINE__); \
		fprintf(file, __VA_ARGS__); \
	} while(0)

#define log_assert(exp, msg)\
	do {  \
		assert((exp) && msg);\
	} while(0)

#define log_panic(...) \
	do {\
		log_format(stderr, LOG_LABEL_ERROR, __VA_ARGS__); \
		assert(false); \
	} while(0)
