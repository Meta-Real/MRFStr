#include <mrfstr.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define SIZE (1024 * 1024 * 1024ULL)
#define COUNT 100

int main(void)
{
    mrfstr_t a = mrfstr_init();
    mrfstr_alloc(a, SIZE);

    mrfstr_t b = mrfstr_init();
    mrfstr_alloc(b, SIZE);
    mrfstr_repeat_chr(b, 'q', SIZE);

    double t = 0;
    double c;
    for (uint64_t i = 0; i < COUNT; i++)
    {
        clock_t s = clock();
        mrfstr_set(a, b);
        c = (clock() - s) * 1000.0 / CLOCKS_PER_SEC;
        t += c;
        printf("Test%llu: %lf milliseconds\n", i, c);
    }

    printf("Average MRFStr: %lf milliseconds\n", t / COUNT);
    mrfstr_free(a);
    mrfstr_free(b);
    return 0;
}
