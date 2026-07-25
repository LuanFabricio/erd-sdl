#include <time.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "hash_map.h"
#include "log.h"
#include "parser.h"

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

	parse_from_file("./sample.erd");

	return 0;
}
