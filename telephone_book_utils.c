#include "telephone_book_utils.h"
#include "telephone_book.h"
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

const char* TELEPHONE_RECORD_BOOK_FILE_NAME = ".telephone_book";

static const char* TITLE_LAST_NAME          = "Last name";
static const char* TITLE_FIRST_NAME         = "First name";
static const char* TITLE_TELEPHONE_NUMBER   = "Telephone number";
static const char* TITLE_CONTACT_ID         = "ID";

static const size_t ID_HOLDER_STRING_CAPACITY = 40;
static const size_t FORMAT_STRING_CAPACITY = 100;

/*******************************************************************************
* Returns the character representing a path separator character ('/' or '\')   *
* used on the current platform.                                                *
*******************************************************************************/
char separator()
{
#ifdef _WIN32
    return '\\';
#else
    return '/';
#endif
}

/*******************************************************************************
* Returns a C string representing the full path to the telephone book record   *
* file.                                                                        *
*******************************************************************************/
char* get_telephone_record_book_file_path()
{
    char* home_directory;
    char* telephone_record_book_file_path;
    size_t home_directory_name_length;
    
    home_directory = getenv("HOME");
    
    if (!home_directory)
    {
        return NULL;
    }
    
    home_directory_name_length = strlen(home_directory);
    
    /* Allocate the space for the entire */
    telephone_record_book_file_path =
    malloc(sizeof(char)
           * (home_directory_name_length
              + 2 + strlen(TELEPHONE_RECORD_BOOK_FILE_NAME)));
    
    if (!telephone_record_book_file_path)
    {
        return NULL;
    }
    
    strcpy(telephone_record_book_file_path, home_directory);
    telephone_record_book_file_path[home_directory_name_length] = separator();
    strcpy(&telephone_record_book_file_path[home_directory_name_length + 1],
           TELEPHONE_RECORD_BOOK_FILE_NAME);
    
    return telephone_record_book_file_path;
}



/*******************************************************************************
 * Allocates and sets a row separator string.                                   *
 *******************************************************************************/
char* load_separator_string(size_t max_last_name_token_length,
                            size_t max_first_name_token_length,
                            size_t max_telephone_number_token_length,
                            size_t max_telephone_contact_id_length)
{
    size_t index;
    size_t i;
    char* separator_string =
    malloc(sizeof(char) *
           (max_last_name_token_length + 1) +
           (max_first_name_token_length + 2) +
           (max_telephone_number_token_length + 2) +
           (max_telephone_contact_id_length + 1) + 4);
    
    for (i = 0; i != max_last_name_token_length + 1; ++i)
    {
        separator_string[i] = '-';
    }
    
    separator_string[max_last_name_token_length + 1] = '+';
    
    index = max_last_name_token_length + 2;
    
    for (i = 0; i != max_first_name_token_length + 2; ++i)
    {
        separator_string[index++] = '-';
    }
    
    separator_string[index++] = '+';
    
    for (i = 0; i != max_telephone_number_token_length + 2; ++i)
    {
        separator_string[index++] = '-';
    }
    
    separator_string[index++] = '+';
    
    for (i = 0; i != max_telephone_contact_id_length + 1; ++i)
    {
        separator_string[index++] = '-';
    }
    
    separator_string[index++] = '\0';
    return separator_string;
}

output_table_strings*
output_table_strings_create(telephone_book_record_list* list)
{
    size_t max_last_name_token_length        = strlen(TITLE_LAST_NAME);
    size_t max_first_name_token_length       = strlen(TITLE_FIRST_NAME);
    size_t max_telephone_number_token_length = strlen(TITLE_TELEPHONE_NUMBER);
    size_t max_telephone_contact_id_length   = strlen(TITLE_CONTACT_ID);
    
    size_t last_name_token_length;
    size_t first_name_token_length;
    size_t telephone_number_token_length;
    size_t telephone_contact_id_length;
    
    output_table_strings* output_table;
    
    char* record_format_string;
    char* remove_record_format_string;
    char* title_string;
    char* separator_string;
    char* id_holder_string;
    char* title_string_format;
    
    telephone_book_record_list_node* current_node;
    telephone_book_record* current_record;
    
    output_table = malloc(sizeof *output_table);
    
    if (!output_table)
    {
        fputs("Cannot allocate memory for the output format structures.",
              stderr);
        return NULL;
    }
    
    id_holder_string = malloc(ID_HOLDER_STRING_CAPACITY);
    
    if (!id_holder_string)
    {
        fputs("Cannot allocate memory for the ID field holder.", stderr);
        return NULL;
    }
    
    current_node = list->head;
    
    while (current_node)
    {
        current_record = current_node->record;
        
        last_name_token_length  = strlen(current_record->last_name);
        first_name_token_length = strlen(current_record->first_name);
        telephone_number_token_length =
        strlen(current_record->telephone_number);
        
        sprintf(id_holder_string, "%d", current_record->id);
        telephone_contact_id_length = strlen(id_holder_string);
        
        max_last_name_token_length = MAX(max_last_name_token_length,
                                         last_name_token_length);
        
        max_first_name_token_length = MAX(max_first_name_token_length,
                                          first_name_token_length);
        
        max_telephone_number_token_length =
        MAX(max_telephone_number_token_length,
            telephone_number_token_length);
        
        max_telephone_contact_id_length = MAX(max_telephone_contact_id_length,
                                              telephone_contact_id_length);
        
        current_node = current_node->next;
    }
    
    record_format_string = malloc(sizeof(char) * FORMAT_STRING_CAPACITY);
    
    if (!record_format_string)
    {
        fputs("Cannot allocate memory for the format string.", stderr);
        return NULL;
    }
    
    sprintf(record_format_string,
            "%%-%zus | %%-%zus | %%-%zus | %%-%zuzu\n",
            max_last_name_token_length,
            max_first_name_token_length,
            max_telephone_number_token_length,
            max_telephone_contact_id_length);
    
    remove_record_format_string = malloc(sizeof(char) * FORMAT_STRING_CAPACITY);
    
    if (!remove_record_format_string)
    {
        fputs("Cannot allocate memory for the remove format string.", stderr);
        return NULL;
    }
    
    sprintf(remove_record_format_string,
            "%%-%zus %%-%zus %%-%zus %%-%zuzu\n",
            max_last_name_token_length,
            max_first_name_token_length,
            max_telephone_number_token_length,
            max_telephone_contact_id_length);
    
    free(id_holder_string);
    
    title_string =
    malloc(sizeof(char) *
           (max_last_name_token_length + 1) +
           (max_first_name_token_length + 2) +
           (max_telephone_number_token_length + 2) +
           (max_telephone_contact_id_length + 1) + 5);
    
    title_string_format = malloc(sizeof(char) * FORMAT_STRING_CAPACITY);
    
    if (!title_string_format)
    {
        fputs("Cannot allocate memory for the title string format.", stderr);
        exit(EXIT_FAILURE);
    }
    
    sprintf(title_string_format,
            "%%-%zus | %%-%zus | %%-%zus | %%-%zus",
            max_last_name_token_length,
            max_first_name_token_length,
            max_telephone_number_token_length,
            max_telephone_contact_id_length);
    
    sprintf(title_string,
            title_string_format,
            TITLE_LAST_NAME,
            TITLE_FIRST_NAME,
            TITLE_TELEPHONE_NUMBER,
            TITLE_CONTACT_ID);
    
    separator_string =
    malloc(sizeof(char) *
           (max_last_name_token_length + 1) +
           (max_first_name_token_length + 2) +
           (max_telephone_number_token_length + 2) +
           (max_telephone_contact_id_length + 1) +
           5);
    
    separator_string =
    load_separator_string(max_last_name_token_length,
                          max_first_name_token_length,
                          max_telephone_number_token_length,
                          max_telephone_contact_id_length);
    
    output_table->title_string = title_string;
    output_table->separator_string = separator_string;
    output_table->record_format_string = record_format_string;
    output_table->remove_record_format_string = remove_record_format_string;
    
    return output_table;
}

/*******************************************************************************
* Creates and returns a structure containing all format strings necessary for  *
* printing the telephone book record list.                                     *
*******************************************************************************/
void output_table_strings_free(output_table_strings* output_table_strs)
{
    if (!output_table_strs)
    {
        return;
    }
    
    if (output_table_strs->record_format_string)
    {
        free(output_table_strs->record_format_string);
    }
    
    if (output_table_strs->separator_string)
    {
        free(output_table_strs->separator_string);
    }
    
    if (output_table_strs->title_string)
    {
        free(output_table_strs->title_string);
    }
    
    if (output_table_strs->remove_record_format_string)
    {
        free(output_table_strs->remove_record_format_string);
    }
    
    free(output_table_strs);
}
