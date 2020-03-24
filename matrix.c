#include "main.h"


void
error (char *msg, FILE * fp)
{
  fprintf (fp, "ERROR: %s\n", msg);
  exit (1);
}


/*
 * **  Function  : void *MMmalloc(size_t size, const char * function_name) ** 
 * Return    : the pointer to the memory of the new allocated block **
 * Remarks   : MMmalloc() allocates size bytes and returns a pointer to the
 * **              allocated memory.  The memory is not cleared. **
 *    If an error occours, an error is printed, including the **
 * function name of the calling function. 
 */
void *
MMmalloc (size_t size, const char *f_name)
{
  void *temp_pointer;
  if ((temp_pointer = malloc (size)) == NULL)
    {
      /*
       * If problems arise from the memory allocation, an error message is
       *   
       */
      /*
       * printed before exiting the program execution.
       *   
       */
      printf ("\nError:  Memory allocation error in ANCLES function ");
      printf ("%s\n", f_name);
      exit (1);
    }
  return temp_pointer;
}


