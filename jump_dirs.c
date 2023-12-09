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
 *  -a      don't return a match, just build up the datafile
 *  -x      remove directory from the datafile
 *  -h      print help message
 *
 */

#define PRINT_DEBUG(...) do {\
    if (debug_output) printf("[DEBUG] "__VA_ARGS__);\
} while(0)

#define MAX_SCORE 5000.f

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
Entry *find_common_entry(EntryList *matches);
void print_data(EntryList *list);
int search_match(EntryList *list, char *keyword);
int add_entry_to_entrylist(EntryList *list, char* path);
int remove_entry_from_entrylist(EntryList *list, char *path);
bool match(char *str, char *keyword);
void print_help();

int main(int argc, char **argv) {
    char *datafile_path = argv[1];

    if (argc <= 2) {
        EntryList data = {};

        read_datafile(datafile_path, &data);
        print_data(&data);
        exit(1);
    }

    if (strcmp(argv[2], "-a") == 0) {
        char *path_to_add = argv[3];

        EntryList data = {};

        read_datafile(datafile_path, &data);
        if (is_directory(path_to_add)) {
            add_entry_to_entrylist(&data, path_to_add);
        }
        write_datafile(datafile_path, &data);
        exit(1);
    } else if (strcmp(argv[2], "-h") == 0 || strcmp(argv[2], "--help") == 0) {
        print_help();
        exit(1);
    } else if (strcmp(argv[2], "-x") == 0) {
        char *path_to_remove = argv[3];

        EntryList data = {};

        read_datafile(datafile_path, &data);

        int remove_ret = remove_entry_from_entrylist(&data, path_to_remove);
        if (remove_ret != 0) {
            print_data(&data);
        }

        write_datafile(datafile_path, &data);

        exit(1);
    } else if (strcmp(argv[2], "-e") == 0) {
        char *keyword = argv[3];

        EntryList data = {};

        read_datafile(datafile_path, &data);

        int search_ret = search_match(&data, keyword);

        // Exit with error no matter what because we just print the match
        exit(1);
    } else if (strcmp(argv[2], "-l") == 0) {
        EntryList data = {};

        read_datafile(datafile_path, &data);
        print_data(&data);
        exit(1);
    } else {
        char *keyword = argv[2];

        EntryList data = {};

        read_datafile(datafile_path, &data);

        int search_ret = search_match(&data, keyword);
        if (search_ret != 0) {
            print_data(&data);
        }

        write_datafile(datafile_path, &data);
        exit(search_ret);
    }

    exit(0);
}

void print_help() {
    printf(
"\n z - jump around your most frecent directories!\n"
"\n"
" Usage:\n"
"   z [option] [search]\n"
"\n"
" Options:\n"
"   -e <search>     Print the best match without changing the datafile\n"
"   -a <path>       Add directory to the datafile, but do not cd\n"
"   -l              Print the datafile contents\n"
"   -x <path>       Remove the path from the datafile\n"
"   -h / --help     Print this help message\n"
);
}

bool starts_with(char *str, char *start) {
    int start_len = strlen(start);

    if (strlen(str) < start_len) return false;

    return (strncmp(str, start, start_len) == 0);
}

Entry *find_common_entry(EntryList *matches) {
    Entry *common = NULL;
    for (int match_index = 0; match_index < matches->count; ++match_index) {
        Entry *match = &matches->items[match_index];

        if (common == NULL || strlen(match->path) < strlen(common->path)) {
            common = match;
        }
    }

    for (int match_index = 0; match_index < matches->count; ++match_index) {
        Entry *match = &matches->items[match_index];

        if (!starts_with(match->path, common->path)) return NULL;
    }

    return common;
}

int search_match(EntryList *list, char *keyword) {
    int64_t now = (int64_t) time(NULL);

    Entry *best_match = NULL;
    float best_rank = 0.f;

    EntryList matches = {};

    for (int entry_index = 0; entry_index < list->count; ++entry_index) {
        Entry *entry = &list->items[entry_index];
        float rank = frecent(entry->rank, entry->time, now);

        if (match(entry->path, keyword)) {
            Entry new_match = { .rank = rank, .time = now };
            da_append(&matches, new_match);
            strncpy(da_last(&matches).path, entry->path,
                    ARRAY_LENGTH(da_last(&matches).path)-1);
            if (best_match == NULL || rank > best_rank) {
                best_match = entry;
                best_rank = rank;
            }
        }
    }

    // Found something
    if (best_match) {
        Entry *common_entry = find_common_entry(&matches);
        if (common_entry) best_match = common_entry;

        add_entry_to_entrylist(list, best_match->path);

        printf("%s\n", best_match->path);
        return 0;
    } else {
        return 1;
    }
}

bool match(char *str, char *keyword) {
    return (strstr(str, keyword) != NULL);
}

void print_data(EntryList *list) {
    printf("\n-------- ENTRY LIST --------\n");
    for (int entry_index = 0; entry_index < list->count; ++entry_index) {
        Entry entry = list->items[entry_index];
        printf("| %6.1f | %s\n", entry.rank, entry.path);
    }
    printf("----------------------------\n");
}

int add_entry_to_entrylist(EntryList *list, char* path) {
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
                ARRAY_LENGTH(da_last(list).path)-1);
    }

    // Aging if the sum of all of the ranks is greater than MAX_SCORE
    if (score_count > MAX_SCORE) {
        for (int entry_index = 0; entry_index < list->count; ++entry_index) {
            Entry *entry = &list->items[entry_index];
            entry->rank = entry->rank * 0.99f;
            if (entry->rank < 1.f) {
                da_remove(list, entry_index);
            }
        }
    }
    return 0;
}

int remove_entry_from_entrylist(EntryList *list, char *path) {
    for (int entry_index = 0; entry_index < list->count; ++entry_index) {
        Entry *entry = &list->items[entry_index];

        if (strcmp(entry->path, path) == 0) {
            da_remove(list, entry_index);
            return 0;
        }
    }
    return 1;
}

float frecent(float entry_rank, int64_t entry_time, int64_t now) {
    int64_t dt = now - entry_time;
    return 10000.f * entry_rank * (3.75f/((0.0001f * (float)dt + 1.f) + 0.25f));
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
                    ARRAY_LENGTH(da_last(list).path)-1);
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
