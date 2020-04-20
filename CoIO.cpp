
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
       //printf("2");
        //printf(".%u.",exp);
        if(exp == FREE){
            if(std::atomic_compare_exchange_weak(cofile->state,&exp,WRITE)==true){
                
                write(fd,buf,nbytes);
                fsync(fd);
                do{
                     //printf(".%u.",exp);
                    exp=WRITE;
                 //printf("1");
                
                }while(std::atomic_compare_exchange_weak(cofile->state,&exp,QUEUE)!=true);
                
                task_t* task = cofile->lfq->Dequeue();
                
                if(task==nullptr){
                    std::atomic_store(cofile->state,FREE);
                    return true;
                }else{
                    do{
                        //printf("3");
                        std::atomic_store(cofile->state,WRITE);
                        write(fd,task->buf,task->nbytes);
                        fsync(fd);
                 
                        do{
                            
                             
                           // printf("4");
                           // printf(".%u.",exp);
                            exp=WRITE;
                            //printf(".%u.",exp);
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


ssize_t co_read(int fd, void *buf, size_t nbytes,co_file_t * cofile,char pc){
    unsigned int exp = std::atomic_load(cofile->state);
    while(1){
        
        if(exp==FREE){
            if(std::atomic_compare_exchange_weak(cofile->state,&exp,READ+1)){
               // printf("%c:FREE to READ+1\n",pc);
                break;
            }
            else
                continue;   
        }else if ((exp & MASK)==READ){
            if(std::atomic_compare_exchange_weak(cofile->state,&exp,exp+1)){
                //printf("%c:%u to +1\n",pc,exp);
                break;  

            }
                
            else
                continue;
        }else{
            exp = std::atomic_load(cofile->state);
            continue;
        }
    }

    auto ret = read(fd,buf,nbytes);

    std::atomic_fetch_sub(cofile->state,1U);
    //printf("%c:sub 1\n",pc);
    exp = READ;

    int a =std::atomic_compare_exchange_strong(cofile->state,&exp,FREE);
    //printf("%c:isisisis %u   %d\n",pc,exp,a);
    return ret;
    
}


