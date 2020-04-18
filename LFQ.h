
#include <atomic>
#include<sys/types.h>

typedef struct task_s{
    task_s(int _fd,const void *_buf,size_t _nbytes){
        fd = _fd;
        buf = _buf;
        nbytes = _nbytes;
    };
    task_s(){};
    int fd;
    const void *buf;
    size_t nbytes;

}task_t;

typedef struct record_s{
    record_s(){
       next = new std::atomic_intptr_t(0L);
    };
    task_t task;
    std::atomic_intptr_t * next;
}record_t;




class LFQueue{

public:
LFQueue();
int Enqueue(record_t * r);
task_t * Dequeue();



private:
std::atomic_intptr_t *head;
std::atomic_intptr_t *tail;

};

