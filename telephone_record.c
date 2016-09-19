#include "telephone_record.h"
#include <stdlib.h>
#include <string.h>

#define RECORD_TOKEN_LENGTH 64
#define RECORD_TOKEN_LENGTH_STR "64"
#define MAX(a,b) ((a) > (b) ? (a) : (b))

typedef struct telephone_record_list_node {
    struct telephone_record_list_node* next;
    telephone_record* record;
} telephone_record_list_node;

int cmp(const void* pa, const void* pb)
{
    telephone_record* a = *(telephone_record *const *) pa;
    telephone_record* b = *(telephone_record *const *) pb;
    
    int c = strcmp(a->last_name, b->last_name);
    
    if (c)
    {
        return c;
    }
    
    return strcmp(a->first_name, b->first_name);
}

/*******************************************************************************
 * Prints in a nifty format the contents of the telephone book.                 *
 *******************************************************************************/
//bool telephone_record_list_print(telephone_record_list* list);

/*******************************************************************************
 * Appends a record to the record list.                                         *
 *******************************************************************************/
/*bool telephone_record_list_add_record(telephone_record_list* list,
                                      telephone_record* record);*/

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



telephone_record_list_node*
telephone_record_list_node_alloc(telephone_record* record)
{
    telephone_record_list_node* new_node = malloc(sizeof *new_node);
    
    if (!new_node)
    {
        return NULL;
    }
    
    new_node->record = record;
    new_node->next = NULL;
    return new_node;
}

bool telephone_record_list_add_record(telephone_record_list* list,
                                      telephone_record* record)
{
    if (!list || !record)
    {
        return false;
    }
    
    telephone_record_list_node* new_list_node =
    telephone_record_list_node_alloc(record);
    
    if (!new_list_node)
    {
        return false;
    }
    
    if (list->tail)
    {
        /* Once here, the list is not empty; append to the tail. */
        list->tail->next = new_list_node;
        list->tail = new_list_node;
    }
    else
    {
        /* Once here, the list is empty. */
        list->head = new_list_node;
        list->tail = new_list_node;
    }
    
    list->size++;
    return true;
}

bool telephone_record_list_init(telephone_record_list* list)
{
    if (!list)
    {
        return false;
    }
    
    list->size = 0;
    list->bad_record_count = 0;
    list->head = NULL;
    list->tail = NULL;
    return true;
}

bool telephone_record_list_print(telephone_record_list* list)
{
    if (!list)
    {
        return false;
    }
    
    size_t first_name_token_max_length   = 0;
    size_t last_name_token_max_length    = 0;
    size_t phone_number_token_max_length = 0;
    size_t id_token_max_length           = 0;
    char id_string_holder[40];
    
    telephone_record_list_node* current_list_node = list->head;
    
    while (current_list_node)
    {
        if (!current_list_node->record->is_valid)
        {
            puts("omitting");
            current_list_node = current_list_node->next;
            continue;
        }
        
        first_name_token_max_length =
            MAX(first_name_token_max_length,
                strlen(current_list_node->record->first_name));
        
        last_name_token_max_length =
            MAX(last_name_token_max_length,
                strlen(current_list_node->record->last_name));
        
        phone_number_token_max_length =
            MAX(phone_number_token_max_length,
                strlen(current_list_node->record->telephone_number));
        
        sprintf(id_string_holder, "%d", current_list_node->record->id);
        
        id_token_max_length = MAX(id_token_max_length, strlen(id_string_holder));
        
        // Proceed to the next record:
        current_list_node = current_list_node->next;
    }
    
    // Print the header:
    size_t array_length = list->size - list->bad_record_count;
    
    telephone_record** records_as_array =
        malloc(sizeof (telephone_record*) * array_length);
    
    current_list_node = list->head;
    size_t index = 0;
    
    while (current_list_node)
    {
        if (current_list_node->record->is_valid)
        {
            records_as_array[index++] = current_list_node->record;
        }
        
        current_list_node = current_list_node->next;
    }
    
    qsort(records_as_array, array_length, sizeof(telephone_record*), cmp);
    
    for (size_t i = 0; i != array_length; ++i)
    {
        telephone_record* current_record = records_as_array[i];
        
        printf("%s, %s - %s, ID %d\n",
               current_record->last_name,
               current_record->first_name,
               current_record->telephone_number,
               current_record->id);
    }
    
    return true;
}

telephone_record_list* telephone_record_list_read_from_file(FILE* f)
{
    telephone_record_list* record_list = malloc(sizeof *record_list);
    
    if (!record_list)
    {
        return NULL;
    }
    
    telephone_record_list_init(record_list);
    
    char first_name_token  [RECORD_TOKEN_LENGTH + 1];
    char last_name_token   [RECORD_TOKEN_LENGTH + 1];
    char phone_number_token[RECORD_TOKEN_LENGTH + 1];
    char id_buffer         [30];
    int  id;
    
    while (!feof(f) && !ferror(f))
    {
        int result = fscanf(f,
                            "%" RECORD_TOKEN_LENGTH_STR "s\
                            %"  RECORD_TOKEN_LENGTH_STR "s\
                            %"  RECORD_TOKEN_LENGTH_STR "s\
                            %d\n",
                            last_name_token,
                            first_name_token,
                            phone_number_token,
                            &id);
        
        telephone_record* new_record = malloc(sizeof *new_record);
        
        if (!new_record)
        {
            record_list->bad_record_count++;
        }
        else if (result == 4)
        {
            char* new_first_name_token = malloc(sizeof(char) * strlen(first_name_token) + 1);
            char* new_last_name_token  = malloc(sizeof(char) * strlen(last_name_token) + 1);
            char* new_phone_number_token = malloc(sizeof(char) * strlen(phone_number_token) + 1);
            
            strcpy(new_first_name_token, first_name_token);
            strcpy(new_last_name_token, last_name_token);
            strcpy(new_phone_number_token, phone_number_token);
            
            new_record->first_name       = new_first_name_token;
            new_record->last_name        = new_last_name_token;
            new_record->telephone_number = new_phone_number_token;
            new_record->id = id;
            new_record->is_valid = true;
        }
        else
        {
            new_record->is_valid = false;
            record_list->bad_record_count++;
        }
        
        telephone_record_list_add_record(record_list, new_record);
    }
    
    return record_list;
}

static void telephone_record_free(telephone_record* record)
{
    if (!record)
    {
        return;
    }
    
    if (record->is_valid)
    {
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
    }
    
    free(record);
}

void telephone_record_list_free(telephone_record_list* list)
{
    if (!list)
    {
        return;
    }
    
    telephone_record_list_node* current_node = list->head;
    telephone_record_list_node* next_node = current_node->next;
    
    while (true)
    {
        telephone_record_free(current_node->record);
        free(current_node);
        current_node = next_node;
        
        if (!next_node)
        {
            break;
        }
        
        next_node = next_node->next;
    }
    
    free(list);
}

bool telephone_record_init(telephone_record* record,
                           char* first_name,
                           char* last_name,
                           char* telephone_number);

bool telephone_record_append_to_file(FILE* file, telephone_record* record);
