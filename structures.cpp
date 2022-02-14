#include "structures.h"

Point operator*(Point &pt1, const Point &pt2)
{
    return {pt1.x * pt2.x, pt1.y * pt2.y};
}

QPointF toQPointF(const Point &pt)
{
    return QPointF(pt.x, pt.y);
}

bool ptCompY(const Point &pt1, const Point &pt2)
{
    return pt1.y < pt2.y;
}

template<class Number>
Number int_pow(Number a, int b)
{
    Number res = 1;
    if(b < 0)
    {
        for(int i = 0 ; i > b ; i--)
            res /= a;
    }
    else
    {
        for(int i = 0 ; i < b ; i++)
            res *= a;
    }

    return res;
}
