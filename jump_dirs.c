#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <locale.h>
#include <wchar.h>
#include <inttypes.h>

#include "common.h"

#define PRINT_DEBUG(...) do {\
    if (debug_output) wprintf("[DEBUG] "__VA_ARGS__);\
} while(0)

typedef struct Entry {
    float rank;
    int64_t time;
    wchar_t path[512];
} Entry;

typedef struct EntryList {
    Entry *items;
    int count;
    size_t capacity;
} EntryList;

bool debug_output = false;
long max_score = 5000; // 5000 is the actual default, but will be overwritten no matter what

BOOL is_directory(wchar_t *path);
float frecent(float entry_rank, int64_t entry_time, int64_t now);
int write_datafile(wchar_t *path, EntryList *list);
int read_datafile(wchar_t *path, EntryList *list);
Entry *find_common_entry(EntryList *matches);
void print_data(EntryList *list);
int search_match(EntryList *list, wchar_t **keyword, int keywords_len);
int add_entry_to_entrylist(EntryList *list, wchar_t *path);
int remove_entry_from_entrylist(EntryList *list, wchar_t *path);
void print_help();

int wmain(int argc, wchar_t **argv) {
    setlocale(LC_ALL, "");
    setlocale(LC_NUMERIC, "en_US.UTF-8");

    if (argc < 3) {
        wprintf(L"Unsufficient arguments given! (%d instead of 3)\n", argc);
        for (int i = 0; i < argc; ++i) {
            wprintf(L"  argument %d: '%s'\n", i, argv[i]);
        }
        wprintf(L"\n\nThis jump_dirs installation is broken: try restarting the terminal.\nIf it's still broken, please open up an Issue on GitHub (GiGGioSo/jump_dirs).\n");
        exit(1);
    }

    wchar_t *datafile_path = argv[1];

    // Check validity of the max score passed as argument
    wchar_t *max_score_raw = argv[2];
    wchar_t *end = NULL;
    long max_score = wcstol(max_score_raw, &end, 10);
    if (max_score <= 0 || end == NULL || *end != L'\0') {
        wprintf(L"'%ls' is an invalid max score value!\n\nCorrect the JUMP_DIRS_MAX_SCORE variable and restart the terminal.\n", max_score_raw);
        exit(1);
    }

    if (argc == 3) {
        EntryList data = {};

        int read_result = read_datafile(datafile_path, &data);
        if (read_result) {
            wprintf(L"Could not read the 'jump_dirs' datafile %ls: %d\n",
                    datafile_path, read_result);
            exit(1);
        }

        print_data(&data);
        exit(1);
    }

    if (wcscmp(argv[3], L"-a") == 0) {
        int arg4_len = wcslen(argv[4]);
        if (argv[4][arg4_len-1] == L'"') {
            argv[4][arg4_len-1] = L'\0';
        }
        wchar_t *path_to_add = argv[4];

        EntryList data = {};

        int read_result = read_datafile(datafile_path, &data);
        if (read_result) {
            wprintf(L"Could not read the 'jump_dirs' datafile %ls: %d\n",
                    datafile_path, read_result);
            exit(1);
        }

        if (is_directory(path_to_add)) {
            add_entry_to_entrylist(&data, path_to_add);
        }

        int write_result = write_datafile(datafile_path, &data);
        if (read_result) {
            wprintf(
                L"Could not update the 'jump_dirs' datafile %ls: %d\n",
                datafile_path, write_result);
            exit(1);
        }
        exit(1);
    } else if (wcscmp(argv[3], L"-h") == 0 || wcscmp(argv[3], L"--help") == 0) {
        print_help();
        exit(1);
    } else if (wcscmp(argv[3], L"-x") == 0) {
        int arg4_len = wcslen(argv[4]);
        if (argv[4][arg4_len-1] == L'"') {
            argv[4][arg4_len-1] = L'\0';
        }
        wchar_t *path_to_remove = argv[4];

        EntryList data = {};

        int read_result = read_datafile(datafile_path, &data);
        if (read_result) {
            wprintf(L"Could not read the 'jump_dirs' datafile %ls: %d\n",
                    datafile_path, read_result);
            exit(1);
        }

        int remove_ret = remove_entry_from_entrylist(&data, path_to_remove);
        if (remove_ret != 0) {
            print_data(&data);
        }

        int write_result = write_datafile(datafile_path, &data);
        if (read_result) {
            wprintf(
                L"Could not update the 'jump_dirs' datafile %ls: %d\n",
                datafile_path, write_result);
            exit(1);
        }

        // Exit with error no matter what because we just print the match
        exit(1);
    } else if (wcscmp(argv[3], L"-e") == 0) {
        int keywords_len = argc - 4;
        wchar_t **keywords = argv + 4;

        EntryList data = {};

        int read_result = read_datafile(datafile_path, &data);
        if (read_result) {
            wprintf(L"Could not read the 'jump_dirs' datafile %ls: %d\n",
                    datafile_path, read_result);
            exit(1);
        }

        int search_ret = search_match(&data, keywords, keywords_len);

        // Exit with error no matter what because we just print the match
        exit(1);
    } else if (wcscmp(argv[3], L"-l") == 0) {
        EntryList data = {};

        int read_result = read_datafile(datafile_path, &data);
        if (read_result) {
            wprintf(L"Could not read the 'jump_dirs' datafile %ls: %d\n",
                    datafile_path, read_result);
            exit(1);
        }
        print_data(&data);
        exit(1);
    } else {
        /*
         * exe data score D E F G
         *
         * 0   1    2     3 4 5 6
         *
         * length = 7
         *
         * len(argv[n:]) = length - n
         *
         */

        int keywords_len = argc - 3;
        wchar_t **keywords = argv + 3;

        EntryList data = {};

        read_datafile(datafile_path, &data);

        int search_ret = search_match(&data, keywords, keywords_len);
        if (search_ret != 0) {
            // print_data(&data);
        } else {
            int write_result = write_datafile(datafile_path, &data);
            if (write_result) {
                wprintf(
                    L"Could not update the 'jump_dirs' datafile %ls: %d\n",
                    datafile_path, write_result);
                exit(1);
            }
        }
        exit(search_ret);
    }

    exit(0);
}

void print_help() {
    wprintf(
        L"\n z - jump around your most frecent directories!\n"
        L"\n"
        L" Usage:\n"
        L"   z [option] [search items]\n"
        L"\n"
        L" Options:\n"
        L"   -c <item1> <item2> ...   Restrict matches to subdirectories of the current directory\n"
        L"   -e <item1> <item2> ...   Print the best match without changing the datafile\n"
        L"   -a <path>                Add directory to the datafile, but do not cd\n"
        L"   -x <path>                Remove the path from the datafile\n"
        L"   -l                       Print the datafile contents with their relative frecencies\n"
        L"   -h / --help              Print this help message\n"
    );
}

bool starts_with(wchar_t *str, wchar_t *start) {
    int start_len = wcslen(start);

    if (wcslen(str) < start_len) return false;

    return (wcsncmp(str, start, start_len) == 0);
}

Entry *find_common_entry(EntryList *matches) {
    Entry *common = NULL;
    for (int match_index = 0; match_index < matches->count; ++match_index) {
        Entry *match = &matches->items[match_index];

        if (common == NULL || wcslen(match->path) < wcslen(common->path)) {
            common = match;
        }
    }

    for (int match_index = 0; match_index < matches->count; ++match_index) {
        Entry *match = &matches->items[match_index];

        if (!starts_with(match->path, common->path)) return NULL;
    }

    return common;
}

int search_match(EntryList *list, wchar_t **keywords, int keywords_len) {
    int64_t now = (int64_t) time(NULL);

    if (keywords_len == 0) return 1;
    if (!keywords) return 1;

    Entry *best_match = NULL;
    float best_rank = 0.f;

    EntryList matches = {};

    for (int entry_index = 0; entry_index < list->count; ++entry_index) {
        Entry *entry = &list->items[entry_index];
        float rank = frecent(entry->rank, entry->time, now);

        wchar_t *search_base = entry->path;

        bool is_match = true;
        for(int keyword_index = 0;
            keyword_index < keywords_len;
            ++keyword_index) {
            wchar_t *keyword = keywords[keyword_index];

            if ((search_base = wcsstr(search_base, keyword)) != NULL) {
                search_base += wcslen(keyword);
            } else {
                is_match = false;
                break;
            }
        }

        if (is_match) {
            Entry new_match = { .rank = rank, .time = now };
            da_append(&matches, new_match);
            wcsncpy(da_last(&matches).path, entry->path,
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

        wprintf(L"%ls\n", best_match->path);
        return 0;
    } else {
        return 1;
    }
}

void print_data(EntryList *list) {
    int64_t now = (int64_t) time(NULL);

    // Sort the list in descending order based on frecency
    for (int entry_index = 1; entry_index < list->count; ++entry_index) {
        int i = entry_index;
        Entry *entry_to_sort = &list->items[i];
        float rank_to_sort = frecent(entry_to_sort->rank,
                                     entry_to_sort->time, now);
        Entry *entry_to_compare = &list->items[i-1];
        float rank_to_compare = frecent(entry_to_compare->rank,
                                        entry_to_compare->time, now);
        while(i > 0 && rank_to_sort < rank_to_compare) {
            da_swap(list, i, i-1, Entry);
            i--;
            entry_to_sort = &list->items[i];
            rank_to_sort = frecent(entry_to_sort->rank,
                                   entry_to_sort->time, now);
            entry_to_compare = &list->items[i-1];
            rank_to_compare = frecent(entry_to_compare->rank,
                                      entry_to_compare->time, now);
        }
    }

    wprintf(L"\n-------- ENTRY LIST --------\n");
    for (int entry_index = 0; entry_index < list->count; ++entry_index) {
        Entry entry = list->items[entry_index];
        wprintf(L"| %10.1f | %ls\n",
                frecent(entry.rank, entry.time, now),
                entry.path);
    }
    wprintf(L"----------------------------\n");
}

int add_entry_to_entrylist(EntryList *list, wchar_t* path) {
    int64_t now = (int64_t) time(NULL);

    bool entry_was_present = false;

    float score_count = 0.f;

    for (int entry_index = 0; entry_index < list->count; ++entry_index) {
        Entry *entry = &list->items[entry_index];
        if (wcscmp(path, entry->path) == 0) {
            entry_was_present = true;
            entry->rank += 1.f;
            entry->time = now;
        }

        score_count += entry->rank;
    }

    if (!entry_was_present) {
        Entry new_entry = { .rank = 1, .time = now };
        da_append(list, new_entry);
        wcsncpy(da_last(list).path, path,
                ARRAY_LENGTH(da_last(list).path)-1);
    }

    // Aging if the sum of all of the ranks is greater than max_score
    if (score_count > max_score) {
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

int remove_entry_from_entrylist(EntryList *list, wchar_t *path) {
    for (int entry_index = 0; entry_index < list->count; ++entry_index) {
        Entry *entry = &list->items[entry_index];

        if (wcscmp(entry->path, path) == 0) {
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

int write_datafile(wchar_t *path, EntryList *list) {
    FILE *datafile = NULL;
    int result = 0;

    {
        datafile = _wfopen(path, L"w,ccs=UNICODE");
        if (datafile == NULL) {
            PRINT_DEBUG(L"ERROR: Could not open to write file %ls\n", path);
            return_defer(1);
        }

        for (int entry_index = 0; entry_index < list->count; ++entry_index) {

            Entry entry = list->items[entry_index];

            fwprintf(datafile, L"%ls|%.1f|%lld\n",
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

int read_datafile(wchar_t *path, EntryList *list) {
    FILE *datafile = NULL;
    int result = 0;

    {
        datafile = _wfopen(path, L"r,ccs=UNICODE");
        if (datafile == NULL) {
            PRINT_DEBUG(L"ERROR: Could not open to read file %ls\n", path);
            return_defer(1);
        }

        wchar_t new_path[512] = {};
        float new_rank = 42;
        int64_t new_time = 42;

        int count = 0;

        while (fwscanf(datafile, L" %511l[^|]|%f|%lld",
               new_path, &new_rank, &new_time) == 3) {
            count++;
            PRINT_DEBUG(L"Read new entry: path=%ls, rank=%f, time=%lld\n",
                        new_path, new_rank, new_time);

            Entry new_entry = { .rank = new_rank, .time = new_time };
            da_append(list, new_entry);
            wcsncpy(da_last(list).path, new_path,
                    ARRAY_LENGTH(da_last(list).path)-1);
        }
        PRINT_DEBUG(L"Read %d entries in total!\n", count);

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

BOOL is_directory(wchar_t *path) {
    DWORD dwAttrib = GetFileAttributesW(path);

    return (dwAttrib != INVALID_FILE_ATTRIBUTES && 
           (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}
