#include <mrfstr.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define SIZE (1024 * 1024 * 1024)
#define COUNT 100

int main(void)
{
    mrfstr_t a = mrfstr_init();
    mrfstr_alloc(a, SIZE);
    mrfstr_repeat_chr(a, 'r', SIZE);

    //mrfstr_t b = mrfstr_init();
    //mrfstr_alloc(b, SIZE);

    size_t t = 0;
    clock_t c;
    for (int i = 0; i < COUNT; i++)
    {
        c = clock();
        c += memcmp(a->data, a->data, SIZE);
        //mrfstr_set(b, a);
        c = clock() - c;

        t += c;
        printf("Test%d: %ld msc\n", i, c);
    }

    printf("Avg: %lf msc\n", (double)t / COUNT);

    mrfstr_free(a);
    //mrfstr_free(b);
    return 0;
}
