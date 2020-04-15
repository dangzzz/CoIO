
#include <atomic>


typedef struct task_s{

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

extern record_t* HeadRecord(){
    return &record_t();
}




class LFQueue{

public:
LFQueue();
int Enqueue(record_t * r);
task_t * Dequeue();



private:
std::atomic_intptr_t *head;
std::atomic_intptr_t *tail;

}

