
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

struct cmp_str  
{  
    bool operator()(char const *a, char const *b)  
    {  
        return std::strcmp(a, b) < 0;  
    }  
}; 

typedef struct co_file_s{
    
    
    co_file_s(){
        *lfq = LFQueue();
        state.store(0);
    };
    std::atomic_int32_t state;
    LFQueue* lfq;

}co_file_t;

std::map<const char*, co_file_t *, cmp_str> co_place;  

pthread_rwlock_t cprwl; 

extern int co_open(const char *path, int oflags,co_file_t ** cofile_ptr){
    
    bool b=true;
    pthread_rwlock_rdlock(&cprwl);
    auto iter = co_place.find(path);
    if(iter==co_place.end()){
        b=false;
    }else{
        *cofile_ptr = iter->second;
    }
    pthread_rwlock_unlock(&cprwl);
    

    if(!b){
        pthread_rwlock_wrlock(&cprwl);
        *cofile_ptr = &co_file_t();
        if(co_place.find(path)==co_place.end()){
            co_place.insert(std::pair<const char*, co_file_t *>(path,*cofile_ptr ));
        }
        pthread_rwlock_unlock(&cprwl);
    }

    return open(path,oflags);
}

extern size_t co_write(int fildes, const void *buf, size_t nbytes,co_file_t * cofile){
    
    
    int temp_state = cofile->state.load();
    while(1){
        if(temp_state == 0){
            //CAS
            
        }






    }
}





