#include "plane.h"

#include <cmath>

using namespace std;

Hit Plane::intersect(Ray const &ray)
{
    /****************************************************
    * RT1.1: INTERSECTION CALCULATION
    *
    * Given: ray, position, r
    * Sought: intersects? if true: *t
    *
    * Insert calculation of ray/sphere intersection here.
    *
    * You have the sphere's center (C) and radius (r) as well as
    * the ray's origin (ray.O) and direction (ray.D).
    *
    * If the ray does not intersect the sphere, return false.
    * Otherwise, return true and place the distance of the
    * intersection point from the ray origin in *t (see example).
    ****************************************************/
    Triple ray_origin = ray.O;
    Triple ray_direction = ray.D;

    double t;
    Vector N = Triple(a,b,c);

    double denom = N.dot(ray_direction);
    if (denom < 1e-6) {
    //Vec3f p0l0 = p0 - l0;
    //t = dotProduct(p0l0, n) / denom;
    //return (t >= 0);
        return Hit::NO_HIT();
    }

    //return false;

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
