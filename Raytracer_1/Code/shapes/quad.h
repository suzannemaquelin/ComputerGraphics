#ifndef QUAD_H_
#define QUAD_H_

#include "../object.h"
#include "triangle.h"

class Quad: public Object
{
    public:
        Quad(Point const &v1, Point const &v2, Point const &v3, Point const &v4);

        Point function(Point const &v1, Point const &v2, Point const &v3, Point const &v4);

        virtual Hit intersect(Ray const &ray);

        Point const v1;
        Point const v2;
        Point const v3;
        Point const v4;
};

#endif
