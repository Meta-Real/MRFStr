#include <stdio.h>
#include <mrtstr.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE (1024 * 1024 * 1024)
#define COUNT 100

int main()
{
    mrtstr_init_threads(6);

    mrtstr_t a = mrtstr_init();
    mrtstr_alloc(a, SIZE + 1);

    mrtstr_bres_t r[COUNT];

    size_t t = 0;
    clock_t q = clock();
    for (mrtstr_bit_t i = 0; i < COUNT; i++)
    {
        clock_t o = clock();
        mrtstr_repeat_chr(a, 'W', SIZE);
        o = clock() - o;
        t += o;

        printf("%ld\n", o);
    }
    printf("%ld\n", clock() - q);

    printf("AVG: %lf\n", (double)t / COUNT);

    mrtstr_free(a);
    mrtstr_free_threads();
    return 0;
}
