#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string>
#include"CoIO.h"

#include <sys/time.h>
#define TEST

typedef struct pdata_s{
	pdata_s(char pc1,long time1){
		pc =pc1;
		time = time1;
	}
	char pc;
	long time;
}pdata_t;

//线程函数

 pthread_mutex_t mutex[4]; 

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
	
	
	int fd[4] = {0,0,0,0};
	fd[0] = open("/novadir/1",O_RDWR|O_SYNC|O_APPEND|O_RSYNC);
	fd[1] = open("/novadir/2",O_RDWR|O_SYNC|O_APPEND|O_RSYNC);
	fd[2] = open("/novadir/3",O_RDWR|O_SYNC|O_APPEND|O_RSYNC);
	
	pdata_t * ps = (pdata_t*) arg;
	int size = 131072;
	char* a= new char[size];
	const char *buf = randstr(a,size,ps->pc);
	char* b =new char[size];
	
	srand( (unsigned)time( NULL ) ); 

#ifdef TEST
	struct timeval start, end;
	gettimeofday( &start, NULL );  
#endif
	for(int i =0 ;i<1000;i++){
		
		
		int random = rand()%10;
		int ranfile = rand()%3;
		if(random <= 6){
			
			pthread_mutex_lock(&mutex[ranfile]);
			//usleep(100U);
			write(fd[ranfile],buf,size);
			pthread_mutex_unlock(&mutex[ranfile]);
		}else{
			pthread_mutex_lock(&mutex[ranfile]);
			//usleep(100U);
			read(fd[ranfile],b,size);
			pthread_mutex_unlock(&mutex[ranfile]);
		}
	}

#ifdef TEST
	gettimeofday( &end, NULL );
	printf("%c::过去了%ld.%ld\n",ps->pc,end.tv_sec-start.tv_sec,end.tv_usec-start.tv_usec);
	ps->time = end.tv_sec*1000000+end.tv_usec-start.tv_sec*1000000-start.tv_usec;
	
#endif
}




void *test(void * arg)
{	co_file_t * c1,*c2,*c3,*c0;
	co_file_t * c[4];
	c[0] = c0;
	c[1] = c1;
	c[2] = c2;
	c[3] = c3;
	
	
	int fd[4] = {0,0,0,0};
	fd[0] = co_open("/novadir/1",O_RDWR|O_SYNC|O_APPEND|O_RSYNC,c[0]);
	fd[1] = co_open("/novadir/2",O_RDWR|O_SYNC|O_APPEND|O_RSYNC,c[1]);
	fd[2] = co_open("/novadir/3",O_RDWR|O_SYNC|O_APPEND|O_RSYNC,c[2]);

	pdata_t * ps = (pdata_t*) arg;
	int size = 131072;
	char* a= new char[size];
	const char *buf = randstr(a,size,ps->pc);
	char* b =new char[size];
	
	srand( (unsigned)time( NULL ) ); 

#ifdef TEST
	struct timeval start, end;
	gettimeofday( &start, NULL );  
#endif
	for(int i =0 ;i<1000;i++){
		
		
		int random = rand()%10;
		int ranfile = rand()%3;
		if(random <= 6){
			//printf("%c:%d write ranfile:%d\n",ps->pc,i,ranfile);
			co_write(fd[ranfile],buf,size,c[ranfile]);
		}else{
			//printf("%c:%d read ranfile:%d\n",ps->pc,i,ranfile);
			co_read(fd[ranfile],b,size,c[ranfile],ps->pc);
		}
	}

#ifdef TEST
	gettimeofday( &end, NULL );
	printf("%c::过去了%ld.%ld\n",ps->pc,end.tv_sec-start.tv_sec,end.tv_usec-start.tv_usec);
	ps->time = end.tv_sec*1000000+end.tv_usec-start.tv_sec*1000000-start.tv_usec;
	
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
	mutex[0] = PTHREAD_MUTEX_INITIALIZER;
	mutex[1] = PTHREAD_MUTEX_INITIALIZER;
	mutex[2] = PTHREAD_MUTEX_INITIALIZER;
	mutex[3] = PTHREAD_MUTEX_INITIALIZER;
	pthread_t p1;
    pthread_t p2;
	pthread_t p3;
	pthread_t p4;
	
	int i,ret;
	//创建子线程，线程id为pId
	pdata_t * p1s = new pdata_t('a',0);
	ret = pthread_create(&p1,NULL,test,p1s);
	
	pdata_t * p2s = new pdata_t('b',0);
    ret = pthread_create(&p2,NULL,test,p2s);

	pdata_t * p3s = new pdata_t('c',0);
	ret = pthread_create(&p3,NULL,test,p3s);

	pdata_t * p4s = new pdata_t('d',0);
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
	printf("sum time %ld\n",p1s->time+p2s->time+p3s->time+p4s->time);
	printf("main thread  exit\n");

	return 0;

}

