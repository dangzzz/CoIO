#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

typedef struct s{
    s(){
        a=2;b=1;
    }
   int a;
   int b;
} t;

void *test(void *ptr)
{
	int i;
    int fd=open("123",O_RDWR|O_DSYNC);
    //|O_DIRECT|O_DSYNC|O_SYNC
    if(fd==-1)
	{
		printf("error is %s\n",strerror(errno));
	}
    printf("this is %d ya",fd);


}

#define WRITE (2U<<30)
#define QUEUE (1U<<30)
#define READ (3U<<30)
#define MASK (3U<<30)
int main(void){

	/*
	pthread_t pId;
    pthread_t p2;
	int i,ret;
	//创建子线程，线程id为pId
	ret = pthread_create(&pId,NULL,test,NULL);
	

    ret = pthread_create(&p2,NULL,test,NULL);
	
	if(ret != 0)
	{
		printf("create pthread error!\n");
    
	}

	

	//等待线程pId的完成
	pthread_join(pId,NULL);
	printf("main thread  exit\n");
*/
	unsigned int a = 0;
	a = QUEUE;
	printf(".%u.",a);
	return 0;
}