#ifndef SPHERE_H_
#define SPHERE_H_

#include "../object.h"
#include "../triple.h"

#include <tuple>
using namespace std;

class Sphere: public Object
{
    public:
        Sphere(Point const &pos, double radius, float angle);

        virtual Hit intersect(Ray const &ray);
        virtual std::tuple<float, float> pointMapping(Triple p);

        Point const position;
        double const r;
        float const a;
};

#endif
