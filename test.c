#include <mrfstr.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE (1024 * 1024 * 1024)
#define COUNT 20

int main()
{
    mrfstr_t a = mrfstr_init();
    mrfstr_alloc(a, SIZE + 1);
    mrfstr_repeat_chr(a, 'Y', SIZE);
    //a->data[178956927] = 'K';
    //a->data[178956928] = 'K';

    double total = 0;
    for (int i = 0; i < COUNT; i++)
    {
        clock_t tm = clock();
        mrfstr_find_chr(a, 'K');
        tm = clock() - tm;

        total += tm;
        printf("Test%llu: %lf seconds\n", i + 1, (double)tm / CLOCKS_PER_SEC);

        a->size = SIZE;
    }

    mrfstr_free(a);

    printf("Average: %lf seconds\n", (double)total / COUNT / CLOCKS_PER_SEC);
    return 0;
}
