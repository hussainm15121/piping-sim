#include <string.h>
typedef struct node{		//Structure for Linked List
    char *data;
    struct node *next; 
} node;

char *
strtok (char *s, const char *delim)		//strtok implementation for use in main code
{
  static char *olds;
  return __strtok_r (s, delim, &olds);
}
