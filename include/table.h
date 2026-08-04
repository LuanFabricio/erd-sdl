#pragma once

#include "stddef.h"

#include "hash_map.h"

typedef struct {
	float x, y, w, h;
} TableRect;

typedef struct {
	TableRect rect;
	char* name;
	struct {
		char** content;
		size_t size;
	} rows;
} Table;


Table table_create(const HashMap *map);
