#include "libfrac.h"
#include <stdio.h>

int
main(void)
{
    struct lf_frac frac;
    lf_find_frac(2.565, &frac);

    printf("%d\n%d\n%d\n", frac.n, frac.numer, frac.denom);
}