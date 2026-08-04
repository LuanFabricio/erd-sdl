#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "table.h"
#include "hash_map.h"
#include "log.h"

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
	t.rect.w = *(float*)rect_map->items[hash_map_key_index(rect_map, "w")].value;
	t.rect.h = *(float*)rect_map->items[hash_map_key_index(rect_map, "h")].value;

#undef fetch_assert_hash_map
	return t;
}
