#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct Queue Queue;

#include <types.h>

Queue   *queue_new(unsigned len) ;
void     queue_destroy(Queue *q) ;

unsigned queue_length(Queue *q) ;
void     queue_enqueue(Queue *q, Vector *v) ;
Vector   queue_dequeue(Queue *q) ;
Vector   queue_pop(Queue *q) ;

#endif /* _QUEUE_H_ */

