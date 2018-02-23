#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "../object.h"

class Triangle: public Object
{
    public:
        Triangle(Point const &v1, Point const &v2, Point const &v3);

        virtual Hit intersect(Ray const &ray);

        Point const vertex1;
        Point const vertex2;
        Point const vertex3;
};

#endif
