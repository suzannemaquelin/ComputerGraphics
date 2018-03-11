#ifndef TRIANGLE_H_
#define TRIANGLE_H_

#include "../object.h"
#include "../triple.h"

#include <tuple>
using namespace std;

class Triangle: public Object
{
    public:
        Triangle(Point const &v1, Point const &v2, Point const &v3);

        virtual Hit intersect(Ray const &ray);
        virtual std::tuple<float, float> pointMapping(Triple p);

        Point vertex1;
        Point vertex2;
        Point vertex3;
};

#endif
