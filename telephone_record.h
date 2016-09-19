#ifndef TELEPHONE_RECORD_H
#define TELEPHONE_RECORD_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int   id;
    char* first_name;
    char* last_name;
    char* telephone_number;
    bool  is_valid;
} telephone_record;

extern struct telephone_record_list_node;

typedef struct {
    size_t size;
    size_t bad_record_count;
    struct telephone_record_list_node* head;
    struct telephone_record_list_node* tail;
} telephone_record_list;

/*******************************************************************************
* Reads the entire phone number list from the file 'f'.                        *
*******************************************************************************/
telephone_record_list* telephone_record_list_read_from_file(FILE* f);

/*******************************************************************************
* Prints in a nifty format the contents of the telephone book.                 *
*******************************************************************************/
bool telephone_record_list_print(telephone_record_list* list);

/*******************************************************************************
* Appends a record to the record list.                                         *
*******************************************************************************/
bool telephone_record_list_add_record(telephone_record_list* list,
                                      telephone_record* record);

/*******************************************************************************
* Removes the first record with the ID equal to 'id'.                          *
*******************************************************************************/
bool telephone_record_list_remove_record_by_id(telephone_record_list* list,
                                               int id);

/*******************************************************************************
* Writes the entire record list to the file 'f'.                               *
*******************************************************************************/
bool telephone_record_list_write_to_file(telephone_record_list* list,
                                         FILE* f);

/*******************************************************************************
 * Releases all the memory occupied by the record list.                        *
 *******************************************************************************/
void telephone_record_list_free(telephone_record_list* list);

#endif /* TELEPHONE_BOOK_H */
