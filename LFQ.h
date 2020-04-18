
#include <atomic>
#include<sys/types.h>

typedef struct task_s{
    task_s(int _fd,const void *_buf,size_t _nbytes){
        fd = _fd;
        buf = _buf;
        nbytes = _nbytes;
    }
    int fd;
    const void *buf;
    size_t nbytes;

}task_t;

typedef struct record_s{
    record_s(){
       std::atomic_store(next,(intptr_t)0);
    };
    task_t* task;
    std::atomic_intptr_t * next;
}record_t;


extern record_t* NewRecord(){

}





class LFQueue{

public:
LFQueue();
int Enqueue(record_t * r);
task_t * Dequeue();



private:
std::atomic_intptr_t *head;
std::atomic_intptr_t *tail;

};

