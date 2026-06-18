#include <stdlib.h>
#include <pthread.h>
#include "array.h"

// global parallel computation configuration
static ParallelConfig parallel_config = {
    .num_threads = 4,      // default: use 4 threads
    .enable_parallel = 1   // default: enable parallel computation
};

// set parallel computation configuration
void set_parallel_config(int num_threads, int enable_parallel) {
    parallel_config.num_threads = num_threads > 0 ? num_threads : 1;
    parallel_config.enable_parallel = enable_parallel;
}

// get parallel computation configuration
ParallelConfig* get_parallel_config() {
    return &parallel_config;
}

// thread task structure
typedef struct {
    void *data;
    int start;
    int end;
    void (*func)(void*, int, int);
} ThreadTask;

// thread function
static void* thread_func(void *arg) {
    ThreadTask *task = (ThreadTask*)arg;
    task->func(task->data, task->start, task->end);
    return NULL;
}

// execute tasks in parallel
void parallel_execute(void *data, int size, void (*func)(void*, int, int)) {
    ParallelConfig *config = get_parallel_config();

    // if parallel computation is not enabled or data size is too small, execute serially
    if (!config->enable_parallel || size < 1000 || config->num_threads <= 1) {
        func(data, 0, size - 1);
        return;
    }

    // create threads
    int num_threads = config->num_threads;
    pthread_t *threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    ThreadTask *tasks = (ThreadTask*)malloc(num_threads * sizeof(ThreadTask));

    if (threads == NULL || tasks == NULL) {
        // memory allocation failed, fall back to serial execution
        func(data, 0, size - 1);
        free(threads);
        free(tasks);
        return;
    }

    // compute per-thread data range
    int chunk_size = size / num_threads;
    int remainder = size % num_threads;

    // create and start threads
    for (int i = 0; i < num_threads; i++) {
        tasks[i].data = data;
        tasks[i].start = i * chunk_size + (i < remainder ? i : remainder);
        tasks[i].end = tasks[i].start + chunk_size + (i < remainder ? 1 : 0) - 1;
        tasks[i].func = func;

        if (pthread_create(&threads[i], NULL, thread_func, &tasks[i]) != 0) {
            // thread creation failed, fall back to serial execution
            for (int j = 0; j < i; j++) {
                pthread_join(threads[j], NULL);
            }
            func(data, 0, size - 1);
            free(threads);
            free(tasks);
            return;
        }
    }

    // wait for all threads to complete
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    // free resources
    free(threads);
    free(tasks);
}