#ifndef QUAD_H_
#define QUAD_H_

#include "../object.h"

class Quad: public Object
{
    public:
        Quad(Triangle const &t1, Triangle const &t2);

        virtual Hit intersect(Ray const &ray);

        Triangle const t1;
        Triangle const t2;
};

#endif
