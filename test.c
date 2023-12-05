#include <stdio.h>
#include <mrfstr.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE (1024 * 1024 * 1024)
#define COUNT 100

int main()
{
    mrfstr_t a = mrfstr_init();
    mrfstr_repeat_chr(a, 'a', SIZE);
    a->data[100] = '3';
    a->data[101] = '4';
    a->data[304022] = 'y';
    a->data[301010] = 'q';
    a->data[11222] = 'p';
    a->data[1000] = 'o';
    a->data[20202] = 'e';
    a->data[900000] = ';';

    mrfstr_t b = mrfstr_init();
    mrfstr_alloc(b, SIZE + 1);

    size_t s = clock();
    mrfstr_reverse(b, a);
    mrfstr_reverse(a, a);

    printf("%llu\n", mrfstr_equal(a, b));
    printf("%ld\n", clock() - s);

    s = clock();
    strrev(a->data);
    strrev(b->data);
    printf("%llu\n", !memcmp(a->data, b->data, SIZE));
    printf("%ld\n", clock() - s);
    //mrfstr_reverse(a, a);

    //printf("%hu\n", mrfstr_equal(a, b));

    /*size_t t = 0;
    clock_t q = clock();
    for (mrfstr_bit_t i = 0; i < COUNT; i++)
    {
        clock_t o = clock();
        o = clock() - o;
        t += o;

        printf("%ld\n", o);
    }
    printf("%ld\n", clock() - q);

    printf("AVG: %lf\n", (double)t / COUNT);*/

    mrfstr_free(a);
    mrfstr_free(b);
    return 0;
}
