#include "telephone_record.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MAX_RECORD_TOKEN_LENGTH_STRING "64"

static const size_t MAX_RECORD_TOKEN_LENGTH = 64;

typedef struct telephone_book_record_list_node {
    telephone_book_record* record;
    struct telephone_book_record_list_node* next;
    struct telephone_book_record_list_node* prev;
} telephone_book_record_list_node;

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
 * returned is something fails.                                                 *
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
 * Returns a zero value if the appropriate entry was not found, 1 if the entry  *
 * was found and removed, and -1 on error.                                      *
 *******************************************************************************/
int telephone_book_record_list_remove_entry(telephone_book_record_list* list,
                                            int id)
{
    telephone_book_record_list_node* current_node;
    telephone_book_record_list_node* next_node;
    
    if (!list)
    {
        return -1;
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
            
            telephone_book_record_free(current_node->record);
            free(current_node);
            return 1;
        }
        
        current_node = next_node;
    }
    
    return 0;
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
    
    char last_name_token   [MAX_RECORD_TOKEN_LENGTH + 1];
    char first_name_token  [MAX_RECORD_TOKEN_LENGTH + 1];
    char phone_number_token[MAX_RECORD_TOKEN_LENGTH + 1];
    int  id_holder;
    
    int read_result;
    
    telephone_book_record* current_record;
    
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
                fputs("Cannot allocate memory for a telephone book record.",
                      stderr);
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