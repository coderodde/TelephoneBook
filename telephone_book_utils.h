#ifndef TELEPHONE_BOOK_UTILS_H
#define TELEPHONE_BOOK_UTILS_H

#include <stdio.h>
#include "telephone_book.h"

#ifdef _WIN32
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

/*******************************************************************************
* This structures holds the string required for neat result output.            *
*******************************************************************************/
typedef struct {
    char* title_string;
    char* separator_string;
    char* record_format_string;
} output_table_strings;




/*******************************************************************************
* Returns a C string representing the full path to the telephone book record   *
* file.                                                                        *
*******************************************************************************/
char* get_telephone_record_book_file_path();

/*******************************************************************************
* Creates and returns all format strings for printing the record list.         *
*******************************************************************************/
output_table_strings*
output_table_strings_create(telephone_book_record_list* list);

/*******************************************************************************
* Creates and returns a structure containing all format strings necessary for  *
* printing the telephone book record list.                                     *
*******************************************************************************/
void output_table_strings_free(output_table_strings* output_table_strs);

/*******************************************************************************
* Returns the format string for nifty printing the removed records.            *
*******************************************************************************/
char* get_removed_record_output_format_string(telephone_book_record_list* list);

#endif /* TELEPHONE_BOOK_UTILS_H */
