
#include "CoIO.h"
#include <iostream>

#include <sys/times.h>




pthread_rwlock_t cprwl; 

std::map<const char*, co_file_t *, cmp_str> co_place;  


int co_open(const char *path, int oflags,co_file_t *& cofile_ptr){
    
    bool b=true;
    pthread_rwlock_rdlock(&cprwl);
    auto iter = co_place.find(path);
    if(iter==co_place.end()){
        b=false;
    }else{
        cofile_ptr = iter->second;
    }
    pthread_rwlock_unlock(&cprwl);
    

    if(!b){
        pthread_rwlock_wrlock(&cprwl);

        cofile_ptr = new co_file_t();
        if(co_place.find(path)==co_place.end()){
            co_place.insert(std::pair<const char*, co_file_t *>(path,cofile_ptr ));
        }
        pthread_rwlock_unlock(&cprwl);
    }

    return open(path,oflags);
}

int co_close(int fd,const char *path){
    delete co_place.find(path)->second;
    pthread_rwlock_wrlock(&cprwl);
        co_place.erase(co_place.find(path));
    pthread_rwlock_unlock(&cprwl);

    return  close(fd);
}



bool co_write(int fd, const void *buf, size_t nbytes,co_file_t * cofile){

    unsigned int exp = std::atomic_load(cofile->state);
    while(1){
        if(exp == FREE){
            if(std::atomic_compare_exchange_weak(cofile->state,&exp,WRITE)==true){
                std::cout<<fd<<((char *)buf)[5]<<nbytes<<std::endl;
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
                         std::cout<<"other"<<fd<<((char *)(task->buf))[5]<<task->nbytes<<std::endl;
                         std::cout<<(long long)(task->buf)<<std::endl;
                        write(fd,task->buf,task->nbytes);
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
                record_t * r=new record_t();
                task_t* t =new task_t(fd,buf,nbytes);
                r->task = *t;
                cofile->lfq->Enqueue(r);

                std::atomic_fetch_sub(cofile->state,1U);
                return true;
            }else{
                continue;
            }

        }else if((exp & MASK)==QUEUE||(exp & MASK)==READ){
            exp = std::atomic_load(cofile->state);
            continue;
        }else{
            return false;
        }


    }
}


ssize_t co_read(int fd, void *buf, size_t nbytes,co_file_t * cofile){
    unsigned int exp = std::atomic_load(cofile->state);
    while(1){
        if(exp==FREE){
            if(std::atomic_compare_exchange_weak(cofile->state,&exp,READ+1))
                break;
            else
                continue;   
        }else if ((exp & MASK)==READ){
            if(std::atomic_compare_exchange_weak(cofile->state,&exp,exp+1))
                break;
            else
                continue;
        }else{
            exp = std::atomic_load(cofile->state);
            continue;
        }
    }

    auto ret = read(fd,buf,nbytes);

    std::atomic_fetch_sub(cofile->state,1U);
    exp == READ+1;
    std::atomic_compare_exchange_strong(cofile->state,&exp,0U);
    return ret;
    
}


