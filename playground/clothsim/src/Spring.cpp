#include "Spring.hpp"
#include "Point.hpp"

Spring::Spring(Point *p1, Point *p2, float restLength, int p1Index, int p2Index)
    : p1(p1), p2(p2), restLength(restLength), p1Index(p1Index),
      p2Index(p2Index) {}
