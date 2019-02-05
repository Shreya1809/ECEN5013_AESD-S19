#ifndef __HELPERFUNCTIONS_H__
#define __HELPERFUNCTIONS_H__

//#define _POSIX_SOURCE
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
//#undef _POSIX_SOURCE

char* trimlastchar(char* name);
char* readstringinput();
int file_exist (char *filename);
char* permissions(char *file);
void modifyperm();
void openforwrite();
void openforappend();
void openforread();
int findSize(const char *file_name);


#endif