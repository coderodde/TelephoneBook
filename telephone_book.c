#include "telephone_book.h"
#include "telephone_book_utils.h"
#include <stdlib.h>
#include <string.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

/*******************************************************************************
* Documentation comments may be found in telephone_book.h                      *
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
    
    return node;
}

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
    
    record->last_name        = malloc(strlen(last_name) + 1);
    record->first_name       = malloc(strlen(first_name) + 1);
    record->telephone_number = malloc(strlen(phone_number) + 1);
    record->id = id;
    
    strcpy(record->last_name, last_name);
    strcpy(record->first_name, first_name);
    strcpy(record->telephone_number, phone_number);
    
    return record;
}

void telephone_book_record_free(telephone_book_record* record)
{
    if (!record)
    {
        return;
    }
    
    free(record->first_name);
    free(record->last_name);
    free(record->telephone_number);
    free(record);
}

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

telephone_book_record*
telephone_book_record_list_remove_entry(telephone_book_record_list* list,
                                        int id)
{
    telephone_book_record_list_node* previous_node;
    telephone_book_record_list_node* current_node;
    telephone_book_record_list_node* next_node;
    telephone_book_record* removed_record;
    
    if (!list)
    {
        return NULL;
    }
    
    previous_node = NULL;
    current_node = list->head;
    
    while (current_node)
    {
        next_node = current_node->next;
        
        /* Since the program makes sure that all IDs are unique, we can */
        /* return as soon as we remove the first match. */
        if (current_node->record->id == id)
        {
            if (previous_node)
            {
                previous_node->next = current_node->next;
            }
            else
            {
                list->head = current_node->next;
            }
            
            if (!current_node->next)
            {
                list->tail = previous_node;
            }
            
            removed_record = current_node->record;
            free(current_node);
            return removed_record;
        }
        
        previous_node = current_node;
        current_node = next_node;
    }
    
    return NULL;
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
    
    list->tail->next = NULL;
    
    for (index = 0; index < list_length - 1; ++index)
    {
        array[index]->next = array[index + 1];
    }
    
    /* Freeing memory! */
    free(array);
    return 0;
}

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
