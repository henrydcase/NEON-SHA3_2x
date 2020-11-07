#include <stdio.h>
#include <stdlib.h>
#include <papi.h>
#include <time.h>
#include "fips202x2.h"
#include "fips202.h"

#define TESTS 1000000
#define OUTLENGTH SHAKE128_RATE*10
#define INLENGTH SHAKE128_RATE
#define BRUTE 0

int main()
{
    // srand(time(0));
    uint8_t out1[OUTLENGTH], out2[OUTLENGTH];
    uint8_t in1[INLENGTH], in2[INLENGTH];
    uint8_t out_gold[OUTLENGTH], in_gold[INLENGTH];

    uint64_t bla, blo;

    for (int i = 0; i < INLENGTH; i++)
    {
        bla = rand();
        blo = rand();
        in1[i] = bla;
        in2[i] = blo;
        in_gold[i] = bla;
        // in_gold[i] = blo;
    }
    long_long start, end;

#if BRUTE
    for (int ol = 1; ol < OUTLENGTH; ol++)
    {
        for (int il = 1; il < INLENGTH; il++)
        {
#else
    int ol = OUTLENGTH;
    int il = INLENGTH;
#endif 
            start = PAPI_get_real_cyc();
            for (int j = 0; j < TESTS; j++)
            {
                shake128x2(out1, out2, ol, in1, in2, il);
            }
            end = PAPI_get_real_cyc();
            // printf("NEON: %lld/operation\n", (end - start)/TESTS);
            printf("NEON: %f cyc/operation\n", ((double) (end - start) )/TESTS);

            start = PAPI_get_real_cyc();
            for (int j = 0; j < TESTS; j++)
            {
                shake128(out_gold, ol, in_gold, il);
            }
            end = PAPI_get_real_cyc();
            // printf("FIPS: %lld/operation\n", (end - start)/TESTS);
            printf("FIPS: %f cyc/operation\n", ((double) (end - start) )/TESTS);

            int check = 0;
            uint8_t m, n;

            // for (int i = 0; i < OUTLENGTH - 8; i += 8)
            for (int i = 0; i < ol - 8; i += 8)
            {
                for (int j = i; j < i + 8; j++)
                {
                    m = out_gold[j];
                    n = out1[j];
                    if (out_gold[j] != out1[j])
                    {
                        printf("![%2d] %2x != %2x  ?? %2x\n", j, m, n, out2[j]);
                        check++;
                    }
                    else
                    {
                        // printf("+[%2d] %2x == %2x ?? %2x\n", j, m, n, out2[j]);
                    }
                }

                if (check)
                {
                    printf("%d %d ERROR! %d\n", ol, il, i);
                    // printf("ERROR! %d\n", i);
                    return 1;
                }
            }
#if BRUTE
        }
    }
#endif 

    printf("CORRECT!\n");

    return 0;
}
