#include <time.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "raylib.h"

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

	InitWindow(720, 480,  "ERD");
	char buffer[0xff];

	while (!WindowShouldClose()) {
		BeginDrawing();

		int  x = 20;
		const int y = 150;
		const int font_size = 32;
		const int padding = 35;
		for (size_t i = 0; i < parsed_map.size; i++) {
			Node node = parsed_map.items[i];
			char temp_buffer[NODE_VALUE_BUFFER_LEN];
			memset(temp_buffer, 0, sizeof(temp_buffer));
			node_value_to_cstr(node, temp_buffer);
			snprintf(buffer, sizeof(buffer), "%s: %s", node.key, temp_buffer);
			DrawText(buffer, x, y, font_size, GREEN);
			x += MeasureText(buffer, font_size) + padding;
		}

		EndDrawing();
	}

	CloseWindow();

	return 0;
}
