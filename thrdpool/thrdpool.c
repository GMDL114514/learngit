#include "thrdpool.h"
static void* worker_thread(void* arg);
static int task_queue_init(TaskQueue* queue, int capacity) 
{
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
    queue->capacity = capacity;
    if (pthread_mutex_init(&queue->lock, NULL) != 0) {
        return -1;
    }

    if (pthread_cond_init(&queue->cond, NULL) != 0) {
        pthread_mutex_destroy(&queue->lock);
        return -1;
    }

    if (pthread_cond_init(&queue->not_full, NULL) != 0) {
        pthread_mutex_destroy(&queue->lock);
        pthread_cond_destroy(&queue->cond);
        return -1;
    }

    return 0;

}


ThreadPool* threadpool_create(int num_threads, int queue_capacity)
{
    if (num_threads <= 0 || queue_capacity <= 0) {
        return NULL;
    }
    ThreadPool* pool = (ThreadPool*)malloc(sizeof(ThreadPool));
    if (pool)
    {
        //初始化线程池基础字段
        pool->thread_count = num_threads;
        pool->active_count = 0;
        pool->shutdown = 0;
        pool->threads = NULL;
        pool->task_queue = NULL;
        //初始化线程池的锁和条件变量
        if (pthread_mutex_init(&pool->pool_lock, NULL) != 0) {
            free(pool);
            return NULL;
        }

        if (pthread_cond_init(&pool->all_idle, NULL) != 0) {
            pthread_mutex_destroy(&pool->pool_lock);
            free(pool);
            return NULL;
        }
        pool->task_queue = (TaskQueue*)malloc(sizeof(TaskQueue));
        if (pool->task_queue)
        {   //初始化任务队列
            if (task_queue_init(pool->task_queue, queue_capacity) != 0) {
                free(pool->task_queue);
                pthread_mutex_destroy(&pool->pool_lock);
                pthread_cond_destroy(&pool->all_idle);
                free(pool);
                return NULL;
            }
            pool->threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));//分配线程ID数组
            if (pool->threads == NULL) {
                free(pool->task_queue);
                pthread_mutex_destroy(&pool->pool_lock);
                pthread_cond_destroy(&pool->all_idle);
                free(pool);
                return NULL;
            }
            for (int i = 0; i < num_threads; i++) {
                if (pthread_create(&pool->threads[i], NULL, worker_thread, (void*)pool) != 0) 
                {
                    // 创建线程失败，需要清理已创建的线程和资源
                    pool->shutdown = 1;  // 设置关闭标志

                    // 唤醒所有可能正在等待的线程
                    pthread_mutex_lock(&pool->task_queue->lock);
                    pthread_cond_broadcast(&pool->task_queue->cond);
                    pthread_mutex_unlock(&pool->task_queue->lock);

                    // 等待已创建的线程退出
                    for (int j = 0; j < i; j++) {
                        pthread_join(pool->threads[j], NULL);
                    }

                    // 清理资源
                    free(pool->threads);
                    free(pool->task_queue);
                    pthread_mutex_destroy(&pool->pool_lock);
                    pthread_cond_destroy(&pool->all_idle);
                    free(pool);
                    return NULL;
                }
            }

            return pool;
        }
        pthread_mutex_destroy(&pool->pool_lock);
        pthread_cond_destroy(&pool->all_idle);
        free(pool);
        return NULL;


    }

}

static void* worker_thread(void* arg)
{
    ThreadPool* pool = (ThreadPool*)arg;
    while (1)
    {
        pthread_mutex_lock(&pool->task_queue->lock);
        while (pool->task_queue->size == 0 && !pool->shutdown) {//防止虚假唤醒
            pthread_cond_wait(&pool->task_queue->cond, &pool->task_queue->lock);
        }
        if (pool->shutdown && pool->task_queue->size == 0) //检查是否需要退出线程
        {
            pthread_mutex_unlock(&pool->task_queue->lock);
            break;
        }
        Task* task = pool->task_queue->head;
        if (task != NULL) {
            // 更新队列头部
            pool->task_queue->head = task->next;
            pool->task_queue->size--;
            
            // 如果队列为空，更新尾部指针
            if (pool->task_queue->head == NULL) {
                pool->task_queue->tail = NULL;
            }
            
            // 通知生产者线程：队列现在有空位了
            pthread_cond_signal(&pool->task_queue->not_full);
        }
        pthread_mutex_unlock(&pool->task_queue->lock);
        if (task != NULL) {
            // 更新活跃线程数（需要线程池锁）
            pthread_mutex_lock(&pool->pool_lock);
            pool->active_count++;
            pthread_mutex_unlock(&pool->pool_lock);
            
            // 执行任务函数
            task->function(task->arg);
            
            // 任务执行完毕，释放任务内存
            free(task);
            
            // 更新活跃线程数
            pthread_mutex_lock(&pool->pool_lock);
            pool->active_count--;
            
            // 如果所有线程都空闲了，通知可能等待的线程
            if (pool->active_count == 0) {
                pthread_cond_signal(&pool->all_idle);
            }
            pthread_mutex_unlock(&pool->pool_lock);
        }
    }
    
    // 线程退出
    pthread_exit(NULL);
    return NULL;
}
