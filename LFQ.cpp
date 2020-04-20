#include "LFQ.h"


LFQueue::LFQueue(){
    head = new std::atomic_intptr_t(0L);
    tail = new std::atomic_intptr_t(0L);
    std::atomic_store(head,(intptr_t)new record_t());
    std::atomic_store(tail,std::atomic_load(head));

}




int LFQueue::Enqueue(record_t * r){
    intptr_t null= (intptr_t)0;
    record_t * p;
    
    do{
        null = (intptr_t)0;
        p = (record_t *)std::atomic_load(tail); //取链表尾指针的快照
    }while( std::atomic_compare_exchange_weak(p->next,&null,(intptr_t)r)); //如果没有把结点链上，再试
    
   std::atomic_store(tail,(intptr_t)r);
    //std::atomic_compare_exchange_strong(tail,(intptr_t* )&p,(intptr_t)r);
}




task_t * LFQueue::Dequeue()//出队列
{
    record_t * p = (record_t *)std::atomic_load(head);;
    do{
       
        if(std::atomic_load(p->next) == (intptr_t)0L){
            return nullptr;
        }
    }
    while( std::atomic_compare_exchange_weak(head,(intptr_t*)&p,std::atomic_load(p->next)) != true );

    return &(((record_t*)std::atomic_load(p->next))->task);
}