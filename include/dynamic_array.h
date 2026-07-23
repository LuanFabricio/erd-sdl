#pragma once

#include <stdlib.h>

#define da_create(type)\
	struct {\
		type *items;\
		size_t size, capacity;\
	}

#define da_append(arr, x)\
	do {\
		if ((arr)->items == NULL || (arr)->capacity <= 0){\
			free((arr)->items);\
			(arr)->size = 0;\
			(arr)->capacity = 5;\
			(arr)->items = malloc(sizeof(x) * (arr)->capacity);\
		} else if (((arr)->size + 1) >= (arr)->capacity) {\
				(arr)->capacity *= 2;\
				(arr)->items = realloc((arr)->items, sizeof(*(arr)->items) * (arr)->capacity);\
		}\
		(arr)->items[(arr)->size] = x;\
		(arr)->size += 1;\
	} while(0)

#define da_remove(arr, i)\
	do {\
		size_t size = (arr)->size;\
		if (i < size) {\
			if (size > 0 ) {\
				size--;\
				(arr)->size = size;\
			}\
			(arr)->items[i] = (arr)->items[size];\
		}\
	} while(0)

#define da_for(arr, counter) for (size_t counter = 0; counter < (arr)->size; counter++)

#define da_for_each(arr, type)\
	for (\
		struct { size_t i; type* item; } loop = { 0, &(arr)->items[0] };\
		loop.i < (arr)->size;\
		loop.i++, loop.item = &(arr)->items[loop.i]\
	)
