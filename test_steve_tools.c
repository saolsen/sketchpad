// I want to learn iocp, write some iocp code (and port it to also work with epoll on linux).
// I want a really great priority queue multithreaded task system thingy.
// I need some reasons to write either of those tho. A problem to solve.

#include "steves_tools.h"

typedef struct {
    float p[3];
    u32 follys;
} Tester;

int main() {
    logInfo("Hello Steve Tools wow");

    logInfo("size of my struct is %lu", sizeof(STBuf));

    Tester *testers = NULL;
    for (int i=0; i<10; i++) {
        Tester t = (Tester){.p={1,2,3}, .follys=i};
        stbuf_push(testers, t);
    }

    Tester *more_testers = stbuf_add(testers, 10);
    for (int i=0; i<10; i++) {
        more_testers[i] = (Tester){.p={1,2,3}, .follys=i*2};
    }

    for (int i=0; i<20; i++) {
        logInfo("thingy is %i", testers[i].follys);
    }

    logInfo("length is %" PRIu64, stbuf_len(testers));
    logInfo("capacity is %" PRIu64, stbuf_cap(testers));

    stbuf_free(testers);

    return 0;
}
