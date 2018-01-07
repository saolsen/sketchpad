#include "steve.h"
// @TODO: Please steve, write a really great work queue (work stealing? hmm?) and use it for lots of stuff.

#include "minitrace.h"

// @TODO: Windows
// OSX
#include <libkern/OSAtomic.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

// storeRelease(ptr, value)
// set ptr to value and require that all earlier writes(stores) are not moved after tzhis.
// This is casey's CompletePastWritesBeforeFutureWrites, before he does the store.
// __WriteBarrier() in vs
// __store_fence for the processor

// loadAquire(ptr)
// means all earlier reads(loads) are not moved before this.
// This is casey's CompletePastReadsBeforeFutureReads, before he does the store.
// __ReadBarrier() in vs
// __load_fence for the processor


// @NOTE: Wanna do a windows only experiemnt later, iocompletion ports and createthread for stuff.

// @TODO: Which handmade hero episode do I watch for this?
// Gonna start with what casey does then try to learn about a work stealing version.
// This is my current learning project, I also do really want to learn some graphics stuff with opengl.

// This is the sort of thing that is suppossed to be way easier to get right in rust, and I do believe that's probably
// true.

// locked exchange
// - replace a location in memory with a new value and ensure nobody else can do that at the same time.
// 
// interlocked compare exchange
// - this is the main one for doing a bunch of work to figure out what to do.
// watch the multithreaded simulation stuff next.
// I would like to learn raw opengl from casey too, do that.

// How to do a pushbuffer multithreaded?
// Just use the increment thing?
// if you want a multithreaded push buffer to be processed later you should reserve a cache line at a time per thread.
// that way their pushes don't invalidate other threads work.

// How do you check the size of the cache line?

typedef struct {
    char *string_to_print;
} WorkQueueEntry;

// @NOTE: Volatile tells the compiler that this value can change from non local stuff so the compiler is forced to reload it every time
// it wants to use it and can't just keep in in a register. "Don't optimize out the loads of this thing."
// The compiler might also insert the write barrior to all writes to volatile variables.
static i32 volatile next_entry_to_do;
static i32 volatile entry_count;
static WorkQueueEntry entries[256];

typedef struct {
    int thread_id;
} ThreadInfo;

void *worker_fn(void* param)
{
    ThreadInfo *thread_info = (ThreadInfo*)param;
    int thread_id = thread_info->thread_id;
    
    char buf[32];
    snprintf(buf, 32, "worker thread %i", thread_id);
    MTR_META_THREAD_NAME(buf);
    
    MTR_BEGIN("worker", "setup");
    logInfo("I am thread %i", thread_id);
    MTR_END("worker", "setup");

    for (int tick=0;tick<100000;tick++) {
        if (next_entry_to_do < entry_count) {
            MTR_BEGIN("worker", "print string");

            // @ This line is not interlocked so two threads can see the same value.
            // @ The compiler doesn't know that multiple threads could write this value.
            int entry_index = OSAtomicIncrement32Barrier(&next_entry_to_do) - 1; // InterlockedIncrement() in windows.
            
            // could use the barrior version of atomic increment and not need the loadAquire.
            // def safer to do that. (I think?)
            WorkQueueEntry *entry = entries + entry_index;
            
            logInfo("Thread %i: %s", thread_id, entry->string_to_print);
            MTR_END("worker", "print string");
        }
    }

    return NULL;
}

// on x64 you don't need fences, you need the barriors for the compiler but mov instructions are guaranteed to be in order.
// this means aquire and release are FREE on x64, don't add a fence when you don't need one sucka.
// I should probably work on somthing way more low level.

void pushString(char *s)
{
    assert(entry_count < LEN(entries));
    // @TODO: These writes are not in order.
    WorkQueueEntry *entry = entries + entry_count;
    entry->string_to_print = s;

    // write all before here.


    ++entry_count;
}

int main()
{
    mtr_init("work_queue_trace.json");
    MTR_META_PROCESS_NAME("work_queue");
    MTR_META_THREAD_NAME("main thread");

    MTR_BEGIN("main", "setup");
    
    i64 num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    // cache line size is useful for avoiding contention between threads, want each thread to have exclusive
    // access to full cache lines so there's no syncronization.
    i64 cache_line_size = 64; // could be 128 also, maybe that's a better number to use? Dunno how to query it.

    // 32 threads max
    assert(num_cores <= 32);
    pthread_t threads[32];
    ThreadInfo thread_infos[32];
    i32 num_threads = num_cores - 1;

    pushString("String 0\n");
    pushString("String 1\n");
    pushString("String 2\n");
    pushString("String 3\n");
    pushString("String 4\n");
    pushString("String 5\n");
    pushString("String 6\n");
    pushString("String 7\n");
    pushString("String 8\n");
    pushString("String 9\n");
    pushString("String 10\n");
    pushString("String 11\n");
    pushString("String 12\n");
    pushString("String 13\n");
    pushString("String 14\n");
    pushString("String 15\n");
    pushString("String 16\n");
    pushString("String 17\n");
    pushString("String 18\n");
    pushString("String 19\n");
    pushString("String 20\n");
    pushString("String 21\n");
    pushString("String 22\n");
    pushString("String 23\n");
    pushString("String 24\n");
    pushString("String 25\n");
    pushString("String 26\n");
    pushString("String 27\n");
    pushString("String 28\n");
    pushString("String 29\n");

    for (int i=0; i<num_threads; i++) {
        thread_infos[i].thread_id = i;
        pthread_create(&threads[i], NULL, worker_fn, &thread_infos[i]);
    }

    MTR_END("main", "setup");
    
    for (int i=0; i<num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    mtr_flush();
    mtr_shutdown();
    return 0;
}