#include <mrfstr.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define SIZE (1024 * 1024 * 1024)
#define COUNT 100

int main(void)
{
    mrfstr_config_tcount(5);

    mrfstr_t a = mrfstr_init();
    mrfstr_alloc(a, SIZE);
    mrfstr_repeat_chr(a, '0', SIZE);

    mrfstr_t b = mrfstr_init();
    mrfstr_alloc(b, SIZE);

    double t = 0;
    clock_t c;
    for (int i = 0; i != COUNT; i++)
    {
        c = clock();
        mrfstr_replace_chr(a, a, '0', '1');
        c = clock() - c;
        mrfstr_replace_chr(a, a, '1', '0');

        t += c;
        printf("%ld msc\n", c);
    }

    printf("AVG: %lf msc\n", t / COUNT);

    mrfstr_free(a);
    mrfstr_free(b);
    return 0;
}
