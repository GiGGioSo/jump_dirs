#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "common.h"

/*
 *
 * FLAGS:
 *  -e      echo the best match, don't cd and don't change frecencies
 *  -c      restrict matches to subdirectories of the current directory
 *  -a      don't return a match, just build up the datafile
 *  -x      remove the match from the datafile
 *
 */

// function frecent(rank, time) {
//     # relate frequency and time
//     dx = t - time
//         return int(10000 * rank * (3.75/((0.0001 * dx + 1) + 0.25)))
// }

#define PRINT_DEBUG(...) do {\
    if (debug_output) printf("[DEBUG] "__VA_ARGS__);\
} while(0)

#define MAX_SCORE 500.f

typedef struct Entry {
    float rank;
    int64_t time;
    char path[512];
} Entry;

typedef struct EntryList {
    Entry *items;
    int count;
    size_t capacity;
} EntryList;

int debug_output = 0;

BOOL is_directory(LPCSTR path);
float frecent(float entry_rank, int64_t entry_time, int64_t now);
int write_datafile(char *path, EntryList *list);
int read_datafile(char *path, EntryList *list);
void print_data(EntryList *list);

int add_entry(char *path, EntryList *list);

int main(int argc, char **argv) {
    if (argc <= 1) {
        printf("No arguments!\n");
        exit(1);
    }

    char *datafile_path = argv[1];

    if (strcmp(argv[2], "-a") == 0) {

        char *path_to_add = argv[3];

        EntryList data = {};

        read_datafile(datafile_path, &data);
        print_data(&data);
        if (is_directory(path_to_add)) {
            add_entry(path_to_add, &data);
            print_data(&data);
        }
        write_datafile(datafile_path, &data);

    } else {
    }

    exit(0);
}

void print_data(EntryList *list) {
    printf("\n-------- ENTRY LIST --------\n");
    for (int entry_index = 0; entry_index < list->count; ++entry_index) {
        Entry entry = list->items[entry_index];
        printf("%.2f | %s\n", entry.rank, entry.path);
    }
    printf("----------------------------\n");
}

int add_entry(char *path, EntryList *list) {
    int64_t now = (int64_t) time(NULL);

    bool entry_was_present = false;

    float score_count = 0.f;

    for (int entry_index = 0; entry_index < list->count; ++entry_index) {
        Entry *entry = &list->items[entry_index];
        if (strcmp(path, entry->path) == 0) {
            entry_was_present = true;
            entry->rank += 1.f;
            entry->time = now;
        }

        score_count += entry->rank;
    }

    if (!entry_was_present) {
        Entry new_entry = { .rank = 1, .time = now };
        da_append(list, new_entry);
        strncpy(da_last(list).path, path,
                ARRAY_LENGTH(da_last(list).path));
    }

    // Aging if the sum of all of the ranks is greater than MAX_SCORE
    if (score_count > MAX_SCORE) {
        for (int entry_index = 0; entry_index < list->count; ++entry_index) {
            Entry *entry = &list->items[entry_index];
            entry->rank = entry->rank * 0.95f;
        }
    }
}

float frecent(float entry_rank, int64_t entry_time, int64_t now) {
    int64_t dt = entry_time - now;
    return 10000.f * entry_rank * (3.75f/((0.0001f * (float)dt + 1.f) + 0.25));
}

int write_datafile(char *path, EntryList *list) {
    FILE *datafile = NULL;
    int result = 0;

    {
        datafile = fopen(path, "wb");
        if (datafile == NULL) {
            PRINT_DEBUG("ERROR: Could not open to write file %s\n", path);
            return_defer(1);
        }

        for (int entry_index = 0; entry_index < list->count; ++entry_index) {

            Entry entry = list->items[entry_index];

            fprintf(datafile, "%s|%f|%lld\n",
                    entry.path, entry.rank, entry.time);

            if (ferror(datafile)) return_defer(2);
        }
    }

    defer:
    if (datafile) {
        fclose(datafile);
        datafile = NULL;
    }
    return result;
}

int read_datafile(char *path, EntryList *list) {
    FILE *datafile = NULL;
    int result = 0;

    {
        datafile = fopen(path, "rb");
        if (datafile == NULL) {
            PRINT_DEBUG("ERROR: Could not open to read file %s\n", path);
            return_defer(1);
        }

        char new_path[512] = {};
        float new_rank;
        int64_t new_time;

        int count = 0;

        while (fscanf(datafile, " %511[^|]|%f|%lld",
               new_path, &new_rank, &new_time) == 3) {
            count++;
            PRINT_DEBUG("Read new entry: path=%s, rank=%f, time=%lld\n",
                        new_path, new_rank, new_time);

            Entry new_entry = { .rank = new_rank, .time = new_time };
            da_append(list, new_entry);
            strncpy(da_last(list).path, new_path,
                    ARRAY_LENGTH(da_last(list).path));
        }
        PRINT_DEBUG("Read %d entries in total!\n", count);

        if (ferror(datafile)) return_defer(2);
    }

    defer:
    if (datafile) {
        fclose(datafile);
        datafile = NULL;
    }
    if (result != 0) {
        da_clear(list);
    }
    return result;
}

BOOL is_directory(LPCSTR path) {
    DWORD dwAttrib = GetFileAttributesA(path);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
           (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
