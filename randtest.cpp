#include <Windows.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char uint8_t;

#define MAX_ITERATIONS 10000
#define MAX_VALUES 100
uint8_t freq[MAX_VALUES];

void test1()
{
/*
    typedef union _LARGE_INTEGER
    {
        struct
        {
            DWORD LowPart;
            LONG HighPart;
        };
        LONGLONG QuadPart;
    } LARGE_INTEGER;
*/

    printf("\
TEST 1 \n\
This test mods the time in CPU ticks with %d, for %d iterations \n\
to produce a frequency of 'random' numbers. This frequency should be plotted \n\
in Excel to see the distribution curve.\n\
", MAX_VALUES, MAX_ITERATIONS);

    int n;
    LARGE_INTEGER counts;

    memset(freq, 0, MAX_VALUES);

    for(n = 0; n < MAX_ITERATIONS; ++n)
    {
        QueryPerformanceCounter(&counts);
        ++freq[counts.QuadPart % MAX_VALUES];
    }

    for(n = 0; n < MAX_VALUES; ++n)
    {
        printf("%d\t%d\n", n, freq[n]);
    }
}

void test2()
{
    srand(time(NULL));

    printf("\
TEST 2 \n\
This test mods the rand() function with %d, for %d iterations to produce \n\
a frequency of random numbers. This frequency should be plotted in Excel to \n\
see the distribution curve.\n\
", MAX_VALUES, MAX_ITERATIONS);

    int n;

    memset(freq, 0, MAX_VALUES);

    for(n = 0; n < MAX_ITERATIONS; ++n)
    {
        ++freq[rand() % MAX_VALUES];
    }

    for(n = 0; n < MAX_VALUES; ++n)
    {
        printf("%d\t%d\n", n, freq[n]);
    }
}

int main(int argc, char *argv[])
{
    printf("randtest (c) 2011 Steve Connet\n\n");

    test1();
    test2();

    return EXIT_SUCCESS;
}
