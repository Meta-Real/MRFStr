#include <mrfstr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE (1024 * 1024 * 1024)
#define COUNT 50

int main()
{
    mrfstr_t a = mrfstr_init();
    mrfstr_repeat_chr(a, 'e', SIZE);

    mrfstr_t b = mrfstr_init();
    mrfstr_alloc(b, SIZE + 1);

    double total = 0;
    for (int i = 0; i < COUNT; i++)
    {
        clock_t tm = clock();
        mrfstr_set(b, a);
        tm = clock() - tm;

        total += tm;
        printf("Test%llu: %lf seconds\n", i + 1, (double)tm / CLOCKS_PER_SEC);
    }

    printf("Average: %lf seconds\n", (double)total / COUNT / CLOCKS_PER_SEC);

    mrfstr_free(a);
    return 0;
}
