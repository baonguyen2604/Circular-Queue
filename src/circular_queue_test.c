#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>
#include "circular_queue.h"

struct data_element {
    int x;
    int y;
};

/*
 * @return:
 * 1 if userArg > ptr
 * 0 if userArg == ptr
 * -1 if userArg < ptr
 */
int data_compare(void *userArg, void *ptr) {
    struct data_element *data1 = (struct data_element *) userArg;
    struct data_element *data2 = (struct data_element *) ptr;
    
    if (data1->x == data2->x) {
        if (data1->y == data2->y) {
            return 0;
        }
        return data1->y < data2->y ? -1 : 1;
    }
    return data1->x < data2->x ? -1 : 1;
}

struct data_element *random_element() {
    struct data_element *element = (struct data_element *) malloc(sizeof(struct data_element));
    element->x = rand();
    element->y = rand();
    return element;
}

void validate_element(struct data_element *element) {
    assert(element->x >= 0 && element->x < RAND_MAX);
    assert(element->y >= 0 && element->y < RAND_MAX);
}

void *producer_thread(void *args) {
    struct Queue *queue = (struct Queue *) args;
    void *data = (void *) random_element();
    for (int i = 0; i < 100; ++i) {
        int full = Queue_full(queue);
        int ret = Queue_add(queue, data);
        assert(ret != -1);
        assert(full != -1);
    }

    return (void *)0;
}

void *consumer_thread(void *args) {
    struct Queue *queue = (struct Queue *) args;
    for (int i = 0; i < 50; ++i) {
        struct data_element *data_element = (struct data_element *) Queue_remove(queue);
        if (data_element != NULL) {
            validate_element(data_element);
        }
    }

    return (void *)0;
}

void test_concurrent_modify(struct Queue *queue, const size_t producers_num, const size_t consumers_num) {
    printf("\n=== Testing concurrent modification of queue ===\n");
    printf("Creating %lu producer threads and %lu consumer threads\n", producers_num, consumers_num);
    pthread_t producers[producers_num];
    pthread_t consumers[consumers_num];

    for (size_t i = 0; i < producers_num; ++i) {
        pthread_create(&producers[i], NULL, producer_thread, (void *) queue);
    }

    for (size_t i = 0; i < consumers_num; ++i) {
        pthread_create(&consumers[i], NULL, consumer_thread, (void *) queue);
    }

    for (size_t i = 0; i < producers_num; ++i) {
        pthread_join(producers[i], NULL);
    }

    for (size_t i = 0; i < consumers_num; ++i) {
        pthread_join(consumers[i], NULL);
    }

    printf("Test successed\n");
}

void test_find_peek(struct Queue *queue) {
    printf("\n=== Testing peeking and finding queue datas ===\n");
    for (int i = 0; i < 10; ++i) {
        struct data_element *elem = (struct data_element *) malloc(sizeof(struct data_element));
        elem->x = (i + 1) * 10;
        elem->y = (i + 1) * 20;
        void *data = (void *) elem;
        int ret = Queue_add(queue, data);
        assert(ret != -1);
    }

    void *head = Queue_peek_head(queue);
    void *find_head = Queue_find(queue, &data_compare, head);
    assert(head != NULL);
    assert(head == find_head);

    void *tail = Queue_peek_tail(queue);
    void *find_tail = Queue_find(queue, &data_compare, tail);
    assert(tail != NULL);
    assert(tail == find_tail);


    struct data_element *test = (struct data_element *) malloc(sizeof(struct data_element));
    struct data_element *head_data = (struct data_element *) head;
    struct data_element *tail_data = (struct data_element *) tail;
    test->x = head_data->x - 1;
    test->y = head_data->y + 1;

    void *find_test = Queue_find(queue, &data_compare, test);
    assert(find_test == NULL);

    assert(head_data->x == 10);
    assert(head_data->y == 20);
    assert(tail_data->x == 100);
    assert(tail_data->y == 200);

    printf("Test successed\n");
}

void test_size_empty_full_cap(struct Queue *queue, const size_t cap) {
    printf("\n=== Testing size, empty, full, and cap methods of queue ===\n");
    assert(Queue_cap(queue) == cap);
    assert(Queue_empty(queue));

    for (int i = 1; i <= cap; ++i) {
        void *data = (void *) random_element();
        int ret = Queue_add(queue, data);
        assert(ret != -1);
        assert(Queue_size(queue) == i);
    }

    assert(Queue_full(queue));

    for (int i = cap; i > 0; --i) {
        assert(Queue_size(queue) == i);
        void *data = Queue_remove(queue);
        assert(data != NULL);
        validate_element((struct data_element *) data);
    }

    assert(Queue_empty(queue));
    printf("Test successed\n");
}

int main(int argc, char **argv) {
    const size_t SIZE = 200;
    const size_t PRODUCERS = 1000;
    const size_t CONSUMERS = 2000;
    printf("=== C Circular Queue Test ===\n\n");

    struct Queue *queue = Queue_new(SIZE);
    printf("Buffer initialized with size of %ld\n", SIZE);
    test_concurrent_modify(queue, PRODUCERS, CONSUMERS);
    Queue_delete(queue);
    printf("Buffer destroyed\n");

    queue = Queue_new(SIZE);
    printf("\nBuffer initialized with size of %ld\n", SIZE);
    test_find_peek(queue);
    Queue_delete(queue);
    printf("Buffer destroyed\n");

    queue = Queue_new(SIZE);
    printf("\nBuffer initialized with size of %ld\n", SIZE);
    test_size_empty_full_cap(queue, SIZE);
    Queue_delete(queue);
    printf("Buffer destroyed\n");

    return 0;
}