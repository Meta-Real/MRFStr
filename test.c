#include <stdio.h>
#include <mrfstr.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE (1024 * 1024 * 1024)
#define COUNT 200

int main()
{
    mrfstr_t a = mrfstr_init();
    mrfstr_repeat_chr(a, 'w', SIZE);

    //char *s = _aligned_malloc(SIZE + 1, 64);
    //memset(s, 'w', SIZE);
    //s[SIZE] = '\0';

    size_t t = 0;
    clock_t q = clock();
    for (mrfstr_bit_t i = 0; i < COUNT; i++)
    {
        clock_t o = clock();
        mrfstr_contain_chr(a, 'e');
        o = clock() - o;
        t += o;

        printf("%ld\n", o);
    }
    printf("%ld\n", clock() - q);

    printf("AVG: %lf\n", (double)t / COUNT);

    mrfstr_free(a);
    //_aligned_free(s);
    return 0;
}
