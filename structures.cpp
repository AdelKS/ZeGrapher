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
