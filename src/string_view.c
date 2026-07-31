#include "string_view.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEMP_BUFFER_LEN 1024
static char temp_buffer[TEMP_BUFFER_LEN];

void string_view_append(String_View *sv, const char* cstr)
{
	if (sv->data == NULL) {
		sv->size = 0;
	}

	const size_t cstr_len = strlen(cstr);
	const size_t prev_size = sv->size;
	const size_t new_size = sv->size + cstr_len;

	if (new_size > prev_size) {
		// NOTE: According with `man realloc(3)`
		// "If p is NULL, then the call is equivalent to malloc(size), for all values of size."
		sv->data = realloc(sv->data, new_size);
	}
	memcpy(sv->data + prev_size, cstr, cstr_len);
	sv->size = new_size;
}

void string_view_free(String_View *sv)
{
	sv->size = 0;
	free(sv->data);
	sv->data = NULL;
}

bool string_view_compare(const String_View sv1, const String_View sv2)
{
	if (sv1.size != sv2.size) {
		return false;
	}

	for (size_t i = 0; i < sv1.size; i++) {
		if (sv1.data[i] != sv2.data[i]) {
			return false;
		}
	}

	return true;
}

bool string_view_compare_cstr(const String_View sv, const char* cstr)
{
	const size_t cstr_len = strlen(cstr);
	if (sv.size != cstr_len) {
		return false;
	}

	for (size_t i = 0; i < sv.size; i++) {
		if (sv.data[i] != cstr[i]) {
			return false;
		}
	}

	return true;
}

size_t string_view_count_char(const String_View sv, const char c)
{
	size_t count = 0;

	for (size_t i = 0; i < sv.size; i++) {
		if (sv.data[i] == c) {
			count++;
		}
	}

	return count;
}

static void split_copy_offset(String_View sv, String_View* output, const size_t len, size_t offset)
{
	char temp_buffer[TEMP_BUFFER_LEN];
	memset(temp_buffer, 0, TEMP_BUFFER_LEN);
	snprintf(
			temp_buffer,
			TEMP_BUFFER_LEN,
			SV_FORMAT,
			(int)len, sv.data + offset);
	log_format(
			stdout,
			LOG_LABEL_INFO,
			"%s\n", temp_buffer);
	string_view_append(output, temp_buffer);
}

size_t string_view_split(const String_View sv, const char c, String_View **buffer)
{
	const size_t splits = string_view_count_char(sv, c) + 1;
	*buffer = malloc(sizeof(String_View) * splits);
	memset(*buffer, 0, sizeof(String_View) * splits);

	size_t last_index_c = 0;

	for (size_t i = 0, j = 0; i <= sv.size && j < splits; i++) {
		if (sv.data[i] == c || i == sv.size) {
			size_t temp_buffer_len = i - last_index_c;
			char temp_buffer[TEMP_BUFFER_LEN];
			memset(temp_buffer, 0, TEMP_BUFFER_LEN);
			snprintf(
				temp_buffer,
				TEMP_BUFFER_LEN,
				SV_FORMAT,
				(int)temp_buffer_len, sv.data + last_index_c);
			string_view_append((*buffer + j), temp_buffer);
			j++;
			last_index_c = i + 1;

		}
	}

	return splits;
}

void string_view_strip_char(String_View* sv, const char search)
{
	char* buffer = malloc(sv->size);
	size_t buffer_size = 0;
	for (size_t i = 0; i < sv->size; i++) {
		if (sv->data[i] == search) {
			continue;
		}
		buffer[buffer_size++] = sv->data[i];
	}

	memcpy(sv->data, buffer, buffer_size);
	sv->size = buffer_size;
	free(buffer);
}
