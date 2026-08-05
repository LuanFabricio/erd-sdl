#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "raylib.h"
#include "raymath.h"

#include "hash_map.h"
#include "parser.h"
#include "table.h"
#include "log.h"

#define MOUSE_DRAGGIN_SPEED 10.f

int main(void)
{
	HashMap parsed_map = parse_from_filename("./sample.erd");
	InitWindow(720, 480,  "ERD");
	char buffer[0xff];

	Camera2D camera = {
		.offset = {0, 0},
		.target = {0, 0},
		.rotation = 0,
		.zoom = 1.f,
	};

	struct {
		Vector2 origin;
		bool active;
	} anchor = {0};

	hash_map_log(parsed_map, 0);

	void* table_map_ptr = parsed_map.items[hash_map_key_index(&parsed_map, "table")].value;
	Table t = table_create(table_map_ptr);

	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(BLACK);

		DrawFPS(0, 0);
		BeginMode2D(camera);

		table_draw(t);

		int x = 0;
		const int y = 150;
		const int font_size = 32;
		const int padding = 35;
		for (size_t i = 0; i < parsed_map.size; i++) {
			Node node = parsed_map.items[i];
			char temp_buffer[NODE_VALUE_BUFFER_LEN];
			memset(temp_buffer, 0, sizeof(temp_buffer));
			node_value_to_cstr(parsed_map, i, temp_buffer, sizeof(temp_buffer));
			snprintf(buffer, sizeof(buffer), "%s: %s", node.key, temp_buffer);
			DrawText(buffer, x, y, font_size, GREEN);
			x += MeasureText(buffer, font_size) + padding;
		}

		DrawText("Test", -200, 0, 24, GREEN);

		EndMode2D();

		EndDrawing();

		const Vector2 mouse_pos = GetScreenToWorld2D(GetMousePosition(), camera);
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			anchor.origin = mouse_pos;
			anchor.active = true;
		}
		if (anchor.active) {
			const Vector2 delta = Vector2Subtract(mouse_pos, anchor.origin);
			camera.target = Vector2Subtract(
				camera.target,
				Vector2Scale(delta, MOUSE_DRAGGIN_SPEED * GetFrameTime()));
		}

		if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			anchor.active = false;
		}
		camera.zoom += 10 * GetFrameTime() * GetMouseWheelMove();
	}

	CloseWindow();

	return 0;
}
