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
    //mrtstr_repeat_chr(a, 'K', SIZE);

    //for (; mrtstr_locked(a););

    //mrtstr_bres_t r[100];

    mrtstr_size_t s = 0, i;
    clock_t t;
    for (i = 0; i < 100;)
    {
        t = clock();
        mrtstr_repeat_chr(a, 'K', SIZE);
        //mrtstr_contain_chr(r + i, a, 'W');
        t = clock() - t;
        s += t;

        printf("test%llu: %ld milliseconds\n", ++i, t);
    }

    printf("avg time: %lf milliseconds\n", s / 100.0);

    for (; mrtstr_locked(a););

    mrtstr_size_t o = 0;

    t = clock();
    for (i = 0; i < a->size; i++)
        o += a->data[i];
    printf("sum: %llu, time: %ld milliseconds\n", o, clock() - t);

    mrtstr_free(a);
    mrtstr_free_threads();
    printf("END\n");
    return 0;
}
