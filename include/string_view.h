#pragma once

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
	char* data;
	size_t size;
} String_View;

#define SV_FORMAT "%.*s"

void string_view_append(String_View *sv, const char* cstr);
bool string_view_compare(const String_View sv1, const String_View sv2);
bool string_view_compare_cstr(const String_View sv, const char* cstr);
size_t string_view_count_char(const String_View sv, const char c);
size_t string_view_split(const String_View sv, const char c, String_View  **buffer);
void string_view_free(String_View *sv);
