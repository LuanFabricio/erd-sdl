#include <time.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "dynamic_array.h"
#include "log.h"

/**
 * SDL structure
 * key=value, where the value can be
 * - literal (int, float, string)
 * - recursive data
 * - field id (reference)
 */

typedef enum {
	NODE_KIND_INT = 0,
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
		log_format(
			stdout,
			LOG_LABEL_INFO,
			"%p:\n"
			"\tkey=%s\n"
			"\tvalue ptr=%p\n"
			"\tvalue int=%d\n"
			"\tkind=%u\n",
			node, node->key, node->value, *(int*)node->value, node->kind);
		return;
	}

	da_append(map, node);
}

int main(void)
{
	srand(time(NULL));

	HashMap map = {0};
	for (int i = 0; i < 10; i++) {
		Node node = {
			.value = malloc(sizeof(int)),
			.kind = (rand() % NODE_KIND_LEN) - 1
		};
		memset(node.key, 0, 10);
		snprintf(node.key, 10, "%c%c", 'a' + i, 'a' + rand() % 24);
		*(int*)node.value = 42;

		hash_map_append(&map, node);
	}

	for (size_t i = 0; i < map.size; i++) {
		Node *node = &map.items[i];

		log_format(
			stdout,
			LOG_LABEL_INFO,
			"%p:\n"
			"\tkey=%s\n"
			"\tvalue ptr=%p\n"
			"\tvalue int=%d\n"
			"\tkind=%u\n",
			node, node->key, node->value, *(int*)node->value, node->kind);
	}

	return 0;
}
