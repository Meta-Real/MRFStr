#include <mrfstr.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define SIZE (1024 * 1024 * 1024)
#define COUNT 100

int main()
{
    mrfstr_t a = mrfstr_init();
    mrfstr_alloc(a, SIZE);
    mrfstr_repeat_chr(a, '8', SIZE);

    double t = 0;
    double c;
    for (uint64_t i = 0; i < COUNT; i++)
    {
        struct timeval s;
        mingw_gettimeofday(&s, NULL);

        mrfstr_replace(a, a, '8', '1');

        struct timeval e;
        mingw_gettimeofday(&e, NULL);

        c = (e.tv_sec - s.tv_sec) * 1000 + (e.tv_usec - s.tv_usec) / 1000.0;
        t += c;
        printf("Test%llu: %lf milliseconds\n", i, c);

        mrfstr_replace(a, a, '1', '8');
    }

    printf("Average MRFStr: %lf milliseconds\n", t / COUNT);
    mrfstr_free(a);
    return 0;
}
