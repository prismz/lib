#ifndef LIBFRAC_H
#define LIBFRAC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct lf_frac {
    int n;
    int numer;
    int denom;
};

void
lf_find_frac(double n, struct lf_frac *frac)
{
    frac->n = floor(n);
    double rem = n - floor(n);  /* decimal remainder */
    if (rem == 0) {
        frac->numer = n;
        frac->denom = 1;
        return;
    }

    char* str_rem = malloc(sizeof(char) * 30);
    snprintf(str_rem, 30, "%f", rem);

    int after_decimal = 0;
    int n_places = 0;
    for (size_t i = 0; i < strlen(str_rem); i++) {
        if (after_decimal)
            n_places++;

        if (str_rem[i] == '.')
            after_decimal = 1;
    }

    for (int i = (int)strlen(str_rem) - 1; i >= 0; i--) {
        if (str_rem[i] != '0')
            break;
        
        n_places--;
    }

    free(str_rem);

    /* euclid's algorithm */
    long long b = rem * pow(10, n_places);
    long long a = pow(10, n_places);
    long long o_a = a;
    long long o_b = b;

    double r = a % b;
    while (r != 0) {
        a = b;
        b = r;
        r = a % b;
    }

    frac->numer = o_b / b;
    frac->denom = o_a / b;
}

#endif
