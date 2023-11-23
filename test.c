#include <stdio.h>
#include <mrfstr.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main()
{
    mrfstr_file_t f1 = mrfstr_openfile_read("test");
    mrfstr_file_t f2 = mrfstr_openfile_write("test1", f1.size);

    mrfstr_size_t s = 0, i;
    clock_t t;
    for (i = 0; i < 100;)
    {
        t = clock();
        mrfstr_transfer(&f2, &f1);
        t = clock() - t;

        printf("test%llu: %ld milliseconds\n", ++i, t);
    }

    printf("avg time: %lf milliseconds\n", s / 100.0);

    mrfstr_closefile(&f1);
    mrfstr_closefile(&f2);

    printf("END\n");
    return 0;
}
