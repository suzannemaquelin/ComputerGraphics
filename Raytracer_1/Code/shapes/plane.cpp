#include "plane.h"

#include <cmath>

using namespace std;

Hit Plane::intersect(Ray const &ray)
{
    Triple ray_origin = ray.O;
    Triple ray_direction = ray.D;

    double t;
    Vector N = Triple(a,b,c);
    N.normalize();

    //The degree between the normal and the vector ray direction should be larger than 90 degrees
    if (N.dot(ray_direction) > 0) {
        N = Triple(-1,-1,-1) * N;
    }

    double denom = N.dot(ray_direction);
    if (denom < 1e-6 & denom > -1e-6) {
        return Hit::NO_HIT();
    }

    t = - (N.dot(ray_origin) + d) / denom;

    return Hit(t, N);

}

Plane::Plane(float a, float b, float c, float d)
:
    a(a),
    b(b),
    c(c),
    d(d)
{}
