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
    //If the denominator is zero, the normal vector and ray direction will be perpendicular, thus no light hits the plane
    if (denom < 1e-6 && denom > -1e-6) {
        return Hit::NO_HIT();
    }

    //calculation of distance between origin of ray and intersection point
    t = - (N.dot(ray_origin) + d) / denom;

    return Hit(t, N);

}

std::tuple<float, float> Plane::pointMapping(Triple p) {
    //trivial implementation
    if (p.x > 1.0 || p.y > 1.0) {
        return std::make_tuple(0.0, 0.0);
    } else {
        return std::make_tuple(p.x, p.y);
    }
}

Plane::Plane(float a, float b, float c, float d)
:
    a(a),
    b(b),
    c(c),
    d(d)
{}
