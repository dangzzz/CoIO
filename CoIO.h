
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
#define WRITE (1U<<31)
#define QUEUE (2U<<30)
#define MASK (3U<<30)


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
        std::atomic_store(state,(unsigned int)0);
    };
    std::atomic_uint32_t * state;
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

extern bool co_write(int fd, const void *buf, size_t nbytes,co_file_t * cofile){
    
    
    unsigned int exp = std::atomic_load(cofile->state);
    while(1){
        if(exp == FREE){
            if(std::atomic_compare_exchange_weak(cofile->state,&exp,WRITE)==true){
                write(fd,buf,nbytes);
                do{
                    exp=WRITE;
                }while(std::atomic_compare_exchange_weak(cofile->state,&exp,QUEUE)!=true);
                task_t* task = cofile->lfq->Dequeue();
                if(task==nullptr){
                    std::atomic_store(cofile->state,FREE);
                    return true;
                }else{
                    do{
                        std::atomic_store(cofile->state,WRITE);
                        write(task->fd,task->buf,task->nbytes);
                        do{
                            exp=WRITE;
                        }while(std::atomic_compare_exchange_weak(cofile->state,&exp,QUEUE)!=true);
                        task = cofile->lfq->Dequeue();
                    }while(task!=nullptr);
                    std::atomic_store(cofile->state,FREE);
                    return true;
                }
                


            }else{
                continue;
            }
            
        }else if((exp & MASK)==WRITE){
            if(std::atomic_compare_exchange_weak(cofile->state,&exp,exp+1)==true){
                record_t * r=&record_t();
                task_t t = task_t(fd,buf,nbytes);
                r->task = &t;
                cofile->lfq->Enqueue(r);

                std::atomic_fetch_sub(cofile->state,1U);
                return true;
            }else{
                continue;
            }

        }else if((exp & MASK)==QUEUE){
            continue;
        }else{
            return false;
        }






    }
}





