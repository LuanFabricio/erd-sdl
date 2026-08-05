#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "utils.h"
#include "hash_map.h"

#include "parser.h"

#define BUFFER_CAPACITY 0xff

typedef struct {
	char value[BUFFER_CAPACITY];
	size_t size;
} Buffer;

void buffer_append_char(Buffer* buffer, const char c)
{
	assert(buffer->size < BUFFER_CAPACITY);
	buffer->value[buffer->size] = c;
	buffer->size += 1;
}

void buffer_clear(Buffer *buffer)
{
	buffer->size = 0;
	memset(buffer->value, 0, sizeof(buffer->value));
}

typedef enum {
	TK_ATTRIBUITION='=',
	TK_STRING_QUOTE='\"',
	TK_BEGIN_MAP='{',
	TK_END_MAP='}',
	TK_END_STATEMENT=',',
	TK_RECURSIVE='@',
	TK_FLOAT='.',
} Token;

typedef enum {
	ST_QUERY_ID=0,
	ST_QUERY_VALUE,
} State;

typedef struct {
	Buffer buffer;
	Token token;
	State state;
} Parser;

HashMap parse_from_filename(const char* filename)
{
	FILE* file = fopen(filename, "r");

	HashMap map = parse_from_file(file);
	fclose(file);

	return map;
}

static bool should_skip_token(int token)
{

	const bool is_not_tk_float = token != TK_FLOAT;
	const bool is_not_tk_recursive = token != TK_RECURSIVE;
	const bool is_not_tk_recursive_kind = token != NODE_RECURSIVE_KIND_KEY
		&& token != NODE_RECURSIVE_KIND_VALUE;
	const bool is_underscore = token == '_';
	const bool is_negative = token == '-';

	return !is_alphanum(token)
		&& is_not_tk_float
		&& is_not_tk_recursive
		&& is_not_tk_recursive_kind
		&& !is_underscore
		&& !is_negative;
}

HashMap parse_from_file(FILE* file)
{
	Parser parser = {
		.buffer = { .size = 0 },
		.state = ST_QUERY_ID,
	};
	memset(parser.buffer.value, 0, sizeof(parser.buffer.value));
	HashMap map = {0};

	char key[BUFFER_CAPACITY];
	int state = ST_QUERY_ID;
	int current_token;
	do {
		current_token = fgetc(file);
		switch (current_token) {
			case TK_ATTRIBUITION: {
				state = ST_QUERY_VALUE;
				strncpy(key, parser.buffer.value, sizeof(parser.buffer.value));
				buffer_clear(&parser.buffer);
			} continue;
			case TK_END_STATEMENT: {
				Node node = node_from_cstr(key, parser.buffer.value);
				buffer_clear(&parser.buffer);
				hash_map_append(&map, node);
				state = ST_QUERY_ID;
			} continue;
			case TK_BEGIN_MAP: {
				Node node = {
					.kind = NODE_KIND_MAP,
					.value = malloc(sizeof(HashMap)),
				};
				strncpy(node.key, key, NODE_KEY_CAPACITY);
				HashMap m = parse_from_file(file);
				*(HashMap*)node.value = m;
				hash_map_append(&map, node);
				state = ST_QUERY_ID;
				fseek(file, 1, SEEK_CUR);
			} continue;
			case TK_END_MAP: return map;
			case TK_STRING_QUOTE: continue;
		}

		if (should_skip_token(current_token)) {
			continue;
		}

		buffer_append_char(&parser.buffer, (char)current_token);
	} while(current_token != EOF);
	if (current_token == EOF && state == ST_QUERY_VALUE) {
		Node node = node_from_cstr(key, parser.buffer.value);
		buffer_clear(&parser.buffer);
		hash_map_append(&map, node);
	}

	return map;
}
