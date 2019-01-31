/*
THe following program can:
1. Print to standard out an interesting string using printf
2. Create a file
3. Modify the permissions of the file to be read/write
4. Open the file (for writing)
5. Write a character to the file
6. Close the file
7. Open the file (in append mode)
8. Dynamically allocate an array of memory
9. Read an input string from the command line and write to the string to the allocated array
10. Write the string to the file
11. Flush file output
12. Close the file
13. Open the file (for reading)
14. Read a single character (getc)
15. Read a string of characters (gets)
16. Close the file
17. Free the memory
*/
/* References :
1. http://man7.org/linux/man-pages/man3/getline.3.html
2. https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.bpxbd00/rtchm.htm
3. Makefile : https://stackoverflow.com/questions/1484817/how-do-i-make-a-simple-makefile-for-gcc-on-linux
4. file permissions : https://stackoverflow.com/questions/8812959/how-to-read-linux-file-permission-programmatically-in-c-c
5. https://www.geeksforgeeks.org/fgetc-fputc-c/
6. https://stackoverflow.com/questions/30428615/taking-user-input-and-storing-it-in-an-array-of-strings-in-c
7. https://www.includehelp.com/c-programs/find-size-of-file.aspx


*/
/* Author :Shreya Chakraborty
   Date : Jan 28 th 2019
   Course : AESD
   University of Colorado BOulder */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "helperfunctions.h"

int main()
{
    char c,x;
    char *str = NULL;
    FILE *fp;
    printf("The following program demonstrates various file operation\n");
    while(1)
    {
        printf("\n");
        printf("Select operation number to proceed:\n");
        printf("1. Print to standard out an interesting string using printf\n");
        printf("2. Create a file\n");
        printf("3. Operations Menu\n");
        c = getchar();
        x = getchar();
        if (c != '1' && c != '2' && c != '3')
        {
            printf("Invalid option. Try again\n");
            continue;
        }
        else if(c == '1')
        {
            printf("Enter string to be printed on the std out using printf : ");
            str = readstringinput();
            printf("The entered string is : %s\n",str);
            free(str);
            continue;
        }
        else if (c == '2')
        {
            printf("Enter the name of the file to create : ");
            str = readstringinput();
            fp = fopen (str, "w+");
            if (fp != NULL)
            {
                printf("File %s has been created\n",str);
            }
            fclose(fp);
            free(str);
            continue;
        }
        else{
            //printf("\n");
            printf("Enter further operation no as follows:\n");
            printf("1. Modify the permissions of the file to be read/write\n");
            printf("2. Open the file (for writing)\n");
            printf("3. Open the file (in append mode)\n");
            printf("4. Open the file (for reading)\n");
            printf("5. Go back to main menu\n");

            c = getchar();
            x = getchar();

            switch(c)
            {
                case '1' : modifyperm();
                        break;
                case '2' : openforwrite();
                        break;
                case '3' : openforappend();
                        break;
                case '4' : openforread();
                        break;
                case '5' : 
                        break;
                default : printf("Invalid choice entered\n");
                        break;
            }   




        }
        /*//printf("3. Modify the permissions of the file to be read/write\n"); --
        //printf("4. Open the file (for writing)\n");--
        //printf("5. Write a character to the file\n");--
        //printf("6. Close the created file\n");--
        printf("7. Open the file (in append mode)\n");--
        //printf("8. Dynamically allocate an array of memory\n");
        //printf("9. Read an input string from the command line and write to the string to the allocated array\n");
        //printf("10. Write the string to the file\n");
        //printf("11. Flush file output\n");
        //printf("12. Close the file opened in append mode\n");
        printf("13. Open the file (for reading)\n");--
        //printf("14. Read a single character (getc)\n");
        //printf("15. Read a string of characters (gets)\n");
        //printf("16. Close the file for reading\n");
        //printf("17. Free the memory\n");*/
    }
    return 0;
}   

