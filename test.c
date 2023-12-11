#include <mrfstr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE (1024 * 1024 * 256)
#define COUNT 20

int main()
{
    mrfstr_t a = mrfstr_init();
    mrfstr_alloc(a, 2 * SIZE + 1);
    mrfstr_repeat_chr(a, 'Y', SIZE);

    char *s = malloc(2 * SIZE + 1);
    memset(s, 'Y', 2 * SIZE);
    s[2 * SIZE] = '\0';

    double total = 0;
    for (int i = 0; i < COUNT; i++)
    {
        clock_t tm = clock();
        mrfstr_concat(a, a, a);
        tm = clock() - tm;

        total += tm;
        printf("Test%llu: %lf seconds, %llu\n", i + 1, (double)tm / CLOCKS_PER_SEC,
            mrfstr_equal_nstr(a, s, 2 * SIZE));

        a->size = SIZE;
    }

    mrfstr_free(a);
    free(s);

    printf("Average: %lf seconds\n", (double)total / COUNT / CLOCKS_PER_SEC);
    return 0;
}
