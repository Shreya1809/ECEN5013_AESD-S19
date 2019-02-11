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
#include <errno.h>

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
	printf("[BBG LOG] ****************** Testing Syscall MERGE Sort by Shreya ******************\n");
	printf("\n");
	int ret = -1;
	const int buff_len = 256;
	int *inptr = NULL;
	int *outptr = NULL;
	long start_time,end_time,diff;
	inptr = (int*)malloc(sizeof(int) * buff_len);
	if(inptr == NULL)
	{
		printf("[BBG ERROR] Input buffer malloc fail\n");
		perror("[BBG ERROR] ");	
		return -1;
	}
	printf("[BBG LOG] Input buffer malloc success\n");

	outptr =(int*)malloc(sizeof(int) * buff_len);
	if(outptr == NULL)
	{
		printf("[BBG ERROR] Output buffer malloc fail \n");
		perror("[BBG ERROR] ");	
		return -1;
	}
	printf("[BBG LOG] Output buffer malloc success\n");

	/*generating random number */
	srand(time(0));
	for(int i = 0; i < buff_len; i++)
	{
		*(inptr+i) =rand()%300;
	}
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
	bzero(outptr,buff_len);
	printf("[BBG LOG] ~~~~~~~~~~~Null input pointer #Use Case-1~~~~~~~~~~~\n");
	/* use case 1, null pointer test 2 */
	start_time = print_getTime();
	ret = syscall(400,NULL,256,outptr);
	end_time = print_getTime();
	diff = end_time - start_time;
	printf("[BBG LOG] Execution time for #Use case-1 is %ld msec\n",diff);
	if(ret == 0)
	{
		printf("[BBG ERROR] Null input pointer #Use Case-1 Success???? : \n");	
	}
	else{
		printf("[BBG LOG] Null input pointer #Use Case-1 FAILED!!!! \n");
		perror("[BBG ERROR] ");		
	}
	printf("\n");
	printf("[BBG LOG] ~~~~~~~~~~~Null output pointer #Use Case-2~~~~~~~~~~~ \n");
	/* use case 1, null pointer test 2 */
	start_time = print_getTime();
	ret = syscall(400,inptr,256,NULL);
	end_time = print_getTime();
	diff = end_time - start_time;
	printf("[BBG LOG] Execution time for #Use case-2 is %ld msec\n",diff);
	if(ret == 0)
	{
		printf("[BBG ERROR] Null output pointer #Use Case-2 Success???? : \n");	
	}
	else{
		printf("[BBG LOG] Null output pointer #Use Case-2 FAILED!!!! \n");
		perror("[BBG ERROR] ");	
	}
	printf("\n");
	printf("[BBG LOG] ~~~~~~~~~~~Invalid buffer Size #Use Case-3~~~~~~~~~~~ \n");
	/* use case 1, null pointer test 2 */
	start_time = print_getTime();
	ret = syscall(400,inptr,200,outptr);
	end_time = print_getTime();
	diff = end_time - start_time;
	printf("[BBG LOG] Execution time for #Use case-3 is %ld msec\n",diff);
	if(ret == 0)
	{
		printf("[BBG ERROR] Invalid buffer Size #Use Case-3 Success???? : \n");	
	}
	else{
		printf("[BBG LOG] Invalid buffer Size #Use Case-3 FAILED!!!! \n");
		perror("[BBG ERROR] ");	
	}
	printf("\n");
	printf("[BBG LOG] ~~~~~~~~~~~Positive and valid #Use Case-4~~~~~~~~~~~ \n");
	/* use case 1, null pointer test 2 */
	start_time = print_getTime();
	ret = syscall(400,inptr,256,outptr);
	end_time = print_getTime();
	diff = end_time - start_time;
	printf("[BBG LOG] Execution time for #Use case-4 is %ld msec\n",diff);
	if(ret == 0)
	{
		printf("[BBG LOG] Positive #Use Case-4 SUCCESS!!!! \n");
		/*printf("[BBG LOG] Unsorted Array is ------------------->\n");
		for(int i = 0;i < 256 ; i++)
		{
			printf("%d",*(inptr+i));
			if(((i + 1) % 16) != 0)
			{
				printf("\t");
			}
			else printf("\n");
		}
		printf("\n");*/
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
		printf("[BBG ERROR] Valid #Use Case-4 Failed??? : \n");	
		perror("[BBG ERROR] ");	
		
	}
	printf("[BBG LOG] ****************** Testing Syscall Completed ******************\n");
	printf("\n");
	printf("---------------------------Displaying Kernel Log-------------------------\n");
	system("dmesg | grep 'SYS_MERGESORT' ");
	printf("\n ---END---\n");
	free(outptr);
	free(inptr);
	return 0;
}










	

