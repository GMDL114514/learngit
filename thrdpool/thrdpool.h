#pragma
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>

//任务结构体
typedef struct Task{
    struct Task *next;//使用链表的队列
    void (*function)(void* arg);
    void *arg;

}Task;

typedef struct TaskQueue{
    Task *head;
    Task *tail;//听说这是常见的设计，但我不太了解
    int block;//默认的阻塞变量
    int size;
    int capacity;
    pthread_mutex_t lock; // 互斥锁保护队列
    pthread_cond_t cond; //表示线程运行或休眠的变量
    pthread_cond_t not_full;//通知队列未满
}TaskQueue;

typedef struct ThreadPool{
    TaskQueue *task_queue;
    int thread_count;      // 总线程数
    int active_count;      // 活跃线程数
    pthread_t *threads;//不懂
    pthread_mutex_t pool_lock;  // 保护线程池状态
    pthread_cond_t all_idle;    // 所有线程空闲的条件
    int shutdown;           // 关闭标志，0为运行，1为关闭
}ThreadPool;

int task_queue_init(TaskQueue* queue, int capacity);
ThreadPool* threadpool_create(int num_threads, int queue_capacity);


#endif // THREADPOOL_H