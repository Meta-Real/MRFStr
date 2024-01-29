#include <mrfstr.h>
#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define SIZE (1024 * 1024 * 1024)
#define COUNT 10

int main(void)
{
    mrfstr_config_thread_count(6);

    mrfstr_t a = mrfstr_init();
    mrfstr_repeat_chr(a, '1', SIZE);
    a->_data[a->_size - 1] = '\0';

    LARGE_INTEGER s, e, t;
    t.QuadPart = 0;
    for (int i = 0; i < COUNT; i++)
    {
        QueryPerformanceCounter(&s);
        mrfstr_strlen(a->_data);
        QueryPerformanceCounter(&e);

        t.QuadPart += e.QuadPart - s.QuadPart;
    }

    printf("%lf milliseconds\n", t.QuadPart / (10000.0 * COUNT));

    mrfstr_free(a);
    return 0;
}
