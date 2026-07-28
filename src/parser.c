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

HashMap parse_from_file(FILE* file)
{
	log_format(stdout, LOG_LABEL_INFO, "position: %zu\n", ftell(file));

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
				log_format(stdout, LOG_LABEL_INFO, "Buffer: %s\n", parser.buffer.value);
				Node node = {
					.kind = NODE_KIND_MAP,
					.value = malloc(sizeof(HashMap)),
				};
				strncpy(node.key, key, NODE_KEY_CAPACITY);
				HashMap m = parse_from_file(file);
				log_format(stdout, LOG_LABEL_INFO, "map size: %zu\n", m.size);
				*(HashMap*)node.value = m;
				hash_map_append(&map, node);
				log_format(stdout, LOG_LABEL_INFO, "position: %zu\n", ftell(file));
				state = ST_QUERY_ID;
			} continue;
			case TK_END_MAP: return map;
			case TK_STRING_QUOTE: continue;
		}
		if (!is_alphanum(current_token) && current_token != TK_FLOAT) {
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
