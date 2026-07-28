#pragma once

#include <stdio.h>

#include "hash_map.h"

HashMap parse_from_filename(const char* filename);
HashMap parse_from_file(FILE* file);
