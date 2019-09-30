
#ifndef _CIRCULAR_QUEUE_H
#define _CIRCULAR_QUEUE_H

/* Required Public APIs */

/* Comparator function that compares 2 data arguments
 */
typedef int (*Queue_matchFn)(void *userArg, void *ptr);

/* Circular queue constructor
 * @params:
 *  - maxSize: maximum capacity of the queue
 * @return:
 *  - newly created empty queue
 */
struct Queue *Queue_new(size_t maxSize);

/* Circular queue destructor
 * Destroy and cleanup a circular queue
 * @param:
 *  - queue: queue to be destroyed
 * @return: void and the queue is destroyed
 */
void Queue_delete(struct Queue *queue);

/* Add an element to the end of queue
 * @param:
 *  - queue: pointer to target queue
 *  - data: pointer to data being added
 * @return:
 *  - 1 if element is successfully added
 *  - 0 if queue is full
 *  - -1 if any errors occured
 */
int Queue_add(struct Queue *queue, void *data);

/* Remove element from start of queue
 * @param:
 *  - queue: pointer to target queue
 * @returns:
 *  - NULL if queue is full or some errors occured
 *  - pointer to the data just removed from the queue if success
 */
void *Queue_remove(struct Queue *queue);

/* Find an element matching userArg in the queue
 * @param:
 *  - queue: pointer to target queue
 *  - matchFn: comparator function that takes two void * as arguments
 *  - userArg: pointer to any user-managed data
 * @return:
 *  - pointer to the found element if found
 *  - NULL if no such element exists in the queue
 */
void *Queue_find(struct Queue *queue, Queue_matchFn matchFn, void *userArg);


/* Additional utility APIs */

/* Check if a queue is empty
 * @params:
 *  - queue: pointer to target queue
 * @return:
 *  - 0 if queue is not empty
 *  - 1 if queue is empty
 *  - -1 if any error occured
 */
int Queue_empty(struct Queue *queue);

/* Check if a queue is full
 * @params:
 *  - queue: pointer to target queue
 * @return:
 *  - 0 if queue is not full
 *  - 1 if queue is full
 *  - -1 if any error occured
 */
int Queue_full(struct Queue *queue);

/* Get the current number of elements in the queue
 * @params:
 *  - queue: pointer to target queue
 * @return:
 *  - -1 if any error occured
 *  - current size of queue if success
 */
int Queue_size(struct Queue *queue);

/* Get the capacity of the queue
 * @params:
 *  - queue: pointer to target queue
 * @return:
 *  - -1 if any error occured
 *  - capacity of queue if success
 */
int Queue_cap(struct Queue *queue);

/* Get current element at head of queue without removing it
 * @params:
 *  - queue: pointer to target queue
 * @return:
 *  - NULL if queue is empty or some errors occurred
 *  - pointer to data at head if success
 */
void *Queue_peek_head(struct Queue *queue);

/* Get current element at tail of queue without removing it
 * @params:
 *  - queue: pointer to target queue
 * @return:
 *  - NULL if queue is empty or some errors occurred
 *  - pointer to data at tail if success
 */
void *Queue_peek_tail(struct Queue *queue);

#endif // _CIRCULAR_QUEUE_H