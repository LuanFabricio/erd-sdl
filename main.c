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
	HashMap parsed_map = parse_from_file("./sample.erd");
	for (size_t i = 0; i < parsed_map.size; i++) {
		Node node = parsed_map.items[i];
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

	return 0;
}
