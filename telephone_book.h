#ifndef TELEPHONE_BOOK_H
#define TELEPHONE_BOOK_H

/*******************************************************************************
* This structure holds a single telephone book record.                         *
*******************************************************************************/
typedef struct {
    char* first_name;
    char* last_name;
    char* telephone_number;
    int id;
} telephone_book_record;

/*******************************************************************************
* This structure defines a linked list node for the telephone book record      *
* list.                                                                        *
*******************************************************************************/
typedef struct telephone_book_record_list_node {
    telephone_book_record* record;
    struct telephone_book_record_list_node* next;
} telephone_book_record_list_node;

/*******************************************************************************
* This structure holds a doubly-linked list of telephone book records.         *
*******************************************************************************/
typedef struct {
    struct telephone_book_record_list_node* head;
    struct telephone_book_record_list_node* tail;
    int size;
} telephone_book_record_list;




/*******************************************************************************
* Returns the length of the telephone book record list.                        *
*******************************************************************************/
int telephone_book_record_list_size(telephone_book_record_list* list);

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
* Frees the memory occupied by the telephone book record: all existing fields  *
* and the actual record.                                                       *
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
* returned if something fails.                                                 *
*******************************************************************************/
int telephone_book_record_list_add_record(telephone_book_record_list* list,
                                          telephone_book_record* record);

/*******************************************************************************
* Removes and returns the telephone book record that has 'id' as its record ID.*
* ---                                                                          *
* Returns NULL if something fails or the list does not contain record with ID  *
* 'id'. Otherwise, a removed record is returned.                               *
*******************************************************************************/
telephone_book_record*
telephone_book_record_list_remove_entry(telephone_book_record_list* list,
                                        int id);

/*******************************************************************************
* Sorts the telephone records. The last name of each record is the primary     *
* sorting key, and the first name of each record is the secondary sorting key. *
* ---                                                                          *
* Returns zero on success, and a non-zero value if the sorting could not be    *
* completed.                                                                   *
*******************************************************************************/
int telephone_book_record_list_sort(telephone_book_record_list* list);

/*******************************************************************************
* Makes sure that each telephone book record has a unique ID.                  *
* ---                                                                          *
* Returns zero on success, and a non-zero value if something fails.            *
*******************************************************************************/
int telephone_book_record_list_fix_ids(telephone_book_record_list* list);

/*******************************************************************************
* Frees all the memory occupied by the argument telephone book record list.    *
*******************************************************************************/
void telephone_book_record_list_free(telephone_book_record_list* list);

#endif /* TELEPHONE_BOOK_H */
