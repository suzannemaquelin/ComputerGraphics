#ifndef PLANE_H_
#define PLANE_H_

#include "../object.h"

class Plane: public Object
{
    public:
        Plane(float a, float b, float c, float d);

        virtual Hit intersect(Ray const &ray);

        float const a;
        float const b;
        float const c;
        float const d;
};

#endif
