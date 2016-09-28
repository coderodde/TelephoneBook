#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "telephone_book.h"
#include "telephone_book_utils.h"

static const char* OPTION_ADD_SHORT = "-a";
static const char* OPTION_ADD_LONG  = "--add";

static const char* OPTION_REMOVE_SHORT = "-r";
static const char* OPTION_REMOVE_LONG  = "--remove";

static const size_t RECORDS_PER_BLOCK = 3;

static void print_help(char* executable_name)
{
    char path_separator = separator();
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
    printf("(3)    %s * FIRST_EXPR\n",         executable_name);
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

static int command_list_telephone_book_records()
{
    char* file_name;
    FILE* f;
    telephone_book_record_list* record_list;
    telephone_book_record_list_node* current_node;
    output_table_strings* output_strings;
    int error_status;
    size_t i;
    
    file_name = get_telephone_record_book_file_path();
    
    if (!file_name)
    {
        fputs("ERROR: Cannot allocate memory for the telephone book file name.",
              stderr);
        return EXIT_FAILURE;
    }
    
    f = fopen(get_telephone_record_book_file_path(), "r");
    
    if (!f)
    {
        fprintf(stderr,
                "ERROR: Cannot open the record book file '%s'.\n",
                file_name);
        
        free(file_name);
        return EXIT_FAILURE;
    }
    
    record_list = telephone_book_record_list_read_from_file(f);
    fclose(f);
    
    if (!record_list)
    {
        fputs("ERROR: Cannot read the record book file.", stderr);
        free(file_name);
        return EXIT_FAILURE;
    }
    
    error_status = telephone_book_record_list_sort(record_list);
    
    if (error_status)
    {
        fputs("ERROR: Cannot sort the entries.", stderr);
        free(file_name);
        telephone_book_record_list_free(record_list);
        return EXIT_FAILURE;
    }
    
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
    
    current_node = record_list->head;
    output_strings = output_table_strings_create(record_list);
    
    if (!output_strings)
    {
        fputs("ERROR: Cannot allocate output table format data.", stderr);
        free(file_name);
        telephone_book_record_list_free(record_list);
        return EXIT_FAILURE;
    }
    
    puts(output_strings->separator_string);
    puts(output_strings->title_string);
    puts(output_strings->separator_string);
    
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
    
    free(file_name);
    telephone_book_record_list_free(record_list);
    output_table_strings_free(output_strings);
    return EXIT_SUCCESS;
}

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
    
    file_name = get_telephone_record_book_file_path();
    
    if (!file_name)
    {
        fputs("ERROR: Cannot allocate memory for the telephone book file name.",
              stderr);
        return EXIT_FAILURE;
    }
    
    f = fopen(file_name, "r");
    
    if (!f)
    {
        fprintf(stderr,
                "ERROR: Cannot open the record book file '%s'.\n",
                file_name);
        
        free(file_name);
        return EXIT_FAILURE;
    }
    
    record_list = telephone_book_record_list_read_from_file(f);
    fclose(f);
    
    if (!record_list)
    {
        fputs("ERROR: Cannot read the record book file.", stderr);
        free(file_name);
        return EXIT_FAILURE;
    }
    
    record = telephone_book_record_alloc(argv[2], argv[3], argv[4], -1);
    
    if (!record)
    {
        fputs("ERROR: Cannot allocate memory for the new record.", stderr);
        free(file_name);
        telephone_book_record_list_free(record_list);
        return EXIT_FAILURE;
    }
    
    if (telephone_book_record_list_add_record(record_list, record))
    {
        fputs("ERROR: Cannot add the new entry to the record book.", stderr);
        free(file_name);
        telephone_book_record_list_free(record_list);
        return EXIT_FAILURE;
    }
    
    /* Even if the following two functions fail, proceed further. */
    telephone_book_record_list_sort(record_list);
    telephone_book_record_list_fix_ids(record_list);
    
    f = fopen(file_name, "w");
    free(file_name);
    
    if (!f)
    {
        fputs("ERROR: Cannot open the record book file.", stderr);
        free(file_name);
        telephone_book_record_list_free(record_list);
        return EXIT_FAILURE;
    }
    
    if (telephone_book_record_list_write_to_file(record_list, f))
    {
        fputs("ERROR: Cannot update the record book file.", stderr);
    }
    
    fclose(f);
    telephone_book_record_list_free(record_list);
    return EXIT_SUCCESS;
}

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
        puts("Warning: no record IDs given. Nothing to remove.");
        return EXIT_SUCCESS;
    }
    
    file_name = get_telephone_record_book_file_path();
    
    if (!file_name)
    {
        fputs("ERROR: Cannot allocate memory for the telephone book file name.",
              stderr);
        return EXIT_FAILURE;
    }
    
    f = fopen(file_name, "r");
    
    if (!f)
    {
        fprintf(stderr,
                "ERROR: Cannot open the record book file '%s'.\n",
                file_name);
        
        free(file_name);
        return EXIT_FAILURE;
    }
    
    record_list = telephone_book_record_list_read_from_file(f);
    fclose(f);
    
    if (!record_list)
    {
        fputs("ERROR: Cannot read the record book file.", stderr);
        free(file_name);
        return EXIT_FAILURE;
    }
    
    removed_record_list = telephone_book_record_list_alloc();
    
    if (!removed_record_list)
    {
        fputs("ERROR: Cannot allocate memory for the list of removed records.",
              stderr);
        free(file_name);
        telephone_book_record_list_free(record_list);
        return EXIT_FAILURE;
    }
    
    /* Even if the following two functions fail, proceed further. */
    telephone_book_record_list_sort(record_list);
    telephone_book_record_list_fix_ids(record_list);
    
    f = fopen(file_name, "w");
    free(file_name); /* We do not need 'file_name' anymore. */
    
    if (!f)
    {
        fputs("ERROR: Cannot open the record book file for writing.", stderr);
        telephone_book_record_list_free(record_list);
        telephone_book_record_list_free(removed_record_list);
        return EXIT_FAILURE;
    }
    
    for (arg_index = 2; arg_index < argc; ++arg_index)
    {
        sscanf(argv[arg_index], "%d", &id);
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
        fputs("ERROR: Cannot update the record book file.", stderr);
        telephone_book_record_list_free(record_list);
        telephone_book_record_list_free(removed_record_list);
        fclose(f);
        return EXIT_FAILURE;
    }
    
    fclose(f);
    
    printf("Number of records to remove: %d, removed: %d.\n",
           argc - 2,
           telephone_book_record_list_size(removed_record_list));
    puts("List of removed entries:");
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
    
    if (removed_record_format)
    {
        free(removed_record_format);
    }
    
    telephone_book_record_list_free(record_list);
    telephone_book_record_list_free(removed_record_list);
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
    if (argc == 1)
    {
        return command_list_telephone_book_records();
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
    
    print_help(argv[0]);
}
