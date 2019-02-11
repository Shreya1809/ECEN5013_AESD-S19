/* tests the sys_sort for various use cases */
/* use case 1 - invalid pointer */
/* use case 2 - invalid size */
/* use case 3 - normal operation */
#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/time.h>
#include <malloc.h>
#include <time.h>
#include <strings.h>

unsigned long print_getTime()
{
	struct timeval tv;
	unsigned long curr_time;
	gettimeofday(&tv,NULL);
	curr_time = (tv.tv_sec*1000) + (tv.tv_usec/1000);
	//printf("[BBG LOG] Timestamp : %ld\n",curr_time); 
	return curr_time;
}

int main()
{
	printf("[BBG LOG] ****************** CRON TASK ******************\n");
	printf("\n");
	int ret = -1;
    int pid,uid;
    time_t now;
	const int buff_len = 256;
	int *inptr = NULL;
	int *outptr = NULL;
	long start_time,end_time,diff;
    printf("[BBG LOG] ~~~~~~~~~~~~~ CURRENT PROCESS ID ~~~~~~~~~~~~~ \n");
    pid = syscall(20);
    printf("\n");
    printf("[BBG LOG] Process ID is %d\n",pid);
    printf("\n\n");
    printf("[BBG LOG] ~~~~~~~~~~~~~~ CURRENT USER ID ~~~~~~~~~~~~~~~ \n");
    uid = syscall(24);
    printf("\n");
    printf("[BBG LOG] Process ID is %d\n",uid);
    printf("\n\n");
    printf("[BBG LOG] ~~~~~~~~~~~~ CURRENT DATE & TIME ~~~~~~~~~~~~~ \n");
    time(&now);
    printf("\n");
    printf("[BBG LOG] Current date and time is %s\n",ctime(&now));
    printf("\n\n");
    printf("[BBG LOG] ~~~~~~~~~~~~~ SYSTEM CALL OUTPUT ~~~~~~~~~~~~~ \n");
	inptr = (int*)malloc(sizeof(int) * buff_len);
	if(inptr == NULL)
	{
		printf("[BBG ERROR] Input buffer malloc fail\n");
		return -1;
	}
	printf("[BBG LOG] Input buffer malloc success\n");

	outptr =(int*)malloc(sizeof(int) * buff_len);
	if(outptr == NULL)
	{
		printf("[BBG ERROR] Output buffer malloc fail\n");
		return -1;
	}
	printf("[BBG LOG] Output buffer malloc success\n");

	/*generating random number */
	srand(time(0));
	for(int i = 0; i < buff_len; i++)
	{
		*(inptr+i) =rand()%300;
	}
	printf("\n");
	bzero(outptr,buff_len);
	
	start_time = print_getTime();
	ret = syscall(399,inptr,256,outptr);
	end_time = print_getTime();
	diff = end_time - start_time;
	printf("[BBG LOG] Execution time for sys_sort is %ld msec\n",diff);
	if(ret == 0)
	{
		printf("[BBG LOG] Unsorted Array is ------------------->\n");
		for(int i = 0;i < 256 ; i++)
		{
			printf("%d",*(inptr+i));
			if(((i + 1) % 16) != 0)
			{
				printf("\t");
			}
			else printf("\n");
		}
		printf("\n");
		printf("[BBG LOG] The Sorted Array is ------------------->\n ");
		for(int i = 0;i < 256 ; i++)
		{
			printf("%d",*(outptr+i));
			if(((i + 1) % 16) != 0)
			{
				printf("\t");
			}
			else printf("\n");
		}
		printf("\n");
	}
	else{
		printf("[BBG ERROR] Failed??? : \n");	
		
	}
	//printf("[BBG LOG] ****************** Testing Syscall Completed ******************\n");
	printf("\n");
	printf("---------------------------Displaying Kernel Log-------------------------\n");
	system("dmesg | grep 'SYS_SORT' ");
	printf("\n ---END---\n");
	free(outptr);
	free(inptr);
	return 0;
}










	

