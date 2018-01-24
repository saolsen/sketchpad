// @TODO: Templated style stuff for this.
#include "steve.h"

// @TODO: This is sort of a copy paste, remember what all the variables mean and then pull this out.
// @TODO: Make this threadsafe and stuff!
typedef struct {
    int data[256];
    // int capacity;
    int head;
    int tail;
} Buf;

// Push element
int buf_q(Buf *buf, int e) {
    int next = buf->head + 1;
    if (next >= LEN(buf->data)) {
        next = 0;
    }
    if (next == buf->tail) {
        return -1; // queue is full
    }
    buf->data[buf->head] = e;
    buf->head = next;
    return 0;
}

int buf_dq(Buf *buf, int *e) {
    if (buf->head == buf->tail) {
        return 0; // queue is empty
    }
    int next = buf->tail + 1;
    if (next >= LEN(buf->data)) {
        next = 0;
    }
    *e = buf->data[buf->tail];
    buf->tail = next;
    return 1;
}
