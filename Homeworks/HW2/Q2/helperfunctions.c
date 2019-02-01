#include "helperfunctions.h"
#include <stdio.h>

char* trimlastchar(char* name)
{
    int i = 0;
    while(name[i] != '\0')
    {
        i++;
         
    }
    name[i-1] = '\0';
    return name;
}

char* readstringinput()
{
    char *buffer = NULL;
    int readline;
    size_t len;
    readline = getline(&buffer, &len, stdin); 
    trimlastchar(buffer);
    if (-1 != readline)
    {
        return buffer; 
    } 
    else 
    {
        return "error";
    }   
}

int file_exist (char *filename)
{
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}

int findSize(const char *file_name)
{
    struct stat st; /*declare stat variable*/
    
    /*get the size using stat()*/
    
    if(stat(file_name,&st)==0)
        return (st.st_size);
    else
        return -1;
}
char* permissions(char *file){
    struct stat st;
    char *modeval = malloc(sizeof(char) * 9 + 1);
    if(stat(file, &st) == 0){
        mode_t perm = st.st_mode;
        modeval[0] = (perm & S_IRUSR) ? 'r' : '-';
        modeval[1] = (perm & S_IWUSR) ? 'w' : '-';
        modeval[2] = (perm & S_IXUSR) ? 'x' : '-';
        modeval[3] = (perm & S_IRGRP) ? 'r' : '-';
        modeval[4] = (perm & S_IWGRP) ? 'w' : '-';
        modeval[5] = (perm & S_IXGRP) ? 'x' : '-';
        modeval[6] = (perm & S_IROTH) ? 'r' : '-';
        modeval[7] = (perm & S_IWOTH) ? 'w' : '-';
        modeval[8] = (perm & S_IXOTH) ? 'x' : '-';
        modeval[9] = '\0';
        return modeval;     
    }
    else{
        return strerror(errno);
    }   
}
void modifyperm()
{
    FILE *fp;
    struct stat info;
    char *str = NULL;
    char *curr_perm = NULL;
    char o,g,t,x;
    int flag = 0;
    printf("Enter the name of the file to change its permission :");
    str = readstringinput();
    if (file_exist (str))
    {
        printf ("File %s exists\n",str);
        curr_perm = permissions(str);
        printf("The current file permission is : %s\n",curr_perm);
        printf("The permission options are as follows:\n");
        printf("1. r(read)  2. w(write  3. rw(read-write) 4. none\n");
        printf("Enter permission for owner : 1 or 2 or 3 or 4: \n");
        o = getchar();
        x = getchar();
        printf("Enter permission for group : 1 or 2 or 3 or 4: \n");
        g = getchar();
        x = getchar();
        printf("Enter permission for others : 1 or 2 or 3 or 4: \n");
        t = getchar();
        x = getchar();
        //for owner
        if(o == '1')
        {
            flag |= S_IRUSR;
        }
        else if (o == '2')
        {
            flag |= S_IWUSR;   
        }
        else if (o == '3')
        {
            flag = flag |S_IRUSR |S_IWUSR;
        }
        else if(o == '4')
        {
            flag |= 1;
        }
        else 
        {
            printf("Invalid entry for owner");
        }
        //for grp
        if(g == '1')
        {
            flag |= S_IRGRP;
        }
        else if (g == '2')
        {
            flag |= S_IWGRP;   
        }
        else if (g == '3')
        {
            flag = flag |S_IRGRP |S_IWGRP;
        }
        else if(g == '4')
        {
            flag |= 1;
        }
        else 
        {
            printf("Invalid entry for group");
        }
        //for others
        if(t == '1')
        {
            flag |= S_IROTH;
        }
        else if (t == '2')
        {
            flag |= S_IWOTH;   
        }
        else if (t == '3')
        {
            flag = flag |S_IROTH |S_IWOTH;
        }
        else if(t == '4')
        {
            flag |= 1;
        }
        else 
        {
            printf("Invalid entry for others");
        }
        chmod(str,flag);
        printf("After changes the permission is %s\n",permissions(str));
        free(str);
    }
    else 
    {
        printf("File %s does not exit\n",str);
        return;
    }

}

void openforwrite()
{
    FILE *fpw;
    char *str = NULL;
    char c,a,r,x;
    printf("Enter the name of file to open for writing : ");
    str = readstringinput();
    if (file_exist (str))
    {
        printf ("File %s exists\n",str);
        fpw = fopen(str, "w");
        if (fpw != NULL)
        {
            printf("File %s has been opened\n",str);
            printf("Enter the operation you want to perform : \n");
            printf("1. Write a character to the file\n");
            printf("2. Close the created file\n");
            printf("3. Go back to previous menu\n");
            c = getchar();
            x = getchar();
            if (c == '1')
            {
AGAIN:          printf("Enter the character to write to the file : ");
                a = getchar();
                x = getchar();
                if((fprintf(fpw,"%c", a)) > 0)
                {
                    printf("Character %c has been successfully written in file %s\n",a,str);
                }
                else
                {
                    printf("File write unsuccessful %s \n",strerror(errno));
                }
                printf("Do you wish to write more characters? Y/N : ") ;
                r = getchar();
                x = getchar();
                if(r == 'Y')
                {
                    goto AGAIN;
                }
                else if (r == 'N')
                {
                   goto CLOSE; 
                }
                else 
                {
                    printf("invalid entry going back to main menu\n");
                   // break;
                }

            }
            if (c == '2')
            {
    CLOSE:      printf("Closing the file...\n");
                fclose(fpw);
                free(str);
            }
            if(c == '3')
            {
                printf("Going back .....\n");
            }

        }
        else 
        {
            printf("fpw is null\n");
        }
        
    }
    else 
    {
        printf("File %s does not exist\n",str);
    }


}

void openforappend()
{
    FILE *fpa;
    char *str = NULL;
    char *buffer = NULL;
    char **ptr = NULL;
    int readline;
    size_t len;
    size_t idx = 0;    //array index counter
    char c,r,x;
    int b;
    printf("Enter the name of file to open for appending : ");
    str = readstringinput();
    if (file_exist (str))
    {
        printf ("File %s exists\n",str);
        fpa = fopen(str, "a");
        if (fpa != NULL)
        {
        
            printf("File %s has been opened\n",str);
            while(1)
            {
                printf("\n");
                printf("Select options from below: \n");
                printf("1. Dynamically allocate an array of memory\n");
                printf("2. Read an input string from the command line and write to the string to the allocated array\n");
                printf("3. Write the string to the file\n");
                printf("4. Flush file output\n");
                printf("5. Close the file opened in append mode\n");
                printf("6. Go back to previous menu\n");
                c = getchar();
                x = getchar();
                if (c == '1')
                {
                    printf("To dynamically allocate an array of memory\n");
                    printf("Enter the number of bytes of memory required : ");
                    scanf("%d",&b);
                    x = getchar();
                    ptr = calloc(b , sizeof(*ptr));
                    if(ptr != NULL)
                    {
                        printf("Memory successfully allocated : No of bytes %d \n",b);
                    }
                    else 
                    {
                        printf("Memory allocation unsuccessfull\n");
                    }
                    continue;

                }
                else if (c == '2')
                {
                    printf("Enter a string to write to the allocated memory : ");
                    readline = getline(&buffer, &len, stdin);
                    while (readline > 0 && (buffer[readline-1] == '\n' || buffer[readline-1] == '\r')) 
                    {
                        buffer[--readline] = 0;
                    }
                    //do
                    //{
                        //ptr[idx] = strdup (buffer);
                        *ptr = strdup (buffer);
                        printf("Bytes in input  string is %d\n",readline);
                        //idx++; 
                        while (readline >= b) {          /* if lines exceed total no of bytes allocated, reallocate */
                            printf("ALlocated bytes is %d\n",b);
                            printf("Memory allocated less than the input string... Reallocating\n");
                            char **tmp = realloc (ptr, b * 2 * sizeof *tmp);
                            if (!tmp) {
                                printf ("error: memory exhausted.\n");
                                break;
                            }
                            ptr = tmp;
                            b *= 2;
                            //printf("readline is %d\n",readline);
                            //printf("New allocated no of bytes is %d\n",b);
                        } 
                    printf ("\nThe line to write in the file is: %s\n",*ptr);
                    free (ptr);


                }
                else if (c == '3')
                {
                    printf("The string to write to the file from step 2 is : %s\n",buffer);
                    //buffer = readstringinput();
                    //x = getchar();
                    if((fprintf(fpa,"%s", buffer)) > 0)
                    {
                        printf("String %s has been successfully written in file %s\n",buffer,str);
                    }
                    else
                    {
                        printf("File write unsuccessful %s \n",strerror(errno));
                    }
        AGAIN:      printf("Do you wish add  more string? Y/N : ") ;
                    r = getchar();
                    x = getchar();
                    if(r == 'Y')
                    {
                        buffer = readstringinput();
                        x = getchar();
                        if((fprintf(fpa,"%s", buffer)) > 0)
                        {
                            printf("String %s has been successfully written in file %s\n",buffer,str);
                        }
                        else
                        {
                            printf("File write unsuccessful %s \n",strerror(errno));
                        }
                        goto AGAIN;
                    }
                    else if (r == 'N')
                    {
                        //goto CLOSE; 
                        continue;
                    }
                    else 
                    {
                        printf("invalid entry going back to main menu\n");
                    // break;
                    }

                }
                else if(c == '4')
                {
                    printf("Flusing the file output: \n");
                    //fprintf(fpa, "Hello world! This is shreya");
                    fflush(fpa);
                    //fflush(buffer);
                    printf("Flushed\n");
                }
                else if (c == '5')
                {
        /*CLOSE:*/      printf("Closing the file...\n");
                    fclose(fpa);
                    free(str);
                    printf("The file has been closed the only option is to go back to menu\n");
                    break;
                }
                else if(c == '6')
                {
                    break;
                }
                else{

                    printf("Invalid choice\n");
                    break;

                }
            }

        }
    
        else 
        {
            printf("fpa is null\n");
        }
        
    }
    else 
    {
        printf("File %s does not exist\n",str);
    }


}

void openforread()
{
    FILE *fpr;
    char *str = NULL;
    char a,c,x;
    int n,ch;
    int size=0;
    
    printf("Enter the name of file to open for reading : ");
    str = readstringinput();
    size=findSize(str);
    char *buff = (char*)malloc (size);
    if (file_exist (str))
    {
        printf ("File %s exists\n",str);
        fpr = fopen(str, "r");
        if (fpr != NULL)
        {
            while(1)
            {
                printf("Select from the following operations: \n");
                printf("1. Read a single character (getc)\n");
                printf("2. Read a string of characters (gets)\n");
                printf("3. Close the file for reading\n");
                printf("4. Free the memory\n");
                printf("5. Go back to previous menu\n");
                c = getchar();
                x = getchar();
                if ((c < '1') || (c > '5'))
                {
                    printf("Invalid Choice\n");
                    break;
                }
                if(c == '1')
                {
                    ch = 0;
                    printf("To read a single character from the file %s\n",str);
                    printf("Enter the no of character you want displayed: (eg 1,2..,10 etc) : ");
                    scanf("%d",&n);
                    x = getchar();
                    do
                    { 
                        // Taking input single character at a time 
                        char c = fgetc(fpr); 
                        ch++;
                        // Checking for end of file 
                        if (feof(fpr))
                        {
                            printf("Reached end of file.\n");
                            break;
                        } 
                        if (ch == n)
                        {
                            printf("%c\n", c); 
                            rewind(fpr);
                            break;
                        }
                        
                    }  while(1); 
                }
                if(c == '2')
                {
                   
                    printf("To read and display a string of characters from the file %s\n",str);
                    //printf("Size is %d\n",size);
                    //printf("in here..\n");
                    rewind(fpr); 
                    while(fgets(buff, size+1, fpr))
                    {
                        printf("The string is: %s\n", buff);
                         
                    }
                    rewind(fpr);
                    continue;
                }
                if(c =='3')
                {
                    printf("Closing the file %s\n",str);
                    fclose(fpr);
                    printf("Closed\n");
                    printf("The only options available now are as below:\n");
                    printf("1. Free the memory\n");
                    printf("2. Go back to previous menu\n");
                    a = getchar();
                    x = getchar();
                    if((a < '1')||(a > '2'))
                    {
                        printf("Invalid option!\n");
                        break;
                    }
                    else if(a == '1')
                    {
                        goto FREE;
                    }
                    else{
                        break;
                    }

                    

                    //continue;
                }
     FREE:      if(c == '4')
                {
                    printf("Freeing the memory\n");
                    free(buff);
                    printf("freed\n");
                    continue;

                }
                if(c == '5')
                {
                    break;
                }
            }
        }
        else 
        {
            printf("file pointer null\n");
            return;
        }
    }
    else 
    {
        printf("File  %s does not exist\n",str);
        return;
    }




}