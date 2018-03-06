#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "../object.h"

class Triangle: public Object
{
    public:
        Triangle(Point const &v1, Point const &v2, Point const &v3);

        virtual Hit intersect(Ray const &ray);

        Point vertex1;
        Point vertex2;
        Point vertex3;
};

#endif
