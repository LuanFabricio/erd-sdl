#include <assert.h>
#include <stdbool.h>

#include "log.h"

#define UNREACHABLE(x) assert(false && x);

const char* log_label_cstr(const log_label_e label)
{
	switch (label) {
		case LOG_LABEL_INFO:
			return "INFO";
		case LOG_LABEL_WARNING:
			return "WARNING";
		case LOG_LABEL_ERROR:
			return "ERROR";
	}
	UNREACHABLE("Invalid label");
}
