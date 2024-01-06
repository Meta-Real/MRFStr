#include <mrfstr.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define SIZE (1024 * 1024 * 1024)
#define COUNT 100

int main(void)
{
    mrfstr_config_tcount(5);
    mrfstr_config_strlen(MRFSTR_CONFIG_SIMD_NONE);

    mrfstr_t a = mrfstr_init();
    mrfstr_alloc(a, SIZE + 1);
    mrfstr_repeat_chr(a, '0', SIZE);
    a->data[SIZE] = '\0';

    double t = 0;
    clock_t c;
    for (int i = 0; i != COUNT; i++)
    {
        c = clock();
        mrfstr_strlen(a->data);
        c = clock() - c;

        t += c;
        printf("%ld msc\n", c);
    }

    printf("AVG: %lf msc\n", t / COUNT);

    mrfstr_free(a);
    return 0;
}
