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

Point operator * (const Point &pt1, const Point &pt2)
{
    return Point {.x = pt1.x * pt2.x, .y = pt1.y * pt2.y };
}
