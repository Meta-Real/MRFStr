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

    char *b = malloc(SIZE + 1);
    for (mrfstr_size_t i = 0; i < SIZE; i++)
        if (b[i] == '\0')
            b[i] = 'Q';
    b[SIZE] = '\0';

    mrfstr_set_nstr(a, b, SIZE);

    double t = 0;
    double c;
    for (uint64_t i = 0; i < COUNT; i++)
    {
        struct timeval s, e;

        mingw_gettimeofday(&s, NULL);
        mrfstr_reverse(a, a);
        mingw_gettimeofday(&e, NULL);

        c = (e.tv_sec - s.tv_sec) * 1000 + (e.tv_usec - s.tv_usec) / 1000.0;
        t += c;
        printf("Test%llu: %lf milliseconds ", i, c);

        mingw_gettimeofday(&s, NULL);
        strrev(b);
        mingw_gettimeofday(&e, NULL);

        c = (e.tv_sec - s.tv_sec) * 1000 + (e.tv_usec - s.tv_usec) / 1000.0;
        printf("vs %lf milliseconds, ", c);

        printf("cmp result: %d\n", memcmp(a->data, b, SIZE));
    }

    printf("Average MRFStr: %lf milliseconds\n", t / COUNT);
    mrfstr_free(a);
    free(b);
    return 0;
}
