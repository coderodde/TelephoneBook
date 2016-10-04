#include "telephone_book_io.h"

#define MAX_RECORD_TOKEN_LENGTH_STRING "64"
#define MAX_RECORD_TOKEN_LENGTH 65

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
