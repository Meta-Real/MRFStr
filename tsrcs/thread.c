/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>

#define THREAD(i) mrtstr_threads.threads[i]

mrtstr_threads_t mrtstr_threads = {};

void *mrtstr_thread_main(void *args);

void mrtstr_init_threads(mrtstr_size_t size)
{
    mrtstr_threads.threads = __mrstr_alloc_una(size * sizeof(mrtstr_thread_t));
    for (mrtstr_size_t i = 0; i < size; i++)
    {
        THREAD(i).func = NULL;
        THREAD(i).open = MRTSTR_TRUE;
        pthread_create(&THREAD(i).id, NULL, mrtstr_thread_main, (void*)i);
    }

    mrtstr_threads.size = size;
    mrtstr_threads.free_threads = size;
    pthread_mutex_init(&mrtstr_threads.mutex, NULL);
}

void mrtstr_load_threads(void (*volatile func)(void*), void *args)
{
    for (; !mrtstr_threads.free_threads;);

    mrtstr_size_t i;
    for (i = 0; i < mrtstr_threads.size; i++)
        if (!THREAD(i).func)
        {
            mrtstr_lock_dec(mrtstr_threads.free_threads, &mrtstr_threads.mutex);

            THREAD(i).args = args;
            THREAD(i).func = func;
            return;
        }
}

void mrtstr_free_threads()
{
    mrtstr_size_t i;
    for (i = 0; i < mrtstr_threads.size; i++)
        THREAD(i).open = MRTSTR_FALSE;

    for (i = 0; i < mrtstr_threads.size; i++)
        pthread_join(THREAD(i).id, NULL);

    __mrstr_free_una(mrtstr_threads.threads);
}

void *mrtstr_thread_main(void *args)
{
    mrtstr_size_t id = (mrtstr_size_t)args;
    while (THREAD(id).open)
        if (THREAD(id).func)
        {
            THREAD(id).func(THREAD(id).args);
            THREAD(id).func = NULL;

            mrtstr_lock_inc(mrtstr_threads.free_threads, &mrtstr_threads.mutex);
        }

    return NULL;
}
