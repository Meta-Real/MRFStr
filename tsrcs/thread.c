/*
    MRTStr Library version 1.0.0
    MetaReal Threaded String Library
*/

#include <mrtstr-intern.h>
#include <alloc.h>

#define THREAD(i) mrtstr_threads.threads[i]

void *mrtstr_thread_main(void *args);

mrtstr_res_enum_t mrtstr_init_threads(mrtstr_bit_t size)
{
    mrtstr_threads.threads = mrstr_alloc(size * sizeof(mrtstr_thread_t));
    if (!mrtstr_threads.threads)
        return MRTSTR_RES_MEM_ERROR;

    for (mrtstr_bit_t i = 0; i < size; i++)
    {
        THREAD(i).func = NULL;
        THREAD(i).open = MRTSTR_TRUE;
        if (pthread_create(&THREAD(i).id, NULL, mrtstr_thread_main, (void*)(uintptr_t)i))
        {
            while (i--)
            {
                THREAD(i).open = MRTSTR_FALSE;
                pthread_join(THREAD(i).id, NULL);
            }

            mrstr_free(mrtstr_threads.threads);
            return MRTSTR_RES_THREAD_ERROR;
        }
    }

    mrtstr_threads.size = size;
    mrtstr_threads.free_threads = size;

    if (pthread_mutex_init(&mrtstr_threads.mutex, NULL))
    {
        while (size--)
        {
            THREAD(size).open = MRTSTR_FALSE;
            pthread_join(THREAD(size).id, NULL);
        }

        mrstr_free(mrtstr_threads.threads);
        return MRTSTR_RES_THREAD_ERROR;
    }

    return MRTSTR_RES_NOERROR;
}

void mrtstr_load_threads(void (*func)(void*), void *args)
{
    for (; !mrtstr_threads.free_threads;);

    mrtstr_bit_t i;
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
    mrtstr_bit_t i;
    for (i = 0; i < mrtstr_threads.size; i++)
        THREAD(i).open = MRTSTR_FALSE;

    for (i = 0; i < mrtstr_threads.size; i++)
        pthread_join(THREAD(i).id, NULL);

    mrstr_free(mrtstr_threads.threads);

    pthread_mutex_lock(&mrtstr_threads.mutex);
    pthread_mutex_unlock(&mrtstr_threads.mutex);
    pthread_mutex_destroy(&mrtstr_threads.mutex);
}

void *mrtstr_thread_main(void *args)
{
    mrtstr_bit_t id = (mrtstr_bit_t)(uintptr_t)args;
    while (THREAD(id).open)
        if (THREAD(id).func)
        {
            THREAD(id).func(THREAD(id).args);
            THREAD(id).func = NULL;

            mrtstr_lock_inc(mrtstr_threads.free_threads, &mrtstr_threads.mutex);
        }

    return NULL;
}
