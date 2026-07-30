#include <stddef.h>
#include <stdbool.h>

#include "log.h"
#include "raylib.h"

#include "hash_map.h"
#include "parser.h"
#include "string_view.h"

int main(void)
{
	HashMap parsed_map = parse_from_filename("./sample.erd");
	hash_map_log(parsed_map, 0);

	log_format(
		stdout,
		LOG_LABEL_INFO,
		"========================= LOG String_View =========================\n");

	String_View sv = {0};
	const char* cstr = "Hello, world!";
	string_view_append(&sv, cstr);
	log_format(
		stdout,
		LOG_LABEL_INFO,
		SV_FORMAT"\n", (int)sv.size, sv.data);
	log_format(
		stdout,
		LOG_LABEL_INFO,
		"|"SV_FORMAT"| == %s (%b)\n",
		(int)sv.size, sv.data,
		cstr,
		string_view_compare_cstr(sv, cstr));

	string_view_append(&sv, "\nTesting");
	string_view_append(&sv, "\n\tmore messages");
	log_format(
		stdout,
		LOG_LABEL_INFO,
		SV_FORMAT"\n", (int)sv.size, sv.data);
	log_format(
		stdout,
		LOG_LABEL_INFO,
		"|"SV_FORMAT"| == %s (%b)\n",
		(int)sv.size, sv.data,
		cstr,
		string_view_compare_cstr(sv, cstr));


	sv.size = 0;
	string_view_append(&sv, "v1@v2@v3");
	log_format(
		stdout,
		LOG_LABEL_INFO,
		SV_FORMAT"\n", (int)sv.size, sv.data);

	String_View *split = NULL;
	const size_t split_len = string_view_split(sv, '@', &split);
	log_format(
		stdout,
		LOG_LABEL_INFO,
		"split_len: %lu\n", split_len);

	for (size_t i = 0; i < split_len; i++) {
		log_format(
			stdout,
			LOG_LABEL_INFO,
			SV_FORMAT"\n",
			(int)split[i].size, split[i].data);
	}

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
