#include <assert.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include "circular_queue.h"

/*
 * To support concurrent read from multiple threads but only one thread can write,
 * we can use a POSIX read-write lock to achieve this type of synchronization
 */

struct Queue {
    void **data;                // array of pointers pointing to the real user data
    size_t capacity;            // capacity of the queue
    size_t head;                // position of the first data 
    size_t tail;                // position after the last data (i.e last data at index 5 -> tail = 6)
    size_t size;                // current size of queue
    pthread_rwlock_t rwlock;    // POSIX read-write lock
};

// Public APIs

// Circular queue constructor
struct Queue *Queue_new(size_t maxSize) {
    assert(maxSize);

    struct Queue *queue = (struct Queue *) malloc(sizeof(struct Queue));
    assert(queue);

    queue->data = (void **) malloc(sizeof(void *) * maxSize);
    queue->capacity = maxSize;
    queue->head = 0;
    queue->tail = 0;
    queue->size = 0;
    pthread_rwlock_init(&queue->rwlock, NULL);

    assert(Queue_empty(queue));

    return queue;
}

// Circular queue destructor
void Queue_delete(struct Queue *queue) {
    if (queue == NULL) {
        return;
    }

    assert(queue);
    pthread_rwlock_destroy(&queue->rwlock);
    free(queue->data);
    free(queue);
}

// Add an element to the end of queue
int Queue_add(struct Queue *queue, void *data) {
    if (queue == NULL) {
        return -1;
    }
    assert(queue);

    int ret = pthread_rwlock_wrlock(&queue->rwlock);

    if (ret == EDEADLK) {
        pthread_rwlock_unlock(&queue->rwlock);
        return -1;
    }

    assert(ret != EINVAL);

    if (queue->size == queue->capacity) {
        pthread_rwlock_unlock(&queue->rwlock);
        return 0;
    }

    queue->data[queue->tail] = data;
    queue->tail = (queue->tail + 1) % queue->capacity;
    ++queue->size;

    pthread_rwlock_unlock(&queue->rwlock);

    return 1;
}

// Remove element from start of queue
void *Queue_remove(struct Queue *queue) {
    assert(queue);

    int ret = pthread_rwlock_wrlock(&queue->rwlock);

    if (ret == EDEADLK || queue->size == 0) {
        pthread_rwlock_unlock(&queue->rwlock);
        return NULL;
    }

    assert(ret != EINVAL);

    void *data = queue->data[queue->head];
    queue->head = (queue->head + 1) % queue->capacity;
    --queue->size;

    pthread_rwlock_unlock(&queue->rwlock);

    return data;
}

void *Queue_find(struct Queue *queue, Queue_matchFn matchFn, void *userArg) {
    assert(queue);

    int ret = pthread_rwlock_rdlock(&queue->rwlock);

    if (ret == EDEADLK || queue->size == 0) {
        pthread_rwlock_unlock(&queue->rwlock);
        return NULL;
    }

    if (ret == EAGAIN) {
        return NULL;
    }

    assert(ret != EINVAL);

    for (size_t i = queue->head; i <= queue->tail; i = (i + 1) % queue->capacity) {
        if (matchFn(userArg, queue->data[i]) == 0) {
            pthread_rwlock_unlock(&queue->rwlock);
            return queue->data[i];
        }
    }

    pthread_rwlock_unlock(&queue->rwlock);

    return NULL;
}

// Additional utility APIs

// Check if a queue is empty, return 0 if it is and 1 otherwise
int Queue_empty(struct Queue *queue) {
    assert(queue);

    int ret = pthread_rwlock_rdlock(&queue->rwlock);

    if (ret == EDEADLK) {
        pthread_rwlock_unlock(&queue->rwlock);
        return -1;
    }

    if (ret == EAGAIN) {
        return -1;
    }

    assert(ret != EINVAL);

    if (queue->size == 0) {
        pthread_rwlock_unlock(&queue->rwlock);
        return 1;
    } else {
        pthread_rwlock_unlock(&queue->rwlock);
        return 0;
    }
}

// Check if a queue is full, return 0 if it is and 1 otherwise
int Queue_full(struct Queue *queue) {
    assert(queue);

    int ret = pthread_rwlock_rdlock(&queue->rwlock);

    if (ret == EDEADLK) {
        pthread_rwlock_unlock(&queue->rwlock);
        return -1;
    }

    if (ret == EAGAIN) {
        return -1;
    }

    assert(ret != EINVAL);

    if (queue->size == queue->capacity) {
        pthread_rwlock_unlock(&queue->rwlock);
        return 1;
    } else {
        pthread_rwlock_unlock(&queue->rwlock);
        return 0;
    }
}

// Get the current number of elements in the queue
int Queue_size(struct Queue *queue) {
    assert(queue);

    int ret = pthread_rwlock_rdlock(&queue->rwlock);

    if (ret == EDEADLK) {
        pthread_rwlock_unlock(&queue->rwlock);
        return -1;
    }

    if (ret == EAGAIN) {
        return -1;
    }

    assert(ret != EINVAL);

    int size = queue->size;

    pthread_rwlock_unlock(&queue->rwlock);

    return size;
}

// Get the capacity of the queue
int Queue_cap(struct Queue *queue) {
    assert(queue);

    int ret = pthread_rwlock_rdlock(&queue->rwlock);

    if (ret == EDEADLK) {
        pthread_rwlock_unlock(&queue->rwlock);
        return -1;
    }

    if (ret == EAGAIN) {
        return -1;
    }

    assert(ret != EINVAL);

    int cap = queue->capacity;

    pthread_rwlock_unlock(&queue->rwlock);

    return cap;
}

// Get current element at head of queue without removing it
void *Queue_peek_head(struct Queue *queue) {
    assert(queue);

    int ret = pthread_rwlock_rdlock(&queue->rwlock);

    if (ret == EDEADLK || queue->size == 0) {
        pthread_rwlock_unlock(&queue->rwlock);
        return NULL;
    }

    if (ret == EAGAIN) {
        return NULL;
    }

    assert(ret != EINVAL);

    void *data = queue->data[queue->head];

    pthread_rwlock_unlock(&queue->rwlock);

    return data;
}

// Get current element at tail of queue without removing it
void *Queue_peek_tail(struct Queue *queue) {
    assert(queue);

    int ret = pthread_rwlock_rdlock(&queue->rwlock);

    if (ret == EDEADLK || queue->size == 0) {
        pthread_rwlock_unlock(&queue->rwlock);
        return NULL;
    }

    if (ret == EAGAIN) {
        return NULL;
    }

    assert(ret != EINVAL);
    assert(queue->tail >= 1);

    void *data = queue->data[queue->tail - 1];

    pthread_rwlock_unlock(&queue->rwlock);

    return data;
}
