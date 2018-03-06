#ifndef SPHERE_H_
#define SPHERE_H_

#include "../object.h"

class Sphere: public Object
{
    public:
        Sphere(Point const &pos, double radius, float a);

        virtual Hit intersect(Ray const &ray);

        Point const position;
        double const r;
        float const angle;

        virtual std::tuple<float, float> pointMapping(Triple p);
};

#endif
