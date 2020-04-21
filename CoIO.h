
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <map>
#include "LFQ.h"
#include <cstring>  
#include<pthread.h>

#define FREE 0U
#define WRITE (2U<<30)
#define QUEUE (1U<<30)
#define READ (3U<<30)
#define MASK (31U<<27)


struct cmp_str  
{  
    bool operator()(char const *a, char const *b)  
    {  
        return std::strcmp(a, b) < 0;  
    }  
}; 

typedef struct co_file_s{
    
    
    co_file_s(){
        lfq =new LFQueue();
        state = new std::atomic_uint32_t(0U);
    };
    std::atomic_uint32_t * state;
    LFQueue* lfq;

}co_file_t;

extern std::map<const char*, co_file_t *, cmp_str> co_place;  

extern pthread_rwlock_t cprwl; 

extern int co_open(const char *path, int oflags,co_file_t *& cofile_ptr);

extern bool co_write(int fd, const void *buf, size_t nbytes,co_file_t * cofile);

extern ssize_t co_read(int fd, void *buf, size_t nbytes,co_file_t * cofile,char pc);


extern int co_close(int fd,const char *path);


                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               