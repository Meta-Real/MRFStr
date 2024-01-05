#include <mrfstr.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define SIZE (1024 * 1024 * 1024)
#define COUNT 100

int main(void)
{
    mrfstr_config_thread_count(5);
    mrfstr_config_cmp(MRFSTR_SIMD_CONFIG_NONE, MRFSTR_SIMD_CONFIG_SSE);

    mrfstr_t a = mrfstr_init();
    mrfstr_alloc(a, SIZE);
    mrfstr_repeat_chr(a, '0', SIZE);

    mrfstr_t b = mrfstr_init();
    mrfstr_alloc(b, SIZE);
    mrfstr_repeat_chr(b, '0', SIZE);

    double t = 0;
    clock_t c;
    for (int i = 0; i < COUNT; i++)
    {
        c = clock();
        mrfstr_equal(a, b);
        c = clock() - c;

        t += c;
        printf("%ld msc\n", c);
    }

    printf("AVG: %lf msc\n", t / COUNT);

    mrfstr_free(a);
    mrfstr_free(b);
    return 0;
}
