#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include <sys/time.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string>
#include"CoIO.h"
#define TEST
//线程函数

 pthread_mutex_t mutex; 

char *randstr(char *pointer, int n,char c)
{
    int i,randnum;

    for (i = 0; i < n; i++)
    {
                 //随机数生成函数
        *pointer = c;          //从字符数组中取值
        pointer++;
    }
    *pointer = '\0';                            //字符串结束符
    return (pointer - n);                       //返回指向随机字符串的指针首地址
}

void *test2(void * arg)
{

	int fd = open("/novadir/123",O_RDWR|O_SYNC|O_APPEND);
	char * ps = (char*) arg;
	char* a= new char[104857600];
	const char *buf = randstr(a,104857600,*ps);
	printf("%c",buf[5]);
	printf("%lld\n",(long long)buf);

#ifdef TEST
	struct timeval start, end;
	gettimeofday( &start, NULL );  
#endif
	pthread_mutex_lock(&mutex);
	usleep(100000U);
	write(fd,buf,104857600);
	fsync(fd);
	pthread_mutex_unlock(&mutex);
#ifdef TEST
	gettimeofday( &end, NULL );
	printf("%s::过去了%ld\n",ps,end.tv_sec*1000000+end.tv_usec-start.tv_sec*1000000-start.tv_usec);

#endif
}



void *test(void * arg)
{
	co_file_t * cofile;
	int fd = co_open("/novadir/123",O_RDWR|O_SYNC|O_APPEND,cofile);
	char * ps = (char*) arg;
	char* a= new char[104857600];
	const char *buf = randstr(a,104857600,*ps);
	printf("%c",buf[5]);
	printf("%lld\n",(long long)buf);

#ifdef TEST
	struct timeval start, end;
	gettimeofday( &start, NULL );  
#endif
	co_write(fd,buf,104857600,cofile);

#ifdef TEST
	gettimeofday( &end, NULL );
	printf("%s::过去了%ld\n",ps,end.tv_sec*1000000+end.tv_usec-start.tv_sec*1000000-start.tv_usec);
	
	
#endif
}






int main(void)
{	/*
	co_file_t * cofile;
    int fd = co_open("123",O_RDWR|O_DSYNC,cofile);
	
	char a[50];
	const char *buf = randstr(a,50,'a');
	const char *buf2 = randstr(a,50,'b');
	const char *buf3 = randstr(a,50,'c');

	write(fd,buf,50);
	write(fd,buf2,50);
	write(fd,buf3,50);
	
*/
	mutex = PTHREAD_MUTEX_INITIALIZER;
	pthread_t p1;
    pthread_t p2;
	pthread_t p3;
	pthread_t p4;
	pthread_t p5;
	int i,ret;
	//创建子线程，线程id为pId
	char p1c = 'a';
	char * p1s = &p1c;
	ret = pthread_create(&p1,NULL,test,p1s);
	
	char p2c = 'b';
	char * p2s = &p2c;
    ret = pthread_create(&p2,NULL,test,p2s);

		char p3c = 'c';
	char * p3s = &p3c;
	ret = pthread_create(&p3,NULL,test,p3s);

		char p4c = 'd';
	char * p4s = &p4c;
	ret = pthread_create(&p4,NULL,test,p4s);

		char p5c = 'e';
	char * p5s = &p5c;
	ret = pthread_create(&p5,NULL,test,p5s);

	pthread_t p6;
		char p6c = 'e';
	char * p6s = &p6c;
	ret = pthread_create(&p6,NULL,test,p6s);

	pthread_t p7;
		char p7c = 'e';
	char * p7s = &p7c;
	ret = pthread_create(&p7,NULL,test,p7s);

pthread_t p8;
		char p8c = 'e';
	char * p8s = &p8c;
	ret = pthread_create(&p8,NULL,test,p8s);



	if(ret != 0)
	{
		printf("create pthread error!\n");
		exit(1);
	}


	
	printf("main thread will exit when pthread is over\n");
	//等待线程pId的完成
	pthread_join(p1,NULL);
	pthread_join(p2,NULL);
	pthread_join(p3,NULL);
	pthread_join(p4,NULL);
	pthread_join(p6,NULL);
pthread_join(p7,NULL);
	pthread_join(p8,NULL);
	pthread_join(p5,NULL);
	printf("main thread  exit\n");

	return 0;

}

