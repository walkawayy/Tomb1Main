#pragma once

#include <stdint.h>

typedef struct {
    const char *key;
    const char *name;
    const char *description;
} GS_OBJECT_ENTRY;

typedef struct {
    const char *key;
    const char *value;
} GS_GAME_STRING_ENTRY;

typedef struct {
    GS_OBJECT_ENTRY *objects;
    GS_GAME_STRING_ENTRY *game_strings;
} GS_TABLE;

typedef struct {
    const char *title;
    GS_TABLE table;
} GS_LEVEL;

typedef struct {
    int32_t level_count;
    GS_TABLE global;
    GS_LEVEL *levels;
} GS_FILE;

extern GS_FILE g_GST_File;

void GS_Table_Free(GS_TABLE *gs_table);
void GS_File_Free(GS_FILE *gs_file);