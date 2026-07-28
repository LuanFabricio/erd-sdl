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
	HashMap parsed_map = parse_from_filename("./sample.erd");
	hash_map_log(parsed_map, 0);

#if 0
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
#endif

	return 0;
}
