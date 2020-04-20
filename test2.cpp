#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include <sys/times.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

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

	int fd = open("/novadir/123",O_RDWR|O_SYNC|O_APPEND|O_RSYNC);
	char * ps = (char*) arg;
	char* a= new char[40960000];
	const char *buf = randstr(a,40960000,*ps);
	printf("%c",buf[5]);
	printf("%lld\n",(long long)buf);

#ifdef TEST
	struct tms* tm = new struct tms();
clock_t t1 = times(tm);
#endif
	pthread_mutex_lock(&mutex);
	write(fd,buf,409600000);
	pthread_mutex_unlock(&mutex);
	#ifdef TEST

	
clock_t t2 = times(tm);


printf("%s::过去了%ld\n",ps,t2-t1);
#endif
}



void *test(void * arg)
{
	co_file_t * c[4];
	int fd[4] = {0,0,0,0};
	fd[0] = co_open("/novadir/1",O_RDWR|O_SYNC|O_APPEND|O_RSYNC,c[0]);
	fd[1] = co_open("/novadir/2",O_RDWR|O_SYNC|O_APPEND|O_RSYNC,c[1]);
	fd[2] = co_open("/novadir/3",O_RDWR|O_SYNC|O_APPEND|O_RSYNC,c[2]);
	fd[3] = co_open("/novadir/4",O_RDWR|O_SYNC|O_APPEND|O_RSYNC,c[3]);
	char * ps = (char*) arg;
	int size = 131072;
	char* a= new char[size];
	const char *buf = randstr(a,size,*ps);
	char* b =new char[size];
	
	srand( (unsigned)time( NULL ) ); 

#ifdef TEST
	struct timeval start, end;
	gettimeofday( &start, NULL );  
#endif
	for(int i =0 ;i<500;i++){
		int random = rand()%10;
		int ranfile = rand()%4;
		if(random <= 10){
			co_write(fd[ranfile],buf,size,c[ranfile]);
		}else{
			co_read(fd[ranfile],b,size,c[ranfile]);
		}
	}

#ifdef TEST
	gettimeofday( &end, NULL );
	printf("%s::过去了%d.%d\n",ps,start.tv_sec-end.tv_sec,start.tv_usec-end.tv_usec);
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
	printf("main thread  exit\n");

	return 0;

}

