#ifndef QUAD_H_
#define QUAD_H_

#include "../object.h"
#include "triangle.h"
#include "../triple.h"

#include <tuple>

class Quad: public Object
{
    public:
        Quad(Point const &v1, Point const &v2, Point const &v3, Point const &v4);

        Point furthest_point(Point const &v1, Point const &v2, Point const &v3, Point const &v4);
        Triangle take_nearest(Point furthest, Point triangle_point, Point p1, Point p2, Point p3);

        virtual Hit intersect(Ray const &ray);
        virtual std::tuple<float, float> pointMapping(Triple p);

        Point const v1;
        Point const v2;
        Point const v3;
        Point const v4;
};

#endif
