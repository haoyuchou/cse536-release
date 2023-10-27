/* CSE 536: User-Level Threading Library */
#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "user/ulthread.h"

/* Standard definitions */
#include <stdbool.h>
#include <stddef.h> 

#define MAXARG 8

struct ulthrea ulthread;
struct thread *current_thread;
void args_to_context(struct context_switch *context_swit, uint64 args[], uint64 args_length);
bool add_thread_to_ulthread(struct thread* threa);
uint64 retrieve_yield_ulthread(void);

struct thread* first_come_first_serve(void);
struct thread* Priority(void);

/* Get thread ID*/
uint64 get_current_tid() {
    return current_thread->id;
}

/* Thread initialization */
void ulthread_init(int schedalgo) {
    struct thread *new_thread = malloc(sizeof(struct thread));
    new_thread->id = 0;
    new_thread->priority = 0;
    memset(&new_thread->context_swit, 0, sizeof(new_thread->context_swit));
    
    // assign the first kernel-provided thread 
    // to be the user-level scheduler thread.
    // keep track of the self thread[ulthread]
    ulthread.threads[0] = new_thread;
    ulthread.next_id = 1;
    ulthread.size = 1;
    ulthread.schedule_algo = schedalgo;
}

/* Thread creation */
bool ulthread_create(uint64 start, uint64 stack, uint64 args[], int priority) {
    /* Please add thread-id instead of '0' here. */
    printf("[*] ultcreate(tid: %d, ra: %p, sp: %p)\n", ulthread.next_id, start, stack);

    struct thread *new_thread = malloc(sizeof(struct thread));
    
    new_thread->id = ulthread.next_id;
    new_thread->priority = priority;
    new_thread->state = RUNNABLE;
    
    // create and maintain context
    memset(&new_thread->context_swit, 0, sizeod(new_thread->context_swit));
    new_thread->context_swit.ra = start;
    new_thread->context_swit.sp = stack;

    uint64 args_length = strlen(args);

    if (args_length > MAXARG){
        args_length = MAXARG;
    }
    args_to_context(&new_thread->context_swit, args, args_length);
    
    if(!add_thread_to_ulthread(new_thread)){
        return false;
    }

    ulthread.size ++;
    ulthread.next_id ++;
    return false;
}

void args_to_context(struct context_switch *context_swit, uint64 args[], uint64 args_length){
    memmove(&context_swit->a0, args, args_length);
}

bool add_thread_to_ulthread(struct thread* threa){
    // keep track of the self thread[ulthread]
    if (ulthread.size < MAXULTHREADS){
        ulthread.threads[ulthread.size] = threa;
        return true;
    }
    // see if any thread in ulthread array is free
    for(int i = 1; i < MAXULTHREADS; i++){
        if (ulthread.threads[i]->state == FREE){
            ulthread.threads[i] = threa;
            return true;
        }
    }

    return false;
}

/* Thread scheduler */
void ulthread_schedule(void) {
    struct thread *next_thread;

    for(;;){
        switch (ulthread.schedule_algo){
            case FCFS:
                current_thread = first_come_first_serve();
                break;
            case PRIORITY:
                current_thread = Priority();
                break;
            default:
                return;
        }
        // make all yield thread runnable again
        uint64 recover_yield_count = retrieve_yield_ulthread();
        // if no next thread 
        // and there is nothing to be yield
        // just return from the schedulor
        if (!next_thread){
            if (recover_yield_count == 0){
                return;
            }
            continue;
        }

        current_thread = next_thread;
    

    
    /* Add this statement to denote which thread-id is being scheduled next */
    printf("[*] ultschedule (next tid: %d)\n", next_thread->id);

    // Switch betwee thread contexts
    // previous context and next context
    ulthread_context_switch(&ulthread.threads[0]->context_swit, &next_thread->context_swit);
    }
}

struct thread* first_come_first_serve(void){
    struct thread *return_thread = ulthread.threads[0];

    for (int i = 1; i < ulthread.size; i++){
        if (ulthread.threads[i]->state != RUNNABLE){
            continue;
        }

        if (return_thread->id == 0){
            return_thread = ulthread.threads[i];
        }
        
        // find smaller id
        if (return_thread->id > ulthread.threads[i]->id){
            return_thread = ulthread.threads[i];
        }
    }
    return return_thread;
}

struct thread* Priority(void){
    struct thread *return_thread = ulthread.threads[0];
    for (int i = 1; i < ulthread.size; i++){
        if (ulthread.threads[i]->state != RUNNABLE){
            continue;
        }

        if (return_thread->id == 0){
            return_thread = ulthread.threads[i];
        }
        
        // find higher priority
        if (return_thread->priority < ulthread.threads[i]->priority){
            return_thread = ulthread.threads[i];
        }
    }
    return return_thread;
}

uint64 retrieve_yield_ulthread(void){
    uint64 recover_yield_count = 0;
    for(int i = 1; i < ulthread.size; i++ ){
        if(ulthread.threads[i]->state == YIELD){
            ulthread.threads[i]->state = RUNNABLE;
            recover_yield_count += 1;
        }
    }

    return recover_yield_count;
}

/* Yield CPU time to some other thread. */
void ulthread_yield(void) {
    struct thread *yield_thread = current_thread;
    current_thread = ulthread.threads[0];
    yield_thread->state = YIELD;
    ulthread_context_switch(&yield_thread->context_swit, &ulthread.threads[0]->context_swit);
    /* Please add thread-id instead of '0' here. */
    printf("[*] ultyield(tid: %d)\n", yield_thread->id);
}

/* Destroy thread */
void ulthread_destroy(void) {
    struct thread *destroy_thread = current_thread;
    current_thread = ulthread.threads[0];
    destroy_thread->state = FREE;
    ulthread_context_switch(&destroy_thread->context_swit, &ulthread.threads[0]->context_swit);
    printf("[*] ultdestroy(tid: %d)\n", destroy_thread->id);
}