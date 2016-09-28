#include "telephone_book.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MAX_RECORD_TOKEN_LENGTH_STRING "64"
#define MAX_RECORD_TOKEN_LENGTH 65

/*******************************************************************************
* Returns the length of the telephone book record list.                        *
*******************************************************************************/
int telephone_book_record_list_size(telephone_book_record_list* list)
{
    return list ? list->size : -1;
}

static telephone_book_record_list_node*
telephone_book_record_list_node_alloc(telephone_book_record* record)
{
    telephone_book_record_list_node* node;
    
    if (!record)
    {
        return NULL;
    }
    
    node = malloc(sizeof *node);
    
    if (!node)
    {
        return NULL;
    }
    
    node->record = record;
    node->next = NULL;
    node->prev = NULL;
    
    return node;
}

/*******************************************************************************
* Allocates and initializes a new telephone book record.                       *
* ---                                                                          *
* Returns a new telephone book record or NULL if something goes wrong.         *
*******************************************************************************/
telephone_book_record* telephone_book_record_alloc(const char* last_name,
                                                   const char* first_name,
                                                   const char* phone_number,
                                                   int id)
{
    telephone_book_record* record = malloc(sizeof *record);
    
    if (!record)
    {
        return NULL;
    }
    
    record->last_name        = malloc(sizeof(char) * (strlen(last_name) + 1));
    record->first_name       = malloc(sizeof(char) * (strlen(first_name) + 1));
    record->telephone_number = malloc(sizeof(char) *
                                      (strlen(phone_number) + 1));
    record->id = id;
    
    strcpy(record->last_name, last_name);
    strcpy(record->first_name, first_name);
    strcpy(record->telephone_number, phone_number);
    
    return record;
}

/*******************************************************************************
* Frees the memory occupied by the telephone book record: all existing fields  *
* and the actual record.                                                       *
*******************************************************************************/
void telephone_book_record_free(telephone_book_record* record)
{
    if (!record)
    {
        return;
    }
    
    if (record->first_name)
    {
        free(record->first_name);
    }
    
    if (record->last_name)
    {
        free(record->last_name);
    }
    
    if (record->telephone_number)
    {
        free(record->telephone_number);
    }
    
    free(record);
}

/*******************************************************************************
* Allocates and initializes an empty telephone book record list.               *
* ---                                                                          *
* Returns a new empty telephone book record list or NULL if something goes     *
* wrong.                                                                       *
*******************************************************************************/
telephone_book_record_list* telephone_book_record_list_alloc()
{
    telephone_book_record_list* record_list = malloc(sizeof *record_list);
    
    if (!record_list)
    {
        return NULL;
    }
    
    record_list->head = NULL;
    record_list->tail = NULL;
    record_list->size = 0;
    return record_list;
}

/*******************************************************************************
* Appends the argument telephone book record to the tail of the argument       *
* telephone book record list.                                                  *
* ---                                                                          *
* Returns a zero value if the operation was successfull. A non-zero value is   *
* returned if something fails.                                                 *
*******************************************************************************/
int telephone_book_record_list_add_record(telephone_book_record_list* list,
                                          telephone_book_record* record)
{
    telephone_book_record_list_node* new_node;
    
    if (!list || !record)
    {
        return 1;
    }
    
    new_node = telephone_book_record_list_node_alloc(record);
    
    if (!new_node)
    {
        return 1;
    }
    
    if (list->head)
    {
        new_node->prev = list->tail;
        list->tail->next = new_node;
    }
    else
    {
        list->head = new_node;
    }
    
    list->tail = new_node;
    list->size++;
    return 0;
}

/*******************************************************************************
* Removes a telephone book record that has 'id' as its record ID.              *
* ---                                                                          *
* On error or ID mismatch returns NULL. Otherwise the record with the same ID  *
* (which is removed) is returned.                                              *
*******************************************************************************/
telephone_book_record*
telephone_book_record_list_remove_entry(telephone_book_record_list* list,
                                        int id)
{
    telephone_book_record_list_node* current_node;
    telephone_book_record_list_node* next_node;
    telephone_book_record* removed_record;
    
    if (!list)
    {
        return NULL;
    }
    
    current_node = list->head;
    
    while (current_node)
    {
        next_node = current_node->next;
        
        /* Since the program makes sure that all IDs are unique, we can */
        /* return as soon as we remove the first match. */
        if (current_node->record->id == id)
        {
            if (current_node->prev)
            {
                current_node->prev->next = current_node->next;
            }
            else
            {
                list->head = current_node->next;
            }
            
            if (current_node->next)
            {
                current_node->next->prev = current_node->prev;
            }
            else
            {
                list->tail = current_node->prev;
            }
            
            removed_record = current_node->record;
            free(current_node);
            return removed_record;
        }
        
        current_node = next_node;
    }
    
    return NULL;
}

/*******************************************************************************
* Reconstructs the telephone book record list from a file pointed to by the    *
* argument file handle.                                                        *
* ---                                                                          *
* Returns the record list on success, and NULL on failure.                     *
*******************************************************************************/
telephone_book_record_list* telephone_book_record_list_read_from_file(FILE* f)
{
    telephone_book_record_list* record_list;
    telephone_book_record* current_record;
    
    char last_name_token   [MAX_RECORD_TOKEN_LENGTH];
    char first_name_token  [MAX_RECORD_TOKEN_LENGTH];
    char phone_number_token[MAX_RECORD_TOKEN_LENGTH];
    int  id_holder;
    int read_result;
    
    if (!f)
    {
        return NULL;
    }
    
    record_list = telephone_book_record_list_alloc();
    
    if (!record_list)
    {
        return NULL;
    }
    
    while (!feof(f) && !ferror(f))
    {
        read_result = fscanf(f,
                             "%" MAX_RECORD_TOKEN_LENGTH_STRING "s\
                             %"  MAX_RECORD_TOKEN_LENGTH_STRING "s\
                             %"  MAX_RECORD_TOKEN_LENGTH_STRING "s\
                             %d\n",
                             last_name_token,
                             first_name_token,
                             phone_number_token,
                             &id_holder);
        
        if (read_result == 4)
        {
            current_record = telephone_book_record_alloc(last_name_token,
                                                         first_name_token,
                                                         phone_number_token,
                                                         id_holder);
            if (!current_record)
            {
                fputs("ERROR: Cannot allocate memory for a telephone book "
                      "record.", stderr);
                telephone_book_record_list_free(record_list);
                return NULL;
            }
            
            telephone_book_record_list_add_record(record_list, current_record);
        }
    }
    
    return record_list;
}

/*******************************************************************************
* Writes the entire contents of the telephone record list to a specified file  *
* handle.                                                                      *
* ---                                                                          *
* Returns zero on success, and a non-zero value if something fails.            *
*******************************************************************************/
int telephone_book_record_list_write_to_file(telephone_book_record_list* list,
                                             FILE* f)
{
    telephone_book_record_list_node* current_node;
    
    if (!list || !f)
    {
        return 1;
    }
    
    if (list->size == 0)
    {
        return 0;
    }
    
    current_node = list->head;
    
    while (current_node)
    {
        fprintf(f,
                "%s %s %s %d\n",
                current_node->record->last_name,
                current_node->record->first_name,
                current_node->record->telephone_number,
                current_node->record->id);
        
        current_node = current_node->next;
    }
    
    return 0;
}

/*******************************************************************************
* Defines a telephone book record comparator. The last name of a record is the *
* primary sorting key, and the first name is the secondary sorting key.        *
* ---
* Returns zero, if both the input records have the same last and first name.   *
* If the first entry should precede the second, a negative value is returned.  *
* If the second entry should precede the first, a positive value is returned.  *
*******************************************************************************/
static int record_cmp(const void* pa, const void* pb)
{
    int c;
    
    telephone_book_record_list_node* a =
    *(telephone_book_record_list_node *const *) pa;
    
    telephone_book_record_list_node* b =
    *(telephone_book_record_list_node *const *) pb;
    
    c = strcmp(a->record->last_name, b->record->last_name);
    
    if (c)
    {
        return c;
    }
    
    return strcmp(a->record->first_name, b->record->first_name);
}

/*******************************************************************************
* Sorts the telephone records. The last name of each record is the primary     *
* sorting key, and the first name of each record is the secondary sorting key. *
* ---                                                                          *
* Returns zero on success, and a non-zero value if the sorting could not be    *
* completed.                                                                   *
*******************************************************************************/
int telephone_book_record_list_sort(telephone_book_record_list* list)
{
    telephone_book_record_list_node** array;
    telephone_book_record_list_node* current_node;
    
    int list_length;
    int index;
    
    if (!list)
    {
        return 1;
    }
    
    list_length = telephone_book_record_list_size(list);
    
    if (list_length == 0)
    {
        /* Nothing to sort. */
        return 0;
    }
    
    array = malloc(list_length * sizeof *array);
    
    if (!array)
    {
        return 1;
    }
    
    current_node = list->head;
    
    for (index = 0;
         index < list_length;
         index++, current_node = current_node->next)
    {
        array[index] = current_node;
    }
    
    qsort(array, list_length, sizeof *array, record_cmp);
    
    /* Relink the nodes: */
    list->head = array[0];
    list->tail = array[list_length - 1];
    
    list->head->prev = NULL;
    list->tail->next = NULL;
    
    for (index = 0; index < list_length - 1; ++index)
    {
        array[index]->next = array[index + 1];
    }
    
    for (index = 1; index < list_length; ++index)
    {
        array[index]->prev = array[index - 1];
    }
    
    /* Freeing memory! */
    free(array);
    return 0;
}

/*******************************************************************************
* Makes sure that each telephone book record has an unique ID.                 *
* ---                                                                          *
* Returns zero on success, and a non-zero value if something fails.            *
*******************************************************************************/
int telephone_book_record_list_fix_ids(telephone_book_record_list* list)
{
    int id;
    telephone_book_record_list_node* current_node;
    
    if (!list)
    {
        return 1;
    }
    
    id = 0;
    current_node = list->head;
    
    while (current_node)
    {
        current_node->record->id = ++id;
        current_node = current_node->next;
    }
    
    return 0;
}

/*******************************************************************************
* Frees all the memory occupied by the argument telephone book record list.    *
*******************************************************************************/
void telephone_book_record_list_free(telephone_book_record_list* list)
{
    telephone_book_record_list_node* current_node;
    telephone_book_record_list_node* next_node;
    
    if (!list)
    {
        return;
    }
    
    current_node = list->head;
    
    while (current_node)
    {
        next_node = current_node->next;
        telephone_book_record_free(current_node->record);
        free(current_node);
        current_node = next_node;
    }
    
    free(list);
}
