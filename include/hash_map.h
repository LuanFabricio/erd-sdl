#pragma once

#include "dynamic_array.h"

#define NODE_VALUE_BUFFER_LEN 50

/**
 * SDL structure
 * key=value, where the value can be
 * - literal (int, float, string)
 * - recursive data
 * - field id (reference)
 */

typedef enum {
	NODE_KIND_INVALID = 0,
	NODE_KIND_INT,
	NODE_KIND_FLOAT,
	NODE_KIND_STRING,
	NODE_KIND_RECURSIVE_DATA,

	NODE_KIND_LEN,
} Node_Kind;

#define NODE_KEY_CAPACITY 100

typedef struct {
	char key[NODE_KEY_CAPACITY];
	void* value;
	Node_Kind kind;
} Node;

typedef da_create(Node) HashMap;

size_t hash_map_key_index(const HashMap* map, const char* key);
void hash_map_append(HashMap* map, Node node);
Node node_from_cstr(const char* key, const char* value);
const char* node_kind_to_cstr(const Node_Kind kind);
void node_value_to_cstr(Node n, char buffer[NODE_VALUE_BUFFER_LEN]);
