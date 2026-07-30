#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "utils.h"
#include "hash_map.h"
#include "string_view.h"

static void* node_malloc_by_kind(const Node_Kind kind, size_t len)
{
	size_t bytes = 0;
	void *ptr = NULL;

	switch (kind) {
		case NODE_KIND_INT: {
			ptr = malloc(sizeof(int));
		} break;
		case NODE_KIND_FLOAT: {
			ptr = malloc(sizeof(float));
		} break;
		case NODE_KIND_RECURSIVE_DATA:
		case NODE_KIND_STRING: {
			ptr = malloc(sizeof(char) * len);
		} break;
		default:
			assert(false && "Invalid `Node_Kind`");
	}

	return ptr;
}

size_t hash_map_key_index(const HashMap* map, const char* key)
{
	da_for_each(map, Node) {
		if (strcmp(loop.item->key, key) == 0) {
			return loop.i;
		}
	}
	return -1;
}

void hash_map_append(HashMap* map, Node node)
{
	const size_t index = hash_map_key_index(map, node.key);
	if (index != -1) {
		log_format(stdout, LOG_LABEL_WARNING, "Key `%s` already exists in the hash map (index %lu).\n", node.key, index);

		const Node* node = &map->items[index];
		char temp_buffer[NODE_VALUE_BUFFER_LEN];
		memset(temp_buffer, 0, NODE_VALUE_BUFFER_LEN);
		node_value_to_cstr(*map, index, temp_buffer);
		log_format(
			stdout,
			LOG_LABEL_INFO,
			"%p:\n"
			"\tkey=%s\n"
			"\tvalue ptr=%p\n"
			"\tvalue=%s\n"
			"\tkind=%u\n",
			node, node->key, node->value, temp_buffer, node->kind);
		return;
	}

	da_append(map, node);
}

Node node_from_cstr(const char key[NODE_KEY_CAPACITY], const char* value)
{
	Node node = {
		.kind = NODE_KIND_INVALID,
		.value = NULL,
	};
	strncpy(node.key, key, NODE_KEY_CAPACITY);

	size_t dot_count = 0;
	size_t dot_last_index = -1;
	Node_Kind kind = NODE_KIND_INT;
	const size_t value_len = strlen(value);
	for (size_t i = 0; i < value_len; i++) {
		const char token = value[i];
		if (is_alpha(token)) {
			kind = NODE_KIND_STRING;
			break;
		} else if (token == '.') {
			dot_count++;
			dot_last_index = i;
		}
	}

	if (dot_count == 1) {
		kind = NODE_KIND_FLOAT;
	} else if (dot_count > 1) {
		kind = NODE_KIND_STRING;
	}
	if (value[0] == '@') {
		kind = NODE_KIND_RECURSIVE_DATA;
	}

	node.kind = kind;
	node.value = node_malloc_by_kind(kind, value_len + 1);

	switch (kind) {
		case NODE_KIND_RECURSIVE_DATA:
		case NODE_KIND_STRING: {
			 memset(node.value, 0, value_len + 1);
			 strncpy(node.value, value, value_len);
	       } break;
		case NODE_KIND_INT: {
			*(int*)node.value = atoi(value);
	       } break;
		case NODE_KIND_FLOAT: {
			*(float*)node.value = atof(value);
	       } break;
		default:
			assert(false && "Invalid `Node_Kind`");
	}

	return node;
}

const char* node_kind_to_cstr(const Node_Kind kind)
{
	switch (kind) {
		case NODE_KIND_INT: return "int";
		case NODE_KIND_FLOAT: return "float";
		case NODE_KIND_STRING: return "string";
		case NODE_KIND_MAP: return "map";
		case NODE_KIND_RECURSIVE_DATA: return "recursive";
		default:
			assert(false && "Unreacheable");
	}
}

void node_value_to_cstr(const HashMap map, const size_t node_index, char *buffer)
{
	Node n = map.items[node_index];
	switch (n.kind) {
		case NODE_KIND_INT:
			snprintf(buffer, NODE_VALUE_BUFFER_LEN, "%d", *(int*)n.value);
			break;
		case NODE_KIND_FLOAT:
			snprintf(buffer, NODE_VALUE_BUFFER_LEN, "%f", *(float*)n.value);
			break;
		case NODE_KIND_RECURSIVE_DATA: {
				String_View sv = {0};
				string_view_append(&sv, n.value);
				string_view_strip_char(&sv, '@');
				String_View *split = NULL;
				size_t split_len = string_view_split(sv, '.', &split);

				// TODO: Move temp_buffer size to a const
				char temp_buffer[0xff];
				const HashMap* current_map = &map;
				for (size_t i = 0; i < split_len - 1; i++) {
					// TODO: Refactor/extract this
					memset(temp_buffer, 0, sizeof(temp_buffer));
					assert(sizeof(temp_buffer) > split->size);
					memcpy(temp_buffer, split[i].data, split->size);
					size_t j = hash_map_key_index(current_map, temp_buffer);
					assert(j != -1);
					Node *n = &map.items[j];
					assert(n != NULL);
					assert(n->kind == NODE_KIND_MAP);
					current_map = n->value;
				}
				memset(temp_buffer, 0, sizeof(temp_buffer));
				assert(sizeof(temp_buffer) > split->size);
				memcpy(temp_buffer, split[split_len-1].data, split->size);
				size_t i = hash_map_key_index(current_map, temp_buffer);
				// TODO: Add the buffer size as parameter
				snprintf(
					buffer,
					NODE_VALUE_BUFFER_LEN,
					"recursive(%s)|",
					(char*)n.value);
				const size_t buffer_offset = strlen(buffer);
				node_value_to_cstr(*current_map, i, buffer + buffer_offset);

				string_view_free(&sv);
				for (size_t i = 0; i < split_len; i++) {
					string_view_free(&split[i]);
				}
		       } break;
		case NODE_KIND_STRING:
			snprintf(buffer, NODE_VALUE_BUFFER_LEN, "%s", (char*)n.value);
			break;
		case NODE_KIND_MAP: {
			HashMap m = *(HashMap*)n.value;
			// NOTE: Maybe print the nested values
			snprintf(buffer, NODE_VALUE_BUFFER_LEN, "map size %zu", m.size);
		} break;
		default: {
			log_format(
				stdout,
				LOG_LABEL_ERROR,
				"Node kind invalid: %d\n", n.kind);
			assert(false && "Unreacheable");
		}
	}
}

void hash_map_log(const HashMap map, const int depth)
{
	for (size_t i = 0; i < map.size; i++) {
		Node node = map.items[i];
		char buffer[NODE_VALUE_BUFFER_LEN];
		memset(buffer, 0, NODE_VALUE_BUFFER_LEN);
		node_value_to_cstr(map, i, buffer);
		log_format(stdout, LOG_LABEL_INFO,
				"\n"
				"%*skey:%s\n"
				"%*svalue_ptr: %p\n"
				"%*svalue: %s\n"
				"%*skind: %s(%i)\n",
				depth * 2, "", node.key,
				depth * 2, "", node.value,
				depth * 2, "", buffer,
				depth * 2, "", node_kind_to_cstr(node.kind), node.kind);
		if (node.kind == NODE_KIND_MAP) {
			hash_map_log(*(HashMap*)node.value, depth+1);
		}
	}
}
