// Some quick and dirty tools for writing small programs fast
// Ideas
// - A stretchy buffer style array thingy.
// - A hash table.
// - Some sorting stuff.
// - A way to read files easily.
#ifndef _steve_tools_h
#define _steve_tools_h

// Stretchy Buffer!
// An array, stores a count and length internally in the allocated memory.
// You can't really pass these in functions because push and add will update the
// passed in pointer so that they can realloc. No addresses are stable either.
// not what you want to use for entities or whatever.

// This is almost exactly the same as stb by sean barret.

#include "steve.h"

#pragma pack(1)
typedef struct {
    u64 length;
    u64 capacity;
} STBuf;

#define stbuf__c(buf) stbuf__raw(buf)->capacity
#define stbuf__l(buf) stbuf__raw(buf)->length
#define stbuf__raw(buf) ((STBuf*)buf-1)
#define stbuf__needgrow(buf,n)  (buf==NULL || stbuf__l(buf) + n >= stbuf__c(buf))
#define stbuf__maybegrow(buf,n) (stbuf__needgrow(buf,n) ? stbuf__grow(buf,n) : 0)
#define stbuf__grow(buf,n)      ((buf) = stbuf__growf((buf), (n), sizeof(*(buf))))

static void * stbuf__growf(void *buf, u32 num, u32 itemsize)
{
    u64 twox_count = buf ? 2*stbuf__c(buf) : 0;
    u64 min_needed = buf ? num + stbuf__l(buf) : num;
    u64 m = twox_count > min_needed ? twox_count : min_needed;
    STBuf *raw = (STBuf*)realloc(buf ? stbuf__raw(buf): NULL, itemsize * m + sizeof(STBuf));
    if (raw) {
        if (!buf) {
            raw->length = 0;
        }
        raw->capacity = m;
        return raw+1;
    } else {
        // OUT OF MEMORY OH NO!
        logError("Out of memory!");
        return NULL;
    }
}

// These are the functions you should actually use.
#define stbuf_free(buf)         ((buf) ? free((STBuf*)buf-1) : 0)
#define stbuf_len(buf)          ((buf) ? stbuf__l(buf) : 0)
#define stbuf_cap(buf)          ((buf) ? stbuf__c(buf) : 0)
#define stbuf_push(buf,val)     (stbuf__maybegrow(buf,1), (buf)[stbuf__l(buf)++] = (val))
#define stbuf_add(buf,n)        (stbuf__maybegrow(buf,n), stbuf__l(buf)+=(n), &(buf)[stbuf__l(buf)-(n)])

// I really want some good file stuff because that was the biggest thing preventing me from doing any of the advent of
// code stuff in c.

#endif