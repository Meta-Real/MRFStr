#include <stdio.h>
#include <mrfstr.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE 1073741824

int main()
{
    char *st = _aligned_malloc(SIZE + 1, 64);
    memset(st, 'T', SIZE);
    st[SIZE] = '\0';

    mrfstr_t a = mrfstr_init();
    mrfstr_realloc(a, SIZE + 1);

    mrfstr_size_t s = 0;
    clock_t t;
    for (int i = 0; i < 100; i++)
    {
        t = clock();
        mrfstr_set_nstr(a, st, SIZE);
        t = clock() - t;
        s += t;

        printf("%ld\n", t);
    }

    printf("%lf\n", s / 100.0);

    mrfstr_clear(a);
    _aligned_free(st);
    return 0;
}
