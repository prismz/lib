#ifndef LIBGEOM_H
#define LIBGEOM_H

#include <math.h>

#ifndef lg_point
struct lg_point {
    double x, y;
};
#endif

void
lg_midpoint(struct lg_point *p1, struct lg_point *p2, struct lg_point *dest)
{
    dest->x = (double)(p1->x + p2->x) / (double)2;
    dest->y = (double)(p1->y + p2->y) / (double)2;
}

double
lg_distance(struct lg_point *p1, struct lg_point *p2)
{
    double diff_x = (p2->x - p1->x);
    double diff_y = (p2->y - p1->y);
    
    diff_x = pow(diff_x, 2);
    diff_y = pow(diff_y, 2);

    return sqrt(diff_x + diff_y);
}

/* pass 0 for unknown variable */
double
lg_pythagorean(double a, double b, double c)
{
    if (a >= c || b >= c)
        return -1;

    if (a == 0) {
        if (b == 0 || c == 0)
            return -1;
        
        a = sqrt(pow(c, 2) - pow(b, 2));
        return a;
    }

    if (b == 0) {
        if (a == 0 || c == 0)
            return -1;
        
        b = sqrt(pow(c, 2) - pow(a, 2));
        return b;
    }

    if (c == 0) {
        if (a == 0 || b == 0)
            return -1;
        
        c = sqrt(pow(a, 2) + pow(b, 2));
        return c;
    }

    return c;
}

#endif