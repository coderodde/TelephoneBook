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
static const size_t FORMAT_STRING_CAPACITY    = 100;

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
    
    telephone_record_book_file_path[home_directory_name_length] =
        PATH_SEPARATOR;
    
    strcpy(&telephone_record_book_file_path[home_directory_name_length + 1],
           TELEPHONE_RECORD_BOOK_FILE_NAME);
    
    return telephone_record_book_file_path;
}

static char* write_separator(char* str, char c, size_t n)
{
    memset(str, c, n);
    return str + n;
}

/*******************************************************************************
* Allocates and sets a row separator string.                                   *
*******************************************************************************/
char* load_separator_string(size_t max_last_name_token_length,
                            size_t max_first_name_token_length,
                            size_t max_telephone_number_token_length,
                            size_t max_telephone_contact_id_length)
{
    char* save_separator_string;
    
    /* The separator string is composed of 4 horizontal bars: one bar for the
     * last name, one for first name, one for phone number, and one for the ID
     * (in that order). The magic constants 1 and 2, are the additional padding
     * so that for each attribute (column) there is a space before and after the
     * column title. 'max_first_name_token_length + 2' means that the longest
     * first name (or column) title is no longer than 
     * 'max_first_name_token_length', and it will be preceded and followed by 
     * one space.
     *
     * The magic constant 4, counts 3 bars between the columns and a final 
     * '\0' terminator.
     */
    char* separator_string =
    malloc((max_last_name_token_length + 1) +
           (max_first_name_token_length + 2) +
           (max_telephone_number_token_length + 2) +
           (max_telephone_contact_id_length + 1) + 4);
    
    if (!separator_string)
    {
        return NULL;
    }
    
    save_separator_string = separator_string;
    separator_string = write_separator(separator_string,
                                       '-',
                                       max_last_name_token_length + 1);
    
    separator_string = write_separator(separator_string, '+', 1);
    separator_string = write_separator(separator_string,
                                       '-',
                                       max_first_name_token_length + 2);
    
    separator_string = write_separator(separator_string, '+', 1);
    separator_string = write_separator(separator_string,
                                       '-',
                                       max_telephone_number_token_length + 2);
    
    separator_string = write_separator(separator_string, '+', 1);
    separator_string = write_separator(separator_string,
                                       '-',
                                       max_telephone_contact_id_length + 1);
    
    write_separator(separator_string, '\0', 1);
    return save_separator_string;
}

/*******************************************************************************
* Creates and returns a structure containing all format strings necessary for  *
* printing the telephone book record list.                                     *
*******************************************************************************/
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
    
    /* The format string used to output the actual telephone book records. */
    char* record_format_string;
    
    /* The title string. */
    char* title_string;
    
    /* The separating horizontal bar. */
    char* separator_string;
    char* id_holder_string;
    
    telephone_book_record_list_node* current_node;
    telephone_book_record* current_record;
    
    if (!list)
    {
        return NULL;
    }
    
    /* ALLOCATED: output_table */
    output_table = malloc(sizeof *output_table);
    
    if (!output_table)
    {
        return NULL;
    }
    
    /* ALLOCATED: output_table, id_holder_string */
    id_holder_string = malloc(ID_HOLDER_STRING_CAPACITY);
    
    if (!id_holder_string)
    {
        free(output_table);
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
    
    /* ALLOCATED: output_table */
    free(id_holder_string);
    
    /* ALLOCATED: output_table, record_format_string */
    record_format_string = malloc(FORMAT_STRING_CAPACITY);
    
    if (!record_format_string)
    {
        free(output_table);
        return NULL;
    }
    
    sprintf(record_format_string,
            "%%-%zus | %%-%zus | %%-%zus | %%-%zuzu\n",
            max_last_name_token_length,
            max_first_name_token_length,
            max_telephone_number_token_length,
            max_telephone_contact_id_length);
    
    /* @ALLOC: output_table, record_format_string, title_string */
    title_string = malloc((max_last_name_token_length + 1) +
                          (max_first_name_token_length + 2) +
                          (max_telephone_number_token_length + 2) +
                          (max_telephone_contact_id_length + 1) + 4);
    
    if (!title_string)
    {
        free(output_table);
        free(record_format_string);
        return NULL;
    }
    
    sprintf(title_string,
            "%-*s | %-*s | %-*s | %-*s",
            (int) max_last_name_token_length,
            TITLE_LAST_NAME,
            (int) max_first_name_token_length,
            TITLE_FIRST_NAME,
            (int) max_telephone_number_token_length,
            TITLE_TELEPHONE_NUMBER,
            (int) max_telephone_contact_id_length,
            TITLE_CONTACT_ID);
    
    /* ALLOCATED: output_table, record_format_string,
                  title_string, title_string_format */
    /*title_string_format = malloc(sizeof(char) * FORMAT_STRING_CAPACITY);
    
    if (!title_string_format)
    {
        free(output_table);
        free(record_format_string);
        free(title_string);
        return NULL;
    }*/
    /*
    sprintf(title_string_format,
            "%.*s | %.*s | %.*s | %-zu",
            max_last_name_token_length,
            max_first_name_token_length,
            max_telephone_number_token_length,
            max_telephone_contact_id_length);*/
    /*
    sprintf(title_string_format,
            "%%-%zus | %%-%zus | %%-%zus | %%-%zus",
            max_last_name_token_length,
            max_first_name_token_length,
            max_telephone_number_token_length,
            max_telephone_contact_id_length);
    */
    /*
    sprintf(title_string,
            title_string_format,
            TITLE_LAST_NAME,
            TITLE_FIRST_NAME,
            TITLE_TELEPHONE_NUMBER,
            TITLE_CONTACT_ID);
    */
    /*
    sprintf(title_string,
            "%.*s | %.*s | %.*s | %.*s",
            (int) max_last_name_token_length,
            TITLE_LAST_NAME,
            (int) max_first_name_token_length,
            TITLE_FIRST_NAME,
            (int) max_telephone_number_token_length,
            TITLE_TELEPHONE_NUMBER,
            (int) max_telephone_contact_id_length,
            TITLE_CONTACT_ID);*/
    
    /* ALLOCATED: output_table, record_format_string, title_string */
    //free(title_string_format);
    
    separator_string =
        load_separator_string(max_last_name_token_length,
                              max_first_name_token_length,
                              max_telephone_number_token_length,
                              max_telephone_contact_id_length);
    
    if (!separator_string)
    {
        free(output_table);
        free(record_format_string);
        free(title_string);
        return NULL;
    }
    
    output_table->title_string = title_string;
    output_table->separator_string = separator_string;
    output_table->record_format_string = record_format_string;
    
    return output_table;
}

/*******************************************************************************
* Frees all the memory alloated by the output table format strings.            *
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
    
    free(output_table_strs);
}

/*******************************************************************************
* Returns the format string for nifty printing the removed records.            *
*******************************************************************************/
char* get_removed_record_output_format_string(telephone_book_record_list* list)
{
    size_t max_last_name_token_length        = 0;
    size_t max_first_name_token_length       = 0;
    size_t max_telephone_number_token_length = 0;
    size_t max_telephone_contact_id_length   = 0;
    
    size_t last_name_token_length;
    size_t first_name_token_length;
    size_t telephone_number_token_length;
    size_t telephone_contact_id_length;
    
    /* ALLOCATED: format_string */
    char* format_string = malloc(sizeof(char) * FORMAT_STRING_CAPACITY);
    char* id_holder_string;
    
    telephone_book_record_list_node* current_node;
    telephone_book_record* current_record;
    
    if (!format_string)
    {
        return NULL;
    }
    
    /* ALLOCATED: format_string, id_holder_string */
    id_holder_string = malloc(ID_HOLDER_STRING_CAPACITY);
    
    if (!id_holder_string)
    {
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
    
    
    
    sprintf(format_string,
            "%%-%zus %%-%zus %%-%zus %%-%zuzu\n",
            max_last_name_token_length,
            max_first_name_token_length,
            max_telephone_number_token_length,
            max_telephone_contact_id_length);
    
    /* ALLOCATED: format_string */
    free(id_holder_string);
    return format_string;
}
