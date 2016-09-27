#ifndef TELEPHONE_BOOK_H
#define TELEPHONE_BOOK_H

#include <stdio.h>

/*******************************************************************************
 * This structure holds a single telephone book record.                         *
 *******************************************************************************/
typedef struct {
    char* first_name;
    char* last_name;
    char* telephone_number;
    int         id;
} telephone_book_record;

/*******************************************************************************
 * This structure holds a doubly-linked list of telephone book records.         *
 *******************************************************************************/
typedef struct {
    struct telephone_book_record_list_node* head;
    struct telephone_book_record_list_node* tail;
    int size;
} telephone_book_record_list;




/*******************************************************************************
 * Allocates and initializes a new telephone book record.                       *
 * ---                                                                          *
 * Returns a new telephone book record or NULL if something goes wrong.         *
 *******************************************************************************/
telephone_book_record* telephone_book_record_alloc(const char* last_name,
                                                   const char* first_name,
                                                   const char* phone_number,
                                                   int id);

/*******************************************************************************
 * Frees the memory occupied by the telephone book record: all existing fields *
 * and the actual record.                                                      *
 *******************************************************************************/
void telephone_book_record_free(telephone_book_record* record);




/*******************************************************************************
 * Allocates and initializes an empty telephone book record list.               *
 * ---                                                                          *
 * Returns a new empty telephone book record list or NULL if something goes     *
 * wrong.                                                                       *
 *******************************************************************************/
telephone_book_record_list* telephone_book_record_list_alloc();

/*******************************************************************************
 * Appends the argument telephone book record to the tail of the argument       *
 * telephone book record list.                                                  *
 * ---                                                                          *
 * Returns a zero value if the operation was successfull. A non-zero value is   *
 * returned is something fails.                                                 *
 *******************************************************************************/
int telephone_book_record_list_add_record(telephone_book_record_list* list,
                                          telephone_book_record* record);

/*******************************************************************************
 * Removes a telephone book record that has 'id' as its record ID.              *
 * ---                                                                          *
 * Returns a zero value if the appropriate entry was not found, 1 if the entry  *
 * was found and removed, and -1 on error.                                      *
 *******************************************************************************/
int telephone_book_record_list_remove_entry(telephone_book_record_list* list,
                                            int id);

/*******************************************************************************
 * Reconstructs the telephone book record list from a file pointed to by the    *
 * argument file handle.                                                        *
 * ---                                                                          *
 * Returns the record list on success, and NULL on failure.                     *
 *******************************************************************************/
telephone_book_record_list* telephone_book_record_list_read_from_file(FILE* f);

/*******************************************************************************
 * Writes the entire contents of the telephone record list to a specified file  *
 * handle.                                                                      *
 * ---                                                                          *
 * Returns zero on success, and a non-zero value if something fails.            *
 *******************************************************************************/
int telephone_book_record_list_write_to_file(telephone_book_record_list* list,
                                             FILE* f);

/*******************************************************************************
 * Sorts the telephone records. The last name of each record is the primary     *
 * sorting key, and the first name of each record is the secondary sorting key. *
 * ---                                                                          *
 * Returns zero on success, and a non-zero value if the sorting could not be    *
 * completed.                                                                   *
 *******************************************************************************/
int telephone_book_record_list_sort(telephone_book_record_list* list);

/*******************************************************************************
 * Makes sure that each telephone book record has an unique ID.                 *
 * ---                                                                          *
 * Returns zero on success, and a non-zero value if something fails.            *
 *******************************************************************************/
int telephone_book_record_list_fix_ids(telephone_book_record_list* list);

/*******************************************************************************
 * Frees all the memory occupied by the argument telephone book record list.    *
 *******************************************************************************/
void telephone_book_record_list_free(telephone_book_record_list* list);


#endif /* TELEPHONE_BOOK_H */
