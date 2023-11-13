#include <stdio.h>
#include <mrtstr.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE 1000000000

void test_each(char *src);

void test_each(char *src)
{
    mrtstr_init_threads(7);

    mrtstr_t a = mrtstr_init();
    mrtstr_realloc(a, SIZE + 1);
    mrtstr_nset_str(a, src, SIZE);

    mrtstr_t b = mrtstr_init();
    b->alloc = SIZE + 1;
    b->size = SIZE;
    b->data = src;

    for (; MRTSTR_LOCKED(a););

    mrtstr_cmpres_t r[100];

    mrtstr_size_t s = 0;
    clock_t t;
    for (int i = 0; i < 100; i++)
    {
        t = clock();
        mrtstr_equal(r + i, a, b);
        t = clock() - t;
        s += t;

        _sleep(100);

        printf("%ld\n", t);
    }

    printf("%lf\n", s / 100.0);

    mrtstr_clear(a);
    mrtstr_clear(b);
    mrtstr_free_threads();
}

int main()
{
    char *s = _aligned_malloc(SIZE + 1, 64);
    memset(s, 'H', SIZE);
    s[SIZE] = '\0';

    test_each(s);
    return 0;
}
