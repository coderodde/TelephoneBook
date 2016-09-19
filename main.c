#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "telephone_record.h"

// By 'token' we mean first names, last names and phone numbers.
static const size_t MAXIMUM_TEXT_TOKEN_LENGTH = 2;
static const char *const TELEPHONE_BOOK_FILE_NAME = ".telephone_book";

static void print_help(char* executable_path)
{
    printf("Usage: %s [--add FIRST LAST NUMBER | --remove FIRST LAST]\n",
           basename(executable_path));
    puts("Where:");
    puts("    --add FIRST LAST NUMBER: add new entry.");
    puts("    --remove FIRST LAST: remove the entry.");
    puts("If neither '--add' or '--remove' is supplied, lists all entries.");
}

static char* get_telephone_book_file_path()
{
    char* home_directory = getenv("HOME");
    unsigned long home_directory_length = strlen(home_directory);
    // Below, + 2 is for the path separator and the terminating zero char:
    char* telephone_book_file_path =
        malloc(sizeof(char) * (home_directory_length +
                               2 + strlen(TELEPHONE_BOOK_FILE_NAME)));
    strcpy(telephone_book_file_path, home_directory);
    telephone_book_file_path[home_directory_length] = '/';
    strcpy(&telephone_book_file_path[home_directory_length + 1],
           TELEPHONE_BOOK_FILE_NAME);
    return telephone_book_file_path;
}

static int count_entries(FILE* f)
{
    if (!f)
    {
        return -1;
    }
 
    rewind(f);
    int number_of_lines = 0;
    char* dummy_line = NULL;
    size_t dummy_len;
    
    while (getline(&dummy_line, &dummy_len, f) != -1)
    {
        number_of_lines++;
    }
    
    return number_of_lines;
}

static int get_maximum_id(FILE* f)
{
    if (!f)
    {
        return -1;
    }
    
    int maximum_id = 0;
    int id;
    
    while (!feof(f))
    {
        if (fscanf(f, "%*s %*s %*s %d\n", &id) < 0)
        {
            return -1;
        }
        
        if (maximum_id < id)
        {
            maximum_id = id;
        }
    }
    
    return maximum_id;
}

static int list_all_entries()
{
    FILE* f = fopen(get_telephone_book_file_path(), "r");
    
    telephone_record_list* list = telephone_record_list_read_from_file(f);
    puts("read");
    telephone_record_list_print(list);
    telephone_record_list_free(list);
/*
    char* line = NULL;
    size_t len;
    
    char first_name[MAXIMUM_TEXT_TOKEN_LENGTH + 1];
    char last_name [MAXIMUM_TEXT_TOKEN_LENGTH + 1];
    char phone_num [MAXIMUM_TEXT_TOKEN_LENGTH + 1];
    int id;
    
    size_t maximum_first_name_length   = 0;
    size_t maximum_last_name_length    = 0;
    size_t maximum_phone_number_length = 0;
    size_t maximum_record_id_length    = 0;
    
    while (getline(&line, &len, f) != -1)
    {
        
    }
    
    int number_of_lines = count_entries(f);
    fclose(f);
    printf("Lines: %d\n", number_of_lines);*/
    return EXIT_SUCCESS;
}

static int add_entry(int argc, char* argv[])
{
    if (argc < 5 || !argv[2] || !argv[3] || !argv[4])
    {
        return EXIT_FAILURE;
    }
    
    if (strlen(argv[2]) > MAXIMUM_TEXT_TOKEN_LENGTH)
    {
        fprintf(stderr, "The first name \"%s\" is too long!\n", argv[2]);
        return EXIT_FAILURE;
    }
    
    if (strlen(argv[3]) > MAXIMUM_TEXT_TOKEN_LENGTH)
    {
        fprintf(stderr, "The last name \"%s\" is too long!\n", argv[3]);
        return EXIT_FAILURE;
    }
    
    if (strlen(argv[4]) > MAXIMUM_TEXT_TOKEN_LENGTH)
    {
        fprintf(stderr, "The phone number \"%s\" is too long!\n", argv[4]);
        return EXIT_FAILURE;
    }
    
    telephone_record new_record;
    //telephone_record_init(&new_record, argv[2], argv[3], argv[4]);
    char* file_name = get_telephone_book_file_path();
    FILE* f = fopen(file_name, "r");
    
    if (!f)
    {
        fprintf(stderr, "Error: could not open \"%s\".\n", file_name);
        return EXIT_FAILURE;
    }
    
    int number_of_entries = count_entries(f);
    int new_record_id;
    
    if (number_of_entries == 0)
    {
        new_record_id = 0;
    }
    else
    {
        fclose(f);
        f = fopen(file_name, "r");
        
        if (!f)
        {
            return EXIT_FAILURE;
        }
        
        int maximum_id = get_maximum_id(f);
        new_record_id = maximum_id + 1;
        fclose(f);
    }
    
    f = fopen(file_name, "a");
    
    if (!f)
    {
        return EXIT_FAILURE;
    }
    
    int number_of_characters = fprintf(f,
                                       "%s %s %s %d\n",
                                       new_record.first_name,
                                       new_record.last_name,
                                       new_record.telephone_number,
                                       new_record_id);
    fclose(f);
    
    return number_of_characters < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}

static int remove_entry(int argc, char* argv[])
{
    puts("remove");
    return EXIT_SUCCESS;
}

static bool file_exists(char* file_name)
{
    struct stat dummy;
    return (stat(file_name, &dummy)) == 0;
}

int main(int argc, char * argv[]) {
    
    if (argc == 1)
    {
        return list_all_entries();
    }
    /*
    if (strcmp("--add", argv[1]) == 0)
    {
        return add_entry(argc, argv);
    }
    
    if (strcmp("--remove", argv[1]) == 0)
    {
        return remove_entry(argc, argv);
    }
    */
    print_help(argv[0]);
    return EXIT_SUCCESS;
}
