#include <assert.h>
#include <raylib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "table.h"
#include "hash_map.h"
#include "log.h"

#define TABLE_NAME_FONT_SIZE 32
#define TABLE_ROW_FONT_SIZE 16
#define TABLE_PADDING 10

static Vector2 get_text_size(const char* text, const int font_size)
{
	const float default_font_size = 10;
	int spacing = default_font_size / font_size;
	if (font_size < default_font_size) {
		spacing = 1;
	}
	return MeasureTextEx(GetFontDefault(), text, font_size, spacing);
}

static void table_mesaure_size(Table *table)
{
	Vector2 text_size = get_text_size(table->name, TABLE_NAME_FONT_SIZE);
	float max_width = text_size.x;
	float total_height = text_size.y;
	for (size_t i = 0; i < table->rows.size; i++) {
		text_size = get_text_size(table->rows.content[i], TABLE_ROW_FONT_SIZE);
		if (text_size.x > max_width) {
			max_width = text_size.x;
		}
		total_height += text_size.y + TABLE_PADDING;
	}

	table->rect.w = max_width + TABLE_PADDING;
	table->rect.h = total_height;
}

Table table_create(const HashMap *map)
{
#define fetch_assert_hash_map(_map, _kind)\
	do { \
		log_assert(idx_##_map >= 0, "Key "#_map" not found.");\
		log_assert(\
			map->items[idx_##_map].kind == _kind,\
			"Key "#_map" should be `"#_kind"`.");\
	} while(0)

	Table t = {0};
	const ssize_t idx_table_name = hash_map_key_index(map, "table_name");
	fetch_assert_hash_map(table_name, NODE_KIND_STRING);
	t.name = (char*)map->items[idx_table_name].value;

	const ssize_t idx_rows = hash_map_key_index(map, "rows");
	fetch_assert_hash_map(rows, NODE_KIND_MAP);
	const HashMap* row_map = (HashMap*)map->items[idx_rows].value;
	t.rows.size = row_map->size;
	t.rows.content = malloc(sizeof(char*) * t.rows.size);
	for (size_t i = 0; i < row_map->size ; i++) {
		const Node n = row_map->items[i];
		log_assert(
			n.kind == NODE_KIND_STRING,
			"Rows should be `NODE_KIND_STRING`.\n");
		size_t idx = i;
		t.rows.content[idx] = n.value;
	}

	const ssize_t idx_rect = hash_map_key_index(map, "rect");
	fetch_assert_hash_map(rect, NODE_KIND_MAP);
	const HashMap* rect_map = (HashMap*)map->items[idx_rect].value;
	t.rect.x = *(float*)rect_map->items[hash_map_key_index(rect_map, "x")].value;
	t.rect.y = *(float*)rect_map->items[hash_map_key_index(rect_map, "y")].value;
	ssize_t idx_width = hash_map_key_index(rect_map, "w");
	if (idx_width >= 0) {
		t.rect.w = *(float*)rect_map->items[idx_width].value;
	}
	ssize_t idx_height = hash_map_key_index(rect_map, "h");
	if (idx_height >= 0) {
		t.rect.h = *(float*)rect_map->items[idx_height].value;
	}

	table_mesaure_size(&t);

#undef fetch_assert_hash_map
	return t;
}

void table_draw(const Table table)
{
	const TableRect rect = table.rect;
	DrawRectangleLines(rect.x, rect.y, rect.w, rect.h, GREEN);

	DrawText(
		table.name,
		rect.x,
		rect.y,
		TABLE_NAME_FONT_SIZE, GREEN);

	Vector2 text_size = get_text_size(table.name, TABLE_NAME_FONT_SIZE);
	float y = rect.y + TABLE_PADDING + text_size.y;

	const float line_y = y - (y - rect.y) / 4.f;
	DrawLine(rect.x, line_y, rect.x + rect.w, line_y, GREEN);
	for (size_t i = 0; i < table.rows.size; i++) {
		const char* row = table.rows.content[i];
		text_size = get_text_size(row, TABLE_ROW_FONT_SIZE);
		DrawText(
			row,
			rect.x,
			y,
			TABLE_ROW_FONT_SIZE,
			GREEN);
		const float padding = TABLE_PADDING + text_size.y;
		y += padding;
	}
}
