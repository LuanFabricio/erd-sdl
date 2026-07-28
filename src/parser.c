#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "utils.h"
#include "hash_map.h"
#include "log.h"

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

HashMap parse_from_file(const char* filename)
{
	FILE* file = fopen(filename, "r");

	Parser parser = {
		.buffer = {
			.size = 0,
		},
		.state = ST_QUERY_ID,
	};
	memset(parser.buffer.value, 0, sizeof(parser.buffer.value));
	HashMap map = {0};

	char key[BUFFER_CAPACITY];
	int state = ST_QUERY_ID;
	int current_token;
	while((current_token = fgetc(file)) != EOF) {
		switch (current_token) {
			case TK_ATTRIBUITION: {
				state = ST_QUERY_VALUE;
				log_format(stdout, LOG_LABEL_INFO, "id:%s\n", parser.buffer.value);
				strncpy(key, parser.buffer.value, sizeof(parser.buffer.value));
				buffer_clear(&parser.buffer);
			} continue;
			case TK_END_STATEMENT: {
				log_format(stdout, LOG_LABEL_INFO, "key:%s\n", key);
				log_format(stdout, LOG_LABEL_INFO, "value:%s\n", parser.buffer.value);

				Node node = node_from_cstr(key, parser.buffer.value);

				buffer_clear(&parser.buffer);
				hash_map_append(&map, node);
				state = ST_QUERY_ID;
			} continue;
			case TK_STRING_QUOTE: continue;
		}
		if (!is_alphanum(current_token) && current_token != TK_FLOAT) {
			continue;
		}

		buffer_append_char(&parser.buffer, (char)current_token);
	}
	fclose(file);

	log_format(stdout, LOG_LABEL_INFO, "key:%s\n", key);
	log_format(stdout, LOG_LABEL_INFO, "value:%s\n", parser.buffer.value);

	Node node = node_from_cstr(key, parser.buffer.value);
	hash_map_append(&map, node);
	buffer_clear(&parser.buffer);

	for (size_t i = 0; i < map.size; i++) {
		Node node = map.items[i];
		char buffer[NODE_VALUE_BUFFER_LEN];
		memset(buffer, 0, NODE_VALUE_BUFFER_LEN);
		node_value_to_cstr(node, buffer);
		log_format(stdout, LOG_LABEL_INFO,
				"\n"
				"key:%s\n"
				"value_ptr: %p\n"
				"value: %s\n"
				"kind: %s(%i)\n",
				node.key,
				node.value, buffer,
				node_kind_to_cstr(node.kind), node.kind);
	}

	return map;
}
