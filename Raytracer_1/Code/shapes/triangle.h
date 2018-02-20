#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "../object.h"

class Triangle: public Object
{
    public:
        Triangle(Point v1, Point v2, Point v3);

        virtual Hit intersect(Ray const &ray);

        Point const vertex1;
        Point const vertex2;
        Point const vertex3;
};

#endif
