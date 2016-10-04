#ifndef telephone_book_io_h
#define telephone_book_io_h

#include "telephone_book.h"
#include <stdio.h>

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


#endif /* telephone_book_io_h */
