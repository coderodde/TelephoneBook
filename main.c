#include "telephone_book.h"
#include "telephone_book_io.h"
#include "telephone_book_utils.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERROR "[ERROR] "
#define WARNING "[WARNING] "
#define INFO "[INFO] "

static const char* OPTION_ADD_SHORT = "-a";
static const char* OPTION_ADD_LONG  = "--add";

static const char* OPTION_REMOVE_SHORT = "-r";
static const char* OPTION_REMOVE_LONG  = "--remove";

static const char* OPTION_HELP_SHORT = "-h";
static const char* OPTION_HELP_LONG  = "--help";

static const size_t RECORDS_PER_BLOCK = 3;

/*******************************************************************************
* Returns the smallest of 'a', 'b', and 'c'.                                   *
*******************************************************************************/
static size_t min3(size_t a, size_t b, size_t c)
{
    if (a < b)
    {
        if (c < a)
        {
            return c;
        }
        
        return a;
    }
    
    /* b <= a */
    if (c < b)
    {
        return c;
    }
    
    return b;
}

/*******************************************************************************
* Prints the help message to the standard output.                              *
*******************************************************************************/
static void print_help(char* executable_name)
{
    char path_separator = PATH_SEPARATOR;
    int executable_name_length = (int) strlen(executable_name);
    int i;
    
    /* Doing my own basename for the sake of portability. */
    for (i = executable_name_length - 1; i >= 0; --i)
    {
        if (executable_name[i] == path_separator)
        {
            executable_name = &executable_name[i + 1];
            break;
        }
    }
    
    printf("Usage: %s -a    LAST FIRST NUMBER\n", executable_name);
    printf("       %s --add LAST FIRST NUMBER\n", executable_name);
    puts("");
    printf("       %s -r       ID1 ID2 ... IDn\n", executable_name);
    printf("       %s --remove ID1 ID2 ... IDn\n", executable_name);
    puts("");
    printf("(1)    %s\n",                      executable_name);
    printf("(2)    %s LAST_EXPR\n",            executable_name);
    printf("(3)    %s - FIRST_EXPR\n",         executable_name);
    printf("(4)    %s LAST_EXPR FIRST_EXPR\n", executable_name);
    puts("");
    puts("Where: -a or --add for adding one new book entry.");
    puts("       -r or --remove for removing book entries by their IDs.");
    puts("");
    puts("(1) List all book entries in order.");
    puts("(2) Match by last name and list the closest book entries.");
    puts("(3) Match by first name and list the closest book entries.");
    puts("(4) Match by both last and first names and list the closest book "
         "entries");
}

/*******************************************************************************
* Implements the Levenshtein distance algorithm.                               *
*******************************************************************************/
static size_t edit_distance(char* word1,
                            char* word2,
                            size_t length1,
                            size_t length2)
{
    int cost;
    
    if (length1 == 0)
    {
        return length2;
    }
    
    if (length2 == 0)
    {
        return length1;
    }
    
    cost = tolower(word1[length1 - 1]) ==
           tolower(word2[length2 - 1]) ? 0 : 1;
    
    return min3(edit_distance(word1, word2, length1, length2 - 1) + 1,
                edit_distance(word1, word2, length1 - 1, length2) + 1,
                edit_distance(word1, word2, length1 - 1, length2 - 1) + cost);
}

/*******************************************************************************
* Computes the Levenshtein disance between words 'word1' and 'word2'.          *
*******************************************************************************/
static size_t compute_edit_distance(char* word1, char* word2)
{
    return edit_distance(word1, word2, strlen(word1), strlen(word2));
}

/*******************************************************************************
* Implements listing the telephone book records.                               *
*******************************************************************************/
int command_list_telephone_book_records_impl(
                        telephone_book_record_list* record_list,
                        char* last_name,
                        char* first_name)
{
    size_t best_tentative_distance = 1000 * 1000 * 1000;
    size_t temp_distance;
    size_t i;
    output_table_strings* output_strings;
    telephone_book_record* record;
    telephone_book_record_list_node* current_node;
    telephone_book_record_list* best_record_list =
        telephone_book_record_list_alloc();
    
    /* ALLOCATED: best_record_list */
    if (!best_record_list)
    {
        fputs(ERROR "Cannot allocate the best record list.\n", stderr);
        return EXIT_FAILURE;
    }
    
    current_node = record_list->head;
    
    while (current_node)
    {
        temp_distance =
        (last_name ?
            compute_edit_distance(last_name,
                                  current_node->record->last_name) : 0) +
        (first_name ?
            compute_edit_distance(first_name,
                                  current_node->record->first_name) : 0);
        
        if (best_tentative_distance > temp_distance)
        {
            /* 'temp_distance' improves the best known edit distance,     */
            /* clear the current best list and append the current record: */
            telephone_book_record_list_free(best_record_list);
            best_record_list = telephone_book_record_list_alloc();
            
            if (!best_record_list)
            {
                fputs(ERROR "Cannot allocate new best record list.\n", stderr);
                return EXIT_FAILURE;
            }
            
            record = telephone_book_record_alloc(
                            current_node->record->last_name,
                            current_node->record->first_name,
                            current_node->record->telephone_number,
                            current_node->record->id);
            
            if (!record)
            {
                fputs(ERROR "Cannot allocate a copy record.\n", stderr);
                telephone_book_record_list_free(best_record_list);
                return EXIT_FAILURE;
            }
            
            if (telephone_book_record_list_add_record(best_record_list, record))
            {
                fputs(ERROR "Cannot add a new record to the best list.\n",
                      stderr);
                telephone_book_record_list_free(best_record_list);
                telephone_book_record_free(record);
                return EXIT_FAILURE;
            }
            
            best_tentative_distance = temp_distance;
        }
        else if (best_tentative_distance == temp_distance)
        {
            /* Append the current record to the best list: */
            record = telephone_book_record_alloc(
                            current_node->record->last_name,
                            current_node->record->first_name,
                            current_node->record->telephone_number,
                            current_node->record->id);
            
            if (!record)
            {
                fputs(ERROR "Cannot allocate a copy record.\n", stderr);
                telephone_book_record_list_free(best_record_list);
                return EXIT_FAILURE;
            }
            
            
            if (telephone_book_record_list_add_record(best_record_list, record))
            {
                fputs(ERROR "Cannot add a new record to the best list.\n",
                      stderr);
                telephone_book_record_list_free(best_record_list);
                telephone_book_record_free(record);
                return EXIT_FAILURE;
            }
        }
        
        current_node = current_node->next;
    }
    
    output_strings = output_table_strings_create(best_record_list);
    
    if (!output_strings)
    {
        telephone_book_record_list_free(best_record_list);
        return EXIT_FAILURE;
    }
    
    puts(output_strings->separator_string);
    puts(output_strings->title_string);
    puts(output_strings->separator_string);
    
    current_node = best_record_list->head;
    i = 0;
    
    while (current_node)
    {
        printf(output_strings->record_format_string,
               current_node->record->last_name,
               current_node->record->first_name,
               current_node->record->telephone_number,
               current_node->record->id);
        
        current_node = current_node->next;
        ++i;
        
        if (current_node && i % RECORDS_PER_BLOCK == 0)
        {
            puts(output_strings->separator_string);
        }
    }
    
    output_table_strings_free(output_strings);
    telephone_book_record_list_free(best_record_list);
    return EXIT_SUCCESS;
}

/*******************************************************************************
* Handles the command for listing the records.                                 *
*******************************************************************************/
static int command_list_telephone_book_records(int argc, char* argv[])
{
    char* file_name;
    FILE* f;
    telephone_book_record_list* record_list;
    char* last_name;
    char* first_name;
    
    /* ALLOCATED: file_name */
    file_name = get_telephone_record_book_file_path();
    
    if (!file_name)
    {
        fputs(ERROR
              "Cannot allocate memory for the telephone book file name.\n",
              stderr);
        return EXIT_FAILURE;
    }
    
    f = fopen(get_telephone_record_book_file_path(), "r");
    
    if (!f)
    {
        fprintf(stderr,
                ERROR "Cannot open the record book file '%s'.\n",
                file_name);
        
        free(file_name);
        return EXIT_FAILURE;
    }
    
    /* ALLOCATED: file_name, record_list */
    record_list = telephone_book_record_list_read_from_file(f);
    fclose(f);
    
    if (!record_list)
    {
        fputs(ERROR "Cannot read the record book file.\n", stderr);
        free(file_name);
        return EXIT_FAILURE;
    }
    
    /* If fails, silently ignore: */
    telephone_book_record_list_sort(record_list);
    /* Does not ask for resources, should be OK: */
    telephone_book_record_list_fix_ids(record_list);
    
    /* "w" means overwrite the file. */
    f = fopen(file_name, "w");
    
    if (f)
    {
        /* Write the file back. It will update the order of the records and */
        /* fix the record IDs, if needed. */
        telephone_book_record_list_write_to_file(record_list, f);
        fclose(f);
    }
    
    last_name  = argc >= 2 ? argv[1] : NULL;
    first_name = argc >= 3 ? argv[2] : NULL;
    
    if (argc > 1 && strcmp(argv[1], "-") == 0)
    {
        /* Match all last names: */
        last_name = NULL;
    }
    
    /* ALLOCATED: record_list */
    free(file_name);
    
    return command_list_telephone_book_records_impl(record_list,
                                                    last_name,
                                                    first_name);
}

/*******************************************************************************
* Handles the command for adding a new record.                                 *
*******************************************************************************/
static int command_add_record(int argc, char* argv[])
{
    char* file_name;
    FILE* f;
    telephone_book_record_list* record_list;
    telephone_book_record* record;
    
    if (argc != 5)
    {
        print_help(argv[0]);
        return EXIT_FAILURE;
    }
    
    /* ALLOCATED: file_name */
    file_name = get_telephone_record_book_file_path();
    
    if (!file_name)
    {
        fputs(ERROR
              "Cannot allocate memory for the telephone book file name.\n",
              stderr);
        return EXIT_FAILURE;
    }
    
    f = fopen(file_name, "r");
    
    if (!f)
    {
        fprintf(stderr, ERROR "Cannot open the record book file '%s'.\n",
                file_name);
        
        free(file_name);
        return EXIT_FAILURE;
    }
    
    /* ALLOCATED: file_name, record_list */
    record_list = telephone_book_record_list_read_from_file(f);
    fclose(f);
    
    if (!record_list)
    {
        fputs(ERROR "Cannot read the record book file.\n", stderr);
        free(file_name);
        return EXIT_FAILURE;
    }
    
    /* ALLOCATED: file_name, record_list, record */
    record = telephone_book_record_alloc(argv[2], argv[3], argv[4], -1);
    
    if (!record)
    {
        fputs(ERROR "Cannot allocate memory for the new record.\n", stderr);
        free(file_name);
        telephone_book_record_list_free(record_list);
        return EXIT_FAILURE;
    }
    
    if (telephone_book_record_list_add_record(record_list, record))
    {
        fputs(ERROR "Cannot add the new entry to the record book.\n", stderr);
        free(file_name);
        telephone_book_record_list_free(record_list);
        telephone_book_record_free(record);
        return EXIT_FAILURE;
    }
    
    /* Even if the following two functions fail, proceed further. */
    telephone_book_record_list_sort(record_list);
    telephone_book_record_list_fix_ids(record_list);
    
    f = fopen(file_name, "w");
    free(file_name);
    
    if (!f)
    {
        fputs(ERROR "Cannot open the record book file.\n", stderr);
        /* 'record' is contained in 'record_list' so is freed by it: */
        telephone_book_record_list_free(record_list);
        telephone_book_record_free(record);
        return EXIT_FAILURE;
    }
    
    if (telephone_book_record_list_write_to_file(record_list, f))
    {
        fputs(ERROR "Cannot update the record book file.\n", stderr);
    }
    
    fclose(f);
    /* 'record' is contained in 'record_list' so is freed by it: */
    telephone_book_record_list_free(record_list);
    return EXIT_SUCCESS;
}

/*******************************************************************************
* Handles the commmand for removing records by their IDs.                      *
*******************************************************************************/
static int command_remove_records(int argc, char* argv[])
{
    char* file_name;
    FILE* f;
    telephone_book_record_list* record_list;
    telephone_book_record_list* removed_record_list;
    telephone_book_record* removed_record;
    telephone_book_record_list_node* current_node;
    char* removed_record_format;
    int arg_index;
    int id;
    
    if (argc < 3)
    {
        puts(WARNING "No record IDs given. Nothing to remove.");
        return EXIT_SUCCESS;
    }
    
    /* ALLOCATED: file_name */
    file_name = get_telephone_record_book_file_path();
    
    if (!file_name)
    {
        fputs(ERROR "Cannot allocate memory for the telephone book file name.",
              stderr);
        return EXIT_FAILURE;
    }
    
    f = fopen(file_name, "r");
    
    if (!f)
    {
        fprintf(stderr,
                ERROR "Cannot open the record book file '%s'.\n",
                file_name);
        
        free(file_name);
        return EXIT_FAILURE;
    }
    
    /* ALLOCATED: file_name, record_list */
    record_list = telephone_book_record_list_read_from_file(f);
    fclose(f);
    
    if (!record_list)
    {
        fputs(ERROR "Cannot read the record book file.", stderr);
        free(file_name);
        return EXIT_FAILURE;
    }
    
    /* ALLOCATED: file_name, record_list, removed_record_list */
    removed_record_list = telephone_book_record_list_alloc();
    
    if (!removed_record_list)
    {
        fputs(ERROR "Cannot allocate memory for the list of removed records.",
              stderr);
        free(file_name);
        telephone_book_record_list_free(record_list);
        return EXIT_FAILURE;
    }
    
    /* Even if the following two functions fail, proceed further. */
    telephone_book_record_list_sort(record_list);
    telephone_book_record_list_fix_ids(record_list);
    
    f = fopen(file_name, "w");
    
    /* ALLOCATED: record_list, removed_record_list */
    free(file_name); /* We do not need 'file_name' anymore. */
    
    if (!f)
    {
        fputs(ERROR "Cannot open the record book file for writing.", stderr);
        telephone_book_record_list_free(record_list);
        telephone_book_record_list_free(removed_record_list);
        return EXIT_FAILURE;
    }
    
    for (arg_index = 2; arg_index < argc; ++arg_index)
    {
        if (sscanf(argv[arg_index], "%d", &id) != 1)
        {
            printf(WARNING "Bad ID = \'%s\'. Ignored.\n", argv[arg_index]);
            continue;
        }
        
        removed_record = telephone_book_record_list_remove_entry(record_list,
                                                                 id);
        
        if (removed_record)
        {
            telephone_book_record_list_add_record(removed_record_list,
                                                  removed_record);
        }
    }
    
    if (telephone_book_record_list_write_to_file(record_list, f))
    {
        fputs(ERROR "Cannot update the record book file.", stderr);
        telephone_book_record_list_free(record_list);
        telephone_book_record_list_free(removed_record_list);
        fclose(f);
        return EXIT_FAILURE;
    }
    
    fclose(f);
    
    printf(INFO "Number of records to remove: %d, removed: %d.\n",
           argc - 2,
           telephone_book_record_list_size(removed_record_list));
    
    if (telephone_book_record_list_size(removed_record_list) == 0)
    {
        puts(INFO "Nothing to remove.");
        telephone_book_record_list_free(removed_record_list);
        telephone_book_record_list_free(record_list);
        return EXIT_SUCCESS;
    }
    
    puts(INFO "List of removed entries:");
    current_node = removed_record_list->head;
    removed_record_format =
        get_removed_record_output_format_string(removed_record_list);
    
    while (current_node)
    {
        if (removed_record_format)
        {
            printf(removed_record_format,
                   current_node->record->last_name,
                   current_node->record->first_name,
                   current_node->record->telephone_number,
                   current_node->record->id);
        }
        else
        {
            /* Fallback format output: */
            printf("%s, %s - %s, ID %d\n",
                   current_node->record->last_name,
                   current_node->record->first_name,
                   current_node->record->telephone_number,
                   current_node->record->id);
        }
        
        current_node = current_node->next;
    }
    
    free(removed_record_format);
    telephone_book_record_list_free(record_list);
    telephone_book_record_list_free(removed_record_list);
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
    if (argc == 1)
    {
        return command_list_telephone_book_records(argc, argv);
    }
    
    if (strcmp(argv[1], OPTION_HELP_SHORT) == 0 ||
        strcmp(argv[1], OPTION_HELP_LONG) == 0)
    {
        print_help(argv[0]);
        return EXIT_SUCCESS;
    }
    
    if (strcmp(argv[1], OPTION_ADD_SHORT) == 0 ||
        strcmp(argv[1], OPTION_ADD_LONG) == 0)
    {
        return command_add_record(argc, argv);
    }
    
    if (strcmp(argv[1], OPTION_REMOVE_SHORT) == 0 ||
        strcmp(argv[1], OPTION_REMOVE_LONG) == 0)
    {
        return command_remove_records(argc, argv);
    }
    
    return command_list_telephone_book_records(argc, argv);
}
