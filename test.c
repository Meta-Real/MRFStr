#include <stdio.h>
#include <mrtstr.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE 1073741824

int main()
{
    mrtstr_init_threads(7);

    mrtstr_t a = mrtstr_init();
    mrtstr_alloc(a, SIZE + 1);
    mrtstr_repeat_chr(a, 'K', SIZE);

    for (; mrtstr_locked(a););

    mrtstr_bres_t r[100];

    mrtstr_size_t s = 0;
    clock_t t;
    for (int i = 0; i < 100; i++)
    {
        t = clock();
        mrtstr_contain_chr(r + i, a, 'W');
        t = clock() - t;
        s += t;

        printf("%ld\n", t);
    }

    printf("%lf\n", s / 100.0);

    mrtstr_free(a);
    mrtstr_free_threads();
    printf("END\n");
    return 0;
}
